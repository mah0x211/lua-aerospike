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
 *  las_ctx.c
 *  lua-aerospike
 *
 *  Created by Masatoshi Teruya on 2014/08/25.
 *
 */

#include "las_ctx.h"
#include "las_record.h"


static inline las_ctx_t *get_context( lua_State *L, las_conn_t **conn )
{
    las_ctx_t *ctx = luaL_checkudata( L, 1, LAS_CONTEXT_MT );
    
    lstate_pushref( L, ctx->ref_conn );
    *conn = lua_touserdata( L, -1 );
    lua_pop( L, 1 );
    
    return ctx;
}


// MARK: key operations
typedef struct {
    aerospike *as;
    void *policy;
    as_key *key;
    as_record *rec;
} las_key_t;


static inline int las_key_init( lua_State *L, las_key_t *lkey, int nbins,
                                las_ctx_t *ctx )
{
    const char *pk = lstate_checkstring( L, 2 );
    
    if( !( lkey->key = as_key_new( ctx->ns, ctx->set, pk ) ) ){
        return -1;
    }
    // init record
    if( nbins < 0 ){
        lkey->rec = NULL;
    }
    else if( !( lkey->rec = as_record_new( (uint16_t)nbins ) ) ){
        as_key_destroy( lkey->key );
        return -1;
    }
    
    return 0;
}

#define las_key_init_prepare( L, lkey, nbins, type ) ({ \
    las_conn_t *_conn = NULL; \
    las_ctx_t *_ctx = get_context( L, &_conn ); \
    (lkey)->as = _conn->as; \
    (lkey)->policy = (void*)&_ctx->policies.type; \
    las_key_init( L, (lkey), nbins, _ctx ); \
})

#define las_key_write_init(L,lkey)      las_key_init_prepare(L,lkey,-1,write)
#define las_key_read_init(L,lkey)       las_key_init_prepare(L,lkey,0,read)
#define las_key_remove_init(L,lkey)     las_key_init_prepare(L,lkey,0,remove)

#define las_key_dispose( lkey ) do { \
    as_record_destroy( (lkey)->rec ); \
    as_key_destroy( (lkey)->key ); \
}while(0)


static int put_lua( lua_State *L )
{
    int rv = 1;
    as_record rec;
    las_key_t lkey = { .rec = &rec };
    as_error err;
    lua_Integer ttl = -1;
    
    if( las_key_write_init( L, &lkey ) != 0 ){
        lua_pushboolean( L, 0 );
        lua_pushstring( L, strerror( errno ) );
        return 2;
    }
    // check record
    else if( lua_type( L, 3 ) != LUA_TTABLE ){
        lua_pushboolean( L, 0 );
        lua_pushstring( L, "record must be type of table" );
        return 2;
    }
    // check ttl
    else if( lua_gettop( L ) > 3 )
    {
        ttl = lstate_checkinteger( L, 4 );
        if( lua_type( L, 4 ) != LUA_TNUMBER ||
            ( ttl = lua_tointeger( L, 4 ) ) < -1 || ttl > UINT32_MAX ){
            lua_pushboolean( L, 0 );
            lua_pushfstring( L, "ttl must be -1 to %d", UINT32_MAX );
            return 2;
        }
        lua_settop( L, 3 );
    }
    
    // read table
    if( !( lkey.rec = lstate_tbl2asrec( L ) ) ){
        las_key_dispose( &lkey );
        return 2;
    }
    // set ttl
    lkey.rec->ttl = (uint32_t)ttl;
    switch( aerospike_key_put( lkey.as, &err, lkey.policy, lkey.key, lkey.rec ) ){
        case AEROSPIKE_OK:
            lua_pushboolean( L, 1 );
        break;
        
        // got error
        default:
            lua_pushboolean( L, 0 );
            lua_pushstring( L, err.message );
            rv++;
    }
    las_key_dispose( &lkey );
    
	return rv;
}


