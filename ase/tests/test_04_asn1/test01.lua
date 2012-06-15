
local Test = ASE.CPP['ASETestCPP']
local arr = { "foo", "foobar", "baz" }
local s = Test.ToXML(arr)
print(s)

