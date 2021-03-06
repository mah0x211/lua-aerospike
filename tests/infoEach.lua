require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONTEXT = require('./context');
local _, v;

for _, v in ipairs( DATA.INFOREQ ) do
    printUsage( 'context:infoEach', v );
    print( '>>', inspect(assert(
        CONTEXT:infoEach( v )
    )));
    
end

