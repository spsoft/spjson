/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sppbcodec.hpp"
#include "sppbfield.hpp"

SP_ProtoBufEncoder :: SP_ProtoBufEncoder( int initLen )
{
	mBuffer = NULL;
	mTotal = 0;
	mSize = 0;

	ensureSpace( initLen );
}

SP_ProtoBufEncoder :: ~SP_ProtoBufEncoder()
{
	if( NULL != mBuffer ) free( mBuffer );

	mBuffer = NULL;
	mTotal = 0;
	mSize = 0;
}

void SP_ProtoBufEncoder :: reset()
{
	mSize = 0;

	if( NULL != mBuffer ) memset( mBuffer, 0, mTotal );
}

int SP_ProtoBufEncoder :: ensureSpace( int space )
{
	int need = mSize + space;
	if( need > mTotal ) {
		int total = mTotal < 256 ? 256 : mTotal;

		for( ; total < need; ) total = total * 2;

		char * tmp = (char*)realloc( mBuffer, total );

		if( NULL == tmp ) return -1;

		mBuffer = tmp;
		mTotal = total;
	}

	return 0;
}

int SP_ProtoBufEncoder :: addVarint( int fieldNumber, uint64_t value )
{
	ensureSpace( sizeof( uint64_t ) * 4 );

	uint64_t key = ( fieldNumber << 3 ) | SP_ProtoBufDecoder::eWireVarint;

	mSize += SP_ProtoBufCodecUtils::encodeVarint( key, mBuffer + mSize );
	mSize += SP_ProtoBufCodecUtils::encodeVarint( value, mBuffer + mSize );

	return 0;
}

int SP_ProtoBufEncoder :: addZigZagInt( int fieldNumber, int64_t value )
{
	uint64_t tmpVal = ( value << 1 ) ^ ( value >> 63 );

	return addVarint( fieldNumber, tmpVal );
}

int SP_ProtoBufEncoder :: addDouble( int fieldNumber, double value )
{
	uint64_t tmp;
	memcpy( &tmp, &value, sizeof( tmp ) );

	return addVarint( fieldNumber, tmp );
}

int SP_ProtoBufEncoder :: addFloat( int fieldNumber, float value )
{
	uint32_t tmp;
	memcpy( &tmp, &value, sizeof( tmp ) );

	return addVarint( fieldNumber, tmp );
}

int SP_ProtoBufEncoder :: add64Bit( int fieldNumber, uint64_t value )
{
	ensureSpace( sizeof( uint64_t ) * 3 );

	uint64_t key = ( fieldNumber << 3 ) | SP_ProtoBufDecoder::eWire64Bit;

	mSize += SP_ProtoBufCodecUtils::encodeVarint( key, mBuffer + mSize );
	mSize += SP_ProtoBufCodecUtils::encode64Bit( value, mBuffer + mSize );

	return 0;
}

int SP_ProtoBufEncoder :: addBinary( int fieldNumber, const char * buffer, int len )
{
	ensureSpace( sizeof( uint64_t ) * 4 + len );

	uint64_t key = ( fieldNumber << 3 ) | SP_ProtoBufDecoder::eWireBinary;

	mSize += SP_ProtoBufCodecUtils::encodeVarint( key, mBuffer + mSize );
	mSize += SP_ProtoBufCodecUtils::encodeVarint( len, mBuffer + mSize );

	memcpy( mBuffer + mSize, buffer, len );
	mSize += len;

	return 0;
}

int SP_ProtoBufEncoder :: add32Bit( int fieldNumber, uint32_t value )
{
	ensureSpace( sizeof( uint64_t ) * 3 );

	uint64_t key = ( fieldNumber << 3 ) | SP_ProtoBufDecoder::eWire32Bit;

	mSize += SP_ProtoBufCodecUtils::encodeVarint( key, mBuffer + mSize );
	mSize += SP_ProtoBufCodecUtils::encode32Bit( value, mBuffer + mSize );

	return 0;
}

int SP_ProtoBufEncoder :: addPacked( int fieldNumber, uint16_t * array, int size )
{
	char * buffer = (char*)malloc( sizeof( uint16_t ) * 2 * size );

	char * pos = buffer;
	for( int i = 0; i < size; i++ ) {
		int ret = SP_ProtoBufCodecUtils::encodeVarint( array[i], pos );
		pos += ret;
	}

	addBinary( fieldNumber, buffer, pos - buffer );

	free( buffer );

	return 0;
}

int SP_ProtoBufEncoder :: addPacked( int fieldNumber, uint32_t * array, int size )
{
	char * buffer = (char*)malloc( sizeof( uint32_t ) * 2 * size );

	char * pos = buffer;
	for( int i = 0; i < size; i++ ) {
		int ret = SP_ProtoBufCodecUtils::encodeVarint( array[i], pos );
		pos += ret;
	}

	addBinary( fieldNumber, buffer, pos - buffer );

	free( buffer );

	return 0;
}

