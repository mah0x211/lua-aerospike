local keys = {};
local i;
for i = 1, 3 do
    keys[#keys+1] = 'test-key' .. i;
end

local UDF_TMPL = [[

function $NAME( ... )
    local args = {};
    local i, v;
    for i, v in ipairs({...}) do
        args[#args+1] = i .. ':' .. tostring( v );
    end
    return '$NAME - args: ' .. table.concat( args, ' | ' );
end

]];

local SCAN_APPLY_UDF = [[
function mytransform(rec)
    rec['b'] = 'add ' .. rec['a'];
    aerospike:update(rec)
end
]];

return {
    NAMESPACE = 'test',
    SET = 'test-set',
    INFOREQ = {
        'help', 'features', 'namespaces', 'sets'
    },
    KEYS = keys,
    TTL = -1,
    SELECT = { 'a', 'b', 'list' },
    IDX_STR = {
        { NAME = 'index_a', BIN = 'a' },
        { NAME = 'index_b', BIN = 'b' }
    },
    IDX_INT = {
        { NAME = 'index_c', BIN = 'c' }
    },
    UDF = {
        ['sample_udf1.lua'] = UDF_TMPL:gsub( '$NAME', 'hello1' ) ..
                              UDF_TMPL:gsub( '$NAME', 'world1' ),
        ['sample_udf2.lua'] = UDF_TMPL:gsub( '$NAME', 'hello2' ) ..
                              UDF_TMPL:gsub( '$NAME', 'world2' ),
        ['scan_apply_udf.lua'] = SCAN_APPLY_UDF
    },
    SCAN_OPT = {
        priority = aerospike.SCAN_PRIORITY_AUTO,
        --percent = 99,
        --concurrent = true,
        apply = {
            module = 'scan_apply_udf',
            func = 'mytransform',
            args = {
                'a1', 'a2', 'a3', {
                    b1 = {
                        x = 'x',
                        y = 'y'
                    },
                    b2 = 'zp'
                }
            }
        }
    },
    APPLY = {
        module = 'sample_udf1',
        func = 'hello1',
        args = {
            'a1', 'a2', 'a3', {
                b1 = {
                    x = 'x',
                    y = 'y'
                },
                b2 = 'z'
            }
        }
    },
    QUERY = {
        select = { 'b', 'c', 'map' },
        where = {
            c = { 15, 18 }
        }
    },
    OPEARATE = {
        a = {
            append = 'append str'
        },
        b = {
            prepend = 'prepend str'
        },
        c = {
            incr = 1900
        },
        map = {
            write = {
                hello = 'world'
            }
        }
    },
    DATA = {
        a = 'a',
        b = 'b',
        c = 10,
        map = {
            d = 'd',
            list_in_map = {
                'lm1', 'lm2', 'lm3', {
                    map_in_list = {
                        x = 'x',
                        y = 'y'
                    },
                    z = 'z'
                }
            },
        },
        list = {
            'l1', 'l2', 'l3', {
                map_in_list = {
                    x = 'x',
                    y = 'y'
                },
                z = 'z'
            }
        }
    }
};


