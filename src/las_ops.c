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

static las_ops_t *get_operaions( lua_State *L, las_ops_binop_t **op )
{
    // check arguments
    las_ops_t *ops = luaL_checkudata( L, 1, LAS_OPERATION_MT );
    
    if( ops->nalloc <= ops->nops )
    {
        las_ops_binop_t *bops = prealloc( ops->nalloc + 1, las_ops_binop_t, ops->bops );
    
        // mem error
        if( !bops ){
            lua_pushnil( L );
            lua_pushstring( L, strerror( errno ) );
            return NULL;
        }
        ops->bops = bops;
        ops->nalloc++;
    }
    
    *op = &ops->bops[ops->nops];
    
    return ops;
}


static int set_binname2binop( lua_State *L, las_ops_binop_t *op )
{
    size_t len = 0;
    const char *binname = LAS_CHK_LBINNAME( L, 2, &len );
    
    // check binname length
    if( binname ){
        memcpy( op->name, binname, len );
        op->name[len] = 0;
        return 0;
    }
    
    // error
    lua_pushnil( L );
    lua_pushliteral( L, LAS_ERR_BIN_NAME );
    
    return 2;
}


static int set_str2binop( lua_State *L, int idx, las_ops_binop_t *op )
{
    op->type = LUA_TSTRING;
    op->arg.refval = lstate_ref( L, idx );
    return 0;
}

static void set_int2binop( lua_State *L, int idx, las_ops_binop_t *op )
{
    op->type = LUA_TNUMBER;
    op->arg.ival = lua_tointeger( L, idx );
}

static int set_tbl2binop( lua_State *L, int idx, las_ops_binop_t *op )
{
    op->type = LUA_TTABLE;
    op->arg.refval = lstate_ref( L, idx );
    
    return 0;
}


static void set_nil2binop( las_ops_binop_t *op )
{
    op->type = LUA_TNIL;
}


static int mwrite_lua( lua_State *L, las_ops_binop_t *op )
{
    int rc = 0;
    
    // these value type does not supported
    switch( lua_type( L, 3 ) ){
        case LUA_TNUMBER:
            set_int2binop( L, 3, op );
        break;
        case LUA_TSTRING:
            rc = set_str2binop( L, 3, op );
        break;
        case LUA_TTABLE:
            rc = set_tbl2binop( L, 3, op );
        break;
        
        // LUA_TBOOLEAN:
        // LUA_TLIGHTUSERDATA:
        // LUA_TFUNCTION:
        // LUA_TUSERDATA:
        // LUA_TTHREAD:
        default:
            lua_pushnil( L );
            lua_pushfstring( L, "data type %s is not supported",
                             lua_typename( L, lua_type( L, 3 ) ) );
            return 2;
    }
    
    if( rc == 0 ){
        op->op = AS_OPERATOR_WRITE;
        return 0;
    }
    
    return 2;
}


static int mappend_lua( lua_State *L, las_ops_binop_t *op )
{
    if( lua_type( L, 3 ) != LUA_TSTRING ){
        lua_pushnil( L );
        lua_pushfstring( L, "data type %s is not supported",
                         lua_typename( L, lua_type( L, 3 ) ) );
        return 2;
    }
    else if( set_str2binop( L, 3, op ) == 0 ){
        op->op = AS_OPERATOR_APPEND;
        return 0;
    }
    
    return 2;
}


static int mprepend_lua( lua_State *L, las_ops_binop_t *op )
{
    if( lua_type( L, 3 ) != LUA_TSTRING ){
        lua_pushnil( L );
        lua_pushfstring( L, "data type %s is not supported",
                         lua_typename( L, lua_type( L, 3 ) ) );
        return 2;
    }
    else if( set_str2binop( L, 3, op ) == 0 ){
        op->op = AS_OPERATOR_PREPEND;
        return 0;
    }
    
    return 2;
}


#define modify_lua(L,operation) do { \
    las_ops_binop_t *op = NULL; \
    las_ops_t *ops = get_operaions( L, &op ); \
    if( ops && set_binname2binop( L, op ) == 0 ){ \
        if( operation( L, op ) == 0 ){ \
            ops->nops++; \
            lua_settop( L, 1 ); \
            return 1; \
        } \
    } \
    return 2; \
}while(0)


static int write_lua( lua_State *L )
{
    modify_lua( L, mwrite_lua );
}

static int append_lua( lua_State *L )
{
    modify_lua( L, mappend_lua );
}

static int prepend_lua( lua_State *L )
{
    modify_lua( L, mprepend_lua );
}


