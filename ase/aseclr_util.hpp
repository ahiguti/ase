
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASECLR_UTIL_HPP
#define GENTYPE_ASECLR_UTIL_HPP

#include "aseclr_sctx.hpp"

namespace aseclr {

ase_string to_ase_string(System::String ^str);
System::String ^to_clr_string(const char *str, ase_size_type len);
System::String ^to_clr_string(const ase_string& str);
ase_variant to_ase_value(ase_clr_script_engine *sc, System::Object ^obj);
System::Object ^to_clr_number(double val);
System::Object ^to_clr_value(ase_clr_script_engine *sc, const ase_variant& val);
array<System::Object ^> ^to_clr_value_array(ase_clr_script_engine *sc,
  const ase_variant *arr, ase_size_type alen);
System::Object ^create_wrapper(System::Type ^ty);
ase_variant get_executing_assembly(ase_clr_script_engine *sc);
ase_variant get_mscorlib_assembly(ase_clr_script_engine *sc);
ase_variant create_delegate(ase_clr_script_engine *sc, const ase_variant& de,
  const ase_variant& val);
ase_variant add_event_handler(ase_clr_script_engine *sc, const ase_variant& obj,
  const ase_string& name, const ase_variant& handler);
void remove_event_handler(ase_clr_script_engine *sc, const ase_variant& obj,
  const ase_string& name, const ase_variant& handler);
void throw_ase_exception(ase_clr_script_engine *sc, System::Exception ^ex);

}; // namespace aseclr

#endif

