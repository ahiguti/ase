
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

/* JAVA2_IMPL */

package asejni;

import java.lang.reflect.*;

public interface LateBinding {

  final int Attrib_None = 0;
  final int Attrib_Function = 1;
  final int Attrib_Array = 2;
  final int Attrib_Mutable = 4;

  int getAttributes();
  String getString();
  Object getProperty(String name);
  void setProperty(String name, Object value);
  void delProperty(String name);
  Object getElement(int idx);
  void setElement(int idx, Object value);
  int getLength();
  void setLength(int len);
  Object invokeByName(String name, Object[] args);
  Object invokeSelf(Object[] args);
  Object getEnumerator();
  String enumNext();

}

