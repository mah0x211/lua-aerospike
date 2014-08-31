require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');
local cl = require('ansicolors');

local printOrg = print;
local function hookPrint()
    _G.print = function() end;
end
local function unhookPrint()
    _G.print = printOrg;
end

local CWD = require('process').getcwd();
local SEACHPATH = CWD .. '/?.lua;' .. package.path;
local EXTENSION = '.lua';
local TESTS = {
    'connect',
    'context',
    'put',
    'indexCreate',
    'indexRemove',
    'get',
    'select',
    'exists',
    'batchGet',
    'batchExists',
    'scanEach',
    'scanBackground',
    'remove',
    'info',
    'infoEach',
    'disconnect'
};

local fmtSuccess, fmtFailure;
-- setup output format
do
    local fmt = '%%{yellow}TEST %-%ds cost %f sec %%{reset}| ';
    local len = 0;
    for _, v in ipairs( TESTS ) do
        if ( #v + #EXTENSION ) > len then
            len = #v + 4;
        end
    end

    fmt = '%%{yellow}TEST %-' .. len .. 's %%{cyan}%f sec %%{reset} ';
    fmtSuccess = fmt .. '%%{green}SUCCESS';
    fmtFailure = fmt .. '%%{red}FAILURE - %s';
end

local _, v, file, ok, err, sec, cost, costAll;
local success = {};
local failure = {};

-- set search path
package.path = SEACHPATH;

print( cl( ('%%{magenta}START %d TESTS'):format( #TESTS ) ) );
print( '====================================================================' );

costAll = 0;
sec = os.clock();
for _, v in ipairs( TESTS ) do
    -- append file extension
    file = ('%s%s'):format( v, EXTENSION );
    
    -- run test
    hookPrint();
    cost = os.clock();
    ok, err = pcall( require, './' .. v );
    unhookPrint();
    cost = os.clock() - cost;
    costAll = costAll + cost;
    
    if ok then
        success[#success+1] = file;
        print( cl( fmtSuccess:format( file, cost ) ) );
    else
        -- remove cwd path
        err = err:gsub( '/+', '/' ):sub( #CWD + 2 );
        failure[#failure+1] = err;
        print( cl( fmtFailure:format( file, cost, err ) ) );
    end
end
sec = os.clock() - sec;

print( '====================================================================' );
print( cl( ('%%{magenta}TIME: %f sec, TOTAL COST: %f sec\n'):format( sec, costAll ) ) );
print( cl( ('%%{green}SUCCESS: %d'):format( #success ) ) );
print( cl( ('%%{red}FAILURE: %d'):format( #failure ) ) );
print( '\t' .. table.concat( failure, '\n\t' ) );
print( '--------------------------------------------------------------------' );
print( '\n' );

