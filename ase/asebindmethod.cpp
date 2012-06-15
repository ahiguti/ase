
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asebindmethod.hpp>
#include <ase/aseatomic.hpp>

namespace {

class vimpl_bindid : public ase_variant_impl_default {

 public:

  vimpl_bindid(const ase_variant& v, ase_int i)
    : value(v), id(i), refcount(1) { }
  static void add_ref(const ase_variant& v) {
    get(v)->refcount++;
  }
  static void release(const ase_variant& v) {
    vimpl_bindid *p = get(v);
    if ((p->refcount--) == 1) {
      ase_variant::debug_on_destroy_object(v, "BindMethodId");
      delete p;
    }
  }
  static ase_int get_attributes(const ase_variant& v) {
    return ase_attrib_function;
  }
  static ase_string get_string(const ase_variant& v) {
    return ase_string("[ASEBindMethodId]");
  }
  static ase_variant invoke_self(const ase_variant& v, const ase_variant *args,
    ase_size_type nargs) {
    vimpl_bindid *p = get(v);
    return p->value.invoke_by_id(p->id, args, nargs);
  }

 protected:

  static vimpl_bindid *get(const ase_variant& v) {
    return static_cast<vimpl_bindid *>(v.get_rep().p);
  }

 private:

  const ase_variant value;
  const ase_int id;
  ase_atomic_count refcount;

};

class vimpl_bindname : public ase_variant_impl_default {

 public:

  vimpl_bindname(const ase_variant& v, const ase_string& s)
    : value(v), name(s), refcount(1) { }
  static void add_ref(const ase_variant& v) {
    get(v)->refcount++;
  }
  static void release(const ase_variant& v) {
    vimpl_bindname *p = get(v);
    if ((p->refcount--) == 1) {
      ase_variant::debug_on_destroy_object(v, "BindMethodName");
      delete p;
    }
  }
  static ase_int get_attributes(const ase_variant& v) {
    return ase_attrib_function;
  }
  static ase_string get_string(const ase_variant& v) {
    return ase_string("[ASEBindMethodName]");
  }
  static ase_variant invoke_self(const ase_variant& v, const ase_variant *args,
    ase_size_type nargs) {
    vimpl_bindname *p = get(v);
    return p->value.invoke_by_name(p->name.data(), p->name.size(), args,
      nargs);
  }

 protected:

  static vimpl_bindname *get(const ase_variant& v) {
    return static_cast<vimpl_bindname *>(v.get_rep().p);
  }

 private:

  const ase_variant value;
  const ase_string name;
  ase_atomic_count refcount;

};

}; // anonymous namespace

ase_variant
ase_new_variant_bind_method_id(const ase_variant& val, ase_int id)
{
  if ((val.get_vtoption() & ase_vtoption_multithreaded) != 0) {
    /* inherit unlock flag */
    return ase_variant::create_object(
      &ase_variant_impl<
	vimpl_bindid,
	ase_vtoption_multithreaded
      >::vtable,
      new vimpl_bindid(val, id),
      "BindMethodId");
  } else {
    return ase_variant::create_object(
      &ase_variant_impl<vimpl_bindid>::vtable,
      new vimpl_bindid(val, id),
      "BindMethodId");
  }
}

ase_variant
ase_new_variant_bind_method_name(const ase_variant& val, const ase_string& s)
{
  if ((val.get_vtoption() & ase_vtoption_multithreaded) != 0) {
    /* inherit unlock flag */
    return ase_variant::create_object(
      &ase_variant_impl<
	vimpl_bindname,
	ase_vtoption_multithreaded
      >::vtable,
      new vimpl_bindname(val, s),
      "BindMethodName");
  } else {
    return ase_variant::create_object(
      &ase_variant_impl<vimpl_bindname>::vtable,
      new vimpl_bindname(val, s),
      "BindMethodName");
  }
}

