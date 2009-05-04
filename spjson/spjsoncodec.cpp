/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <math.h>

#include "spjsoncodec.hpp"
#include "spjsonutils.hpp"

const char SP_JsonCodec :: ESCAPE_CHARS [] =
		{ '"', '\\', '\b', '\f', '\n', '\r', '\t' };
const char SP_JsonCodec :: UNESCAPED_CHARS [] =
		{ '"', '\\', 'b',  'f',  'n',  'r',  't' };

int SP_JsonCodec :: decode( const char * encodeValue,
		SP_JsonStringBuffer * outBuffer )
{
	int isEscape = 0;

	const char * pos = encodeValue;
	for( ; '\0' != *pos; pos++ ) {
		if( '\\' == *pos && 0 == isEscape ) {
			isEscape = 1;
		} else {
			if( 1 == isEscape ) {
				isEscape = 0;
				int index = -1;
				for( int i = 0; i < (int)sizeof( UNESCAPED_CHARS ); i++ ) {
					if( UNESCAPED_CHARS[i] == *pos ) {
						index = i;
						break;
					}
				}
				if( index >= 0 ) {
					outBuffer->append( ESCAPE_CHARS[ index ] );
				} else {
					// unknown escape, keep it not change
					outBuffer->append( '\\' );
					outBuffer->append( *pos );
				}
			} else {
				outBuffer->append( *pos );
			}
		}
	}

	return 0;
}

int SP_JsonCodec :: encode( const char * decodeValue,
		SP_JsonStringBuffer * outBuffer )
{
	const char * pos = decodeValue;
	for( ; '\0' != *pos; pos++ ) {
		int index = -1;
		for( int i = 0; i < (int)sizeof( ESCAPE_CHARS ); i++ ) {
			if( ESCAPE_CHARS[i] == *pos ) {
				index = i;
				break;
			}
		}
		if( index >= 0 ) {
			outBuffer->append( '\\' );
			outBuffer->append( UNESCAPED_CHARS[index] );
		} else {
			outBuffer->append( *pos );
		}	
	}

	return 0;
}

int SP_JsonCodec :: encode( double value, SP_JsonStringBuffer * outBuffer )
{
	char buffer[ 128 ] = { 0 };

	encode( value, buffer, sizeof( buffer ) );

	return outBuffer->append( buffer );
}

int SP_JsonCodec :: encode( double value, char * buffer, int len )
{
	if( ((double)ceil( value )) == value ) {
		snprintf( buffer, len, "%.0f", value );
	} else {
		snprintf( buffer, len, "%f", value );
	}

	return 0;
}

