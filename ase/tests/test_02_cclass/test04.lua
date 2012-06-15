
local Test = ASE.CPP['ASETestCPP']
local o1 = Test.Create()
local o2 = Test.Create()
o1.ObjRef(o2);
o1.ObjCRef(o2);
o1.VariantRef("AA");
o1.VariantCRef("BBB");
