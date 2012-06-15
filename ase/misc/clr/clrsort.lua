
local hndlr = {
  CompareTo = function(x) 
    print("CompareTo: x=" .. x)
    return 1
  end
}
print("C1");
local IComparable = CLR.Load("mscorlib", "System.IComparable")
print("C2");
local arr = IComparable.NewArray(100)
local dprx = IComparable(hndlr)
print("C3");
dprx.CompareTo(30);
print("C4");
