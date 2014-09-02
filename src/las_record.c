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
 *  las_record.c
 *  lua-aerospike
 *
 *  Created by Masatoshi Teruya on 2014/08/26.
 *
 */

#include "las_record.h"


static int verify( lua_State *L, int ttype, int vtype, int depth, void *udata )
{
    // cannot be included both of array and hash into same nested level
    if( ttype == LUA_TTABLE_HASHLIST ){
        return LSTATE_TBLREAD_ABRT;
    }
    // these value type does not supported
    switch( vtype ){
        case LUA_TLIGHTUSERDATA:
        case LUA_TFUNCTION:
        case LUA_TUSERDATA:
        case LUA_TTHREAD:
            return LSTATE_TBLREAD_ABRT;
    }
    
    if( depth == 0 )
    {
        uint16_t *nbin = (uint16_t*)udata;
        const char *name = NULL;
        
        // number of bin limit(UINT16_MAX) exceeded
        if( *nbin == UINT16_MAX ){
            return LSTATE_TBLREAD_ABRT;
        }
        (*nbin)++;
        
        // check name type
        // bin name must be type of string
        switch( lua_type( L, -2 ) ){
            case LUA_TBOOLEAN:
            case LUA_TNUMBER:
            case LUA_TTABLE:
            case LUA_TLIGHTUSERDATA:
            case LUA_TFUNCTION:
            case LUA_TUSERDATA:
            case LUA_TTHREAD:
                return LSTATE_TBLREAD_ABRT;
        }
        
        // check name length
        // bin name length must be 1-AS_BIN_NAME_MAX_LEN
        if( !( name = LAS_CHK_BINNAME( L, -2 ) ) ){
            return LSTATE_TBLREAD_ABRT;
        }
    }
    
    return LSTATE_TBLREAD_NEXT;
}


static int newindex_lua( lua_State *L )
{
    las_record_t *lrec = luaL_checkudata( L, 1, LAS_RECORD_MT );
    size_t len = 0;
    const char *name = lstate_checklstring( L, 2, &len );
    
    switch( len )
    {
        case 2:
            if( strncmp( "pk", name, len ) == 0 )
            {
                luaL_checktype( L, 3, LUA_TSTRING );
                if( lrec->ref_key != LUA_NOREF ){
                    lstate_unref( L, lrec->ref_key );
                }
                lrec->ref_key = lstate_ref( L, 3 );
            }
        break;
        case 3:
            if( strncmp( "ttl", name, len ) == 0 )
            {
                lua_Integer ttl = lstate_checkinteger( L, 3 );
                if( ttl > UINT32_MAX ){
                    return luaL_error( L, LAS_ERR_TTL_RANGE );
                }
                lrec->ttl = (uint32_t)ttl;
            }
        break;
        case 4:
            if( strncmp( "bins", name, len ) == 0 ){
                luaL_checktype( L, 3, LUA_TTABLE );
                lstate_unref( L, lrec->ref_bins );
                lrec->ref_bins = lstate_ref( L, 3 );
            }
        break;
    }
    
    return 0;
}


