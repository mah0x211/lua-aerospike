local keys = {};
local i;
for i = 1, 1 do
    keys[#keys+1] = 'test-key' .. i;
end


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
