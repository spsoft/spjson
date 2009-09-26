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

void printAll( SP_ProtoBufReader * reader )
{
	SP_ProtoBufReader::KeyValPair_t pair;

	for( ; ; ) {
		if( ! reader->getNext( &pair ) ) break;

    	printf("field number: %d, wire type %d\n",
				pair.mFieldNumber, pair.mWireType );

		printf( "\tvalue: " );

		switch( pair.mWireType ) {
			case SP_ProtoBufReader::eWireVarint:
				printf( "%lld\n",  pair.mValue.mVarint );
				break;
			case SP_ProtoBufReader::eWire64Bit:
				printf( "%lld\n",  pair.mValue.mVarint );
				break;
			case SP_ProtoBufReader::eWireBinary:
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

					SP_ProtoBufReader tmpReader( buf, len );
					printAll( &tmpReader );

					printf( "}}}\n" );
				}

				break;
			}
			case SP_ProtoBufReader::eWire32Bit:
				printf( "%d\n", pair.mValue.m32Bit );
				break;
			default:
				printf( "unknown\n" );
				break;
		}
	}
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
		SP_ProtoBufReader reader( source, aStat.st_size );

		printAll( &reader );
	}

	free( source );

	return 0;
}

