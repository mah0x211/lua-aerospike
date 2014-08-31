require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONTEXT = require('./context');

print( 'createIndex IDX_STRING', inspect( DATA.IDX_STR ), assert(
    CONTEXT:indexCreate( CONTEXT.IDX_STRING, DATA.IDX_STR.NAME, DATA.IDX_STR.BIN )
));
print( 'createIndex IDX_INTEGER', inspect( DATA.IDX_INT ), assert(
    CONTEXT:indexCreate( CONTEXT.IDX_INTEGER, DATA.IDX_INT.NAME, DATA.IDX_INT.BIN )
));
