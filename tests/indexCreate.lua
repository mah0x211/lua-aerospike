require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONTEXT = require('./context');
local _, v;

for _, v in ipairs( DATA.IDX_STR ) do
    print( 'createIndex IDX_STRING', inspect( v ), assert(
        CONTEXT:indexCreate( CONTEXT.IDX_STRING, v.NAME, v.BIN )
    ));
end

for _, v in ipairs( DATA.IDX_INT ) do
    print( 'createIndex IDX_INTEGER', inspect( v ), assert(
        CONTEXT:indexCreate( CONTEXT.IDX_INTEGER, v.NAME, v.BIN )
    ));
end