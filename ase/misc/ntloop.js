
var Math = Java['java.lang.Math'];
var Thread = Native.Util.Thread;

function Worker(id) {
  return function() {
    r = Math.abs(-99);
    // print(r);
  }
}

var loop = 10000;
for (var i = 0; i < loop; ++i) {
  var thr = Thread(Worker(i));
  try {
    thr.start();
  } finally {
    thr.join();
  }
}

