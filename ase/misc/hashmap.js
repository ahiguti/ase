
var HashMap = Java["java.util.HashMap"];
var Integer = Java["java.lang.Integer"];

var m = HashMap();
m.put("abc", "ABCVAL");
m.put("foo", "FOOVAL");
m.put("baz", Integer(10));
var x = m.put("baz", undefined);
print("abc: " + m.get("abc"));
print("foo: " + m.get("foo"));
print("bar: " + m.get("bar"));
print("baz: " + m.get("baz"));
print("x: " + x);
