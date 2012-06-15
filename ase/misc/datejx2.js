
var JavaDate = Java['java.util.Date'];
var jd = JavaDate();
print(jd.getDate());
// print(jd.toString());
// print("T: " + jd.getTime()); // FIXME: throws
print("L: " + jd.toLocaleString());
print("S: " + jd.toString());
print("D: " + jd.getDay());
print("G: " + jd.toGMTString());

// print("tostring begin");
// var f = jd.toLocaleString;
// print("tostring end");
// print(f());

