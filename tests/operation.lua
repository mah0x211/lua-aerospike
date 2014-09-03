require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONTEXT = require('./context');
local operation;

printUsage( 'aerospike.operation' );
operation = assert( aerospike.operation() );
print( '>>', operation );


local bin, ops, op, val, _, res;

for bin, ops in pairs( DATA.OPEARATE ) do
    for op, val in pairs( ops ) do
        printUsage( 'operation:' .. op, bin, val );
        print( '>>', assert( operation[op]( operation, bin, val ) ) );
    end

    printUsage( 'operation:read', bin );
    print( '>>', assert( operation:read( bin ) ) );
end

return operation;
