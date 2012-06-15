
function xfunc(f) {
  // print('p1');
  f();
  // print('p2');
}

function dummyfunc() {
}

xfunc(dummyfunc);

