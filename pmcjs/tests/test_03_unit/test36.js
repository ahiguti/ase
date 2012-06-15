
use('Perl');
Perl.Use('ProtoTest');

var pt = Perl.Functions['ProtoTest'];
var arr = Perl.NewArray([10, 20, 30]);
pt.mypush(arr, 98, 99);
for (var i = 0; i < arr.length; ++i) {
  print(arr[i]);
}

