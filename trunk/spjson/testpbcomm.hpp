/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __testpbcomm_hpp__
#define __testpbcomm_hpp__

class SP_ProtoBufDecoder;

extern void printAll( SP_ProtoBufDecoder * decoder );

extern void printAllConst( const char * buffer, int len );

extern void printAll( char * buffer, int len );

#endif

