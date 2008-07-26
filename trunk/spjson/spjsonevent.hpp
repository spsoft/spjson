/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spjsonevent_hpp__
#define __spjsonevent_hpp__

class SP_JsonArrayList;
class SP_JsonQueue;

class SP_JsonPullEvent {
public:
	enum EventType { eStartObject, eEndObject, eStartArray, eEndArray, eNull,
			eName, eString, eNumber, eBoolean, eComment, ePadding };

	SP_JsonPullEvent( int eventType );
	virtual ~SP_JsonPullEvent();

	int getEventType();

private:
	/// Private copy constructor and copy assignment ensure classes derived from
	/// this cannot be copied.
	SP_JsonPullEvent( SP_JsonPullEvent & );
	SP_JsonPullEvent & operator=( SP_JsonPullEvent & );

protected:
	const int mEventType;
};

class SP_JsonPullEventQueue {
public:
	SP_JsonPullEventQueue();
	~SP_JsonPullEventQueue();

	void enqueue( SP_JsonPullEvent * event );
	SP_JsonPullEvent * dequeue();

private:
	SP_JsonPullEventQueue( SP_JsonPullEventQueue & );
	SP_JsonPullEventQueue & operator=( SP_JsonPullEventQueue & );

	SP_JsonQueue * mQueue;
};

class SP_JsonStartObjectEvent : public SP_JsonPullEvent {
public:
	SP_JsonStartObjectEvent();
	virtual ~SP_JsonStartObjectEvent();
};

class SP_JsonEndObjectEvent : public SP_JsonPullEvent {
public:
	SP_JsonEndObjectEvent();
	virtual ~SP_JsonEndObjectEvent();
};

class SP_JsonStartArrayEvent : public SP_JsonPullEvent {
public:
	SP_JsonStartArrayEvent();
	virtual ~SP_JsonStartArrayEvent();
};

class SP_JsonEndArrayEvent : public SP_JsonPullEvent {
public:
	SP_JsonEndArrayEvent();
	virtual ~SP_JsonEndArrayEvent();
};

class SP_JsonTextEvent : public SP_JsonPullEvent {
public:
	SP_JsonTextEvent( int eventType );
	virtual ~SP_JsonTextEvent();

	void setText( const char * text, int len );
	const char * getText() const;

private:
	char * mText;
};

class SP_JsonNullEvent : public SP_JsonTextEvent {
public:
	SP_JsonNullEvent();
	virtual ~SP_JsonNullEvent();
};

class SP_JsonNameEvent : public SP_JsonTextEvent {
public:
	SP_JsonNameEvent();
	virtual ~SP_JsonNameEvent();
};

class SP_JsonStringEvent : public SP_JsonTextEvent {
public:
	SP_JsonStringEvent();
	virtual ~SP_JsonStringEvent();
};

class SP_JsonNumberEvent : public SP_JsonTextEvent {
public:
	SP_JsonNumberEvent();
	virtual ~SP_JsonNumberEvent();
};

class SP_JsonBooleanEvent : public SP_JsonTextEvent {
public:
	SP_JsonBooleanEvent();
	virtual ~SP_JsonBooleanEvent();
};

class SP_JsonCommentEvent : public SP_JsonTextEvent {
public:
	SP_JsonCommentEvent();
	virtual ~SP_JsonCommentEvent();
};

class SP_JsonPaddingEvent : public SP_JsonTextEvent {
public:
	SP_JsonPaddingEvent();
	virtual ~SP_JsonPaddingEvent();
};

#endif

