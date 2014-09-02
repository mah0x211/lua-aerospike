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
 *  las_util.h
 *  lua-aerospike
 *
 *  Created by Masatoshi Teruya on 2014/08/25.
 *
 */

#ifndef lua_aerospike_las_util_h
#define lua_aerospike_las_util_h

#include "las.h"

// memory alloc/dealloc
#define palloc(t)       (t*)malloc( sizeof(t) )
#define pnalloc(n,t)    (t*)malloc( (n) * sizeof(t) )
#define pcalloc(n,t)    (t*)calloc( n, sizeof(t) )
#define prealloc(n,t,p) (t*)realloc( p, (n) * sizeof(t) )
#define pdealloc(p)     free((void*)p)


// buf size must be larger than len*2
static inline void digest2hex( uint8_t *buf, uint8_t digest[], size_t len )
{
    static const char HEXCHR[] = "0123456789abcdef";
    uint8_t *pos = buf;
    size_t i = 0;
    
    for(; i < len; i++ ){
        *pos++ = HEXCHR[digest[i] >> 4];
        *pos++ = HEXCHR[digest[i] & 0xf];
    }
}


// helper macros
#define lstate_setmetatable(L,tname) do { \
    luaL_getmetatable( L, tname ); \
    lua_setmetatable( L, -2 ); \
}while(0)


#define lstate_num2tbl(L,k,v) do{ \
    lua_pushstring(L,k); \
    lua_pushnumber(L,v); \
    lua_rawset(L,-3); \
}while(0)

#define lstate_num2arr(L,i,v) do{ \
    lua_pushnumber(L,i); \
    lua_pushnumber(L,v); \
    lua_rawset(L,-3); \
}while(0)


#define lstate_bool2tbl(L,k,v) do{ \
    lua_pushstring(L,k); \
    lua_pushboolean(L,v); \
    lua_rawset(L,-3); \
}while(0)

#define lstate_bool2arr(L,i,v) do{ \
    lua_pushnumber(L,i); \
    lua_pushboolean(L,v); \
    lua_rawset(L,-3); \
}while(0)


#define lstate_str2tbl(L,k,v) do{ \
    lua_pushstring(L,k); \
    lua_pushstring(L,v); \
    lua_rawset(L,-3); \
}while(0)

#define lstate_str2arr(L,i,v) do{ \
    lua_pushnumber(L,i); \
    lua_pushstring(L,v); \
    lua_rawset(L,-3); \
}while(0)


#define lstate_strn2tbl(L,k,v,n) do{ \
    lua_pushstring(L,k); \
    lua_pushlstring(L,v,n); \
    lua_rawset(L,-3); \
}while(0)

#define lstate_strn2arr(L,i,v,n) do{ \
    lua_pushnumber(L,i); \
    lua_pushlstring(L,v,n); \
    lua_rawset(L,-3); \
}while(0)


#define lstate_fn2tbl(L,k,v) do{ \
    lua_pushstring(L,k); \
    lua_pushcfunction(L,v); \
    lua_rawset(L,-3); \
}while(0)


#define lstate_checklstring(L,idx,len) \
    (luaL_checktype(L,idx,LUA_TSTRING),lua_tolstring(L,idx,len))

#define lstate_checkstring(L,idx) \
    (luaL_checktype(L,idx,LUA_TSTRING),lua_tostring(L,idx))

#define lstate_checkinteger(L,idx) \
    (luaL_checktype(L,idx,LUA_TNUMBER),lua_tointeger(L,idx))

#define lstate_checknumber(L,idx) \
    (luaL_checktype(L,idx,LUA_TNUMBER),lua_tonumber(L,idx))


#define lstate_ref(L,idx) \
    (lua_pushvalue(L,idx),luaL_ref( L, LUA_REGISTRYINDEX ))

#define lstate_pushref(L,ref) \
    lua_rawgeti( L, LUA_REGISTRYINDEX, ref )

#define lstate_unref(L,ref) \
    luaL_unref( L, LUA_REGISTRYINDEX, (ref) )

#define lstate_isref(ref)   ((ref) > 0)


