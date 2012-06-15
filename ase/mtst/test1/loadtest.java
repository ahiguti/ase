
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

class loadtest {

  static class testClassLoader extends java.lang.ClassLoader {
    protected Class loadClass(String name, boolean resolve)
      throws ClassNotFoundException {
      Class klass = null;
      klass = findSystemClass(name);
      if (resolve) {
	resolveClass(klass);
      }
      return klass;
    }
  };

  public static void main(String args[]) {
    testClassLoader loader = new testClassLoader();
    try {
      Class fooklass = loader.loadClass("testfoo", true);
      Object obj = fooklass.newInstance();

      Class argtypes[] = new Class[2];
      argtypes[0] = String.class;
      argtypes[1] = String.class;
      java.lang.reflect.Method mtd = fooklass.getMethod("foo", argtypes);
      Object margs[] = new Object[2];
      margs[0] = "abc";
      margs[1] = "xyz";
      mtd.invoke(obj, margs);

      java.lang.reflect.Method mbar = fooklass.getMethod("bar", null);
      Integer r = null;
      for (int i = 0; i < 10000000; ++i) {
	r = (Integer)mbar.invoke(obj, null);
      }
      System.out.println("r=" + r.intValue());

    } catch (Exception e) {
      e.printStackTrace();
    }
  }

};

