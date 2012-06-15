
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

public class testfoo {
  int count = 0;
  public testfoo() {
    System.out.println("testfoo created");
  }
  public String foo(String x, String y) {
    System.out.println("testfoo::foo x=" + x);
    System.out.println("testfoo::foo y=" + y);
    return x + y;
  }
  public String foobar(String x, String y) {
    System.out.println("testfoo::foobar x=" + x);
    System.out.println("testfoo::foobar y=" + y);
    return x + y;
  }
  public int bar() {
    return ++count;
  }
  public static void main(String[] args) {
    testfoo f = new testfoo();
    f.foobar("abc", "def");
  }
};

