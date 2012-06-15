
local Random = CLR.Load("mscorlib", "System.Random");
local robj = Random(15);
local i, r;
for i = 1, 1000000 do
  r = robj.Next();
end
print(r);

