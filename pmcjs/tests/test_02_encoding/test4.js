
function dump(txt) {
  for (var i = 0; i < txt.length; ++i) {
    print(txt.charCodeAt(i).toString(16));
  }
}

var txt = "あいうえお";
print("step1:");
dump(txt);
txt = Perl.TextToBin(txt);
print("step2:");
dump(txt);
txt = Perl.BinToText(txt);
print("step3:");
dump(txt);
