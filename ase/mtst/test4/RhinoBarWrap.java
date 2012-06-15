
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

import org.mozilla.javascript.*;

public class RhinoBarWrap implements Scriptable, Callable {

  private final RhinoBar wrapped;

  public RhinoBarWrap(RhinoBar wr) {
    wrapped = wr;
  }

  public void delete(int index) { }
  public void delete(String name) { }
  public Object get(int index, Scriptable start) {
    return wrapped.getElement(index);
  }
  public Object get(String name, Scriptable start) {
    return wrapped.getProperty(name);
  }
  public String getClassName() {
    return "RhinoBarWrap";
  }
  public Object getDefaultValue(Class hint) {
    return "[object RhinoBarWrap]";
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
    System.out.println("RhinoBarWrap: has " + name);
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
  public void put(int index, Scriptable start, Object value) {
    wrapped.setElement(index, value);
  }
  public void put(String name, Scriptable start, Object value) {
    wrapped.setProperty(name, value);
  }
  public void setParentScope(Scriptable parent) {
    this.parent = parent;
  }
  public void setPrototype(Scriptable prototype) {
    this.prototype = prototype;
  }
  public Object call(Context cx, Scriptable scope, Scriptable thisobj,
    Object[] args) {
    return wrapped.invokeSelf(args);
  }

  private Scriptable prototype, parent;

}

