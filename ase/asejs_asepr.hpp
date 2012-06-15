
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEJS_ASEPR_HPP
#define GENTYPE_ASEJS_ASEPR_HPP

#include "asejs_impl.hpp"

namespace asejs {

struct ase_js_aseproxy_priv : public ase_variant {

 public:

  friend struct ase_js_script_engine;

  struct init_arg {
    init_arg(ase_js_script_engine *sc, const ase_variant& v)
      : script_engine(sc), valueref(v) { }
    ase_js_script_engine *const script_engine;
    const ase_variant& valueref;
  };

  ase_js_aseproxy_priv(const init_arg& ini)
    : ase_variant(ini.valueref), script_engine(ini.script_engine),
      attributes(-1) {
    #ifndef NDEBUG
    ase_js_asepr_count_constr++;
    #endif
  }
  ~ase_js_aseproxy_priv() /* DCHK */ {
    #ifndef NDEBUG
    ase_js_asepr_count_destr++;
    #endif
  }
  void dispose(const ase_js_script_engine *sc) {
    ase_variant& s = *this;
    s = ase_variant();
  }
  ase_js_script_engine *get_script_engine() const {
    return script_engine;
  }
  ase_int get_attributes_internal() const {
    if (attributes < 0) {
      attributes = get_attributes();
    }
    return attributes;
  }

 private:

  ase_js_script_engine *const script_engine;
  mutable ase_int attributes;

 private:

  /* noncopyable */
  ase_js_aseproxy_priv(const ase_js_aseproxy_priv& x);
  ase_js_aseproxy_priv& operator =(const ase_js_aseproxy_priv& x);

};

JSBool ase_js_aseproxy_delprop(JSContext *cx, JSObject *obj, jsval id,
  jsval *vp);
JSBool ase_js_aseproxy_getprop(JSContext *cx, JSObject *obj, jsval id,
  jsval *vp);
JSBool ase_js_aseproxy_setprop(JSContext *cx, JSObject *obj, jsval id,
  jsval *vp);
JSBool ase_js_aseproxy_enumerate(JSContext *cx, JSObject *obj);
JSBool ase_js_aseproxy_convert(JSContext *cx, JSObject *obj, JSType ty,
  jsval *val_r);
JSBool ase_js_aseproxy_constr(JSContext *cx, JSObject *obj, uintN argc,
  jsval *argv, jsval *val_r);
void ase_js_aseproxy_finalize(JSContext *cx, JSObject *obj);
JSBool ase_js_aseproxy_call(JSContext *cx, JSObject *obj, uintN argc,
  jsval *argv, jsval *val_r);
JSBool ase_js_aseproxy_to_string(JSContext *cx, JSObject *obj, uintN argc,
  jsval *argv, jsval *val_r);
JSBool ase_js_asemethodproxy_call(JSContext *cx, JSObject *obj, uintN argc,
  jsval *argv, jsval *val_r);
JSBool ase_js_asemethodproxy_convert(JSContext *cx, JSObject *obj, JSType ty,
  jsval *val_r);
JSBool ase_js_aseproxy_method(JSContext *cx, JSObject *obj, uintN argc,
  jsval *argv, jsval *val_r);

}; // namespace asejs

#endif

