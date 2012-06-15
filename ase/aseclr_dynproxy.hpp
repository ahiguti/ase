
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASECLR_DYNPROXY_HPP
#define GENTYPE_ASECLR_DYNPROXY_HPP

#include <vcclr.h>
#include <ase/ase.hpp>
#include <ase/aselist.hpp>
#include <ase/asemutex.hpp>

namespace aseclr {

struct ase_clr_script_engine;

typedef ase_list_node< ase_synchronized<ase_variant> > aseproxy_node;

System::Object ^create_dynamic_proxy(ase_clr_script_engine *sc,
  System::Type ^typ, const ase_variant& val, System::Object ^baseobj);

}; // namespace aseclr

#endif

