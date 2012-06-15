
var sysprops = GCJ["java.lang.System"].getProperties();
print("GCJ: " + sysprops.getProperty("java.vendor.url"));
// print("HostVM: " + java.lang.System.getProperties()
//   .getProperty("java.vendor.url"));

var jd = GCJ["java.util.Date"].NewInstance();
print(jd.getDate());
// print(jd.toString());
print("T: " + jd.getTime());
print("L: " + jd.toLocaleString());
print("S: " + jd.toString());
print("D: " + jd.getDay());
print("G: " + jd.toGMTString());

// print("tostring begin");
// var f = jd.toLocaleString;
// print("tostring end");
// print(f());

