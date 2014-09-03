require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local UDF = require('./udf');

printUsage( 'udf:list' );
print( '>>', inspect(assert(
    UDF:list()
)));

