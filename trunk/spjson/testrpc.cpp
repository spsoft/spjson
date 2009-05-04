/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <string.h>

#include "spjsonrpc.hpp"
#include "spjsonutils.hpp"

void testReq()
{
	const char * req = "{\"jsonrpc\": \"2.0\", "
			"\"method\": \"subtract\", \"params\": [42, 23], \"id\": 1}";

	SP_JsonRpcReqObject reqObject( req, strlen( req ) );

	SP_JsonStringBuffer buffer;

	SP_JsonRpcUtils::toReqBuffer( "subtract", "1", reqObject.getParams(), &buffer );

	printf( "req: %s\n", buffer.getBuffer() );
}

void testResp()
{
	const char * resp = "{\"jsonrpc\": \"2.0\", "
			"\"error\": {\"code\": -32601, \"message\": \"Procedure not found.\"}, \"id\": 10}";

	SP_JsonRpcRespObject respObject( resp, strlen( resp ) );

	SP_JsonStringBuffer buffer;

	SP_JsonRpcUtils::toRespBuffer( respObject.getID(), respObject.getResult(),
			respObject.getError(), &buffer );

	printf( "resp: %s\n", buffer.getBuffer() );
}

int main( int argc, char * argv[] )
{
	testReq();

	testResp();

	return 0;
}

