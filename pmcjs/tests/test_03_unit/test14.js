
use('Perl');
Perl.Use("SortTest");
var arr = Perl.NewArray();
arr[0] = "abc";
arr[1] = "a";
arr[2] = "0";
arr[3] = "xyz";
arr[4] = "ABC";
var cmpf = function(a, b) {
  if (a < b) { return 1; }
  if (a > b) { return -1; }
  return 0;
}
var srt = new Perl("SortTest", cmpf);
var r = srt(arr);
for (var i = 0; i < r.length; ++i) {
  print(r[i]);
}

