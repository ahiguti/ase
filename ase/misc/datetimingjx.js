
var jd = Java["java.util.Date"].NewInstance();
print(jd.getDate());
// print(jd.toString());
print("T: " + jd.getTime()); // FIXME: throws
print("L: " + jd.toLocaleString());
// print("S: " + jd.toString());
print("D: " + jd.getDay());
print("G: " + jd.toGMTString());

var r;
for (var i = 0; i < 10000000; ++i) {
  r = jd.getHours();
}
print("r=" + r);

