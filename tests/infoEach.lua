require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONTEXT = require('./context');
local _, v;

for _, v in ipairs( DATA.INFOREQ ) do
    print( 'infoEach', v, inspect(assert(
        CONTEXT:infoEach( v )
    )));
end
