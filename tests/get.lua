require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONTEXT = require('./context');
local _, v;

for _, v in ipairs( DATA.KEYS ) do
    print( 'get', v, inspect(assert(
        CONTEXT:get( v )
    )));
end
