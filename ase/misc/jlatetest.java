
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

import asejni.*;

public class jlatetest implements LateBinding {

  public jlatetest(LateBinding globals, int x, String y) {
    System.out.println("jlatetest constr x=" + x + " y=" + y);
    LateBinding Native = (LateBinding)globals.getProperty("Native");
    LateBinding Util = (LateBinding)Native.getProperty("Util");
    LateBinding UUID = (LateBinding)Util.getProperty("UUID");
    LateBinding uuid = (LateBinding)UUID.invokeSelf(null);
    String s = (String)uuid.invokeByName("upper", new LateBinding[]{ uuid });
    System.out.println("uuid: " + s);
  }

  public int getAttributes() {
    System.out.println("jlatetest getAttributes");
    return Attrib_Mutable;
  }
  public String getString() {
    System.out.println("jlatetest getString");
    return "jlatetest getString";
  }
  public Object getProperty(String name) {
    System.out.println("jlatetest getProperty name=" + name);
    return "jlatetest property name=" + name;
  }
  public void setProperty(String name, Object value) {
    System.out.println("jlatetest name=" + name + " value="
      + value.toString());
  }
  public void delProperty(String name) {
    System.out.println("jlatetest delProperty name=" + name);
  }
  public Object getElement(int idx) {
    System.out.println("jlatetest getElement idx=" + idx);
    return "jlatetest element idx=" + idx;
  }
  public void setElement(int idx, Object value) {
    System.out.println("jlatetest idx=" + idx + " value="
      + value.toString());
  }
  public int getLength() {
    System.out.println("jlatetest getLength");
    return -1;
  }
  public void setLength(int len) {
    System.out.println("jlatetest setLength len=" + len);
  }
  public Object invokeByName(String name, Object[] args) {
    System.out.println("jlatetest invokeByName name=" + name);
    return null;
  }
  public Object invokeSelf(Object[] args) {
    System.out.println("jlatetest invokeSelf");
    return null;
  }
  public Object getEnumerator() {
    System.out.println("jlatetest getEnumerator");
    return null;
  }
  public String enumNext() {
    System.out.println("jlatetest enumNext");
    return null;
  }

  static public void main(LateBinding globals, LateBinding args) {
    int nargs = args.getLength();
    System.out.println("jlatetest main nargs=" + nargs);
    for (int i = 0; i < nargs; ++i) {
      System.out.println("arg " + (String)args.getElement(i));
    }
    LateBinding Native = (LateBinding)globals.getProperty("Native");
    LateBinding Util = (LateBinding)Native.getProperty("Util");
    LateBinding UUID = (LateBinding)Util.getProperty("UUID");
    LateBinding uuid = (LateBinding)UUID.invokeSelf(null);
    String s = (String)uuid.invokeByName("lower", new LateBinding[]{ uuid });
    System.out.println("uuid: " + s);
  }

  static {
    System.out.println("jlatetest static init");
  }

}

