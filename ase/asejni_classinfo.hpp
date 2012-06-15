
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEJNI_CLASSINFO_HPP
#define GENTYPE_ASEJNI_CLASSINFO_HPP

#include "asejni_util.hpp"

namespace asejni {

struct ase_jni_script_engine;

enum ase_jni_jvalue_type {
  ase_jni_jvalue_type_void,
  ase_jni_jvalue_type_boolean, /* jboolean z */
  ase_jni_jvalue_type_byte,    /* jbyte    b */
  ase_jni_jvalue_type_char,    /* jchar    c */
  ase_jni_jvalue_type_short,   /* jshort   s */
  ase_jni_jvalue_type_int,     /* jint     i */
  ase_jni_jvalue_type_long,    /* jlong    j */
  ase_jni_jvalue_type_float,   /* jfloat   f */
  ase_jni_jvalue_type_double,  /* jdouble  d */
  ase_jni_jvalue_type_object,  /* jobject  l */
};

typedef ase_variant ase_jni_j2a_func(JNIEnv *env, ase_jni_script_engine *sc,
  const jvalue& jval, ase_jni_classinfo *ci, bool examine_dyntype);
typedef void ase_jni_a2j_func(JNIEnv *env, ase_jni_script_engine *sc,
  const ase_variant& val, jvalue& jval_r, bool& is_localref_r);
typedef ase_variant ase_jni_j2a_boxed_func(JNIEnv *env,
  ase_jni_script_engine *sc, jobject obj, ase_jni_classinfo *ci);
typedef jobject ase_jni_a2j_boxed_func(JNIEnv *env, ase_jni_script_engine *sc,
  const ase_variant& val, bool& is_localref_r);
typedef void ase_jni_invoke_func(JNIEnv *env, jobject obj, jmethodID,
  const jvalue *args, jvalue& jval_r);
typedef void ase_jni_getfld_func(JNIEnv *env, jobject obj, jfieldID,
  jvalue& jval_r);
typedef void ase_jni_setfld_func(JNIEnv *env, jobject obj, jfieldID,
  const jvalue& val);
typedef jarray ase_jni_createarr_func(JNIEnv *env, jclass kl, jsize len);
typedef void ase_jni_getelem_func(JNIEnv *env, jarray arr, jsize pos,
  jvalue& elem_r);
typedef void ase_jni_setelem_func(JNIEnv *env, jarray arr, jsize pos,
  const jvalue& elem);

struct ase_jni_typeinfo {
  ase_jni_jvalue_type valtype;
  ase_vtype asevtype;
  ase_jni_j2a_func *to_ase;
  ase_jni_a2j_func *to_java;
  ase_jni_j2a_boxed_func *to_ase_boxed;
  ase_jni_a2j_boxed_func *to_java_boxed;
  ase_jni_invoke_func *invoke;
  ase_jni_invoke_func *invokestatic;
  ase_jni_getfld_func *getfld;
  ase_jni_getfld_func *getfldstatic;
  ase_jni_setfld_func *setfld;
  ase_jni_setfld_func *setfldstatic;
  ase_jni_createarr_func *create_arr;
  ase_jni_getelem_func *get_elem;
  ase_jni_setelem_func *set_elem;
  bool is_objtype;
};

extern const ase_jni_typeinfo ase_jni_typeinfo_void;
extern const ase_jni_typeinfo ase_jni_typeinfo_boolean;
extern const ase_jni_typeinfo ase_jni_typeinfo_byte;
extern const ase_jni_typeinfo ase_jni_typeinfo_char;
extern const ase_jni_typeinfo ase_jni_typeinfo_short;
extern const ase_jni_typeinfo ase_jni_typeinfo_int;
extern const ase_jni_typeinfo ase_jni_typeinfo_long;
extern const ase_jni_typeinfo ase_jni_typeinfo_float;
extern const ase_jni_typeinfo ase_jni_typeinfo_double;
extern const ase_jni_typeinfo ase_jni_typeinfo_object;
extern const ase_jni_typeinfo ase_jni_typeinfo_string;

struct ase_jni_funcinfo;
struct ase_jni_fieldinfo;
struct ase_jni_propinfo;
struct ase_jni_classinfo;
struct ase_jni_classmap;

struct ase_jni_funcinfo {

  friend struct ase_jni_propinfo;
  friend struct ase_jni_classinfo;
  friend struct ase_jni_classmap;

  typedef std::vector<ase_jni_classinfo *> argtypes_type;

  ase_jni_funcinfo(JNIEnv *env, ase_jni_script_engine *sc,
    const util::localref<jobject>& mtd, bool iscnstr);

  util::globalref<jobject> method; /* Constructor or Method */
  jmethodID mid; /* become non-null when resolved */
  bool is_constr;
  ase_jni_classinfo *rettype; /* set when resolved */
  argtypes_type argtypes; /* set when resolved */

 private:

  void resolve_nolock(JNIEnv *env, ase_jni_script_engine *sc,
    ase_jni_classmap *cm);

};

struct ase_jni_fieldinfo {

  friend struct ase_jni_propinfo;
  friend struct ase_jni_classinfo;
  friend struct ase_jni_classmap;