static int get_lua( lua_State *L )
{
    int rv = 1;
    las_key_t lkey;
    as_error err;
	
    if( las_key_read_init( L, &lkey ) != 0 ){
        lua_pushnil( L );
        lua_pushstring( L, strerror( errno ) );
        return 2;
    }
    
    switch( aerospike_key_get( lkey.as, &err, lkey.policy, lkey.key, &lkey.rec ) ){
        case AEROSPIKE_OK:
            lua_createtable( L, 0, 3 );
            lstate_num2tbl( L, "ttl", lkey.rec->ttl );
            lstate_num2tbl( L, "gen", lkey.rec->gen );
            lua_pushstring( L, "bins" );
            lstate_asrec2tbl( L, lkey.rec );
            lua_rawset( L, -3 );
        break;
        
        default:
            lua_pushnil( L );
            lua_pushstring( L, err.message );
            rv++;
    }
    las_key_dispose( &lkey );
    
	return rv;
}


static int select_lua( lua_State *L )
{
    int rv = 1;
    int argc = lua_gettop( L );
    const char **bins = NULL;
    int idx = 3;
    las_key_t lkey;
    as_error err;
    
    if( las_key_read_init( L, &lkey ) != 0 ){
        lua_pushnil( L );
        lua_pushstring( L, strerror( errno ) );
        return 2;
    }
    else if( !( bins = pnalloc( argc + 1, const char* ) ) ){
        las_key_dispose( &lkey );
        lua_pushnil( L );
        lua_pushstring( L, strerror( errno ) );
        return 2;
    }
    // set bin names
    for(; idx <= argc; idx++ )
    {
        if( lua_type( L, idx ) != LUA_TSTRING ){
            las_key_dispose( &lkey );
            pdealloc( bins );
            lua_pushnil( L );
            lua_pushfstring( L, "bin name#%d must be type of string", idx-1 );
            return 2;
        }
        bins[idx-3] = lua_tostring( L, idx );
    }
    bins[idx-3] = NULL;
    
    switch( aerospike_key_select( lkey.as, &err, lkey.policy, lkey.key, bins,
                                  &lkey.rec ) ){
        case AEROSPIKE_OK:
            lua_createtable( L, 0, 3 );
            lstate_num2tbl( L, "ttl", lkey.rec->ttl );
            lstate_num2tbl( L, "gen", lkey.rec->gen );
            lua_pushstring( L, "bins" );
            lstate_asrec2tbl( L, lkey.rec );
            lua_rawset( L, -3 );
        break;
        
        default:
            lua_pushnil( L );
            lua_pushstring( L, err.message );
            rv++;
    }
    las_key_dispose( &lkey );
    pdealloc( bins );
    
	return rv;
}


static int exists_lua( lua_State *L )
{
    int rv = 1;
    las_key_t lkey;
    as_error err;
	
    if( las_key_read_init( L, &lkey ) != 0 ){
        lua_pushboolean( L, 0 );
        lua_pushstring( L, strerror( errno ) );
        return 2;
    }
    switch( aerospike_key_exists( lkey.as, &err, lkey.policy, lkey.key, &lkey.rec ) ){
        case AEROSPIKE_OK:
            lua_pushboolean( L, 1 );
        break;
        case AEROSPIKE_ERR_RECORD_NOT_FOUND:
            lua_pushboolean( L, 0 );
        break;
        default:
            lua_pushnil( L );
            lua_pushstring( L, err.message );
            rv++;
    }
    las_key_dispose( &lkey );
    
	return rv;
}


static int remove_lua( lua_State *L )
{
    int rv = 1;
    las_key_t lkey;
    as_error err;
	
    if( las_key_remove_init( L, &lkey ) != 0 ){
        lua_pushboolean( L, 0 );
        lua_pushstring( L, strerror( errno ) );
        return 2;
    }
    switch( aerospike_key_remove( lkey.as, &err, lkey.policy, lkey.key ) ){
        case AEROSPIKE_OK:
            lua_pushboolean( L, 1 );
        break;
        case AEROSPIKE_ERR_RECORD_NOT_FOUND:
            lua_pushboolean( L, 0 );
        break;
        default:
            lua_pushnil( L );
            lua_pushstring( L, err.message );
            rv++;
    }
    las_key_dispose( &lkey );
    
	return rv;
}


// MARK: batch operations

typedef struct {
    aerospike *as;
    void *policy;
    lua_State *L;
    as_batch batch;
    int existence;
} las_batch_t;


