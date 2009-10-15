/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __sppbtree_hpp__
#define __sppbtree_hpp__

#include "sppbcodec.hpp"

class SP_ProtoBufDecoder;
class SP_ProtoBufFieldList;
class SP_JsonArrayList;

class SP_ProtoBufTree {
public:
	SP_ProtoBufTree();
	~SP_ProtoBufTree();

	/**
	 * @brief attach the buffer to the decoder
	 * 
	 * @note  1. The buffer will been modified by the decoder
	 *        2. It is the caller's responsibility to free the buffer
	 */
	int attach( char * buffer, int len );

	int copyFrom( const char * buffer, int len );

	SP_ProtoBufDecoder * getDecoder();

	SP_ProtoBufTree * findChild( int fieldNumber, int index = 0 );

private:
	SP_ProtoBufDecoder * mDecoder;
	SP_JsonArrayList * mChildren;
};

#endif

