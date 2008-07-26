/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spjsoniterator_hpp__
#define __spjsoniterator_hpp__

class SP_JsonNode;

/// DFS iterator -- Depth First Searc
class SP_JsonIterator {
public:
	/// node as tree node, iterator this tree by DFS
	SP_JsonIterator( const SP_JsonNode * node );
	~SP_JsonIterator();

	/// @return NULL : reach the end
	const SP_JsonNode * getNext();

private:
	const SP_JsonNode * mRoot;
	const SP_JsonNode * mCurrent;

private:
	SP_JsonIterator( SP_JsonIterator & );
	SP_JsonIterator & operator=( SP_JsonIterator & );
};


#endif

