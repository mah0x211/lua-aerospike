require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONN = require('./connect');
local udf = assert(
    aerospike.udf( CONN )
);

print( 'udf', udf );

return udf;
