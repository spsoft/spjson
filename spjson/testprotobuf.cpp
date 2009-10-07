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

#include "sppbcodec.hpp"

void printString( const char * buffer, int len )
{
	char * tmp = SP_ProtoBufCodecUtils::dup( buffer, len );

	printf( "%s", tmp );

	free( tmp );
}

void printAll( const char * buffer, int len )
{
	SP_ProtoBufDecoder decoder( buffer, len );

	SP_ProtoBufDecoder::KeyValPair_t pair;

	for( ; ; ) {
		if( ! decoder.getNext( &pair ) ) break;

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
					printString( buf, len );
					printf( "\n" );
				} else {
					printf( "buffer %p, len %d\n", buf, len );

					printf( "{{{\n" );

					printAll( buf, len );

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

		person.addBinary( 4, phone.getBuffer(), phone.getSize() );

		phone.reset();

		phone.addBinary( 1, "6789", 4 );
		phone.addVarint( 2, 1 );
		person.addBinary( 4, phone.getBuffer(), phone.getSize() );

		addrbook.addBinary( 1, person.getBuffer(), person.getSize() );
	}

	{
		const char * filename = "testprotobuf.db";

		printf( "\nwrite addrbook info into %s ...\n\n", filename );

		FILE * fp = fopen( filename, "w" );
		if( NULL != fp ) {
			fwrite( addrbook.getBuffer(), 1, addrbook.getSize(), fp );
			fclose( fp );
		}
	}

	printf( "decode addrbook info ...\n\n" );

	printAll( addrbook.getBuffer(), addrbook.getSize() );

	SP_ProtoBufDecoder decoder( addrbook.getBuffer(), addrbook.getSize() );

	SP_ProtoBufDecoder::KeyValPair_t pair;
	assert( decoder.find( 1, &pair ) );

	{
		SP_ProtoBufDecoder person( pair.mBinary.mBuffer, pair.mBinary.mLen );
		assert( person.find( 4, &pair, 1 ) );
		assert( SP_ProtoBufDecoder::eWireBinary == pair.mWireType );

		SP_ProtoBufDecoder phone( pair.mBinary.mBuffer, pair.mBinary.mLen );
		assert( phone.find( 1, &pair ) );
		assert( SP_ProtoBufDecoder::eWireBinary == pair.mWireType );

		char * tmp = SP_ProtoBufCodecUtils::dup( pair.mBinary.mBuffer, pair.mBinary.mLen );

		assert( 0 == strcmp( tmp, "6789" ) );

		free( tmp );
	}
}

void testPacked()
{
	printf( "\ntest packed array\n" );

	SP_ProtoBufEncoder encoder;
	{
		uint32_t array[] = { 3, 270, 86942 };
		encoder.addPacked( 1, array, sizeof( array ) / sizeof( array[0] ) );
	}

	SP_ProtoBufDecoder decoder( encoder.getBuffer(), encoder.getSize() );

	SP_ProtoBufDecoder::KeyValPair_t pair;

	assert( decoder.find( 1, &pair ) );

	uint32_t array[ 100 ] = { 0 };

	int count = SP_ProtoBufCodecUtils::getPacked( pair.mBinary.mBuffer, pair.mBinary.mLen, array, 100 );

	for( int i = 0; i < count; i++ ) {
		printf( "#%d: %d\n", i, array[i] );
	}
}

int main( int argc, char * argv[] )
{
	if( argc > 1 ) {
		const char * filename = argv[1];

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


		printf( "\ndecode file -- %s ...\n\n", filename );

		printAll( source, aStat.st_size );

		free( source );
	}

	{
		testEncoder();
	}

	{
		testPacked();
	}

	return 0;
}

