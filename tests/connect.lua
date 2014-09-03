require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local conn;

printUsage( 'aerospike.open', '<default host: 127.0.0.1>', '<default port: 3000>' );
conn = assert( aerospike.open() );
print( '>>', conn );

return conn;

