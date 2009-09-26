/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>

#include "spprotobuf.hpp"

SP_ProtoBufReader :: SP_ProtoBufReader( const char * buffer, int len )
{
	mBuffer = buffer;
	mEnd = mBuffer + len;

	mCurr = mBuffer;
}

SP_ProtoBufReader :: ~SP_ProtoBufReader()
{
	mBuffer = NULL;
	mEnd = NULL;
	mCurr = NULL;
}

int SP_ProtoBufReader :: decodeVarint( uint64_t *value, const char *buffer )
{
	int pos = 0, shift = 0;
	uint64_t dummy, result = 0;
	unsigned char b = (unsigned char)buffer[pos++];
	while (b >= 128) {
		dummy = (b & 127);
		result += (dummy << shift);
		shift += 7;
		b = (unsigned char)buffer[pos++];
	}
	dummy = b;
	result += (dummy << shift);
	*value = result;
	return pos;
}

uint32_t SP_ProtoBufReader :: get32Bit( const char * buffer )
{
	return buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);
}

bool SP_ProtoBufReader :: getNext( KeyValPair_t * pair )
{
	if( mCurr >= mEnd ) return false;

	int ret = getPair( mCurr, pair );

	if( ret > 0 ) mCurr += ret;

	return ret > 0;
}

int SP_ProtoBufReader :: getPair( const char * buffer, KeyValPair_t * pair )
{
	int ret = 0;

	const char * curr = buffer;

	uint64_t tmpVal = 0;
	int pos = decodeVarint( &tmpVal, curr );

	curr += pos;

	pair->mFieldNumber = tmpVal >> 3;
	pair->mWireType = tmpVal & 0x07;

	switch( pair->mWireType )
	{
		case eWireVarint:
			pos = decodeVarint( &pair->mValue.mVarint, curr );
			curr += pos;
			break;

		case eWire64Bit:
			pair->mValue.m64Bit = get32Bit( curr );
			curr += 4;
			pair->mValue.m64Bit |= (uint64_t)get32Bit( curr ) << 32;
			curr += 4;
			break;

		case eWireBinary:
			pos = decodeVarint( &tmpVal, curr );
			curr += pos;

			pair->mValue.mBinary.mLen = tmpVal;
			pair->mValue.mBinary.mBuffer = curr ;
			curr += tmpVal;
			break;

		case eWire32Bit:
			pair->mValue.m32Bit = get32Bit( curr );
			curr += 4;
			break;

		default:
			ret = -1;
			break;
	}

	return 0 == ret ? ( curr - buffer ) : -1;
}

bool SP_ProtoBufReader :: find( int fieldNumber, KeyValPair_t * pair )
{
	bool isExist = false;

	const char * curr = mBuffer;

	for( ; ; ) {
		int ret = getPair( curr, pair );

		if( ret < 0 ) break;

		if( pair->mFieldNumber == fieldNumber ) {
			isExist = true;
			break;
		}

		curr += ret;
	}

	return isExist;
}

void SP_ProtoBufReader :: rewind()
{
	mCurr = mBuffer;
}

