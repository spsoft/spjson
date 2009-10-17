/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "testpbcomm.hpp"

#include "sppbcodec.hpp"

void printAll( char * buffer, int len )
{
	SP_ProtoBufDecoder decoder;
	decoder.attach( buffer, len );

	printAll( &decoder );
}

void printAll( SP_ProtoBufDecoder * decoder )
{
	SP_ProtoBufDecoder::KeyValPair_t pair;

	for( ; ; ) {
		if( ! decoder->getNext( &pair ) ) break;

    	printf("field number: %d, wire type %d, repeated count %d\n",
				pair.mFieldNumber, pair.mWireType, pair.mRepeatedCount );

		printf( "\tvalue: " );

		switch( pair.mWireType ) {
			case SP_ProtoBufDecoder::eWireVarint:
				printf( "%lld\n",  pair.mVarint.u );
				break;
			case SP_ProtoBufDecoder::eWire64Bit:
				printf( "%lld\n",  pair.mVarint.u );
				break;
			case SP_ProtoBufDecoder::eWireBinary:
			{
				const char * buf = pair.mBinary.mBuffer;
				int len = pair.mBinary.mLen;

				int isString = 1;

				for( int i = 0; i < len; i++ ) {
					if( ! isprint( buf[i] ) ) {
						isString = 0;
						break;
					}
				}

				if( isString ) {
					printf( "[%s]\n", buf );
				} else {
					printf( "buffer %p, len %d\n", buf, len );

					printf( "{{{\n" );

					printAllConst( buf, len );

					printf( "}}}\n" );
				}

				break;
			}
			case SP_ProtoBufDecoder::eWire32Bit:
				printf( "%d\n", pair.m32Bit.u );
				break;
			default:
				printf( "unknown\n" );
				break;
		}
	}
}

void printAllConst( const char * buffer, int len )
{
	char * tmp = (char*)malloc( len + 1 );
	memcpy( tmp, buffer, len );
	tmp[ len ] = '\0';

	printAll( tmp, len );

	printf( "\n" );

	free( tmp );
}

