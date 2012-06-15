
Perl.Use("JSObjTest");
var jo = Perl.Functions["JSObjTest"];

var obj = {
  海豹 : 'これは海豹',
  魑魅魍魎 : 'これは魑魅魍魎',
  イヌワシ : 'これはイヌワシ',
  プロパティ : 'これはプロパティ',
  foo : function(x, y, z) { return x + y + z; },
  手続き : function(x, y, z) { return x * y * z; }
};

function dump() {
  for (var i in obj) {
    print("key=[" + i + "] val=[" + obj[i] + "]");
  }
}

//dump();
print("getprop: ", jo.jsobj_getprop(obj, "海豹"));
print("getprop_smart: ", jo.jsobj_getprop_smart(obj, "海豹"));
jo.jsobj_setprop(obj, "イヌワシ", 99);
jo.jsobj_setprop_smart(obj, "魑魅魍魎", 123);
print("setprop: ", obj.イヌワシ);
print("setprop_smart: ", obj.魑魅魍魎);
print("invoke: ", jo.jsobj_invoke_手続き(obj, 2, 3, 4));
print("invoke_smart: ",
  jo.jsobj_invoke_手続き_smart(obj, 15, 2, 7));
//dump();

