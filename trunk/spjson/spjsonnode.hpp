/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spjsonnode_hpp__
#define __spjsonnode_hpp__

class SP_JsonArrayList;

class SP_JsonNode {
public:
	enum { eObject, ePair, eArray, eString, eDouble, eInt, eBoolean, eNull, eComment };

	SP_JsonNode( int type );
	virtual ~SP_JsonNode();

	int getType() const;
	SP_JsonNode * getParent() const;
	void setParent( SP_JsonNode * parent );

	int isLastChild() const;

protected:
	const int mType;
	SP_JsonNode * mParent;
};

class SP_JsonStringNode : public SP_JsonNode {
public:
	SP_JsonStringNode( const char * value );
	virtual ~SP_JsonStringNode();

	const char * getValue() const;

private:
	char * mValue;
};

class SP_JsonDoubleNode : public SP_JsonNode {
public:
	SP_JsonDoubleNode( double value );
	virtual ~SP_JsonDoubleNode();

	double getValue() const;

private:
	double mValue;
};

class SP_JsonIntNode : public SP_JsonNode {
public:
	SP_JsonIntNode( long long value );
	virtual ~SP_JsonIntNode();

	long long getValue() const;

private:
	long long mValue;
};

class SP_JsonNullNode : public SP_JsonNode {
public:
	SP_JsonNullNode();
	virtual ~SP_JsonNullNode();
};

class SP_JsonBooleanNode : public SP_JsonNode {
public:
	SP_JsonBooleanNode( bool value );
	virtual ~SP_JsonBooleanNode();

	bool getValue() const;

private:
	bool mValue;
};

class SP_JsonCommentNode : public SP_JsonNode {
public:
	SP_JsonCommentNode( const char * comment );
	virtual ~SP_JsonCommentNode();

	const char * getValue() const;

private:
	char * mValue;
};

class SP_JsonPairNode : public SP_JsonNode {
public:
	SP_JsonPairNode();
	virtual ~SP_JsonPairNode();

	void setName( const char * name );
	const char * getName() const;

	void setValue( SP_JsonNode * value );
	SP_JsonNode * getValue() const;

private:
	char * mName;
	SP_JsonNode * mValue;
};

class SP_JsonObjectNode : public SP_JsonNode {
public:
	SP_JsonObjectNode();
	virtual ~SP_JsonObjectNode();

	int getCount() const;

	int addValue( SP_JsonPairNode * value );
	SP_JsonPairNode * getValue( int index ) const;
	SP_JsonPairNode * takeValue( int index );

	int find( const char * name ) const;

	int isLastChild( const SP_JsonNode * value ) const;

private:
	SP_JsonArrayList * mValueList;
};

class SP_JsonArrayNode : public SP_JsonNode {
public:
	SP_JsonArrayNode();
	virtual ~SP_JsonArrayNode();

	int getCount() const;

	int addValue( SP_JsonNode * value );
	SP_JsonNode * getValue( int index ) const;
	SP_JsonNode * takeValue( int index );

	int isLastChild( const SP_JsonNode * value ) const;

private:
	SP_JsonArrayList * mValueList;
};

#endif

