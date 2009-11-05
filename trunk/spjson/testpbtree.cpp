/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>

#include "sppbtree.hpp"

void printPair( SP_ProtoBufDecoder::KeyValPair_t * pair )
{
   	printf("field number: %d, wire type %d, repeated count %d\n",
			pair->mFieldNumber, pair->mWireType, pair->mRepeatedCount );

	printf( "\tvalue: " );

	switch( pair->mWireType ) {
		case SP_ProtoBufDecoder::eWireVarint:
			printf( "%lld\n",  pair->mVarint.u );
			break;
		case SP_ProtoBufDecoder::eWire64Bit:
			printf( "%lld\n",  pair->mVarint.u );
			break;
		case SP_ProtoBufDecoder::eWireBinary:
			printf( "[%s]\n", pair->mBinary.mBuffer );
			break;
		case SP_ProtoBufDecoder::eWire32Bit:
			printf( "%d\n", pair->m32Bit.u );
			break;
		default:
			printf( "unknown\n" );
			break;
	}
}

void testTree()
{
	int i = 0;

	SP_ProtoBufEncoder encoder;

	encoder.addVarint( 1, 16 );

	for( i = 0; i < 3; i++ ) {
		SP_ProtoBufEncoder tmp;

		tmp.addVarint( 1, i );

		char tmpstr[ 128 ] = { 0 };
		snprintf( tmpstr, sizeof( tmpstr ), "test%d", i );
		tmp.addString( 2, tmpstr );

		snprintf( tmpstr, sizeof( tmpstr ), "test%d@abc.com", i );
		tmp.addString( 3, tmpstr );

		encoder.addBinary( 5, tmp.getBuffer(), tmp.getSize() );
	}

	{
		SP_ProtoBufEncoder tmp;

		tmp.addVarint( 1, 9999 );
		tmp.addString( 2, "test9999" );
		tmp.addString( 3, "test9999@abc.com" );

		encoder.addBinary( 3, tmp.getBuffer(), tmp.getSize() );
	}

	SP_ProtoBufTree tree;
	tree.copyFrom( encoder.getBuffer(), encoder.getSize() );

	tree.findChild( 3 );

	SP_ProtoBufDecoder::KeyValPair_t pair;

	tree.getDecoder()->find( 1, &pair );
	printPair( &pair );

	tree.getDecoder()->find( 5, &pair );
	printPair( &pair );

	for( i = 0; i < pair.mRepeatedCount; i++ ) {
		SP_ProtoBufTree * item = tree.findChild( 5, i );

		SP_ProtoBufDecoder::KeyValPair_t tmp;

		item->getDecoder()->find( 1, &tmp );
		printPair( &tmp );

		item->getDecoder()->find( 2, &tmp );
		printPair( &tmp );

		item->getDecoder()->find( 3, &tmp );
		printPair( &tmp );

		printf( "\n\n" );
	}
}

int main( int argc, char * argv[] )
{
	testTree();

	return 0;
}

