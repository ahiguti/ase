
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

static void *callers[65536];
static int num_callers = 0;
static FILE *logfile = NULL;

void __cyg_profile_func_enter (void *this_fn, void *call_site)
{
  callers[num_callers] = this_fn;
  if (++num_callers >= 65536) {
    fprintf(stderr, "__cyg_profile_func_enter: stack is full\n");
    abort();
  }
}

void __cyg_profile_func_exit  (void *this_fn, void *call_site)
{
  num_callers--;
  if (num_callers < 0) {
    fprintf(stderr, "__cyg_profile_func_exit: stack is empty\n");
    abort();
  }
}

static void dbgmalloc_init_hook(void);
static void *dbgmalloc_malloc_hook(size_t size, const void *caller);
static void *dbgmalloc_memalign_hook(size_t s, size_t algn,
  const void *caller);
static void *dbgmalloc_realloc_hook(void *ptr, size_t size,
  const void *caller);
static void dbgmalloc_free_hook(void *ptr, const void *caller);

void (*__malloc_initialize_hook)(void) = dbgmalloc_init_hook;

static void *(*old_malloc_hook)(size_t size, const void *caller);
static void *(*old_memalign_hook)(size_t size, size_t algn,
  const void *caller);
static void *(*old_realloc_hook)(void *ptr, size_t size, const void *caller);
static void (*old_free_hook)(void *ptr, const void *caller);

static void
install_old_hook(void)
{
  __malloc_hook = old_malloc_hook;
  __memalign_hook = old_memalign_hook;
  __realloc_hook = old_realloc_hook;
  __free_hook = old_free_hook;
}

static void
install_our_hook(void)
{
  __malloc_hook = dbgmalloc_malloc_hook;
  __memalign_hook = dbgmalloc_memalign_hook;
  __realloc_hook = dbgmalloc_realloc_hook;
  __free_hook = dbgmalloc_free_hook;
}

static void
dbgmalloc_init_hook(void)
{
  old_malloc_hook = __malloc_hook;
  old_memalign_hook = __memalign_hook;
  old_realloc_hook = __realloc_hook;
  old_free_hook = __free_hook;
  install_our_hook();
}

static void
open_logfile(void)
{
  if (logfile)
    return;
  logfile = fopen("/tmp/dbgml.out", "w");
}

static void
show_stack_trace_internal(void)
{
  int i;
  for (i = 0; i < num_callers; i++) {
    fprintf(logfile, "%p ", callers[i]);
  }
}

static void
show_stack_trace(void)
{
  show_stack_trace_internal();
  fprintf(logfile, "\n");
}

static void *
dbgmalloc_malloc_hook(size_t size, const void *caller)
{
  void *r;
  install_old_hook();
  r = malloc(size);
  open_logfile();
  if (r) fprintf(logfile, "M %p %zu : ", r, size);
  show_stack_trace();
  install_our_hook();
  return r;
}

static void *
dbgmalloc_memalign_hook(size_t size, size_t algn, const void *caller)
{
  void *r;
  install_old_hook();
  r = memalign(size, algn);
  open_logfile();
  if (r) fprintf(logfile, "M %p %zu : ", r, size);
  show_stack_trace();
  install_our_hook();
  return r;
}

static void *
dbgmalloc_realloc_hook(void *ptr, size_t size, const void *caller)
{
  void *r;
  install_old_hook();
  r = realloc(ptr, size);
  open_logfile();
  if (ptr) fprintf(logfile, "F %p 0 : ", ptr);
  show_stack_trace();
  if (r) fprintf(logfile, "M %p %zu : ", r, size);
  show_stack_trace();
  install_our_hook();
  return r;
}

static void
dbgmalloc_free_hook(void *ptr, const void *caller)
{
  install_old_hook();
  free(ptr);
  open_logfile();
  if (ptr) fprintf(logfile, "F %p 0 : ", ptr);
  show_stack_trace();
  install_our_hook();
}