static int las_batch_init( lua_State *L, las_batch_t *lbatch, int exists )
{
    int argc = lua_gettop( L );
    las_conn_t *conn = NULL;
    las_ctx_t *ctx = get_context( L, &conn );
    int idx = 2;
    
    lbatch->as = conn->as;
    lbatch->L = L;
    lbatch->existence = exists;
    lbatch->policy = &ctx->policies.batch;
    
    // keys: 2...N
    if( !as_batch_init( &lbatch->batch, argc - 1 ) ){
        lua_pushnil( L );
        lua_pushstring( L, strerror( errno ) );
        return 2;
    }
    // set key names
    for(; idx <= argc; idx++ )
    {
        if( lua_type( L, idx ) != LUA_TSTRING ){
            as_batch_destroy( &lbatch->batch );
            lua_pushnil( L );
            lua_pushfstring( L, "key#%d must be type of string", idx-1 );
            return 2;
        }
        as_key_init( as_batch_keyat( &lbatch->batch, idx - 2 ), ctx->ns,
                     ctx->set, lua_tostring( L, idx ) );
    }
    
    return 0;
}


static bool batchget_cb( const as_batch_read *results, uint32_t n, void *udata )
{
    las_batch_t *lbatch = (las_batch_t*)udata;
    lua_State *L = lbatch->L;
    uint32_t i = 0;
    const char *key = NULL;
    as_error err;
    
    lua_createtable( L, 0, n );
    for(; i < n; i++ )
    {
        key = as_string_get( (as_string*)&results[i].key->value );
        switch( results[i].result )
        {
            case AEROSPIKE_OK:
                if( lbatch->existence ){
                    lstate_bool2tbl( L, key, 1 );
                }
                else {
                    lua_pushstring( L, key );
                    lua_createtable( L, 0, 3 );
                    lstate_num2tbl( L, "ttl", results[i].record.ttl );
                    lstate_num2tbl( L, "gen", results[i].record.gen );
                    lua_pushstring( L, "bins" );
                    lstate_asrec2tbl( L, (as_record*)&results[i].record );
                    lua_rawset( L, -3 );
                    lua_rawset( L, -3 );
                }
            break;
            case AEROSPIKE_ERR_RECORD_NOT_FOUND:
                if( lbatch->existence ){
                    lstate_bool2tbl( L, key, 0 );
                }
            break;
            
            // The transaction didn't succeed.
            default:
                as_error_init( &err );
                las_ctx_aserror( &err, results[i].result );
                lstate_str2tbl( L, key, err.message );
        }
    }
    
    return true;
}

static int batchget_lua( lua_State *L )
{
    as_error err;
    las_batch_t lbatch;
    int rv = las_batch_init( L, &lbatch, 0 );
    
    if( rv != 0 ){
        return rv;
    }
    else if( aerospike_batch_get( lbatch.as, &err, lbatch.policy, &lbatch.batch,
                                  batchget_cb, (void*)&lbatch ) != AEROSPIKE_OK ){
        lua_pushnil( L );
        lua_pushstring( L, err.message );
        rv = 2;
    }
    else {
        rv = 1;
    }
    as_batch_destroy( &lbatch.batch );
    
	return rv;
}


static int batchexists_lua( lua_State *L )
{
    las_batch_t lbatch;
    as_error err;
    int rv = las_batch_init( L, &lbatch, 1 );
    
    if( rv != 0 ){
        return rv;
    }
    else if( aerospike_batch_exists( lbatch.as, &err, lbatch.policy,
                                     &lbatch.batch, batchget_cb,
                                     (void*)&lbatch ) != AEROSPIKE_OK ){
        lua_pushnil( L );
        lua_pushstring( L, err.message );
        rv = 2;
    }
    else {
        rv = 1;
    }
    as_batch_destroy( &lbatch.batch );
    
	return rv;
}




// MARK: scan operations

typedef struct {
    aerospike *as;
    as_policy_scan *policy;
    lua_State *L;
    as_scan scan;
    int nitem;
} las_scaneach_t;


