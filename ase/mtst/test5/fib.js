
function fib(n) {
  if (n < 2) { return 1; }
  return fib(n - 2) + fib(n - 1);
}

var r = fib(32);
print(r);

