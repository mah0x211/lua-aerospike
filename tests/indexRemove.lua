require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONTEXT = require('./context');
local _, v;

for _, v in ipairs( DATA.IDX_STR ) do
    printUsage( 'context:indexRemove', v.NAME );
    print( '>>', assert(
        CONTEXT:indexRemove( v.NAME )
    ));
end

for _, v in ipairs( DATA.IDX_INT ) do
    printUsage( 'context:indexRemove', v.NAME );
    print( '>>', assert(
        CONTEXT:indexRemove( v.NAME )
    ));
end

