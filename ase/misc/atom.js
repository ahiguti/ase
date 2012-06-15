
var obj = {};
var i = 0;
while (true) {
  for (var j = 0; j < 10000; ++j) {
    ++i;
    var s = "x" + i;
    obj[s] = i;
    delete obj[s];
  }
  gc();
  print(i);
}
