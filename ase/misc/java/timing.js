
var File = Java["java.io.File"];
var URL = Java["java.net.URL"];
var URLClassLoader = Java["java.net.URLClassLoader"];
var burl = File("testjar.jar").getCanonicalFile().toURL();
var urls = URL.NewArray(1);
urls[0] = burl;
var loader = URLClassLoader(urls);
var klass = loader.loadClass("testclass");
var obj = klass(100);
for (var i = 0; i < 10000000; ++i) {
  obj.add(1);
}
print(obj.getValue());

