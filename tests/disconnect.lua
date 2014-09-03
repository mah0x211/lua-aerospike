require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );

local CONN = require('./connect');

printUsage( 'connection:close' );
print( '>>', assert(
    CONN:close()
));

