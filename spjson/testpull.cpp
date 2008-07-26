/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include "spjsonparser.hpp"
#include "spjsonevent.hpp"
#include "spjsonutils.hpp"

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

	SP_JsonPullParser parser;
	parser.append( source, strlen( source ) );

	free( source );

	for( SP_JsonPullEvent * event = parser.getNext();
			NULL != event;
			event = parser.getNext() ) {

		switch( event->getEventType() ) {
			case SP_JsonPullEvent::eStartObject:
				printf( "{\n" );
				break;
			case SP_JsonPullEvent::eEndObject:
				printf( "},\n" );
				break;
			case SP_JsonPullEvent::eStartArray:
				printf( "[" );
				break;
			case SP_JsonPullEvent::eEndArray:
				printf( "],\n" );
				break;
			case SP_JsonPullEvent::eNull:
				printf( "null,\n" );
				break;
			case SP_JsonPullEvent::eName:
				printf( "\"%s\" : ", ((SP_JsonTextEvent*)event)->getText() );
				break;
			case SP_JsonPullEvent::eString:
				printf( "\"%s\",\n", ((SP_JsonTextEvent*)event)->getText() );
				break;
			case SP_JsonPullEvent::eNumber:
				printf( "%s,\n", ((SP_JsonTextEvent*)event)->getText() );
				break;
			case SP_JsonPullEvent::eBoolean:
				printf( "%s,\n", ((SP_JsonTextEvent*)event)->getText() );
				break;
			case SP_JsonPullEvent::eComment:
				printf( "//%s\n", ((SP_JsonTextEvent*)event)->getText() );
				break;
			case SP_JsonPullEvent::ePadding:
				printf( "%s\n", ((SP_JsonTextEvent*)event)->getText() );
				break;
		}

		delete event;
	}

	if( NULL != parser.getError() ) {
		printf( "\n\nerror: %s\n", parser.getError() );
	}

	return 0;
}

