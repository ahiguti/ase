
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEDUMMY_UTIL_HPP
#define GENTYPE_ASEDUMMY_UTIL_HPP

#include "asedummy_sctx.hpp"

namespace asedummy {

struct ase_dummy_context { };
struct ase_dummy_value { };
struct ase_dummy_string { };

struct ase_dummy_util {

  static ase_variant create_dummy_proxy(ase_dummy_context& ctx,
    const ase_dummy_value& val);
  static ase_dummy_value create_aseproxy(ase_dummy_context& ctx,
    const ase_variant& value);

  static ase_string to_ase_string(ase_dummy_context& ctx,
    const ase_dummy_string& str);
  static ase_dummy_string to_dummy_string(ase_dummy_context& ctx,
    const char *str, ase_size_type slen);
  static ase_dummy_string to_dummy_string(ase_dummy_context& ctx,
    const ase_string& str);
  static ase_variant to_ase_variant(ase_dummy_context& ctx,
    const ase_dummy_value& val);
  static ase_dummy_value to_dummy_value(ase_dummy_context& ctx,
    const ase_variant& value);

  static void handle_ase_exception(ase_dummy_context& ctx);

};

}; // namespace asedummy

#endif

