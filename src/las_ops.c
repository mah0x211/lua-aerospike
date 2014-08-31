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
 *  las_ops.c
 *  lua-aerospike
 *
 *  Created by Masatoshi Teruya on 2014/08/31.
 *
 */

#include "las_ops.h"


static int write_lua( lua_State *L )
{
    // check arguments
    las_ops_t *ops = luaL_checkudata( L, 1, LAS_OPERATE_MT );
    size_t len = 0;
    const char *name = lstate_checkstring( L, 2 );
    int type = lua_type( L, 3 );
    void *val = NULL;
    lua_Integer ival = 0;
    as_binop *bops = NULL;
    
    // check name length
    if( len > AS_BIN_NAME_MAX_LEN ){
        lua_pushnil( L );
        lua_pushfstring( L, "bin name length must be 0-%d", AS_BIN_NAME_MAX_LEN );
        return 2;
    }
    
    // these value type does not supported
    switch( type ){
        case LUA_TSTRING:
            val = (void*)lua_tostring( L, 3 );
            val = as_string_new( val, false );
        break;
        case LUA_TNUMBER:
            ival = lua_tointeger( L, 3 );
            val = as_integer_new( ival );
        break;
        case LUA_TBOOLEAN:
            ival = lua_toboolean( L, 3 );
            val = as_boolean_new( ival );
        break;
        case LUA_TTABLE:
        break;
        
        case LUA_TLIGHTUSERDATA:
        case LUA_TFUNCTION:
        case LUA_TUSERDATA:
        case LUA_TTHREAD:
            lua_pushnil( L );
            lua_pushfstring( L, "data type %s is not supported",
                             lua_typename( L, type ) );
            return 2;
    }
    
    if( val )
    {
        if( ( bops = prealloc( ops->nbops + 1, as_binop, ops->bops ) ) )
        {
            as_binop *op = &bops[ops->nbops];
            
            op->op = AS_OPERATOR_WRITE;
            memcpy( op->bin.name, name, len );
            op->bin.valuep = val;
            
            ops->nbops++;
            ops->bops = bops;
            lua_settop( L, 1 );
            return 1;
        }
        
        as_val_destroy( (as_val*)val );
    }
    
    // mem error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );
    
    return 2;
}


static int alloc_lua( lua_State *L )
{
    las_ops_t *ops = NULL;
    
    // check arguments
    luaL_checkudata( L, 1, LAS_CONTEXT_MT );
    if( ( ops = lua_newuserdata( L, sizeof( las_ops_t ) ) ) &&
        ( ops->bops = pcalloc( 0, as_binop ) ) ){
        ops->nbops = 0;
        lstate_setmetatable( L, LAS_OPERATE_MT );
        return 1;
    }
    
    // mem error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );
    
    return 2;
}

static int gc_lua( lua_State *L )
{
    las_ops_t *ops = (las_ops_t*)lua_touserdata( L, 1 );
    
    pdealloc( ops->bops );
    
    return 0;
}

static int tostring_lua( lua_State *L )
{
    return TOSTRING_MT( L, LAS_CONTEXT_MT );
}


int las_ops_init( lua_State *L )
{
    struct luaL_Reg mmethod[] = {
        { "__gc", gc_lua },
        { "__tostring", tostring_lua },
        { NULL, NULL }
    };
    struct luaL_Reg method[] = {
        { "write", write_lua },
        /*
        { "read", read_lua },
        { "incr", incr_lua },
        { "prepend", prepend_lua },
        { "append", append_lua },
        { "touch", touch_lua },
        */
        { NULL, NULL }
    };
    
    // define metatable
    lstate_definemt( L, LAS_CONTEXT_MT, mmethod, method, NULL );
    // add methods
    lua_pushcfunction( L, alloc_lua );

    return 1;
}
