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
 *  las_util.c
 *  lua-aerospike
 *
 *  Created by Masatoshi Teruya on 2014/08/26.
 *
 */

#include "las_util.h"
#include "bitvec.h"

// MARK: table traverse
int lstate_tblread( lua_State *L, lstate_tblread_cb callback, void *udata )
{
    bitvec_t bv;
    uint64_t pos = 0;
    int depth = 0;
    int ttype = 0;
    int vtype;
    int rc;
    
    if( bitvec_alloc( &bv, 64 ) == -1 ){
        return LSTATE_TBLREAD_ERR;
    }
    
CHECK_TABLE:
    // push space
    lua_pushnil( L );
    // key: -2, val: -1
    while( lua_next( L, -2 ) )
    {
        vtype = lua_type( L, -1 );
        switch( lua_type( L, -2 ) ){
            case LUA_TNUMBER:
                bitvec_set( &bv, pos );
                ttype |= LUA_TTABLE_LIST;
            break;
            case LUA_TSTRING:
                bitvec_set( &bv, pos + 1 );
                ttype |= LUA_TTABLE_HASH;
            break;
        }
        
        rc = callback( L, ttype & LUA_TTABLE_HASHLIST, vtype, depth, udata );
        if( rc != LSTATE_TBLREAD_NEXT ){
            bitvec_dealloc( &bv );
            return rc;
        }
        else if( vtype == LUA_TTABLE ){
            depth++;
            pos = depth * 2;
            ttype = 0;
            goto CHECK_TABLE;
        }
CHECK_REMAIN:
        lua_pop( L, 1 );
    }
    
    if( depth > 0 ){
        depth--;
        bitvec_unset( &bv, pos );
        bitvec_unset( &bv, pos + 1 );
        pos = depth * 2;
        ttype = bitvec_get( &bv, pos ) | ( bitvec_get( &bv, pos + 1 ) << 1 );
        goto CHECK_REMAIN;
    }
    
    bitvec_dealloc( &bv );
    
    return LSTATE_TBLREAD_DONE;
}



// MARK: convert lua table to as_record
static as_val *set_tbl2asmap( lua_State *L, const uint32_t len )
{
    as_map *map = (as_map*)as_hashmap_new( len );
    
    if( map )
    {
        as_val *val = NULL;
        const char *name = NULL;
        int rc = 0;
        
        // push space
        lua_pushnil( L );
        // key: -2, val: -1
        while( rc == 0 && lua_next( L, -2 ) )
        {
            name = lua_tostring( L, -2 );
            switch( lua_type( L, -1 ) ){
                case LUA_TSTRING:
                    rc = as_stringmap_set_str( map, name, lua_tostring( L, -1 ) );
                break;
                case LUA_TNUMBER:
                    rc = as_stringmap_set_int64( map, name, lua_tointeger( L, -1 ) );
                break;
                case LUA_TBOOLEAN:
                    rc = as_stringmap_set_int64( map, name, lua_toboolean( L, -1 ) );
                break;
                case LUA_TTABLE:
                    if( !( val = lstate_tbl2asval( L ) ) ){
                        as_map_destroy( map );
                        return NULL;
                    }
                    rc = as_stringmap_set( map, name, val );
                break;
                
                // unsupported value
                default:
                    as_map_destroy( map );
                    lua_pushboolean( L, 0 );
                    lua_pushfstring( L, "unsupported value type %s at field %s",
                                     lua_typename( L, lua_type( L, -1 ) ), name );
                    return NULL;
            }
            lua_pop( L, 1 );
        }
        
        // FIXME: cannot find error code descriptions.
        // check error code
        if( rc != 0 ){
            as_map_destroy( map );
            lua_pushboolean( L, 0 );
            lua_pushstring( L, strerror( errno ) );
            return NULL;
        }
    }
    
    return (as_val*)map;
}


