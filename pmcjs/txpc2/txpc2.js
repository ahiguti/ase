
var x = Components.classes["txpc"].createInstance();

x.setValue(100);
print("getValue: " + x.getValue());

new function() {
  var cb = {
    QueryInterface: function(aIID) {
      //if (aIID.equals(Components.interfaces.ITXPCCallback) ||
      //    aIID.equals(Components.interfaces.nsISupportsWeakReference) ||
      //    aIID.equals(Components.interfaces.nsISupports))
      //  return this;
      //throw Components.results.NS_NOINTERFACE;
      return this;
    },
    callback : function() { print("JS function called"); }
  };
  x.saveCallback(cb, 1);
  x.doSavedCallback(3);
  gc();
}();

gc();
x.doSavedCallback(3);
//x.clearSavedCallback();

