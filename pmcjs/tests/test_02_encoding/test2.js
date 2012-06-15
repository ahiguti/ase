
Perl.Use("TextTest");
var tt = Perl.Functions["TextTest"];
tt.setbinmode();
var txt = "あいうえお";
var s = tt.print(txt);
dump(s);
print();
var txtwrap = Perl.NewScalar();
txtwrap.SetText(txt);
s = tt.print(txtwrap);
dump(s);
print();
var binwrap = Perl.NewScalar();
binwrap.SetBinary(txt);
s = tt.print(binwrap);
dump(s);
print();
