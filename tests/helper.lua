require('process').chdir( (arg[0]):match( '^(.+[/])[^/]+%.lua$' ) );

_G.inspect = require('util').inspect;
_G.aerospike = require('aerospike');
_G.DATA = require('./data');

return true;

