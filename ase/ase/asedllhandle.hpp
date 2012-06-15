
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASE_DLLHANDLE_HPP
#define GENTYPE_ASE_DLLHANDLE_HPP

#include <ase/ase.hpp>

#include <string>

#ifndef __GNUC__
#include <windows.h>
#endif

class ase_auto_dll_handle : public ase_noncopyable {

 public:

  ASELIB_DLLEXT ase_auto_dll_handle(const ase_string& filename, bool global);
  ASELIB_DLLEXT ~ase_auto_dll_handle(); /* DCHK */
  ASELIB_DLLEXT void *get_symbol(const ase_string& name);
  const ase_string& get_error() const { return error; }

 protected:

  ase_string error;

 private:

  #ifdef __GNUC__
  void *handle;
  #else
  HMODULE handle;
  #endif

};

#endif

