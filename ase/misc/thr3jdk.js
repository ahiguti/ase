
var Thread = Java['java.lang.Thread'];
var System = Java['java.lang.System'];

var globalsum = 0;

function WorkerThread(id) {
  var sum = 0;
  this.getsum = function() {
    return sum;
  }
  this.run = function() {
    var m = Math;
    var s = 0;
    var idx = id;
    for (var j = 0; j < 1000000; ++j) {
      s += j;
      // s += m.floor(j);
      // globalsum += j;
      if (idx == 0 && (j & 65535) == 0) {
	gc();
	System.gc();
      }
    }
    sum += s;
  }
}

var num = 10;
var wkr = [];
var thr = new Array(num);
for (var i = 0; i < num; ++i) {
  wkr[i] = new WorkerThread(i);
  thr[i] = Thread.NewInstance(wkr[i]);
}
try {
  for (var i = 0; i < num; ++i) {
    thr[i].start();
  }
} finally {
  for (var i = 0; i < num; ++i) {
    thr[i].join();
    print("id=" + i + " join done");
  }
}

var sum = 0;
for (var i = 0; i < num; ++i) {
  sum += wkr[i].getsum();
}
print("globalsum=" + globalsum);
print("localsum=" + sum);

