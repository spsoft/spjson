/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "spjsondomparser.hpp"

#include "spjsonparser.hpp"
#include "spjsonnode.hpp"
#include "spjsonutils.hpp"
#include "spjsonevent.hpp"
#include "spjsoncodec.hpp"

SP_JsonDomParser :: SP_JsonDomParser()
{
	mParser = new SP_JsonPullParser();	
	mContainer = new SP_JsonArrayNode();
	mCurrent = mContainer;
}

SP_JsonDomParser :: ~SP_JsonDomParser()
{
	if( NULL != mParser ) delete mParser;
	mParser = NULL;

	if( NULL != mContainer ) delete mContainer;
	mContainer = NULL;
}

int SP_JsonDomParser :: append( const char * source, int len )
{
	int ret = 0;

	for( int pos = 0; pos < len && NULL == mParser->getError(); pos += 64 ) {
		int realLen = ( len - pos ) > 64 ? 64 : ( len - pos );
		ret += mParser->append( source + pos, realLen );
		buildTree();
	}

	return ret;
}

void SP_JsonDomParser :: addNode( SP_JsonNode * node )
{
	if( SP_JsonNode::eObject == mCurrent->getType() ) {
		assert( SP_JsonNode::ePair == node->getType() );
		((SP_JsonObjectNode*)mCurrent)->addValue( (SP_JsonPairNode*)node );
	} else if( SP_JsonNode::ePair == mCurrent->getType() ) {
		((SP_JsonPairNode*)mCurrent)->setValue( node );
		mCurrent = mCurrent->getParent();
	} else {
		assert( SP_JsonNode::eArray == mCurrent->getType() );
		((SP_JsonArrayNode*)mCurrent)->addValue( node );

		if( mCurrent == mContainer ) node->setParent( NULL );
	}
}

void SP_JsonDomParser :: buildTree()
{
	for( SP_JsonPullEvent * event = mParser->getNext();
			NULL != event;
			event = mParser->getNext() ) {

		switch( event->getEventType() ) {
			case SP_JsonPullEvent::eStartObject:
			{
				SP_JsonObjectNode * newObject = new SP_JsonObjectNode();
				addNode( newObject );
				mCurrent = newObject;
				break;
			}
			case SP_JsonPullEvent::eEndObject:
			{
				mCurrent = mCurrent->getParent();
				if( mCurrent && SP_JsonNode::ePair == mCurrent->getType() ) {
					mCurrent = mCurrent->getParent();
					assert( SP_JsonNode::eObject == mCurrent->getType() );
				}
				break;
			}
			case SP_JsonPullEvent::eStartArray:
			{
				SP_JsonArrayNode * newArray = new SP_JsonArrayNode();
				addNode( newArray );
				mCurrent = newArray;
				break;
			}
			case SP_JsonPullEvent::eEndArray:
			{
				mCurrent = mCurrent->getParent();
				if( mCurrent && SP_JsonNode::ePair == mCurrent->getType() ) {
					mCurrent = mCurrent->getParent();
					assert( SP_JsonNode::eObject == mCurrent->getType() );
				}
				break;
			}
			case SP_JsonPullEvent::eName:
			{
				SP_JsonPairNode * newPair = new SP_JsonPairNode();
				newPair->setName( ((SP_JsonTextEvent*)event)->getText() );
				addNode( newPair );
				mCurrent = newPair;
				break;
			}
			case SP_JsonPullEvent::eNull:
			{
				SP_JsonNullNode * newNull = new SP_JsonNullNode();
				addNode( newNull );
				break;
			}
			case SP_JsonPullEvent::eString:
			{
				SP_JsonStringNode * newString = new SP_JsonStringNode( ((SP_JsonTextEvent*)event)->getText() );
				addNode( newString );
				break;
			}
			case SP_JsonPullEvent::eNumber:
			{
				SP_JsonNode * newNode = NULL;

				const char * text = ((SP_JsonTextEvent*)event)->getText();
				if( NULL != strchr( text, '.' ) ) {
					newNode = new SP_JsonDoubleNode( strtod( text, NULL ) );
				} else {
					newNode = new SP_JsonIntNode( atoi( text ) );
				}
				addNode( newNode );
				break;
			}
			case SP_JsonPullEvent::eBoolean:
			{
				const char * text = ((SP_JsonTextEvent*)event)->getText();
				
				SP_JsonBooleanNode * newBoolean = new SP_JsonBooleanNode( 0 == strcasecmp( text, "true" ) );
				addNode( newBoolean );
				break;
			}
			case SP_JsonPullEvent::eComment:
			{
				const char * text = ((SP_JsonTextEvent*)event)->getText();
				
				SP_JsonCommentNode * newComment = new SP_JsonCommentNode( text );
				addNode( newComment );
				break;
			}
			case SP_JsonPullEvent::ePadding:
				// ignore
				break;
		}

		delete event;
	}
}

const char * SP_JsonDomParser :: getError() const
{
	return mParser->getError();
}

SP_JsonNode * SP_JsonDomParser :: getValue() const
{
	return mContainer->getValue( 0 );
}

//=========================================================

SP_JsonDomBuffer :: SP_JsonDomBuffer( const SP_JsonNode * node, int indent )
{
	mBuffer = new SP_JsonStringBuffer();
	dump( node, mBuffer, indent ? 0 : -1 );
}

SP_JsonDomBuffer :: ~SP_JsonDomBuffer()
{
	delete mBuffer;
	mBuffer = NULL;
}

