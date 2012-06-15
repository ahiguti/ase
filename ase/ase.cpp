
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/ase.hpp>
#include <ase/aseatomic.hpp>
#include <ase/aseexcept.hpp>
#include <ase/aseflags.hpp>
#include <ase/asealgo.hpp>

#include <memory>
#include <vector>
#include <set>
#include <map>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <cstdio>

#include <assert.h>
#include <stdarg.h>

#define DBG_WEAK(x)
#define DBG_THROW_ABORT(x)
#define DBG_CATCH_ABORT(x)
#define DBG_THROW(x)
#define DBG_CATCH(x)
#define DBG_DUMPOBJ(x)
#define DBG_GMI(x)

namespace {

ase_atomic_count ase_variant_object_count_create = 0;
ase_atomic_count ase_variant_object_count_destroy = 0;

int
snprintf_nulterm(char *buffer, size_t buffer_size, const char *format, ...)
{
  assert(buffer_size > 0);
  va_list ap;
  va_start(ap, format);
  #ifdef __GNUC__
  int r = vsnprintf(buffer, buffer_size, format, ap);
  #elif _MSC_VER >= 1400
  int r = _vsnprintf_s(buffer, buffer_size, buffer_size - 1, format, ap);
  #else
  int r = _vsnprintf(buffer, buffer_size, format, ap);
  #endif
  va_end(ap);
  /* win32: _vsnprintf does not nul-terminate if buffer is full */
  buffer[buffer_size - 1] = '\0';
  if (r >= 0 && static_cast<size_t>(r) < buffer_size) {
    return r;
  }
  return -1;
}

ase_variant
create_exception_variant(const char *errid, const std::exception& e)
{
  return ase_new_exception(ase_string(errid), ase_string(e.what()));
}

}; // anonymous namespace

ase_variant
ase_variant::create_from_active_exception() throw()
{
  try {
    try {
      throw;
    } catch (const ase_variant& v) {
      return v;
    } catch (const std::bad_alloc& ) {
      return ase_new_out_of_memory_exception();
    } catch (const std::exception& e) {
      return ase_stdexcept_to_ase_exception(e);
    }
  } catch (...) {
    /* out of memory */
    return ase_new_out_of_memory_exception();
  }
}

void
ase_variant::debug_createobj_internal(const ase_variant& v,
  const char *debugid)
{
  if (v.get_type() == ase_vtype_string || debugid == 0) {
    return;
  }
  assert(v.get_type() == ase_vtype_object);
  ASE_DEBUG(const ase_variant_vtable& vtbl = *v.vtbl);
  assert(vtbl.add_ref);
  assert(vtbl.release);
  assert(vtbl.get_attributes);
  assert(vtbl.get_boolean);
  assert(vtbl.get_int);
  assert(vtbl.get_long);
  assert(vtbl.get_double);
  assert(vtbl.get_string);
  assert(vtbl.is_same_object);
  assert(vtbl.get_property);
  assert(vtbl.set_property);
  assert(vtbl.del_property);
  assert(vtbl.get_element);
  assert(vtbl.set_element);
  assert(vtbl.get_length);
  assert(vtbl.set_length);
  assert(vtbl.invoke_by_name);
  assert(vtbl.invoke_by_id);
  assert(vtbl.invoke_self);
  assert(vtbl.get_enumerator);
  assert(vtbl.enum_next);
  ase_variant_object_count_create++;
  DBG_DUMPOBJ(fprintf(stderr, "VOBJ create id=%s vtbl=%p rep=%p\n",
    debugid, v.vtbl, v.get_rep().p));
}

void
ase_variant::debug_destroyobj_internal(const ase_variant& v,
  const char *debugid)
{
  if (v.get_type() == ase_vtype_string) {
    return;
  }
  assert(v.get_type() == ase_vtype_object);
  ase_variant_object_count_destroy++;
  DBG_DUMPOBJ(fprintf(stderr, "VOBJ destroy id=%s vtbl=%p rep=%p\n",
    debugid, v.vtbl, v.rep.p));
}

void
ase_variant::show_statistics()
{
  ASE_DEBUG_VARIANT_COUNT_DO(fprintf(stderr, "VOBJ     c = %d\n",
    (int)ase_variant_object_count_create));
  ASE_DEBUG_VARIANT_COUNT_DO(fprintf(stderr, "VOBJ     d = %d\n",
    (int)ase_variant_object_count_destroy));
}

