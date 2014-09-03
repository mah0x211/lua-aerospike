require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONTEXT = require('./context');
local _, v;

for _, v in ipairs( DATA.IDX_STR ) do
    printUsage( 'context:indexCreate', 'context.IDX_STRING', v.NAME, v.BIN );
    print( '>>', assert(
        CONTEXT:indexCreate( CONTEXT.IDX_STRING, v.NAME, v.BIN )
    ));
end

for _, v in ipairs( DATA.IDX_INT ) do
    printUsage( 'context:indexCreate', 'context.IDX_INTEGER', v.NAME, v.BIN );
    print( '>>', assert(
        CONTEXT:indexCreate( CONTEXT.IDX_INTEGER, v.NAME, v.BIN )
    ));
end
