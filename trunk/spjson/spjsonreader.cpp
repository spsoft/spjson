/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <typeinfo>

#include <ctype.h>

#include "spjsonreader.hpp"

#include "spjsonutils.hpp"
#include "spjsonparser.hpp"
#include "spjsonevent.hpp"
#include "spjsoncodec.hpp"

SP_JsonReader :: SP_JsonReader()
{
	mBuffer = new SP_JsonStringBuffer();
}

SP_JsonReader :: ~SP_JsonReader()
{
	delete mBuffer;
}

void SP_JsonReader :: changeReader( SP_JsonPullParser * parser, SP_JsonReader * reader )
{
	//printf( "\nchange: %s\n", typeid( *reader ).name() );
	parser->changeReader( reader );
}

void SP_JsonReader :: setError( SP_JsonPullParser * parser, const char * error )
{
	parser->setError( error );
}

int SP_JsonReader :: isWait4Name( SP_JsonPullParser * parser )
{
	return parser->isWait4Name();
}

int SP_JsonReader :: isObjectParent( SP_JsonPullParser * parser )
{
	return parser->isObjectParent();
}

//=========================================================

SP_JsonSpaceReader :: SP_JsonSpaceReader( int hasReadComma )
{
	mHasReadComma = hasReadComma;
}

SP_JsonSpaceReader :: ~SP_JsonSpaceReader()
{
}

void SP_JsonSpaceReader :: read( SP_JsonPullParser * parser, char c )
{
	if( isspace( c ) ) {
		//skip
	} else if( ',' == c && 0 == mHasReadComma ) {
		mHasReadComma = 1;
	} else if( '/' == c ) {
		SP_JsonReader * reader = new SP_JsonCommentReader(
				new SP_JsonSpaceReader( mHasReadComma ) );
		changeReader( parser, reader );
		reader->read( parser, c );
	} else if( '}' == c ) {
		SP_JsonReader * reader = new SP_JsonEndObjectReader();
		changeReader( parser, reader );
		reader->read( parser, c );
	} else if( ']' == c ) {
		SP_JsonReader * reader = new SP_JsonEndArrayReader();
		changeReader( parser, reader );
		reader->read( parser, c );
	} else {
		if( 0 == mHasReadComma ) {
			setError( parser, "miss ',' between values" );
		} else if( isWait4Name( parser ) ) {
			SP_JsonReader * reader = new SP_JsonNameReader();
			changeReader( parser, reader );
			reader->read( parser, c );
		} else {
			if( isObjectParent( parser ) ) {
				if( ':' == c ) {
					changeReader( parser, new SP_JsonValueReader() );
				} else {
					setError( parser, "miss ':' between name & value" );
				}
			} else {
				if( '{' == c ) {
					SP_JsonReader * reader = new SP_JsonStartObjectReader();
					changeReader( parser, reader );
					reader->read( parser, c );
				} else if( '[' == c ) {
					SP_JsonReader * reader = new SP_JsonStartArrayReader();
					changeReader( parser, reader );	
					reader->read( parser, c );
				} else {
					SP_JsonReader * reader = new SP_JsonValueReader();
					changeReader( parser, reader );
					reader->read( parser, c );
				}
			}
		}
	}
}

SP_JsonPullEvent * SP_JsonSpaceReader :: getEvent( SP_JsonPullParser * parser )
{
	return NULL;
}

//=========================================================

SP_JsonStartObjectReader :: SP_JsonStartObjectReader( int hasReadLeftBrace )
{
	mHasReadLeftBrace = 0 == hasReadLeftBrace ? 0 : 2;
}

SP_JsonStartObjectReader :: ~SP_JsonStartObjectReader()
{
}

