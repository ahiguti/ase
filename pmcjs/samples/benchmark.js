
use('Perl');
Perl.Use('Benchmark');
var timethese = Perl.Functions['Benchmark'].timethese;

var foocount = 0;
var barcount = 0;
/* seems that hash must be a perl hash instead of a javascript object */
var hash = Perl.NewHash({
 foo: function() { foocount++; },
 bar: function() { barcount++; }
});
timethese(100, hash);

