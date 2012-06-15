
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

import org.mozilla.javascript.*;
import java.io.*;

public class RhinoTest {

  static class MyWrapFactory extends WrapFactory {
    public Object wrap(Context cx, Scriptable scope, Object obj,
      Class staticType) {
      System.out.println("wrap " + obj);
      if (obj instanceof RhinoBar) {
	return new RhinoBarWrap((RhinoBar)obj);
      }
      return super.wrap(cx, scope, obj, staticType);
    }
    public Scriptable wrapNewObject(Context cx, Scriptable scope, Object obj) {
      System.out.println("wrapNewObject " + obj);
      return super.wrapNewObject(cx, scope, obj);
    }
  }

  public static void main(String args[]) {
    Context cx = Context.enter();
    cx.setWrapFactory(new MyWrapFactory());
    try {
      Scriptable scope = cx.initStandardObjects();
      scope.put("rhinofoo", scope, new RhinoFoo());
      scope.put("rhinobar", scope, new RhinoBar());
      try {
	ScriptableObject.defineClass(scope, RhinoFoo.class);
      } catch (IllegalAccessException e) {
	e.printStackTrace();
      } catch (InstantiationException e) {
	e.printStackTrace();
      } catch (java.lang.reflect.InvocationTargetException e) {
	e.printStackTrace();
      }
      String s = "";
      try {
	FileReader f = new FileReader(args[0]);
	char buf[] = new char[65536];
	int len = 0;
	while (true) {
	  len = f.read(buf, 0, 65536);
	  if (len <= 0) {
	    break;
	  }
	  s += new String(buf, 0, len);
	}
      } catch (IOException e) {
	e.printStackTrace();
      }
      Object result = cx.evaluateString(scope, s, "<cmd>", 1, null);
      System.err.println(cx.toString(result));
    } finally {
      Context.exit();
    }
  }

}
