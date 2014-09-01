require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONTEXT = require('./context');
local operation = aerospike.operation();
local bin, ops, op, val, _;

for bin, ops in pairs( DATA.OPEARATE ) do
    print( bin, ops );
    for op, val in pairs( ops ) do
        print( op, val );
        assert( operation[op]( operation, bin, val ) );
    end
    assert( operation:read( bin ) );
end

for _, v in ipairs( DATA.KEYS ) do
    print( 'operate', v, inspect(assert(
        CONTEXT:operate( v, operation )
    )));
end
