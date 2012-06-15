
Perl.Use("CycleTest");

var jso = { abc : 'val abc', foo : function() { return 100; } };
jso.pobj = new Perl("CycleTest", jso);

print(jso.pobj.jso.abc);
print(jso.pobj.jso.foo());

