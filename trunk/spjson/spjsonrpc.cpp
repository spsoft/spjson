/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>

#include "spjsonrpc.hpp"

#include "spjsonnode.hpp"
#include "spjsondomparser.hpp"
#include "spjsonhandle.hpp"
#include "spjsonutils.hpp"
#include "spjsoncodec.hpp"

SP_JsonRpcReqObject :: SP_JsonRpcReqObject( const char * buffer, int len )
{
	mPacketError = NULL;
	mStrID[ 0 ] = '\0';

	mParser = new SP_JsonDomParser();
	mParser->append( buffer, len );

	SP_JsonHandle handle( mParser->getValue() );

	mVersion = handle.getChild( "jsonrpc" ).toString();

	mMethod = handle.getChild( "method" ).toString();

	mParams = handle.getChild( "params" ).toArray();

	if( NULL == mMethod ) {
		mPacketError = "invalid json stream, not method property";
	} else {
		if( NULL == mParams ) {
			mPacketError = "invalid json stream, not params property";
		}
	}

	mID = handle.getChild( "id" ).toNode();

	if( NULL != mID ) {
		SP_JsonDomBuffer::dump( mID, mStrID, sizeof( mStrID ) );
	}
}

SP_JsonRpcReqObject :: ~SP_JsonRpcReqObject()
{
	delete mParser, mParser = NULL;
}

const char * SP_JsonRpcReqObject :: getVersion() const
{
	return mVersion ? mVersion->getValue() : NULL;
}

const char * SP_JsonRpcReqObject :: getMethod() const
{
	return mMethod ? mMethod->getValue() : NULL;
}

const SP_JsonArrayNode * SP_JsonRpcReqObject :: getParams() const
{
	return mParams;
}

const SP_JsonNode * SP_JsonRpcReqObject :: getID() const
{
	return mID;
}

const char * SP_JsonRpcReqObject :: getStrID() const
{
	return mStrID;
}

const char * SP_JsonRpcReqObject :: getPacketError() const
{
	return mParser->getError() ? mParser->getError() : mPacketError;
}

//============================================================================

SP_JsonRpcRespObject :: SP_JsonRpcRespObject( const char * buffer, int len )
{
	mPacketError = NULL;
	mStrID[ 0 ] = '\0';

	mParser = new SP_JsonDomParser();
	mParser->append( buffer, len );

	SP_JsonHandle handle( mParser->getValue() );

	mVersion = handle.getChild( "jsonrpc" ).toString();

	mResult = handle.getChild( "result" ).toNode();

	mError = handle.getChild( "error" ).toObject();

	if( NULL == mResult && NULL == mError ) {
		mPacketError = "invalid json stream, not result property";
	}

	mID = handle.getChild( "id" ).toNode();

	if( NULL != mID ) {
		SP_JsonDomBuffer::dump( mID, mStrID, sizeof( mStrID ) );
	}
}

SP_JsonRpcRespObject :: ~SP_JsonRpcRespObject()
{
	delete mParser, mParser = NULL;

	mResult = NULL;
	mError = NULL;
}

const char * SP_JsonRpcRespObject :: getVersion() const
{
	return mVersion ? mVersion->getValue() : NULL;
}

SP_JsonNode * SP_JsonRpcRespObject :: getResult() const
{
	return mResult;
}

SP_JsonNode * SP_JsonRpcRespObject :: getID() const
{
	return mID;
}

const char * SP_JsonRpcRespObject :: getStrID() const
{
	return mStrID;
}

SP_JsonObjectNode * SP_JsonRpcRespObject :: getError() const
{
	return mError;
}

const char * SP_JsonRpcRespObject :: getPacketError() const
{
	return mParser->getError() ? mParser->getError() : mPacketError;
}

//============================================================================

int SP_JsonRpcUtils :: toReqBuffer( const char * method, const char * id,
			const SP_JsonArrayNode * params, SP_JsonStringBuffer * buffer )
{
	buffer->append( "{ \"method\": \"" );
	SP_JsonCodec::encode( method, buffer );
	buffer->append( "\", \"params\": " );

	SP_JsonDomBuffer::dumpArray( params, buffer, -1 );

	buffer->append( ", \"id\": \"" );

	SP_JsonCodec::encode( id, buffer );

	buffer->append( "\" }" );

	return 0;
}

int SP_JsonRpcUtils :: toRespBuffer( const SP_JsonNode * id, const SP_JsonNode * result,
			const SP_JsonObjectNode * error, SP_JsonStringBuffer * buffer )
{
	buffer->append( "{" );
	if( NULL != result ) {
		buffer->append( "\"result\": " );
		SP_JsonDomBuffer::dump( result, buffer, -1 );
		buffer->append( ", " );
	} else {
		if( NULL != error ) {
			buffer->append( "\"error\": " );
			SP_JsonDomBuffer::dumpObject( error, buffer, -1 );
			buffer->append( ", " );
		}
	}

	if( NULL != id ) {
		buffer->append( "\"id\": " );
		SP_JsonDomBuffer::dump( id, buffer, -1 );
	}

	buffer->append( " }" );

	return 0;
}

int SP_JsonRpcUtils :: setError( SP_JsonObjectNode * error, int code, const char * msg )
{
	SP_JsonPairNode * codePair = new SP_JsonPairNode();
	codePair->setName( "code" );
	codePair->setValue( new SP_JsonIntNode( code ) );

	error->addValue( codePair );

	SP_JsonPairNode * msgPair = new SP_JsonPairNode();
	msgPair->setName( "message" );
	msgPair->setValue( new SP_JsonStringNode( msg ) );

	error->addValue( msgPair );

	return 0;
}

