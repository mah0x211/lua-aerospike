require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONTEXT = require('./context');

printUsage( 'context:batchGet', unpack( DATA.KEYS ) );
print( '>>', inspect(assert(
    CONTEXT:batchGet( unpack( DATA.KEYS ) )
)));