void SP_JsonStartObjectReader :: read( SP_JsonPullParser * parser, char c )
{
	if( isspace( c ) ) {
		//skip
	} else if( 0 == mHasReadLeftBrace && '{' == c ) {
		mHasReadLeftBrace = 1;
	} else if( '}' == c ) {
		SP_JsonReader * reader = new SP_JsonEndObjectReader();
		changeReader( parser, reader );
		reader->read( parser, c );
	} else if( '"' == c ) {
		SP_JsonReader * reader = new SP_JsonNameReader();
		changeReader( parser, reader );
		reader->read( parser, c );
	} else if( '/' == c ) {
		SP_JsonReader * reader = new SP_JsonCommentReader(
				new SP_JsonStartObjectReader( mHasReadLeftBrace ) );
		changeReader( parser, reader );
		reader->read( parser, c );
	} else {
		setError( parser, "unknown name" );
	}
}

SP_JsonPullEvent * SP_JsonStartObjectReader :: getEvent( SP_JsonPullParser * parser )
{
	return new SP_JsonStartObjectEvent();
}

//=========================================================

SP_JsonEndObjectReader :: SP_JsonEndObjectReader()
{
}

SP_JsonEndObjectReader :: ~SP_JsonEndObjectReader()
{
}

void SP_JsonEndObjectReader :: read( SP_JsonPullParser * parser, char c )
{
	if( '}' == c ) {
		changeReader( parser, new SP_JsonSpaceReader() );
	} else {
		setError( parser, "unknown object end" );
	}
}

SP_JsonPullEvent * SP_JsonEndObjectReader :: getEvent( SP_JsonPullParser * parser )
{
	return new SP_JsonEndObjectEvent();
}

//=========================================================

SP_JsonStartArrayReader :: SP_JsonStartArrayReader()
{
	mHasReadLeftBracket = 0;
}

SP_JsonStartArrayReader :: ~SP_JsonStartArrayReader()
{
}

void SP_JsonStartArrayReader :: read( SP_JsonPullParser * parser, char c )
{
	if( 0 == mHasReadLeftBracket && '[' == c ) {
		mHasReadLeftBracket = 1;
	} else if( ']' == c ) {
		SP_JsonReader * reader = new SP_JsonEndArrayReader();
		changeReader( parser, reader );
		reader->read( parser, c );
	} else {
		SP_JsonReader * reader = new SP_JsonSpaceReader( 1 );
		changeReader( parser, reader );
		reader->read( parser, c );
	}
}

SP_JsonPullEvent * SP_JsonStartArrayReader :: getEvent( SP_JsonPullParser * parser )
{
	return new SP_JsonStartArrayEvent();
}

//=========================================================

SP_JsonEndArrayReader :: SP_JsonEndArrayReader()
{
}

SP_JsonEndArrayReader :: ~SP_JsonEndArrayReader()
{
}

void SP_JsonEndArrayReader :: read( SP_JsonPullParser * parser, char c )
{
	if( ']' == c ) {
		changeReader( parser, new SP_JsonSpaceReader() );
	} else {
		setError( parser, "unknown array end" );
	}
}

SP_JsonPullEvent * SP_JsonEndArrayReader :: getEvent( SP_JsonPullParser * parser )
{
	return new SP_JsonEndArrayEvent();
}

//=========================================================

SP_JsonNameReader :: SP_JsonNameReader()
{
	mLastChar = '\0';
	mHasReadStartQuot = 0;
}

SP_JsonNameReader :: ~SP_JsonNameReader()
{
}

void SP_JsonNameReader :: read( SP_JsonPullParser * parser, char c )
{
	if( 0 == mHasReadStartQuot ) {
		if( isspace( c ) ) {
			//skip
		} else if( '"' == c ) {
			mHasReadStartQuot = 1;
		} else {
			setError( parser, "unknown name" );
		}
	} else {
		if( '"' == c && '\\' != mLastChar ) {
			changeReader( parser, new SP_JsonSpaceReader( 1 ) );
		} else {
			if( '\\' == mLastChar && '\\' == c ) {
				mLastChar = '\0';
			} else {
				mLastChar = c;
			}
			mBuffer->append( c );
		}
	}
}