  ase_jni_fieldinfo(JNIEnv *env, ase_jni_script_engine *sc,
    const util::localref<jobject>& fld);

  util::globalref<jobject> field; /* Field */
  jfieldID fid; /* become non-null when resolved */
  ase_jni_classinfo *fldtype; /* set when resolved */

 private:

  void resolve_nolock(JNIEnv *env, ase_jni_script_engine *sc,
    ase_jni_classmap *cm);

};

struct ase_jni_propinfo {

  friend struct ase_jni_classinfo;
  friend struct ase_jni_classmap;

  typedef util::ptrvector<ase_jni_funcinfo *> methods_type;

  ase_jni_propinfo(const ase_string& nm)
    : name(nm), resolved(false), is_classmethod(false),
      is_builtinmethod(false), class_or_builtin_method_id(-1) { }
  void register_method(JNIEnv *env, ase_jni_script_engine *sc,
    const util::localref<jobject>& mtd);
  void register_field(JNIEnv *env, ase_jni_script_engine *sc,
    const util::localref<jobject>& fld);
  void register_as_classmethod(ase_int id);
  void register_as_builtin(ase_int id);

  ase_string name;
  bool resolved : 1;
  bool is_classmethod : 1;
  bool is_builtinmethod : 1;
  methods_type methods;
  std::auto_ptr<ase_jni_fieldinfo> field;
  ase_int class_or_builtin_method_id;

 private:

  void resolve_nolock(JNIEnv *env, ase_jni_script_engine *sc,
    ase_jni_classmap *cm);

};

struct less_method_entry {
  bool operator ()(const ase_string& x, const ase_string& y) const {
    return ase_variant::compare_method_entry(x.data(), x.size(), y.data(),
      y.size()) < 0;
  }
};

struct ase_jni_classinfo {

  friend struct ase_jni_classmap;

  typedef util::ptrvector<ase_jni_funcinfo *> constructors_type;
  typedef util::ptrmap<ase_string, ase_jni_propinfo *, less_method_entry>
    props_type;
  typedef std::vector<ase_jni_propinfo *> proparr_type;

  ase_jni_classinfo(const ase_string& nm, const ase_jni_typeinfo *ti);
    /* for primitive types */
  ase_jni_classinfo(JNIEnv *env, ase_jni_script_engine *sc,
    ase_jni_classmap *cm, const ase_string& nm, const ase_jni_typeinfo *ti,
    const util::localref<jclass>& kl);
    /* for object types */
  props_type::iterator register_prop(ase_jni_script_engine *sc,
    const ase_string& name, bool is_static);

  ase_string name;
  const ase_jni_typeinfo *const tinfo;
  const ase_jni_typeinfo *const tinfo_unbox; /* Boolean -> boolean etc. */
  util::globalref<jclass> klass;
  constructors_type constructors;
  bool constr_resolved : 1;
  bool is_interface : 1;
  bool is_reflectproxy : 1;   /* java.lang.reflect.Proxy */
  bool is_javalangobject : 1; /* java.lang.Object */
  bool is_javalangclass : 1;  /* java.lang.Class */
  bool is_latebind : 1;       /* asejni.LateBinding */
  bool can_hold_string : 1;   /* superclass of string or latebind */
  props_type props, static_props;
  proparr_type proparr, static_proparr;
  ase_variant_vtable vtbl, static_vtbl;
  std::vector<ase_variant_method_entry> vtbl_methods, static_vtbl_methods;
  ase_jni_classinfo *component_kinfo;

 private:

  void resolve_constr_nolock(JNIEnv *env, ase_jni_script_engine *sc,
    ase_jni_classmap *cm);

};

struct ase_jni_classmap {

  friend struct ase_jni_funcinfo;
  friend struct ase_jni_fieldinfo;
  friend struct ase_jni_classinfo;

  void register_primitive_classinfo(JNIEnv *env, ase_jni_script_engine *sc,
    const std::string& name, const ase_jni_typeinfo *ti, jclass kl);
  ase_jni_classinfo *find_type_synchronized(JNIEnv *env,
    ase_jni_script_engine *sc, const util::localref<jclass>& kl);
  void resolve_funcinfo_synchronized(ase_jni_funcinfo& fi,
    JNIEnv *env, ase_jni_script_engine *sc);
  void resolve_field_synchronized(ase_jni_fieldinfo& fi, JNIEnv *env,
    ase_jni_script_engine *sc);
  void resolve_propinfo_synchronized(ase_jni_propinfo& pi, JNIEnv *env,
    ase_jni_script_engine *sc);
  void resolve_constr_synchronized(ase_jni_classinfo& ci, JNIEnv *env,
    ase_jni_script_engine *sc);

 private:

  ase_jni_classinfo *find_type_internal_nolock(JNIEnv *env,
    ase_jni_script_engine *sc, const util::localref<jclass>& kl);

 private:

  typedef util::ptrmap<ase_string, ase_jni_classinfo *> map_type;
  typedef ase_recursive_mutex mutex_type;

  mutex_type mutex;
  map_type classmap;

};

}; // namespace asejni

#endif

