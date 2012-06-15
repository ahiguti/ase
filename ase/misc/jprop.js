
var props = GCJ["java.lang.System"].getProperties();
print(props.getProperty("java.vm.name") + ' ' +
  props.getProperty("java.version"));

