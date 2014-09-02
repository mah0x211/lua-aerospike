require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONTEXT = require('./context');
local _, v;

for _, v in ipairs( DATA.IDX_STR ) do
    print( 'removeIndex IDX_STRING', inspect( v ), assert(
        CONTEXT:indexRemove( v.NAME )
    ));
end

for _, v in ipairs( DATA.IDX_INT ) do
    print( 'removeIndex IDX_INTEGER', inspect( v ), assert(
        CONTEXT:indexRemove( v.NAME )
    ));
end