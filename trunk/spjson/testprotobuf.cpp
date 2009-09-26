/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <ctype.h>

#include "spprotobuf.hpp"

void printAll( const char * buffer, int len )
{
	SP_ProtoBufDecoder decoder( buffer, len );

	SP_ProtoBufDecoder::KeyValPair_t pair;

	for( ; ; ) {
		if( ! decoder.getNext( &pair ) ) break;

    	printf("field number: %d, wire type %d\n",
				pair.mFieldNumber, pair.mWireType );

		printf( "\tvalue: " );

		switch( pair.mWireType ) {
			case SP_ProtoBufDecoder::eWireVarint:
				printf( "%lld\n",  pair.mValue.mVarint );
				break;
			case SP_ProtoBufDecoder::eWire64Bit:
				printf( "%lld\n",  pair.mValue.mVarint );
				break;
			case SP_ProtoBufDecoder::eWireBinary:
			{
				const char * buf = pair.mValue.mBinary.mBuffer;
				int len = pair.mValue.mBinary.mLen;

				int isString = 1;

				for( int i = 0; i < len; i++ ) {
					if( ! isprint( buf[i] ) ) {
						isString = 0;
						break;
					}
				}

				if( isString ) {
					char * tmp = (char*)malloc( len + 1 );
					memcpy( tmp, buf, len );
					tmp[ len ] = '\0';
					printf( "%s\n", tmp );
					free( tmp );
				} else {
					printf( "buffer %p, len %d\n", buf, len );

					printf( "{{{\n" );

					printAll( buf, len );

					printf( "}}}\n" );
				}

				break;
			}
			case SP_ProtoBufDecoder::eWire32Bit:
				printf( "%d\n", pair.mValue.m32Bit );
				break;
			default:
				printf( "unknown\n" );
				break;
		}
	}
}

void testEncoder()
{
	SP_ProtoBufEncoder addrbook;

	{
		SP_ProtoBufEncoder person;

		person.addBinary( 1, "abc", 3 );
		person.addVarint( 2, 1 );
		person.addBinary( 3, "abc@abc.com", 11 );

		SP_ProtoBufEncoder phone;
		phone.addBinary( 1, "12345", 5 );
		phone.addVarint( 2, 0 );

		person.addBinary( 4, phone.getBuffer(), phone.getLen() );

		addrbook.addBinary( 1, person.getBuffer(), person.getLen() );
	}

	printAll( addrbook.getBuffer(), addrbook.getLen() );
}

int main( int argc, char * argv[] )
{
	if( argc < 2 ) {
		printf( "Usage: %s <file>\n", argv[0] );
		return -1;
	}

	char * filename = NULL;

	if( argc < 2 ) {
		printf( "Usage: %s <json_file>\n", argv[0] );
		exit( -1 );
	} else {
		filename = argv[1];
	}

	FILE * fp = fopen ( filename, "r" );
	if( NULL == fp ) {
		printf( "cannot not open %s\n", filename );
		exit( -1 );
	}

	struct stat aStat;
	char * source = NULL;
	stat( filename, &aStat );
	source = ( char * ) malloc ( aStat.st_size + 1 );
	fread ( source, aStat.st_size, sizeof ( char ), fp );
	fclose ( fp );
	source[ aStat.st_size ] = '\0';

	{
		printAll( source, aStat.st_size );
	}

	free( source );

	{
		testEncoder();
	}

	return 0;
}

