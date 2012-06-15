
local File = Java["java.io.File"];
local URL = Java["java.net.URL"];
local burl = File("testjar.jar").getCanonicalFile().toURL();
local burl2 = File("testjar.jar").getCanonicalFile().toURL();
local urls = URL.NewArray(1);
urls[0] = burl;
local x = urls[0];
local y = urls[0];
print(x == y);
print(burl == burl2);
print(burl == y);

