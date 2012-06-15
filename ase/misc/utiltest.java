
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

import asejni.*;

public class utiltest extends LateBindingBase {

  static public void main(LateBinding globals, LateBinding args) {
    LateBinding Native = (LateBinding)globals.getProperty("Native");
    LateBinding Util = (LateBinding)Native.getProperty("Util");
    LateBinding Test = (LateBinding)Util.getProperty("Test");
    Integer r = null;
    // LateBinding sum = (LateBinding)Test.getProperty("Sum");
    // Integer cargs[] = new Integer[]{ 10 };
    // StringBuffer x = new StringBuffer();
    // Integer x = new Integer(3);
    // Integer cargs[] = new Integer[]{ 10, 20, 30 };
    for (int i = 0; i < 10000000; ++i) {
      r = (Integer)Test.invokeByName("Sum", new Integer[] { 10, 20, 30 });
      // Test.setElement(10, Native);
      // Test.getProperty(null);
      // r = (Integer)sum.invokeSelf(null);
      // r = (Integer)sum.invokeSelf(cargs);
      // Test.getElement(10);
      // Test.getLength();
    }
    System.out.println(r);
  }

}

