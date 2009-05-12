/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>

#include "spjsonhandle.hpp"

#include "spjsonnode.hpp"

SP_JsonHandle :: SP_JsonHandle( const SP_JsonNode * node )
{
	mNode = node;
}

SP_JsonHandle :: SP_JsonHandle( const SP_JsonHandle & ref )
{
	mNode = ref.mNode;
}

SP_JsonHandle & SP_JsonHandle :: operator=( const SP_JsonHandle & ref )
{
	mNode = ref.mNode;
	return *this;
}

SP_JsonHandle SP_JsonHandle :: getChild( const char * name ) const
{
	SP_JsonNode * ret = NULL;

	if( NULL != mNode ) {
		if( SP_JsonNode::eObject == mNode->getType() ) {
			SP_JsonObjectNode * object = (SP_JsonObjectNode*)mNode;
			int index = object->find( name );
			if( index >= 0 ) {
				ret = object->getValue( index )->getValue();
			}
		}
	}

	return SP_JsonHandle( ret );
}

SP_JsonHandle SP_JsonHandle :: getChild( int index ) const
{
	SP_JsonNode * ret = NULL;

	if( NULL != mNode ) {
		if( SP_JsonNode::eArray == mNode->getType() ) {
			ret = ((SP_JsonArrayNode *)mNode)->getValue( index );
		} else if( SP_JsonNode::eObject == mNode->getType() ) {
			SP_JsonObjectNode * object = (SP_JsonObjectNode*)mNode;
			ret = object->getValue( index );
		}
	}

	return SP_JsonHandle( ret );
}

SP_JsonNode * SP_JsonHandle :: toNode() const
{
	return (SP_JsonNode*)mNode;
}

SP_JsonNode * SP_JsonHandle :: checkType( int type ) const
{
	if( NULL != mNode && mNode->getType() == type ) {
		return (SP_JsonNode*)mNode;
	}

	return NULL;
}

SP_JsonObjectNode * SP_JsonHandle :: toObject() const
{
	return (SP_JsonObjectNode*)checkType( SP_JsonNode::eObject );
}

SP_JsonArrayNode * SP_JsonHandle :: toArray() const
{
	return (SP_JsonArrayNode*)checkType( SP_JsonNode::eArray );
}

SP_JsonPairNode * SP_JsonHandle :: toPair() const
{
	return (SP_JsonPairNode*)checkType( SP_JsonNode::ePair );
}

SP_JsonStringNode * SP_JsonHandle :: toString() const
{
	return (SP_JsonStringNode*)checkType( SP_JsonNode::eString );
}

SP_JsonDoubleNode * SP_JsonHandle :: toDouble() const
{
	return (SP_JsonDoubleNode*)checkType( SP_JsonNode::eDouble );
}

SP_JsonIntNode * SP_JsonHandle :: toInt() const
{
	return (SP_JsonIntNode*)checkType( SP_JsonNode::eInt );
}

SP_JsonBooleanNode * SP_JsonHandle :: toBoolean() const
{
	return (SP_JsonBooleanNode*)checkType( SP_JsonNode::eBoolean );
}

SP_JsonNullNode * SP_JsonHandle :: toNull() const
{
	return (SP_JsonNullNode*)checkType( SP_JsonNode::eNull );
}

SP_JsonCommentNode * SP_JsonHandle :: toComment() const
{
	return (SP_JsonCommentNode*)checkType( SP_JsonNode::eComment );
}

