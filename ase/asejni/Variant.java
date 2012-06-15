
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

/* JAVA2_IMPL */

package asejni;

public class Variant {

  public static String getString(Object value) {
    return value.toString();
  }
  public static boolean getBoolean(Object value) {
    if (value instanceof Boolean) {
      return (Boolean)value;
    }
    return false;
  }
  public static int getInt(Object value) {
    if (value instanceof Integer) {
      return (Integer)value;
    } else if (value instanceof Long) {
      return (int)(long)(Long)value;
    } else if (value instanceof Double) {
      return (int)(double)(Double)value;
    }
    return 0;
  }
  public static long getLong(Object value) {
    if (value instanceof Integer) {
      return (int)(Integer)value;
    } else if (value instanceof Long) {
      return (Long)value;
    } else if (value instanceof Double) {
      return (long)(double)(Double)value;
    }
    return 0;
  }
  public static double getDouble(Object value) {
    if (value instanceof Integer) {
      return (Integer)value;
    } else if (value instanceof Long) {
      return (Long)value;
    } else if (value instanceof Double) {
      return (Double)value;
    }
    return 0;
  }
  public static int getAttributes(Object value) {
    return ((LateBinding)value).getAttributes();
  }
  public static Object getProperty(Object value, String name) {
    return ((LateBinding)value).getProperty(name);
  }
  public static void setProperty(Object value, String name, Object e) {
    ((LateBinding)value).setProperty(name, e);
  }
  public static void delProperty(Object value, String name) {
    ((LateBinding)value).delProperty(name);
  }
  public static Object getElement(Object value, int idx) {
    return ((LateBinding)value).getElement(idx);
  }
  public static void setElement(Object value, int idx, Object e) {
    ((LateBinding)value).setElement(idx, e);
  }
  public static int getLength(Object value) {
    return ((LateBinding)value).getLength();
  }
  public static void setLength(Object value, int len) {
    ((LateBinding)value).setLength(len);
  }
  public static Object invokeByName(Object value, String name, Object[] args) {
    return ((LateBinding)value).invokeByName(name, args);
  }
  public static Object invokeSelf(Object value, Object[] args) {
    return ((LateBinding)value).invokeSelf(args);
  }
  public static Object getEnumerator(Object value) {
    return ((LateBinding)value).getEnumerator();
  }
  public static String enumNext(Object value) {
    return ((LateBinding)value).enumNext();
  }

}

