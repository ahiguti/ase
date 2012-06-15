
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

/* JAVA2_IMPL */

package asejni;

import java.lang.reflect.*;

class NativeProxy implements InvocationHandler, LateBinding {

  private native void finalizeImpl(long pr);
  private native Object invokeImpl(long pr, String methodname, Object proxy,
    Method method, Object[] args);
  private native int getAttributesImpl(long pr);
  private native String getStringImpl(long pr);
  private native Object getPropertyImpl(long pr, String name);
  private native void setPropertyImpl(long pr, String name, Object value);
  private native void delPropertyImpl(long pr, String name);
  private native Object getElementImpl(long pr, int idx);
  private native void setElementImpl(long pr, int idx, Object value);
  private native int getLengthImpl(long pr);
  private native void setLengthImpl(long pr, int len);
  private native Object invokeByNameImpl(long pr, String name, Object[] args);
  private native Object invokeSelfImpl(long pr, Object[] args);
  private native Object getEnumeratorImpl(long pr);
  private native String enumNextImpl(long pr);

  static private final boolean debug = false;

  private void debugPrint(String s) {
    if (debug) {
      System.err.println(s);
    }
  }

  /* Object */
  public void finalize() {
    finalizeImpl(priv);
  }
  /* InvocationHandler */
  public Object invoke(Object proxy, Method method, Object[] args)
    throws Throwable {
    return invokeImpl(priv, method.getName(), proxy, method, args);
  }
  /* asejava.JScriptObject */
  public int getAttributes() {
    return getAttributesImpl(priv);
  }
  public String getString() {
    return getStringImpl(priv);
  }
  public Object getProperty(String name) {
    Object r = getPropertyImpl(priv, name);
    if (r != null) {
      debugPrint("NP getprop r=" + r + " klass=" + r.getClass()
	+ " thisldr=" + this.getClass().getClassLoader());
    } else {
      debugPrint("NP getprop r=null");
    }
    return r;
  }
  public void setProperty(String name, Object value) {
    setPropertyImpl(priv, name, value);
  }
  public void delProperty(String name) {
    delPropertyImpl(priv, name);
  }
  public Object getElement(int idx) {
    return getElementImpl(priv, idx);
  }
  public void setElement(int idx, Object value) {
    setElementImpl(priv, idx, value);
  }
  public int getLength() {
    return getLengthImpl(priv);
  }
  public void setLength(int len) {
    setLengthImpl(priv, len);
  }
  public Object invokeByName(String name, Object[] args) {
    return invokeByNameImpl(priv, name, args);
  }
  public Object invokeSelf(Object[] args) {
    return invokeSelfImpl(priv, args);
  }
  public Object getEnumerator() {
    return getEnumeratorImpl(priv);
  }
  public String enumNext() {
    return enumNextImpl(priv);
  }

  public String toString() {
    return getString();
  }

  private NativeProxy(long pr) {
    priv = pr;
  }
  static private Object newProxy(Class kl, long pr) {
    if (kl != null) {
      return Proxy.newProxyInstance(kl.getClassLoader(), new Class[] { kl },
	new NativeProxy(pr));
    } else {
      return new NativeProxy(pr);
    }
  }
  private final long /* RAW POINTER */ priv;

}

