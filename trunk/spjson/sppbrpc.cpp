/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <string.h>
#include <assert.h>

#include "sppbrpc.hpp"
#include "sppbtree.hpp"

SP_ProtoBufRpcReqObject :: SP_ProtoBufRpcReqObject()
{
	mPacketError = NULL;
	mStrID[ 0 ] = '\0';

	mTree = NULL;
}

SP_ProtoBufRpcReqObject :: ~SP_ProtoBufRpcReqObject()
{
	if( NULL != mTree ) delete mTree, mTree = NULL;
}

int SP_ProtoBufRpcReqObject :: attach( char * buffer, int len )
{
	assert( NULL == mTree );

	mTree = new SP_ProtoBufTree();
	mTree->attach( buffer, len );

	SP_ProtoBufDecoder::KeyValPair_t method, params;

	if( ! mTree->getDecoder()->find( eMethod, &method ) ) {
		mPacketError = "invalid protobuf, not method property";
	} else {
		if( ! mTree->getDecoder()->find( eParams, &params ) ) {
			mPacketError = "invalid protobuf, not params property";
		}
	}

	SP_ProtoBufDecoder::KeyValPair_t id;

	if( mTree->getDecoder()->find( eID, &id ) ) {
		SP_ProtoBufCodecUtils::toString( &id, mStrID, sizeof( mStrID ) );
	}

	return NULL == mPacketError ? 0 : -1;
}

int SP_ProtoBufRpcReqObject :: copyFrom( const char * buffer, int len )
{
	assert( NULL == mTree );

	mTree = new SP_ProtoBufTree();
	mTree->copyFrom( buffer, len );

	SP_ProtoBufDecoder::KeyValPair_t method, params;

	if( ! mTree->getDecoder()->find( eMethod, &method ) ) {
		mPacketError = "invalid protobuf, not method property";
	} else {
		if( ! mTree->getDecoder()->find( eParams, &params ) ) {
			mPacketError = "invalid protobuf, not params property";
		}
	}

	SP_ProtoBufDecoder::KeyValPair_t id;

	if( mTree->getDecoder()->find( eID, &id ) ) {
		SP_ProtoBufCodecUtils::toString( &id, mStrID, sizeof( mStrID ) );
	}

	return NULL == mPacketError ? 0 : -1;
}

const char * SP_ProtoBufRpcReqObject :: getVersion() const
{
	SP_ProtoBufDecoder::KeyValPair_t value;

	if( mTree->getDecoder()->find( eVersion, &value ) ) {
		if( SP_ProtoBufDecoder::eWireBinary == value.mWireType ) {
			return value.mBinary.mBuffer;
		}
	}

	return NULL;
}

const char * SP_ProtoBufRpcReqObject :: getMethod() const
{
	SP_ProtoBufDecoder::KeyValPair_t value;

	if( mTree->getDecoder()->find( eMethod, &value ) ) {
		if( SP_ProtoBufDecoder::eWireBinary == value.mWireType ) {
			return value.mBinary.mBuffer;
		}
	}

	return NULL;
}

bool SP_ProtoBufRpcReqObject :: getParams( SP_ProtoBufDecoder::KeyValPair_t * pair ) const
{
	return mTree->getDecoder()->find( eParams, pair );
}

bool SP_ProtoBufRpcReqObject :: getID( SP_ProtoBufDecoder::KeyValPair_t * pair ) const
{
	return mTree->getDecoder()->find( eID, pair );
}

SP_ProtoBufTree * SP_ProtoBufRpcReqObject :: getParams() const
{
	return mTree->findChild( eParams );
}

const char * SP_ProtoBufRpcReqObject :: getStrID() const
{
	return mStrID;
}

const char * SP_ProtoBufRpcReqObject :: getPacketError() const
{
	return mPacketError;
}

//============================================================================

SP_ProtoBufRpcRespObject :: SP_ProtoBufRpcRespObject()
{
	mPacketError = NULL;
	mStrID[ 0 ] = '\0';

	mTree = NULL;
}

SP_ProtoBufRpcRespObject :: ~SP_ProtoBufRpcRespObject()
{
	delete mTree, mTree = NULL;
}

int SP_ProtoBufRpcRespObject :: attach( char * buffer, int len )
{
	assert( NULL == mTree );

	mTree = new SP_ProtoBufTree();
	mTree->attach( buffer, len );

	SP_ProtoBufDecoder::KeyValPair_t result, error;

	if( ( ! mTree->getDecoder()->find( eResult, &result ) )
			 && ( ! mTree->getDecoder()->find( eError, &error ) ) ) {
		mPacketError = "invalid json stream, not result property";
	}

	SP_ProtoBufDecoder::KeyValPair_t id;

	if( mTree->getDecoder()->find( eID, &id ) ) {
		SP_ProtoBufCodecUtils::toString( &id, mStrID, sizeof( mStrID ) );
	}

	return NULL == mPacketError ? 0 : -1;
}

