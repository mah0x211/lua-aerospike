require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONTEXT = require('./context');
local operation = require('./operation');

for _, v in ipairs( DATA.KEYS ) do
    printUsage( 'context:operate', v, operation );
    print( '>>', inspect(assert(
        CONTEXT:operate( v, operation )
    )));
end

