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
#include "spjsonhandle.hpp"

int main( int argc, char * argv[] )
{
	const char * source = "{ \"popup\": {"
		"\"menuitem\": ["
			"{\"value\": \"New\", \"onclick\": \"CreateNewDoc()\"},"
			"{\"value\": \"Open\", \"onclick\": \"OpenDoc()\"},"
			"{\"value\": \"Close\", \"onclick\": \"CloseDoc()\"}"
		"] } }";

	SP_JsonDomParser parser;
	parser.append( source, strlen( source ) );

	if( NULL != parser.getError() ) {
		printf( "\n\nerror: %s\n", parser.getError() );
	}

	SP_JsonHandle rootHandle( parser.getValue() );

	const SP_JsonStringNode * onclick = rootHandle.getChild( "popup" ).getChild( "menuitem" )
			.getChild( 2 ).getChild( "onclick" ).toString();

	if( NULL != onclick ) {
		printf( "popup/menuitem[2]/onclick is \"%s\"\n", onclick->getValue() );
	} else {
		printf( "Cannot found popup/menuitem[2]/onclick\n" );
	}

	const SP_JsonArrayNode * menuitem = rootHandle.getChild( "popup" )
			.getChild( "menuitem" ).toArray();

	if( NULL != menuitem ) {
		SP_JsonDomBuffer buffer( menuitem );

		printf( "menuitem is\n%s\n", buffer.getBuffer() );
	} else {
		printf( "Cannot found popup/menuitem\n" );
	}

	printf( "\n" );

	return 0;
}

