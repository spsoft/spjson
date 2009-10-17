/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "sppbtree.hpp"

#include "spjsonutils.hpp"

typedef struct tagSP_ProtoBufTreeItem {
	int mFieldNumber : 31;
	int mIsRepeated : 1;

	union {
		SP_ProtoBufTree * mTree;
		SP_JsonArrayList * mList;
	};
} SP_ProtoBufTreeItem_t;


SP_ProtoBufTree :: SP_ProtoBufTree()
{
	mDecoder = NULL;
	mChildren = NULL;
}

SP_ProtoBufTree :: ~SP_ProtoBufTree()
{
	if( NULL != mDecoder ) delete mDecoder;
	mDecoder = NULL;

	if( NULL != mChildren ) {
		for( int i = 0; i < mChildren->getCount(); i++ ) {
			SP_ProtoBufTreeItem_t * item = (SP_ProtoBufTreeItem_t*)mChildren->getItem(i);

			if( item->mIsRepeated ) {
				for( int j = 0; j < item->mList->getCount(); j++ ) {
					SP_ProtoBufTree * tree = (SP_ProtoBufTree*)item->mList->getItem(j);
					delete tree;
				}
				delete item->mList;
			} else {
				delete item->mTree;
			}

			free( item );
		}

		delete mChildren;
		mChildren = NULL;
	}
}

int SP_ProtoBufTree :: attach( char * buffer, int len )
{
	assert( NULL == mDecoder );

	mDecoder = new SP_ProtoBufDecoder();
	mDecoder->attach( buffer, len );

	return 0;
}

int SP_ProtoBufTree :: copyFrom( const char * buffer, int len )
{
	assert( NULL == mDecoder );

	mDecoder = new SP_ProtoBufDecoder();
	mDecoder->copyFrom( buffer, len );

	return 0;
}

SP_ProtoBufDecoder * SP_ProtoBufTree :: getDecoder()
{
	return mDecoder;
}

SP_ProtoBufTree * SP_ProtoBufTree :: findChild( int fieldNumber, int index )
{
	SP_ProtoBufTree * ret = NULL;

	bool isExist = false;

	if( NULL != mChildren ) {
		for( int i = 0; i < mChildren->getCount(); i++ ) {
			SP_ProtoBufTreeItem_t * item = (SP_ProtoBufTreeItem_t*)mChildren->getItem(i);

			if( fieldNumber == item->mFieldNumber ) {
				if( item->mIsRepeated ) {
					ret = (SP_ProtoBufTree*)item->mList->getItem( index );
				} else {
					if( 0 == index ) ret = item->mTree;
				}
				isExist = true;
				break;
			}
		}
	}

	if( ! isExist ) {
		SP_ProtoBufDecoder::KeyValPair_t pair;

		isExist = mDecoder->find( fieldNumber, &pair );

		if( isExist ) {
			if( SP_ProtoBufDecoder::eWireBinary == pair.mWireType ) {
				SP_ProtoBufTreeItem_t * item = (SP_ProtoBufTreeItem_t*)
						calloc( sizeof( SP_ProtoBufTreeItem_t ), 1 );

				item->mFieldNumber = fieldNumber;
				item->mIsRepeated = pair.mRepeatedCount > 1;

				if( 1 == pair.mRepeatedCount ) {
					item->mTree = new SP_ProtoBufTree();
					item->mTree->attach( pair.mBinary.mBuffer, pair.mBinary.mLen );
				} else {
					item->mList = new SP_JsonArrayList();

					for( int i = 0; i < pair.mRepeatedCount; i++ ) {
						mDecoder->find( fieldNumber, &pair, i );

						SP_ProtoBufTree * tree = new SP_ProtoBufTree();
						tree->attach( pair.mBinary.mBuffer, pair.mBinary.mLen );

						item->mList->append( tree );
					}
				}

				if( NULL == mChildren ) mChildren = new SP_JsonArrayList();

				mChildren->append( item );

				if( item->mIsRepeated ) {
					ret = (SP_ProtoBufTree*)item->mList->getItem( index );
				} else {
					if( 0 == index ) ret = item->mTree;
				}

			}
		}
	}

	return ret;
}

