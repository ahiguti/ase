
local Math = CLR.Load("mscorlib", "System.Math")
local i, r
for i = 1, 1000000 do
  r = Math.Sqrt(3)
end
print(r)

