
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asecast.hpp>
#include <ase/aseutf16.hpp>

#include "asejni_util.hpp"
#include "asejni_sctx.hpp"
#include "asejni_jobjpr.hpp"
#include "asejni_priv.hpp"
#include "asejni_classinfo.hpp"
#include "asejni_jcl.hpp"

#include <cstdlib>

#ifdef __GNUC__
#define ASE_THREAD_LOCAL __thread
#else
#define ASE_THREAD_LOCAL __declspec(thread)
#endif

#define DBG(x)
#define DBG_STR(x)
#define DBG_EXC(x)
#define DBG_CPPEXC(x)
#define DBG_EXC_ABORT(x)
#define DBG_THROW(x)

namespace asejni {

unsigned int util::verbose = 0;

JNIEnv *
util::getEnv(JavaVM *jvm)
{
  JNIEnv *env = 0;
  jint r = jvm->AttachCurrentThread(
    &reinterpret_cast<void *&>(env), 0);
    /* -fno-strict-aliasing */
  if (r < 0) {
    ase_throw_error("JNI: failed to attach JVM");
  }
  return env;
}

util::javastaticdata::javastaticdata(JavaVM *vm, JNIEnv *env,
  const ase_jni_engines& eng)
  : engines(eng),
    mainthread_env(env),
    mid_getclass(0), mid_equals(0),
    mid_forname(0), mid_isprimitive(0), mid_getname(0), mid_getconstructors(0),
    mid_getmethods(0), mid_getfields(0), mid_isarray(0),
    mid_getcomponenttype(0), mid_isinterface(0), mid_newinstance(0),
    mid_getsyscldr(0),
    mid_gc(0),
    mid_cgetparametertypes(0),
    mid_mgetname(0), mid_mgetmodifiers(0), mid_mgetreturntype(0),
    mid_mgetparametertypes(0),
    mid_fgetname(0), mid_fgetmodifiers(0), mid_fgettype(0), mid_isstatic(0),
    mid_anewinstance(0),
    mid_getinvocationhandler(0),
    mid_newproxy(0), fid_nprx_priv(0),
    mid_lbgetattributes(0), mid_lbgetstring(0),
    mid_lbgetproperty(0), mid_lbsetproperty(0), mid_lbdelproperty(0),
    mid_lbgetelement(0), mid_lbsetelement(0),
    mid_lbgetlength(0), mid_lbsetlength(0),
    mid_lbinvokebyname(0), mid_lbinvokebyid(0),
    mid_lbinvokeself(0), mid_lbgetenumerator(0), mid_lbenumnext(0),
    mid_aexcconstr(0), mid_aexcgetwrapped(0), mid_aexcinitwrapped(0),
    mid_compile(0),
    mid_objtostr(0), mid_ujoinall(0), mid_loadclass(0), mid_loadclasscur(0),
    mid_bvalueof(0), mid_booleanvalue(0),
    mid_byteconstr(0), mid_bytevalue(0),
    mid_charconstr(0), mid_charvalue(0),
    mid_shortconstr(0), mid_shortvalue(0),
    mid_intconstr(0), mid_intvalue(0),
    mid_longconstr(0), mid_longvalue(0),
    mid_floatconstr(0), mid_floatvalue(0),
    mid_doubleconstr(0), mid_doublevalue(0)
{
  util::localref<jclass> objectk(env, 0, util::findClass(env,
    "java/lang/Object"));
  objectklass.set(vm, env, objectk);
  util::localref<jclass> classk(env, 0, util::findClass(env,
    "java/lang/Class"));
  classklass.set(vm, env, classk);
  util::localref<jclass> cldrk(env, 0, util::findClass(env,
    "java/lang/ClassLoader"));
  cldrklass.set(vm, env, cldrk);
  util::localref<jclass> systemk(env, 0, util::findClass(env,
    "java/lang/System"));
  systemklass.set(vm, env, systemk);
  util::localref<jclass> constrk(env, 0, util::findClass(env,
    "java/lang/reflect/Constructor"));
  constrklass.set(vm, env, constrk);
  util::localref<jclass> methodk(env, 0, util::findClass(env,
    "java/lang/reflect/Method"));
  methodklass.set(vm, env, methodk);
  util::localref<jclass> fieldk(env, 0, util::findClass(env,
    "java/lang/reflect/Field"));
  fieldklass.set(vm, env, fieldk);
  util::localref<jclass> modk(env, 0, util::findClass(env,
    "java/lang/reflect/Modifier"));
  modklass.set(vm, env, modk);
  util::localref<jclass> arrayk(env, 0, util::findClass(env,
    "java/lang/reflect/Array"));
  arrayklass.set(vm, env, arrayk);
  util::localref<jclass> rprxk(env, 0, util::findClass(env,
    "java/lang/reflect/Proxy"));
  rprxklass.set(vm, env, rprxk);
  util::localref<jclass> jnprxk(env, 0, util::findClass(env,
    "asejni/NativeProxy"));
  jnprxklass.set(vm, env, jnprxk);
  util::localref<jclass> jlatek(env, 0, util::findClass(env,
    "asejni/LateBinding"));
  jlateklass.set(vm, env, jlatek);
  util::localref<jclass> aexck(env, 0, util::findClass(env,
    "asejni/ASEException"));
  aexcklass.set(vm, env, aexck);
  util::localref<jclass> cmpk(env, 0, util::findClass(env,
    "asejni/Compiler"));
  cmpklass.set(vm, env, cmpk);
  util::localref<jclass> utilk(env, 0, util::findClass(env,
    "asejni/Util"));
  utilklass.set(vm, env, utilk);
  util::localref<jclass> voidk(env, 0, util::findClass(env,
    "java/lang/Void"));
  voidklass.set(vm, env, voidk);
  util::localref<jclass> boolk(env, 0, util::findClass(env,
    "java/lang/Boolean"));
  boolklass.set(vm, env, boolk);
  util::localref<jclass> bytek(env, 0, util::findClass(env,
    "java/lang/Byte"));
  byteklass.set(vm, env, bytek);
  util::localref<jclass> chark(env, 0, util::findClass(env,
    "java/lang/Character"));
  charklass.set(vm, env, chark);
  util::localref<jclass> shortk(env, 0, util::findClass(env,
    "java/lang/Short"));
  shortklass.set(vm, env, shortk);
  util::localref<jclass> intk(env, 0, util::findClass(env,
    "java/lang/Integer"));
  intklass.set(vm, env, intk);
  util::localref<jclass> longk(env, 0, util::findClass(env,
    "java/lang/Long"));
  longklass.set(vm, env, longk);
  util::localref<jclass> floatk(env, 0, util::findClass(env,
    "java/lang/Float"));
  floatklass.set(vm, env, floatk);
  util::localref<jclass> doublek(env, 0, util::findClass(env,
    "java/lang/Double"));
  doubleklass.set(vm, env, doublek);
  util::localref<jclass> stringk(env, 0, util::findClass(env,
    "java/lang/String"));
  stringklass.set(vm, env, stringk);

  /* java.lang.Object */
  mid_getclass = util::getJMethodID(env, objectklass.get(), "getClass",
    "()Ljava/lang/Class;");
  mid_equals = util::getJMethodID(env, objectklass.get(), "equals",
    "(Ljava/lang/Object;)Z");
  /* java.lang.Class */
  mid_forname = util::getStaticMethodId(env, classklass.get(), "forName",
    "(Ljava/lang/String;ZLjava/lang/ClassLoader;)Ljava/lang/Class;");
    /* TODO: not used */
  mid_isprimitive = util::getJMethodID(env, classklass.get(), "isPrimitive",
    "()Z");
  mid_getname = util::getJMethodID(env, classklass.get(), "getName",
    "()Ljava/lang/String;");
  mid_getconstructors = util::getJMethodID(env, classklass.get(),
    "getConstructors", "()[Ljava/lang/reflect/Constructor;");
  mid_getmethods = util::getJMethodID(env, classklass.get(), "getMethods",
    "()[Ljava/lang/reflect/Method;");
  mid_getfields = util::getJMethodID(env, classklass.get(), "getFields",
    "()[Ljava/lang/reflect/Field;");
  mid_isarray = util::getJMethodID(env, classklass.get(), "isArray", "()Z");
  mid_getcomponenttype = util::getJMethodID(env, classklass.get(),
    "getComponentType", "()Ljava/lang/Class;");
  mid_isinterface = util::getJMethodID(env, classklass.get(), "isInterface",
    "()Z");
  mid_newinstance = util::getJMethodID(env, classklass.get(), "newInstance",
    "()Ljava/lang/Object;");
  /* java.lang.ClassLoader */
  mid_getsyscldr = util::getStaticMethodId(env, cldrklass.get(),
    "getSystemClassLoader", "()Ljava/lang/ClassLoader;");
  /* java.lang.System */
  mid_gc = util::getStaticMethodId(env, systemklass.get(), "gc", "()V");
  /* java.lang.reflect.Constructor */
  mid_cgetparametertypes = util::getJMethodID(env, constrklass.get(),
    "getParameterTypes", "()[Ljava/lang/Class;");
  /* java.lang.reflect.Method */
  mid_mgetname = util::getJMethodID(env, methodklass.get(), "getName",
    "()Ljava/lang/String;");
  mid_mgetmodifiers = util::getJMethodID(env, methodklass.get(),
    "getModifiers", "()I");
  mid_mgetreturntype = util::getJMethodID(env, methodklass.get(),
    "getReturnType", "()Ljava/lang/Class;");
  mid_mgetparametertypes = util::getJMethodID(env, methodklass.get(),
    "getParameterTypes", "()[Ljava/lang/Class;");
  /* java.lang.reflect.Field */
  mid_fgetname = util::getJMethodID(env, fieldklass.get(), "getName",
    "()Ljava/lang/String;");
  mid_fgetmodifiers = util::getJMethodID(env, fieldklass.get(),
    "getModifiers", "()I");
  mid_fgettype = util::getJMethodID(env, fieldklass.get(), "getType",
    "()Ljava/lang/Class;");
  /* java.lang.reflect.Modifier */
  mid_isstatic = util::getStaticMethodId(env, modklass.get(), "isStatic",
    "(I)Z");
  /* java.lang.reflect.Array */
  mid_anewinstance = util::getStaticMethodId(env, arrayklass.get(),
    "newInstance", "(Ljava/lang/Class;[I)Ljava/lang/Object;");
  /* java.lang.reflect.Proxy */
  mid_getinvocationhandler = util::getStaticMethodId(env, rprxklass.get(),
    "getInvocationHandler",
    "(Ljava/lang/Object;)Ljava/lang/reflect/InvocationHandler;");
  mid_isproxyclass = util::getStaticMethodId(env, rprxklass.get(),
    "isProxyClass", "(Ljava/lang/Class;)Z");
  /* asejni.NativeProxy */
  mid_newproxy = util::getStaticMethodId(env, jnprxklass.get(), "newProxy",
    "(Ljava/lang/Class;J)Ljava/lang/Object;");
  fid_nprx_priv = util::getFieldID(env, jnprxklass.get(), "priv", "J");
  /* asejni.LateBinding */
  mid_lbgetattributes = util::getJMethodID(env, jlateklass.get(),
    "getAttributes", "()I");
  mid_lbgetstring = util::getJMethodID(env, jlateklass.get(), "getString",
    "()Ljava/lang/String;");
  mid_lbgetproperty = util::getJMethodID(env, jlateklass.get(), "getProperty",
    "(Ljava/lang/String;)Ljava/lang/Object;");
  mid_lbsetproperty = util::getJMethodID(env, jlateklass.get(), "setProperty",
    "(Ljava/lang/String;Ljava/lang/Object;)V");
  mid_lbdelproperty = util::getJMethodID(env, jlateklass.get(), "delProperty",
    "(Ljava/lang/String;)V");
  mid_lbgetelement = util::getJMethodID(env, jlateklass.get(), "getElement",
    "(I)Ljava/lang/Object;");
  mid_lbsetelement = util::getJMethodID(env, jlateklass.get(), "setElement",
    "(ILjava/lang/Object;)V");
  mid_lbgetlength = util::getJMethodID(env, jlateklass.get(), "getLength",
    "()I");
  mid_lbsetlength = util::getJMethodID(env, jlateklass.get(), "setLength",
    "(I)V");
  mid_lbinvokebyname = util::getJMethodID(env, jlateklass.get(),
    "invokeByName",
    "(Ljava/lang/String;[Ljava/lang/Object;)Ljava/lang/Object;");
  /*
  mid_lbinvokebyid = util::getJMethodID(env, jlateklass.get(),
    "invokeById", "(I[Ljava/lang/Object;)Ljava/lang/Object;");
  */
  mid_lbinvokeself = util::getJMethodID(env, jlateklass.get(), "invokeSelf",
    "([Ljava/lang/Object;)Ljava/lang/Object;");
  mid_lbgetenumerator = util::getJMethodID(env, jlateklass.get(),
    "getEnumerator", "()Ljava/lang/Object;");
  mid_lbenumnext = util::getJMethodID(env, jlateklass.get(), "enumNext",
    "()Ljava/lang/String;");
  /* asejni.ASEException */
  mid_aexcconstr = util::getJMethodID(env, aexcklass.get(), "<init>",
    "(Ljava/lang/Object;)V");
  mid_aexcgetwrapped = util::getJMethodID(env, aexcklass.get(), "getWrapped",
    "()Ljava/lang/Object;");
  mid_aexcinitwrapped = util::getJMethodID(env, aexcklass.get(), "initWrapped",
    "()V");
  /* asejni.Util */
  mid_objtostr = util::getStaticMethodId(env, utilklass.get(),
    "objectToString", "(Ljava/lang/Object;)Ljava/lang/String;");
  mid_ujoinall = util::getStaticMethodId(env, utilklass.get(), "joinAll",
    "()I");
  mid_loadclass = util::getStaticMethodId(env, utilklass.get(), "loadClass",
    "(Ljava/lang/String;Ljava/lang/ClassLoader;)Ljava/lang/Class;");
  mid_loadclasscur = util::getStaticMethodId(env, utilklass.get(),
    "loadClassCurrentDir", "(Ljava/lang/String;)Ljava/lang/Class;");
  /* asejni.Compiler */
  mid_compile = util::getStaticMethodId(env, cmpklass.get(), "compile",
    "(Ljava/lang/String;)V");
  /* java.lang.Boolean */
  mid_bvalueof = util::getStaticMethodId(env, boolklass.get(), "valueOf",
    "(Z)Ljava/lang/Boolean;");
  mid_booleanvalue = util::getJMethodID(env, boolklass.get(), "booleanValue",
    "()Z");
  /* java.lang.Byte */
  mid_byteconstr = util::getJMethodID(env, byteklass.get(), "<init>", "(B)V");
  mid_bytevalue = util::getJMethodID(env, byteklass.get(), "byteValue", "()B");
  /* java.lang.Character */
  mid_charconstr = util::getJMethodID(env, charklass.get(), "<init>", "(C)V");
  mid_charvalue = util::getJMethodID(env, charklass.get(), "charValue", "()C");
  /* java.lang.Short */
  mid_shortconstr = util::getJMethodID(env, shortklass.get(), "<init>",
    "(S)V");
  mid_shortvalue = util::getJMethodID(env, shortklass.get(), "shortValue",
    "()S");
  /* java.lang.Integer */
  mid_intconstr = util::getJMethodID(env, intklass.get(), "<init>", "(I)V");
  mid_intvalue = util::getJMethodID(env, intklass.get(), "intValue", "()I");
  /* java.lang.Long */
  mid_longconstr = util::getJMethodID(env, longklass.get(), "<init>", "(J)V");
  mid_longvalue = util::getJMethodID(env, longklass.get(), "longValue", "()J");
  /* java.lang.Float */
  mid_floatconstr = util::getJMethodID(env, floatklass.get(), "<init>",
    "(F)V");
  mid_floatvalue = util::getJMethodID(env, floatklass.get(), "floatValue",
    "()F");
  mid_doubleconstr = util::getJMethodID(env, doubleklass.get(), "<init>",
    "(D)V");
  mid_doublevalue = util::getJMethodID(env, doubleklass.get(), "doubleValue",
    "()D");

}

namespace {

class stringchars {

