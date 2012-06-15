
use('Perl');

function exc() { this.toString = function() { return "EXC"; } }
throw new exc();

