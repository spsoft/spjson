/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spjsonreader_hpp__
#define __spjsonreader_hpp__

class SP_JsonPullParser;
class SP_JsonPullEvent;
class SP_JsonStringBuffer;

class SP_JsonReader {
public:
	/**
	 * @param  parser : act as reader's context
	 * @param  c : a char in json stream
	 */
	virtual void read( SP_JsonPullParser * parser, char c ) = 0;

	/**
	 * convert internal json string to event
	 * @return NULL : this reader cannot generate any event or error occured
	 */
	virtual SP_JsonPullEvent * getEvent( SP_JsonPullParser * parser ) = 0;

protected:
	SP_JsonStringBuffer * mBuffer;

	SP_JsonReader();
	virtual ~SP_JsonReader();

	friend class SP_JsonPullParser;

	/// help to call parser->changeReader
	void changeReader( SP_JsonPullParser * parser, SP_JsonReader * reader );

	/// help to call parser->setError
	static void setError( SP_JsonPullParser * parser, const char * error );

	/// help to call parser->isWait4Name
	int isWait4Name( SP_JsonPullParser * parser );

	/// help to call parser->isObjectParent
	int isObjectParent( SP_JsonPullParser * parser );

private:
	SP_JsonReader( SP_JsonReader & );
	SP_JsonReader & operator=( SP_JsonReader & );
};

class SP_JsonSpaceReader : public SP_JsonReader {
public:
	SP_JsonSpaceReader( int hasReadComma = 0 );
	virtual ~SP_JsonSpaceReader();
	virtual void read( SP_JsonPullParser * parser, char c );
	virtual SP_JsonPullEvent * getEvent( SP_JsonPullParser * parser );

private:
	int mHasReadComma;
};

class SP_JsonStartObjectReader : public SP_JsonReader {
public:
	SP_JsonStartObjectReader( int hasReadLeftBrace = 0 );
	virtual ~SP_JsonStartObjectReader();
	virtual void read( SP_JsonPullParser * parser, char c );
	virtual SP_JsonPullEvent * getEvent( SP_JsonPullParser * parser );

private:
	int mHasReadLeftBrace;
};

class SP_JsonEndObjectReader : public SP_JsonReader {
public:
	SP_JsonEndObjectReader();
	virtual ~SP_JsonEndObjectReader();
	virtual void read( SP_JsonPullParser * parser, char c );
	virtual SP_JsonPullEvent * getEvent( SP_JsonPullParser * parser );
};

class SP_JsonStartArrayReader : public SP_JsonReader {
public:
	SP_JsonStartArrayReader();
	virtual ~SP_JsonStartArrayReader();
	virtual void read( SP_JsonPullParser * parser, char c );
	virtual SP_JsonPullEvent * getEvent( SP_JsonPullParser * parser );

private:
	int mHasReadLeftBracket;
};

class SP_JsonEndArrayReader : public SP_JsonReader {
public:
	SP_JsonEndArrayReader();
	virtual ~SP_JsonEndArrayReader();
	virtual void read( SP_JsonPullParser * parser, char c );
	virtual SP_JsonPullEvent * getEvent( SP_JsonPullParser * parser );
};

class SP_JsonNameReader : public SP_JsonReader {
public:
	SP_JsonNameReader();
	virtual ~SP_JsonNameReader();
	virtual void read( SP_JsonPullParser * parser, char c );
	virtual SP_JsonPullEvent * getEvent( SP_JsonPullParser * parser );

private:
	char mLastChar;
	int mHasReadStartQuot;
};

class SP_JsonValueReader : public SP_JsonReader {
public:
	SP_JsonValueReader();
	virtual ~SP_JsonValueReader();
	virtual void read( SP_JsonPullParser * parser, char c );
	virtual SP_JsonPullEvent * getEvent( SP_JsonPullParser * parser );
};

class SP_JsonStringReader : public SP_JsonReader {
public:
	SP_JsonStringReader();
	virtual ~SP_JsonStringReader();
	virtual void read( SP_JsonPullParser * parser, char c );
	virtual SP_JsonPullEvent * getEvent( SP_JsonPullParser * parser );

private:
	char mLastChar;
	int mHasReadStartQuot;
};

class SP_JsonNumberReader : public SP_JsonReader {
public:
	SP_JsonNumberReader();
	virtual ~SP_JsonNumberReader();
	virtual void read( SP_JsonPullParser * parser, char c );
	virtual SP_JsonPullEvent * getEvent( SP_JsonPullParser * parser );
};

class SP_JsonBooleanReader : public SP_JsonReader {
public:
	SP_JsonBooleanReader();
	virtual ~SP_JsonBooleanReader();
	virtual void read( SP_JsonPullParser * parser, char c );
	virtual SP_JsonPullEvent * getEvent( SP_JsonPullParser * parser );
};

class SP_JsonNullReader : public SP_JsonReader {
public:
	SP_JsonNullReader();
	virtual ~SP_JsonNullReader();
	virtual void read( SP_JsonPullParser * parser, char c );
	virtual SP_JsonPullEvent * getEvent( SP_JsonPullParser * parser );
};

class SP_JsonCommentReader : public SP_JsonReader {
public:
	SP_JsonCommentReader( SP_JsonReader * savedReader );
	virtual ~SP_JsonCommentReader();
	virtual void read( SP_JsonPullParser * parser, char c );
	virtual SP_JsonPullEvent * getEvent( SP_JsonPullParser * parser );

private:
	int mHasReadStartSolidus;
	SP_JsonReader * mSavedReader;
};

class SP_JsonPaddingReader : public SP_JsonReader {
public:
	SP_JsonPaddingReader();
	virtual ~SP_JsonPaddingReader();
	virtual void read( SP_JsonPullParser * parser, char c );
	virtual SP_JsonPullEvent * getEvent( SP_JsonPullParser * parser );
};

#endif

