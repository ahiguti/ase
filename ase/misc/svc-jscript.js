
var svcs = GetObject("winmgmts:").ExecQuery("select * from Win32_Process");
for (var e = new Enumerator(svcs); !e.atEnd(); e.moveNext()) {
  WScript.Echo("e=" + e.item().Name);
}

