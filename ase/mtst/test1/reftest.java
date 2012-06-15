
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

import java.lang.reflect.*;

public class reftest {
  public static void main(String[] args) {
    try {
      // Class klass = Void.TYPE;
      Class klass = Class.forName("testfoo");
      // String sx = "";
      // Class klass = sx.getClass();

      Field[] flds = klass.getFields();
      for (int i = 0; i < flds.length; ++i) {
	Field fld = flds[i];
	String s = "";
	s += Modifier.toString(fld.getModifiers());
	s += " " + fld.getType().getName();
	s += " " + fld.getName();
	s += ";";
	System.out.println(s);
      }

      Constructor[] ctors = klass.getConstructors();
      for (int i = 0; i < ctors.length; ++i) {
	Constructor ctor = ctors[i];
	String s = "";
	s += Modifier.toString(ctor.getModifiers());
	s += " " + ctor.getDeclaringClass().getName();
	s += "(";
	Class[] params = ctor.getParameterTypes();
	for (int j = 0; j < params.length; ++j) {
	  s += " " + params[j].getName();
	}
	s += " );";
	System.out.println(s);
      }

      Method[] methods = klass.getMethods();
      for (int i = 0; i < methods.length; ++i) {
	Method mtd = methods[i];
	String s = "";
	s += Modifier.toString(mtd.getModifiers());
	s += " " + mtd.getReturnType().getName();
	s += " " + mtd.getName();
	s += "(";
	Class[] params = mtd.getParameterTypes();
	for (int j = 0; j < params.length; ++j) {
	  s += " " + params[j].getName();
	}
	s += " );";
	System.out.println(s);
      }

    } catch (Exception e) {
      e.printStackTrace();
    }
  }
}

