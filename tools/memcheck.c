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
 *  memcheck.c
 *  lua-aerospike
 *
 *  Created by Masatoshi Teruya on 2014/09/04.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "../src/las.h"

int main (int argc, const char * argv[])
{
    const char *script = ( argc > 1 ) ? argv[1] : NULL;
    lua_State *L = NULL;
    const char *path = NULL;;
    
    if( !script ){
        printf("script path argument undefined\n");
        return 0;
    }
    
    if( ( path = getenv( "LUA_PATH" ) ) ){
        setenv( "LUA_PATH", path, 1 );
    }
    else if( ( path = getenv( "LUA_CPATH" ) ) ){
        setenv( "LUA_CPATH", path, 1 );
    }
    
    L = luaL_newstate();
    luaL_openlibs( L );
    // add aerospike module to package.loaded
    lua_getglobal( L, "package" );
    lua_getfield( L, -1, "loaded" );
    lua_pushstring( L, "aerospike");
    luaopen_aerospike( L );
    lua_rawset( L, -3 );
    lua_pop( L, 2 );
    
    
    lua_createtable( L, 1, 0 );
    lua_pushstring( L, script );
    lua_rawseti( L, -2, 0 );
    lua_setglobal( L, "arg" );
    
    luaL_loadfile( L, script );
    if( lua_pcall( L, 0, 0, 0 ) ){
        printf( "%s\n", lua_tostring(L, -1) );
    }
    
    lua_close( L );
    
    return 0;
}


