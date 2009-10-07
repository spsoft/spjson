/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spprotobuf_hpp__
#define __spprotobuf_hpp__

#include <stdint.h>

class SP_ProtoBufEncoder
{
public:
	SP_ProtoBufEncoder( int initLen = 0 );
	~SP_ProtoBufEncoder();

	int addVarint( int fieldNumber, uint64_t value );
	int addZigZagInt( int fieldNumber, int64_t value );

	int addDouble( int fieldNumber, double value );
	int addFloat( int fieldNumber, float value );

	int add64Bit( int fieldNumber, uint64_t value );
	int addBinary( int fieldNumber, const char * buffer, int len );
	int add32Bit( int fieldNumber, uint32_t value );

	int addPacked( int fieldNumber, uint16_t * array, int size );
	int addPacked( int fieldNumber, uint32_t * array, int size );
	int addPacked( int fieldNumber, uint64_t * array, int size );
	int addPacked( int fieldNumber, float * array, int size );
	int addPacked( int fieldNumber, double * array, int size );

	const char * getBuffer();
	int getSize();

	void reset();

private:

	int ensureSpace( int space );

private:
	char * mBuffer;
	int mTotal, mSize;
};

class SP_ProtoBufFieldList;

class SP_ProtoBufDecoder
{
public:

	enum {
		eWireVarint = 0,
		eWire64Bit  = 1,
		eWireBinary = 2,
		eWire32Bit  = 5
	};

	typedef struct tagKeyValPair {
		int mFieldNumber;
		int mWireType;
		int mRepeatedCount;

		union {              // wire type 0
			uint64_t u;
			int64_t  s;
		} mVarint;

		int64_t  mZigZagInt; // wire type 0

		union {              // wire type 1
			uint64_t u;
			int64_t  s;
			double   d;
		} m64Bit;

		struct {             // wire type 2
			const char * mBuffer;
			int mLen;
		} mBinary;

		union {              // wire type 5
			uint32_t u;
			int32_t  s;
			float    f;
		} m32Bit;
	} KeyValPair_t;

public:

	SP_ProtoBufDecoder( const char * buffer, int len );
	~SP_ProtoBufDecoder();

	bool getNext( KeyValPair_t * pair );

	void rewind();

	bool find( int fieldNumber, KeyValPair_t * pair, int index = 0 );

	void initFieldList();

private:

	// @return > 0 : parse ok, consume how many bytes, -1 : unknown type
	static int getPair( const char * buffer, KeyValPair_t * pair );

private:
	const char * mBuffer, * mEnd;
	const char * mCurr;

	SP_ProtoBufFieldList * mFieldList;
};

class SP_ProtoBufCodecUtils
{
public:

	static char * dup( const char * buffer, int len );

	static int getPacked( const char * buffer, int len, uint16_t * array, int size );
	static int getPacked( const char * buffer, int len, uint32_t * array, int size );
	static int getPacked( const char * buffer, int len, uint64_t * array, int size );
	static int getPacked( const char * buffer, int len, float * array, int size );
	static int getPacked( const char * buffer, int len, double * array, int size );

	// @return > 0 : parse ok, consume how many bytes, -1 : unknown type
	static int decodeVarint( uint64_t *value, const char *buffer );

	static uint32_t decode32Bit( const char * buffer );

	static int encodeVarint( uint64_t value, char *buffer );

	static int encode32Bit( uint32_t value, char * buffer );

	static int encode64Bit( uint64_t value, char * buffer );

private:
	SP_ProtoBufCodecUtils();
	~SP_ProtoBufCodecUtils();
};

#endif

