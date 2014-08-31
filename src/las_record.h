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
 *  las_record.h
 *  lua-aerospike
 *
 *  Created by Masatoshi Teruya on 2014/08/26.
 *
 */

#ifndef lua_aerospike_las_record_h
#define lua_aerospike_las_record_h

#include "las.h"

#define LAS_RECORD_PK_LEN   (AS_DIGEST_VALUE_SIZE * 2)

typedef struct {
    uint32_t ttl;
    uint16_t gen;
    uint16_t nbins;
    uint8_t pk[LAS_RECORD_PK_LEN];
    int ref_key;
    int ref_bins;
} las_record_t;


// prototypes
LUALIB_API int luaopen_aerospike_record( lua_State *L );


#endif
