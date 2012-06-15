
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asecast.hpp>

#include "asejni_classinfo.hpp"
#include "asejni_jobjpr.hpp"

#include <algorithm>

#define DBG_RESOLVE(x)
#define DBG_RESOLVE_FI(x)
#define DBG_LOCALREF(x)
#define DBG_COMPARE(x)
#define DBG_ARR(x)
#define DBG_BOXED(x)
#define DBG_PROXY(x)
#define DBG_STR(x)
#define DBG_TOJ(x)
#define DBG_CLASS(x)
#define DBG_FINDTYPE(x)
#define DBG_CLSMETA(x)
#define DBG_VT(x)

namespace asejni {

ase_variant
ase_jni_j2a_string_internal(JNIEnv *env, ase_jni_script_engine *sc,
  jobject obj, ase_jni_classinfo *ci)
{
  jstring v = static_cast<jstring>(obj);
  return util::to_ase_string(env, sc, v).to_variant();
}

jobject
ase_jni_a2j_string_internal(JNIEnv *env, ase_jni_script_engine *sc,
  const ase_variant& val, bool& is_localref_r)
{
  ase_string s = val.get_string();
  is_localref_r = true;
  jobject obj = util::to_java_string(env, s); /* localref */
  DBG_LOCALREF(fprintf(stderr, "A2J string localref %p\n", obj));
  return obj;
}

ase_variant
ase_jni_j2a_object_internal(JNIEnv *env, ase_jni_script_engine *sc,
  jobject obj, ase_jni_classinfo *ci, bool examine_dyntype)
{
  if (ci->is_latebind && env->IsInstanceOf(obj, sc->getJNPRXKlass())) {
    /* wrapped ase object */
    return ase_jni_jniproxy_unwrap_latebinding(env, sc, obj);
  }
  if (obj == 0) {
    return ase_variant();
  }
  if (ci->can_hold_string && env->IsInstanceOf(obj, sc->getStringKlass())) {
    /* convert to ase string */
    DBG_STR(fprintf(stderr, "j2a_object_internal: string\n"));
    return util::to_ase_string(env, sc, static_cast<jstring>(obj))
      .to_variant();
  }
  /* not a ase object. create a java proxy */
  DBG_STR(fprintf(stderr, "j2a_object_internal: java proxy\n"));
  return ase_jni_jobject_new(env, sc, ci, obj, examine_dyntype);
}

jobject
ase_jni_a2j_object_internal(JNIEnv *env, ase_jni_script_engine *sc,
  const ase_variant& val, bool& is_localref_r)
{
  if (ase_jni_variant_impl_jobject::is_jobject_or_clobj(sc, val)) {
    /* wrapped java object */
    ase_jni_variant_impl_jobject *jo = ase_jni_variant_impl_jobject::get(val);
    jobject obj = jo->get_object_or_clobj();
    DBG_TOJ(fprintf(stderr, "a2j_object_internal: wrapped java object %p\n",
      obj));
    return obj; /* not a localref */
  }
  ase_vtype vt = val.get_type();
  if (vt == ase_vtype_string) {
    /* convert to java string */
    DBG_TOJ(fprintf(stderr, "a2j_object_internal: string\n"));
    return ase_jni_a2j_string_internal(env, sc, val, is_localref_r);
  }
  if (vt != ase_vtype_object) {
    DBG_TOJ(fprintf(stderr, "a2j_object_internal: zero\n"));
    if (vt == ase_vtype_void) {
      return 0;
    }
    ase_throw_type_mismatch("JNI: failed to convert to a java object");
  }
  /* not a java object. create an ase proxy */
  is_localref_r = true; /* created jobject is a localref */
  ase_jni_classinfo *ci = sc->get_jlobject_classinfo(); /* java.lang.Object */
  DBG_TOJ(fprintf(stderr, "a2j_object_internal: ase proxy\n"));
  return ase_jni_jniproxy_new(env, sc, *ci, val);
}

ase_variant
ase_jni_j2a_void(JNIEnv *env, ase_jni_script_engine *sc,
  const jvalue& jval, ase_jni_classinfo *ci, bool examine_dyntype)
{
  return ase_variant();
}

ase_variant
ase_jni_j2a_boolean(JNIEnv *env, ase_jni_script_engine *sc,
  const jvalue& jval, ase_jni_classinfo *ci, bool examine_dyntype)
{
  jboolean v = jval.z;
  return ase_variant::create_boolean(v != 0);
}

ase_variant
ase_jni_j2a_byte(JNIEnv *env, ase_jni_script_engine *sc,
  const jvalue& jval, ase_jni_classinfo *ci, bool examine_dyntype)
{
  jbyte v = jval.b;
  return ase_variant::create_int(v);
}

ase_variant
ase_jni_j2a_char(JNIEnv *env, ase_jni_script_engine *sc,
  const jvalue& jval, ase_jni_classinfo *ci, bool examine_dyntype)
{
  jchar v = jval.c;
  return ase_variant::create_int(v);
}

ase_variant
ase_jni_j2a_short(JNIEnv *env, ase_jni_script_engine *sc,
  const jvalue& jval, ase_jni_classinfo *ci, bool examine_dyntype)
{
  jshort v = jval.s;
  return ase_variant::create_int(v);
}

ase_variant
ase_jni_j2a_int(JNIEnv *env, ase_jni_script_engine *sc,
  const jvalue& jval, ase_jni_classinfo *ci, bool examine_dyntype)
{
  jint v = jval.i;
  return ase_variant::create_int(v);
}

ase_variant
ase_jni_j2a_long(JNIEnv *env, ase_jni_script_engine *sc,
  const jvalue& jval, ase_jni_classinfo *ci, bool examine_dyntype)
{
  jlong v = jval.j;
  return ase_variant::create_long(v);
}

ase_variant
ase_jni_j2a_float(JNIEnv *env, ase_jni_script_engine *sc,
  const jvalue& jval, ase_jni_classinfo *ci, bool examine_dyntype)
{
  jfloat v = jval.f;
  return ase_variant::create_double(v);
}

ase_variant
ase_jni_j2a_double(JNIEnv *env, ase_jni_script_engine *sc,
  const jvalue& jval, ase_jni_classinfo *ci, bool examine_dyntype)
{
  jdouble v = jval.d;
  return ase_variant::create_double(v);
}

ase_variant
ase_jni_j2a_string(JNIEnv *env, ase_jni_script_engine *sc,
  const jvalue& jval, ase_jni_classinfo *ci, bool examine_dyntype)
{
  return ase_jni_j2a_string_internal(env, sc, jval.l, ci);
}

ase_variant
ase_jni_j2a_object(JNIEnv *env, ase_jni_script_engine *sc,
  const jvalue& jval, ase_jni_classinfo *ci, bool examine_dyntype)
{
  return ase_jni_j2a_object_internal(env, sc, jval.l, ci, examine_dyntype);
}

void
ase_jni_a2j_void(JNIEnv *env, ase_jni_script_engine *sc,
  const ase_variant& val, jvalue& jval_r, bool& is_localref_r)
{
}

void
ase_jni_a2j_boolean(JNIEnv *env, ase_jni_script_engine *sc,
  const ase_variant& val, jvalue& jval_r, bool& is_localref_r)
{
  jval_r.z = val.get_boolean();
}

void
ase_jni_a2j_byte(JNIEnv *env, ase_jni_script_engine *sc,
  const ase_variant& val, jvalue& jval_r, bool& is_localref_r)
{
  jval_r.b = val.get_int();
}

void
ase_jni_a2j_char(JNIEnv *env, ase_jni_script_engine *sc,
  const ase_variant& val, jvalue& jval_r, bool& is_localref_r)
{
  jval_r.c = val.get_int();
}

void
ase_jni_a2j_short(JNIEnv *env, ase_jni_script_engine *sc,
  const ase_variant& val, jvalue& jval_r, bool& is_localref_r)
{
  jval_r.s = val.get_int();
}

void
ase_jni_a2j_int(JNIEnv *env, ase_jni_script_engine *sc,
  const ase_variant& val, jvalue& jval_r, bool& is_localref_r)
{
  jval_r.i = val.get_int();
}

void
ase_jni_a2j_long(JNIEnv *env, ase_jni_script_engine *sc,
  const ase_variant& val, jvalue& jval_r, bool& is_localref_r)
{
  jval_r.j = val.get_long();
}

void
ase_jni_a2j_float(JNIEnv *env, ase_jni_script_engine *sc,
  const ase_variant& val, jvalue& jval_r, bool& is_localref_r)
{
  jval_r.f = static_cast<jfloat>(val.get_double());
}

void
ase_jni_a2j_double(JNIEnv *env, ase_jni_script_engine *sc,
  const ase_variant& val, jvalue& jval_r, bool& is_localref_r)
{
  jval_r.d = val.get_double();
}

void
ase_jni_a2j_string(JNIEnv *env, ase_jni_script_engine *sc,
  const ase_variant& val, jvalue& jval_r, bool& is_localref_r)
{
  jval_r.l = ase_jni_a2j_string_internal(env, sc, val, is_localref_r);
}

void
ase_jni_a2j_object(JNIEnv *env, ase_jni_script_engine *sc,
  const ase_variant& val, jvalue& jval_r, bool& is_localref_r)
{
  jval_r.l = ase_jni_a2j_object_internal(env, sc, val, is_localref_r);
}

ase_variant
ase_jni_j2a_boxed_void(JNIEnv *env, ase_jni_script_engine *sc, jobject obj,
  ase_jni_classinfo *ci)
{
  DBG_BOXED(fprintf(stderr, "j2ab void\n"));
  return ase_variant();
}

ase_variant
ase_jni_j2a_boxed_boolean(JNIEnv *env, ase_jni_script_engine *sc,
  jobject obj, ase_jni_classinfo *ci)
{
  jboolean v = env->CallBooleanMethod(obj, sc->getMIDBooleanValue());
  DBG_BOXED(util::checkJavaException(env, sc, "j2a_boxed_boolean"));
  DBG_BOXED(fprintf(stderr, "j2ab boolean %d\n", (int)v));
  return ase_variant::create_boolean(v != 0);
}

ase_variant
ase_jni_j2a_boxed_byte(JNIEnv *env, ase_jni_script_engine *sc, jobject obj,
  ase_jni_classinfo *ci)
{
  jbyte v = env->CallByteMethod(obj, sc->getMIDByteValue());
  DBG_BOXED(util::checkJavaException(env, sc, "j2a_boxed_byte"));
  DBG_BOXED(fprintf(stderr, "j2ab byte %d\n", (int)v));
  return ase_variant::create_int(v);
}

ase_variant
ase_jni_j2a_boxed_char(JNIEnv *env, ase_jni_script_engine *sc, jobject obj,
  ase_jni_classinfo *ci)
{
  jchar v = env->CallCharMethod(obj, sc->getMIDCharValue());
  DBG_BOXED(util::checkJavaException(env, sc, "j2a_boxed_char"));
  DBG_BOXED(fprintf(stderr, "j2ab char %d\n", (int)v));
  return ase_variant::create_int(v);
}

ase_variant
ase_jni_j2a_boxed_short(JNIEnv *env, ase_jni_script_engine *sc, jobject obj,
  ase_jni_classinfo *ci)
{
  jshort v = env->CallShortMethod(obj, sc->getMIDShortValue());
  DBG_BOXED(util::checkJavaException(env, sc, "j2a_boxed_short"));
  DBG_BOXED(fprintf(stderr, "j2ab short %hd\n", (short)v));
  return ase_variant::create_int(v);
}

ase_variant
ase_jni_j2a_boxed_int(JNIEnv *env, ase_jni_script_engine *sc, jobject obj,
  ase_jni_classinfo *ci)
{
  jint v = env->CallIntMethod(obj, sc->getMIDIntValue());
  DBG_BOXED(util::checkJavaException(env, sc, "j2a_boxed_int"));
  DBG_BOXED(fprintf(stderr, "j2ab int %d\n", v));
  return ase_variant::create_int(v);
}

ase_variant
ase_jni_j2a_boxed_long(JNIEnv *env, ase_jni_script_engine *sc, jobject obj,
  ase_jni_classinfo *ci)
{
  jlong v = env->CallLongMethod(obj, sc->getMIDLongValue());
  DBG_BOXED(util::checkJavaException(env, sc, "j2a_boxed_long"));
  DBG_BOXED(fprintf(stderr, "j2ab long %lld\n", (long long)v));
  return ase_variant::create_long(v);
}

ase_variant
ase_jni_j2a_boxed_float(JNIEnv *env, ase_jni_script_engine *sc, jobject obj,
  ase_jni_classinfo *ci)
{
  jfloat v = env->CallFloatMethod(obj, sc->getMIDFloatValue());
  DBG_BOXED(util::checkJavaException(env, sc, "j2a_boxed_float"));
  DBG_BOXED(fprintf(stderr, "j2ab float %f\n", v));
  return ase_variant::create_double(v);
}

ase_variant
ase_jni_j2a_boxed_double(JNIEnv *env, ase_jni_script_engine *sc, jobject obj,
  ase_jni_classinfo *ci)
{
  jdouble v = env->CallDoubleMethod(obj, sc->getMIDDoubleValue());
  DBG_BOXED(util::checkJavaException(env, sc, "j2a_boxed_double"));
  DBG_BOXED(fprintf(stderr, "j2ab double %lf\n", v));
  return ase_variant::create_double(v);
}

ase_variant
ase_jni_j2a_boxed_string(JNIEnv *env, ase_jni_script_engine *sc, jobject obj,
  ase_jni_classinfo *ci)
{
  return ase_jni_j2a_string_internal(env, sc, obj, ci);
}

ase_variant
ase_jni_j2a_boxed_object(JNIEnv *env, ase_jni_script_engine *sc, jobject obj,
  ase_jni_classinfo *ci)
{
  const bool examine_dyntype = true;
  return ase_jni_j2a_object_internal(env, sc, obj, ci, examine_dyntype);
}

jobject
ase_jni_a2j_boxed_void(JNIEnv *env, ase_jni_script_engine *sc,
  const ase_variant& val, bool& is_localref_r)
{
  return 0;
}

jobject
ase_jni_a2j_boxed_boolean(JNIEnv *env, ase_jni_script_engine *sc,
  const ase_variant& val, bool& is_localref_r)
{
  bool v = val.get_boolean();
  jvalue args[1];
  args[0].z = v;
  return env->CallStaticObjectMethodA(sc->getBoolKlass(),
    sc->getMIDBValueOf(), args);
}

jobject
ase_jni_a2j_boxed_byte(JNIEnv *env, ase_jni_script_engine *sc,
  const ase_variant& val, bool& is_localref_r)
{
  ase_int v = val.get_int();
  jvalue args[1];
  args[0].b = v;
  return env->NewObjectA(sc->getByteKlass(), sc->getMIDByteConstr(), args);
}

jobject
ase_jni_a2j_boxed_char(JNIEnv *env, ase_jni_script_engine *sc,
  const ase_variant& val, bool& is_localref_r)
{
  ase_int v = val.get_int();
  jvalue args[1];
  args[0].c = v;
  return env->NewObjectA(sc->getCharKlass(), sc->getMIDCharConstr(), args);
}

jobject
ase_jni_a2j_boxed_short(JNIEnv *env, ase_jni_script_engine *sc,
  const ase_variant& val, bool& is_localref_r)
{
  ase_int v = val.get_int();
  jvalue args[1];
  args[0].s = v;
  return env->NewObjectA(sc->getShortKlass(), sc->getMIDShortConstr(), args);
}

jobject
ase_jni_a2j_boxed_int(JNIEnv *env, ase_jni_script_engine *sc,
  const ase_variant& val, bool& is_localref_r)
{
  ase_int v = val.get_int();
  jvalue args[1];
  args[0].i = v;
  return env->NewObjectA(sc->getIntKlass(), sc->getMIDIntConstr(), args);
}

jobject
ase_jni_a2j_boxed_long(JNIEnv *env, ase_jni_script_engine *sc,
  const ase_variant& val, bool& is_localref_r)
{
  ase_long v = val.get_long();
  jvalue args[1];
  args[0].j = v;
  return env->NewObjectA(sc->getLongKlass(), sc->getMIDLongConstr(), args);
}

jobject
ase_jni_a2j_boxed_float(JNIEnv *env, ase_jni_script_engine *sc,
  const ase_variant& val, bool& is_localref_r)
{
  double v = val.get_double();
  jvalue args[1];
  args[0].f = static_cast<jfloat>(v);
  return env->NewObjectA(sc->getFloatKlass(), sc->getMIDFloatConstr(), args);
}

jobject
ase_jni_a2j_boxed_double(JNIEnv *env, ase_jni_script_engine *sc,
  const ase_variant& val, bool& is_localref_r)
{
  double v = val.get_double();
  jvalue args[1];
  args[0].d = v;
  return env->NewObjectA(sc->getDoubleKlass(), sc->getMIDDoubleConstr(),
    args);
}

jobject
ase_jni_a2j_boxed_string(JNIEnv *env, ase_jni_script_engine *sc,
  const ase_variant& val, bool& is_localref_r)
{
  return ase_jni_a2j_string_internal(env, sc, val, is_localref_r);
}

jobject
ase_jni_a2j_boxed_object(JNIEnv *env, ase_jni_script_engine *sc,
  const ase_variant& val, bool& is_localref_r)
{
  return ase_jni_a2j_object_internal(env, sc, val, is_localref_r);
}

void
ase_jni_invoke_void(JNIEnv *env, jobject obj, jmethodID mid,
  const jvalue *args, jvalue& jval_r)
{
  env->CallVoidMethodA(obj, mid, args);
}

void
ase_jni_invoke_boolean(JNIEnv *env, jobject obj, jmethodID mid,
  const jvalue *args, jvalue& jval_r)
{
  jval_r.z = env->CallBooleanMethodA(obj, mid, args);
}

void
ase_jni_invoke_byte(JNIEnv *env, jobject obj, jmethodID mid,
  const jvalue *args, jvalue& jval_r)
{
  jval_r.b = env->CallByteMethodA(obj, mid, args);
}

void
ase_jni_invoke_char(JNIEnv *env, jobject obj, jmethodID mid,
  const jvalue *args, jvalue& jval_r)
{
  jval_r.c = env->CallCharMethodA(obj, mid, args);
}

void
ase_jni_invoke_short(JNIEnv *env, jobject obj, jmethodID mid,
  const jvalue *args, jvalue& jval_r)
{
  jval_r.s = env->CallShortMethodA(obj, mid, args);
}

void
ase_jni_invoke_int(JNIEnv *env, jobject obj, jmethodID mid,
  const jvalue *args, jvalue& jval_r)
{
  jval_r.i = env->CallIntMethodA(obj, mid, args);
}

void
ase_jni_invoke_long(JNIEnv *env, jobject obj, jmethodID mid,
  const jvalue *args, jvalue& jval_r)
{
  jval_r.j = env->CallLongMethodA(obj, mid, args);
}

void
ase_jni_invoke_float(JNIEnv *env, jobject obj, jmethodID mid,
  const jvalue *args, jvalue& jval_r)
{
  jval_r.f = env->CallFloatMethodA(obj, mid, args);
}

void
ase_jni_invoke_double(JNIEnv *env, jobject obj, jmethodID mid,
  const jvalue *args, jvalue& jval_r)
{
  jval_r.d = env->CallDoubleMethodA(obj, mid, args);
}

void
ase_jni_invoke_object(JNIEnv *env, jobject obj, jmethodID mid,
  const jvalue *args, jvalue& jval_r)
{
  jval_r.l = env->CallObjectMethodA(obj, mid, args);
}

void
ase_jni_invokestatic_void(JNIEnv *env, jobject obj, jmethodID mid,
  const jvalue *args, jvalue& jval_r)
{
  env->CallStaticVoidMethodA(static_cast<jclass>(obj), mid, args);
}

void
ase_jni_invokestatic_boolean(JNIEnv *env, jobject obj, jmethodID mid,
  const jvalue *args, jvalue& jval_r)
{
  jval_r.z = env->CallStaticBooleanMethodA(static_cast<jclass>(obj), mid,
    args);
}

void
ase_jni_invokestatic_byte(JNIEnv *env, jobject obj, jmethodID mid,
  const jvalue *args, jvalue& jval_r)
{
  jval_r.b = env->CallStaticByteMethodA(static_cast<jclass>(obj), mid, args);
}

void
ase_jni_invokestatic_char(JNIEnv *env, jobject obj, jmethodID mid,
  const jvalue *args, jvalue& jval_r)
{
  jval_r.c = env->CallStaticCharMethodA(static_cast<jclass>(obj), mid, args);
}

void
ase_jni_invokestatic_short(JNIEnv *env, jobject obj, jmethodID mid,
  const jvalue *args, jvalue& jval_r)
{
  jval_r.s = env->CallStaticShortMethodA(static_cast<jclass>(obj), mid, args);
}

void
ase_jni_invokestatic_int(JNIEnv *env, jobject obj, jmethodID mid,
  const jvalue *args, jvalue& jval_r)
{
  jval_r.i = env->CallStaticIntMethodA(static_cast<jclass>(obj), mid, args);
}

void
ase_jni_invokestatic_long(JNIEnv *env, jobject obj, jmethodID mid,
  const jvalue *args, jvalue& jval_r)
{
  jval_r.j = env->CallStaticLongMethodA(static_cast<jclass>(obj), mid, args);
}

void
ase_jni_invokestatic_float(JNIEnv *env, jobject obj, jmethodID mid,
  const jvalue *args, jvalue& jval_r)
{
  jval_r.f = env->CallStaticFloatMethodA(static_cast<jclass>(obj), mid, args);
}

void
ase_jni_invokestatic_double(JNIEnv *env, jobject obj, jmethodID mid,
  const jvalue *args, jvalue& jval_r)
{
  jval_r.d = env->CallStaticDoubleMethodA(static_cast<jclass>(obj), mid,
    args);
}

void
ase_jni_invokestatic_object(JNIEnv *env, jobject obj, jmethodID mid,
  const jvalue *args, jvalue& jval_r)
{
  jval_r.l = env->CallStaticObjectMethodA(static_cast<jclass>(obj), mid,
    args);
}

void
ase_jni_getfld_void(JNIEnv *env, jobject obj, jfieldID fid, jvalue& jval_r)
{
}

void
ase_jni_getfld_boolean(JNIEnv *env, jobject obj, jfieldID fid, jvalue& jval_r)
{
  jval_r.z = env->GetBooleanField(obj, fid);
}

void
ase_jni_getfld_byte(JNIEnv *env, jobject obj, jfieldID fid, jvalue& jval_r)
{
  jval_r.b = env->GetByteField(obj, fid);
}

void
ase_jni_getfld_char(JNIEnv *env, jobject obj, jfieldID fid, jvalue& jval_r)
{
  jval_r.c = env->GetCharField(obj, fid);
}

void
ase_jni_getfld_short(JNIEnv *env, jobject obj, jfieldID fid, jvalue& jval_r)
{
  jval_r.s = env->GetShortField(obj, fid);
}

void
ase_jni_getfld_int(JNIEnv *env, jobject obj, jfieldID fid, jvalue& jval_r)
{
  jval_r.i = env->GetIntField(obj, fid);
}

void
ase_jni_getfld_long(JNIEnv *env, jobject obj, jfieldID fid, jvalue& jval_r)
{
  jval_r.j = env->GetLongField(obj, fid);
}

void
ase_jni_getfld_float(JNIEnv *env, jobject obj, jfieldID fid, jvalue& jval_r)
{
  jval_r.f = env->GetFloatField(obj, fid);
}

void
ase_jni_getfld_double(JNIEnv *env, jobject obj, jfieldID fid, jvalue& jval_r)
{
  jval_r.d = env->GetDoubleField(obj, fid);
}

void
ase_jni_getfld_object(JNIEnv *env, jobject obj, jfieldID fid, jvalue& jval_r)
{
  jval_r.l = env->GetObjectField(obj, fid);
}

void
ase_jni_getfldstatic_void(JNIEnv *env, jobject obj, jfieldID fid,
  jvalue& jval_r)
{
}

void
ase_jni_getfldstatic_boolean(JNIEnv *env, jobject obj, jfieldID fid,
  jvalue& jval_r)
{
  jval_r.z = env->GetStaticBooleanField(static_cast<jclass>(obj), fid);
}

void
ase_jni_getfldstatic_byte(JNIEnv *env, jobject obj, jfieldID fid,
  jvalue& jval_r)
{
  jval_r.b = env->GetStaticByteField(static_cast<jclass>(obj), fid);
}

void
ase_jni_getfldstatic_char(JNIEnv *env, jobject obj, jfieldID fid,
  jvalue& jval_r)
{
  jval_r.c = env->GetStaticCharField(static_cast<jclass>(obj), fid);
}

void
ase_jni_getfldstatic_short(JNIEnv *env, jobject obj, jfieldID fid,
  jvalue& jval_r)
{
  jval_r.s = env->GetStaticShortField(static_cast<jclass>(obj), fid);
}

void
ase_jni_getfldstatic_int(JNIEnv *env, jobject obj, jfieldID fid,
  jvalue& jval_r)
{
  jval_r.i = env->GetStaticIntField(static_cast<jclass>(obj), fid);
}

void
ase_jni_getfldstatic_long(JNIEnv *env, jobject obj, jfieldID fid,
  jvalue& jval_r)
{
  jval_r.j = env->GetStaticLongField(static_cast<jclass>(obj), fid);
}

void
ase_jni_getfldstatic_float(JNIEnv *env, jobject obj, jfieldID fid,
  jvalue& jval_r)
{
  jval_r.f = env->GetStaticFloatField(static_cast<jclass>(obj), fid);
}

void
ase_jni_getfldstatic_double(JNIEnv *env, jobject obj, jfieldID fid,
  jvalue& jval_r)
{
  jval_r.d = env->GetStaticDoubleField(static_cast<jclass>(obj), fid);
}

void
ase_jni_getfldstatic_object(JNIEnv *env, jobject obj, jfieldID fid,
  jvalue& jval_r)
{
  jval_r.l = env->GetStaticObjectField(static_cast<jclass>(obj), fid);
}

void
ase_jni_setfld_void(JNIEnv *env, jobject obj, jfieldID fid, const jvalue& val)
{
}

void
ase_jni_setfld_boolean(JNIEnv *env, jobject obj, jfieldID fid,
  const jvalue& val)
{
  env->SetBooleanField(obj, fid, val.z);
}

void
ase_jni_setfld_byte(JNIEnv *env, jobject obj, jfieldID fid, const jvalue& val)
{
  env->SetByteField(obj, fid, val.b);
}

void
ase_jni_setfld_char(JNIEnv *env, jobject obj, jfieldID fid, const jvalue& val)
{
  env->SetCharField(obj, fid, val.c);
}

void
ase_jni_setfld_short(JNIEnv *env, jobject obj, jfieldID fid, const jvalue& val)
{
  env->SetShortField(obj, fid, val.s);
}

void
ase_jni_setfld_int(JNIEnv *env, jobject obj, jfieldID fid, const jvalue& val)
{
  env->SetIntField(obj, fid, val.i);
}

void
ase_jni_setfld_long(JNIEnv *env, jobject obj, jfieldID fid, const jvalue& val)
{
  env->SetLongField(obj, fid, val.j);
}

void
ase_jni_setfld_float(JNIEnv *env, jobject obj, jfieldID fid, const jvalue& val)
{
  env->SetFloatField(obj, fid, val.f);
}

void
ase_jni_setfld_double(JNIEnv *env, jobject obj, jfieldID fid,
  const jvalue& val)
{
  env->SetDoubleField(obj, fid, val.d);
}

void
ase_jni_setfld_object(JNIEnv *env, jobject obj, jfieldID fid,
  const jvalue& val)
{
  env->SetObjectField(obj, fid, val.l);
}

void
ase_jni_setfldstatic_void(JNIEnv *env, jobject obj, jfieldID fid,
  const jvalue& val)
{
}

void
ase_jni_setfldstatic_boolean(JNIEnv *env, jobject obj, jfieldID fid,
  const jvalue& val)
{
  env->SetStaticBooleanField(static_cast<jclass>(obj), fid, val.z);
}

void
ase_jni_setfldstatic_byte(JNIEnv *env, jobject obj, jfieldID fid,
  const jvalue& val)
{
  env->SetStaticByteField(static_cast<jclass>(obj), fid, val.b);
}

void
ase_jni_setfldstatic_char(JNIEnv *env, jobject obj, jfieldID fid,
  const jvalue& val)
{
  env->SetStaticCharField(static_cast<jclass>(obj), fid, val.c);
}

void
ase_jni_setfldstatic_short(JNIEnv *env, jobject obj, jfieldID fid,
  const jvalue& val)
{
  env->SetStaticShortField(static_cast<jclass>(obj), fid, val.s);
}

void
ase_jni_setfldstatic_int(JNIEnv *env, jobject obj, jfieldID fid,
  const jvalue& val)
{
  env->SetStaticIntField(static_cast<jclass>(obj), fid, val.i);
}

void
ase_jni_setfldstatic_long(JNIEnv *env, jobject obj, jfieldID fid,
  const jvalue& val)
{
  env->SetStaticLongField(static_cast<jclass>(obj), fid, val.j);
}

void
ase_jni_setfldstatic_float(JNIEnv *env, jobject obj, jfieldID fid,
  const jvalue& val)
{
  env->SetStaticFloatField(static_cast<jclass>(obj), fid, val.f);
}

void
ase_jni_setfldstatic_double(JNIEnv *env, jobject obj, jfieldID fid,
  const jvalue& val)
{
  env->SetStaticDoubleField(static_cast<jclass>(obj), fid, val.d);
}

void
ase_jni_setfldstatic_object(JNIEnv *env, jobject obj, jfieldID fid,
  const jvalue& val)
{
  env->SetStaticObjectField(static_cast<jclass>(obj), fid, val.l);
}

jarray
ase_jni_createarr_void(JNIEnv *env, jclass kl, jsize len)
{
  return 0;
}

jarray
ase_jni_createarr_boolean(JNIEnv *env, jclass kl, jsize len)
{
  return env->NewBooleanArray(len);
}

jarray
ase_jni_createarr_byte(JNIEnv *env, jclass kl, jsize len)
{
  return env->NewByteArray(len);
}

jarray
ase_jni_createarr_char(JNIEnv *env, jclass kl, jsize len)
{
  return env->NewCharArray(len);
}

jarray
ase_jni_createarr_short(JNIEnv *env, jclass kl, jsize len)
{
  return env->NewShortArray(len);
}

jarray
ase_jni_createarr_int(JNIEnv *env, jclass kl, jsize len)
{
  return env->NewIntArray(len);
}

jarray
ase_jni_createarr_long(JNIEnv *env, jclass kl, jsize len)
{
  return env->NewLongArray(len);
}

jarray
ase_jni_createarr_float(JNIEnv *env, jclass kl, jsize len)
{
  return env->NewFloatArray(len);
}

jarray
ase_jni_createarr_double(JNIEnv *env, jclass kl, jsize len)
{
  return env->NewDoubleArray(len);
}

jarray
ase_jni_createarr_object(JNIEnv *env, jclass kl, jsize len)
{
  return env->NewObjectArray(len, kl, 0);
}

void
ase_jni_getelem_void(JNIEnv *env, jarray arr, jsize pos, jvalue& elem_r)
{
}

void
ase_jni_getelem_boolean(JNIEnv *env, jarray arr, jsize pos, jvalue& elem_r)
{
  env->GetBooleanArrayRegion(static_cast<jbooleanArray>(arr), pos, 1,
    &elem_r.z);
}

void
ase_jni_getelem_byte(JNIEnv *env, jarray arr, jsize pos, jvalue& elem_r)
{
  env->GetByteArrayRegion(static_cast<jbyteArray>(arr), pos, 1, &elem_r.b);
}

void
ase_jni_getelem_char(JNIEnv *env, jarray arr, jsize pos, jvalue& elem_r)
{
  env->GetCharArrayRegion(static_cast<jcharArray>(arr), pos, 1, &elem_r.c);
}

void
ase_jni_getelem_short(JNIEnv *env, jarray arr, jsize pos, jvalue& elem_r)
{
  env->GetShortArrayRegion(static_cast<jshortArray>(arr), pos, 1, &elem_r.s);
}

void
ase_jni_getelem_int(JNIEnv *env, jarray arr, jsize pos, jvalue& elem_r)
{
  env->GetIntArrayRegion(static_cast<jintArray>(arr), pos, 1, &elem_r.i);
}

void
ase_jni_getelem_long(JNIEnv *env, jarray arr, jsize pos, jvalue& elem_r)
{
  env->GetLongArrayRegion(static_cast<jlongArray>(arr), pos, 1, &elem_r.j);
}

void
ase_jni_getelem_float(JNIEnv *env, jarray arr, jsize pos, jvalue& elem_r)
{
  env->GetFloatArrayRegion(static_cast<jfloatArray>(arr), pos, 1, &elem_r.f);
}

void
ase_jni_getelem_double(JNIEnv *env, jarray arr, jsize pos, jvalue& elem_r)
{
  env->GetDoubleArrayRegion(static_cast<jdoubleArray>(arr), pos, 1,
    &elem_r.d);
}

void
ase_jni_getelem_object(JNIEnv *env, jarray arr, jsize pos, jvalue& elem_r)
{
  elem_r.l = env->GetObjectArrayElement(static_cast<jobjectArray>(arr), pos);
}

void
ase_jni_setelem_void(JNIEnv *env, jarray arr, jsize pos, const jvalue& elem)
{
}

void
ase_jni_setelem_boolean(JNIEnv *env, jarray arr, jsize pos,
  const jvalue& elem)
{
  env->SetBooleanArrayRegion(static_cast<jbooleanArray>(arr), pos, 1,
    &elem.z);
}

void
ase_jni_setelem_byte(JNIEnv *env, jarray arr, jsize pos, const jvalue& elem)
{
  env->SetByteArrayRegion(static_cast<jbyteArray>(arr), pos, 1, &elem.b);
}

void
ase_jni_setelem_char(JNIEnv *env, jarray arr, jsize pos, const jvalue& elem)
{
  env->SetCharArrayRegion(static_cast<jcharArray>(arr), pos, 1, &elem.c);
}

void
ase_jni_setelem_short(JNIEnv *env, jarray arr, jsize pos, const jvalue& elem)
{
  env->SetShortArrayRegion(static_cast<jshortArray>(arr), pos, 1, &elem.s);
}

void
ase_jni_setelem_int(JNIEnv *env, jarray arr, jsize pos, const jvalue& elem)
{
  env->SetIntArrayRegion(static_cast<jintArray>(arr), pos, 1, &elem.i);
}

void
ase_jni_setelem_long(JNIEnv *env, jarray arr, jsize pos, const jvalue& elem)
{
  env->SetLongArrayRegion(static_cast<jlongArray>(arr), pos, 1, &elem.j);
}

void
ase_jni_setelem_float(JNIEnv *env, jarray arr, jsize pos, const jvalue& elem)
{
  env->SetFloatArrayRegion(static_cast<jfloatArray>(arr), pos, 1, &elem.f);
}

void
ase_jni_setelem_double(JNIEnv *env, jarray arr, jsize pos, const jvalue& elem)
{
  env->SetDoubleArrayRegion(static_cast<jdoubleArray>(arr), pos, 1, &elem.d);
}

void
ase_jni_setelem_object(JNIEnv *env, jarray arr, jsize pos, const jvalue& elem)
{
  env->SetObjectArrayElement(static_cast<jobjectArray>(arr), pos, elem.l);
}

const ase_jni_typeinfo ase_jni_typeinfo_void = {
  ase_jni_jvalue_type_void,
  ase_vtype_void,
  ase_jni_j2a_void,
  ase_jni_a2j_void,
  ase_jni_j2a_boxed_void,
  ase_jni_a2j_boxed_void,
  ase_jni_invoke_void,
  ase_jni_invokestatic_void,
  ase_jni_getfld_void,
  ase_jni_getfldstatic_void,
  ase_jni_setfld_void,
  ase_jni_setfldstatic_void,
  ase_jni_createarr_void,
  ase_jni_getelem_void,
  ase_jni_setelem_void,
  false,
};

const ase_jni_typeinfo ase_jni_typeinfo_boolean = {
  ase_jni_jvalue_type_boolean,
  ase_vtype_bool,
  ase_jni_j2a_boolean,
  ase_jni_a2j_boolean,
  ase_jni_j2a_boxed_boolean,
  ase_jni_a2j_boxed_boolean,
  ase_jni_invoke_boolean,
  ase_jni_invokestatic_boolean,
  ase_jni_getfld_boolean,
  ase_jni_getfldstatic_boolean,
  ase_jni_setfld_boolean,
  ase_jni_setfldstatic_boolean,
  ase_jni_createarr_boolean,
  ase_jni_getelem_boolean,
  ase_jni_setelem_boolean,
  false,
};

const ase_jni_typeinfo ase_jni_typeinfo_byte = {
  ase_jni_jvalue_type_byte,
  ase_vtype_int,
  ase_jni_j2a_byte,
  ase_jni_a2j_byte,
  ase_jni_j2a_boxed_byte,
  ase_jni_a2j_boxed_byte,
  ase_jni_invoke_byte,
  ase_jni_invokestatic_byte,
  ase_jni_getfld_byte,
  ase_jni_getfldstatic_byte,
  ase_jni_setfld_byte,
  ase_jni_setfldstatic_byte,
  ase_jni_createarr_byte,
  ase_jni_getelem_byte,
  ase_jni_setelem_byte,
  false,
};

const ase_jni_typeinfo ase_jni_typeinfo_char = {
  ase_jni_jvalue_type_char,
  ase_vtype_int,
  ase_jni_j2a_char,
  ase_jni_a2j_char,
  ase_jni_j2a_boxed_char,
  ase_jni_a2j_boxed_char,
  ase_jni_invoke_char,
  ase_jni_invokestatic_char,
  ase_jni_getfld_char,
  ase_jni_getfldstatic_char,
  ase_jni_setfld_char,
  ase_jni_setfldstatic_char,
  ase_jni_createarr_char,
  ase_jni_getelem_char,
  ase_jni_setelem_char,
  false,
};

const ase_jni_typeinfo ase_jni_typeinfo_short = {
  ase_jni_jvalue_type_short,
  ase_vtype_int,
  ase_jni_j2a_short,
  ase_jni_a2j_short,
  ase_jni_j2a_boxed_short,
  ase_jni_a2j_boxed_short,
  ase_jni_invoke_short,
  ase_jni_invokestatic_short,
  ase_jni_getfld_short,
  ase_jni_getfldstatic_short,
  ase_jni_setfld_short,
  ase_jni_setfldstatic_short,
  ase_jni_createarr_short,
  ase_jni_getelem_short,
  ase_jni_setelem_short,
  false,
};

const ase_jni_typeinfo ase_jni_typeinfo_int = {
  ase_jni_jvalue_type_int,
  ase_vtype_int,
  ase_jni_j2a_int,
  ase_jni_a2j_int,
  ase_jni_j2a_boxed_int,
  ase_jni_a2j_boxed_int,
  ase_jni_invoke_int,
  ase_jni_invokestatic_int,
  ase_jni_getfld_int,
  ase_jni_getfldstatic_int,
  ase_jni_setfld_int,
  ase_jni_setfldstatic_int,
  ase_jni_createarr_int,
  ase_jni_getelem_int,
  ase_jni_setelem_int,
  false,
};

const ase_jni_typeinfo ase_jni_typeinfo_long = {
  ase_jni_jvalue_type_long,
  ase_vtype_long,
  ase_jni_j2a_long,
  ase_jni_a2j_long,
  ase_jni_j2a_boxed_long,
  ase_jni_a2j_boxed_long,
  ase_jni_invoke_long,
  ase_jni_invokestatic_long,
  ase_jni_getfld_long,
  ase_jni_getfldstatic_long,
  ase_jni_setfld_long,
  ase_jni_setfldstatic_long,
  ase_jni_createarr_long,
  ase_jni_getelem_long,
  ase_jni_setelem_long,
  false,
};

const ase_jni_typeinfo ase_jni_typeinfo_float = {
  ase_jni_jvalue_type_float,
  ase_vtype_double,
  ase_jni_j2a_float,
  ase_jni_a2j_float,
  ase_jni_j2a_boxed_float,
  ase_jni_a2j_boxed_float,
  ase_jni_invoke_float,
  ase_jni_invokestatic_float,
  ase_jni_getfld_float,
  ase_jni_getfldstatic_float,
  ase_jni_setfld_float,
  ase_jni_setfldstatic_float,
  ase_jni_createarr_float,
  ase_jni_getelem_float,
  ase_jni_setelem_float,
  false,
};

const ase_jni_typeinfo ase_jni_typeinfo_double = {
  ase_jni_jvalue_type_double,
  ase_vtype_double,
  ase_jni_j2a_double,
  ase_jni_a2j_double,
  ase_jni_j2a_boxed_double,
  ase_jni_a2j_boxed_double,
  ase_jni_invoke_double,
  ase_jni_invokestatic_double,
  ase_jni_getfld_double,
  ase_jni_getfldstatic_double,
  ase_jni_setfld_double,
  ase_jni_setfldstatic_double,
  ase_jni_createarr_double,
  ase_jni_getelem_double,
  ase_jni_setelem_double,
  false,
};

const ase_jni_typeinfo ase_jni_typeinfo_string = {
  ase_jni_jvalue_type_object,
  ase_vtype_string,
  ase_jni_j2a_string,
  ase_jni_a2j_string,
  ase_jni_j2a_boxed_string,
  ase_jni_a2j_boxed_string,
  ase_jni_invoke_object,
  ase_jni_invokestatic_object,
  ase_jni_getfld_object,
  ase_jni_getfldstatic_object,
  ase_jni_setfld_object,
  ase_jni_setfldstatic_object,
  ase_jni_createarr_object,
  ase_jni_getelem_object,
  ase_jni_setelem_object,
  true,
};

const ase_jni_typeinfo ase_jni_typeinfo_object = {
  ase_jni_jvalue_type_object,
  ase_vtype_object,
  ase_jni_j2a_object,
  ase_jni_a2j_object,
  ase_jni_j2a_boxed_object,
  ase_jni_a2j_boxed_object,
  ase_jni_invoke_object,
  ase_jni_invokestatic_object,
  ase_jni_getfld_object,
  ase_jni_getfldstatic_object,
  ase_jni_setfld_object,
  ase_jni_setfldstatic_object,
  ase_jni_createarr_object,
  ase_jni_getelem_object,
  ase_jni_setelem_object,
  true,
};

const ase_jni_typeinfo *
get_tinfo_unbox(JNIEnv *env, ase_jni_script_engine *sc, jclass kl)
{
  if (env->IsSameObject(kl, sc->getVoidKlass())) {
    return &ase_jni_typeinfo_void;
  } else if (env->IsSameObject(kl, sc->getBoolKlass())) {
    return &ase_jni_typeinfo_boolean;
  } else if (env->IsSameObject(kl, sc->getByteKlass())) {
    return &ase_jni_typeinfo_byte;
  } else if (env->IsSameObject(kl, sc->getCharKlass())) {
    return &ase_jni_typeinfo_char;
  } else if (env->IsSameObject(kl, sc->getShortKlass())) {
    return &ase_jni_typeinfo_short;
  } else if (env->IsSameObject(kl, sc->getIntKlass())) {
    return &ase_jni_typeinfo_int;
  } else if (env->IsSameObject(kl, sc->getLongKlass())) {
    return &ase_jni_typeinfo_long;
  } else if (env->IsSameObject(kl, sc->getFloatKlass())) {
    return &ase_jni_typeinfo_float;
  } else if (env->IsSameObject(kl, sc->getDoubleKlass())) {
    return &ase_jni_typeinfo_double;
  }
  return &ase_jni_typeinfo_object;
}

/* constructor for primitive types */
ase_jni_classinfo::ase_jni_classinfo(const ase_string& nm,
  const ase_jni_typeinfo *ti)
  : name(nm), tinfo(ti), tinfo_unbox(ti),
    constr_resolved(false), is_interface(false),
    is_reflectproxy(false), is_javalangobject(false), is_latebind(false),
    can_hold_string(false), component_kinfo(0)
{
}

/* constructor for non-primitive types */
ase_jni_classinfo::ase_jni_classinfo(JNIEnv *env, ase_jni_script_engine *sc,
  ase_jni_classmap *cm, const ase_string& nm, const ase_jni_typeinfo *ti,
  const util::localref<jclass>& kl)
  : name(nm), tinfo(ti),
    tinfo_unbox(get_tinfo_unbox(env, sc, kl.get())),
    constr_resolved(false),
    is_interface(false), is_reflectproxy(false), is_javalangobject(false),
    is_javalangclass(false), is_latebind(false), can_hold_string(false),
    component_kinfo(0)
{
  DBG_CLASS(fprintf(stderr, "CLASSINFO CLASS %s\n", name.data()));
  klass.set(sc->getJVM(), env, kl);
  /* is_interface? */
  {
    is_interface = env->CallBooleanMethod(klass.get(),
      sc->getMIDIsInterface()) != 0;
    util::checkJavaException(env, sc, "JNI isInterface");
  }
  /* is_reflectproxy? */
  {
    jvalue cargs[1];
    cargs[0].l = klass.get();
    is_reflectproxy = env->CallStaticBooleanMethodA(sc->getRPrxKlass(),
      sc->getMIDIsProxyClass(), cargs) != 0;
    util::checkJavaException(env, sc, "JNI isInterface");
    if (is_reflectproxy) {
      DBG_PROXY(fprintf(stderr, "PROXY CLASS %s\n", name.c_str()));
    }
  }
  /* is_javalanglobject? etc. */
  {
    is_javalangobject = env->IsSameObject(klass.get(), sc->getObjectKlass())
      != 0;
    is_javalangclass = env->IsSameObject(klass.get(), sc->getClassKlass())
      != 0;
    is_latebind = env->IsAssignableFrom(klass.get(), sc->getJLateKlass())
      != 0;
    can_hold_string = is_latebind ||
      (env->IsAssignableFrom(sc->getStringKlass(), klass.get()));
      /* latebind or a superclass of string */
  }
  /* methods */
  {
    /* mtds: java.lang.reflect.Method[] */
    util::localref<jobjectArray> mtds(env, sc, static_cast<jobjectArray>(
      env->CallObjectMethod(klass.get(), sc->getMIDGetMethods())));
    const jsize nmtds = env->GetArrayLength(mtds.get());
    for (jsize i = 0; i < nmtds; ++i) {
      /* mtd: java.lang.reflect.Method */
      util::localref<jobject> mtd(env, sc, env->GetObjectArrayElement(
	mtds.get(), i));
      /* mname: java.lang.String */
      util::localref<jstring> mname(env, sc, static_cast<jstring>(
	env->CallObjectMethod(mtd.get(), sc->getMIDMGetName())));
      ase_string mnstr = util::to_ase_string(env, sc, mname.get());
      /* modif: java.lang.reflect.Modifier */
      jint modif = env->CallIntMethod(mtd.get(), sc->getMIDMGetModifiers());
      util::checkJavaException(env, sc, "JNI getModifiers");
      jvalue args[1];
      args[0].i = modif;
      bool is_static = env->CallStaticBooleanMethodA(sc->getModKlass(),
	sc->getMIDIsStatic(), args) != 0;
      util::checkJavaException(env, sc, "JNI isStatic");
      props_type::iterator piter = register_prop(sc, mnstr, is_static);
      piter->second->register_method(env, sc, mtd);
      DBG_CLASS(fprintf(stderr, "method %s\n", mnstr.data()));
    }
  }
  /* register fields */
  {
    /* flds: java.lang.reflect.Field[] */
    util::localref<jobjectArray> flds(env, sc, static_cast<jobjectArray>(
      env->CallObjectMethod(klass.get(), sc->getMIDGetFields())));
    const jsize nflds = env->GetArrayLength(flds.get());
    for (jsize i = 0; i < nflds; ++i) {
      /* fld: java.lang.reflect.Field */
      util::localref<jobject> fld(env, sc, env->GetObjectArrayElement(
	flds.get(), i));
      /* mname: java.lang.String */
      util::localref<jstring> fname(env, sc, static_cast<jstring>(
	env->CallObjectMethod(fld.get(), sc->getMIDFGetName())));
      ase_string fnstr = util::to_ase_string(env, sc, fname.get());
      /* modif: java.lang.reflect.Modifier */
      jint modif = env->CallIntMethod(fld.get(), sc->getMIDFGetModifiers());
      util::checkJavaException(env, sc, "JNI getModifiers");
      jvalue args[1];
      args[0].i = modif;
      bool is_static = env->CallStaticBooleanMethodA(sc->getModKlass(),
	sc->getMIDIsStatic(), args) != 0;
      util::checkJavaException(env, sc, "JNI isStatic");
      props_type::iterator piter = register_prop(sc, fnstr, is_static);
      piter->second->register_field(env, sc, fld);
    }
  }
  /* initialize proparr */
  {
    /* because props and static_props are sorted maps, proparr and
      static_proparr will be sorted also. */
    props_type::const_iterator i;
    if (!is_latebind) {
      for (i = props.begin(); i != props.end(); ++i) {
	if (i->second->field.get() != 0) {
	  /* not a method */
	  continue;
	}
	proparr.push_back(i->second);
      }
    }
  }
  /* register builtin methods and class metamethods */
  {
    /* builtin methods */
    register_prop(sc, ase_string("NewInstance"), true)
      ->second->register_as_builtin(0);
    register_prop(sc, ase_string("NewArray"), true)
      ->second->register_as_builtin(1);
    register_prop(sc, ase_string("NewArrayValue"), true)
      ->second->register_as_builtin(2);
    /* class metamethods */
    ase_jni_classinfo *jlc_ci = sc->get_jlclass_classinfo();
    if (jlc_ci == 0) {
      jlc_ci = this;
	/* 'this' must be java.lang.Class, becuase j.l.Class is initialized
	  first. note that proparr for j.l.Class must be initialized
	  already. */
    }
    ase_size_type i;
    for (i = 0; i < jlc_ci->proparr.size(); ++i) {
      ase_jni_propinfo *pi = jlc_ci->proparr[i];
      if (pi->methods.empty()) {
	continue;
      }
      DBG_CLSMETA(fprintf(stderr, "REGISTER CLASS META: %s\n",
	pi->name.data()));
      register_prop(sc, pi->name, true)->second
	->register_as_classmethod(static_cast<ase_int>(i));
    }
  }
  /* initialize static_proparr */
  {
    props_type::const_iterator i;
    for (i = static_props.begin(); i != static_props.end(); ++i) {
      if (i->second->field.get() != 0) {
	/* not a method */
	continue;
      }
      static_proparr.push_back(i->second);
    }
  }
  /* register constructors */
  {
    /* cstrs: java.lang.reflect.Constructor[] */
    util::localref<jobjectArray> cstrs(env, sc, static_cast<jobjectArray>(
      env->CallObjectMethod(klass.get(), sc->getMIDGetConstructors())));
    jsize ncstrs = env->GetArrayLength(cstrs.get());
    for (jsize i = 0; i < ncstrs; ++i) {
      /* cstr: java.lang.reflect.Constructor */
      util::localref<jobject> cstr(env, sc, env->GetObjectArrayElement(
	cstrs.get(), i));
      std::auto_ptr<ase_jni_funcinfo> finf(new ase_jni_funcinfo(env, sc,
	cstr, true));
      constructors.push_back(finf.get());
      finf.release();
    }
  }
  /* initialize vtbl and static_vtbl */
  {
    vtbl = *sc->get_variant_vtable_for_jobject();
    vtbl_methods.resize(proparr.size());
    proparr_type::size_type i;
    for (i = 0; i < proparr.size(); ++i) {
      vtbl_methods[i].u8name = proparr[i]->name.data();
      vtbl_methods[i].u8name_len = proparr[i]->name.size();
      DBG_VT(printf("p %d u8name=%s\n", i, vtbl_methods[i].u8name));
    }
    if (!vtbl_methods.empty()) {
      vtbl.methods.methods = &vtbl_methods[0];
      vtbl.methods.num_methods = vtbl_methods.size();
    }
    static_vtbl = *sc->get_variant_vtable_for_jobject();
    static_vtbl_methods.resize(static_proparr.size());
    for (i = 0; i < static_proparr.size(); ++i) {
      static_vtbl_methods[i].u8name = static_proparr[i]->name.data();
      static_vtbl_methods[i].u8name_len = static_proparr[i]->name.size();
      DBG_VT(printf("p %d u8name=%s\n", i, static_vtbl_methods[i].u8name));
    }
    if (!static_vtbl_methods.empty()) {
      static_vtbl.methods.methods = &static_vtbl_methods[0];
      static_vtbl.methods.num_methods = static_vtbl_methods.size();
    }
  }
  /* array component type */
  {
    jboolean isarr = env->CallBooleanMethod(klass.get(), sc->getMIDIsArray());
    util::checkJavaException(env, sc, "JNI isArray");
    if (isarr) {
      util::localref<jclass> ckl(env, sc, static_cast<jclass>(
	env->CallObjectMethod(klass.get(), sc->getMIDGetComponentType())));
      component_kinfo = cm->find_type_internal_nolock(env, sc, ckl);
	/* can be recursive */
      DBG_ARR(fprintf(stderr, "ARR %s component=%s\n", name.c_str(),
	component_kinfo->name.c_str()));
    }
  }
}

ase_jni_classinfo::props_type::iterator
ase_jni_classinfo::register_prop(ase_jni_script_engine *sc,
  const ase_string& name, bool isst)
{
  props_type& tprops = (isst) ? static_props : props;
  props_type::iterator iter = tprops.find(name);
  if (iter != tprops.end()) {
    return iter;
  }
  std::auto_ptr<ase_jni_propinfo> pinf(new ase_jni_propinfo(name));
  iter = tprops.insert(std::make_pair(name, pinf.get())).first;
  pinf.release();
  return iter;
}

struct compare_funcinfo {
  bool operator ()(ase_jni_funcinfo *fx, ase_jni_funcinfo *fy) const {
    return compare_internal(fx, fy) < 0;
  }
  int compare_internal(ase_jni_funcinfo *fx, ase_jni_funcinfo *fy) const {
    /* descendent order (i.e., sign(fy - fx) ) */
    ase_size_type xlen = fx->argtypes.size();
    ase_size_type ylen = fy->argtypes.size();
    const int lendiff = static_cast<int>(ylen) - static_cast<int>(xlen);
    if (lendiff != 0) {
      return lendiff;
    }
    /* xlen == ylen */
    for (ase_size_type i = 0; i < xlen; ++i) {
      ase_jni_classinfo& ax = *(fx->argtypes[i]);
      ase_jni_classinfo& ay = *(fy->argtypes[i]);
      DBG_COMPARE(fprintf(stderr, "COMPARE %d %d\n", ax.tinfo->valtype,
	ay.tinfo->valtype));
      const int tydiff = ax.tinfo->valtype - ay.tinfo->valtype;
      if (tydiff != 0) {
	return tydiff;
      }
      DBG_COMPARE(fprintf(stderr, "COMPARE %s %s\n", ax.name.c_str(),
	ay.name.c_str()));
      int c = ay.name.compare(ax.name);
      if (c != 0) {
	return c;
      }
    }
    return 0;
  }
};

void
ase_jni_classinfo::resolve_constr_nolock(JNIEnv *env,
  ase_jni_script_engine *sc, ase_jni_classmap *cm)
{
  if (constr_resolved) {
    return;
  }
  DBG_RESOLVE(fprintf(stderr, "RESOLVE constr class=%s num=%d\n",
    name.data(), (int)constructors.size()));
  for (constructors_type::size_type i = 0; i < constructors.size(); ++i) {
    constructors[i]->resolve_nolock(env, sc, cm);
  }
  /* sort constructors */
  std::sort(constructors.begin(), constructors.end(), compare_funcinfo());
  constr_resolved = true;
}

void
ase_jni_propinfo::register_method(JNIEnv *env, ase_jni_script_engine *sc,
  const util::localref<jobject>& mtd)
{
  std::auto_ptr<ase_jni_funcinfo> finf(
    new ase_jni_funcinfo(env, sc, mtd, false));
  methods.push_back(finf.get());
  finf.release();
}

void
ase_jni_propinfo::register_field(JNIEnv *env, ase_jni_script_engine *sc,
  const util::localref<jobject>& fld)
{
  field.reset(new ase_jni_fieldinfo(env, sc, fld));
}

void
ase_jni_propinfo::register_as_classmethod(ase_int id)
{
  is_classmethod = true;
  class_or_builtin_method_id = id;
}

void
ase_jni_propinfo::register_as_builtin(ase_int id)
{
  is_builtinmethod = true;
  class_or_builtin_method_id = id;
}

ase_jni_funcinfo::ase_jni_funcinfo(JNIEnv *env, ase_jni_script_engine *sc,
  const util::localref<jobject>& mtd, bool iscnstr)
  : mid(0), is_constr(iscnstr)
{
  method.set(sc->getJVM(), env, mtd);
}

void
ase_jni_funcinfo::resolve_nolock(JNIEnv *env, ase_jni_script_engine *sc,
  ase_jni_classmap *cm)
{
  if (mid) {
    return; /* already resolved */
  }
  if (!is_constr) {
    util::localref<jclass> rtype(env, sc, static_cast<jclass>(
      env->CallObjectMethod(method.get(), sc->getMIDMGetReturnType())));
    rettype = cm->find_type_internal_nolock(env, sc, rtype);
  }
  jmethodID getpt_methodid = is_constr
    ? sc->getMIDCGetParameterTypes()
    : sc->getMIDMGetParameterTypes();
  util::localref<jobjectArray> paramtypes(env, sc, static_cast<jobjectArray>(
    env->CallObjectMethod(method.get(), getpt_methodid)));
  const jsize nparams = env->GetArrayLength(paramtypes.get());
  argtypes.resize(nparams);
  for (jsize i = 0; i < nparams; ++i) {
    /* fldkl: java.lang.Class */
    util::localref<jclass> fldkl(env, sc, static_cast<jclass>(
      env->GetObjectArrayElement(paramtypes.get(), i)));
    argtypes[i] = cm->find_type_internal_nolock(env, sc, fldkl);
  }
  mid = env->FromReflectedMethod(method.get());
  DBG_RESOLVE_FI(fprintf(stderr, "funcinfo resolve fi=%p mid=%p nparams=%d\n",
    this, mid, (int)nparams));
}

ase_jni_fieldinfo::ase_jni_fieldinfo(JNIEnv *env, ase_jni_script_engine *sc,
  const util::localref<jobject>& fld)
  : fid(0)
{
  field.set(sc->getJVM(), env, fld);
}

void
ase_jni_fieldinfo::resolve_nolock(JNIEnv *env, ase_jni_script_engine *sc,
  ase_jni_classmap *cm)
{
  if (fid) {
    return; /* already resolved */
  }
  util::localref<jclass> rtype(env, sc, static_cast<jclass>(
    env->CallObjectMethod(field.get(), sc->getMIDFGetType())));
  fldtype = cm->find_type_internal_nolock(env, sc, rtype);
  fid = env->FromReflectedField(field.get());
}

void
ase_jni_propinfo::resolve_nolock(JNIEnv *env, ase_jni_script_engine *sc,
  ase_jni_classmap *cm)
{
  if (resolved) {
    return;
  }
  if (field.get()) {
    field->resolve_nolock(env, sc, cm);
  }
  DBG_RESOLVE(fprintf(stderr, "RESOLVE prop=%s num=%d\n",
    name.data(), (int)methods.size()));
  for (methods_type::size_type i = 0; i < methods.size(); ++i) {
    methods[i]->resolve_nolock(env, sc, cm);
  }
  /* sort methods */
  std::sort(methods.begin(), methods.end(), compare_funcinfo());
  resolved = true;
}

void
ase_jni_classmap::register_primitive_classinfo(JNIEnv *env,
  ase_jni_script_engine *sc, const std::string& name,
  const ase_jni_typeinfo *ti, jclass kl)
{
  ase_string s(name);
  util::localref<jclass> lkl(env, sc,
    static_cast<jclass>(env->NewLocalRef(kl)));
  std::auto_ptr<ase_jni_classinfo> ci(new ase_jni_classinfo(env, sc, this,
    s, ti, lkl));
  classmap[s] = ci.get();
  ci.release();
}

ase_jni_classinfo *
ase_jni_classmap::find_type_internal_nolock(JNIEnv *env,
  ase_jni_script_engine *sc, const util::localref<jclass>& kl)
{
  util::localref<jstring> nstr(env, sc,
    static_cast<jstring>(env->CallObjectMethod(kl.get(),
      sc->getMIDGetName())));
  ase_string name = util::to_ase_string(env, sc, nstr.get());
  DBG_FINDTYPE(fprintf(stderr, "findtype: %s\n", name.data()));
  map_type::iterator iter = classmap.find(name);
  if (iter != classmap.end()) {
    DBG_FINDTYPE(fprintf(stderr, "findtype: found\n"));
    return iter->second;
  }
  DBG_FINDTYPE(fprintf(stderr, "findtype: new\n"));
  std::auto_ptr<ase_jni_classinfo> ci(new ase_jni_classinfo(
    env, sc, this, name, &ase_jni_typeinfo_object, kl));
  classmap[name] = ci.get();
  ase_jni_classinfo *ciraw = ci.release(); /* classmap holds the ownership */
  return ciraw;
}

ase_jni_classinfo *
ase_jni_classmap::find_type_synchronized(JNIEnv *env,
  ase_jni_script_engine *sc, const util::localref<jclass>& kl)
{
  ase_mutex_guard<mutex_type> g(mutex);
  return find_type_internal_nolock(env, sc, kl);
}

void
ase_jni_classmap::resolve_funcinfo_synchronized(ase_jni_funcinfo& fi,
  JNIEnv *env, ase_jni_script_engine *sc)
{
  ase_mutex_guard<mutex_type> g(mutex);
  return fi.resolve_nolock(env, sc, this);
}

void
ase_jni_classmap::resolve_field_synchronized(ase_jni_fieldinfo& fi,
  JNIEnv *env, ase_jni_script_engine *sc)
{
  ase_mutex_guard<mutex_type> g(mutex);
  return fi.resolve_nolock(env, sc, this);
}

void
ase_jni_classmap::resolve_propinfo_synchronized(ase_jni_propinfo& pi,
  JNIEnv *env, ase_jni_script_engine *sc)
{
  ase_mutex_guard<mutex_type> g(mutex);
  return pi.resolve_nolock(env, sc, this);
}

void
ase_jni_classmap::resolve_constr_synchronized(ase_jni_classinfo& ci,
  JNIEnv *env, ase_jni_script_engine *sc)
{
  ase_mutex_guard<mutex_type> g(mutex);
  return ci.resolve_constr_nolock(env, sc, this);
}

}; // namespace asejni

