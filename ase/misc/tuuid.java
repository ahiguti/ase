
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

import asejni.*;

public class tuuid extends LateBindingBase {

  static public void main(LateBinding globals, LateBinding args) {
    LateBinding Native = (LateBinding)globals.getProperty("Native");
    LateBinding Util = (LateBinding)Native.getProperty("Util");
    LateBinding UUID = (LateBinding)Util.getProperty("UUID");
    for (int i = 0; i < 10; ++i) {
      LateBinding x = (LateBinding)UUID.invokeSelf(null);
      String s = (String)x.invokeByName("upper", null);
      System.out.println(s);
    }
  }

}

