
use('Perl');

var obj = {
  foo : function() { abc(); }
};

Perl.Use("ExcTest");
var et = Perl.Functions["ExcTest"];
for (var i = 0; i < 100000; ++i) {
  try {
    et.callfoo(obj);
  } catch (e) {
  }
}

