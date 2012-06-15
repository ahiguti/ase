
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

package asejni.engines;

import org.mozilla.javascript.*;
import java.util.*;
import asejni.*;

public class Rhino implements LateBinding {

  private static final boolean debug = false;

  private final RhinoProxy global;
  private final LateBinding eval_func;
  private final Object js_undefined_value;

  public Rhino() {
    if (debug) {
      System.out.println("asejni.Rhino created");
    }
    Context cx = Context.enter();
    try {
      Scriptable scope = cx.initStandardObjects();
      ScriptableObject.defineProperty(scope, "print",
	new FunctionObjectPrint(),
	ScriptableObject.PERMANENT | ScriptableObject.READONLY);
      global = new RhinoProxy(scope, null);
      eval_func = new LateBindingFunctionObject(scope, new FuncEvalString());
      js_undefined_value = cx.getUndefinedValue();
    } finally {
      cx.exit();
    }
    if (debug) {
      System.out.println("asejni.Rhino global=" + global);
    }
  }
  public int getAttributes() {
    return global.getAttributes();
  }
  public String getString() {
    return global.getString();
  }
  public Object getProperty(String name) {
    if (name.equals("EvalString")) {
      return eval_func;
    }
    if (debug) {
      System.out.println("RhinoGlobal getprop " + name + " self=" + this);
    }
    Object r = global.getProperty(name);
    if (debug) {
      System.out.println("RhinoGlobal getprop " + name + " value=" +
	r.toString() + " self=" + this);
    }
    return r;
  }
  public void setProperty(String name, Object value) {
    if (debug) {
      System.out.println("RhinoGlobal setprop " + name);
	/* Note: don't show 'value', or getString() is called on it */
    }
    global.setProperty(name, value);
  }
  public void delProperty(String name) {
    if (debug) {
      System.out.println("RhinoGlobal delprop " + name);
    }
    global.delProperty(name);
  }
  public Object getElement(int idx) {
    if (debug) {
      System.out.println("RhinoGlobal getelem " + idx);
    }
    return null;
  }
  public void setElement(int idx, Object value) {
    if (debug) {
      System.out.println("RhinoGlobal setelem " + idx + " value=" + value);
    }
  }
  public int getLength() {
    if (debug) {
      System.out.println("RhinoGlobal getlen ");
    }
    return -1;
  }
  public void setLength(int len) {
    if (debug) {
      System.out.println("RhinoGlobal setlen " + len);
    }
  }
  public Object invokeByName(String name, Object[] args) {
    if (debug) {
      System.out.println("RhinoGlobal invokebyname");
    }
    if (name.equals("EvalString")) {
      return eval_func.invokeSelf(args);
    }
    return global.invokeByName(name, args);
  }
  public Object invokeSelf(Object[] args) {
    if (debug) {
      System.out.println("RhinoGlobal invokeself");
    }
    return global.invokeSelf(args);
  }
  public Object getEnumerator() {
    if (debug) {
      System.out.println("RhinoGlobal getEnumerator");
    }
    return null;
  }
  public String enumNext() {
    if (debug) {
      System.out.println("RhinoGlobal enumNext");
    }
    return null;
  }

