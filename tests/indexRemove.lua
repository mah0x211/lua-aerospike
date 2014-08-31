require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONTEXT = require('./context');

print( 'removeIndex', inspect( DATA.IDX_STR ), assert(
    CONTEXT:indexRemove( DATA.IDX_STR.NAME )
));
print( 'removeIndex', inspect( DATA.IDX_INT ), assert(
    CONTEXT:indexRemove( DATA.IDX_INT.NAME )
));
