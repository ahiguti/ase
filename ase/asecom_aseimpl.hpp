
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASECOM_ASEIMPL_HPP
#define GENTYPE_ASECOM_ASEIMPL_HPP

#include <vector>

#include "asecom_sctx.hpp"

namespace asecom {

struct ase_com_ase_variant_impl {

 public:

  friend struct ase_com_script_engine;
  friend struct ase_com_ase_variant_glfunc_impl;

  static ase_com_ase_variant_implnode *get(const ase_variant& self) {
    return static_cast<ase_com_ase_variant_implnode *>(self.get_rep().p);
  }
  static ase_com_script_engine *getctx(const ase_variant& self) {
    return static_cast<ase_com_script_engine *>(self.get_impl_priv());
  }
  static ase_int get_method_id(const ase_variant& self, const char *name,
    ase_size_type namelen);

  struct init_arg {
    init_arg(ase_com_script_engine *sc, IDispatch *val)
      : sctx(sc), value(val) { }	
    ase_com_script_engine *const sctx;
    IDispatch *const value;
  };

  ase_com_ase_variant_impl(const init_arg& arg);
  ~ase_com_ase_variant_impl(); /* DCHK */
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

 public:

  void dispose();
  void connect(ase_com_script_engine *sc, IDispatch *sink,
    const ase_string& interface_name);
  
  IDispatch *get() const { return value; }

 private:

  /* noncopyable */
  ase_com_ase_variant_impl(const ase_com_ase_variant_impl& x);
  ase_com_ase_variant_impl& operator =(const ase_com_ase_variant_impl& x);

 private:

  struct connection_type {
    CComPtr<IConnectionPoint> source;
    DWORD cookie;
  };
  typedef std::vector<connection_type> connections_type;

  mutable ase_atomic_count refcount;
  IDispatch *value;
  connections_type *connections;

};

}; // namespace asecom

#endif

