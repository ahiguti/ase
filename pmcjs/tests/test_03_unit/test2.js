
Perl.Use("JSObjTest");
var jo = Perl.Functions["JSObjTest"];

var obj = {
  x : "this is prop-x",
  y : "this is prop-y",
  z : "this is prop-z",
  foo : function(x, y, z) { return x + y + z; }
};

print("getprop: ", jo.jsobj_getprop(obj, "y"));
print("getprop_smart: ", jo.jsobj_getprop_smart(obj, "y"));
jo.jsobj_setprop(obj, "x", 99);
jo.jsobj_setprop_smart(obj, "z", 123);
print("setprop: ", obj.x);
print("setprop_smart: ", obj.z);
print("invoke: ", jo.jsobj_invoke_foo(obj, "abc", "def", "ghi"));
print("invoke_smart: ", jo.jsobj_invoke_foo_smart(obj, "abc", "def", "ghi"));

