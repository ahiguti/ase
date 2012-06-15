
import asejni.*;

public class javainv {

  public static void main(String[] args) {
    System.out.println("javainv main");
    Object global = asejni.Runtime.getGlobal();
    Object host = Variant.getProperty(global, "Host");
    Object func = Variant.invokeByName(host, "Load",
      new Object[] { "fibfunc.lua" });
    Object rval = Variant.invokeSelf(func, new Object[] { 10 });
    System.out.println(rval);
  }

}

