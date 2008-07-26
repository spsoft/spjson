/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spjsonparser_hpp__
#define __spjsonparser_hpp__

class SP_JsonPullEvent;
class SP_JsonPullEventQueue;
class SP_JsonReader;
class SP_JsonArrayList;

class SP_JsonPullParser {
public:
	SP_JsonPullParser();
	~SP_JsonPullParser();

	/// append more input json source
	int append( const char * source, int len );

	/// @return NOT NULL : the pull event
	/// @return NULL : error or need more input
	SP_JsonPullEvent * getNext();	

	/// @return NOT NULL : the detail error message
	/// @return NULL : no error
	const char * getError();

	int getLevel();

protected:
	void changeReader( SP_JsonReader * reader );

	void setError( const char * error );

	int isWait4Name();

	int isObjectParent();

	friend class SP_JsonReader;

private:
	SP_JsonPullEventQueue * mEventQueue;
	SP_JsonReader * mReader;
	SP_JsonArrayList * mStack;

	SP_JsonArrayList * mNameList;

	int mLevel;

	char * mError;

	char mErrorSegment[ 32 ];
	int mErrorIndex;
	int mColIndex, mRowIndex;

	int mObjectCount;
};

#endif

