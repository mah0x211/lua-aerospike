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
 *  Created by Masatoshi Teruya on 14/08/24.
 *
 */


#ifndef lua_aerospike_las_h
#define lua_aerospike_las_h

#include <inttypes.h>
#include <sys/errno.h>
#include <lua.h>
#include <lauxlib.h>

#include <aerospike/aerospike.h>
#include <aerospike/aerospike_key.h>
#include <aerospike/aerospike_batch.h>
#include <aerospike/aerospike_scan.h>
#include <aerospike/aerospike_info.h>
#include <aerospike/aerospike_index.h>
// record
#include <aerospike/as_record.h>
#include <aerospike/as_record_iterator.h>
// lists
#include <aerospike/as_list.h>
#include <aerospike/as_iterator.h>
#include <aerospike/as_arraylist.h>
#include <aerospike/as_arraylist_iterator.h>
// maps
#include <aerospike/as_hashmap.h>
#include <aerospike/as_stringmap.h>
#include <aerospike/as_pair.h>
#include <aerospike/as_hashmap_iterator.h>
// primitives
#include <aerospike/as_node.h>
#include <aerospike/as_boolean.h>
#include <aerospike/as_status.h>
#include <aerospike/as_error.h>
#include <aerospike/as_key.h>
#include <aerospike/as_bin.h>


// define module names
#define LAS_CONNECT_MT      "aerospike.connect"
#define LAS_CONTEXT_MT      "aerospike.context"
#define LAS_OPERATION_MT    "aerospike.operation"
#define LAS_RECORD_MT       "aerospike.record"

#include "las_util.h"

// common metamethods
#define TOSTRING_MT(L,tname) ({ \
    lua_pushfstring( L, tname ": %p", lua_touserdata( L, 1 ) ); \
    1; \
})

// prototypes
LUALIB_API int luaopen_aerospike( lua_State *L );


#endif

