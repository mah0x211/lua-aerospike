require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONTEXT = require('./context');
local _, v;

for _, v in ipairs( DATA.KEYS ) do
    printUsage( 'context:select', v, unpack( DATA.SELECT ) );
    print( '>>', inspect(assert(
        CONTEXT:select( v, unpack( DATA.SELECT ) )
    )));
end

