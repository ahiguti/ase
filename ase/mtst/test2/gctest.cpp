
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gcj/cni.h>
#include <java/lang/StringBuffer.h>
#include <java/util/ArrayList.h>
// #include "gctest.h"

using namespace java::lang;
using namespace java::util;

ArrayList *vars = 0;

int testbar_init()
{
  if (JvCreateJavaVM(0) != 0) {
    throw 0;
  }
  JvAttachCurrentThread(0, 0);
  JvInitClass(&StringBuffer::class$);
  JvInitClass(&ArrayList::class$);
  vars = new ArrayList();
  return 0;
}

struct xtestbar {
  java::lang::StringBuffer *sb;
};

xtestbar *
new_testbar()
{
  xtestbar *x = new xtestbar();
  StringBuffer *b = new StringBuffer();
  vars->add(b);
  x->sb = b;
  return x;
}


int
testbar_getval(xtestbar *v)
{
  return v->sb->toString()->length();
}


#include <stdio.h>

int main()
{
  int i = 0;
  int r = 0;
  testbar_init();
  xtestbar *tb1 = new_testbar();
  // StringBuffer *sb1 = new StringBuffer();
  for (i = 0; i < 10000000; ++i) {
    // StringBuffer *sb2 = new StringBuffer();
    // r += sb2->toString()->length();
    // r += sb1->toString()->length();
    xtestbar *tb2 = new_testbar();
    r += testbar_getval(tb2);
    r += testbar_getval(tb1);
  }
  fprintf(stderr, "r=%d\n", r);
  return 0;
}

