
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <stdlib.h>
#include <stdio.h>
#include <new.h>

#include <ase/asemalloc.hpp>

#define DBG(x)

namespace {

int
mhook_handle_oom(size_t size)
{
  static const char oom[] = "ASERE: OutOfMemory\n";
  fwrite(oom, sizeof(oom) - 1, 1, stderr);
  fprintf(stderr, "ASERE: malloc: failed to allocate %lu byte\n",
    (unsigned long)size);
  fprintf(stderr, "ASERE: _exit(-1)\n");
  _exit(-1);
}

}; // anonymous namespace

void
ase_install_malloc_hook()
{
  _set_new_mode(1);
  _set_new_handler(mhook_handle_oom);
}