SP_JsonPullEvent * SP_JsonNameReader :: getEvent( SP_JsonPullParser * parser )
{
	SP_JsonStringBuffer outBuffer;

	SP_JsonCodec::decode( mBuffer->getBuffer(), &outBuffer );

	SP_JsonNameEvent * event = new SP_JsonNameEvent();
	event->setText( outBuffer.getBuffer(), outBuffer.getSize() );

	return event;
}

//=========================================================

SP_JsonValueReader :: SP_JsonValueReader()
{
}

SP_JsonValueReader :: ~SP_JsonValueReader()
{
}

void SP_JsonValueReader :: read( SP_JsonPullParser * parser, char c )
{
	if( isspace( c ) ) {
		//skip
	} else if( '"' == c ) {
		SP_JsonReader * reader = new SP_JsonStringReader();
		changeReader( parser, reader );
		reader->read( parser, c );
	} else if( '-' == c || isdigit( c ) ) {
		SP_JsonReader * reader = new SP_JsonNumberReader();
		changeReader( parser, reader );
		reader->read( parser, c );
	} else if( 't' == c || 'T' == c || 'f' == c || 'F' == c ) {
		SP_JsonReader * reader = new SP_JsonBooleanReader();
		changeReader( parser, reader );
		reader->read( parser, c );
	} else if( 'n' == c || 'N' == c ) {
		SP_JsonReader * reader = new SP_JsonNullReader();
		changeReader( parser, reader );
		reader->read( parser, c );
	} else if( '/' == c ) {
		SP_JsonReader * reader = new SP_JsonCommentReader(
				new SP_JsonValueReader() );
		changeReader( parser, reader );
		reader->read( parser, c );
	} else if( '{' == c ) {
		SP_JsonReader * reader = new SP_JsonStartObjectReader();
		changeReader( parser, reader );
		reader->read( parser, c );
	} else if( '[' == c ) {
		SP_JsonReader * reader = new SP_JsonStartArrayReader();
		changeReader( parser, reader );
		reader->read( parser, c );
	} else {
		setError( parser, "unknown value" );
	}
}

SP_JsonPullEvent * SP_JsonValueReader :: getEvent( SP_JsonPullParser * parser )
{
	return NULL;
}

//=========================================================

SP_JsonStringReader :: SP_JsonStringReader()
{
	mLastChar = '\0';
	mHasReadStartQuot = 0;
}

SP_JsonStringReader :: ~SP_JsonStringReader()
{
}

void SP_JsonStringReader :: read( SP_JsonPullParser * parser, char c )
{
	if( 0 == mHasReadStartQuot ) {
		if( isspace( c ) ) {
			//skip
		} else if( '"' == c ) {
			mHasReadStartQuot = 1;
		} else {
			setError( parser, "unknown string" );
		}
	} else {
		if( '"' == c && '\\' != mLastChar ) {
			changeReader( parser, new SP_JsonSpaceReader() );
		} else {
			if( '\\' == mLastChar && '\\' == c ) {
				mLastChar = '\0';
			} else {
				mLastChar = c;
			}
			mBuffer->append( c );
		}
	}
}

SP_JsonPullEvent * SP_JsonStringReader :: getEvent( SP_JsonPullParser * parser )
{
	SP_JsonStringBuffer outBuffer;

	SP_JsonCodec::decode( mBuffer->getBuffer(), &outBuffer );

	SP_JsonStringEvent * event = new SP_JsonStringEvent();
	event->setText( outBuffer.getBuffer(), outBuffer.getSize() );

	return event;
}

//=========================================================

SP_JsonNumberReader :: SP_JsonNumberReader()
{
}

SP_JsonNumberReader :: ~SP_JsonNumberReader()
{
}

