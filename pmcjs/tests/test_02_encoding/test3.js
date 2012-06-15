
var pobj = Perl.NewHash();
pobj["あいうえお"] = "かきくけこ";
//pobj.SetProperty('a', 15);
pobj.SetProperty("さしすせそ", "たちつてと");
for (var i in pobj) {
  print("key=[" + i + "] val=[" + pobj[i] + "]");
  for (var j = 0; j < i.length; ++j) {
    print(i.charCodeAt(j).toString(16));
  }
}
