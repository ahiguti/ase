
use('Perl');
Perl.Use('Getopt::Std');
var getopt = Perl.Functions['Getopt::Std'].getopt;

var opts = {};
getopt('oDI', opts);
for (var i in opts) {
  print('opt=' + i + ' val=' + opts[i]);
}

