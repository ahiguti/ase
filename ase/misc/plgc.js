
var loop = 1000000;
var numthreads = 2;
var dumplp = true;
var dumpcb = true;

Perl.Use('plgcsub');
var x = new Perl('plgcsub', cbfunc);
x.foo();
x.foo();
x.foo();

var Thread = GCJ['java.lang.Thread'];
var System = GCJ['java.lang.System'];

function WorkerThread(id) {
  this.run = function() {
    for (var j = 0; j < loop; ++j) {
      if (dumplp) {
	print("id=" + id + " j=" + j);
      }
      // hangs
      var x = new Perl('plgcsub', cbfunc);
      x.foo();
      if (0) {
      if ((j & 4095) == 0) {
	System.gc();
	print("js gc");
	gc();
      }
      } /* 0 */
    }
  }
}

function cbfunc() {
  if (dumpcb) {
    print("cbfunc called");
  }
}

if (1) {
var arr = new Array(numthreads);
for (var i = 0; i < numthreads; ++i) {
  arr[i] = Thread.NewInstance(new WorkerThread(i));
}
for (var i = 0; i < numthreads; ++i) {
  arr[i].start();
}
for (var i = 0; i < numthreads; ++i) {
  arr[i].join();
  print("id=" + i + " join done");
}
}
