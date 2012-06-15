
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <ase/asemalloc.hpp>

#define DBG(x)

namespace {

void *(*old_malloc_hook)(size_t size, const void *caller);
void *(*old_memalign_hook)(size_t size, size_t algn, const void *caller);
void *(*old_realloc_hook)(void *ptr, size_t size, const void *caller);
void (*old_free_hook)(void *ptr, const void *caller);

void *mhook_malloc_hook(size_t size, const void *caller);
void *mhook_memalign_hook(size_t size, size_t algn, const void *caller);
void *mhook_realloc_hook(void *ptr, size_t size, const void *caller);
void mhook_free_hook(void *ptr, const void *caller);

void
install_old_hook(void)
{
  __malloc_hook = old_malloc_hook;
  __memalign_hook = old_memalign_hook;
  __realloc_hook = old_realloc_hook;
  __free_hook = old_free_hook;
}

void
install_our_hook(void)
{
  __malloc_hook = mhook_malloc_hook;
  __memalign_hook = mhook_memalign_hook;
  __realloc_hook = mhook_realloc_hook;
  __free_hook = mhook_free_hook;
}

void
mhook_outofmemory(const char *str, size_t size)
{
  static const char oom[] = "ASERE: OutOfMemory\n";
  fwrite(oom, sizeof(oom) - 1, 1, stderr);
  fprintf(stderr, "ASERE: %s: failed to allocate %lu byte\n",
    str, (unsigned long)size);
  fprintf(stderr, "ASERE: _exit(-1)\n");
  _exit(-1);
}

void *
mhook_malloc_hook(size_t size, const void *caller)
{
  void *r;
  install_old_hook();
  r = malloc(size);
  DBG(fprintf(stderr, "malloc %p %lu\n", r, (unsigned long)size));
  if (r == 0 && size != 0) {
    mhook_outofmemory("malloc", size);
  }
  install_our_hook();
  return r;
}

void *
mhook_memalign_hook(size_t size, size_t algn, const void *caller)
{
  void *r;
  install_old_hook();
  r = memalign(size, algn);
  DBG(fprintf(stderr, "memalign %p %lu %lu\n", r, (unsigned long)size,
    (unsigned long)algn));
  if (r == 0 && size != 0) {
    mhook_outofmemory("memalign", size);
  }
  install_our_hook();
  return r;
}

void *
mhook_realloc_hook(void *ptr, size_t size, const void *caller)
{
  void *r;
  install_old_hook();
  r = realloc(ptr, size);
  DBG(fprintf(stderr, "realloc %p %p %lu\n", r, ptr, (unsigned long)size));
  if (r == 0 && size != 0) {
    mhook_outofmemory("realloc", size);
  }
  install_our_hook();
  return r;
}

void
mhook_free_hook(void *ptr, const void *caller)
{
  install_old_hook();
  DBG(fprintf(stderr, "free %p\n", ptr));
  free(ptr);
  install_our_hook();
}

}; // anonymous namespace

void
ase_install_malloc_hook()
{
  old_malloc_hook = __malloc_hook;
  old_memalign_hook = __memalign_hook;
  old_realloc_hook = __realloc_hook;
  old_free_hook = __free_hook;
  __malloc_hook = mhook_malloc_hook;
  __memalign_hook = mhook_memalign_hook;
  __realloc_hook = mhook_realloc_hook;
  __free_hook = mhook_free_hook;
}