static bool scaneach_cb( const as_val *val, void *udata )
{
    as_record *rec = as_record_fromval( val );
    las_scaneach_t *seach = (las_scaneach_t*)udata;
    lua_State *L = seach->L;

    if( rec )
    {
        as_digest *digest = as_key_digest( &rec->key );
        static const size_t len = AS_DIGEST_VALUE_SIZE * 2;
        char id[AS_DIGEST_VALUE_SIZE * 2] = {0};
        
        digest2hex( (uint8_t*)id, digest->value, AS_DIGEST_VALUE_SIZE );
        lua_createtable( L, 0, 3 );
        lstate_strn2tbl( L, "pk", id, len );
        lstate_num2tbl( L, "ttl", rec->ttl );
        lstate_num2tbl( L, "gen", rec->gen );
        if( as_record_numbins( rec ) ){
            lua_pushstring( L, "bins" );
            lstate_asrec2tbl( L, rec );
            lua_rawset( L, -3 );
        }
        lua_rawseti( L, -2, ++(seach->nitem) );
        return true;
    }
    
    return false;
}

static int scaneach_lua( lua_State *L )
{
    int argc = lua_gettop( L );
    las_conn_t *conn = NULL;
    las_ctx_t *ctx = get_context( L, &conn );
    las_scaneach_t seach = {
        .as = conn->as,
        .L = L,
        .policy = &ctx->policies.scan,
        .nitem = 0
    };
    int rv = 2;
    int idx = 3;
    as_error err;
    
    
    if( !as_scan_init( &seach.scan, ctx->ns, ctx->set ) ){
        lua_pushnil( L );
        lua_pushstring( L, strerror( errno ) );
        return 2;
    }
    // option table: 2 (not yet)
    
    // bin names: 3...N
    if( argc < 3 ){
        as_scan_set_nobins( &seach.scan, true );
    }
    else if( !as_scan_select_init( &seach.scan, (uint16_t)argc - 2 ) ){
        as_scan_destroy( &seach.scan );
        lua_pushnil( L );
        lua_pushstring( L, strerror( errno ) );
        return 2;
    }
    // set select bin names
    else
    {
        for(; idx <= argc; idx++ )
        {
            if( lua_type( L, idx ) != LUA_TSTRING ){
                as_scan_destroy( &seach.scan );
                lua_pushnil( L );
                lua_pushfstring( L, "bin#%d must be type of string", idx-1 );
                return 2;
            }
            as_scan_select( &seach.scan, lua_tostring( L, idx ) );
        }
    }
    
    lua_newtable( L );
    if( aerospike_scan_foreach( seach.as, &err, seach.policy, &seach.scan,
                                scaneach_cb, (void*)&seach ) != AEROSPIKE_OK ){
        lua_pop( L, 1 );
        lua_pushnil( L );
        lua_pushstring( L, err.message );
        rv = 2;
    }
    else {
        rv = 1;
    }
    as_scan_destroy( &seach.scan );
    
	return rv;
}


static int scanbackground_lua( lua_State *L )
{
    las_conn_t *conn = NULL;
    las_ctx_t *ctx = get_context( L, &conn );
    uint64_t scan_id = 0;
    as_error err;
    as_scan scan;
    as_status status;
    as_scan_info info;
    
    if( !as_scan_init( &scan, ctx->ns, ctx->set ) ){
        lua_pushboolean( L, 0 );
        lua_pushstring( L, strerror( errno ) );
        return 2;
    }
    
    // get background id
    status = aerospike_scan_background( conn->as, &err, &ctx->policies.scan,
                                        &scan, &scan_id );
    as_scan_destroy( &scan );
    // got error
    if( status != AEROSPIKE_OK ){
        lua_pushboolean( L, 0 );
        lua_pushstring( L, err.message );
        return 2;
    }
    
RETRY_SCANINFO:
    status = aerospike_scan_info( conn->as, &err, &ctx->policies.info, scan_id,
                                  &info );
    if( status != AEROSPIKE_OK ){
        lua_pushboolean( L, 0 );
        lua_pushstring( L, err.message );
        return 2;
    }
    
    // check scaninfo.status
    switch( info.status ){
        // The scan is currently running.
        case AS_SCAN_STATUS_INPROGRESS:
            goto RETRY_SCANINFO;
        
        // The scan completed successfully.
        case AS_SCAN_STATUS_COMPLETED:
            lua_pushboolean( L, 1 );
            return 1;
        
        // The scan was aborted. Due to failure or the user.
        // AS_SCAN_STATUS_ABORTED
        // The scan status is undefined.
        // AS_SCAN_STATUS_UNDEF:
        default:
            lua_pushboolean( L, 0 );
            return 1;
    }
}


