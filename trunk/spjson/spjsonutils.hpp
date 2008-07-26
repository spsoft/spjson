/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spjsonutils_hpp__
#define __spjsonutils_hpp__

#include <stdio.h>

typedef struct tagSP_JsonArrayListNode SP_JsonArrayListNode_t;

class SP_JsonArrayList {
public:
	static const int LAST_INDEX;

	SP_JsonArrayList( int initCount = 2 );
	virtual ~SP_JsonArrayList();

	int getCount() const;
	int append( void * value );
	const void * getItem( int index ) const;
	void * takeItem( int index );
	void sort( int ( * cmpFunc )( const void *, const void * ) );

private:
	SP_JsonArrayList( SP_JsonArrayList & );
	SP_JsonArrayList & operator=( SP_JsonArrayList & );

	int mMaxCount;
	int mCount;
	void ** mFirst;
};

class SP_JsonQueue {
public:
	SP_JsonQueue();
	virtual ~SP_JsonQueue();

	void push( void * item );
	void * pop();
	void * top();

private:
	void ** mEntries;
	unsigned int mHead;
	unsigned int mTail;
	unsigned int mCount;
	unsigned int mMaxCount;
};

class SP_JsonStringBuffer {
public:
	SP_JsonStringBuffer();
	virtual ~SP_JsonStringBuffer();
	int append( char c );
	int append( const char * value, int size = 0 );
	int getSize() const;
	const char * getBuffer() const;
	char * takeBuffer();
	void clean();

private:
	SP_JsonStringBuffer( SP_JsonStringBuffer & );
	SP_JsonStringBuffer & operator=( SP_JsonStringBuffer & );

	void init();

	char * mBuffer;
	int mMaxSize;
	int mSize;	
};

#ifdef WIN32

#define snprintf _snprintf
#define strncasecmp strnicmp
#define strcasecmp  stricmp
#endif

#endif

