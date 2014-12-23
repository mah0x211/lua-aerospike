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
 *  las_deps.h
 *  lua-aerospike
 *
 *  Created by Masatoshi Teruya on 2014/09/02.
 *
 */

#ifndef lua_aerospike_las_deps_h
#define lua_aerospike_las_deps_h

#include <aerospike/aerospike.h>
#include <aerospike/aerospike_key.h>
#include <aerospike/aerospike_batch.h>
#include <aerospike/aerospike_scan.h>
#include <aerospike/aerospike_info.h>
#include <aerospike/aerospike_index.h>
#include <aerospike/aerospike_udf.h>
#include <aerospike/aerospike_query.h>

// record
#include <aerospike/as_record.h>
#include <aerospike/as_record_iterator.h>
// lists
#include <aerospike/as_list.h>
#include <aerospike/as_iterator.h>
#include <aerospike/as_arraylist.h>
#include <aerospike/as_arraylist_iterator.h>
// maps
#include <aerospike/as_hashmap.h>
#include <aerospike/as_stringmap.h>
#include <aerospike/as_pair.h>
#include <aerospike/as_hashmap_iterator.h>
// query
#include <aerospike/as_query.h>
// primitives
#include <aerospike/as_node.h>
#include <aerospike/as_boolean.h>
#include <aerospike/as_status.h>
#include <aerospike/as_error.h>
#include <aerospike/as_key.h>
#include <aerospike/as_bin.h>
#include <aerospike/as_nil.h>


#define STRINGIZE(A) #A

#define LAS_ERR_NAMESPACE \
    "namespace length must be less than " STRINGIZE(AS_NAMESPACE_MAX_SIZE)

#define LAS_ERR_SET \
    "set length must be less than " STRINGIZE(AS_SET_MAX_SIZE)


#define LAS_ERR_RECORD_TYPE \
    "record must be type of table"

#define LAS_ERR_BIN_NAME \
    "bin name must be type of string and length must be 1 to " STRINGIZE(AS_BIN_NAME_MAX_SIZE)

#define LAS_ERR_BIN_LIMIT \
    "number of bin limit(" STRINGIZE(UINT16_MAX) ") exceeded"

#define LAS_ERR_TTL_RANGE \
    "ttl must be 0 to " STRINGIZE(UINT32_MAX)

#define LAS_ERR_ORDERBY_LIMIT \
    "number of orderby limit(" STRINGIZE(UINT16_MAX) ") exceeded"

// UDF arguments
#define LAS_ERR_UDF_MODULE \
    "UDF module name must be string and length less than " STRINGIZE(AS_UDF_MODULE_MAX_LEN)

#define LAS_ERR_UDF_FUNCTION \
    "UDF function name must be string and length less than " STRINGIZE(AS_UDF_FUNCTION_MAX_LEN)

#define LAS_ERR_UDF_ARGUMENT \
    "UDF argument does not support "


// scan option errors
#define LAS_ERR_SCANOPT_PRIORITY \
    "opt.priority must be SCAN_PRIORITY_<AUTO|LOW|MEDIUM|HIGH>"

#define LAS_ERR_SCANOPT_PERCENT \
    "opt.percent must be 1 to 100(default)"

#define LAS_ERR_SCANOPT_CONCURRENT \
    "opt.concurrent must be type of boolean"

#define LAS_ERR_SCANOPT_APPLY \
    "opt.apply must be type of table"


static inline const char *LAS_CHK_LBINNAME( lua_State *L, int idx, size_t *len )
{
    const char *bin = NULL;
    
    if( lua_type( L, idx ) == LUA_TSTRING )
    {
        bin = lua_tolstring( L, idx, len );
        if( !*len || *len > AS_BIN_NAME_MAX_LEN ){
            return NULL;
        }
    }
    
    return bin;
}

#define LAS_CHK_BINNAME(L,idx) ({ \
    size_t _len = 0; \
    LAS_CHK_LBINNAME( L, idx, &_len ); \
})




// MARK: status to error string
#define LAS_SET_ASERR(err,__enum) do { \
    err->code = __enum; \
    strcpy(err->message, #__enum); \
}while(0)

static inline void LAS_SET_ASERROR( as_error *err, as_status rc )
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
