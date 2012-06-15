
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

package asejni;

class ASEException extends RuntimeException {

  ASEException(Object obj) {
    wrapped = obj;
  }
  public Object getWrapped() {
    return wrapped;
  }
  public void initWrapped() {
    wrapped = null;
  }
  public String toString() {
    String s = null;
    try {
      if (wrapped != null) {
	s = wrapped.toString();
      }
    } catch (Throwable e) {
      /* ignore */
    }
    if (s != null) {
      return "(ASEException) " + s;
    }
    return "ASEException";
  }

  private Object wrapped;

}

