
local File = Java["java.io.File"];
local URL = Java["java.net.URL"];
local URLClassLoader = Java["java.net.URLClassLoader"];
local burl = File("testjar.jar").getCanonicalFile().toURL();
local urls = URL.NewArray(1);
urls[0] = burl;
local loader = URLClassLoader(urls);
local klass = {}
ASELuaUtil.Import(klass, loader.loadClass("testclass"), "");
local obj = klass.NewInstance(100);
for i = 1, 10000000 do
  klass.add(obj, 1);
end
print(obj.getValue());