int
ase_variant::compare_method_entry(const char *xname, ase_size_type xnamelen,
  const char *yname, ase_size_type ynamelen)
{
  /* NOTE: this function compares lengths first and memcmp second. */
  if (xnamelen != ynamelen) {
    return static_cast<int>(xnamelen) - static_cast<int>(ynamelen);
  }
  return std::memcmp(xname, yname, xnamelen);
}

namespace {

struct compare_method_entry_fobj {
  int operator ()(const ase_variant_method_entry& x,
    const ase_variant_method_entry& y) {
    return ase_variant::compare_method_entry(x.u8name, x.u8name_len, y.u8name,
      y.u8name_len);
  }
};

ase_int
get_method_id_internal(const ase_variant_vtable& vtbl, const char *name,
  ase_size_type namelen)
{
  if (vtbl.methods.num_methods > 0) {
    ase_variant_method_entry e;
    e.u8name = name;
    e.u8name_len = namelen;
    const ase_variant_method_entry *const first = vtbl.methods.methods;
    const ase_variant_method_entry *const last = first +
      vtbl.methods.num_methods;
    const ase_variant_method_entry *const notfound = 0;
    const ase_variant_method_entry *const i = ase_binary_search(first, last,
      notfound, e, compare_method_entry_fobj());
      /* compares length first, memcmp second */
    DBG_GMI(std::printf("M NUM=%d\n", vtbl.methods.num_methods));
    if (i != notfound) {
      return static_cast<ase_int>(i - first);
    }
  }
  return ase_method_id_not_found;
}

};

ase_int
ase_variant::get_method_id(const char *name, ase_size_type namelen) const
{
  return get_method_id_internal(*vtbl, name, namelen);
}

ase_int
ase_variant::get_instance_method_id(const char *name, ase_size_type namelen)
  const
{
  const ase_variant_vtable& vt = *vtbl;
  if (vt.instance == 0) {
    return ase_method_id_not_found;
  }
  return get_method_id_internal(*vt.instance, name, namelen);
}

void
ase_variant_impl_default::add_ref(const ase_variant& v)
{
}

void
ase_variant_impl_default::release(const ase_variant& v)
{
}

ase_int
ase_variant_impl_default::get_attributes(const ase_variant& v)
{
  return ase_attrib_none;
}

bool
ase_variant_impl_default::get_boolean(const ase_variant& v)
{
  return false;
}

ase_int
ase_variant_impl_default::get_int(const ase_variant& v)
{
  return 0;
}

ase_long
ase_variant_impl_default::get_long(const ase_variant& v)
{
  return 0;
}

double
ase_variant_impl_default::get_double(const ase_variant& v)
{
  return 0.0;
}

ase_string
ase_variant_impl_default::get_string(const ase_variant& v)
{
  return ase_string();
}
bool
ase_variant_impl_default::is_same_object(const ase_variant& v,
  const ase_variant& val)
{
  if (&v == &val) {
    return true;
  }
  if (v.get_vtable_address() != val.get_vtable_address()) {
    return false;
  }
  const ase_variant::rep_type& xrep = v.get_rep();
  const ase_variant::rep_type& yrep = val.get_rep();
  switch (v.get_type()) {
  case ase_vtype_void:
    return true;
  case ase_vtype_bool:
  case ase_vtype_int:
    return xrep.i == yrep.i;
  case ase_vtype_long:
    return xrep.l == yrep.l;
  case ase_vtype_double:
    return xrep.d == yrep.d;
  case ase_vtype_string:
  case ase_vtype_object:
    return xrep.p == yrep.p;
  }
  return false;
}

ase_variant
ase_variant_impl_default::get_property(const ase_variant& v, const char *name,
  ase_size_type len)
{
  ase_throw_illegal_operation("attempt to get property");
  return ase_variant();
}

void
ase_variant_impl_default::set_property(const ase_variant& v, const char *name,
  ase_size_type len, const ase_variant& value)
{
  ase_throw_illegal_operation("attempt to set property");
}

void
ase_variant_impl_default::del_property(const ase_variant& v, const char *name,
  ase_size_type len)
{
  ase_throw_illegal_operation("attempt to delete property");
}

ase_variant
ase_variant_impl_default::get_element(const ase_variant& v, ase_int idx)
{
  ase_throw_illegal_operation("attempt to get element");
  return ase_variant();
}

void
ase_variant_impl_default::set_element(const ase_variant& v, ase_int idx,
  const ase_variant& value)
{
  ase_throw_illegal_operation("attempt to set element");
}

ase_int
ase_variant_impl_default::get_length(const ase_variant& v)
{
  return -1;
}

void
ase_variant_impl_default::set_length(const ase_variant& v, ase_int len)
{
}

