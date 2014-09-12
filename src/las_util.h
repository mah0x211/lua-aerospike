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



#define lstate_popfalse(L,i) ({ \
    lua_pop( L, i ); \
    0; \
})

#define lstate_getidx(L,i,t) ({ \
    lua_pushnumber(L,i); \
    lua_rawget(L,-2); \
    (lua_type(L,-1) == t) ? 1 : lstate_popfalse(L,1); \
})

#define lstate_getfield(L,v,t) ({ \
    lua_pushstring(L,v); \
    lua_rawget(L,-2); \
    (lua_type(L,-1) == t) ? 1 : lstate_popfalse(L,1); \
})


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
                                    struct luaL_Reg method[] )
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
    
    lua_rawset( L, -3 );
    lua_pop( L, 1 );
}

#define LUA_TTABLE_EMPTY    0x0
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
    int type = LUA_TTABLE_EMPTY;
    
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
as_query *lstate_tbl2asqry( lua_State *L, const char *ns, const char *set );

int lstate_asval2lua( lua_State *L, as_val *val );
uint16_t lstate_asrec2tbl( lua_State *L, as_record *rec );



#endif
