
var filename = "testjar";
var loader = Java.java.net.URLClassLoader(
  Java.java.net.URL.NewArrayValue(
    Java.java.io.File(filename).getCanonicalFile().toURL()));
var testclass = loader.loadClass("testclass");
var obj = testclass(100);
obj.add(1);
print(obj.getValue());

