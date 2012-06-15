
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEJS_UTIL_HPP
#define GENTYPE_ASEJS_UTIL_HPP

#include "asejs_impl.hpp"

#define ASE_JS_VERBOSE_PMC(x) \
  if ((asejs::ase_js_util::verbose & 1) != 0) { x; };
#define ASE_JS_VERBOSE_EXC(x) \
  if ((asejs::ase_js_util::verbose & 2) != 0) { x; };

namespace asejs {

struct ase_js_scoped_root {

 public:

  ase_js_scoped_root();
  ase_js_scoped_root(JSContext *cx, jsval val, bool need_addroot);
  ase_js_scoped_root(const ase_js_scoped_root& x);
  ase_js_scoped_root& operator =(const ase_js_scoped_root& x);
  ~ase_js_scoped_root(); /* DCHK */

  jsval get() const { return value; }
  jsval *getaddr() { return &value; }

 private:

  jsval value;
  JSContext *jsctx;
  bool add_root;

};

struct ase_js_util {

  static unsigned int verbose;

  static ase_string get_string(ase_js_script_engine *sc, JSContext *cx,
    jsval val);
  static bool is_ase_variant(ase_js_script_engine *sc, JSContext *cx,
    jsval jv);
  static ase_variant to_ase_variant(ase_js_script_engine *sc,
    JSContext *cx, jsval jv);
  static ase_variant to_ase_variant(ase_js_script_engine *sc,
    JSContext *cx, jsval jv, JSObject *thisobj);
  static ase_js_scoped_root to_js_value_internal(ase_js_script_engine *sc,
    JSContext *cx, const ase_variant& av, bool need_root);
    /* NOTE: if av is already a js value, retval is not additionally
      rooted. so, when av is destroyed, retval.value becomes invalid. */ 
  static ase_js_scoped_root to_js_value_need_root(ase_js_script_engine *sc,
    JSContext *cx, const ase_variant& av);
  static ase_js_scoped_root to_js_value_no_root(ase_js_script_engine *sc,
    JSContext *cx, const ase_variant& av);
  static jsval *to_js_value_array(ase_js_script_engine *sc, JSContext *cx,
    const ase_variant *arr, ase_size_type len,
    std::vector<ase_js_scoped_root>& jrarr, std::vector<jsval>& jarr);
  static jsval new_ase_proxy(ase_js_script_engine *sc, JSContext *cx,
    const ase_variant& av);
  static void handle_exception(ase_js_script_engine *sc, JSContext *cx)
    throw();
  static void ase_js_dump_jsval(JSContext *cx, jsval val);

  static const jschar *to_jschar(const ase_ucchar *s) {
    return reinterpret_cast<const jschar *>(s);
  }
  static const ase_ucchar *to_ucchar(const jschar *s) {
    return reinterpret_cast<const ase_ucchar *>(s);
  }

};

}; // namespace asejs

#endif