ase_variant
ase_variant_impl_default::invoke_by_name(const ase_variant& v,
  const char *name, ase_size_type namelen, const ase_variant *args,
  ase_size_type nargs)
{
  ase_throw_illegal_operation("attempt to invoke method");
  return ase_variant();
}

ase_variant
ase_variant_impl_default::invoke_by_id(const ase_variant& v, ase_int id,
  const ase_variant *args, ase_size_type nargs)
{
  ase_throw_illegal_operation("attempt to invoke method");
  return ase_variant();
}

ase_variant
ase_variant_impl_default::invoke_self(const ase_variant& v,
  const ase_variant *args, ase_size_type nargs)
{
  ase_throw_illegal_operation("attempt to invoke");
  return ase_variant();
}

ase_variant
ase_variant_impl_default::get_enumerator(const ase_variant& v)
{
  return ase_variant();
}

ase_string
ase_variant_impl_default::enum_next(const ase_variant& v, bool& hasnext_r)
{
  ase_throw_illegal_operation("attempt to enumerate");
  return ase_string();
}

class ase_variant_impl_boolean {

 public:

  static ase_int get(const ase_variant& v) { return v.get_rep().i; }
  static bool get_boolean(const ase_variant& v) { return get(v) != 0; }
  static ase_int get_int(const ase_variant& v) { return get(v) != 0; }
  static ase_long get_long(const ase_variant& v) { return get(v) != 0; }
  static double get_double(const ase_variant& v) { return get(v) != 0; }
  static ase_string get_string(const ase_variant& v) {
    if (get(v)) {
      return ase_string("true");
    } else {
      return ase_string("false");
    }
  }

};

class ase_variant_impl_int {

 public:

  static ase_int get(const ase_variant& v) { return v.get_rep().i; }
  static bool get_boolean(const ase_variant& v) { return get(v) != 0; }
  static ase_int get_int(const ase_variant& v) { return get(v); }
  static ase_long get_long(const ase_variant& v) { return get(v); }
  static double get_double(const ase_variant& v) { return get(v); }
  static ase_string get_string(const ase_variant& v) {
    char buf[64];
    const ase_int iv = get(v);
    snprintf_nulterm(buf, 64, "%d", iv);
    return ase_string(buf, std::strlen(buf));
  }

};

class ase_variant_impl_long {

 public:

  static ase_long get(const ase_variant& v) { return v.get_rep().l; }
  static bool get_boolean(const ase_variant& v) { return get(v) != 0; }
  static ase_int get_int(const ase_variant& v) {
    return static_cast<ase_int>(get(v)); }
  static ase_long get_long(const ase_variant& v) { return get(v); }
  static double get_double(const ase_variant& v) {
    return static_cast<double>(get(v)); }
  static ase_string get_string(const ase_variant& v) {
    char buf[64];
    const ase_long lv = get(v);
    snprintf_nulterm(buf, 64, "%lld", lv);
    return ase_string(buf, std::strlen(buf));
  }

};

class ase_variant_impl_double {

 public:

  static double get(const ase_variant& v) { return v.get_rep().d; }
  static bool get_boolean(const ase_variant& v) { return get(v) != 0.0; }
  static ase_int get_int(const ase_variant& v) {
    return static_cast<ase_int>(get(v)); }
  static ase_long get_long(const ase_variant& v) {
    return static_cast<ase_long>(get(v)); }
  static double get_double(const ase_variant& v) { return get(v); }
  static ase_string get_string(const ase_variant& v) {
    char buf[64];
    const double dv = get(v);
    snprintf_nulterm(buf, 64, "%lf", dv);
    return ase_string(buf, std::strlen(buf));
  }

};

const ase_variant_vtable
ase_variant_default_vtable = {
  0, ase_vtype_void, ase_vtoption_none, { 0, 0 }, 0,
  ase_variant_impl_default::add_ref,
  ase_variant_impl_default::release,
  ase_variant_impl_default::get_attributes,
  ase_variant_impl_default::get_boolean,
  ase_variant_impl_default::get_int,
  ase_variant_impl_default::get_long,
  ase_variant_impl_default::get_double,
  ase_variant_impl_default::get_string,
  ase_variant_impl_default::is_same_object,
  ase_variant_impl_default::get_property,
  ase_variant_impl_default::set_property,
  ase_variant_impl_default::del_property,
  ase_variant_impl_default::get_element,
  ase_variant_impl_default::set_element,
  ase_variant_impl_default::get_length,
  ase_variant_impl_default::set_length,
  ase_variant_impl_default::invoke_by_name,
  ase_variant_impl_default::invoke_by_id,
  ase_variant_impl_default::invoke_self,
  ase_variant_impl_default::get_enumerator,
  ase_variant_impl_default::enum_next,
};

