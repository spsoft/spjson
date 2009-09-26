/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spprotobuf_hpp__
#define __spprotobuf_hpp__

#include <stdint.h>

/**
 * a simple reader for protobuf
 */
class SP_ProtoBufReader
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
		union {
			uint64_t mVarint;
			uint64_t m64Bit;
			struct {
				const char * mBuffer;
				int mLen;
			} mBinary;
			uint32_t m32Bit;
		} mValue;
	} KeyValPair_t;

public:

	SP_ProtoBufReader( const char * buffer, int len );
	~SP_ProtoBufReader();

	bool getNext( KeyValPair_t * pair );

	bool find( int fieldNumber, KeyValPair_t * pair );

	void rewind();

private:

	// @return > 0 : parse ok, consume how many bytes, -1 : unknown type
	static int decodeVarint( uint64_t *value, const char *buffer );

	static uint32_t get32Bit( const char * buffer );

	// @return > 0 : parse ok, consume how many bytes, -1 : unknown type
	static int getPair( const char * buffer, KeyValPair_t * pair );

private:
	const char * mBuffer, * mEnd;
	const char * mCurr;
};

#endif