int SP_ProtoBufEncoder :: addPacked( int fieldNumber, uint64_t * array, int size )
{
	char * buffer = (char*)malloc( sizeof( uint64_t ) * 2 * size );

	char * pos = buffer;
	for( int i = 0; i < size; i++ ) {
		int ret = SP_ProtoBufCodecUtils::encodeVarint( array[i], pos );
		pos += ret;
	}

	addBinary( fieldNumber, buffer, pos - buffer );

	free( buffer );

	return 0;
}

int SP_ProtoBufEncoder :: addPacked( int fieldNumber, float * array, int size )
{
	char * buffer = (char*)malloc( sizeof( float ) * 2 * size );

	char * pos = buffer;
	for( int i = 0; i < size; i++ ) {
		uint32_t tmp;
		memcpy( &tmp, array + i, sizeof( tmp ) );

		int ret = SP_ProtoBufCodecUtils::encodeVarint( tmp, pos );
		pos += ret;
	}

	addBinary( fieldNumber, buffer, pos - buffer );

	free( buffer );

	return 0;
}

int SP_ProtoBufEncoder :: addPacked( int fieldNumber, double * array, int size )
{
	char * buffer = (char*)malloc( sizeof( double ) * 2 * size );

	char * pos = buffer;
	for( int i = 0; i < size; i++ ) {
		uint64_t tmp;
		memcpy( &tmp, array + i, sizeof( tmp ) );

		int ret = SP_ProtoBufCodecUtils::encodeVarint( tmp, pos );
		pos += ret;
	}

	addBinary( fieldNumber, buffer, pos - buffer );

	free( buffer );

	return 0;
}

const char * SP_ProtoBufEncoder :: getBuffer()
{
	return mBuffer;
}

int SP_ProtoBufEncoder :: getSize()
{
	return mSize;
}

//=========================================================

SP_ProtoBufDecoder :: SP_ProtoBufDecoder( const char * buffer, int len )
{
	mBuffer = buffer;
	mEnd = mBuffer + len;

	mCurr = mBuffer;

	mFieldList = NULL;
}

SP_ProtoBufDecoder :: ~SP_ProtoBufDecoder()
{
	mBuffer = NULL;
	mEnd = NULL;
	mCurr = NULL;

	if( NULL != mFieldList ) delete mFieldList;
	mFieldList = NULL;
}

bool SP_ProtoBufDecoder :: getNext( KeyValPair_t * pair )
{
	if( mCurr >= mEnd ) return false;

	int ret = getPair( mCurr, pair );

	if( ret > 0 ) mCurr += ret;

	return ret > 0;
}

int SP_ProtoBufDecoder :: getPair( const char * buffer, KeyValPair_t * pair )
{
	int ret = 0;

	const char * curr = buffer;
	memset( pair, 0, sizeof( KeyValPair_t ) );

	uint64_t tmpVal = 0;
	int pos = SP_ProtoBufCodecUtils::decodeVarint( &tmpVal, curr );

	curr += pos;

	pair->mFieldNumber = tmpVal >> 3;
	pair->mWireType = tmpVal & 0x07;

	switch( pair->mWireType )
	{
		case eWireVarint:
			pos = SP_ProtoBufCodecUtils::decodeVarint( &tmpVal, curr );
			pair->mVarint.u = tmpVal;
			pair->mZigZagInt = ( (tmpVal >> 1) ^ - (int64_t)(tmpVal & 1) );
			curr += pos;
			break;

		case eWire64Bit:
			pair->m64Bit.u = SP_ProtoBufCodecUtils::decode32Bit( curr );
			curr += 4;
			pair->m64Bit.u |= (uint64_t)SP_ProtoBufCodecUtils::decode32Bit( curr ) << 32;
			curr += 4;
			break;

		case eWireBinary:
			pos = SP_ProtoBufCodecUtils::decodeVarint( &tmpVal, curr );
			curr += pos;

			pair->mBinary.mLen = tmpVal;
			pair->mBinary.mBuffer = curr ;
			curr += tmpVal;
			break;

		case eWire32Bit:
			pair->m32Bit.u = SP_ProtoBufCodecUtils::decode32Bit( curr );
			curr += 4;
			break;

		default:
			ret = -1;
			break;
	}

	return 0 == ret ? ( curr - buffer ) : -1;
}

void SP_ProtoBufDecoder :: initFieldList()
{
	if( NULL == mFieldList ) {
		mFieldList = new SP_ProtoBufFieldList();

		KeyValPair_t pair;

		const char * curr = mBuffer;

		for( ; curr < mEnd; ) {
			int ret = getPair( curr, &pair );

			if( ret < 0 ) break;

			mFieldList->addFieldOffset( pair.mFieldNumber, curr - mBuffer );

			curr += ret;
		}
	}
}

