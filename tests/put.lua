require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONTEXT = require('./context');
local _, v;

for _, v in ipairs( DATA.KEYS ) do
    DATA.DATA.c = DATA.DATA.c + 1;
    printUsage( 'context:put', v, DATA.DATA, DATA.TTL );
    print( '>>', assert(
        CONTEXT:put( v, DATA.DATA, DATA.TTL )
    ));
end

