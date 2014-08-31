require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );
require('./helper');

local _, v, rec;
local records = {};

for _, v in ipairs( DATA.KEYS ) do
    rec = assert( aerospike.record( v, DATA.DATA, DATA.TTL ) );
    records[#records+1] = rec;
end

DATA.RECORDS = records;

