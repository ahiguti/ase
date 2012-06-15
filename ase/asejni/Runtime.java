
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

/* JAVA2_IMPL */

package asejni;

public class Runtime {

  public static Object getGlobal() {
    return getGlobalImpl(priv);
  }

  static {
    System.loadLibrary("ase-scr-Java");
    priv = constructImpl();
  }

  private static native long constructImpl();
  private static native Object getGlobalImpl(long p);
  private static native long finalizeImpl(long p); /* unused */

  private static long /* RAW POINTER */ priv;

}