 public:

  stringchars(JNIEnv *e, ase_jni_script_engine *sc, jstring s)
    : env(e), str(s), cs(env->GetStringChars(s, 0)) {
    if (!cs) {
      util::handleJavaException(env, sc, "stringchars");
    }
  }
  ~stringchars() /* DCHK */ {
    env->ReleaseStringChars(str, cs);
  }
  const jchar *get() const { return cs; }

 private:

  JNIEnv *env;
  jstring str;
  const jchar *cs;

 private:

  /* noncopyable */
  stringchars(const stringchars& x);
  stringchars& operator =(const stringchars& x);

};

const jchar *
to_jchar(const ase_ucchar *s)
{
  return reinterpret_cast<const jchar *>(s);
}

const ase_ucchar *
to_ucchar(const jchar *s)
{
  return reinterpret_cast<const ase_ucchar *>(s);
}



}; // anonymous namespace

ase_string
util::to_ase_string(JNIEnv *env, ase_jni_script_engine *sc, jstring str)
{
  if (!str) {
    return ase_string();
  }
  stringchars cs(env, sc, str);
  jsize len = env->GetStringLength(str);
  return ase_string(to_ucchar(cs.get()), ase_numeric_cast<ase_size_type>(len));
}

jstring
util::to_java_string(JNIEnv *env, const char *str, ase_size_type len)
{
  /* we don't use NewStringUTF because Java's utf8 is not genuine. */
  ase_utf16_string s(str, len, ase_utf16_string::permissive());
  DBG_STR(fprintf(stderr, "%s\n", ase_string(s.data(), s.size()).data()));
  return env->NewString(to_jchar(s.data()), ase_numeric_cast<jsize>(s.size()));
}

jstring
util::to_java_string(JNIEnv *env, const ase_string& str)
{
  return to_java_string(env, str.data(), str.size());
}

ase_variant
util::to_ase_variant_from_jobject(JNIEnv *env, ase_jni_script_engine *sc,
  jobject jv)
{
  if (!jv) {
    return ase_variant();
  }
  util::localref<jclass> dynkl(env, sc, static_cast<jclass>(
    env->CallObjectMethod(jv, sc->getMIDGetClass())));
  ase_jni_classinfo *dyn_ci = sc->getClassMap().find_type_synchronized(env,
    sc, dynkl);
  return dyn_ci->tinfo_unbox->to_ase_boxed(env, sc, jv, dyn_ci);
}

void
util::to_jobject_from_ase_variant(JNIEnv *env, ase_jni_script_engine *sc,
  const ase_variant& val, util::jobjectref& jvref_r)
{
  ase_vtype vt = val.get_type();
  const ase_jni_typeinfo *ti = 0;
  switch (vt) {
  case ase_vtype_void:
    jvref_r.set_rawref(0);
    return;
  case ase_vtype_bool:
    ti = &ase_jni_typeinfo_boolean;
    break;
  case ase_vtype_int:
    ti = &ase_jni_typeinfo_int;
    break;
  case ase_vtype_long:
    ti = &ase_jni_typeinfo_long;
    break;
  case ase_vtype_double:
    ti = &ase_jni_typeinfo_double;
    break;
  case ase_vtype_string:
    ti = &ase_jni_typeinfo_string;
    break;
  case ase_vtype_object:
    ti = &ase_jni_typeinfo_object;
    break;
  }
  bool is_localref = false;
  jobject obj = ti->to_java_boxed(env, sc, val, is_localref);
  if (is_localref) {
    jvref_r.set_localref(obj);
  } else {
    jvref_r.set_rawref(obj);
  }
}

void
util::jthrow_outofmemory(JNIEnv *env)
{
  jclass kl = env->FindClass("java/lang/OutOfMemoryError");
    /* because this function is called only from native callback
      functions, we have no need to delete local references */
  if (!kl) {
    return;
  }
  env->ThrowNew(kl, "ASE OutOfMemory");
}

void
util::jthrow_unknownerror(JNIEnv *env, const char *mess)
{
  jclass kl = env->FindClass("java/lang/RuntimeException");
  if (!kl) {
    return;
  }
  env->ThrowNew(kl, mess);
}

void
util::handleCppException(JNIEnv *env,
  ase_jni_script_engine *sc /* nullable */)
{
  try {
    ase_variant ex = ase_variant::create_from_active_exception();
    if (!sc) {
      jthrow_unknownerror(env, "HandleCPPException: no context");
      return;
    }
    /* convert ex to java */
    DBG_CPPEXC(fprintf(stderr, "handleCppException: %s\n",
      ex.get_string().data()));
    jobjectref jvref(env);
    to_jobject_from_ase_variant(env, sc, ex, jvref);
    jvalue args[1];
    args[0].l = jvref.get();
    jobject obj = 0;
    obj = env->NewObjectA(sc->getAExcKlass(), sc->getMIDAExcConstr(), args);
    if (!obj) {
      return;
    }
    env->Throw(static_cast<jthrowable>(obj));
  } catch (const std::bad_alloc&) {
    jthrow_outofmemory(env);
  } catch (...) {
    jthrow_unknownerror(env, "failed to convert to ase exception");
  }
}

void
util::handleSimpleJavaException(JNIEnv *env, const char *mess)
{
  handleJavaException(env, 0, mess);
}

void
util::handleJavaException(JNIEnv *env,
  ase_jni_script_engine *sc /* nullable */, const char *mess)
{
  if (sc != 0 && !sc->isConstructComplete()) {
    sc = 0;
  }
  DBG_EXC(fprintf(stderr, "java exception\n"));
  DBG_EXC_ABORT(::abort());
  jthrowable t = env->ExceptionOccurred();
  if (!t) {
    /* no java exception is thrown */
    ase_string s = "JNI: no java exception: " + ase_string(mess);
    ase_throw_error(s.data());
  }
  util::localref<jthrowable> tref(env, sc, t, no_exception_check());
  DBG_EXC(env->ExceptionDescribe());
  env->ExceptionClear();
  if (!sc) {
    /* sc is not initialized yet */
    ase_string s = "JNI: unhandled java exception: " + ase_string(mess);
    ase_throw_error(s.data());
  }
  /* convert t to ase_variant */
  const ase_variant av = to_ase_variant_from_jobject(env, sc, t);
  /* if t is an ASEException, unwrap it */
  if (env->IsInstanceOf(t, sc->getAExcKlass())) {
    util::localref<jobject> wrapped(env, sc, env->CallObjectMethod(t,
      sc->getMIDGetWrapped()));
    env->CallObjectMethod(t, sc->getMIDInitWrapped());
    const ase_variant ae = to_ase_variant_from_jobject(env, sc,
      wrapped.get());
    ase_exception_append_trace(ae, av);
    throw ae;
  } else {
    util::localref<jclass> tkl(env, sc, static_cast<jclass>(
      env->CallObjectMethod(t, sc->getMIDGetClass())));
    util::localref<jstring> nstr(env, sc,
      static_cast<jstring>(env->CallObjectMethod(tkl.get(),
	sc->getMIDGetName())));
    ase_string kname = util::to_ase_string(env, sc, nstr.get());
    const ase_variant ae = ase_convert_to_ase_exception(
      ase_string("Java/"), kname, ase_string(), av);
    throw ae;
  }
}

};  // namespace asejni

