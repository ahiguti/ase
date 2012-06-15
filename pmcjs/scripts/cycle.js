
/* SKIP_CHECKMEM */

use('Perl');

Perl.Use("CycleTest");
for (var i = 0; i < 100000; ++i) {
  var jso = { abc : 'val abc', foo : function() { return 100; } };
  jso.pobj = new Perl("CycleTest", jso);
  /* jso leaks because of cyclic reference between jso and pobj */

  // jso.pobj = null;

  // Perl.DisposeAll();
}

