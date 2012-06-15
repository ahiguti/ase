
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASECLR_DELEGATE_HPP
#define GENTYPE_ASECLR_DELEGATE_HPP

#include <vcclr.h>
#include <ase/ase.hpp>

#include "aseclr_dynproxy.hpp"
#include "aseclr_sctx.hpp"

namespace aseclr {

interface struct untyped_function_object {
  System::Object ^invoke(array<System::Object ^> ^args);
};

System::Delegate ^create_typed_delegate(ase_clr_script_engine *sc,
  System::Type ^deltype, untyped_function_object ^fobj);
System::Delegate ^create_typed_delegate(ase_clr_script_engine *sc,
  System::Type ^deltype, const ase_variant& value);

}; // namespace aseclr

#endif

