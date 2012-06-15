
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

import org.mozilla.javascript.*;

public class RhinoFoo implements Scriptable, Callable {

  public void delete(int index) { }
  public void delete(String name) { }
  public Object get(int index, Scriptable start) {
    return NOT_FOUND;
  }
  public Object get(String name, Scriptable start) {
    System.out.println("RhinoFoo: get " + name);
    return "FOOGETRETVAL";
  }
  public String getClassName() {
    return "RhinoFoo";
  }
  public Object getDefaultValue(Class hint) {
    return "[object RhinoFoo]";
  }
  public Object[] getIds() {
    return new Object[0];
  }
  public Scriptable getParentScope() {
    return parent;
  }
  public Scriptable getPrototype() {
    return prototype;
  }
  public boolean has(int index, Scriptable start) {
    return false;
  }
  public boolean has(String name, Scriptable start) {
    System.out.println("RhinoFoo: has " + name);
    return false;
  }
  public boolean hasInstance(Scriptable instance) {
    for (Scriptable proto = instance.getPrototype(); proto != null;
      proto = proto.getPrototype()) {
      if (proto.equals(this)) {
	return true;
      }
    }
    return false;
  }
  public void put(int index, Scriptable start, Object value) { }
  public void put(String name, Scriptable start, Object value) {
    System.out.println("RhinoFoo: put " + name);
    System.out.println("value class: " + value.getClass().getName());
    System.out.println("value: " + value.toString());
  }
  public void setParentScope(Scriptable parent) {
    this.parent = parent;
  }
  public void setPrototype(Scriptable prototype) {
    this.prototype = prototype;
  }
  public Object call(Context cx, Scriptable scope, Scriptable thisobj,
    Object[] args) {
    System.out.println("RhinoFoo: call");
    for (int i = 0; i < args.length; ++i) {
      System.out.println("RhinoFoo: arg: " + args[i].toString());
    }
    return "rhinofoocallretval";
  }

  private Scriptable prototype, parent;

}

