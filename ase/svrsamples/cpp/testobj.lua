
local Test = ASE.CPP['ASETestCPP']
local o1 = Test.Create()
o1.SetInt(3)
local o2 = Test.Create()
o2.SetInt(123)
o1.ObjRef(o2);
o1.ObjCRef(o2);
o1.VariantRef("AAAA");
o1.VariantCRef("AAAA");
-- print(o1.GetInt())

