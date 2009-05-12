/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spjsonrpc_hpp__
#define __spjsonrpc_hpp__

class SP_JsonNode;
class SP_JsonArrayNode;
class SP_JsonObjectNode;
class SP_JsonStringNode;
class SP_JsonDomParser;

class SP_JsonStringBuffer;

class SP_JsonRpcReqObject {
public:
	SP_JsonRpcReqObject( const char * buffer, int len );
	~SP_JsonRpcReqObject();

	const char * getVersion() const;
	const char * getMethod() const;
	const SP_JsonArrayNode * getParams() const;
	const SP_JsonNode * getID() const;

	const char * getStrID() const;

	const char * getPacketError() const;

private:
	SP_JsonDomParser * mParser;

	SP_JsonStringNode * mVersion;
	SP_JsonArrayNode * mParams;
	SP_JsonStringNode * mMethod;
	SP_JsonNode * mID;

	char mStrID[ 32 ];

	const char * mPacketError;
};

class SP_JsonRpcRespObject {
public:
	SP_JsonRpcRespObject( const char * buffer, int len );
	~SP_JsonRpcRespObject();

	const char * getVersion() const;
	SP_JsonNode * getResult() const;
	SP_JsonNode * getID() const;

	const char * getStrID() const;

	SP_JsonObjectNode * getError() const;

	const char * getPacketError() const;

private:
	SP_JsonDomParser * mParser;

	SP_JsonStringNode * mVersion;
	SP_JsonNode * mResult;
	SP_JsonObjectNode * mError;
	SP_JsonNode * mID;

	char mStrID[ 32 ];

	const char * mPacketError;
};

class SP_JsonRpcUtils {
public:

	enum {
		eParseError     = -32700,
		eInvalidRequest = -32600,
		eMethodNoFound  = -32601,
		eInvalidParams  = -32602,
		eInternalError  = -32603
	};

public:
	static int toReqBuffer( const char * method, const char * id,
			const SP_JsonArrayNode * params, SP_JsonStringBuffer * buffer );

	static int toRespBuffer( const SP_JsonNode * id, const SP_JsonNode * result,
			const SP_JsonObjectNode * error, SP_JsonStringBuffer * buffer );

	static int setError( SP_JsonObjectNode * error, int code, const char * msg );

private:
	SP_JsonRpcUtils();
};

#endif

