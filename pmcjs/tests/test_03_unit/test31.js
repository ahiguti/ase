
/* checks ht cleanness */

var ht = Perl.NewHash();

var names = [
  '__proto__',
  '__parent__',
  '__count__',
  'proto',
  '__noSuchMethod__',
];

for (var i = 0; i < names.length; ++i) {
  var n = names[i];
  print(ht[n]);
}

for (var i = 0; i < names.length; ++i) {
  var n = names[i];
  ht[n] = 'VAL: ' + n;
}

for (var i = 0; i < names.length; ++i) {
  var n = names[i];
  print(ht[n]);
}
