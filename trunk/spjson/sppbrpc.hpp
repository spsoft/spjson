/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __sppbrpc_hpp__
#define __sppbrpc_hpp__

#include "sppbcodec.hpp"

class SP_ProtoBufTree;

class SP_ProtoBufRpcReqObject {
public:

	enum {
		eMethod = 1,
		eParams = 2,
		eID = 3,
		eVersion = 4
	};

public:
	SP_ProtoBufRpcReqObject();
	~SP_ProtoBufRpcReqObject();

	/**
	 * @brief attach the buffer to the decoder
	 * 
	 * @note  1. The buffer will been modified by the decoder
	 *        2. It is the caller's responsibility to free the buffer
	 */
	int attach( char * buffer, int len );

	int copyFrom( const char * buffer, int len );

	const char * getVersion() const;
	const char * getMethod() const;

	bool getParams( SP_ProtoBufDecoder::KeyValPair_t * pair ) const;
	bool getID( SP_ProtoBufDecoder::KeyValPair_t * pair ) const;

	SP_ProtoBufTree * getParams() const;

	const char * getStrID() const;

	const char * getPacketError() const;

private:
	SP_ProtoBufTree * mTree;

	char mStrID[ 32 ];

	const char * mPacketError;
};

class SP_ProtoBufRpcRespObject {
public:

	enum {
		eResult = 1,
		eError = 2,
		eID = 3,
		eVersion = 4,

		eErrorCode = 1,
		eErrorMsg = 2,
		eErrorData = 3
	};

public:
	SP_ProtoBufRpcRespObject();
	~SP_ProtoBufRpcRespObject();

	/**
	 * @brief attach the buffer to the decoder
	 * 
	 * @note  1. The buffer will been modified by the decoder
	 *        2. It is the caller's responsibility to free the buffer
	 */
	int attach( char * buffer, int len );

	int copyFrom( const char * buffer, int len );

	const char * getVersion() const;
	bool getResult( SP_ProtoBufDecoder::KeyValPair_t * pair ) const;
	bool getID( SP_ProtoBufDecoder::KeyValPair_t * pair ) const;

	SP_ProtoBufTree * getResult() const;

	SP_ProtoBufTree * getError() const;

	int getErrorCode() const;

	const char * getErrorMsg() const;

	const char * getStrID() const;

	const char * getPacketError() const;

private:

	SP_ProtoBufTree * mTree;

	char mStrID[ 32 ];

	const char * mPacketError;
};

class SP_ProtoBufRpcUtils {
public:
	static int initReqEncoder( SP_ProtoBufEncoder * reqEncoder,
			const char * method, const char * id );

	static int initRespEncoder( SP_ProtoBufEncoder * respEncoder,
			SP_ProtoBufDecoder::KeyValPair_t * id, SP_ProtoBufEncoder * error );

	static int initRespEncoder( SP_ProtoBufEncoder * respEncoder,
			const char * id, SP_ProtoBufEncoder * error );

	static int setError( SP_ProtoBufEncoder * error , int code, const char * msg );

private:
	SP_ProtoBufRpcUtils();
};

#endif

