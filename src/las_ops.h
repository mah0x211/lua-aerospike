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
 *  las_ops.h
 *  lua-aerospike
 *
 *  Created by Masatoshi Teruya on 2014/08/31.
 *
 */


#ifndef lua_aerospike_las_ops_h
#define lua_aerospike_las_ops_h

#include "las.h"

typedef struct {
    as_operator op;
    char name[AS_BIN_NAME_MAX_SIZE];
    int type;
    union {
        lua_Integer ival;
        int refval;
    } arg;
} las_ops_binop_t;

typedef struct {
    las_ops_binop_t *bops;
    uint16_t nalloc;
    uint16_t nops;
} las_ops_t;

LUALIB_API int luaopen_aerospike_operation( lua_State *L );

as_operations *las_ops2asops( lua_State *L, las_ops_t *ops, as_operations *asops );


#endif
