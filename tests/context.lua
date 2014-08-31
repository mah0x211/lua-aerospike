require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONN = require('./connect');
local context = assert(
    CONN:context( DATA.NAMESPACE, DATA.SET )
);
print( 'context', context );

return context;
