require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local conn = assert( aerospike.open() );

print( 'open', conn );

return conn;

