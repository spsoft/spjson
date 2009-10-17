/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>

#include "sppbrpc.hpp"
#include "sppbtree.hpp"

#include "testpbcomm.hpp"

void testReq()
{
	SP_ProtoBufEncoder reqEncoder;

	SP_ProtoBufRpcUtils::initReqEncoder( &reqEncoder, "subtract", "1" );

	{
		SP_ProtoBufEncoder paramsEncoder;

		paramsEncoder.addVarint( 1, 42 );
		paramsEncoder.addVarint( 1, 23 );

		reqEncoder.addBinary( SP_ProtoBufRpcReqObject::eParams,
			paramsEncoder.getBuffer(), paramsEncoder.getSize() );
	}

	printf( "\ntest req\n\n" );
	printAllConst( reqEncoder.getBuffer(), reqEncoder.getSize() );

	SP_ProtoBufRpcReqObject reqObject;
	reqObject.copyFrom( reqEncoder.getBuffer(), reqEncoder.getSize() );

	if( NULL != reqObject.getPacketError() ) {
		printf( "error %s\n", reqObject.getPacketError() );
	} else {
		printf( "method: %s\n", reqObject.getMethod() );
		printf( "id: %s\n", reqObject.getStrID() );

		SP_ProtoBufDecoder::KeyValPair_t pair0, pair1;

		reqObject.getParams()->getDecoder()->find( 1, &pair0, 0 );
		reqObject.getParams()->getDecoder()->find( 1, &pair1, 1 );

		printf( "param#0: %lld\n", pair0.mVarint.u );
		printf( "param#1: %lld\n", pair1.mVarint.u );
	}
}

void testResp()
{
	SP_ProtoBufEncoder respEncoder;

	SP_ProtoBufEncoder error;
	SP_ProtoBufRpcUtils::setError( &error,  -32601, "Procedure not found." );

	SP_ProtoBufRpcUtils::initRespEncoder( &respEncoder, "1", &error );

	printf( "\ntest resp\n\n" );
	printAllConst( respEncoder.getBuffer(), respEncoder.getSize() );

	SP_ProtoBufRpcRespObject respObject;
	respObject.copyFrom( respEncoder.getBuffer(), respEncoder.getSize() );

	if( NULL != respObject.getPacketError() ) {
		printf( "error %s\n", respObject.getPacketError() );
	} else {
		printf( "id: %s\n", respObject.getStrID() );
		printf( "error.code: %d\n", respObject.getErrorCode() );
		printf( "error.msg: %s\n", respObject.getErrorMsg() );
	}
}

int main( int argc, char * argv[] )
{
	testReq();

	testResp();

	return 0;
}