static as_val *set_tbl2asarr( lua_State *L, const uint32_t len )
{
    as_arraylist *list = as_arraylist_new( len, 0 );
    
    if( list )
    {
        as_val *val = NULL;
        uint32_t idx = 0;
        int rc = AS_ARRAYLIST_OK;
        
        // push space
        lua_pushnil( L );
        // key: -2, val: -1
        while( rc == AS_ARRAYLIST_OK && lua_next( L, -2 ) )
        {
            switch( lua_type( L, -1 ) ){
                case LUA_TSTRING:
                    rc = as_arraylist_set_str( list, idx, lua_tostring( L, -1 ) );
                break;
                case LUA_TNUMBER:
                    rc = as_arraylist_set_int64( list, idx, lua_tointeger( L, -1 ) );
                break;
                case LUA_TBOOLEAN:
                    rc = as_arraylist_set_int64( list, idx, lua_toboolean( L, -1 ) );
                break;
                case LUA_TTABLE:
                    if( !( val = lstate_tbl2asval( L ) ) ){
                        as_arraylist_destroy( list );
                        return NULL;
                    }
                    rc = as_arraylist_set( list, idx, val );
                break;
                
                // unsupported data type
                default:
                    as_arraylist_destroy( list );
                    lua_pushboolean( L, 0 );
                    lua_pushfstring( L, "unsupported value type %s at index %u",
                                     lua_typename( L, lua_type( L, -1 ) ), idx );
                    return NULL;
            }
            lua_pop( L, 1 );
            idx++;
        }
        
        // check error code
        switch( rc ){
            case AS_ARRAYLIST_ERR_ALLOC:
                as_arraylist_destroy( list );
                lua_pushboolean( L, 0 );
                lua_pushstring( L, strerror( errno ) );
                return NULL;
            break;
            // does not reach here
            case AS_ARRAYLIST_ERR_MAX:
                as_arraylist_destroy( list );
                lua_pushboolean( L, 0 );
                lua_pushfstring( L, "%d", AS_ARRAYLIST_ERR_MAX );
                return NULL;
            break;
        }
    }
    
    return (as_val*)list;
}


as_val *lstate_tbl2asval( lua_State *L )
{
    size_t len = 0;
    int type = lstate_tablelen( L, &len );
    
    switch( type ){
        case LUA_TTABLE_HASH:
            return set_tbl2asmap( L, (uint32_t)len );
        break;
        case LUA_TTABLE_LIST:
            return set_tbl2asarr( L, (uint32_t)len );
        break;
        
        default:
            lua_pushboolean( L, 0 );
            lua_pushstring( L, "should not be included both of array and hash" );
            return NULL;
    }
}


// 1 = las_ctx_t, 2 = key string, 3 = record table, 4 = ttl
as_record *lstate_tbl2asrec( lua_State *L )
{
    bool rv = true;
    size_t len = 0;
    as_bin_value *bin = NULL;
    const char *name = NULL;
    as_record *rec = NULL;
    
    // check table
    if( lstate_tablelen( L, &len ) != LUA_TTABLE_HASH ){
        lua_pushboolean( L, 0 );
        lua_pushliteral( L, "record must be hash table" );
        return NULL;
    }
    // number of bin limit exceeded
    else if( len >= UINT16_MAX ){
        lua_pushboolean( L, 0 );
        lua_pushliteral( L, LAS_ERR_BIN_LIMIT );
        return NULL;
    }
    // allocate record
    else if( !( rec = as_record_new( (uint16_t)len ) ) ){
        lua_pushboolean( L, 0 );
        lua_pushstring( L, strerror( errno ) );
        return NULL;
    }
    
    // push space
    lua_pushnil( L );
    // key: -2, val: -1
    while( rv && lua_next( L, -2 ) )
    {
        // check name length
        if( !( name = LAS_CHK_BINNAME( L, -2 ) ) ){
            lua_pushboolean( L, 0 );
            lua_pushliteral( L, LAS_ERR_BIN_NAME );
            as_record_destroy( rec );
            return NULL;
        }
        
        switch( lua_type( L, -1 ) ){
            case LUA_TNUMBER:
                rv = as_record_set_int64( rec, name, lua_tointeger( L, -1 ) );
            break;
            case LUA_TBOOLEAN:
                // drop bin if false
                if( !lua_toboolean( L, -1 ) ){
                    rv = as_record_set_nil( rec, name );
                }
            break;
            case LUA_TSTRING:
                rv = as_record_set_str( rec, name, lua_tostring( L, -1 ) );
            break;
            case LUA_TTABLE:
                if( !( bin = (as_bin_value*)lstate_tbl2asval( L ) ) ){
                    as_record_destroy( rec );
                    return NULL;
                }
                rv = as_record_set( rec, name, bin );
            break;
            
            default:
                lua_pushboolean( L, 0 );
                lua_pushfstring( L, "%s = <%s> is unsupported data type",
                                 name, lua_typename( L , lua_type( L, -1 ) ) );
                as_record_destroy( rec );
                return NULL;
        }
        lua_pop( L, 1 );
    }
    
    // push error
    if( !rv ){
        lua_pushboolean( L, 0 );
        lua_pushstring( L, strerror( errno ) );
        as_record_destroy( rec );
        return NULL;
    }

    return rec;
}



