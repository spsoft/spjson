/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "spjsonnode.hpp"
#include "spjsonutils.hpp"

SP_JsonNode :: SP_JsonNode( int type )
	: mType( type )
{
	mParent = NULL;
}

SP_JsonNode :: ~SP_JsonNode()
{
}

int SP_JsonNode :: getType() const
{
	return mType;
}

SP_JsonNode * SP_JsonNode :: getParent() const
{
	return mParent;
}

void SP_JsonNode :: setParent( SP_JsonNode * parent )
{
	mParent = parent;
}

int SP_JsonNode :: isLastChild() const
{
	if( NULL == mParent ) return 0;

	if( SP_JsonNode::eObject == mParent->getType() ) {
		return ((SP_JsonObjectNode*)mParent)->isLastChild( this );
	} else if( SP_JsonNode::eArray == mParent->getType() ) {
		return ((SP_JsonArrayNode*)mParent)->isLastChild( this );
	} else if( SP_JsonNode::ePair == mParent->getType() ) {
		SP_JsonObjectNode * object = (SP_JsonObjectNode*)mParent->getParent();
		return object->isLastChild( mParent );
	}

	return 0;
}

//=========================================================

SP_JsonStringNode :: SP_JsonStringNode( const char * value )
	: SP_JsonNode( eString )
{
	mValue = strdup( value );
}

SP_JsonStringNode :: ~SP_JsonStringNode()
{
	if( NULL != mValue ) free( mValue );
	mValue = NULL;
}

const char * SP_JsonStringNode :: getValue() const
{
	return mValue;
}

//=========================================================

SP_JsonDoubleNode :: SP_JsonDoubleNode( double value )
	: SP_JsonNode( eDouble )
{
	mValue = value;
}

SP_JsonDoubleNode :: ~SP_JsonDoubleNode()
{
}

double SP_JsonDoubleNode :: getValue() const
{
	return mValue;
}

//=========================================================

SP_JsonIntNode :: SP_JsonIntNode( long long value )
	: SP_JsonNode( eInt )
{
	mValue = value;
}

SP_JsonIntNode :: ~SP_JsonIntNode()
{
}

long long SP_JsonIntNode :: getValue() const
{
	return mValue;
}

//=========================================================

SP_JsonNullNode :: SP_JsonNullNode()
	: SP_JsonNode( eNull )
{
}

SP_JsonNullNode :: ~SP_JsonNullNode()
{
}

//=========================================================

SP_JsonBooleanNode :: SP_JsonBooleanNode( bool value )
	: SP_JsonNode( eBoolean )
{
	mValue = value;
}

SP_JsonBooleanNode :: ~SP_JsonBooleanNode()
{
}

bool SP_JsonBooleanNode :: getValue() const
{
	return mValue;
}

//=========================================================

SP_JsonCommentNode :: SP_JsonCommentNode( const char * comment )
	: SP_JsonNode( eComment )
{
	mValue = strdup( comment );
}

SP_JsonCommentNode :: ~SP_JsonCommentNode()
{
	free( mValue );
}

const char * SP_JsonCommentNode :: getValue() const
{
	return mValue;
}

//=========================================================

SP_JsonPairNode :: SP_JsonPairNode()
	: SP_JsonNode( ePair )
{
	mName = NULL;
	mValue = NULL;
}

SP_JsonPairNode :: ~SP_JsonPairNode()
{
	if( NULL != mName ) free( mName );
	mName = NULL;

	if( NULL != mValue ) delete mValue;
	mValue = NULL;
}

void SP_JsonPairNode :: setName( const char * name )
{
	if( name != mName ) {
		if( NULL != mName ) free( mName );
		mName = strdup( name );
	}
}

const char * SP_JsonPairNode :: getName() const
{
	return mName;
}

void SP_JsonPairNode :: setValue( SP_JsonNode * value )
{
	value->setParent( this );
	mValue = value;
}

SP_JsonNode * SP_JsonPairNode :: getValue() const
{
	return mValue;
}

//=========================================================

SP_JsonObjectNode :: SP_JsonObjectNode()
	: SP_JsonNode( eObject )
{
	mValueList = new SP_JsonArrayList();
}

SP_JsonObjectNode :: ~SP_JsonObjectNode()
{
	for( int i = 0; i < mValueList->getCount(); i++ ) {
		delete (SP_JsonNode*)mValueList->getItem(i);
	}
	delete mValueList;
	mValueList = NULL;
}

int SP_JsonObjectNode :: getCount() const
{
	return mValueList->getCount();
}

int SP_JsonObjectNode :: addValue( SP_JsonPairNode * value )
{
	value->setParent( this );

	mValueList->append( value );

	return 0;
}

SP_JsonPairNode * SP_JsonObjectNode :: getValue( int index ) const
{
	return (SP_JsonPairNode*)mValueList->getItem( index );
}

SP_JsonPairNode * SP_JsonObjectNode :: takeValue( int index )
{
	SP_JsonPairNode * ret = (SP_JsonPairNode*)mValueList->takeItem( index );
	if( NULL != ret ) ret->setParent( NULL );

	return ret;
}

int SP_JsonObjectNode :: isLastChild( const SP_JsonNode * value ) const
{
	const void * lastChild = mValueList->getItem( SP_JsonArrayList::LAST_INDEX );

	return NULL == value ? 0 : ( lastChild == value ? 1 : 0 );
}

int SP_JsonObjectNode :: find( const char * name ) const
{
	int ret = -1;
	for( int i = 0; i < mValueList->getCount(); i++ ) {
		SP_JsonPairNode * iter = (SP_JsonPairNode*)mValueList->getItem(i);
		if( 0 == strcmp( iter->getName(), name ) ) {
			ret = i;
			break;
		}
	}

	return ret;
}

//=========================================================

SP_JsonArrayNode :: SP_JsonArrayNode()
	: SP_JsonNode( eArray )
{
	mValueList = new SP_JsonArrayList();
}

SP_JsonArrayNode :: ~SP_JsonArrayNode()
{
	for( int i = 0; i < mValueList->getCount(); i++ ) {
		delete (SP_JsonNode*)mValueList->getItem(i);
	}
	delete mValueList;
	mValueList = NULL;
}

int SP_JsonArrayNode :: getCount() const
{
	return mValueList->getCount();
}

int SP_JsonArrayNode :: addValue( SP_JsonNode * value )
{
	value->setParent( this );
	return mValueList->append( value );
}

SP_JsonNode * SP_JsonArrayNode :: getValue( int index ) const
{
	return (SP_JsonNode*)mValueList->getItem( index );
}

SP_JsonNode * SP_JsonArrayNode :: takeValue( int index )
{
	SP_JsonNode * ret = (SP_JsonNode*)mValueList->takeItem( index );
	if( NULL != ret ) ret->setParent( NULL );

	return ret;
}

int SP_JsonArrayNode :: isLastChild( const SP_JsonNode * value ) const
{
	const void * lastChild = mValueList->getItem( SP_JsonArrayList::LAST_INDEX );

	return NULL == value ? 0 : ( lastChild == value ? 1 : 0 );
}

