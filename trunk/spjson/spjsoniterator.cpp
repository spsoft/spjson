/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>
#include <assert.h>

#include "spjsoniterator.hpp"

#include "spjsonnode.hpp"


SP_JsonIterator :: SP_JsonIterator( const SP_JsonNode * node )
{
	mRoot = node;
	mCurrent = node;
}

SP_JsonIterator :: ~SP_JsonIterator()
{
}

const SP_JsonNode * SP_JsonIterator :: getNext()
{
	if( NULL == mCurrent ) return NULL;

	const SP_JsonNode * retNode = mCurrent;

	// find first left child
	if( SP_JsonNode::eObject == mCurrent->getType() ) {
		SP_JsonObjectNode * object = static_cast<SP_JsonObjectNode*>((SP_JsonNode*)mCurrent);

		mCurrent = object->getValue(0);
	} else if( SP_JsonNode::eArray == mCurrent->getType() ) {
		SP_JsonArrayNode * array = static_cast<SP_JsonArrayNode*>((SP_JsonNode*)mCurrent);

		mCurrent = array->getValue( 0 );
	} else if( SP_JsonNode::ePair == mCurrent->getType() ) {
		SP_JsonPairNode * pair = static_cast<SP_JsonPairNode*>((SP_JsonNode*)mCurrent);
		mCurrent = pair->getValue();
	} else {
		mCurrent = NULL;
	}

	// find next sibling
	if( NULL == mCurrent ) {
		mCurrent = retNode;

		const SP_JsonNode * parent = NULL;
		if( NULL != mCurrent ) parent = mCurrent->getParent();

		for( ; NULL != parent; ) {
			if( SP_JsonNode::eObject == parent->getType() ) {
				SP_JsonObjectNode * object = static_cast<SP_JsonObjectNode*>((SP_JsonNode*)parent);

				int index = -1;
				for( int i = 0; i < object->getCount(); i++ ) {
					if( mCurrent == object->getValue( i ) ) {
						index = i;
						break;
					}
				}

				if( index >= 0 && index < ( object->getCount() - 1 ) ) {
					mCurrent = object->getValue( index + 1 );
				} else {
					mCurrent = NULL;
				}
			} else if( SP_JsonNode::eArray == parent->getType() ) {
				SP_JsonArrayNode * array = static_cast<SP_JsonArrayNode*>((SP_JsonNode*)parent);

				int index = -1;
				for( int i = 0; i < array->getCount(); i++ ) {
					if( mCurrent == array->getValue( i ) ) {
						index = i;
						break;
					}
				}

				if( index >= 0 && index < ( array->getCount() - 1 ) ) {
					mCurrent = array->getValue( index + 1 );
				} else {
					mCurrent = NULL;
				}
			} else if( SP_JsonNode::ePair == parent->getType() ) {
				mCurrent = NULL;
			} else {
				mCurrent = NULL;
				assert( 0 ); // should not occur
			}

			if( NULL == mCurrent ) {
				mCurrent = parent;
				parent = mCurrent->getParent();
				if( NULL == parent ) mCurrent = NULL;
				if( mRoot == mCurrent ) mCurrent = NULL;
			} else {
				break;
			}
		}
	}

	return retNode;
}

