
var Thread = Java['java.lang.Thread'];
var System = Java['java.lang.System'];
var Runnable = Java['java.lang.Runnable'];

function WorkerThread(id) {
  this.sum = 0;
  this.run = function() {
    for (var j = 1; j <= 100000000; ++j) {
      this.sum += j;
    }
    if ((j & 16383) == 0) {
      gc();
      System.gc();
    }
  }
}

var num = 5;
var wkr = [];
var thr = new Array(num);
for (var i = 0; i < num; ++i) {
  wkr[i] = new WorkerThread(i);
  thr[i] = Thread.NewInstance(Runnable(wkr[i]));
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
  sum += wkr[i].sum;
}
print("sum=" + sum);

