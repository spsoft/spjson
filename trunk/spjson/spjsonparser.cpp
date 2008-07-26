/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <typeinfo>

#include <string.h>
#include <stdlib.h>

#include "spjsonparser.hpp"

#include "spjsonutils.hpp"
#include "spjsonreader.hpp"
#include "spjsonevent.hpp"


SP_JsonPullParser :: SP_JsonPullParser()
{
	mEventQueue = new SP_JsonPullEventQueue();
	mReader = new SP_JsonSpaceReader( 1 );
	mStack = new SP_JsonArrayList();
	mNameList = new SP_JsonArrayList();

	mLevel = 0;

	mError = NULL;
	memset( mErrorSegment, 0, sizeof( mErrorSegment ) );
	mErrorIndex = mColIndex = mRowIndex = 0;

	mObjectCount = 0;
}

SP_JsonPullParser :: ~SP_JsonPullParser()
{
	delete mEventQueue;
	mEventQueue = NULL;

	delete mReader;
	mReader = NULL;

	delete mStack;
	mStack = NULL;

	for( int i = 0; i < mNameList->getCount(); i++ ) {
		free( (char*)mNameList->getItem( i ) );
	}
	delete mNameList;
	mNameList = NULL;
}

int SP_JsonPullParser :: append( const char * source, int len )
{
	if( NULL != mError ) return 0;

	int consumed = 0;

	for( int i = 0; i < len && NULL == mError; i++ ) {

		consumed++;

		char c = source[ i ];

		mErrorSegment[ mErrorIndex++ % sizeof( mErrorSegment ) ] = c;
		mReader->read( this, c );
		if( '\n' == c ) {
			mRowIndex++;
			mColIndex = 0;
		} else {
			mColIndex++;
		}
	}

	return consumed;
}

SP_JsonPullEvent * SP_JsonPullParser :: getNext()
{
	SP_JsonPullEvent * event = mEventQueue->dequeue();

	if( NULL != event ) {
		if( SP_JsonPullEvent::eStartObject == event->getEventType() ) mLevel++;
		if( SP_JsonPullEvent::eEndObject == event->getEventType() ) mLevel--;
	}

	return event;
}

const char * SP_JsonPullParser :: getError()
{
	return mError;
}

int SP_JsonPullParser :: getLevel()
{
	return mLevel;
}

void SP_JsonPullParser :: changeReader( SP_JsonReader * reader )
{
	SP_JsonPullEvent * event = mReader->getEvent( this );

	if( NULL != event ) {
		const int eventType = event->getEventType();

		char errmsg[ 256 ] = { 0 };

		if( SP_JsonPullEvent::eStartObject == eventType ) {
			mStack->append( (void*)"o" );

			++mObjectCount;
		} else if( SP_JsonPullEvent::eEndObject == eventType ) {
			if( mObjectCount <= mNameList->getCount() ) {
				snprintf( errmsg, sizeof( errmsg ), "miss value for name \"%s\"",
						(char*)mNameList->getItem( SP_JsonArrayList::LAST_INDEX ) );
			} else {
				char * last = (char*)mStack->takeItem( SP_JsonArrayList::LAST_INDEX );
				if( NULL != last ) {
					if( 'o' != *last ) {
						snprintf( errmsg, sizeof( errmsg ), "mismatched object, start-with<%s>", last );
					}
				} else {
					snprintf( errmsg, sizeof( errmsg ), "mismatched object, start-with<NULL>" );
				}

				--mObjectCount;
				if( isObjectParent() ) free( mNameList->takeItem( SP_JsonArrayList::LAST_INDEX ) );
			}
		} else if( SP_JsonPullEvent::eStartArray == eventType ) {
			mStack->append( (void*)"a" );
		} else if( SP_JsonPullEvent::eEndArray == eventType ) {
			char * last = (char*)mStack->takeItem( SP_JsonArrayList::LAST_INDEX );
			if( NULL != last ) {
				if( 'a' != *last ) {
					snprintf( errmsg, sizeof( errmsg ), "mismatched array, start-with<%s>", last );
				}
			} else {
				snprintf( errmsg, sizeof( errmsg ), "mismatched array, start-with<NULL>" );
			}

			if( isObjectParent() ) free( mNameList->takeItem( SP_JsonArrayList::LAST_INDEX ) );
		} else if( SP_JsonPullEvent::eName == eventType ) {
			mNameList->append( strdup( ((SP_JsonNameEvent*)event)->getText() ) );
		} else if( SP_JsonPullEvent::eString == eventType
				|| SP_JsonPullEvent::eNumber == eventType
				|| SP_JsonPullEvent::eBoolean == eventType
				|| SP_JsonPullEvent::eNull == eventType ) {
			if( isObjectParent() ) free( mNameList->takeItem( SP_JsonArrayList::LAST_INDEX ) );
		}

		if( '\0' == errmsg[0] ) {
			mEventQueue->enqueue( event );
		} else {
			delete event;
			setError( errmsg );
		}
	}

	//printf( "\nchange: %s -> %s\n", typeid( *mReader ).name(), typeid( *reader ).name() );

	delete mReader;
	mReader = reader;
}

void SP_JsonPullParser :: setError( const char * error )
{
	if( NULL != error ) {
		if( NULL != mError ) free( mError );

		char segment[ 2 * sizeof( mErrorSegment ) + 1 ];
		{
			memset( segment, 0, sizeof( segment ) );

			char temp[ sizeof( mErrorSegment ) + 1 ];
			memset( temp, 0, sizeof( temp ) );
			if( mErrorIndex < (int)sizeof( mErrorSegment ) ) {
				strncpy( temp, mErrorSegment, mErrorIndex );
			} else {
				int offset = mErrorIndex % sizeof( mErrorSegment );
				strncpy( temp, mErrorSegment + offset, sizeof( mErrorSegment ) - offset );
				strncpy( temp + sizeof( mErrorSegment ) - offset, mErrorSegment, offset );
			}

			for( char * pos = temp, * dest = segment; '\0' != *pos; pos++ ) {
				if( '\r' == *pos ) {
					*dest++ = '\\';
					*dest++ = 'r';
				} else if( '\n' == *pos ) {
					*dest++ = '\\';
					*dest++ = 'n';
				} else if( '\t' == *pos ) {
					*dest++ = '\\';
					*dest++ = 't';
				} else {
					*dest++ = *pos;
				}
			}
		}

		char msg[ 512 ];
		snprintf( msg, sizeof( msg), "%s ( occured at row(%d), col(%d) : %s )",
				error, mRowIndex + 1, mColIndex + 1, segment );

		mError = strdup( msg );
	}
}

int SP_JsonPullParser :: isWait4Name()
{
	int ret = 0;

	if( isObjectParent() && mObjectCount > mNameList->getCount() ) ret = 1;

	return ret;
}

int SP_JsonPullParser :: isObjectParent()
{
	int ret = 0;
	if( mStack->getCount() > 0 ) {
		char * last = (char*)mStack->getItem( SP_JsonArrayList::LAST_INDEX );
		if( 'o' == *last ) ret = 1;		
	}

	return ret;
}