const char * SP_JsonDomBuffer :: getBuffer() const
{
	return mBuffer->getBuffer();
}

int SP_JsonDomBuffer :: getSize() const
{
	return mBuffer->getSize();
}

void SP_JsonDomBuffer :: dump( const SP_JsonNode * node, char * buffer, int len )
{
	buffer[ len - 1 ] = '\0';

	if( SP_JsonNode::eObject == node->getType() ) {
		strncpy( buffer, "@object", len - 1 );
	} else if( SP_JsonNode::eArray == node->getType() ) {
		strncpy( buffer, "@array", len - 1 );
	} else {
		if( SP_JsonNode::eString == node->getType() ) {
			strncpy( buffer, ((SP_JsonStringNode*)node)->getValue(), len - 1 );
		} else if( SP_JsonNode::eDouble == node->getType() ) {
			SP_JsonCodec::encode( ((SP_JsonDoubleNode*)node)->getValue(), buffer, len );
		} else if( SP_JsonNode::eInt == node->getType() ) {
			snprintf( buffer, len, "%lld", ((SP_JsonIntNode*)node)->getValue() );
		} else if( SP_JsonNode::eBoolean == node->getType() ) {
			strncpy( buffer, ((SP_JsonBooleanNode*)node)->getValue() ? "true" : "false", len - 1 );
		} else if( SP_JsonNode::eNull == node->getType() ) {
			strncpy( buffer, "null", len - 1 );
		} else {
			assert( SP_JsonNode::eComment == node->getType() );

			snprintf( buffer, len, "//%s", ((SP_JsonCommentNode*)node)->getValue() );
		}
	}
}

void SP_JsonDomBuffer :: dump( const SP_JsonNode * node,
		SP_JsonStringBuffer * buffer, int level )
{
	if( SP_JsonNode::eObject == node->getType() ) {
		dumpObject( node, buffer, level );
	} else if( SP_JsonNode::eArray == node->getType() ) {
		dumpArray( node, buffer, level );
	} else {
		char tmpBuffer[ 128 ] = { 0 };

		if( SP_JsonNode::eString == node->getType() ) {
			buffer->append( '"' );
			SP_JsonCodec::encode( ((SP_JsonStringNode*)node)->getValue(), buffer );
			buffer->append( '"' );
		} else if( SP_JsonNode::eDouble == node->getType() ) {
			SP_JsonCodec::encode( ((SP_JsonDoubleNode*)node)->getValue(), buffer );
		} else if( SP_JsonNode::eInt == node->getType() ) {
			snprintf( tmpBuffer, sizeof( tmpBuffer ), "%lld", ((SP_JsonIntNode*)node)->getValue() );
			buffer->append( tmpBuffer );
		} else if( SP_JsonNode::eBoolean == node->getType() ) {
			buffer->append( ((SP_JsonBooleanNode*)node)->getValue() ? "true" : "false" );
		} else if( SP_JsonNode::eNull == node->getType() ) {
			buffer->append( "null" );
		} else {
			assert( SP_JsonNode::eComment == node->getType() );

			buffer->append( "//" );
			buffer->append( ((SP_JsonCommentNode*)node)->getValue() );
			buffer->append( "\r\n" );
		}
	}
}

void SP_JsonDomBuffer :: dumpObject( const SP_JsonNode * node,
		SP_JsonStringBuffer * buffer, int level )
{
	SP_JsonObjectNode * objectNode = (SP_JsonObjectNode*)node;

	buffer->append( '{' );

	for( int i = 0; i < objectNode->getCount(); i++ ) {
		if( level >= 0 ) {
			buffer->append( "\n" );
			for( int i = 0; i <= level; i++ ) buffer->append( "\t" );
		}

		SP_JsonPairNode * pairNode = objectNode->getValue( i );
		const char * name = pairNode->getName();
		const SP_JsonNode * iter = pairNode->getValue();

		buffer->append( '"' );

		SP_JsonCodec::encode( name, buffer );

		buffer->append( "\" : " );

		dump( iter, buffer, level >= 0 ? level + 1 : -1 );

		if( ( i + 1 ) != objectNode->getCount()
				&& SP_JsonNode::eComment != iter->getType() ) buffer->append( ',' );
	}

	if( level >= 0 ) {
		buffer->append( "\n" );
		for( int i = 0; i < level; i++ ) buffer->append( "\t" );
	}

	buffer->append( '}' );
}

void SP_JsonDomBuffer :: dumpArray( const SP_JsonNode * node,
		SP_JsonStringBuffer * buffer, int level )
{
	SP_JsonArrayNode * arrayNode = (SP_JsonArrayNode*)node;

	buffer->append( '[' );

	for( int i = 0; i < arrayNode->getCount(); i++ ) {
		if( level >= 0 ) {
			buffer->append( "\n" );
			for( int i = 0; i <= level; i++ ) buffer->append( "\t" );
		}

		const SP_JsonNode * iter = arrayNode->getValue( i );

		dump( iter, buffer, level >= 0 ? level + 1 : -1 );

		if( ( i + 1 ) != arrayNode->getCount()
				&& SP_JsonNode::eComment != iter->getType() ) buffer->append( ',' );
	}

	if( level >= 0 ) {
		buffer->append( "\n" );
		for( int i = 0; i < level; i++ ) buffer->append( "\t" );
	}

	buffer->append( ']' );
}

