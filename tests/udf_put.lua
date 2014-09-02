require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local UDF = require('./udf');
local k, v;

for k, v in pairs( DATA.UDF ) do
    print( 'put', k, inspect(assert(
        UDF:put( k, v )
    )));
end
