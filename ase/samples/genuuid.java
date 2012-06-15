
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

import asejni.*;

public class genuuid extends LateBindingBase {

  static public void main(LateBinding globals, LateBinding args) {
    LateBinding Native = (LateBinding)globals.getProperty("Native");
    LateBinding Util = (LateBinding)Native.getProperty("Util");
    LateBinding UUID = (LateBinding)Util.getProperty("UUID");
    {
      LateBinding x = (LateBinding)UUID.invokeSelf(null);
      String s = (String)x.invokeByName("lower", null);
      System.out.println(s);
    }
  }

}