// table read(traverse)
#define LSTATE_TBLREAD_ERR  -1
#define LSTATE_TBLREAD_DONE 0
#define LSTATE_TBLREAD_NEXT 1
#define LSTATE_TBLREAD_ABRT 3

typedef int (*lstate_tblread_cb)( lua_State *, int, int, int, void* );

int lstate_tblread( lua_State *L, lstate_tblread_cb callback, void *udata );


// module definition register
typedef void (*lstate_definemt_cb)( lua_State* );

static inline void lstate_definemt( lua_State *L, const char *tname,
                                    struct luaL_Reg mmethod[],
                                    struct luaL_Reg method[],
                                    lstate_definemt_cb callback )
{
    int i;
    
    // create table __metatable
    luaL_newmetatable( L, tname );
    // metamethods
    for( i = 0; mmethod[i].name; i++ ){
        lstate_fn2tbl( L, mmethod[i].name, mmethod[i].func );
    }
    // methods
    lua_pushstring( L, "__index" );
    lua_newtable( L );
    for( i = 0; method[i].name; i++ ){
        lstate_fn2tbl( L, method[i].name, method[i].func );
    }
    if( callback ){
        callback( L );
    }
    lua_rawset( L, -3 );
    lua_pop( L, 1 );
}


#define LUA_TTABLE_LIST     0x1
#define LUA_TTABLE_HASH     0x2
#define LUA_TTABLE_HASHLIST 0x3

/** 
 * get the table type and length. 
 * must be set table at the stack top before calling this function.
 */
static inline int lstate_tablelen( lua_State *L, size_t *len )
{
    size_t nitem = 0;
    int type = 0;
    
    // push table
    lua_pushnil( L );
    // key: -2, val: -1
    while( lua_next( L, -2 ) )
    {
        nitem++;
        switch( lua_type( L, -2 ) ){
            case LUA_TNUMBER:
                type |= LUA_TTABLE_LIST;
            break;
            case LUA_TSTRING:
                type |= LUA_TTABLE_HASH;
            break;
        }
        lua_pop( L, 1 );
    }
    
    *len = nitem;
    return type;
}


as_record *lstate_tbl2asrec( lua_State *L );
as_val *lstate_tbl2asval( lua_State *L );
void lstate_asval2lua( lua_State *L, as_val *val );
uint16_t lstate_asrec2tbl( lua_State *L, as_record *rec );


// MARK: status to error string
#define LAS_SET_ASERR(err,__enum) do { \
    err->code = __enum; \
    strcpy(err->message, #__enum); \
}while(0)

