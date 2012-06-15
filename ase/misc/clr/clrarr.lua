
print("C1");
local Int32 = CLR.Load("mscorlib", "System.Int32")
local arr = Int32.NewArray(100)
for i = 0, 99 do
  arr[i] = i
end
print("C3");
for i = 0, 99 do
  print(arr[i])
end
print(Int32.MakeArrayType(10));
print("C4");