// MARK: convert lua table to as_query
static int set_tbl2asqry_orderby( lua_State *L, as_query *qry )
{
    size_t len = 0;
    
    if( !lstate_getfield( L, "orderby", LUA_TTABLE ) ){
        return 0;
    }
    else if( lstate_tablelen( L, &len ) != LUA_TTABLE_HASH ){
        lua_pushnil( L );
        lua_pushliteral( L, "orderby field must be hash table" );
    }
    else if( len > UINT16_MAX ){
        lua_pushnil( L );
        lua_pushliteral( L, LAS_ERR_ORDERBY_LIMIT );
    }
    else if( !as_query_orderby_init( qry, (uint16_t)len ) ){
        lua_pushnil( L );
        lua_pushstring( L, strerror( errno ) );
    }
    else
    {
        const char *bin = NULL;
        as_order order;
        
        lua_pushnil( L );
        while( lua_next( L, -2 ) )
        {
            if( !( bin = LAS_CHK_BINNAME( L, -2 ) ) ){
                lua_pop( L, 2 );
                lua_pushnil( L );
                lua_pushliteral( L, LAS_ERR_BIN_NAME );
                break;
            }
            // check order
            else if( lua_type( L, -1 ) != LUA_TNUMBER ||
                    ( ( order = (as_order)lua_tointeger( L, -1 ) ) != AS_ORDER_ASCENDING &&
                    order != AS_ORDER_DESCENDING ) ) {
                lua_pop( L, 2 );
                lua_pushnil( L );
                lua_pushliteral( L, "order must be ORDER_ASC | ORDER_DESC" );
                return -1;
            }
            
            as_query_orderby( qry, bin, order );
            lua_pop( L, 1 );
        }
        lua_pop( L, 1 );
        
        return 0;
    }
    
    return -1;
}


static int set_tbl2asqry_where_range( lua_State *L, as_query *qry,
                                      const char *bin )
{
    int nval = 0;
    lua_Integer range[2];
    
    lua_pushnil( L );
    while( lua_next( L, -2 ) )
    {
        if( nval > 1 ||
            lua_type( L, -1 ) != LUA_TNUMBER ||
            lua_type( L, -2 ) != LUA_TNUMBER ){
            lua_pushnil( L );
            lua_pushliteral( L, "range value must be { min, max }" );
            return -1;
        }
        
        range[nval] = lua_tointeger( L, -1 );
        nval++;
        lua_pop( L, 1 );
    }
    
    as_query_where( qry, bin, integer_range( range[0], range[1] ) );
    
    return 0;
}

static int set_tbl2asqry_where( lua_State *L, as_query *qry )
{
    size_t len = 0;
    
    if( !lstate_getfield( L, "where", LUA_TTABLE ) ){
        return 0;
    }
    else if( lstate_tablelen( L, &len ) != LUA_TTABLE_HASH ){
        lua_pushnil( L );
        lua_pushliteral( L, "where field must be hash table" );
    }
    else if( len > UINT16_MAX ){
        lua_pushnil( L );
        lua_pushliteral( L, LAS_ERR_ORDERBY_LIMIT );
    }
    else
    {
        const char *bin = NULL;
        
        as_query_where_init( qry, (uint16_t)len );
        lua_pushnil( L );
        while( lua_next( L, -2 ) )
        {
            if( !( bin = LAS_CHK_BINNAME( L, -2 ) ) ){
                lua_pop( L, 2 );
                lua_pushnil( L );
                lua_pushliteral( L, LAS_ERR_BIN_NAME );
                break;
            }
            // check order
            switch( lua_type( L, -1 ) )
            {
                case LUA_TNUMBER:
                    as_query_where( qry, bin,
                                    integer_equals( lua_tointeger( L, -1 ) ) );
                break;
                case LUA_TSTRING:
                    as_query_where( qry, bin,
                                    string_equals( lua_tostring( L, -1 ) ) );
                break;
                case LUA_TTABLE:
                    if( set_tbl2asqry_where_range( L, qry, bin ) != 0 ){
                        return -1;
                    }
                break;
                
                default:
                    lua_pop( L, 2 );
                    lua_pushnil( L );
                    lua_pushfstring( L, "where value %s not supported",
                                     lua_typename( L, lua_type( L, -1 ) ) );
                    return -1;
            }
            lua_pop( L, 1 );
        }
        lua_pop( L, 1 );
        
        return 0;
    }
    
    return -1;
}


static int set_tbl2asqry_select( lua_State *L, as_query *qry )
{
    size_t len = 0;
    
    if( !lstate_getfield( L, "select", LUA_TTABLE ) ){
        return 0;
    }
    else if( lstate_tablelen( L, &len ) != LUA_TTABLE_LIST ){
        lua_pushnil( L );
        lua_pushliteral( L, "select field must be array table" );
    }
    else if( len > UINT16_MAX ){
        lua_pushnil( L );
        lua_pushliteral( L, LAS_ERR_BIN_LIMIT );
    }
    else if( !as_query_select_init( qry, (uint16_t)len ) ){
        lua_pushnil( L );
        lua_pushstring( L, strerror( errno ) );
    }
    else
    {
        const char *bin = NULL;
        
        lua_pushnil( L );
        while( lua_next( L, -2 ) )
        {
            if( !( bin = LAS_CHK_BINNAME( L, -1 ) ) ){
                lua_pop( L, 2 );
                lua_pushnil( L );
                lua_pushliteral( L, LAS_ERR_BIN_NAME );
                return -1;
            }
            
            as_query_select( qry, bin );
            lua_pop( L, 1 );
        }
        lua_pop( L, 1 );
        
        return 0;
    }
    
    return -1;
}

