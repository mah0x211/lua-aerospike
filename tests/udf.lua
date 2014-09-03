require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONN = require('./connect');
local udf;

printUsage( 'aerospike.udf', CONN );
udf = assert(
    aerospike.udf( CONN )
);
print( '>>', udf );

return udf;
