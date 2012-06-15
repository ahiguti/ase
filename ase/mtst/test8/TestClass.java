
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

class TestClass {
  int count = 0;
  TestClass() {
    System.out.println("constr called");
  }
  int foo(int x) {
    System.out.println("x=" + x);
    return x * 2;
  }
  int bar() {
    return ++count;
  }
}

