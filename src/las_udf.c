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
 *  las_udft.c
 *  lua-aerospike
 *
 *  Created by Masatoshi Teruya on 2014/09/02.
 *
 */

#include "las.h"
#include "las_connect.h"

typedef struct {
    int ref_conn;
} las_udf_t;


static inline las_conn_t *get_connection( lua_State *L )
{
    las_udf_t *udf = (las_udf_t*)luaL_checkudata( L, 1, LAS_UDF_MT );
    las_conn_t *conn = NULL;
    
    lstate_pushref( L, udf->ref_conn );
    conn = lua_touserdata( L, -1 );
    lua_pop( L, 1 );
    
    return conn;
}


static inline void set_udf2lua( lua_State *L, as_udf_file *file )
{
    lua_createtable( L, 0, 3 );
    lstate_str2tbl( L, "name", file->name );
    lstate_num2tbl( L, "size", file->content.size );
    lstate_strn2tbl( L, "hash", (char*)file->hash, AS_UDF_FILE_HASH_SIZE );
}


static int get_lua( lua_State *L )
{
    int rv = 1;
    las_conn_t *conn = get_connection( L );
    const char *filename = lstate_checkstring( L, 2 );
    as_error err;
    as_udf_file file;
    
    as_udf_file_init( &file );
    if( aerospike_udf_get( conn->as, &err, NULL, filename, AS_UDF_TYPE_LUA,
        &file ) == AEROSPIKE_OK ){
        set_udf2lua( L, &file );
    }
    // got error
    else {
        lua_pushnil( L );
        lua_pushstring( L, err.message );
        rv++;
    }
    as_udf_file_destroy( &file );
    
    return rv;
}


static int list_lua( lua_State *L )
{
    int rv = 1;
    las_conn_t *conn = get_connection( L );
    as_error err;
    as_udf_files files;
    
    as_udf_files_init( &files, 0 );
    if( aerospike_udf_list( conn->as, &err, NULL, &files ) == AEROSPIKE_OK )
    {
        lua_createtable( L, 0, files.size );
        if( files.size )
        {
            uint32_t i = 0;
            
            for(; i < files.size; i++ ){
                lua_pushnumber( L, i );
                set_udf2lua( L, &files.entries[i] );
                lua_rawset( L, -3 );
            }
        }
    }
    // got error
    else {
        lua_pushnil( L );
        lua_pushstring( L, err.message );
        rv++;
    }
    as_udf_files_destroy( &files );
    
    return rv;
}


static int put_lua( lua_State *L )
{
    int rv = 1;
    las_conn_t *conn = get_connection( L );
    const char *filename = lstate_checkstring( L, 2 );
    size_t len = 0;
    const char *source = lstate_checklstring( L, 3, &len );
    as_error err;
    as_bytes content;
    
    as_bytes_init_wrap( &content, (uint8_t*)source, (uint32_t)len, false );
    if( aerospike_udf_put( conn->as, &err, NULL, filename, AS_UDF_TYPE_LUA,
                           &content ) == AEROSPIKE_OK ){
        lua_pushboolean( L, 1 );
    }
    // got error
    else {
        lua_pushnil( L );
        lua_pushstring( L, err.message );
        rv++;
    }
    as_bytes_destroy( &content );
    
    return rv;
}


static int remove_lua( lua_State *L )
{
    las_conn_t *conn = get_connection( L );
    const char *filename = lstate_checkstring( L, 2 );
    as_error err;
    
    if( aerospike_udf_remove( conn->as, &err, NULL, filename) == AEROSPIKE_OK ){
        lua_pushboolean( L, 1 );
        return 1;
    }
    
    // got error
    lua_pushnil( L );
    lua_pushstring( L, err.message );
    
    return 2;
}


static int alloc_lua( lua_State *L )
{
    las_udf_t *udf = NULL;
    
    // arg#1 connection instance
    luaL_checkudata( L, 1, LAS_CONNECT_MT );
    if( ( udf = lua_newuserdata( L, sizeof( las_udf_t ) ) ) ){
        udf->ref_conn = lstate_ref( L, 1 );
        lstate_setmetatable( L, LAS_UDF_MT );
        return 1;
    }
    // mem error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );

    return 2;
}

static int gc_lua( lua_State *L )
{
    las_udf_t *ctx = (las_udf_t*)lua_touserdata( L, 1 );
    
    // release las_conn_t reference
    lstate_unref( L, ctx->ref_conn );

    return 0;
}


static int tostring_lua( lua_State *L )
{
    return TOSTRING_MT( L, LAS_CONTEXT_MT );
}


LUALIB_API int luaopen_aerospike_udf( lua_State *L )
{
    struct luaL_Reg mmethod[] = {
        { "__gc", gc_lua },
        { "__tostring", tostring_lua },
        { NULL, NULL }
    };
    struct luaL_Reg method[] = {
        { "get", get_lua },
        { "list", list_lua },
        { "put", put_lua },
        { "remove", remove_lua },
        { NULL, NULL }
    };
    
    lstate_definemt( L, LAS_UDF_MT, mmethod, method, NULL );
    // add methods
    lua_pushcfunction( L, alloc_lua );
    
    return 1;
}


