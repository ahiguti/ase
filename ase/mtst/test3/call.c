
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <stdio.h>

#include "cfunc.h"

int main()
{
  int x, y, r;
  x = 10;
  y = 20;
  r = cfunc_add(x, y);
  printf("%d\n", r);
  return 0;
}
