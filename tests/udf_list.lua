require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local UDF = require('./udf');

print( 'list', inspect(assert(
    UDF:list()
)));