static int index_lua( lua_State *L )
{
    las_record_t *lrec = luaL_checkudata( L, 1, LAS_RECORD_MT );
    size_t len = 0;
    const char *name = lstate_checklstring( L, 2, &len );
    
    switch( len )
    {
        case 2:
            if( strncmp( "pk", name, len ) == 0 )
            {
                if( lrec->ref_key == LUA_NOREF ){
                    char pk[LAS_RECORD_PK_LEN] = {0};
                    digest2hex( (uint8_t*)pk, lrec->pk, AS_DIGEST_VALUE_SIZE );
                    lua_pushlstring( L, pk, LAS_RECORD_PK_LEN );
                }
                else {
                    lstate_pushref( L, lrec->ref_key );
                }
                return 1;
            }
        break;
        case 3:
            if( strncmp( "ttl", name, len ) == 0 ){
                lua_pushinteger( L, lrec->ttl );
                return 1;
            }
            else if( strncmp( "gen", name, len ) == 0 ){
                lua_pushinteger( L, lrec->gen );
                return 1;
            }
        break;
        case 4:
            if( strncmp( "bins", name, len ) == 0 ){
                lstate_pushref( L, lrec->ref_bins );
                return 1;
            }
        break;
        case 5:
            if( strncmp( "nbins", name, len ) == 0 ){
                lua_pushinteger( L, lrec->nbins );
                return 1;
            }
            else if( strncmp( "valid", name, len ) == 0 ){
                int valid = 0;
                
                lrec->nbins = 0;
                lstate_pushref( L, lrec->ref_bins );
                // check bins table fields
                lua_pushvalue( L, -1 );
                valid = lstate_tblread( L, verify, &lrec->nbins );
                lua_pop( L, 2 );
                lua_pushboolean( L, valid == LSTATE_TBLREAD_DONE );
                return 1;
            }
        break;
    }
    
    lua_pushnil( L );
    return 1;
}


static int tostring_lua( lua_State *L )
{
    return TOSTRING_MT( L, LAS_RECORD_MT );
}


static int gc_lua( lua_State *L )
{
    las_record_t *lrec = (las_record_t*)lua_touserdata( L, 1 );
    
    lstate_unref( L, lrec->ref_key );
    lstate_unref( L, lrec->ref_bins );

    return 0;
}


int las_record_alloc_lua( lua_State *L, as_record *rec )
{
    las_record_t *lrec = lua_newuserdata( L, sizeof( las_record_t ) );
    
    // allocate
    if( lrec ){
        as_digest *digest = as_key_digest( &rec->key );
        
        lstate_setmetatable( L, LAS_RECORD_MT );
        memcpy( lrec->pk, digest->value, AS_DIGEST_VALUE_SIZE );
        lrec->ttl = rec->ttl;
        lrec->gen = rec->gen;
        lrec->nbins = lstate_asrec2tbl( L, rec );
        lrec->ref_bins = lstate_ref( L, -1 );
        lrec->ref_key = LUA_NOREF;
        
        return 0;
    }
    
    return -1;
}


static int alloc_lua( lua_State *L )
{
    int argc = lua_gettop( L );
    lua_Integer ttl = -1;
    las_record_t *lrec = NULL;
    
    // check arguments
    luaL_checktype( L, 1, LUA_TSTRING );
    // arg#2 bins table
    luaL_checktype( L, 2, LUA_TTABLE );
    // arg#3 ttl
    if( argc > 2 )
    {
        ttl = lstate_checkinteger( L, 3 );
        if( ttl > UINT32_MAX ){
            lua_pushnil( L );
            lua_pushliteral( L, LAS_ERR_TTL_RANGE );
            return 2;
        }
    }
    
    // allocate record
    if( ( lrec = lua_newuserdata( L, sizeof( las_record_t ) ) ) ){
        lstate_setmetatable( L, LAS_RECORD_MT );
        lrec->ttl = (uint32_t)ttl;
        lrec->gen = 0;
        lrec->ref_key = lstate_ref( L, 1 );
        lrec->ref_bins = lstate_ref( L, 2 );
        lrec->nbins = 0;
        return 1;
    }
    
    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );
    
    return 2;
}


LUALIB_API int luaopen_aerospike_record( lua_State *L )
{
    struct luaL_Reg mmethod[] = {
        { "__gc", gc_lua },
        { "__tostring", tostring_lua },
        { "__newindex", newindex_lua },
        { "__index", index_lua },
        { NULL, NULL }
    };
    int i;
    
    // create table __metatable
    luaL_newmetatable( L, LAS_RECORD_MT );
    // metamethods
    for( i = 0; mmethod[i].name; i++ ){
        lstate_fn2tbl( L, mmethod[i].name, mmethod[i].func );
    }
    lua_pop( L, 1 );
    
    // add method
    lua_pushcfunction( L, alloc_lua );
    
    return 1;
}

