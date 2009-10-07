/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "sppbfield.hpp"

SP_ProtoBufFieldList :: SP_ProtoBufFieldList()
{
	mList = NULL;	
	mCount = mTotal = 0;
}

SP_ProtoBufFieldList :: ~SP_ProtoBufFieldList()
{
	if( NULL != mList ) {
		for( int i = 0; i < mCount; i++ ) {
			Field_t * field = mList + i;

			if( field->mIsRepeated ) {
				free( field->mList );
			}
		}

		free( mList );
		mList = NULL;
	}
}

void SP_ProtoBufFieldList :: print()
{
	printf( "field count %d\n", mCount );

	for( int i = 0; i < mCount; i++ ) {
		Field_t * field = mList + i;

		printf( "field#%d: id %d, is.repeated %s", i, field->mFieldNumber,
				field->mIsRepeated ? "1" : "0" );
		if( field->mIsRepeated ) {
			printf( "\n\toffset " );
			for( int j = 0; j < field->mList->mCount; j++ ) {
				printf( "%d, ", field->mList->mList[j] );
			}
			printf( "\n" );
		} else {
			printf( ", offset %d\n", field->mOffset );
		}
	}

	printf( "\n" );
}

int SP_ProtoBufFieldList :: binarySearch( int fieldNumber, int * insertPoint,
		int firstIndex, int size ) const
{
	if( NULL == mList ) {
		if( NULL != insertPoint ) * insertPoint = 0;
		return -1;
	}

	// if aiSize not specify, then search the hold list
	if( size == -1 )  size = mCount;
	// can't find the key
	if( size == 0 ) {
		// set the insert point
		if( insertPoint != NULL ) * insertPoint = firstIndex; 
		return -1;  // return "can't find"
	}

	int cmpRet = fieldNumber - mList[ firstIndex + ( size - 1 ) / 2 ].mFieldNumber;

	if( cmpRet < 0 ) {
		return binarySearch( fieldNumber, insertPoint, firstIndex, ( size - 1 ) / 2 );
	} else if( cmpRet > 0 ) {
		return binarySearch( fieldNumber, insertPoint, firstIndex + ( ( size - 1 ) / 2 ) + 1,
				size - ( ( size - 1 ) / 2 ) - 1 );
	} else { // find it
		return( firstIndex + ( size - 1 ) / 2 );
	}
}

int SP_ProtoBufFieldList :: addFieldOffset( int fieldNumber, int offset )
{
	int ret = -1;

	int insertPoint =  -1;

	int index = binarySearch( fieldNumber, &insertPoint, 0, -1 );

	if( index >= 0 ) {
		Field_t * field = &( mList[ index ] );

		ensureOffsetList( field, 1 );

		OffsetList_t * list = field->mList;

		list->mList[ list->mCount ] = offset;
		++ list->mCount;

		ret = 1;
	} else {
		ensureFieldList( 1 );

		memmove( mList + insertPoint + 1, mList + insertPoint,
				( mTotal - insertPoint - 1 ) * sizeof( Field_t ) );

		Field_t * field = &( mList[ insertPoint ] );
		field->mFieldNumber = fieldNumber;
		field->mIsRepeated = 0;
		field->mOffset = offset;
		++mCount;

		ret = 0;
	}

	return ret;
}

SP_ProtoBufFieldList::Field_t * SP_ProtoBufFieldList :: findField( int fieldNumber )
{
	Field_t * ret = NULL;

	int index = binarySearch( fieldNumber, NULL, 0, -1 );

	if( index >= 0 ) ret = &( mList[ index ] );

	return ret;
}

int SP_ProtoBufFieldList :: getFieldCount()
{
	return mCount;
}

SP_ProtoBufFieldList::Field_t * SP_ProtoBufFieldList :: getField( int index )
{
	Field_t * ret = NULL;

	if( index >= 0 && index < mCount ) ret = &( mList[ index ] );

	return ret;
}

int SP_ProtoBufFieldList :: ensureFieldList( int count )
{
	int need = mCount + count;
	if( need > mTotal ) {
		int total = mTotal < 16 ? 16 : mTotal;

		for( ; total < need; ) total = total * 2;

		Field_t * tmp = (Field_t*)realloc( mList, sizeof( Field_t ) * total );

		if( NULL == tmp ) return -1;

		mList = tmp;
		mTotal = total;
	}

	return 0;
}

int SP_ProtoBufFieldList :: ensureOffsetList( Field_t * field, int count )
{
	int need = 0, total = 0;

	if( field->mIsRepeated ) {
		need = field->mList->mCount + count;
		total = field->mList->mTotal;
	} else {
		need = count + 1;
		total = 0;
	}

	if( need > total ) {
		total = total < 8 ? 8 : total;
		for( ; total < need; ) total = total * 2;

		OffsetList_t * newList = (OffsetList_t*)malloc(
				total * sizeof( int ) + sizeof( OffsetList_t ) );

		newList->mTotal = total;

		if( field->mIsRepeated ) {
			OffsetList_t * oldList = field->mList;
			memcpy( newList, oldList,
				sizeof( int ) * oldList->mTotal + sizeof( OffsetList_t ) );

			field->mList = newList;
			free( oldList );
		} else {
			newList->mList[0] = field->mOffset;
			newList->mCount = 1;

			field->mIsRepeated = 1;
			field->mList = newList;
		}
	}

	return 0;
}

