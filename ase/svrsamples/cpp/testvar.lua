
local Test = ASE.CPP['ASETestCPP']
local o1 = Test.Create()
local o2 = Test.Create()
local a = { 10, 20 }
o1.ObjRef(o2);
o1.ObjCRef(o2);
o1.VariantRef(a);
o1.VariantCRef(a);
