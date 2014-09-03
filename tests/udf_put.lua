require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local UDF = require('./udf');
local k, v;

for k, v in pairs( DATA.UDF ) do
    printUsage( 'udf:put', k, v );
    print( '>>', inspect(assert(
        UDF:put( k, v )
    )));
end
