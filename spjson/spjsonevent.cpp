/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdlib.h>
#include <string.h>

#include "spjsonevent.hpp"

#include "spjsonutils.hpp"

SP_JsonPullEvent :: SP_JsonPullEvent( int eventType )
	: mEventType( eventType )
{
}

SP_JsonPullEvent :: ~SP_JsonPullEvent()
{
}

int SP_JsonPullEvent :: getEventType()
{
	return mEventType;
}

//=========================================================

SP_JsonPullEventQueue :: SP_JsonPullEventQueue()
{
	mQueue = new SP_JsonQueue();
}

SP_JsonPullEventQueue :: ~SP_JsonPullEventQueue()
{
	for( ; NULL != mQueue->top(); ) {
		SP_JsonPullEvent * event = (SP_JsonPullEvent*)mQueue->pop();
		delete event;
	}

	delete mQueue;
}

void SP_JsonPullEventQueue :: enqueue( SP_JsonPullEvent * event )
{
	mQueue->push( event );
}

SP_JsonPullEvent * SP_JsonPullEventQueue :: dequeue()
{
	return (SP_JsonPullEvent*)mQueue->pop();
}

//=========================================================

SP_JsonStartObjectEvent :: SP_JsonStartObjectEvent()
	: SP_JsonPullEvent( eStartObject )
{
}

SP_JsonStartObjectEvent :: ~SP_JsonStartObjectEvent()
{
}

//=========================================================

SP_JsonEndObjectEvent :: SP_JsonEndObjectEvent()
	: SP_JsonPullEvent( eEndObject )
{
}

SP_JsonEndObjectEvent :: ~SP_JsonEndObjectEvent()
{
}

//=========================================================

SP_JsonStartArrayEvent :: SP_JsonStartArrayEvent()
	: SP_JsonPullEvent( eStartArray )
{
}

SP_JsonStartArrayEvent :: ~SP_JsonStartArrayEvent()
{
}

//=========================================================

SP_JsonEndArrayEvent :: SP_JsonEndArrayEvent()
	: SP_JsonPullEvent( eEndArray )
{
}

SP_JsonEndArrayEvent :: ~SP_JsonEndArrayEvent()
{
}

//=========================================================

SP_JsonTextEvent :: SP_JsonTextEvent( int eventType )
	: SP_JsonPullEvent( eventType )
{
	mText = NULL;
}

SP_JsonTextEvent :: ~SP_JsonTextEvent()
{
	if( NULL != mText ) free( mText );
	mText = NULL;
}

void SP_JsonTextEvent :: setText( const char * text, int len )
{
	if( NULL != text ) {
		if( NULL != mText ) free( mText );
		mText = (char*)malloc( len + 1 );
		memcpy( mText, text, len );
		mText[ len ] = '\0';
	}
}

const char * SP_JsonTextEvent :: getText() const
{
	return mText;
}

//=========================================================

SP_JsonNullEvent :: SP_JsonNullEvent()
	: SP_JsonTextEvent( eNull )
{
}

SP_JsonNullEvent :: ~SP_JsonNullEvent()
{
}

//=========================================================

SP_JsonNameEvent :: SP_JsonNameEvent()
	: SP_JsonTextEvent( eName )
{
}

SP_JsonNameEvent :: ~SP_JsonNameEvent()
{
}

//=========================================================

SP_JsonStringEvent :: SP_JsonStringEvent()
	: SP_JsonTextEvent( eString )
{
}

SP_JsonStringEvent :: ~SP_JsonStringEvent()
{
}

//=========================================================

SP_JsonNumberEvent :: SP_JsonNumberEvent()
	: SP_JsonTextEvent( eNumber )
{
}

SP_JsonNumberEvent :: ~SP_JsonNumberEvent()
{
}

//=========================================================

SP_JsonBooleanEvent :: SP_JsonBooleanEvent()
	: SP_JsonTextEvent( eBoolean )
{
}

SP_JsonBooleanEvent :: ~SP_JsonBooleanEvent()
{
}

//=========================================================

SP_JsonCommentEvent :: SP_JsonCommentEvent()
	: SP_JsonTextEvent( eComment )
{
}

SP_JsonCommentEvent :: ~SP_JsonCommentEvent()
{
}

//=========================================================

SP_JsonPaddingEvent :: SP_JsonPaddingEvent()
	: SP_JsonTextEvent( ePadding )
{
}

SP_JsonPaddingEvent :: ~SP_JsonPaddingEvent()
{
}

