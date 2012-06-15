
Perl.Use("PObjTest");
var pobj = new Perl("PObjTest");

print("getprop: ", pobj.GetProperty('y'));
print("getprop: ", pobj['y']);
print("getprop_smart: ", pobj.y);
pobj.SetProperty('x', 99);
pobj.z = 123;
print("setprop: ", pobj.x);
print("setprop_smart: ", pobj.z);
print("invoke: ", pobj.Invoke('foo', 1, 2, 3));
print("invoke_smart: ", pobj.bar(194, 2, 3));
print("tostr: ", pobj.tostr());

print("enum1");
for (var i in pobj) {
  print("  k=" + i + " v=" + pobj[i]);
}
print("enum2");
pobj.baz = 100;
for (var i in pobj) {
  print("  k=" + i + " v=" + pobj[i]);
}
print("enum3");
pobj.SetProperty('fubar', 1200);
pobj['fubar'] = 1200;
for (var i in pobj) {
  print("  k=" + i + " v=" + pobj[i]);
}
