
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/aseexcept.hpp>
#include <ase/aseenum.hpp>
#include <vector>
#include <typeinfo>

class ase_variant_impl_oom_exception : public ase_variant_impl_default {

 public:

  static const char *const debugid;

  static void add_ref(const ase_variant& v) { }
  static void release(const ase_variant& v) { }

  static ase_string get_string(const ase_variant& v);
  static ase_variant get_property(const ase_variant& v, const char *name,
    ase_size_type len);
  static ase_variant get_enumerator(const ase_variant& v);

};

const char *const
ase_variant_impl_oom_exception::debugid = 0;

ase_string
ase_variant_impl_oom_exception::get_string(const ase_variant& v)
{
  return ase_string("(Id=OutOfMemory) out of memory");
}

ase_variant
ase_variant_impl_oom_exception::get_property(const ase_variant& v,
  const char *name, ase_size_type len)
{
  if (ase_string::equals(name, len, "Id")) {
    return ase_string("OutOfMemory").to_variant();
  }
  if (ase_string::equals(name, len, "Description")) {
    return ase_string("OutOfMemory").to_variant();
  }
  return ase_variant();
}

ase_variant
ase_variant_impl_oom_exception::get_enumerator(const ase_variant& v)
{
  ase_string keys[3];
  keys[0] = ase_string("Id");
  keys[1] = ase_string("Description");
  keys[2] = ase_string("Value");
  return ase_new_enum(keys, 3);
}

ase_variant
ase_new_out_of_memory_exception() throw()
{
  return ase_variant::create_object(
    &ase_variant_impl<
      ase_variant_impl_oom_exception,
      ase_vtoption_remote_by_value
    >::vtable, 0, 0);
}

class ase_variant_impl_exception
  : public ase_variant_impl_typed<ase_variant_impl_exception> {

 public:

  static const char *const debugid;

  ase_variant_impl_exception(bool rtex, const ase_string& exid,
    const ase_string& desc, const ase_variant& c);

  static ase_string get_string(const ase_variant& v);
  static ase_variant get_property(const ase_variant& v, const char *name,
    ase_size_type len);
  static ase_variant get_enumerator(const ase_variant& v);
  static ase_variant create(bool rtex, const ase_string& desc);

  const ase_string exceptid;
  const ase_string description;
  ase_variant value;
  typedef std::vector<ase_variant> trace_type;
  trace_type trace;

};

const char *const
ase_variant_impl_exception::debugid = "Exception";

namespace {

typedef ase_variant_impl<
  ase_variant_impl_exception, ase_vtoption_remote_by_value
> ase_variant_impl_exception_tinst;

ase_string
get_string_nothrow_endl(const ase_variant& v)
{
  try {
    const ase_string r = v.get_string();
    if (r.empty()) {
      return ase_string("\n");
    }
    if (r.data()[r.size() - 1] != '\n') {
      return r + "\n";
    }
    return r;
  } catch (...) {
    return ase_string("[GETSTRING FAILED]\n");
  }
}

};

ase_string
ase_variant_impl_exception::get_string(const ase_variant& v)
{
  ase_variant_impl_exception *p = static_cast<ase_variant_impl_exception *>(
    v.get_rep().p);
  ase_string valstr = p->value.get_string();
  ase_string s = "(Id=" + p->exceptid + ") "
    + p->description + get_string_nothrow_endl(p->value);
  for (ase_size_type i = 0; i < p->trace.size(); ++i) {
    s = s + "(trace) " + get_string_nothrow_endl(p->trace[i]);
  }
  return s;
}

ase_variant
ase_variant_impl_exception::get_property(const ase_variant& v,
  const char *name, ase_size_type len)
{
  ase_variant_impl_exception *p = static_cast<ase_variant_impl_exception *>(
    v.get_rep().p);
  if (ase_string::equals(name, len, "Id")) {
    return p->exceptid.to_variant();
  }
  if (ase_string::equals(name, len, "Description")) {
    return p->description.to_variant();
  }
  if (ase_string::equals(name, len, "Value")) {
    return p->value;
  }
  return ase_variant();
}

ase_variant
ase_variant_impl_exception::get_enumerator(const ase_variant& v)
{
  ase_string keys[3];
  keys[0] = ase_string("Id");
  keys[1] = ase_string("Description");
  keys[2] = ase_string("Value");
  return ase_new_enum(keys, 3);
}

ase_variant_impl_exception::ase_variant_impl_exception(bool rtex,
  const ase_string& exid, const ase_string& desc, const ase_variant& val)
  : exceptid(exid), description(desc), value(val)
{
}

bool
ase_is_ase_exception(const ase_variant& val)
{
  if (val.get_vtable_address() == &ase_variant_impl_exception_tinst::vtable) {
    return true;
  }
  return false;
}

void
ase_exception_append_trace(const ase_variant& val, const ase_variant& trace)
{
  if (ase_is_ase_exception(val)) {
    ase_variant_impl_exception *const p =
      static_cast<ase_variant_impl_exception *>(val.get_rep().p);
    p->trace.push_back(trace);
  }
}

ase_variant
ase_new_exception_raw(const ase_string& id, const ase_string& desc,
  const ase_variant& val)
{
  return ase_variant::create_object(
    &ase_variant_impl_exception_tinst::vtable,
    new ase_variant_impl_exception(true, id, desc, val),
    "Exception");
}

