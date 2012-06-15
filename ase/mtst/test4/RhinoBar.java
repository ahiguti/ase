
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

public class RhinoBar {

  public void delElement(int index) { }
  public void delProperty(String name) { }
  public Object getElement(int index) {
    return null;
  }
  public Object getProperty(String name) {
    System.out.println("RhinoBar: get " + name);
    return "BARGETRETVAL";
  }
  public void setElement(int index, Object value) { }
  public void setProperty(String name, Object value) {
    System.out.println("RhinoBar: put " + name);
    System.out.println("value class: " + value.getClass().getName());
    System.out.println("value: " + value.toString());
  }
  public Object invokeSelf(Object[] args) {
    System.out.println("RhinoBar: call");
    for (int i = 0; i < args.length; ++i) {
      System.out.println("RhinoBar: arg: " + args[i].toString());
    }
    return "rhinobarcallretval";
  }

}