const ase_variant_vtable
ase_variant_boolean_vtable = {
  0, ase_vtype_bool, ase_vtoption_none, { 0, 0 }, 0,
  ase_variant_impl_default::add_ref,
  ase_variant_impl_default::release,
  ase_variant_impl_default::get_attributes,
  ase_variant_impl_boolean::get_boolean,
  ase_variant_impl_boolean::get_int,
  ase_variant_impl_boolean::get_long,
  ase_variant_impl_boolean::get_double,
  ase_variant_impl_boolean::get_string,
  ase_variant_impl_default::is_same_object,
  ase_variant_impl_default::get_property,
  ase_variant_impl_default::set_property,
  ase_variant_impl_default::del_property,
  ase_variant_impl_default::get_element,
  ase_variant_impl_default::set_element,
  ase_variant_impl_default::get_length,
  ase_variant_impl_default::set_length,
  ase_variant_impl_default::invoke_by_name,
  ase_variant_impl_default::invoke_by_id,
  ase_variant_impl_default::invoke_self,
  ase_variant_impl_default::get_enumerator,
  ase_variant_impl_default::enum_next,
};

const ase_variant_vtable
ase_variant_int_vtable = {
  0, ase_vtype_int, ase_vtoption_none, { 0, 0 }, 0,
  ase_variant_impl_default::add_ref,
  ase_variant_impl_default::release,
  ase_variant_impl_default::get_attributes,
  ase_variant_impl_int::get_boolean,
  ase_variant_impl_int::get_int,
  ase_variant_impl_int::get_long,
  ase_variant_impl_int::get_double,
  ase_variant_impl_int::get_string,
  ase_variant_impl_default::is_same_object,
  ase_variant_impl_default::get_property,
  ase_variant_impl_default::set_property,
  ase_variant_impl_default::del_property,
  ase_variant_impl_default::get_element,
  ase_variant_impl_default::set_element,
  ase_variant_impl_default::get_length,
  ase_variant_impl_default::set_length,
  ase_variant_impl_default::invoke_by_name,
  ase_variant_impl_default::invoke_by_id,
  ase_variant_impl_default::invoke_self,
  ase_variant_impl_default::get_enumerator,
  ase_variant_impl_default::enum_next,
};

const ase_variant_vtable
ase_variant_long_vtable = {
  0, ase_vtype_long, ase_vtoption_none, { 0, 0 }, 0,
  ase_variant_impl_default::add_ref,
  ase_variant_impl_default::release,
  ase_variant_impl_default::get_attributes,
  ase_variant_impl_long::get_boolean,
  ase_variant_impl_long::get_int,
  ase_variant_impl_long::get_long,
  ase_variant_impl_long::get_double,
  ase_variant_impl_long::get_string,
  ase_variant_impl_default::is_same_object,
  ase_variant_impl_default::get_property,
  ase_variant_impl_default::set_property,
  ase_variant_impl_default::del_property,
  ase_variant_impl_default::get_element,
  ase_variant_impl_default::set_element,
  ase_variant_impl_default::get_length,
  ase_variant_impl_default::set_length,
  ase_variant_impl_default::invoke_by_name,
  ase_variant_impl_default::invoke_by_id,
  ase_variant_impl_default::invoke_self,
  ase_variant_impl_default::get_enumerator,
  ase_variant_impl_default::enum_next,
};

const ase_variant_vtable
ase_variant_double_vtable = {
  0, ase_vtype_double, ase_vtoption_none, { 0, 0 }, 0,
  ase_variant_impl_default::add_ref,
  ase_variant_impl_default::release,
  ase_variant_impl_default::get_attributes,
  ase_variant_impl_double::get_boolean,
  ase_variant_impl_double::get_int,
  ase_variant_impl_double::get_long,
  ase_variant_impl_double::get_double,
  ase_variant_impl_double::get_string,
  ase_variant_impl_default::is_same_object,
  ase_variant_impl_default::get_property,
  ase_variant_impl_default::set_property,
  ase_variant_impl_default::del_property,
  ase_variant_impl_default::get_element,
  ase_variant_impl_default::set_element,
  ase_variant_impl_default::get_length,
  ase_variant_impl_default::set_length,
  ase_variant_impl_default::invoke_by_name,
  ase_variant_impl_default::invoke_by_id,
  ase_variant_impl_default::invoke_self,
  ase_variant_impl_default::get_enumerator,
  ase_variant_impl_default::enum_next,
};