int SP_ProtoBufRpcRespObject :: copyFrom( const char * buffer, int len )
{
	assert( NULL == mTree );

	mTree = new SP_ProtoBufTree();
	mTree->copyFrom( buffer, len );

	SP_ProtoBufDecoder::KeyValPair_t result, error;

	if( ( ! mTree->getDecoder()->find( eResult, &result ) )
			&& ( ! mTree->getDecoder()->find( eError, &error ) ) ) {
		mPacketError = "invalid json stream, not result property";
	}

	SP_ProtoBufDecoder::KeyValPair_t id;

	if( mTree->getDecoder()->find( eID, &id ) ) {
		SP_ProtoBufCodecUtils::toString( &id, mStrID, sizeof( mStrID ) );
	}

	return NULL == mPacketError ? 0 : -1;
}

const char * SP_ProtoBufRpcRespObject :: getVersion() const
{
	SP_ProtoBufDecoder::KeyValPair_t value;

	if( mTree->getDecoder()->find( eVersion, &value ) ) {
		if( SP_ProtoBufDecoder::eWireBinary == value.mWireType ) {
			return value.mBinary.mBuffer;
		}
	}

	return NULL;
}

bool SP_ProtoBufRpcRespObject :: getResult( SP_ProtoBufDecoder::KeyValPair_t * pair ) const
{
	return mTree->getDecoder()->find( eResult, pair );
}

bool SP_ProtoBufRpcRespObject :: getID( SP_ProtoBufDecoder::KeyValPair_t * pair ) const
{
	return mTree->getDecoder()->find( eID, pair );
}

const char * SP_ProtoBufRpcRespObject :: getStrID() const
{
	return mStrID;
}

SP_ProtoBufTree * SP_ProtoBufRpcRespObject :: getResult() const
{
	return mTree->findChild( eResult );
}

SP_ProtoBufTree * SP_ProtoBufRpcRespObject :: getError() const
{
	return mTree->findChild( eError );
}

int SP_ProtoBufRpcRespObject :: getErrorCode() const
{
	SP_ProtoBufDecoder::KeyValPair_t pair;

	SP_ProtoBufTree * tree = mTree->findChild( eError );

	if( NULL != tree ) {
		if( tree->getDecoder()->find( eErrorCode, &pair ) ) {
			return pair.m32Bit.s;
		}
	}

	return 0;
}

const char * SP_ProtoBufRpcRespObject :: getErrorMsg() const
{
	SP_ProtoBufDecoder::KeyValPair_t pair;

	SP_ProtoBufTree * tree = mTree->findChild( eError );

	if( NULL != tree ) {
		if( tree->getDecoder()->find( eErrorMsg, &pair ) ) {
			return pair.mBinary.mBuffer;
		}
	}

	return NULL;
}

const char * SP_ProtoBufRpcRespObject :: getPacketError() const
{
	return mPacketError;
}

//============================================================================

int SP_ProtoBufRpcUtils :: initReqEncoder( SP_ProtoBufEncoder * reqEncoder,
			const char * method, const char * id )
{
	reqEncoder->addString( SP_ProtoBufRpcReqObject::eMethod, method );
	reqEncoder->addString( SP_ProtoBufRpcReqObject::eID, id );

	return 0;
}

int SP_ProtoBufRpcUtils :: initRespEncoder( SP_ProtoBufEncoder * respEncoder,
			SP_ProtoBufDecoder::KeyValPair_t * id, SP_ProtoBufEncoder * error )
{
	SP_ProtoBufCodecUtils::addField( respEncoder, SP_ProtoBufRpcRespObject::eID, id );
	if( NULL != error ) {
		respEncoder->addBinary( SP_ProtoBufRpcRespObject::eError,
				error->getBuffer(), error->getSize() );
	}

	return 0;
}

int SP_ProtoBufRpcUtils :: initRespEncoder( SP_ProtoBufEncoder * respEncoder,
			const char * id, SP_ProtoBufEncoder * error )
{
	respEncoder->addString( SP_ProtoBufRpcRespObject::eID, id );
	if( NULL != error ) {
		respEncoder->addBinary( SP_ProtoBufRpcRespObject::eError,
				error->getBuffer(), error->getSize() );
	}

	return 0;
}

int SP_ProtoBufRpcUtils :: setError( SP_ProtoBufEncoder * error ,
		int code, const char * msg )
{
	error->add32Bit( SP_ProtoBufRpcRespObject::eErrorCode, code );
	error->addString( SP_ProtoBufRpcRespObject::eErrorMsg, msg );

	return 0;
}

