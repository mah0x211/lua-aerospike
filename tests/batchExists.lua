require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONTEXT = require('./context');

printUsage( 'context:batchExists', unpack( DATA.KEYS ) );
print( '>>', inspect(assert(
    CONTEXT:batchExists( unpack( DATA.KEYS ) )
)));