// MARK: info operations

static int info_lua( lua_State *L )
{
    int argc = lua_gettop( L );
    las_conn_t *conn = NULL;
    las_ctx_t *ctx = get_context( L, &conn );
    const char *req = lstate_checkstring( L, 2 );
    const char *host = conn->as->config.hosts[0].addr;
    uint16_t port = conn->as->config.hosts[0].port;
    char *res = NULL;
    as_error err;
    
    // check arguments
    if( argc > 2 )
    {
        lua_Integer iport;
        
        host = lstate_checkstring( L, 2 );
        iport = lstate_checkinteger( L, 3 );
        if( iport > UINT16_MAX ){
            lua_pushnil( L );
            lua_pushfstring( L, "port must be less than %d", UINT16_MAX );
            return 2;
        }
        port = (uint16_t)iport;
    }
    
    if( aerospike_info_host( conn->as, &err, &ctx->policies.info, host, port,
                             req, &res ) == AEROSPIKE_OK ){
        if( res ){
            lua_pushstring( L, res );
            pdealloc( res );
        }
        else {
            lua_pushstring( L, "" );
        }
        return 1;
    }
    
    // got error
    lua_pushnil( L );
    lua_pushstring( L, err.message );
    
    return 2;
}


typedef struct {
    lua_State *L;
    int nitem;
} las_infoeach_t;

static bool infoeach_cb( const as_error *err, const as_node *node,
                         const char *req, char *res, void *udata )
{
    las_infoeach_t *info = (las_infoeach_t*)udata;
    lua_State *L = info->L;
    as_address *host = as_node_get_address_full( (as_node*)node );
    
    if( err->code == AEROSPIKE_OK ){
        lua_createtable( L, 0, 4 );
        lstate_str2tbl( L, "req", req );
        lstate_str2tbl( L, "info", res );
    }
    else {
        lua_createtable( L, 0, 3 );
        lstate_str2tbl( L, "err", err->message );
    }
    lstate_str2tbl( L, "host", host->name );
    lstate_num2tbl( L, "port", NTOHS( host->addr.sin_port ) );
    lua_rawseti( L, -2, info->nitem++ );
    
    return true;
}

static int infoeach_lua( lua_State *L )
{
    las_conn_t *conn = NULL;
    las_ctx_t *ctx = get_context( L, &conn );
    const char *req = lstate_checkstring( L, 2 );
    as_error err;
    las_infoeach_t info = {
        .L = L,
        .nitem = 0
    };
    
    lua_newtable( L );
    if( aerospike_info_foreach( conn->as, &err, &ctx->policies.info, req,
                                infoeach_cb, (void*)&info ) == AEROSPIKE_OK ){
        return 1;
    }
    
    // got error
    lua_pop( L, 1 );
    lua_pushnil( L );
    lua_pushstring( L, err.message );
    
    return 2;
}


// MARK: index operations
#define LAS_IDX_INTEGER 1
#define LAS_IDX_STRING  2

static int indexcreate_lua( lua_State *L )
{
    las_conn_t *conn = NULL;
    las_ctx_t *ctx = get_context( L, &conn );
    lua_Integer type = lstate_checkinteger( L, 2 );
    const char *name = lstate_checkstring( L, 3 );
    const char *bin = lstate_checkstring( L, 4 );
    as_status rc;
    as_error err;
    
    switch( type ){
        case LAS_IDX_INTEGER:
            rc = aerospike_index_integer_create( conn->as, &err,
                                                 &ctx->policies.info, ctx->ns,
                                                 ctx->set, bin, name );
        break;
        case LAS_IDX_STRING:
            rc = aerospike_index_string_create( conn->as, &err,
                                                &ctx->policies.info, ctx->ns,
                                                ctx->set, bin, name );
        break;
        default:
            return luaL_argerror( L, 4, "index type must be IDX_INTEGER | IDX_STRING" );
    }
    
    if( rc == AEROSPIKE_OK ){
        lua_pushboolean( L, 1 );
        return 1;
    }
    // got error
    lua_pushboolean( L, 0 );
    lua_pushstring( L, err.message );
    
    return 2;
}


