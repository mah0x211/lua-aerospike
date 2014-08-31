require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );

local CONN = require('./connect');

print( 'close', assert(
    CONN:close()
));

