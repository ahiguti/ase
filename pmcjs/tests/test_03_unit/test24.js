
var x = Perl.NewHash({ foo: 'bar', baz: 'fubar' });
var mts = [
  'Invoke',
  'InvokeLC',
  'SetProperty',
  'DelProperty',
  'Set',
  'SetText',
  'SetBinary',
  'Get',
  'GetText',
  'GetBinary',
  'toString',
  'Detach',
  '__noSuchMethod__'
];

// x.toString = function() { return "foo"; }

for (var i in mts) {
  var n = mts[i];
  try {
    var r = x[n]('abc');
    print(n + " returns [" + r + "]");
  } catch (e) {
    print(n + " throws " + e.error);
  }
}
