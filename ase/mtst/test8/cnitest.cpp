
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <iostream>

#include <gcj/cni.h>
#include <java/lang/System.h>
#include <java/io/PrintStream.h>
#include <java/lang/Throwable.h>
#include "TestClass.h"

int main(int argc, char **argv)
{
  try {
    JvCreateJavaVM(NULL);

    JvAttachCurrentThread(NULL, NULL);

    java::lang::String *message = JvNewStringLatin1("hello cni");
    JvInitClass(&java::lang::String::class$);
    java::lang::System::out->println(message);

    JvInitClass(&TestClass::class$);
    TestClass *to = new TestClass();
    int v = to->foo(100);
    std::cout << "v = " << v << std::endl;
    int x = 0;
    for (int i = 0; i < 10000000; ++i) {
      x = to->bar();
    }
    std::cout << "x = " << x << std::endl;

    JvDetachCurrentThread();

  } catch (java::lang::Throwable *t) {
    java::lang::System::err->println(JvNewStringLatin1(
      "unhandled java exception"));
    t->printStackTrace();
  }
  return 0;
}

