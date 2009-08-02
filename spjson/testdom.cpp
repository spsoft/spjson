/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "spjsondomparser.hpp"
#include "spjsonnode.hpp"
#include "spjsonutils.hpp"
#include "spjsoncodec.hpp"
#include "spjsoniterator.hpp"

static void postPrint( const SP_JsonNode * node )
{
	if( node->isLastChild() ) {
		const SP_JsonNode * parent = node->getParent();
		if( SP_JsonNode::ePair == parent->getType() ) {
			parent = parent->getParent();
		}

		if( SP_JsonNode::eObject == parent->getType() ) {
			printf( "}" );
		} else {
			assert( SP_JsonNode::eArray == parent->getType() );
			printf( "]" );
		}

		postPrint( parent );
	} else {
		if( NULL != node->getParent() ) printf( "," );
	}
}

int main( int argc, char * argv[] )
{
	char * filename = NULL;

	if( argc < 2 ) {
		printf( "Usage: %s <json_file>\n", argv[0] );
		exit( -1 );
	} else {
		filename = argv[1];
	}

	FILE * fp = fopen ( filename, "r" );
	if( NULL == fp ) {
		printf( "cannot not open %s\n", filename );
		exit( -1 );
	}

	struct stat aStat;
	char * source = NULL;
	stat( filename, &aStat );
	source = ( char * ) malloc ( aStat.st_size + 1 );
	fread ( source, aStat.st_size, sizeof ( char ), fp );
	fclose ( fp );
	source[ aStat.st_size ] = '\0';

	SP_JsonDomParser parser;
	parser.append( source, strlen( source ) );

	if( NULL != parser.getError() ) {
		printf( "\n\nerror: %s\n", parser.getError() );
	}

	free( source );

	printf( "Test DomBuffer\n" );

	SP_JsonDomBuffer buffer( parser.getValue(), 1 );

	printf( "%s\n", buffer.getBuffer() );

	printf( "\n" );

	printf( "Test Iterator\n" );

	SP_JsonIterator iterator( parser.getValue() );

	for( ; ; ) {
		const SP_JsonNode * node = iterator.getNext();
		if( NULL == node ) break;

		switch( node->getType() ) {
			case SP_JsonNode::eObject:
				printf( "{" );
				break;
			case SP_JsonNode::eArray:
				printf( "[" );
				break;
			case SP_JsonNode::ePair:
			{
				SP_JsonStringBuffer buffer;
				SP_JsonCodec::encode( ((SP_JsonPairNode*)node)->getName(), &buffer );
				printf( "\"%s\" : ", buffer.getBuffer() );
				break;
			}
			case SP_JsonNode::eString:
			{
				SP_JsonStringBuffer buffer;
				SP_JsonCodec::encode( ((SP_JsonStringNode*)node)->getValue(), &buffer );
				printf( "\"%s\"", buffer.getBuffer() );
				postPrint( node );
				break;
			}
			case SP_JsonNode::eNull:
				printf( "null" );
				postPrint( node );
				break;
			case SP_JsonNode::eDouble:
				printf( "%lf", ((SP_JsonDoubleNode*)node)->getValue() );
				postPrint( node );
				break;
			case SP_JsonNode::eInt:
				printf( "%lld", ((SP_JsonIntNode*)node)->getValue() );
				postPrint( node );
				break;
			case SP_JsonNode::eBoolean:
				printf( "%s", ((SP_JsonBooleanNode*)node)->getValue() ? "true" : "false" );
				postPrint( node );
				break;
			case SP_JsonNode::eComment:
				printf( "//%s\n", ((SP_JsonCommentNode*)node)->getValue() );
				break;
			default:
				printf( "unknown\n" );
				break;
		}


	}

	printf( "\n" );

	return 0;
}