  private static Object toASE(Scriptable scope, Object obj) {
    if (obj instanceof Callable) {
      if (obj instanceof ASEProxy) {
	ASEProxy asepr = (ASEProxy)obj;
	if (debug) {
	  System.out.println("TOASE: asepr wrapped " + asepr);
	}
	return asepr.wrapped;
      }
      Callable cobj = (Callable)obj; 
      if (debug) {
	System.out.println("TOASE: new callable " + cobj);
      }
      return new RhinoProxy(scope, cobj);
    }
    if (obj instanceof Scriptable) {
      Scriptable sobj = (Scriptable)obj;
      if (debug) {
	System.out.println("TOASE: new scriptable " + sobj);
      }
      return new RhinoProxy(sobj, null);
    }
    if (obj == Context.getUndefinedValue()) {
      return null;
    }
    return obj;
  }
  private static Object toRhino(Object obj) {
    if (obj == null) {
      return Context.getUndefinedValue();
    }
    if (obj instanceof LateBinding) {
      if (obj instanceof RhinoProxy) {
	RhinoProxy rcpr = (RhinoProxy)obj;
	if (rcpr.callable == null) {
	  return rcpr.scriptable;
	} else {
	  return rcpr.callable;
	}
      }
      return new ASEProxy((LateBinding)obj);
    }
    return obj;
  }
  private static Object[] toASE(Scriptable scope, Object[] objarr) {
    if (objarr == null) {
      return null;
    }
    int len = objarr.length;
    Object[] r = null;
    for (int i = 0; i < len; ++i) {
      Object e = objarr[i];
      Object ne = toASE(scope, e);
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
  private static Object[] toRhino(Object[] objarr) {
    if (objarr == null) {
      return null;
    }
    int len = objarr.length;
    Object[] r = null;
    for (int i = 0; i < len; ++i) {
      Object e = objarr[i];
      Object ne = toRhino(e);
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

  private interface RhinoFunctionObject {
    Object run(Context cx, Scriptable scope, Object[] args);
  }

  private static class LateBindingFunctionObject extends LateBindingBase {

    public LateBindingFunctionObject(Scriptable scope,
      RhinoFunctionObject fobj) {
      this.scope = scope;
      this.fobj = fobj;
      if (debug) {
	System.out.println("LBFO created thisldr="
	  + this.getClass().getClassLoader());
      }
    }
    public int getAttributes() {
      return Attrib_Mutable | Attrib_Function;
    }
    public Object invokeSelf(final Object[] args) {
      return Context.call(new ContextAction() {
	public Object run(Context cx) {
	  Object[] rhargs = toRhino(args);
	  Object rhrval = fobj.run(cx, scope, rhargs);
	  return toASE(scope, rhrval);
	}
      });
    }

    private final Scriptable scope;
    private final RhinoFunctionObject fobj;

  }

  private static class FunctionObjectPrint extends ScriptableObject
    implements Function {
    public String getClassName() {
      return "FunctionObjectPrint";
    }
    public Object call(Context cx, Scriptable scope, Scriptable thisobj,
      Object[] args) {
      StringBuilder sb = new StringBuilder();
      for (int i = 0; i < args.length; ++i) {
	sb.append(cx.toString(args[i]));
      }
      System.out.println(sb.toString());
      return null;
    }
    public Scriptable construct(Context cx, Scriptable scope, Object[] args) {
      return null;
    }
  }

  private static class FuncEvalString implements RhinoFunctionObject {
    public Object run(Context cx, Scriptable scope, Object[] args) {
      if (debug) {
	System.out.println("Rhino.EvalString invokeSelf");
      }
      final int nargs = args.length;
      String src = (String)args[0];
      String filename = "<eval>";
      int lineno = 1;
      if (nargs >= 2) {
	filename = (String)args[1];
      }
      if (nargs >= 3) {
	lineno = ((Integer)args[2]).intValue();
      }
      Object r = cx.evaluateString(scope, src, filename, lineno,
	null);
      if (debug) {
	System.out.println("Rhino.EvalString invokeSelf r=" + r);
      }
      return r;
    }
  }

  private static class NewInstance extends LateBindingBase {

    public NewInstance(Scriptable scope, Function func) {
      if (debug) {
	System.out.println("Rhino.NewInstance constr");
      }
      this.scope = scope;
      this.func = func;
    }
    public int getAttributes() {
      return Attrib_Mutable | Attrib_Function;
    }
    public Object invokeSelf(final Object[] args) {
      return Context.call(new ContextAction() {
	public Object run(Context cx) {
	  if (debug) {
	    System.out.println("Rhino.NewInstance invokeSelf");
	  }
	  Object[] rhargs = toRhino(args);
	  Object rhrval = func.construct(cx, scope, rhargs);
	  Object r = toASE(scope, rhrval);
	  if (debug) {
	    System.out.println("Rhino.NewInstance invokeSelf r=" + r);
	  }
	  return r;
	}
      });
    }

    private final Scriptable scope;
    private final Function func;

  }

  private static class RhinoEnumerator extends LateBindingBase {

    public RhinoEnumerator(Object[] ids) {
      if (debug) {
	System.out.println("RhinoEnumerator new");
      }
      idarray = ids;
      index = 0;
    }
    public String enumNext() {
      if (debug) {
	System.out.println("RhinoEnumerator idx=" + index);
      }
      if (index >= idarray.length) {
	return null;
      }
      String r = idarray[index].toString();
      ++index;
      return r;
    }

    private final Object[] idarray;
    private int index;

  }

  private static class RhinoProxy implements LateBinding {

    public RhinoProxy(Scriptable scriptable, Callable callable) {
      this.scriptable = scriptable;
      this.callable = callable;
      if (debug) {
	System.out.println("RhinoProxy created " + this
	  + " klassldr=" + this.getClass().getClassLoader());
      }
    }
    private Scriptable getThis() {
      return (callable == null) ? scriptable : (Scriptable)callable;
    }
    public int getAttributes() {
      int r = Attrib_Mutable;
      if (callable != null) {
	r |= Attrib_Function;
      }
      return r;
    }
    public String getString() {
      Scriptable thisobj = getThis();
      return thisobj.toString();
    }
    public Object getProperty(String name) {
      if (debug) {
	System.out.println("RhinoProxy getprop scriptable=" + scriptable +
	" callable=" + callable + " propname=" + name);
      }
      Scriptable thisobj = getThis();
      Object rhobj = ScriptableObject.getProperty(thisobj, name);
      if (rhobj == Scriptable.NOT_FOUND) {
	if (callable != null && callable instanceof Function &&
	  name.equals("NewInstance")) {
	  return new NewInstance(scriptable, (Function)callable);
	} else {
	  /* property not found */
	  if (debug) {
	    System.out.println("r=NOT_FOUND");
	  }
	  return null;
	}
      }
      Object r = toASE(scriptable, rhobj);
      if (debug) {
	System.out.println("RhinoProxy getprop r=" + r);
      }
      return r;
    }
    public void setProperty(String name, Object obj) {
      Scriptable thisobj = getThis();
      Object rhobj = toRhino(obj);
      if (debug) {
	System.out.println("RhinoProxy setprop name=" + name);
      }
      ScriptableObject.putProperty(thisobj, name, rhobj);
    }
    public void delProperty(String name) {
      Scriptable thisobj = getThis();
      if (debug) {
	System.out.println("RhinoProxy delprop name=" + name);
      }
      ScriptableObject.deleteProperty(thisobj, name);
    }
    public Object getElement(int idx) {
      if (debug) {
	System.out.println("RhinoProxy getelem scriptable=" + scriptable +
	" callable=" + callable + " idx=" + idx);
      }
      Scriptable thisobj = getThis();
      Object rhobj = ScriptableObject.getProperty(thisobj, idx);
      if (rhobj == Scriptable.NOT_FOUND) {
	/* property not found */
	if (debug) {
	  System.out.println("r=NOT_FOUND");
	}
	return null;
      }
      Object r = toASE(scriptable, rhobj);
      if (debug) {
	System.out.println("RhinoProxy getprop r=" + r);
      }
      return r;
    }
    public void setElement(int idx, Object obj) {
      Scriptable thisobj = getThis();
      Object rhobj = toRhino(obj);
      if (debug) {
	System.out.println("RhinoProxy setelem idx=" + idx + " value="
	  + rhobj);
      }
      ScriptableObject.putProperty(thisobj, idx, rhobj);
    }
    public int getLength() {
      return -1;
    }
    public void setLength(int len) {
    }
    public Object invokeByName(String name, Object[] args) {
      return ((LateBinding)getProperty(name)).invokeSelf(args);
    }
    public Object invokeSelf(final Object[] args) {
      return Context.call(new ContextAction() {
	public Object run(Context cx) {
	  Object[] rhargs = toRhino(args);
	  if (debug) {
	    System.out.println("callable call scope = " + scriptable +
	      " call = " + callable + " rhargs = " + rhargs);
	  }
	  Object rhrval = callable.call(cx, scriptable, scriptable, rhargs);
	  if (debug) {
	    System.out.println("callable call rhretval = " + rhrval +
	      " class = " + rhrval.getClass());
	  }
	  Object r = toASE(scriptable, rhrval);
	  if (debug) {
	    System.out.println("callable call aserval = " + r +
	      " class = " + r.getClass());
	  }
	  return r;
	}
      });
    }
    public Object getEnumerator() {
      if (debug) {
	System.out.println("RhinoProxy getenumerator");
      }
      Scriptable thisobj = getThis();
      Object[] ids = thisobj.getIds();
      if (ids == null) {
	return null;
      }
      return new RhinoEnumerator(ids);
    }
    public String enumNext() {
      return null;
    }

    public final Scriptable scriptable;
    public final Callable callable;

  }

  private static class ASEProxy implements Scriptable, Function {

    public ASEProxy(LateBinding wrapped) {
      this.wrapped = wrapped;
    }
    public String toString() {
      return wrapped.getString();
    }
    public void delete(int index) {
    }
    public void delete(String name) {
      wrapped.delProperty(name);
    }
    public Object get(int index, Scriptable start) {
      Object aseobj = wrapped.getElement(index);
      return toRhino(aseobj);
    }
    public Object get(String name, Scriptable start) {
      Object aseobj = wrapped.getProperty(name);
      return toRhino(aseobj);
    }
    public String getClassName() {
      return "ASEProxy";
    }
    public Object getDefaultValue(Class hint) {
      return "[object ASEProxy]";
    }
    public Object[] getIds() {
      LateBinding en = (LateBinding)wrapped.getEnumerator();
      if (en == null) {
	return null;
      }
      ArrayList arr = new ArrayList();
      while (true) {
	String k = en.enumNext();
	if (k == null) {
	  break;
	}
	arr.add(k);
      }
      return arr.toArray();
    }
    public Scriptable getParentScope() {
      return parent;
    }
    public Scriptable getPrototype() {
      return prototype;
    }
    public boolean has(int index, Scriptable start) {
      Object aseobj = wrapped.getElement(index);
      return aseobj != null;
    }
    public boolean has(String name, Scriptable start) {
      Object aseobj = wrapped.getProperty(name);
      return aseobj != null;
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
      Object aseobj = toASE(start, value);
      wrapped.setElement(index, aseobj);
    }
    public void put(String name, Scriptable start, Object value) {
      Object aseobj = toASE(start, value);
      wrapped.setProperty(name, aseobj);
    }
    public void setParentScope(Scriptable parent) {
      this.parent = parent;
    }
    public void setPrototype(Scriptable prototype) {
      this.prototype = prototype;
    }
    public Object call(Context cx, Scriptable scope, Scriptable thisobj,
      Object[] args) {
      Object[] aseargs = toASE(scope, args);
      Object aserval = wrapped.invokeSelf(aseargs);
      return toRhino(aserval);
    }
    public Scriptable construct(Context cx, Scriptable scope, Object[] args) {
      Object[] aseargs = toASE(scope, args);
      Object aserval = wrapped.invokeByName("NewInstance", aseargs);
      Object r = toRhino(aserval);
      return (Scriptable)r;
    }

    public final LateBinding wrapped;
    private Scriptable prototype, parent;

  }

}

