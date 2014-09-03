require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONTEXT = require('./context');
local _, v;

for _, v in ipairs( DATA.KEYS ) do
    printUsage( 'context:get', v );
    print( '>>', inspect(assert(
        CONTEXT:get( v )
    )));
end

