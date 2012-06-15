
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

import asejni.*;

public class fib extends LateBindingBase {

  static int fib(int x) {
    return x <= 1 ? 1 : fib(x - 2) + fib(x - 1);
  }

  static public void main(LateBinding globals, LateBinding args) {
    int r = fib(30);
    System.out.println(r);
  }

}

