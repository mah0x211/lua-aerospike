require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONTEXT = require('./context');

printUsage( 'context:scanEach', nil, unpack( DATA.SELECT ) );
print( '>>', inspect(assert(
    CONTEXT:scanEach( nil, unpack( DATA.SELECT ) )
)));

