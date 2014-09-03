/*
 *  Copyright 2014 Masatoshi Teruya. All rights reserved.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *  THE SOFTWARE.
 *
 *
 *  las.h
 *  lua-aerospike
 *
 *  Created by Masatoshi Teruya on 2014/08/24.
 *
 */


#ifndef lua_aerospike_las_h
#define lua_aerospike_las_h

#include <inttypes.h>
#include <sys/errno.h>
#include <lua.h>
#include <lauxlib.h>

#include "las_deps.h"
#include "las_util.h"

// define module names
#define LAS_CONNECTION_MT   "aerospike.connection"
#define LAS_UDF_MT          "aerospike.udf"
#define LAS_CONTEXT_MT      "aerospike.context"
#define LAS_OPERATION_MT    "aerospike.operation"
#define LAS_RECORD_MT       "aerospike.record"

// common metamethods
#define TOSTRING_MT(L,tname) ({ \
    lua_pushfstring( L, tname ": %p", lua_touserdata( L, 1 ) ); \
    1; \
})

// prototypes
LUALIB_API int luaopen_aerospike( lua_State *L );


#endif

