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
 *  las_connect.c
 *  lua-aerospike
 *
 *  Created by Masatoshi Teruya on 2014/08/24.
 *
 */

#include "las_connect.h"
#include "las_ctx.h"


static int context_lua( lua_State *L )
{
    return las_ctx_alloc_lua( L );
}


static int close_lua( lua_State *L )
{
    int rv = 1;
    las_conn_t *conn = luaL_checkudata( L, 1, LAS_CONNECT_MT );
    as_error err;
    
    if( aerospike_close( conn->as, &err ) == AEROSPIKE_OK ){
        conn->connected = 0;
        lua_pushboolean( L, 1 );
    }
    else {
        lua_pushboolean( L, 0 );
        lua_pushstring( L, err.message );
    }
    
    return rv;
}


static int alloc_lua( lua_State *L )
{
    as_config cfg;

    // allocate
    if( as_config_init( &cfg ) )
    {
        int argc = lua_gettop( L );
        las_conn_t *conn = NULL;
        const char *host = LAS_CONN_DEFAULT_HOST;
        uint16_t port = LAS_CONN_DEFAULT_PORT;
        
        // check host and port
        switch( argc ){
            case 2:
                port = (uint16_t)lstate_checkinteger( L, 2 );
            case 1:
                host = lstate_checkstring( L, 1 );
            break;
        }
        as_config_add_host( &cfg, host, port );
        
        // check user and password
        if( argc > 2 )
        {
            const char *user = lstate_checkstring( L, 3 );
            const char *pswd = lstate_checkstring( L, 4 );
            // got error
            if( !as_config_set_user( &cfg, user, pswd ) ){
                lua_pushnil( L );
                lua_pushstring( L, strerror( errno ) );
                return 2;
            }
        }
        
        // allocate
        if( ( conn = lua_newuserdata( L, sizeof( las_conn_t ) ) ) &&
            ( conn->as = aerospike_new( &cfg ) ) )
        {
            as_error err;
            
            if( aerospike_connect( conn->as, &err ) == AEROSPIKE_OK ){
                conn->connected = 1;
                lstate_setmetatable( L, LAS_CONNECT_MT );
                return 1;
            }
            
            aerospike_destroy( conn->as );
            lua_pushnil( L );
            lua_pushstring( L, err.message );
            
            return 2;
        }
    }
    
    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );
    
    return 2;
}


static int gc_lua( lua_State *L )
{
    las_conn_t *conn = (las_conn_t*)lua_touserdata( L, 1 );

    if( conn->connected ){
        as_error err;
        aerospike_close( conn->as, &err );
    }
    aerospike_destroy( conn->as );
    
    return 0;
}


static int tostring_lua( lua_State *L )
{
    return TOSTRING_MT( L, LAS_CONNECT_MT );
}


LUALIB_API int luaopen_aerospike_connect( lua_State *L )
{
    struct luaL_Reg mmethod[] = {
        { "__gc", gc_lua },
        { "__tostring", tostring_lua },
        { NULL, NULL }
    };
    struct luaL_Reg method[] = {
        { "close", close_lua },
        { "context", context_lua },
        { NULL, NULL }
    };
    
    lstate_definemt( L, LAS_CONNECT_MT, mmethod, method, NULL );
    // add methods
    lua_pushcfunction( L, alloc_lua );
    
    return 1;
}


