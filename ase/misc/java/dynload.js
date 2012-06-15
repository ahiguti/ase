
var File = Java["java.io.File"];
var URL = Java["java.net.URL"];
var URLClassLoader = Java["java.net.URLClassLoader"];
var burl = File("testjar.jar").getCanonicalFile().toURL();
print("URL=" + burl);
var loader = URLClassLoader(URL.NewArrayValue(burl));
var klass = loader.loadClass("testclass");
var obj = klass(100);
obj.add(1);
print(obj.getValue());