void SP_JsonNumberReader :: read( SP_JsonPullParser * parser, char c )
{
	if( isdigit( c ) || '-' == c || '+' == c
			|| 'e' == c || 'E' == c || '.' == c ) {
		mBuffer->append( c );
	} else {
		SP_JsonReader * reader = new SP_JsonSpaceReader();
		changeReader( parser, reader );
		reader->read( parser, c );
	}
}

SP_JsonPullEvent * SP_JsonNumberReader :: getEvent( SP_JsonPullParser * parser )
{
	SP_JsonNumberEvent * event = new SP_JsonNumberEvent();
	event->setText( mBuffer->getBuffer(), mBuffer->getSize() );

	return event;
}

//=========================================================

SP_JsonBooleanReader :: SP_JsonBooleanReader()
{
}

SP_JsonBooleanReader :: ~SP_JsonBooleanReader()
{
}

void SP_JsonBooleanReader :: read( SP_JsonPullParser * parser, char c )
{
	if( !isalpha( c ) ) {
		SP_JsonReader * reader = new SP_JsonSpaceReader();
		changeReader( parser, reader );
		reader->read( parser, c );
	} else {
		mBuffer->append( c );
	}
}

SP_JsonPullEvent * SP_JsonBooleanReader :: getEvent( SP_JsonPullParser * parser )
{
	SP_JsonBooleanEvent * event = new SP_JsonBooleanEvent();
	event->setText( mBuffer->getBuffer(), mBuffer->getSize() );

	return event;
}

//=========================================================

SP_JsonNullReader :: SP_JsonNullReader()
{
}

SP_JsonNullReader :: ~SP_JsonNullReader()
{
}

void SP_JsonNullReader :: read( SP_JsonPullParser * parser, char c )
{
	if( !isalpha( c ) ) {
		SP_JsonReader * reader = new SP_JsonSpaceReader();
		changeReader( parser, reader );
		reader->read( parser, c );
	} else {
		mBuffer->append( c );
	}
}

SP_JsonPullEvent * SP_JsonNullReader :: getEvent( SP_JsonPullParser * parser )
{
	SP_JsonNullEvent * event = new SP_JsonNullEvent();
	event->setText( mBuffer->getBuffer(), mBuffer->getSize() );

	return event;
}

//=========================================================

SP_JsonCommentReader :: SP_JsonCommentReader( SP_JsonReader * savedReader )
{
	mHasReadStartSolidus = 0;
	mSavedReader = savedReader;
}

SP_JsonCommentReader :: ~SP_JsonCommentReader()
{
}

void SP_JsonCommentReader :: read( SP_JsonPullParser * parser, char c )
{
	if( mHasReadStartSolidus < 2 ) {
		if( '/' == c ) {
			mHasReadStartSolidus++;
		} else {
			setError( parser, "unknown comment" );
		}
	} else if( '\n' == c ) {
		changeReader( parser, mSavedReader );
	} else {
		if( '\r' != c ) mBuffer->append( c );
	}
}

SP_JsonPullEvent * SP_JsonCommentReader :: getEvent( SP_JsonPullParser * parser )
{
	SP_JsonCommentEvent * event = new SP_JsonCommentEvent();
	event->setText( mBuffer->getBuffer(), mBuffer->getSize() );

	return event;
}

//=========================================================

SP_JsonPaddingReader :: SP_JsonPaddingReader()
{
}

SP_JsonPaddingReader :: ~SP_JsonPaddingReader()
{
}

void SP_JsonPaddingReader :: read( SP_JsonPullParser * parser, char c )
{
	if( !isspace( c ) ) {
		setError( parser, "unexpected padding" );
	}
}

SP_JsonPullEvent * SP_JsonPaddingReader :: getEvent( SP_JsonPullParser * parser )
{
	SP_JsonPaddingEvent * event = new SP_JsonPaddingEvent();
	event->setText( mBuffer->getBuffer(), mBuffer->getSize() );

	return event;
}

