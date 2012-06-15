
var Comparable = Java["java.lang.Comparable"];
var JDate = Java["java.util.Date"];
var Arrays = Java["java.util.Arrays"];
var JClass = Java["java.lang.Class"];
var arr = JClass.NewArray(10);
arr[0] = JClass;
arr[1] = JDate;
arr[2] = Comparable;
print("A1: " + arr[0].getName());
print("A2: " + arr[1].getName());
print("A3: " + arr[2].getName());

var p = JDate.getSuperclass();
print(Comparable.getName());
print(p.getName());
print(arr.getClass().getName());

