
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asecast.hpp>
#include <ase/asealloca.hpp>

#include "asejni_priv.hpp"
#include "asejni_classinfo.hpp"

#define DBG(x)
#define DBG_GET(x)
#define DBG_GETSTR(x)

namespace asejni {

void
NativeProxy_finalizeImpl(JNIEnv *env, jobject thisobj, jlong pr)
{
  ase_jni_jniproxy_priv *priv = reinterpret_cast<ase_jni_jniproxy_priv *>(pr);
  if (priv) {
    DBG(fprintf(stderr, "finalizeImpl %p\n", priv));
    delete priv;
  }
}

jobject
NativeProxy_invokeImpl(JNIEnv *env, jobject thisobj,
  jlong pr /* RAW POINTER */, jstring mname, jobject proxy /* Object */,
  jobject method /* Method */, jobject args /* Object[] */)
{
  /* called from InvocationHandler.invoke() */
  ase_jni_jniproxy_priv *priv = reinterpret_cast<ase_jni_jniproxy_priv *>(pr);
  assert(priv);
  assert(priv->classinfo);
  ase_jni_script_engine *const sc = priv->script_engine;
  try {
    ase_jni_classinfo& ci = *priv->classinfo;
    ase_string name8 = util::to_ase_string(env, sc, mname);
    ase_jni_classinfo::props_type::iterator iter = ci.props.find(name8);
    if (iter == ci.props.end()) {
      /* method not found */
      ase_string s = "JNI InvocationHandler: method not found: " + name8;
      ase_throw_invalid_arg(s.data());
    }
    DBG(fprintf(stderr, "invokeImpl prop %s found\n", name8.c_str()));
    ase_jni_propinfo& pi = *iter->second;
    ase_jni_propinfo::methods_type::size_type i;
    if (pi.methods.size() == 1) {
      i = 0;
    } else {
      for (i = 0; i < pi.methods.size(); ++i) {
	ase_jni_funcinfo& fi = *pi.methods[i];
	DBG(fprintf(stderr, "method %p fimethod %p\n", method,
	  fi.method.get()));
	jvalue cargs[1];
	cargs[0].l = fi.method.get();
	jboolean eq = env->CallBooleanMethodA(method, sc->getMIDEquals(),
	  cargs);
	util::checkJavaException(env, sc, "JNI equals");
	if (eq) {
	  DBG(fprintf(stderr, "equv\n"));
	  break;
	}
	DBG(fprintf(stderr, "not equv\n"));
      }
    }
    if (i == pi.methods.size()) {
      /* method not found */
      ase_string s = "JNI InvocationHandler: method not found: " + name8;
      ase_throw_invalid_arg(s.data());
    }
    ase_jni_funcinfo& fi = *pi.methods[i];
    sc->getClassMap().resolve_funcinfo_synchronized(fi, env, sc);
    jobjectArray jargs = static_cast<jobjectArray>(args);
    const ase_size_type nargs = fi.argtypes.size();
    ASE_FASTVECTOR(ase_variant, nargs, aargs);
    for (ase_size_type j = 0; j < nargs; ++j) {
      ase_jni_classinfo& argci = *(fi.argtypes[j]);
      const ase_jni_typeinfo& ti = *(argci.tinfo);
      jobject elem = env->GetObjectArrayElement(jargs, static_cast<jsize>(j));
      util::checkJavaException(env, sc, "JNI getarrayelement");
      aargs[j] = ti.to_ase_boxed(env, sc, elem, &argci);
      util::checkJavaException(env, sc, "JNI toaseboxed");
    }
    ase_variant asethis = priv->value.get();
    ase_variant aserval = asethis.invoke_by_name(name8.data(), name8.size(),
      ASE_FASTVECTOR_GETPTR(aargs), nargs);
    const ase_jni_typeinfo& rti = *(fi.rettype->tinfo);
    bool is_localref = false;
    jobject r = rti.to_java_boxed(env, sc, aserval, is_localref);
      /* we don't care whether r is a localref or not */
    util::checkJavaException(env, sc, "JNI tojavaboxed");
    DBG(fprintf(stderr, "invokeImpl %p\n", priv));
    return r;
  } catch (...) {
    util::handleCppException(env, sc);
  }
  return 0;
}

jint
NativeProxy_GetAttributesImpl(JNIEnv *env, jobject thisobj,
  jlong pr /* RAW POINTER */)
{
  ase_jni_jniproxy_priv *priv = reinterpret_cast<ase_jni_jniproxy_priv *>(pr);
  assert(priv);
  assert(priv->classinfo);
  ase_jni_script_engine *const sc = priv->script_engine;
  try {
    ase_int r = priv->value.get().get_attributes();
    return r;
  } catch (...) {
    util::handleCppException(env, sc);
  }
  return 0;
}

jstring
NativeProxy_GetStringImpl(JNIEnv *env, jobject thisobj,
  jlong pr /* RAW POINTER */)
{
  ase_jni_jniproxy_priv *priv = reinterpret_cast<ase_jni_jniproxy_priv *>(pr);
  assert(priv);
  assert(priv->classinfo);
  ase_jni_script_engine *const sc = priv->script_engine;
  try {
    DBG_GETSTR(fprintf(stderr, "NativeProxy_GetStringImpl begin"));
    ase_string ar = priv->value.get().get_string();
    DBG_GETSTR(fprintf(stderr, "NativeProxy_GetStringImpl: %s", ar.data()));
    jstring r = util::to_java_string(env, ar);
    DBG_GETSTR(fprintf(stderr, "NativeProxy_GetStringImpl end"));
    return r;
  } catch (...) {
    util::handleCppException(env, sc);
  }
  return 0;
}

jobject
NativeProxy_GetPropertyImpl(JNIEnv *env, jobject thisobj,
  jlong pr /* RAW POINTER */, jstring name)
{
  ase_jni_jniproxy_priv *priv = reinterpret_cast<ase_jni_jniproxy_priv *>(pr);
  assert(priv);
  assert(priv->classinfo);
  ase_jni_script_engine *const sc = priv->script_engine;
  try {
    ase_string aname = util::to_ase_string(env, sc, name);
    DBG_GET(fprintf(stderr, "NativeProxy_GetPropertyImpl: name=%s\n",
      aname.data()));
    ase_variant r = priv->value.get().get_property(aname);
    DBG_GET(fprintf(stderr, "NativeProxy_GetPropertyImpl: ret\n"));
    util::jobjectref jr(env);
    DBG_GET(fprintf(stderr, "NativeProxy_GetPropertyImpl: conv\n"));
    util::to_jobject_from_ase_variant(env, sc, r, jr);
    DBG_GET(fprintf(stderr, "NativeProxy_GetPropertyImpl: conv1\n"));
    DBG_GET(ase_variant av = util::to_ase_variant_from_jobject(env, sc,
      jr.get()));
    DBG_GET(fprintf(stderr, "NativeProxy_GetPropertyImpl: conv2\n"));
    DBG_GET(fprintf(stderr,
      "NativeProxy_GetPropertyImpl convdone %p str==%s\n",
      jr.get(), av.get_string().data()));
    /* i have no idea why jvm converts retval to Object if it's not a
      localref */
    return jr.to_localref_and_release(sc);
  } catch (...) {
    util::handleCppException(env, sc);
  }
  return 0;
}

void
NativeProxy_SetPropertyImpl(JNIEnv *env, jobject thisobj,
  jlong pr /* RAW POINTER */, jstring name, jobject value)
{
  ase_jni_jniproxy_priv *priv = reinterpret_cast<ase_jni_jniproxy_priv *>(pr);
  assert(priv);
  assert(priv->classinfo);
  ase_jni_script_engine *const sc = priv->script_engine;
  try {
    ase_string aname = util::to_ase_string(env, sc, name);
    ase_variant aval = util::to_ase_variant_from_jobject(env, sc, value);
    priv->value.get().set_property(aname, aval);
  } catch (...) {
    util::handleCppException(env, sc);
  }
}

void
NativeProxy_DelPropertyImpl(JNIEnv *env, jobject thisobj,
  jlong pr /* RAW POINTER */, jstring name)
{
  ase_jni_jniproxy_priv *priv = reinterpret_cast<ase_jni_jniproxy_priv *>(pr);
  assert(priv);
  assert(priv->classinfo);
  ase_jni_script_engine *const sc = priv->script_engine;
  try {
    ase_string aname = util::to_ase_string(env, sc, name);
    priv->value.get().del_property(aname);
  } catch (...) {
    util::handleCppException(env, sc);
  }
}

jobject
NativeProxy_GetElementImpl(JNIEnv *env, jobject thisobj,
  jlong pr /* RAW POINTER */, jint idx)
{
  ase_jni_jniproxy_priv *priv = reinterpret_cast<ase_jni_jniproxy_priv *>(pr);
  assert(priv);
  assert(priv->classinfo);
  ase_jni_script_engine *const sc = priv->script_engine;
  try {
    ase_int aidx = ase_numeric_cast<ase_int>(idx);
    ase_variant r = priv->value.get().get_element(aidx);
    util::jobjectref jr(env);
    util::to_jobject_from_ase_variant(env, sc, r, jr);
    return jr.to_localref_and_release(sc);
  } catch (...) {
    util::handleCppException(env, sc);
  }
  return 0;
}

void
NativeProxy_SetElementImpl(JNIEnv *env, jobject thisobj,
  jlong pr /* RAW POINTER */, jint idx, jobject value)
{
  ase_jni_jniproxy_priv *priv = reinterpret_cast<ase_jni_jniproxy_priv *>(pr);
  assert(priv);
  assert(priv->classinfo);
  ase_jni_script_engine *const sc = priv->script_engine;
  try {
    ase_int aidx = ase_numeric_cast<ase_int>(idx);
    ase_variant aval = util::to_ase_variant_from_jobject(env, sc, value);
    priv->value.get().set_element(aidx, aval);
  } catch (...) {
    util::handleCppException(env, sc);
  }
}

jint
NativeProxy_GetLengthImpl(JNIEnv *env, jobject thisobj,
  jlong pr /* RAW POINTER */)
{
  ase_jni_jniproxy_priv *priv = reinterpret_cast<ase_jni_jniproxy_priv *>(pr);
  assert(priv);
  assert(priv->classinfo);
  ase_jni_script_engine *const sc = priv->script_engine;
  try {
    ase_int alen = priv->value.get().get_length();
    return ase_numeric_cast<jint>(alen);
  } catch (...) {
    util::handleCppException(env, sc);
  }
  return -1;
}

void
NativeProxy_SetLengthImpl(JNIEnv *env, jobject thisobj,
  jlong pr /* RAW POINTER */, jint len)
{
  ase_jni_jniproxy_priv *priv = reinterpret_cast<ase_jni_jniproxy_priv *>(pr);
  assert(priv);
  assert(priv->classinfo);
  ase_jni_script_engine *const sc = priv->script_engine;
  try {
    ase_int alen = ase_numeric_cast<ase_int>(len);
    priv->value.get().set_length(alen);
  } catch (...) {
    util::handleCppException(env, sc);
  }
}

jobject
NativeProxy_InvokeByNameImpl(JNIEnv *env, jobject thisobj,
  jlong pr /* RAW POINTER */, jstring name, jobject args /* Object[] */)
{
  ase_jni_jniproxy_priv *priv = reinterpret_cast<ase_jni_jniproxy_priv *>(pr);
  assert(priv);
  assert(priv->classinfo);
  ase_jni_script_engine *const sc = priv->script_engine;
  try {
    ase_string aname = util::to_ase_string(env, sc, name);
    jobjectArray jargs = static_cast<jobjectArray>(args);
    jsize jnargs = jargs ? env->GetArrayLength(jargs) : 0;
    const ase_size_type nargs = static_cast<ase_size_type>(jnargs);
    ASE_FASTVECTOR(ase_variant, nargs, aargs);
    for (ase_size_type i = 0; i < nargs; ++i) {
      util::localref<jobject> elem(env, sc, env->GetObjectArrayElement(jargs,
	static_cast<jsize>(i)));
      aargs[i] = util::to_ase_variant_from_jobject(env, sc, elem.get());
    }
    ase_variant r = priv->value.get().invoke_by_name(aname,
      ASE_FASTVECTOR_GETPTR(aargs), nargs);
    util::jobjectref jr(env);
    util::to_jobject_from_ase_variant(env, sc, r, jr);
    return jr.to_localref_and_release(sc);
  } catch (...) {
    util::handleCppException(env, sc);
  }
  return 0;
}

jobject
NativeProxy_InvokeSelfImpl(JNIEnv *env, jobject thisobj,
  jlong pr /* RAW POINTER */, jobject args /* Object[] */)
{
  ase_jni_jniproxy_priv *priv = reinterpret_cast<ase_jni_jniproxy_priv *>(pr);
  assert(priv);
  assert(priv->classinfo);
  ase_jni_script_engine *const sc = priv->script_engine;
  try {
    jobjectArray jargs = static_cast<jobjectArray>(args);
    jsize jnargs = jargs ? env->GetArrayLength(jargs) : 0;
    const ase_size_type nargs = static_cast<ase_size_type>(jnargs);
    ASE_FASTVECTOR(ase_variant, nargs, aargs);
    for (ase_size_type i = 0; i < nargs; ++i) {
      util::localref<jobject> elem(env, sc, env->GetObjectArrayElement(jargs,
	static_cast<jsize>(i)));
      aargs[i] = util::to_ase_variant_from_jobject(env, sc, elem.get());
    }
    ase_variant r = priv->value.get().invoke_self(ASE_FASTVECTOR_GETPTR(aargs),
      nargs);
    util::jobjectref jr(env);
    util::to_jobject_from_ase_variant(env, sc, r, jr);
    return jr.to_localref_and_release(sc);
  } catch (...) {
    util::handleCppException(env, sc);
  }
  return 0;
}

jobject
NativeProxy_GetEnumeratorImpl(JNIEnv *env, jobject thisobj,
  jlong pr /* RAW POINTER */)
{
  ase_jni_jniproxy_priv *priv = reinterpret_cast<ase_jni_jniproxy_priv *>(pr);
  assert(priv);
  assert(priv->classinfo);
  ase_jni_script_engine *const sc = priv->script_engine;
  try {
    ase_variant r = priv->value.get().get_enumerator();
    util::jobjectref jr(env);
    util::to_jobject_from_ase_variant(env, sc, r, jr);
    return jr.to_localref_and_release(sc);
  } catch (...) {
    util::handleCppException(env, sc);
  }
  return 0;
}

jstring
NativeProxy_EnumNextImpl(JNIEnv *env, jobject thisobj,
  jlong pr /* RAW POINTER */)
{
  ase_jni_jniproxy_priv *priv = reinterpret_cast<ase_jni_jniproxy_priv *>(pr);
  assert(priv);
  assert(priv->classinfo);
  ase_jni_script_engine *const sc = priv->script_engine;
  try {
    bool hasnext = false;
    ase_string ar = priv->value.get().enum_next(hasnext);
    if (!hasnext) {
      return 0;
    }
    jstring r = util::to_java_string(env, ar);
    return r;
  } catch (...) {
    util::handleCppException(env, sc);
  }
  return 0;
}

const JNINativeMethod nprx_methods[] = {
  { (char *)"finalizeImpl", (char *)"(J)V",
    (void *)NativeProxy_finalizeImpl },
  { (char *)"invokeImpl",
    (char *)"(JLjava/lang/String;Ljava/lang/Object;Ljava/lang/reflect/Method;"
      "[Ljava/lang/Object;)Ljava/lang/Object;",
    (void *)NativeProxy_invokeImpl },
  { (char *)"getAttributesImpl", (char *)"(J)I",
    (void *)NativeProxy_GetAttributesImpl },
  { (char *)"getStringImpl", (char *)"(J)Ljava/lang/String;",
    (void *)NativeProxy_GetStringImpl },
  { (char *)"getPropertyImpl",
    (char *)"(JLjava/lang/String;)Ljava/lang/Object;",
    (void *)NativeProxy_GetPropertyImpl },
  { (char *)"setPropertyImpl",
    (char *)"(JLjava/lang/String;Ljava/lang/Object;)V",
    (void *)NativeProxy_SetPropertyImpl },
  { (char *)"delPropertyImpl", (char *)"(JLjava/lang/String;)V",
    (void *)NativeProxy_DelPropertyImpl },
  { (char *)"getElementImpl", (char *)"(JI)Ljava/lang/Object;",
    (void *)NativeProxy_GetElementImpl },
  { (char *)"setElementImpl", (char *)"(JILjava/lang/Object;)V",
    (void *)NativeProxy_SetElementImpl },
  { (char *)"getLengthImpl", (char *)"(J)I",
    (void *)NativeProxy_GetLengthImpl },
  { (char *)"setLengthImpl", (char *)"(JI)V",
    (void *)NativeProxy_SetLengthImpl },
  { (char *)"invokeByNameImpl",
    (char *)"(JLjava/lang/String;[Ljava/lang/Object;)"
      "Ljava/lang/Object;",
    (void *)NativeProxy_InvokeByNameImpl },
  { (char *)"invokeSelfImpl",
    (char *)"(J[Ljava/lang/Object;)Ljava/lang/Object;",
    (void *)NativeProxy_InvokeSelfImpl },
  { (char *)"getEnumeratorImpl",
    (char *)"(J)Ljava/lang/Object;",
    (void *)NativeProxy_GetEnumeratorImpl },
  { (char *)"enumNextImpl", (char *)"(J)Ljava/lang/String;",
    (void *)NativeProxy_EnumNextImpl },
};

void
util::register_native_jnativeproxy(JNIEnv *env, jclass kl)
{
  const size_t num = sizeof(nprx_methods) / sizeof(nprx_methods[0]);
  if (env->RegisterNatives(kl, nprx_methods, num) < 0) {
    const char *s = "failed to register NativeProxy natives";
    throw std::logic_error(s); /* ASE THROW */
  }
}

}; // namespace asejni

