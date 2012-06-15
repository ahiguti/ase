
import System;

var robj = new System.Random(15);
var r : int = 0;
for (var i : int = 0; i < 1000000; ++i) {
  r = robj.Next();
}
print(r);

