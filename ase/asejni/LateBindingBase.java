
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

/* JAVA2_IMPL */

package asejni;

public class LateBindingBase implements LateBinding {

  public int getAttributes() {
    return Attrib_Mutable;
  }
  public String getString() {
    return "";
  }
  public Object getProperty(String name) {
    return null;
  }
  public void setProperty(String name, Object value) {
  }
  public void delProperty(String name) {
  }
  public Object getElement(int idx) {
    return null;
  }
  public void setElement(int idx, Object value) {
  }
  public int getLength() {
    return -1;
  }
  public void setLength(int len) {
  }
  public Object invokeByName(String name, Object[] args) {
    return ((LateBinding)getProperty(name)).invokeSelf(args);
  }
  public Object invokeSelf(Object[] args) {
    return null;
  }
  public Object getEnumerator() {
    return null;
  }
  public String enumNext() {
    return null;
  }

}

