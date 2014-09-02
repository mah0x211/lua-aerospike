local keys = {};
local i;
for i = 1, 1 do
    keys[#keys+1] = 'test-key' .. i;
end

local UDF_TMPL = [[
function $NAME( ... )
    local args = {};
    local i, v;
    for i, v in ipairs({...}) do
        args[#args+1] = i .. ':' .. tostring( v );
    end
    return '$FUNC - args: ' .. table.concat( args, ' | ' );
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
        NAME = 'index_a',
        BIN = 'a'
    },
    IDX_INT = {
        NAME = 'index_c',
        BIN = 'c'
    },
    UDF = {
        ['sample_udf1.lua'] = UDF_TMPL:gsub( '$NAME', 'hello1' ) ..
                              UDF_TMPL:gsub( '$NAME', 'world1' ),
        ['sample_udf2.lua'] = UDF_TMPL:gsub( '$NAME', 'hello2' ) ..
                              UDF_TMPL:gsub( '$NAME', 'world2' )
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
