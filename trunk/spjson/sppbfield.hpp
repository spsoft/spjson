/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __sppbfield_hpp__
#define __sppbfield_hpp__

class SP_ProtoBufFieldList
{
public:
	typedef struct tagOffsetList {
		int mCount, mTotal;
		int mList[0];
	} OffsetList_t;

	typedef struct tagField {
		int mFieldNumber : 30;
		int mIsRepeated : 1;

		union {
			int mOffset;
			OffsetList_t * mList;
		};
	} Field_t;

public:
	SP_ProtoBufFieldList();
	~SP_ProtoBufFieldList();

	// @return 0 : Insert OK, 1 : Append OK, -1 : Fail
	int addFieldOffset( int fieldNumber, int offset );

	Field_t * findField( int fieldNumber );

	int getFieldCount();

	Field_t * getField( int index );

	void print();

private:

	int ensureFieldList( int count );

	static int ensureOffsetList( Field_t * field, int count );

	int binarySearch( int fieldNumber, int * insertPoint,
			int firstIndex, int size ) const;

private:
	Field_t * mList;	
	int mCount, mTotal;
};

#endif

