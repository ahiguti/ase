
use('Perl');
Perl.Use('Unicode::UCD');
var ucd = Perl.Functions['Unicode::UCD'];
var str = 'Aあアぁｱ漢';
for (var i = 0; i < str.length; ++i) {
  var c = str.charAt(i);
  var code = str.charCodeAt(i);
  var ci = ucd.charinfo(code);
  print("char: " + c);
  print("code: " + code.toString(16));
  print("name: " + ci.name);
}

