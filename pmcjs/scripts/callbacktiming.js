
var x = 0;

var obj = {
  foo : function() { }
};

use('Perl');
Perl.Use("Callback");
Perl.Functions["Callback"].callback(obj);
print(x);
