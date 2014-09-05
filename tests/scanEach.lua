require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONTEXT = require('./context');

printUsage( 'context:scanEach', DATA.SCAN_OPT, unpack( DATA.SELECT ) );
print( '>>', inspect(assert(
    CONTEXT:scanEach( DATA.SCAN_OPT, unpack( DATA.SELECT ) )
)));

