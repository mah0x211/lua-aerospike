require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONTEXT = require('./context');
local _, v;

for _, v in ipairs( DATA.KEYS ) do
    printUsage( 'context:apply', v, DATA.APPLY.module, DATA.APPLY.func );
    print( '>>', inspect(assert(
        CONTEXT:apply( v, DATA.APPLY.module, DATA.APPLY.func, DATA.APPLY.args )
    )));
end
