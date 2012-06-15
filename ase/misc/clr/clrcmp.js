
var hndlr = {
  'CompareTo' : function(x) {
    print("CompareTo: x=" + x);
    return -1;
  }
};
var IComparable = CLR.Load("mscorlib", "System.IComparable");
var dprx = IComparable(hndlr);
dprx.CompareTo(30);