static int indexremove_lua( lua_State *L )
{
    las_conn_t *conn = NULL;
    las_ctx_t *ctx = get_context( L, &conn );
    const char *name = lstate_checkstring( L, 2 );
    as_error err;
    
    if( aerospike_index_remove( conn->as, &err, &ctx->policies.info, ctx->ns,
                                name ) == AEROSPIKE_OK ){
        lua_pushboolean( L, 1 );
        return 1;
    }
    // got error
    lua_pushboolean( L, 0 );
    lua_pushstring( L, err.message );
    
    return 2;
}


int las_ctx_alloc_lua( lua_State *L )
{
    las_ctx_t *ctx = NULL;
    // namespace(like schema)
    size_t ns_len = 0;
    const char *ns = NULL;
    // set(like table)
    size_t nsset_len = 0;
    const char *nsset = NULL;
    
    // check arguments
    luaL_checkudata( L, 1, LAS_CONNECT_MT );
    ns = lstate_checklstring( L, 2, &ns_len );
    nsset = lstate_checklstring( L, 3, &nsset_len );
    if( ns_len >= AS_NAMESPACE_MAX_SIZE ){
        lua_pushnil( L );
        lua_pushfstring( L, "namespace length must be less than %d", AS_NAMESPACE_MAX_SIZE );
    }
    else if( nsset_len >= AS_SET_MAX_SIZE ){
        lua_pushnil( L );
        lua_pushfstring( L, "set length must be less than %d", AS_SET_MAX_SIZE );
    }
    else if( ( ctx = lua_newuserdata( L, sizeof( las_ctx_t ) ) ) ){
        ctx->ref_conn = lstate_ref( L, 1 );
        as_policies_init( &ctx->policies );
        // copy string+null-terminator
        memcpy( (void*)ctx->ns, ns, ns_len + 1 );
        memcpy( (void*)ctx->set, nsset, nsset_len + 1 );
        lstate_setmetatable( L, LAS_CONTEXT_MT );
        return 1;
    }
    // mem error
    else {
        lua_pushnil( L );
        lua_pushstring( L, strerror( errno ) );
    }

    return 2;
}

static int gc_lua( lua_State *L )
{
    las_ctx_t *ctx = (las_ctx_t*)lua_touserdata( L, 1 );
    
    // release las_conn_t reference
    lstate_unref( L, ctx->ref_conn );

    return 0;
}

static int tostring_lua( lua_State *L )
{
    return TOSTRING_MT( L, LAS_CONTEXT_MT );
}

static void define_constants( lua_State *L )
{
    // set constants
    // types for create index
    lstate_num2tbl( L, "IDX_INTEGER", LAS_IDX_INTEGER );
    lstate_num2tbl( L, "IDX_STRING", LAS_IDX_STRING );
}

int las_ctx_init( lua_State *L )
{
    struct luaL_Reg mmethod[] = {
        { "__gc", gc_lua },
        { "__tostring", tostring_lua },
        { NULL, NULL }
    };
    struct luaL_Reg method[] = {
        // key ops
        { "put", put_lua },
        { "get", get_lua },
        { "select", select_lua },
        { "exists", exists_lua },
        { "remove", remove_lua },
        // batch ops
        { "batchGet", batchget_lua },
        { "batchExists", batchexists_lua },
        // scan ops
        { "scanBackground", scanbackground_lua },
        { "scanEach", scaneach_lua },
        // info ops
        { "info", info_lua },
        { "infoEach", infoeach_lua },
        // index ops
        { "indexCreate", indexcreate_lua },
        { "indexRemove", indexremove_lua },
        
        /* not yet implemented
        // advanced ops
        { "operate", operate_lua },
        { "apply", apply_lua },
        { "incr", incr_lua },
        // query ops
        { "query", query_lua },
        // UDF ops
        { "udfPut", udf_put_lua },
        { "udfGet", udf_get_lua },
        { "udfList", udf_list_lua },
        { "udfRemove", udf_remove_lua },
        */
        { NULL, NULL }
    };
    
    // define metatable
    lstate_definemt( L, LAS_CONTEXT_MT, mmethod, method, define_constants );
    
    return 1;
}


