
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asedllhandle.hpp>

#ifdef __GNUC__

#include <dlfcn.h>

ase_auto_dll_handle::ase_auto_dll_handle(const ase_string& filename,
  bool global)
  : handle(0)
{
  /* does not throw */
  int glflag = global ? RTLD_GLOBAL : RTLD_LOCAL;
  handle = ::dlopen(filename.data(), RTLD_NOW | glflag);
  if (!handle) {
    error = "dlopen: " + filename + ": " + ase_string(::dlerror());
  }
}

ase_auto_dll_handle::~ase_auto_dll_handle() /* DCHK */
{
  /* constr does not throw */
  if (handle) {
    ::dlclose(handle);
  }
}

void *
ase_auto_dll_handle::get_symbol(const ase_string& name)
{
  if (!handle) {
    error = ase_string("dlsym: not open");
    return 0;
  }
  void *sym = ::dlsym(handle, name.data());
  if (!sym) {
    error = "dlsym: " + name + ": " + ase_string(::dlerror());
  }
  return sym;
}

#else

#include <windows.h>
#include <ase/aseutf16.hpp>

ase_auto_dll_handle::ase_auto_dll_handle(const ase_string& filename,
  bool global)
  : handle(NULL)
{
  /* does not throw */
  ase_utf16_string wfname(filename);
  handle = LoadLibraryW(wfname.data());
  if (handle == NULL) {
    error = "LoadLibrary: " + filename + ": failed to open";
  }
}

ase_auto_dll_handle::~ase_auto_dll_handle() /* DCHK */
{
  /* constr does not throw */
  if (handle != NULL) {
    FreeLibrary(handle);
  }
}

void *
ase_auto_dll_handle::get_symbol(const ase_string& name)
{
  if (!handle) {
    error = ase_string("dlsym: not open");
    return 0;
  }
  void *sym = GetProcAddress(handle, name.data());
  if (!sym) {
    error = ase_string("GetProcAddress: ") + name
      + ase_string(": not found");
  }
  return sym;
}

#endif

