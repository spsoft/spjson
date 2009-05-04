/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spjsoncodec_hpp__
#define __spjsoncodec_hpp__

class SP_JsonStringBuffer;

class SP_JsonCodec {
public:
	static int decode( const char * encodeValue, SP_JsonStringBuffer * outBuffer );

	static int encode( const char * decodeValue, SP_JsonStringBuffer * outBuffer );

	static int encode( double value, SP_JsonStringBuffer * outBuffer );

	static int encode( double value, char * buffer, int len );

	static const char ESCAPE_CHARS [];
	static const char UNESCAPED_CHARS [];

private:
	SP_JsonCodec();
};

#endif

