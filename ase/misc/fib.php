<?

function fib($x) {
  if ($x <= 1) {
    return 1;
  } else {
    return fib($x - 2) + fib($x - 1);
  }
}

echo fib(30);

?>
