
for (var i = 0; i < 10000000; ++i) {
  var x = Components.classes["txpc"].createInstance();
  x.setVerbose(0);
  var cb = {
    QueryInterface: function() { return this; },
    callback: function() { print("foo"); },
    obj: x /* cycle */
  };
  x.saveCallback(cb, 1); /* weak reference. does not leak. */
  // x.saveCallback(cb, 0); /* strong reference. leaks. */
}

