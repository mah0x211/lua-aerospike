require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONTEXT = require('./context');

print( 'batchGet', unpack( DATA.KEYS ), inspect(assert(
    CONTEXT:batchGet( unpack( DATA.KEYS ) )
)));

