
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

package asejni.engines;

import java.util.*;
import asejni.*;

public class Pnuts implements LateBinding {

  private static final boolean debug = false;
  private static int root_count = 0;

  public final pnuts.lang.Package pkg;
  public final LateBinding global;
  public final LateBinding func_eval;

  private static void debugPrint(String str) {
    if (debug) {
      System.out.println(str);
    }
  }
  private static String toStringForce(Object obj) {
    try {
      if (obj == null) {
	return "[NULL]";
      }
      return obj.toString();
    } catch (Throwable t) {
      return "[EXC " + t.toString() + "]";
    }
  }

  public Pnuts() {
    debugPrint("asejni.Pnuts created");
    pkg = new pnuts.lang.Package("root" + (root_count++), null);
    global = new PnutsProxy(this, pkg);
    func_eval = new LateBindingFunctionObject(this, new FuncEvalString());
  }

  public int getAttributes() {
    return Attrib_Mutable;
  }
  public String getString() {
    return global.getString();
  }
  public Object getProperty(String name) {
    if (name.equals("EvalString")) {
      return func_eval;
    }
    return global.getProperty(name);
  }
  public void setProperty(String name, Object value) {
    debugPrint("SETPROP " + name);
    global.setProperty(name, value);
  }
  public void delProperty(String name) {
    global.delProperty(name);
  }
  public Object getElement(int idx) {
    return global.getElement(idx);
  }
  public void setElement(int idx, Object value) {
    global.setElement(idx, value);
  }
  public int getLength() {
    return -1;
  }
  public void setLength(int len) {
  }
  public Object invokeByName(String name, Object[] args) {
    if (name.equals("EvalString")) {
      return func_eval.invokeSelf(args);
    }
    return global.invokeByName(name, args);
  }
  public Object invokeSelf(Object[] args) {
    return global.invokeSelf(args);
  }
  public Object getEnumerator() {
    return global.getEnumerator();
  }
  public String enumNext() {
    return enumNext();
  }

  private interface PnutsFunctionObject {
    Object run(Pnuts pn, Object[] args);
  }

  private static class LateBindingFunctionObject extends LateBindingBase {

    public LateBindingFunctionObject(Pnuts pn, PnutsFunctionObject fobj) {
      this.pn = pn;
      this.fobj = fobj;
    }
    public int getAttributes() {
      return Attrib_Mutable | Attrib_Function;
    }
    public Object invokeSelf(Object[] args) {
      debugPrint("LBFO invokeSelf");
      Object[] pnargs = toPnuts(pn, args);
      Object pnobj = fobj.run(pn, pnargs);
      return toASE(pn, pnobj);
    }

    private final Pnuts pn;
    private final PnutsFunctionObject fobj;

  }

  private static class FuncEvalString implements PnutsFunctionObject {

    public Object run(Pnuts pn, Object[] args) {
      pnuts.lang.Context ctx = new pnuts.lang.Context(pn.pkg);
      String src = (String)args[0];
      Object r = pnuts.lang.Pnuts.eval(src, ctx);
      return r;
    }

  }

  private static Object toASE(Pnuts pn, Object obj) {
    if (obj instanceof pnuts.lang.Property ||
        obj instanceof pnuts.lang.Indexed ||
        obj instanceof pnuts.lang.Callable) {
      if (obj instanceof ASEProxy) {
	ASEProxy asepr = (ASEProxy)obj;
	return asepr.wrapped;
      }
      return new PnutsProxy(pn, obj);
    }
    return obj;
  }
  private static Object toPnuts(Pnuts pn, Object obj) {
    if (obj instanceof LateBinding) {
      if (obj instanceof PnutsProxy) {
	PnutsProxy pnpr = (PnutsProxy)obj;
	if (debug) {
	  debugPrint("toPnuts: PnutsProxy " + toStringForce(obj));
	}
	return pnpr.wrapped;
      }
      if (debug) {
	debugPrint("toPnuts: LateBinding " + toStringForce(obj));
      }
      return new ASEProxy(pn, (LateBinding)obj);
    }
    if (debug) {
      debugPrint("toPnuts: otherobj " + toStringForce(obj));
    }
    return obj;
  }
  private static Object[] toASE(Pnuts pn, Object[] objarr) {
    if (objarr == null) {
      return null;
    }
    int len = objarr.length;
    Object[] r = null;
    for (int i = 0; i < len; ++i) {
      Object e = objarr[i];
      Object ne = toASE(pn, e);
      if (e != ne) {
        if (r == null) {
          r = new Object[len];
          for (int j = 0; j < i; ++j) {
            r[j] = objarr[j];
          }
        }
      }
      if (r != null) {
        r[i] = ne;
      }
    }
    if (r != null) {
      return r;
    }
    return objarr;
  }
  private static Object[] toPnuts(Pnuts pn, Object[] objarr) {
    int len = objarr.length;
    Object[] r = null;
    for (int i = 0; i < len; ++i) {
      Object e = objarr[i];
      Object ne = toPnuts(pn, e);
      if (e != ne) {
        if (r == null) {
          r = new Object[len];
          for (int j = 0; j < i; ++j) {
            r[j] = objarr[j];
          }
        }
      }
      if (r != null) {
        r[i] = ne;
      }
    }
    if (r != null) {
      return r;
    }
    return objarr;
  }