ase_variant
ase_new_exception_from_strmap(const ase_variant& sm)
{
  const ase_string id = ase_nothrow_get_string(
    ase_nothrow_get_property(sm, "Id", 2));
  const ase_string desc = ase_nothrow_get_string(
    ase_nothrow_get_property(sm, "Description", 11));
  const ase_variant val = ase_nothrow_get_property(sm, "Value", 5);
  return ase_new_exception_raw(id, desc, val);
}

ase_variant
ase_convert_to_ase_exception(const ase_string& mod_name,
  const ase_string& typ_name, const ase_string& desc, const ase_variant& val)
{
  if (ase_is_ase_exception(val)) {
    return val;
  }
  ase_string ty = typ_name;
  if (ty.empty()) {
    switch (val.get_type()) {
    case ase_vtype_void:
      ty = ase_string("Void");
      break;
    case ase_vtype_bool:
      ty = ase_string("Bool");
      break;
    case ase_vtype_int:
      ty = ase_string("Int");
      break;
    case ase_vtype_long:
      ty = ase_string("Long");
      break;
    case ase_vtype_double:
      ty = ase_string("Double");
      break;
    case ase_vtype_string:
      ty = ase_string("String");
      break;
    case ase_vtype_object:
      ty = ase_string("Object");
      break;
    }
  }
  return ase_new_exception_raw(mod_name + ty, desc, val);
}

ase_variant
ase_stdexcept_to_ase_exception(const std::exception& ex)
{
  if (typeid(ex) == typeid(std::bad_alloc)) {
    return ase_new_out_of_memory_exception();
  }
  const char *const name = typeid(ex).name();
  const char *const what = ex.what();
  return ase_convert_to_ase_exception(ase_string("CPP/"), ase_string(name),
    ase_string(what), ase_variant());
}

ase_variant
ase_new_exception(const ase_string& exid, const ase_string& desc)
{
  return ase_convert_to_ase_exception(ase_string("ASE/"), exid, desc,
    ase_variant());
}

void
ase_throw_exception(const ase_string& exid, const ase_string& desc)
{
  throw ase_new_exception(exid, desc);
}

void
ase_throw_exception(const char *exid, const ase_string& desc)
{
  ase_throw_exception(ase_string(exid), desc);
}

void
ase_throw_type_mismatch(const char *desc)
{
  ase_throw_exception(ase_string("ASETypeMismatch"), ase_string(desc));
}

void
ase_throw_missing_arg(const char *desc)
{
  ase_throw_exception(ase_string("ASEMissingArg"), ase_string(desc));
}

void
ase_throw_too_many_args(const char *desc)
{
  ase_throw_exception(ase_string("ASETooManyArgs"), ase_string(desc));
}

void
ase_throw_invalid_arg(const char *desc)
{
  ase_throw_exception(ase_string("ASEInvalidArg"), ase_string(desc));
}

void
ase_throw_illegal_operation(const char *desc)
{
  ase_throw_exception(ase_string("ASEIllegalOperation"), ase_string(desc));
}

void
ase_throw_io_error(const char *desc)
{
  ase_throw_exception(ase_string("ASEIOError"), ase_string(desc));
}

void
ase_throw_error(const char *desc)
{
  ase_throw_exception(ase_string("ASEError"), ase_string(desc));
}

void
ase_throw_runtime_exception(const char *desc)
{
  ase_throw_exception(ase_string("ASERuntimeException"), ase_string(desc));
}

void
ase_check_num_args(ase_size_type nargs, ase_size_type expected,
  const char *desc)
{
  if (nargs < expected) {
    ase_throw_missing_arg(desc);
  }
  if (nargs > expected) {
    ase_throw_too_many_args(desc);
  }
}

bool
ase_validate_boolean(const ase_variant& v, const char *desc)
{
  if (v.is_numeric()) {
    return v.get_boolean();
  }
  ase_throw_type_mismatch(desc);
  return false;
}

ase_int
ase_validate_int(const ase_variant& v, const char *desc)
{
  if (v.is_numeric()) {
    return v.get_int();
  }
  ase_throw_type_mismatch(desc);
  return 0;
}

ase_long ase_validate_long(const ase_variant& v, const char *desc)
{
  if (v.is_numeric()) {
    return v.get_long();
  }
  ase_throw_type_mismatch(desc);
  return 0;
}

double ase_validate_double(const ase_variant& v, const char *desc)
{
  if (v.is_numeric()) {
    return v.get_double();
  }
  ase_throw_type_mismatch(desc);
  return 0;
}

ase_string ase_validate_string(const ase_variant& v, const char *desc)
{
  if (v.get_type() == ase_vtype_string) {
    return v.get_string();
  }
  ase_throw_type_mismatch(desc);
  return ase_string();
}

void ase_validate_object(const ase_variant& v, const char *desc)
{
  if (v.get_type() != ase_vtype_object) {
    ase_throw_type_mismatch(desc);
  }
}

void ase_validate_array(const ase_variant& v, const char *desc)
{
  ase_validate_object(v, desc);
  if (v.get_length() < 0) {
    ase_throw_type_mismatch(desc);
  }
}

ase_string ase_nothrow_get_string(const ase_variant& v)
{
  try {
    return v.get_string();
  } catch (...) {
    try {
      return ase_string("[GetStringThrows]");
    } catch (...) {
    }
  }
  return ase_string();
}

ase_variant ase_nothrow_get_property(const ase_variant& v, const char *name,
  ase_size_type namelen)
{
  try {
    return v.get_property(name, namelen);
  } catch (...) {
    try {
      return ase_string("[GetPropertyThrows]").to_variant();
    } catch (...) {
    }
  }
  return ase_variant();
}

