
use('Perl');

function foo() {
  Perl.Use("aaaa");
}

try {
  foo();
} catch (e) {
  var s = "(\n etype: " + e.etype + ",\n error: " + e.error + ",\n func: "
    + e.func + ",\n description: " + e.description;
  var arr = e.at;
  if (arr) {
    s += ",\n at:";
    for (var i = 0; i < arr.length; ++i) {
      var f = arr[i].func;
      if (f == "") { f = "[anonymous]"; }
      s += "\n  func=" + f + ", file=" + arr[i].file + ", line=" + arr[i].line;
    }
  }
  s += "\n)\n";
  print(s);
}

