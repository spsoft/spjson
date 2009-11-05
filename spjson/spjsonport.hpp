/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spjsonport_hpp__
#define __spjsonport_hpp__

#ifndef WIN32

#include <stdint.h>

#else

typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;

typedef int int32_t;
typedef unsigned int uint32_t;

typedef short int16_t;
typedef unsigned short uint16_t;

#define snprintf _snprintf

#endif

#endif
