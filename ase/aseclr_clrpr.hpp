
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASECLR_CLRPR_HPP
#define GENTYPE_ASECLR_CLRPR_HPP

#include <vcclr.h>
#include <ase/ase.hpp>
#include <ase/aseatomic.hpp>
#include <ase/aselist.hpp>

namespace aseclr {

struct ase_clr_script_engine;

struct ase_clr_ase_variant_impl;
typedef ase_list_node<ase_clr_ase_variant_impl> ase_clr_ase_variant_implnode;

struct ase_clr_ase_variant_impl : ase_noncopyable {

 public:

  struct init_arg {
    ase_clr_script_engine *sc;
    gcroot<System::Object ^> obj;
  };

  static ase_clr_ase_variant_implnode *get(const ase_variant& self) {
    return static_cast<ase_clr_ase_variant_implnode *>(self.get_rep().p);
  }

  ase_clr_ase_variant_impl(const init_arg& arg);
  ~ase_clr_ase_variant_impl();
  void dispose();

  static void add_ref(const ase_variant& self);
  static void release(const ase_variant& self);
  static ase_int get_attributes(const ase_variant& self);
  static bool get_boolean(const ase_variant& self);
  static ase_int get_int(const ase_variant& self);
  static ase_long get_long(const ase_variant& self);
  static double get_double(const ase_variant& self);
  static ase_string get_string(const ase_variant& self);
  static bool is_same_object(const ase_variant& self, const ase_variant& value);
  static ase_variant get_property(const ase_variant& self,
    const char *name, ase_size_type namelen);
  static void set_property(const ase_variant& self, const char *name,
    ase_size_type namelen, const ase_variant& value);
  static void del_property(const ase_variant& self, const char *name,
    ase_size_type namelen);
  static ase_variant get_element(const ase_variant& self, ase_int idx);
  static void set_element(const ase_variant& self, ase_int idx,
    const ase_variant& value);
  static ase_int get_length(const ase_variant& self);
  static void set_length(const ase_variant& self, ase_int len);
  static ase_variant invoke_by_name(const ase_variant& self,
    const char *name, ase_size_type namelen, const ase_variant *args,
    ase_size_type nargs);
  static ase_variant invoke_by_id(const ase_variant& self, ase_int id,
    const ase_variant *args, ase_size_type nargs);
  static ase_variant invoke_self(const ase_variant& self,
    const ase_variant *args, ase_size_type nargs);
  static ase_variant get_enumerator(const ase_variant& self);
  static ase_string enum_next(const ase_variant& self, bool& hasnext_r);

  System::Object ^get() { return value; }
  ase_clr_script_engine *get_sc() { return script_engine; }

 private:

  mutable ase_atomic_count refcount;
  ase_clr_script_engine *script_engine;
  gcroot<System::Object ^> value;
  
};

ase_variant create_clrproxy(ase_clr_script_engine *sc, System::Object ^obj);
System::Object ^extract_clrproxy(const ase_variant& val);

}; // namespace aseclr

#endif

