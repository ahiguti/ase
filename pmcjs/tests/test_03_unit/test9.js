
try {
  Perl.Functions[""].abc();
} catch (e) {
  print("type=" + e.etype);
  print("error=" + e.error);
  print("func=" + e.func);
  print("description=" + e.description);
  print("string=" + e);
}

