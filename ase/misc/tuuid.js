
var UUID = Native.Util.UUID;
for (var i = 0; i < 10; ++i) {
  var x = UUID();
  print(x.upper());
}