static int read_lua( lua_State *L )
{
    las_ops_binop_t *op = NULL;
    las_ops_t *ops = get_operaions( L, &op );
    
    if( ops && set_binname2binop( L, op ) == 0 ){
        set_nil2binop( op );
        op->op = AS_OPERATOR_READ;
        ops->nops++;
        lua_settop( L, 1 );
        return 1;
    }
    
    return 2;
}


static int incr_lua( lua_State *L )
{
    las_ops_binop_t *op = NULL;
    las_ops_t *ops = get_operaions( L, &op );
    
    if( ops && set_binname2binop( L, op ) == 0 ){
        luaL_checktype( L, 3, LUA_TNUMBER );
        set_int2binop( L, 3, op );
        op->op = AS_OPERATOR_INCR;
        ops->nops++;
        lua_settop( L, 1 );
        return 1;
    }
    
    return 2;
}


static int touch_lua( lua_State *L )
{
    las_ops_binop_t *op = NULL;
    las_ops_t *ops = get_operaions( L, &op );
    
    if( ops ){
        op->type = LUA_TNIL;
        op->op = AS_OPERATOR_TOUCH;
        ops->nops++;
        lua_settop( L, 1 );
        return 1;
    }
    
    return 2;
}


static int alloc_lua( lua_State *L )
{
    las_ops_t *ops = lua_newuserdata( L, sizeof( las_ops_t ) );
    
    // check arguments
    if( ops && ( ops->bops = palloc( las_ops_binop_t ) ) ){
        ops->nalloc = 0;
        ops->nops = 0;
        lstate_setmetatable( L, LAS_OPERATION_MT );
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
    int i = ops->nops;
    
    while( i-- > 0 )
    {
        // release references
        switch( ops->bops[i].type ){
            case LUA_TSTRING:
            case LUA_TTABLE:
                lstate_unref( L, ops->bops[i].arg.refval );
            break;
        }
    }
    
    pdealloc( ops->bops );
    
    return 0;
}


static int tostring_lua( lua_State *L )
{
    return TOSTRING_MT( L, LAS_CONTEXT_MT );
}


as_operations *las_ops2asops( lua_State *L, las_ops_t *ops, as_operations *asops )
{
    if( as_operations_init( asops, ops->nops ) )
    {
        uint16_t i = 0;
        las_ops_binop_t *bop = NULL;
        
        for(; i < ops->nops; i++ )
        {
            bop = &ops->bops[i];
            switch( bop->op )
            {
                case AS_OPERATOR_READ:
                    as_operations_add_read( asops, bop->name );
                break;
                case AS_OPERATOR_INCR:
                    as_operations_add_incr( asops, bop->name, bop->arg.ival );
                break;
                
                case AS_OPERATOR_WRITE:
                    if( bop->type == LUA_TNUMBER ){
                        as_operations_add_write_int64( asops, bop->name,
                                                       bop->arg.ival );
                    }
                    else if( bop->type == LUA_TSTRING ){
                        lstate_pushref( L, bop->arg.refval );
                        as_operations_add_write_strp( asops, bop->name,
                                                      lua_tostring( L, -1 ),
                                                      false );
                        lua_pop( L , 1 );
                    }
                    else {
                        void *val = NULL;
                        lstate_pushref( L, bop->arg.refval );
                        val = lstate_tbl2asval( L );
                        lua_pop( L , 1 );
                        as_operations_add_write( asops, bop->name, val );
                    }
                break;
                case AS_OPERATOR_APPEND:
                    lstate_pushref( L, bop->arg.refval );
                    as_operations_add_append_strp( asops, bop->name,
                                                   lua_tostring( L, -1 ),
                                                   false );
                    lua_pop( L, 1 );
                break;
                case AS_OPERATOR_PREPEND:
                    lstate_pushref( L, bop->arg.refval );
                    as_operations_add_prepend_strp( asops, bop->name,
                                                    lua_tostring( L, -1 ),
                                                    false );
                    lua_pop( L, 1 );
                break;
                
                case AS_OPERATOR_TOUCH:
                    as_operations_add_touch( asops );
                break;
            }
        }
        
        return asops;
    }
    
    return NULL;
}

LUALIB_API int luaopen_aerospike_operation( lua_State *L )
{
    struct luaL_Reg mmethod[] = {
        { "__gc", gc_lua },
        { "__tostring", tostring_lua },
        { NULL, NULL }
    };
    struct luaL_Reg method[] = {
        { "write", write_lua },
        { "read", read_lua },
        { "incr", incr_lua },
        { "append", append_lua },
        { "prepend", prepend_lua },
        { "touch", touch_lua },
        { NULL, NULL }
    };
    
    // define metatable
    lstate_definemt( L, LAS_OPERATION_MT, mmethod, method, NULL );
    // add methods
    lua_pushcfunction( L, alloc_lua );

    return 1;
}
