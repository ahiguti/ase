
use('Perl');
Perl.Use("FileHandle");
var fh = new Perl("FileHandle", "sampleutf.txt", "r");
var str_utf;
var buf = Perl.NewScalar();
while (true) {
  var n = fh.read(buf, 1024);
  if (!n) {
    break;
  }
  str_utf += buf.GetBinary();
}
fh.close();
str_utf = Perl.BinToText(str_utf);
print("buftxt: [" + str_utf + "]\n");

var buf = Perl.NewScalar();
buf.Set(str_utf);
Perl.Use("Encode");
var encode = Perl.Functions["Encode"].encode;
// print(str_utf);
var str_euc = encode("euc-jp", str_utf);
fh = new Perl("FileHandle", "sampleeuc.log", "w");
fh.print(str_euc);
fh.close();

