
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEJS_ASEIMPL_HPP
#define GENTYPE_ASEJS_ASEIMPL_HPP

#include "asejs_impl.hpp"

namespace asejs {

struct ase_js_ase_variant_impl : public ase_variant_impl_default {

 public:

  friend struct ase_js_script_engine;
  friend struct ase_js_ase_variant_glfunc_impl;

  static ase_js_ase_variant_implnode *get(const ase_variant& self) {
    return static_cast<ase_js_ase_variant_implnode *>(self.get_rep().p);
  }
  static ase_js_script_engine *getctx(const ase_variant& self) {
    return static_cast<ase_js_script_engine *>(self.get_impl_priv());
  }

  struct init_arg {
    init_arg(ase_js_script_engine *sc, JSContext *cx, JSObject *o,
      JSObject *ft)
      : script_engine(sc), jscontext(cx), object(o), functhis(ft) { }
    ase_js_script_engine *const script_engine;
    JSContext *const jscontext;
    JSObject *const object;
    JSObject *const functhis;
  };

  ase_js_ase_variant_impl(const init_arg& arg);
  ~ase_js_ase_variant_impl(); /* DCHK */
  static void add_ref(const ase_variant& self);
  static void release(const ase_variant& self);
  static ase_int get_attributes(const ase_variant& self);
  static ase_string get_string(const ase_variant& self);
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

 public:

  void dispose(ase_js_script_engine *sc);
  JSObject *get_object() const { return jobject; }

 private:

  /* noncopyable */
  ase_js_ase_variant_impl(const ase_js_ase_variant_impl& x);
  ase_js_ase_variant_impl& operator =(const ase_js_ase_variant_impl& x);

 private:

  mutable JSObject *jobject;
  mutable JSObject *function_thisobject; /* used if jobject is a Function */
  mutable ase_atomic_count refcount;

};

struct ase_js_ase_variant_glfunc_impl : public ase_variant_impl_default {

 public:

  static ase_js_ase_variant_glfunc_impl *get(const ase_variant& self) {
    return static_cast<ase_js_ase_variant_glfunc_impl *>(self.get_rep().p);
  }
  static ase_js_script_engine *getctx(const ase_variant& self) {
    return static_cast<ase_js_script_engine *>(self.get_impl_priv());
  }

  typedef ase_variant (*funcptr_type)(ase_js_script_engine *sc,
    JSContext *cx, const ase_variant *args, ase_size_type nargs);

  ase_js_ase_variant_glfunc_impl(ase_js_script_engine *sc,
    JSContext *cx, funcptr_type ptr);
  ~ase_js_ase_variant_glfunc_impl(); /* DCHK */

  void dispose();

  static void add_ref(const ase_variant& self);
  static void release(const ase_variant& self);
  static ase_variant invoke_self(const ase_variant& self,
    const ase_variant *args, ase_size_type nargs);

 private:

  ase_js_script_engine *script_engine;
  funcptr_type funcptr;
  mutable ase_atomic_count refcount;
  bool disposed;

};

ase_variant ase_js_ase_variant_glfunc_new(ase_js_script_engine *sc,
  JSContext *cx, const ase_variant *args, ase_size_type nargs);
ase_variant ase_js_ase_variant_glfunc_eval(ase_js_script_engine *sc,
  JSContext *cx, const ase_variant *args, ase_size_type nargs);
ase_variant ase_js_ase_variant_glfunc_print(ase_js_script_engine *sc,
  JSContext *cx, const ase_variant *args, ase_size_type nargs);
ase_variant ase_js_ase_variant_glfunc_gc(ase_js_script_engine *sc,
  JSContext *cx, const ase_variant *args, ase_size_type nargs);

}; // namespace asejs

#endif