bool SP_ProtoBufDecoder :: find( int fieldNumber, KeyValPair_t * pair, int index )
{
	bool isExist = false;

	initFieldList();

	SP_ProtoBufFieldList::Field_t * field = mFieldList->findField( fieldNumber );

	if( NULL != field ) {
		int count = 1, offset = -1;

		if( field->mIsRepeated ) {
			count = field->mList->mCount;

			if( index >= 0 && index < field->mList->mCount ) {
				offset = field->mList->mList[ index ];
			}
		} else {
			offset = field->mOffset;
		}

		if( offset >= 0 && ( mBuffer + offset ) < mEnd ) {
			getPair( mBuffer + offset, pair );
			isExist = true;
		}

		pair->mRepeatedCount = count;
	}

	return isExist;
}

void SP_ProtoBufDecoder :: rewind()
{
	mCurr = mBuffer;
}

//=========================================================

char * SP_ProtoBufCodecUtils :: dup( const char * buffer, int len )
{
	char * ret = (char*)malloc( len + 1 );
	memcpy( ret, buffer, len );
	ret[ len ] = '\0';

	return ret;
}

int SP_ProtoBufCodecUtils :: getPacked( const char * buffer, int len, uint16_t * array, int size )
{
	int count = 0;

	const char * pos = buffer;

	for( int i = 0; i < size && ( pos - buffer ) < len; i++ ) {
		uint64_t tmp;

		int ret = decodeVarint( &tmp, pos );
		pos += ret;

		array[i] = tmp;

		count++;
	}

	return count;
}

int SP_ProtoBufCodecUtils :: getPacked( const char * buffer, int len, uint32_t * array, int size )
{
	int count = 0;

	const char * pos = buffer;

	for( int i = 0; i < size && ( pos - buffer ) < len; i++ ) {
		uint64_t tmp;

		int ret = decodeVarint( &tmp, pos );
		pos += ret;

		array[i] = tmp;

		count++;
	}

	return count;
}

int SP_ProtoBufCodecUtils :: getPacked( const char * buffer, int len, uint64_t * array, int size )
{
	int count = 0;

	const char * pos = buffer;

	for( int i = 0; i < size && ( pos - buffer ) < len; i++ ) {
		uint64_t tmp;

		int ret = decodeVarint( &tmp, pos );
		pos += ret;

		array[i] = tmp;

		count++;
	}

	return count;
}

int SP_ProtoBufCodecUtils :: getPacked( const char * buffer, int len, float * array, int size )
{
	int count = 0;

	const char * pos = buffer;

	for( int i = 0; i < size && ( pos - buffer ) < len; i++ ) {
		uint64_t tmp;

		int ret = decodeVarint( &tmp, pos );
		pos += ret;

		uint32_t tmp32 = tmp;
		memcpy( array + i, &tmp32, sizeof( tmp32 ) );

		count++;
	}

	return count;
}

int SP_ProtoBufCodecUtils :: getPacked( const char * buffer, int len, double * array, int size )
{
	int count = 0;

	const char * pos = buffer;

	for( int i = 0; i < size && ( pos - buffer ) < len; i++ ) {
		uint64_t tmp;

		int ret = decodeVarint( &tmp, pos );
		pos += ret;

		memcpy( array + i, &tmp, sizeof( tmp ) );

		count++;
	}

	return count;
}

int SP_ProtoBufCodecUtils :: decodeVarint( uint64_t *value, const char *buffer )
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

uint32_t SP_ProtoBufCodecUtils :: decode32Bit( const char * buffer )
{
	return buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);
}

int SP_ProtoBufCodecUtils :: encodeVarint( uint64_t value, char *buffer )
{
	int pos = 0;
	while (value >= 128) {
		buffer[pos++] = 128 + (value & 127);
		value >>= 7;
	}
	buffer[pos++] = value;
	return pos;
}

int SP_ProtoBufCodecUtils :: encode32Bit( uint32_t value, char * buffer )
{
	buffer[ 0 ] = value & 0xFF;
	buffer[ 1 ] = ( value >> 8 ) & 0xFF;
	buffer[ 2 ] = ( value >> 16 ) & 0xFF;
	buffer[ 3 ] = ( value >> 24 ) & 0xFF;

	return 4;
}

int SP_ProtoBufCodecUtils :: encode64Bit( uint64_t value, char * buffer )
{
	uint32_t tmp = (uint32_t)value;

	encode32Bit( tmp, buffer );

	tmp = (uint32_t)( value >> 32 );

	encode32Bit( tmp, buffer + 4 );

	return 8;
}