  private static class PnutsProxy implements LateBinding {

    public final Pnuts pn;
    public final Object wrapped;

    public PnutsProxy(Pnuts pn, Object obj) {
      this.pn = pn;
      this.wrapped = obj;
    }
    private pnuts.lang.Property getIProperty() {
      return (pnuts.lang.Property)wrapped;
    }
    private pnuts.lang.AbstractData getIAbstractData() {
      return (pnuts.lang.AbstractData)wrapped;
    }
    private pnuts.lang.Indexed getIIndexed() {
      return (pnuts.lang.Indexed)wrapped;
    }
    private pnuts.lang.Callable getICallable() {
      return (pnuts.lang.Callable)wrapped;
    }
    private pnuts.lang.Context enterContext() {
      pnuts.lang.Context ctx = new pnuts.lang.Context(pn.pkg);
      if (debug) {
	debugPrint("context klassldr=" + ctx.getClassLoader());
      }
      return ctx;
    }
    public int getAttributes() {
      int r = Attrib_Mutable;
      if (getICallable() != null) {
	r |= Attrib_Function;
      }
      return r;
    }
    public String getString() {
      return wrapped.toString();
    }
    public Object getProperty(String name) {
      pnuts.lang.Property iprop = getIProperty();
      pnuts.lang.Context ctx = enterContext();
      Object pnobj = iprop.get(name.intern(), ctx);
      return toASE(pn, pnobj);
    }
    public void setProperty(String name, Object obj) {
      pnuts.lang.Property iprop = getIProperty();
      pnuts.lang.Context ctx = enterContext();
      debugPrint("PPSetProp");
      Object pnobj = toPnuts(pn, obj);
      iprop.set(name.intern(), pnobj, ctx);
    }
    public void delProperty(String name) {
      pnuts.lang.Property iprop = getIProperty();
      pnuts.lang.Context ctx = enterContext();
      iprop.set(name.intern(), null, ctx);
    }
    public Object getElement(int idx) {
      pnuts.lang.Indexed iidxd = getIIndexed();
      Object pnobj = iidxd.get(idx);
      return toASE(pn, pnobj);
    }
    public void setElement(int idx, Object obj) {
      pnuts.lang.Indexed iidxd = getIIndexed();
      debugPrint("PPSetElem");
      Object pnobj = toPnuts(pn, obj);
      iidxd.set(idx, pnobj);
    }
    public int getLength() {
      return -1;
    }
    public void setLength(int len) {
    }
    public Object invokeByName(String name, Object[] args) {
      pnuts.lang.AbstractData iad = getIAbstractData();
      pnuts.lang.Context ctx = enterContext();
      debugPrint("PPInvByName " + name);
      Object[] pnargs = toPnuts(pn, args);
      Object pnobj = iad.invoke(name.intern(), pnargs, ctx);
      return toASE(pn, pnobj);
    }
    public Object invokeSelf(Object[] args) {
      pnuts.lang.Callable icall = getICallable();
      pnuts.lang.Context ctx = enterContext();
      debugPrint("PPInvSelf");
      Object[] pnargs = toPnuts(pn, args);
      Object pnobj = icall.call(pnargs, ctx);
      return toASE(pn, pnobj);
    }
    public Object getEnumerator() {
      return null;
    }
    public String enumNext() {
      return null;
    }

  }

