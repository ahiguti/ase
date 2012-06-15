
var Thread = Java['java.lang.Thread'];
var System = Java['java.lang.System'];

var globalsum = 0;

function WorkerThread(id) {
  var sum = 0;
  this.getsum = function() {
    return sum;
  }
  this.run = function() {
    for (var j = 0; j < 100000; ++j) {
      var r = 0;
      for (var k = 0; k < 100; ++k) {
	r += k;
      }
      sum += r;
      globalsum += r;
      if (id == 0 && (j & 16383) == 0) {
	gc();
	System.gc();
      }
    }
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

