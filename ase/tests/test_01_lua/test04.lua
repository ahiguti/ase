
local Test = Native.Util.Test
local Test2 = Native.Util.Test
local UUID = Native.Util.UUID
local inst1 = Test();
local inst2 = Test();
local inst3 = UUID();

print("testclass:");
print(Test.IsUtilTestClass(Test));
print(Test.IsUtilTestClass(Test2));
print(Test.IsUtilTestClass(UUID));
print(Test.IsUtilTestClass(inst1));
print(Test.IsUtilTestClass(inst2));
print(Test.IsUtilTestClass(inst3));
print("testinstance:");
print(Test.IsUtilTestInstance(Test));
print(Test.IsUtilTestInstance(Test2));
print(Test.IsUtilTestInstance(UUID));
print(Test.IsUtilTestInstance(inst1));
print(Test.IsUtilTestInstance(inst2));
print(Test.IsUtilTestInstance(inst3));