  private static class ASEProxy extends java.util.AbstractCollection
    implements
    // java.util.Map,
    pnuts.lang.AbstractData,
    // pnuts.lang.Indexed,
    pnuts.lang.Callable {

    public final Pnuts pn;
    public final LateBinding wrapped;

    public ASEProxy(Pnuts pn, LateBinding wrapped) {
      this.pn = pn;
      this.wrapped = wrapped;
    }
    public String toString() {
      return wrapped.getString();
    }
    public Object get(String name, pnuts.lang.Context ctx) {
      if (debug) {
	debugPrint("APGet begin " + name + " wrapped="
	  + toStringForce(wrapped));
      }
      Object aseobj = wrapped.getProperty(name);
      if (debug) {
	debugPrint("APGet " + name + " klass=" + aseobj.getClass().getName()
	  + " ret=" + toStringForce(aseobj)
	  + " klassldr=" + aseobj.getClass().getClassLoader()
	  + " wrapped=" + toStringForce(wrapped));
      }
      return toPnuts(pn, aseobj);
    }
    /*
    public Object get(int idx) {
      Object aseobj = wrapped.getElement(idx);
      return toPnuts(pn, aseobj);
    }
    */
    public void set(String name, Object value, pnuts.lang.Context ctx) {
      Object aseobj = toASE(pn, value);
      wrapped.setProperty(name, aseobj);
    }
    /*
    public void set(int idx, Object value) {
      Object aseobj = toASE(pn, value);
      wrapped.setElement(idx, aseobj);
    }
    */
    public Object invoke(String name, Object[] args, pnuts.lang.Context ctx) {
      Object[] aseargs = toASE(pn, args);
      Object aseobj = wrapped.invokeByName(name, aseargs);
      debugPrint("APInvoke " + name);
      return toPnuts(pn, aseobj);
    }
    public Object call(Object[] args, pnuts.lang.Context ctx) {
      Object[] aseargs = toASE(pn, args);
      Object aseobj = wrapped.invokeSelf(aseargs);
      debugPrint("APCall");
      return toPnuts(pn, aseobj);
    }
    /* java.util.Map */
    /*
    public void clear() {
    }
    public boolean containsKey(Object key) {
      return false;
    }
    public boolean containsValue(Object value) {
      return false;
    }
    public Set entrySet() {
      return null;
    }
    public boolean equals(Object o) {
      return this == o;
    }
    public Object get(Object key) {
      String name = (String)key;
      if (name == null) {
	return null;
      }
      Object aseobj = wrapped.getProperty(name);
      return toPnuts(pn, aseobj);
    }
    public boolean isEmpty() {
      return true;
    }
    public Set keySet() {
      return null;
    }
    public Object put(Object key, Object value) {
      String name = (String)key;
      if (name == null) {
	return null;
      }
      Object aseobj = toASE(pn, value);
      wrapped.setProperty(name, aseobj);
      return null;
    }
    public void putAll(Map t) {
    }
    public Object remove(Object key) {
      return null;
    }
    public int size() {
      return 0;
    }
    public Collection values() {
      return null;
    }
    */
    public int size() {
      /* slow */
      int c = 0;
      java.util.Iterator iter = iterator();
      while (iter.hasNext()) {
	iter.next();
	++c;
      }
      return c;
    }
    public boolean isEmpty() {
      java.util.Iterator iter = iterator();
      return iter.hasNext();
    }
    public java.util.Iterator iterator() {
      LateBinding iter = (LateBinding)wrapped.getEnumerator();
      return new ASEProxyIterator(pn, iter);
    }

  }

  private static class ASEProxyIterator implements java.util.Iterator {

    public final LateBinding wrappediter;
    String next;

    public ASEProxyIterator(Pnuts pn, LateBinding wrappediter) {
      this.wrappediter = wrappediter;
      this.next = this.wrappediter.enumNext();
    }

    public boolean hasNext() {
      return next != null;
    }
    public Object next() {
      String r = next;
      next = this.wrappediter.enumNext();
      return r;
    }
    public void remove() {
    }

  }

}

