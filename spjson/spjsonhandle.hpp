/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spjsonhandle_hpp__
#define __spjsonhandle_hpp__

class SP_JsonNode;
class SP_JsonObjectNode;
class SP_JsonArrayNode;
class SP_JsonPairNode;
class SP_JsonStringNode;
class SP_JsonDoubleNode;
class SP_JsonIntNode;
class SP_JsonBooleanNode;
class SP_JsonNullNode;
class SP_JsonCommentNode;


/**
 *  This class is a clone of TinyXML's TiXmlHandle class.
 *
 *
 *  A SP_JsonHandle is a class that wraps a node pointer with null checks; this is
 *  an incredibly useful thing. Note that SP_JsonHandle is not part of the SPJson
 *  DOM structure. It is a separate utility class.
 *

    Take an example:

{ "popup": {
    "menuitem": [
      {"value": "New", "onclick": "CreateNewDoc()"},
      {"value": "Open", "onclick": "OpenDoc()"},
      {"value": "Close", "onclick": "CloseDoc()"}
    ]
  }
}

    Assuming you want the value of "popup/menuitem[2]/onclick", it's very
    easy to write a *lot* of code that looks like: 

    @verbatim
    const SP_JsonNode * root = domParser.getValue();
    if( root ) {
        const SP_JsonObjectNode * popup = ......
        if( popup ) {
            const SP_JsonArrayNode * menuitem = ......
            if( menuitem ) {
                const SP_JsonStringNode * onclick = ......
                if( onclick ) {
                    ......

    @endverbatim

    And that doesn't even cover "else" cases. SP_JsonHandle addresses the verbosity
    of such code. A SP_JsonHandle checks for null pointers so it is perfectly safe
    and correct to use:

    @verbatim
    SP_JsonHandle rootHandle( domParser.getValue() );
    const SP_JsonStringNode * onclick = rootHandle.getChild( "popup" ).getChild( "menuitem" )
            .getChild( 2 ).getChild( "onclick" ).toString();
    if( onclick ) {
        // do something useful

    @endverbatim

**/

class SP_JsonHandle {
public:
	SP_JsonHandle( const SP_JsonNode * node );

	SP_JsonHandle( const SP_JsonHandle & ref );

	SP_JsonHandle & operator=( const SP_JsonHandle & ref );

	SP_JsonHandle getChild( const char * name ) const;

	SP_JsonHandle getChild( int index ) const;

	///////////////////////////////////////////////////////

	SP_JsonNode * toNode() const;

	SP_JsonObjectNode * toObject() const;

	SP_JsonArrayNode * toArray() const;

	SP_JsonPairNode * toPair() const;

	SP_JsonStringNode * toString() const;

	SP_JsonDoubleNode * toDouble() const;

	SP_JsonIntNode * toInt() const;

	SP_JsonBooleanNode * toBoolean() const;

	SP_JsonNullNode * toNull() const;

	SP_JsonCommentNode * toComment() const;

private:

	SP_JsonNode * checkType( int type ) const;

	const SP_JsonNode * mNode;
};

#endif