static inline void las_ctx_aserror( as_error *err, as_status rc )
{
    switch( rc )
    {
        /***************************************************************************
         *	SUCCESS (all < 100)
         **************************************************************************/

        /**
         *	Generic success.
         */
        case AEROSPIKE_OK:
            LAS_SET_ASERR( err, AEROSPIKE_OK );
        break;
        /***************************************************************************
         *	ERRORS (all < 100)
         **************************************************************************/
        case AEROSPIKE_QUERY_END:
            LAS_SET_ASERR( err, AEROSPIKE_QUERY_END );
        break;
        case AEROSPIKE_SECURITY_NOT_SUPPORTED:
            LAS_SET_ASERR( err, AEROSPIKE_SECURITY_NOT_SUPPORTED );
        break;
        case AEROSPIKE_SECURITY_NOT_ENABLED:
            LAS_SET_ASERR( err, AEROSPIKE_SECURITY_NOT_ENABLED );
        break;
        case AEROSPIKE_SECURITY_SCHEME_NOT_SUPPORTED:
            LAS_SET_ASERR( err, AEROSPIKE_SECURITY_SCHEME_NOT_SUPPORTED );
        break;
        case AEROSPIKE_INVALID_COMMAND:
            LAS_SET_ASERR( err, AEROSPIKE_INVALID_COMMAND );
        break;
        case AEROSPIKE_INVALID_FIELD:
            LAS_SET_ASERR( err, AEROSPIKE_INVALID_FIELD );
        break;
        case AEROSPIKE_ILLEGAL_STATE:
            LAS_SET_ASERR( err, AEROSPIKE_ILLEGAL_STATE );
        break;
        case AEROSPIKE_INVALID_USER:
            LAS_SET_ASERR( err, AEROSPIKE_INVALID_USER );
        break;
        case AEROSPIKE_USER_ALREADY_EXISTS:
            LAS_SET_ASERR( err, AEROSPIKE_USER_ALREADY_EXISTS );
        break;
        case AEROSPIKE_INVALID_PASSWORD:
            LAS_SET_ASERR( err, AEROSPIKE_INVALID_PASSWORD );
        break;
        case AEROSPIKE_EXPIRED_PASSWORD:
            LAS_SET_ASERR( err, AEROSPIKE_EXPIRED_PASSWORD );
        break;
        case AEROSPIKE_FORBIDDEN_PASSWORD:
            LAS_SET_ASERR( err, AEROSPIKE_FORBIDDEN_PASSWORD );
        break;
        case AEROSPIKE_INVALID_CREDENTIAL:
            LAS_SET_ASERR( err, AEROSPIKE_INVALID_CREDENTIAL );
        break;
        case AEROSPIKE_INVALID_ROLE:
            LAS_SET_ASERR( err, AEROSPIKE_INVALID_ROLE );
        break;
        case AEROSPIKE_INVALID_PRIVILEGE:
            LAS_SET_ASERR( err, AEROSPIKE_INVALID_PRIVILEGE );
        break;
        case AEROSPIKE_NOT_AUTHENTICATED:
            LAS_SET_ASERR( err, AEROSPIKE_NOT_AUTHENTICATED );
        break;
        case AEROSPIKE_ROLE_VIOLATION:
            LAS_SET_ASERR( err, AEROSPIKE_ROLE_VIOLATION );
        break;

        /***************************************************************************
         *	ERRORS (all >= 100)
         **************************************************************************/

        /**
         *	Generic error.
         */
        case AEROSPIKE_ERR:
            LAS_SET_ASERR( err, AEROSPIKE_ERR );
        break;

        /***************************************************************************
         *	CLIENT API USAGE
         **************************************************************************/

        /**
         *	Generic client API usage error.
         */
        case AEROSPIKE_ERR_CLIENT:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_CLIENT );
        break;

        /**
         *	Invalid client API parameter.
         */
        case AEROSPIKE_ERR_PARAM:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_PARAM );
        break;

        /***************************************************************************
         *	CLUSTER DISCOVERY & CONNECTION
         **************************************************************************/

        /**
         *	Generic cluster discovery & connection error.
         */
        case AEROSPIKE_ERR_CLUSTER:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_CLUSTER );
        break;

        /***************************************************************************
         *	INCOMPLETE REQUESTS (i.e. NOT from server-returned error codes)
         **************************************************************************/

        /**
         *	Request timed out.
         */
        case AEROSPIKE_ERR_TIMEOUT:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_TIMEOUT );
        break;

        /**
         *	Request randomly dropped by client for throttling.
         *	@warning	Not yet supported.
         */
        case AEROSPIKE_ERR_THROTTLED:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_THROTTLED );
        break;

        /***************************************************************************
         *	COMPLETED REQUESTS (all >= 500, from server-returned error codes)
         **************************************************************************/

        /**
         *	Generic error returned by server.
         */
        case AEROSPIKE_ERR_SERVER:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_SERVER );
        break;

        /**
         *	Request protocol invalid, or invalid protocol field.
         */
        case AEROSPIKE_ERR_REQUEST_INVALID:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_REQUEST_INVALID );
        break;

        /**
         *	Namespace in request not found on server.
         *	@warning	Not yet supported, shows as AEROSPIKE_ERR_REQUEST_INVALID.
         */
        case AEROSPIKE_ERR_NAMESPACE_NOT_FOUND:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_NAMESPACE_NOT_FOUND );
        break;

        /**
         *	The server node is running out of memory and/or storage device space
         *	reserved for the specified namespace.
         */
        case AEROSPIKE_ERR_SERVER_FULL:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_SERVER_FULL );
        break;

        /**
         *	A cluster state change occurred during the request. This may also be
         *	returned by scan operations with the fail_on_cluster_change flag set.
         */
        case AEROSPIKE_ERR_CLUSTER_CHANGE:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_CLUSTER_CHANGE );
        break;

        /**
         *	Sometimes our doc, or our customers wishes, get ahead of us.  We may have
         *	processed something that the server is not ready for (unsupported feature).
         */
        case AEROSPIKE_ERR_UNSUPPORTED_FEATURE:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_UNSUPPORTED_FEATURE );
        break;

        /**
         *	The server node's storage device(s) can't keep up with the write load.
         */
        case AEROSPIKE_ERR_DEVICE_OVERLOAD:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_DEVICE_OVERLOAD );
        break;

        /***************************************************************************
         *	RECORD-SPECIFIC
         **************************************************************************/

        /**
         *	Generic record error.
         */
        case AEROSPIKE_ERR_RECORD:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_RECORD );
        break;

        /**
         *	Too may concurrent requests for one record - a "hot-key" situation.
         */
        case AEROSPIKE_ERR_RECORD_BUSY:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_RECORD_BUSY );
        break;

        /**
         *	Record does not exist in database. May be returned by read, or write
         *	with policy AS_POLICY_EXISTS_UPDATE.
         *	@warning	AS_POLICY_EXISTS_UPDATE not yet supported.
         */
        case AEROSPIKE_ERR_RECORD_NOT_FOUND:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_RECORD_NOT_FOUND );
        break;

        /**
         *	Record already exists. May be returned by write with policy
         *	AS_POLICY_EXISTS_CREATE.
         */
        case AEROSPIKE_ERR_RECORD_EXISTS:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_RECORD_EXISTS );
        break;

        /**
         *	Generation of record in database does not satisfy write policy.
         */
        case AEROSPIKE_ERR_RECORD_GENERATION:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_RECORD_GENERATION );
        break;

        /**
         *	Record being (re-)written can't fit in a storage write block.
         */
        case AEROSPIKE_ERR_RECORD_TOO_BIG:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_RECORD_TOO_BIG );
        break;

        /**
         *	Bin modification operation can't be done on an existing bin due to its
         *	value type.
         */
        case AEROSPIKE_ERR_BIN_INCOMPATIBLE_TYPE:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_BIN_INCOMPATIBLE_TYPE );
        break;

        /**
         *	Record key sent with transaction did not match key stored on server.
         */
        case AEROSPIKE_ERR_RECORD_KEY_MISMATCH:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_RECORD_KEY_MISMATCH );
        break;

        /***************************************************************************
         * XDR-SPECIFIC
         **************************************************************************/

        /**
         *	XDR is not available for the cluster.
         */
        case AEROSPIKE_ERR_NO_XDR:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_NO_XDR );
        break;

        /***************************************************************************
         *	SCAN OPERATIONS
         **************************************************************************/

        /**
         *	Generic scan error.
         */
        case AEROSPIKE_ERR_SCAN:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_SCAN );
        break;

        /**
         *	Scan aborted by user.
         */
        case AEROSPIKE_ERR_SCAN_ABORTED:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_SCAN_ABORTED );
        break;

        /***************************************************************************
         *	QUERY OPERATIONS
         **************************************************************************/

        /**
         *	Generic query error.
         */
        case AEROSPIKE_ERR_QUERY:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_QUERY );
        break;

        /**
         *	Query was aborted.
         */
        case AEROSPIKE_ERR_QUERY_ABORTED:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_QUERY_ABORTED );
        break;

        /**
         *	Query processing queue is full.
         */
        case AEROSPIKE_ERR_QUERY_QUEUE_FULL:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_QUERY_QUEUE_FULL );
        break;

        /***************************************************************************
         *	SECONDARY INDEX OPERATIONS
         **************************************************************************/

        /**
         *	Generic secondary index error.
         */
        case AEROSPIKE_ERR_INDEX:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_INDEX );
        break;

        /**
         *	Index is out of memory
         */
        case AEROSPIKE_ERR_INDEX_OOM:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_INDEX_OOM );
        break;

        /**
         *	Index not found
         */
        case AEROSPIKE_ERR_INDEX_NOT_FOUND:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_INDEX_NOT_FOUND );
        break;

        /**
         *	Index found.
         */
        case AEROSPIKE_ERR_INDEX_FOUND:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_INDEX_FOUND );
        break;

        /**
         *	Unable to read the index.
         */
        case AEROSPIKE_ERR_INDEX_NOT_READABLE:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_INDEX_NOT_READABLE );
        break;

        /**
         *	Index name is too long.
         */
        case AEROSPIKE_ERR_INDEX_NAME_MAXLEN:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_INDEX_NAME_MAXLEN );
        break;

        /**
         *	System already has maximum allowed indices.
         */
        case AEROSPIKE_ERR_INDEX_MAXCOUNT:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_INDEX_MAXCOUNT );
        break;

        /***************************************************************************
         *	UDF OPERATIONS
         **************************************************************************/

        /**
         *	Generic UDF error.
         */
        case AEROSPIKE_ERR_UDF:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_UDF );
        break;

        /**
         *	UDF does not exist.
         */
        case AEROSPIKE_ERR_UDF_NOT_FOUND:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_UDF_NOT_FOUND );
        break;
        /**
         *	LUA file does not exist.
         */
        case AEROSPIKE_ERR_LUA_FILE_NOT_FOUND:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LUA_FILE_NOT_FOUND );
        break;

        /***************************************************************************
         *	Large Data Type (LDT) OPERATIONS
         **************************************************************************/

        /** Internal LDT error. */
        case AEROSPIKE_ERR_LDT_INTERNAL:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_INTERNAL );
        break;

        /** LDT item not found */
        case AEROSPIKE_ERR_LDT_NOT_FOUND:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_NOT_FOUND );
        break;

        /** Unique key violation: Duplicated item inserted when 'unique key" was set.*/
        case AEROSPIKE_ERR_LDT_UNIQUE_KEY:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_UNIQUE_KEY );
        break;

        /** General error during insert operation. */
        case AEROSPIKE_ERR_LDT_INSERT:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_INSERT );
        break;

        /** General error during search operation. */
        case AEROSPIKE_ERR_LDT_SEARCH:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_SEARCH );
        break;

        /** General error during delete operation. */
        case AEROSPIKE_ERR_LDT_DELETE:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_DELETE );
        break;


        /** General input parameter error. */
        case AEROSPIKE_ERR_LDT_INPUT_PARM:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_INPUT_PARM );
        break;

        // -------------------------------------------------

        /** LDT Type mismatch for this bin.  */
        case AEROSPIKE_ERR_LDT_TYPE_MISMATCH:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_TYPE_MISMATCH );
        break;

        /** The supplied LDT bin name is null. */
        case AEROSPIKE_ERR_LDT_NULL_BIN_NAME:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_NULL_BIN_NAME );
        break;

        /** The supplied LDT bin name must be a string. */
        case AEROSPIKE_ERR_LDT_BIN_NAME_NOT_STRING:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_BIN_NAME_NOT_STRING );
        break;

        /** The supplied LDT bin name exceeded the 14 char limit. */
        case AEROSPIKE_ERR_LDT_BIN_NAME_TOO_LONG:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_BIN_NAME_TOO_LONG );
        break;

        /** Internal Error: too many open records at one time. */
        case AEROSPIKE_ERR_LDT_TOO_MANY_OPEN_SUBRECS:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_TOO_MANY_OPEN_SUBRECS );
        break;

        /** Internal Error: Top Record not found.  */
        case AEROSPIKE_ERR_LDT_TOP_REC_NOT_FOUND:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_TOP_REC_NOT_FOUND );
        break;

        /** Internal Error: Sub Record not found. */
        case AEROSPIKE_ERR_LDT_SUB_REC_NOT_FOUND:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_SUB_REC_NOT_FOUND );
        break;

        /** LDT Bin does not exist. */
        case AEROSPIKE_ERR_LDT_BIN_DOES_NOT_EXIST:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_BIN_DOES_NOT_EXIST );
        break;

        /** Collision: LDT Bin already exists. */
        case AEROSPIKE_ERR_LDT_BIN_ALREADY_EXISTS:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_BIN_ALREADY_EXISTS );
        break;

        /** LDT control structures in the Top Record are damaged. Cannot proceed. */
        case AEROSPIKE_ERR_LDT_BIN_DAMAGED:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_BIN_DAMAGED );
        break;

        // -------------------------------------------------

        /** Internal Error: LDT Subrecord pool is damaged. */
        case AEROSPIKE_ERR_LDT_SUBREC_POOL_DAMAGED:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_SUBREC_POOL_DAMAGED );
        break;

        /** LDT control structures in the Sub Record are damaged. Cannot proceed. */
        case AEROSPIKE_ERR_LDT_SUBREC_DAMAGED:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_SUBREC_DAMAGED );
        break;

        /** Error encountered while opening a Sub Record. */
        case AEROSPIKE_ERR_LDT_SUBREC_OPEN:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_SUBREC_OPEN );
        break;

        /** Error encountered while updating a Sub Record. */
        case AEROSPIKE_ERR_LDT_SUBREC_UPDATE:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_SUBREC_UPDATE );
        break;

        /** Error encountered while creating a Sub Record. */
        case AEROSPIKE_ERR_LDT_SUBREC_CREATE:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_SUBREC_CREATE );
        break;

        /** Error encountered while deleting a Sub Record. */
        case AEROSPIKE_ERR_LDT_SUBREC_DELETE:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_SUBREC_DELETE );
        break;

        /** Error encountered while closing a Sub Record. */
        case AEROSPIKE_ERR_LDT_SUBREC_CLOSE:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_SUBREC_CLOSE );
        break;

        /** Error encountered while updating a TOP Record. */
        case AEROSPIKE_ERR_LDT_TOPREC_UPDATE:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_TOPREC_UPDATE );
        break;

        /** Error encountered while creating a TOP Record. */
        case AEROSPIKE_ERR_LDT_TOPREC_CREATE:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_TOPREC_CREATE );
        break;

        // -------------------------------------------------

        /** The filter function name was invalid. */
        case AEROSPIKE_ERR_LDT_FILTER_FUNCTION_BAD:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_FILTER_FUNCTION_BAD );
        break;

        /** The filter function was not found. */
        case AEROSPIKE_ERR_LDT_FILTER_FUNCTION_NOT_FOUND:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_FILTER_FUNCTION_NOT_FOUND );
        break;

        /** The function to extract the Unique Value from a complex object was invalid. */
        case AEROSPIKE_ERR_LDT_KEY_FUNCTION_BAD:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_KEY_FUNCTION_BAD );
        break;

        /** The function to extract the Unique Value from a complex object was not found. */
        case AEROSPIKE_ERR_LDT_KEY_FUNCTION_NOT_FOUND:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_KEY_FUNCTION_NOT_FOUND );
        break;

        /** The function to transform an object into a binary form was invalid. */
        case AEROSPIKE_ERR_LDT_TRANS_FUNCTION_BAD:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_TRANS_FUNCTION_BAD );
        break;

        /** The function to transform an object into a binary form was not found. */
        case AEROSPIKE_ERR_LDT_TRANS_FUNCTION_NOT_FOUND:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_TRANS_FUNCTION_NOT_FOUND );
        break;

        /** The function to untransform an object from binary form to live form was invalid. */
        case AEROSPIKE_ERR_LDT_UNTRANS_FUNCTION_BAD:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_UNTRANS_FUNCTION_BAD );
        break;

        /** The function to untransform an object from binary form to live form not found. */
        case AEROSPIKE_ERR_LDT_UNTRANS_FUNCTION_NOT_FOUND:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_UNTRANS_FUNCTION_NOT_FOUND );
        break;

        /** The UDF user module name for LDT Overrides was invalid */
        case AEROSPIKE_ERR_LDT_USER_MODULE_BAD:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_USER_MODULE_BAD );
        break;

        /** The UDF user module name for LDT Overrides was not found */
        case AEROSPIKE_ERR_LDT_USER_MODULE_NOT_FOUND:
            LAS_SET_ASERR( err, AEROSPIKE_ERR_LDT_USER_MODULE_NOT_FOUND );
        break;
    }
}



#endif
