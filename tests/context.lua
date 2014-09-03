require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONN = require('./connect');
local context;

printUsage( 'connection:context', DATA.NAMESPACE, DATA.SET );
context = assert(
    CONN:context( DATA.NAMESPACE, DATA.SET )
);
print( '>>', context );

return context;
