require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONTEXT = require('./context');
local _, v;

for _, v in ipairs( DATA.KEYS ) do
    printUsage( 'context:exists', DATA.KEYS[1] );
    print( '>>', assert(
        CONTEXT:exists( v )
    ));
end

