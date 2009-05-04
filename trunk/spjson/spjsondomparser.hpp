/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spjsondomparser_hpp__
#define __spjsondomparser_hpp__

class SP_JsonNode;
class SP_JsonObjectNode;
class SP_JsonArrayNode;

class SP_JsonStringBuffer;
class SP_JsonArrayList;

class SP_JsonPullParser;

class SP_JsonDomParser {
public:
	SP_JsonDomParser();
	~SP_JsonDomParser();

	int append( const char * source, int len );

	const char * getError() const;

	SP_JsonNode * getValue() const;

private:

	void buildTree();

	void addNode( SP_JsonNode * node );

	SP_JsonPullParser * mParser;
	SP_JsonArrayNode * mContainer;
	SP_JsonNode * mCurrent;

private:
	// disable these two default member functions
	SP_JsonDomParser( SP_JsonDomParser & );
	SP_JsonDomParser & operator=( SP_JsonDomParser & );
};

class SP_JsonDomBuffer {
public:
	SP_JsonDomBuffer( const SP_JsonNode * node, int indent = 1 );
	~SP_JsonDomBuffer();

	const char * getBuffer() const;
	int getSize() const;

public:

	static void dump( const SP_JsonNode * node, char * buffer, int len );

	static void dump( const SP_JsonNode * node,
			SP_JsonStringBuffer * buffer, int level );
	static void dumpObject( const SP_JsonNode * node,
			SP_JsonStringBuffer * buffer, int level );
	static void dumpArray( const SP_JsonNode * node,
			SP_JsonStringBuffer * buffer, int level );

private:
	SP_JsonStringBuffer * mBuffer;
};

#endif

