
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

import pnuts.lang.*;
import java.util.*;

public class pnutstest {

  public static void main(String args[]) {
    try {
      pnuts.lang.Package pkg = new pnuts.lang.Package("root", null);
      Context ctx = new Context(pkg);
      Object r = Pnuts.loadFile(args[0], ctx);
      if (r != null) {
	System.out.println("r=" + r.toString());
      }
      Object f = pkg.get("fib", ctx);
      System.out.println("f=" + f.getClass());
      Callable pf = (Callable)f;
      Object fr = pf.call(new Object[] { 20 }, ctx);
      System.out.println("fr=" + fr.getClass());
      Integer iv = (Integer)fr;
      System.out.println("iv=" + iv.intValue());
    } catch (Throwable e) {
      e.printStackTrace();
    }
  }

}

