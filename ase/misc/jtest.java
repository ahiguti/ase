
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

public class jtest {

  static public void main(String[] args) {
    System.out.println("jtest main numargs=" + args.length);
    for (int i = 0; i < args.length; ++i) {
      System.out.println("arg " + args[i]);
    }
  }

  static {
    System.out.println("jtest static init");
  }

}

