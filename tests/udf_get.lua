require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local UDF = require('./udf');
local k, _;

for k, _ in pairs( DATA.UDF ) do
    printUsage( 'udf:get', k );
    print( '>>', inspect(assert(
        UDF:get( k )
    )));
end

