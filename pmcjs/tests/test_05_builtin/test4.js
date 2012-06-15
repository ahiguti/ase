
var core = Perl.Functions['CORE'];
var pi = 3.14159265;

function dprint(x) {
  print(core.sprintf('%.4f', x));
}

dprint(core.abs(-17));
dprint(core.atan2(1, 5));
dprint(core.cos(1));
dprint(core.sin(1));
dprint(core.log(10));
dprint(core.exp(1));
dprint(core['int'](12.40));
dprint(core.sqrt(2));

core.srand(100);
dprint(core.rand(10000));

