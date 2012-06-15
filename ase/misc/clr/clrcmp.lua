
local hndlr = {
  CompareTo = function(x) 
    print("CompareTo: x=" .. x)
    return 1
  end,
  FOOBAR = function()
    print("FOOBAR called")
  end
}
print("C1");
local IComparable = CLR.Load("mscorlib", "System.IComparable")
print("C2");
local dprx = IComparable(hndlr)
print("C3");
dprx.CompareTo(30);
dprx.FOOBAR(30);
print("C4");
