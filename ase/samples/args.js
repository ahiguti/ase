
var args = ASE.Host.Args;
print("nargs=" + args.Length);
for (var i = 0; i < args.Length; ++i) {
  print("arg " + i + " " + args[i]);
}

