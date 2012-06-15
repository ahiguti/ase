
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEEXCEPT_HPP
#define GENTYPE_ASEEXCEPT_HPP

#include <ase/asetyped.hpp>
#include <stdexcept>

ASELIB_DLLEXT ase_variant ase_new_out_of_memory_exception() throw();

ASELIB_DLLEXT bool ase_is_ase_exception(const ase_variant& val);
ASELIB_DLLEXT void ase_exception_append_trace(const ase_variant& val,
  const ase_variant& trace);
ASELIB_DLLEXT ase_variant ase_new_exception_raw(const ase_string& id,
  const ase_string& desc, const ase_variant& val);
ASELIB_DLLEXT ase_variant ase_new_exception_from_strmap(const ase_variant& sm);
ASELIB_DLLEXT ase_variant ase_convert_to_ase_exception(
  const ase_string& mod_name, const ase_string& typ_name,
  const ase_string& desc, const ase_variant& val);
ASELIB_DLLEXT ase_variant ase_stdexcept_to_ase_exception(
  const std::exception& ex);
ASELIB_DLLEXT ase_variant ase_new_exception(const ase_string& errid,
  const ase_string& desc);

ASELIB_DLLEXT void ase_throw_exception(const ase_string& errid,
  const ase_string& desc);
ASELIB_DLLEXT void ase_throw_exception(const char *errid,
  const ase_string& desc);

ASELIB_DLLEXT void ase_throw_type_mismatch(const char *mess);
ASELIB_DLLEXT void ase_throw_missing_arg(const char *mess);
ASELIB_DLLEXT void ase_throw_too_many_args(const char *mess);
ASELIB_DLLEXT void ase_throw_invalid_arg(const char *mess);
ASELIB_DLLEXT void ase_throw_illegal_operation(const char *mess);
ASELIB_DLLEXT void ase_throw_io_error(const char *mess);
ASELIB_DLLEXT void ase_throw_error(const char *mess);
ASELIB_DLLEXT void ase_throw_runtime_exception(const char *mess);

ASELIB_DLLEXT void ase_check_num_args(ase_size_type nargs,
  ase_size_type expected, const char *mess);
ASELIB_DLLEXT bool ase_validate_boolean(const ase_variant& v,
  const char *mess);
ASELIB_DLLEXT ase_int ase_validate_int(const ase_variant& v, const char *mess);
ASELIB_DLLEXT ase_long ase_validate_long(const ase_variant& v,
  const char *mess);
ASELIB_DLLEXT double ase_validate_double(const ase_variant& v,
  const char *mess);
ASELIB_DLLEXT ase_string ase_validate_string(const ase_variant& v,
  const char *mess);
ASELIB_DLLEXT void ase_validate_object(const ase_variant& v, const char *mess);
ASELIB_DLLEXT void ase_validate_array(const ase_variant& v, const char *mess);

ASELIB_DLLEXT ase_string ase_nothrow_get_string(const ase_variant& v);
ASELIB_DLLEXT ase_variant ase_nothrow_get_property(const ase_variant& v,
  const char *name, ase_size_type namelen);

#endif

