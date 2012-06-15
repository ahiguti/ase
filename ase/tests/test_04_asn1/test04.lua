
local Test = ASE.CPP['ASETestCPP']
local x = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
print(Test.ToHEX(Test.ToDER(x)))
x = x .. x .. x .. x
print(Test.ToHEX(Test.ToDER(x)))
x = x .. x .. x .. x
print(Test.ToHEX(Test.ToDER(x)))
x = x .. x .. x .. x
print(Test.ToHEX(Test.ToDER(x)))
