
var EXE = CLR.GetExecutingAssembly();
var Core = CLR.GetCore();
print(Core.GetType());
var Forms = CLR.Load("System.Windows.Forms");
var asname = Core.GetName();
print(asname.GetType());
// var kp = asname.KeyPair();
// print(kp.GetClass());
// print(kp.PublicKey());
// var Core.GetClass().Load

print(EXE.FullName);
print(Core.FullName);
// print(Forms.FullName());

