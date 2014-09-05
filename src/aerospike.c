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
 *  las.c
 *  lua-aerospike
 *
 *  Created by Masatoshi Teruya on 2014/08/24.
 *
 */

#include "las.h"
#include "las_connect.h"
#include "las_udf.h"
#include "las_ctx.h"
#include "las_ops.h"
#include "las_record.h"

LUALIB_API int luaopen_aerospike( lua_State *L )
{
    // context
    las_ctx_init( L );
    
    // add methods
    lua_newtable( L );
    // connection
    luaopen_aerospike_connect( L );
    lua_setfield( L, -2, "open" );
    // operation
    luaopen_aerospike_operation( L );
    lua_setfield( L, -2, "operation" );
    // UDF
    luaopen_aerospike_udf( L );
    lua_setfield( L, -2, "udf" );
    // record
    //luaopen_aerospike_record( L );
    //lua_setfield( L, -2, "record" );
    
    // constants
    // types for create index
    lstate_num2tbl( L, "IDX_INTEGER", LAS_IDX_INTEGER );
    lstate_num2tbl( L, "IDX_STRING", LAS_IDX_STRING );
    // scan priorities
    lstate_num2tbl( L, "SCAN_PRIORITY_AUTO", AS_SCAN_PRIORITY_AUTO );
    lstate_num2tbl( L, "SCAN_PRIORITY_LOW", AS_SCAN_PRIORITY_LOW );
    lstate_num2tbl( L, "SCAN_PRIORITY_MEDIUM", AS_SCAN_PRIORITY_MEDIUM );
    lstate_num2tbl( L, "SCAN_PRIORITY_HIGH", AS_SCAN_PRIORITY_HIGH );
    
    return 1;
}



