
var File = Java["java.io.File"];
var arr = File.NewArrayValue(File("foo.txt"), null, File("bar.txt"));
print(arr.length);
// print(Native.Util.Meta.GetLength(arr));
print(arr[0].toURL().toString());
print(arr[2].toURL().toString());

