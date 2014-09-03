require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONTEXT = require('./context');

printUsage( 'context:scanBackground' );
print( '>>', inspect(assert(
    CONTEXT:scanBackground()
)));

