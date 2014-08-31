require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONTEXT = require('./context');

print( 'batchExists', inspect(assert(
    CONTEXT:batchExists( unpack( DATA.KEYS ) )
)));

