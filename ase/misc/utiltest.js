
var Test = Native.Util.Test;
var r = 0;
for (var i = 0; i < 10000000; ++i) {
  r = Test.Sum(10, 20, 30);
}
print(r);

