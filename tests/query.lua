require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local CONTEXT = require('./context');

printUsage( 'context:query', DATA.QUERY );
print( '>>', inspect( DATA.QUERY ), --[['module: ' .. DATA.APPLY.module .. ' func: ' .. DATA.APPLY.func ..]] ' | result:', inspect(assert(
    CONTEXT:query( DATA.QUERY--[[, DATA.APPLY.module, DATA.APPLY.func, DATA.APPLY.args]] )
)));