as_query *lstate_tbl2asqry( lua_State *L, const char *ns, const char *set )
{
    as_query *qry = NULL;
    
    // check argument
    luaL_checktype( L, 2, LUA_TTABLE );
    if( !( qry = as_query_new( ns, set ) ) ){
        lua_pushnil( L );
        lua_pushstring( L, strerror( errno ) );
    }
    else
    {
        lua_pushvalue( L, 2 );
        if( set_tbl2asqry_select( L, qry ) != 0 ||
            set_tbl2asqry_where( L, qry ) != 0 ||
            set_tbl2asqry_orderby( L, qry ) != 0 ){
            as_query_destroy( qry );
            return NULL;
        }
        lua_pop( L, 1 );
    }
    
    return qry;
}


// MARK: convert record to lua table
static void set_kval2lua( lua_State *L, const char *name, as_val *val );
static void set_ival2lua( lua_State *L, int idx, as_val *val );

static void set_asmap2tbl( lua_State *L, as_hashmap *map )
{
    as_pair *val = NULL;
    as_hashmap_iterator it;
    
    lua_createtable( L, 0, as_hashmap_size( map ) );
    as_hashmap_iterator_init( &it, map );
    while( as_hashmap_iterator_has_next( &it ) ){
        val = (as_pair*)as_hashmap_iterator_next( &it );
        set_kval2lua( L, as_string_get( (const as_string*)as_pair_1( val ) ),
                      as_pair_2( val ) );
    }
    as_hashmap_iterator_destroy( &it );
}

static void set_asarr2tbl( lua_State *L, as_arraylist *arr )
{
    as_val *val = NULL;
    as_arraylist_iterator it;
    int i = 1;
    
    lua_createtable( L, as_arraylist_size( arr ), 0 );
    as_arraylist_iterator_init( &it, arr );
    while( as_arraylist_iterator_has_next( &it ) ){
        val = (as_val*)as_arraylist_iterator_next( &it );
        set_ival2lua( L, i++, val );
    }
    as_arraylist_iterator_destroy( &it );
}


int lstate_asval2lua( lua_State *L, as_val *val )
{
    switch( as_val_type( val ) ){
        case AS_BOOLEAN:
            lua_pushboolean( L, as_boolean_get( (as_boolean*)val ) );
        break;
        case AS_INTEGER:
            lua_pushinteger( L, as_integer_get( (as_integer*)val ) );
        break;
        case AS_STRING:
            lua_pushstring( L, as_string_get( (as_string*)val ) );
        break;
        case AS_BYTES:
            lua_pushlstring( L, (char*)as_bytes_get( (as_bytes*)val ),
                             as_bytes_size( (as_bytes*)val ) );
        break;
        case AS_LIST:
            set_asarr2tbl( L, (as_arraylist*)val );
        break;
        case AS_MAP:
            set_asmap2tbl( L, (as_hashmap*)val );
        break;
        case AS_REC:
            lstate_asrec2tbl( L, (as_record*)val );
        break;
        
        // other: unsupported data types
        // ignore nil value
        // AS_NIL
        default:
            return 0;
    }
    return 1;
}

static void set_kval2lua( lua_State *L, const char *name, as_val *val )
{
    lua_pushstring( L, name );
    if( lstate_asval2lua( L, val ) ){
        lua_rawset( L, -3 );
    }
    else {
        lua_pop( L, 1 );
    }
}


static void set_ival2lua( lua_State *L, int idx, as_val *val )
{
    lua_pushnumber( L, idx );
    if( lstate_asval2lua( L, val ) ){
        lua_rawset( L, -3 );
    }
    else {
        lua_pop( L, 1 );
    }
}


uint16_t lstate_asrec2tbl( lua_State *L, as_record *rec )
{
    as_record_iterator it;
    as_bin *bin = NULL;
    uint16_t nbin = as_record_numbins( rec );
    
    lua_createtable( L, 0, nbin );
    if( nbin )
    {
        as_record_iterator_init( &it, rec );
        while( as_record_iterator_has_next( &it ) ){
            bin = as_record_iterator_next( &it );
            set_kval2lua( L, as_bin_get_name( bin ), (as_val*)as_bin_get_value( bin ) );
        }
        as_record_iterator_destroy( &it );
    }
    
    return nbin;
}



