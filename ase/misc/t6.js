
var globalsum = 0;

function WorkerThread(id) {
  var sum = 0;
  this.getsum = function() {
    return sum;
  }
  this.run = function() {
    var f = Math.floor;
    var x = 0;
    for (var j = 0; j < 1000000; ++j) {
      x += f(j);
if (0) {
      globalsum += r;
      if ((j & 16383) == 0) {
	gc();
	System.gc();
      }
}
    }
    sum += x;
  }
}

var num = 10;
var wkr = [];
var thr = new Array(num);
for (var i = 0; i < num; ++i) {
  wkr[i] = new WorkerThread(i);
  // thr[i] = Thread.NewInstance(wkr[i]);
}
for (var i = 0; i < num; ++i) {
  wkr[i].run();
}


var sum = 0;
for (var i = 0; i < num; ++i) {
  sum += wkr[i].getsum();
}
print("globalsum=" + globalsum);
print("localsum=" + sum);
