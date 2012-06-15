
local File = Java["java.io.File"];
local arr = File.NewArrayValue(File("foo.txt"), null, File("bar.txt"));
print(arr.length);
print(arr[0].toURL().toString());
print(arr[2].toURL().toString());

