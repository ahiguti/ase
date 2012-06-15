
var obj = {
  foo : function() { abc(); }
};

Perl.Use("ExcTest");
var et = Perl.Functions["ExcTest"];
try {
  et.callfoo(obj);
} catch (e) {
  print("exception=" + e.func);
}

