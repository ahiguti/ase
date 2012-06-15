
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEJNI_JOBJPR_HPP
#define GENTYPE_ASEJNI_JOBJPR_HPP

#include "asejni_sctx.hpp"

namespace asejni {

struct ase_jni_variant_impl_jobject {

  ase_jni_variant_impl_jobject(JNIEnv *env, jobject obj,
    ase_jni_classinfo *ci);

  static bool is_jobject_or_clobj(ase_jni_script_engine *sc,
    const ase_variant& v) {
    return (v.get_impl_priv() == sc->GetImplPrivForJObject());
  }
  static ase_jni_variant_impl_jobject *get(const ase_variant& v) {
    return static_cast<ase_jni_variant_impl_jobject *>(v.get_rep().p);
  }
  static ase_jni_script_engine *getContext(const ase_variant& v);

  static void add_ref(const ase_variant& v);
  static void release(const ase_variant& v);
  static ase_int get_attributes(const ase_variant& v);
  static bool get_boolean(const ase_variant& v);
  static ase_int get_int(const ase_variant& v);
  static ase_long get_long(const ase_variant& v);
  static double get_double(const ase_variant& v);
  static ase_string get_string(const ase_variant& v);
  static bool is_same_object(const ase_variant& v, const ase_variant& val);
  static ase_variant get_property(const ase_variant& v, const char *name,
    ase_size_type namelen);
  static void set_property(const ase_variant& v, const char *name,
    ase_size_type namelen, const ase_variant& value);
  static void del_property(const ase_variant& v, const char *name,
    ase_size_type namelen);
  static ase_variant get_element(const ase_variant& v, ase_int idx);
  static void set_element(const ase_variant& v, ase_int idx,
    const ase_variant& value);
  static ase_int get_length(const ase_variant& v);
  static void set_length(const ase_variant& v, ase_int len);
  static ase_variant invoke_by_name(const ase_variant& v, const char *name,
    ase_size_type namelen, const ase_variant *args, ase_size_type nargs);
  static ase_variant invoke_by_id(const ase_variant& self, ase_int id,
    const ase_variant *args, ase_size_type nargs);
  static ase_variant invoke_self(const ase_variant& v, const ase_variant *args,
    ase_size_type nargs);
  static ase_variant InvokeSelfInternal(ase_jni_script_engine *sc,
    ase_jni_variant_impl_jobject *jv, ase_jni_classinfo& ci,
    const ase_variant *args, ase_size_type nargs);
  static ase_variant get_enumerator(const ase_variant& v);
  static ase_string enum_next(const ase_variant& v, bool& hasnext_r);

  jobject get_object() const { return object; }
  jobject get_object_or_clobj() const {
    return object != 0 ? object : classinfo->klass.get();
  }

 private:

  mutable ase_atomic_count refcount;
  jobject object; /* globalref */
  ase_jni_classinfo *classinfo;

 private:

  static ase_variant create_array(const ase_variant& v,
    const ase_variant *args, ase_size_type nargs);
  static ase_variant create_array_multi(const ase_variant& v,
    const ase_variant *args, ase_size_type nargs);
  static ase_variant create_array_value(const ase_variant& v,
    const ase_variant *args, ase_size_type nargs);
  static ase_variant create_proxy(const ase_variant& v,
    const ase_variant *args, ase_size_type nargs);

  /* handlers for reflect.Proxy */
  static ase_variant getProxyWrapped(const ase_variant& v);
  static ase_int proxyGetAttributes(const ase_variant& v);
  static bool proxyGetBoolean(const ase_variant& v);
  static ase_int proxyGetInt(const ase_variant& v);
  static ase_long proxyGetLong(const ase_variant& v);
  static double proxyGetDouble(const ase_variant& v);
  static ase_string proxyGetString(const ase_variant& v);
  static ase_variant proxyGetProperty(const ase_variant& v,
    const char *name, ase_size_type namelen);
  static void proxySetProperty(const ase_variant& v, const char *name,
    ase_size_type namelen, const ase_variant& value);
  static void proxyDelProperty(const ase_variant& v, const char *name,
    ase_size_type namelen);
  static ase_variant proxyGetElement(const ase_variant& v, ase_int idx);
  static void proxySetElement(const ase_variant& v, ase_int idx,
    const ase_variant& value);
  static ase_int proxyGetLength(const ase_variant& v);
  static void proxySetLength(const ase_variant& v, ase_int len);
  static ase_variant proxyInvokeByName(const ase_variant& v,
    const char *name, ase_size_type namelen, const ase_variant *args,
    ase_size_type nargs);
  static ase_variant proxyInvokeById(const ase_variant& self,
    ase_int id, const ase_variant *args, ase_size_type nargs);
  static ase_variant proxyInvokeSelf(const ase_variant& v,
    const ase_variant *args, ase_size_type nargs);
  static ase_variant proxyGetEnumerator(const ase_variant& v);
  static ase_string proxyEnumNext(const ase_variant& v, bool& hasnext_r);

  /* handlers for LateBinding */
  static ase_int lateGetAttributes(const ase_variant& v);
  static bool lateGetBoolean(const ase_variant& v);
  static ase_int lateGetInt(const ase_variant& v);
  static ase_long lateGetLong(const ase_variant& v);
  static double lateGetDouble(const ase_variant& v);
  static ase_string lateGetString(const ase_variant& v);
  static ase_variant lateGetProperty(const ase_variant& v,
    const char *name, ase_size_type namelen);
  static void lateSetProperty(const ase_variant& v, const char *name,
    ase_size_type namelen, const ase_variant& value);
  static void lateDelProperty(const ase_variant& v, const char *name,
    ase_size_type namelen);
  static ase_variant lateGetElement(const ase_variant& v, ase_int idx);
  static void lateSetElement(const ase_variant& v, ase_int idx,
    const ase_variant& value);
  static ase_int lateGetLength(const ase_variant& v);
  static void lateSetLength(const ase_variant& v, ase_int len);
  static ase_variant lateInvokeByName(const ase_variant& v,
    const char *name, ase_size_type namelen, const ase_variant *args,
    ase_size_type nargs);
  static ase_variant lateInvokeById(const ase_variant& self,
    ase_int id, const ase_variant *args, ase_size_type nargs);
  static ase_variant lateInvokeSelf(const ase_variant& v,
    const ase_variant *args, ase_size_type nargs);
  static ase_variant lateGetEnumerator(const ase_variant& v);
  static ase_string lateEnumNext(const ase_variant& v, bool& hasnext_r);

  /* noncopyable */
  ase_jni_variant_impl_jobject(const ase_jni_variant_impl_jobject& x);
  ase_jni_variant_impl_jobject& operator =(const
    ase_jni_variant_impl_jobject& x);

};

ase_variant ase_jni_jobject_new(JNIEnv *env, ase_jni_script_engine *sc,
  ase_jni_classinfo *ci /* nullable */, jobject obj /* nullable */,
  bool examine_dyntype);
jobject ase_jni_jniproxy_new(JNIEnv *env, ase_jni_script_engine *sc,
  ase_jni_classinfo& ci, const ase_variant& value); /* returns a localref */
ase_variant ase_jni_jniproxy_unwrap_latebinding(JNIEnv *env,
  ase_jni_script_engine *sc, jobject obj);
  /* obj must be of type NativeProxy */

}; // namespace asejni

#endif

