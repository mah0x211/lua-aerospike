require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONTEXT = require('./context');

print( 'scanEach', inspect(assert(
    CONTEXT:scanEach( nil, unpack( DATA.SELECT ) )
)));

