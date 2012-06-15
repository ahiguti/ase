
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEJNI_UTIL_HPP
#define GENTYPE_ASEJNI_UTIL_HPP

#include <ase/aseruntime.hpp>
#include <ase/aselist.hpp>
#include <ase/aseatomic.hpp>
#include <ase/aseexcept.hpp>

#include "asejni.hpp"

#include <cassert>
#include <stdexcept>
#include <memory>
#include <vector>
#include <map>

#include <jni.h>

#define ASE_JNI_VERBOSE_JNI(x) if ((asejni::util::verbose &   1) != 0) { x; };
#define ASE_JNI_VERBOSE_CLS(x) if ((asejni::util::verbose &   2) != 0) { x; };
#define ASE_JNI_VERBOSE_GC(x)  if ((asejni::util::verbose &   4) != 0) { x; };
#define ASE_JNI_VERBOSE_SUB(x) if ((asejni::util::verbose &   8) != 0) { x; };
#define ASE_JNI_VERBOSE_REF(x) if ((asejni::util::verbose &  16) != 0) { x; };
#define ASE_JNI_VERBOSE_JAR(x) if ((asejni::util::verbose &  32) != 0) { x; };
#define ASE_JNI_VERBOSE_OPT(x) if ((asejni::util::verbose &  64) != 0) { x; };

namespace asejni {

struct ase_jni_script_engine;
struct ase_jni_jniproxy_priv;
struct ase_jni_classinfo;
struct ase_jni_typeinfo;
struct ase_jni_funcinfo;

struct util {

  static unsigned int verbose;

  static JNIEnv *getEnv(JavaVM *jvm);

  template <typename T> class ptrvector : public std::vector<T> {

   public:

    typedef typename std::vector<T>::size_type size_type;

    ptrvector() { }
    ~ptrvector() /* DCHK */ {
      for (size_type i = 0; i < this->size(); ++i) {
	delete (*this)[i];
      }
    }

   private:

    /* noncopyable */
    ptrvector(const ptrvector& x);
    ptrvector& operator =(const ptrvector& x);

  };

  template <typename Tk, typename Tv, typename Tc = std::less<Tk> >
  class ptrmap :
    public std::map<Tk, Tv, Tc> {

   public:

    typedef typename std::map<Tk, Tv, Tc>::iterator iterator;

    ptrmap() { }
    ~ptrmap() /* DCHK */ {
      for (iterator i = this->begin(); i != this->end(); ++i) {
	delete i->second;
      }
    }

   private:

    /* noncopyable */
    ptrmap(const ptrmap& x);
    ptrmap& operator =(const ptrmap& x);

  };

  struct no_exception_check { };

  template <typename T> class localref {

   public:

    localref(JNIEnv *e, ase_jni_script_engine *sc, T v, const char *mess = 0)
      : env(e), value(v) {
      if (value == 0 && env->ExceptionCheck() /* JNI 1.2 */) {
	handleJavaException(env, sc, mess ? mess : "JNI null localref");
      }
    }
    localref(JNIEnv *e, ase_jni_script_engine *sc, T v,
      const no_exception_check& x) : env(e), value(v) {
    }
    ~localref() /* DCHK */ {
      if (value) {
	env->DeleteLocalRef(value);
      }
    }
    T get() const { return value; }
    T release() {
      T r = value;
      value = 0;
      return r;
    }

   private:

    JNIEnv *env;
    T value;

   private:
   
    /* noncopyable */
    localref(const localref& x);
    localref& operator =(const localref& x);

  };

  template <typename T> class globalref : public ase_noncopyable {

   public:

    globalref() : jvm(0), value(0) { }
    ~globalref() /* DCHK */ {
      if (value) {
	ASE_JNI_VERBOSE_REF(fprintf(stderr, "JNIGR delete %p\n", value));
	getEnv(jvm)->DeleteGlobalRef(value);
      }
    }
    void check_valid() {
      if (value != 0) {
	ASE_JNI_VERBOSE_REF(throw std::logic_error("globalref already set"));
	  /* ASE THROW */
      }
    }
    void set(JavaVM *vm, JNIEnv *e, const localref<T>& obj) {
      check_valid();
      T v = static_cast<T>(newGlobalRef(e, obj.get()));
      value = v;
      jvm = vm;
      ASE_JNI_VERBOSE_REF(fprintf(stderr, "JNIGR new %p\n", value));
    }
    T get() const { return value; }
    T release() {
      T r = value;
      value = 0;
      return r;
    }

   private:

    JavaVM *jvm;
    T value;

  };

  class jobjectref {

   public:

    jobjectref(JNIEnv *e) : env(e), value(0), need_del_localref(false) { }
    ~jobjectref() /* DCHK */ {
      if (value) {
	if (need_del_localref) {
	  env->DeleteLocalRef(value);
	}
      }
    }
    jobject get() const { return value; }
    jobject release() {
      jobject r = value;
      value = 0;
      return r;
    }
    jobject to_localref_and_release(ase_jni_script_engine *sc) {
      jobject r = value;
      value = 0;
      if (r != 0 && !need_del_localref) {
	r = env->NewLocalRef(r);
	if (r == 0 && env->ExceptionCheck() /* JNI 1.2 */) {
	  handleJavaException(env, sc, "JNI jobjectref");
	}
      }
      return r;
    }
    void set_rawref(jobject v) {
      if (need_del_localref) {
	throw std::logic_error("jobjectref reftype!=0"); /* ASE THROW */
      }
      value = v;
    }
    void set_localref(jobject v) {
      if (need_del_localref) {
	throw std::logic_error("jobjectref reftype!=0"); /* ASE THROW */
      }
      need_del_localref = true;
      value = v;
      if (!value) {
	handleSimpleJavaException(env, "JNI null localref");
      }
    }

   private:

    JNIEnv *env;
    jobject value;
    bool need_del_localref;

   private:

    /* noncopyable */
    jobjectref(const jobjectref& x);
    jobjectref& operator =(const jobjectref& x);

  };

  class jvalueref {

   public:

    jvalueref(JNIEnv *e) : env(e), need_del_localref(false) { }
    ~jvalueref() /* DCHK */ {
      if (need_del_localref) {
	if (value.l) {
	  env->DeleteLocalRef(value.l);
	}
      }
    }
    const jvalue& get() const { return value; }
    jvalue release() {
      jvalue r = value;
      value.l = 0;
      return r;
    }
    void check_valid() {
      if (need_del_localref) {
	ASE_JNI_VERBOSE_REF(throw std::logic_error("jvalueref reftype!=0"));
	  /* ASE THROW */
      }
    }
    void set_boolean(jboolean v) {
      check_valid();
      value.z = v;
    }
    void set_byte(jbyte v) {
      check_valid();
      value.b = v;
    }
    void set_char(jchar v) {
      check_valid();
      value.c = v;
    }
    void set_short(jshort v) {
      check_valid();
      value.s = v;
    }
    void set_int(jint v) {
      check_valid();
      value.i = v;
    }
    void set_long(jlong v) {
      check_valid();
      value.j = v;
    }
    void set_float(jfloat v) {
      check_valid();
      value.f = v;
    }
    void set_double(jdouble v) {
      check_valid();
      value.d = v;
    }
    void set_object_rawref(jobject v) {
      check_valid();
      value.l = v;
    }
    void set_object_localref(jobject v) {
      check_valid();
      need_del_localref = true;
      value.l = v;
      if (!v) {
	handleSimpleJavaException(env, "JNI null localref");
      }
    }

   private:

    JNIEnv *env;
    jvalue value;
    bool need_del_localref;

   private:

    /* noncopyable */
    jvalueref(const jvalueref& x);
    jvalueref& operator =(const jvalueref& x);

  };

  struct ase_jni_engine {
    std::string globalname;
    std::string depjar;
  };
  typedef std::vector<ase_jni_engine> ase_jni_engines;

  class javastaticdata {

   public:

    javastaticdata(JavaVM *vm, JNIEnv *env, const ase_jni_engines& eng);

   public:

    ase_jni_engines engines;
    JNIEnv *mainthread_env; /* TODO: ARGUABLE */
    util::globalref<jclass> objectklass; /* java.lang.Object */
    util::globalref<jclass> classklass;  /* java.lang.Class */
    util::globalref<jclass> cldrklass;   /* java.lang.ClassLoader */
    util::globalref<jclass> systemklass; /* java.lang.System */
    util::globalref<jclass> constrklass; /* java.lang.reflect.Constructor */
    util::globalref<jclass> methodklass; /* java.lang.reflect.Method */
    util::globalref<jclass> fieldklass;  /* java.lang.reflect.Field */
    util::globalref<jclass> modklass;    /* java.lang.reflect.Modifier */
    util::globalref<jclass> arrayklass;  /* java.lang.reflect.Array */
    util::globalref<jclass> rprxklass;   /* java.lang.reflect.Proxy */
    util::globalref<jclass> jnprxklass;  /* asejni.NativeProxy */
    util::globalref<jclass> jlateklass;  /* asejni.LateBinding */
    util::globalref<jclass> aexcklass;   /* asejni.ASEException */
    util::globalref<jclass> cmpklass;    /* asejni.Compile */
    util::globalref<jclass> utilklass;   /* asejni.Util */
    util::globalref<jclass> voidklass;   /* java.lang.Void */
    util::globalref<jclass> boolklass;   /* java.lang.Boolean */
    util::globalref<jclass> byteklass;   /* java.lang.Byte */
    util::globalref<jclass> charklass;   /* java.lang.Character */
    util::globalref<jclass> shortklass;  /* java.lang.Short */
    util::globalref<jclass> intklass;    /* java.lang.Integer */
    util::globalref<jclass> longklass;   /* java.lang.Long */
    util::globalref<jclass> floatklass;  /* java.lang.Float */
    util::globalref<jclass> doubleklass; /* java.lang.Double */
    util::globalref<jclass> stringklass; /* java.lang.String */
    jmethodID mid_getclass; /* Object.getClass() */
    jmethodID mid_equals; /* Object.equals() */
    jmethodID mid_forname; /* Class.forName() */
    jmethodID mid_isprimitive; /* Class.isPrimitive() */
    jmethodID mid_getname; /* Class.getName() */
    jmethodID mid_getconstructors; /* Class.getConstructors() */
    jmethodID mid_getmethods; /* Class.get_methods() */
    jmethodID mid_getfields; /* Class.getFields() */
    jmethodID mid_isarray; /* Class.isArray() */
    jmethodID mid_getcomponenttype; /* Class.getComponentType() */
    jmethodID mid_isinterface; /* Class.isInterface() */
    jmethodID mid_newinstance; /* Class.newInstance() */
    jmethodID mid_getsyscldr; /* ClassLoader.getSystemClassLoader() */
    jmethodID mid_gc; /* System.gc() */
    jmethodID mid_cgetparametertypes; /* Constructor.getParameterTypes() */
    jmethodID mid_mgetname; /* Method.getName() */
    jmethodID mid_mgetmodifiers; /* Method.getModifiers() */
    jmethodID mid_mgetreturntype; /* Method.getReturnType() */
    jmethodID mid_mgetparametertypes; /* Method.getParameterTypes() */
    jmethodID mid_fgetname; /* Field.getName() */
    jmethodID mid_fgetmodifiers; /* Field.getModifiers() */
    jmethodID mid_fgettype; /* Field.get_type() */
    jmethodID mid_isstatic; /* Modifier.isStatic() */
    jmethodID mid_anewinstance; /* Array.newInstance() */
    jmethodID mid_getinvocationhandler; /* Proxy.getInvocationHandler() */
    jmethodID mid_isproxyclass; /* Proxy.isProxyClass() */
    jmethodID mid_newproxy; /* NativeProxy.newProxy() */
    jfieldID fid_nprx_priv; /* NativeProxy.priv */
    jmethodID mid_lbgetattributes; /* LateBinding.get_attributes() */
    jmethodID mid_lbgetstring; /* LateBinding.get_string() */
    jmethodID mid_lbgetproperty; /* LateBinding.get_property() */
    jmethodID mid_lbsetproperty; /* LateBinding.set_property() */
    jmethodID mid_lbdelproperty; /* LateBinding.del_property() */
    jmethodID mid_lbgetelement; /* LateBinding.get_element() */
    jmethodID mid_lbsetelement; /* LateBinding.set_element() */
    jmethodID mid_lbgetlength; /* LateBinding.get_length() */
    jmethodID mid_lbsetlength; /* LateBinding.set_length() */
    jmethodID mid_lbinvokebyname; /* LateBinding.invoke_by_name() */
    jmethodID mid_lbinvokebyid; /* LateBinding.invoke_by_id() */
    jmethodID mid_lbinvokeself; /* LateBinding.invoke_self() */
    jmethodID mid_lbgetenumerator; /* LateBinding.get_enumerator() */
    jmethodID mid_lbenumnext; /* LateBinding.enum_next() */
    jmethodID mid_aexcconstr; /* ASEException() */
    jmethodID mid_aexcgetwrapped; /* ASEException.getWrapped() */
    jmethodID mid_aexcinitwrapped; /* ASEException.initWrapped() */
    jmethodID mid_compile; /* Compiler.compile() */
    jmethodID mid_objtostr; /* Util.objectToString() */
    jmethodID mid_ujoinall; /* Util.joinAll() */
    jmethodID mid_loadclass; /* Util.loadClass() */
    jmethodID mid_loadclasscur; /* Util.loadClassCrrentDir() */
    jmethodID mid_bvalueof; /* Boolean.valueOf() */
    jmethodID mid_booleanvalue; /* Boolean.booleanValue() */
    jmethodID mid_byteconstr; /* Byte(byte) */
    jmethodID mid_bytevalue; /* Byte.byteValue() */
    jmethodID mid_charconstr; /* Character(char) */
    jmethodID mid_charvalue; /* Character.charValue() */
    jmethodID mid_shortconstr; /* Short(short) */
    jmethodID mid_shortvalue; /* Short.shortValue() */
    jmethodID mid_intconstr; /* Integer(int) */
    jmethodID mid_intvalue; /* Integer.intValue() */
    jmethodID mid_longconstr; /* Long(long) */
    jmethodID mid_longvalue; /* Long.longValue() */
    jmethodID mid_floatconstr; /* Float(float) */
    jmethodID mid_floatvalue; /* Float.floatValue() */
    jmethodID mid_doubleconstr; /* Double(double) */
    jmethodID mid_doublevalue; /* Double.doubleValue() */

   private:

    JavaVM *jvm;

  };

  static void handleSimpleJavaException(JNIEnv *env, const char *mess);
  static void handleJavaException(JNIEnv *env, ase_jni_script_engine *sc,
    const char *mess);

  static void checkSimpleJavaException(JNIEnv *env, const char *mess) {
    if (env->ExceptionCheck() /* JNI 1.2 */) {
      handleSimpleJavaException(env, mess);
    }
  }
  static void checkJavaException(JNIEnv *env, ase_jni_script_engine *sc,
    const char *mess) {
    if (env->ExceptionCheck() /* JNI 1.2 */) {
      handleJavaException(env, sc, mess);
    }
  }

  static jmethodID getJMethodID(JNIEnv *env, jclass kl, const char *name,
    const char *sig) {
    jmethodID r = env->GetMethodID(kl, name, sig);
    if (!r) {
      std::string mess = std::string("JNI GetMethodID failed name=")
	+ name + " sig=" + sig;
      handleSimpleJavaException(env, mess.c_str());
    }
    return r;
  }
  static jmethodID getStaticMethodId(JNIEnv *env, jclass kl, const char *name,
    const char *sig) {
    jmethodID r = env->GetStaticMethodID(kl, name, sig);
    if (!r) {
      std::string mess = std::string("JNI GetStaticMethodID failed name=")
	+ name + " sig=" + sig;
      handleSimpleJavaException(env, mess.c_str());
    }
    return r;
  }
  static jfieldID getFieldID(JNIEnv *env, jclass kl, const char *name,
    const char *sig) {
    jfieldID r = env->GetFieldID(kl, name, sig);
    if (!r) {
      std::string mess = std::string("JNI GetFieldID failed name=")
	+ name + " sig=" + sig;
      handleSimpleJavaException(env, mess.c_str());
    }
    return r;
  }
  static jobject newObject(JNIEnv *env, jclass kl, jmethodID mid) {
    jobject r = env->NewObject(kl, mid);
    if (!r) {
      handleSimpleJavaException(env, "JNI NewObject failed");
    }
    return r;
  }
  static jclass findClass(JNIEnv *env, const char *name) {
    jclass r = env->FindClass(name);
    if (!r) {
      std::string mess = std::string("JNI FindClass failed name=") + name;
      handleSimpleJavaException(env, mess.c_str());
    }
    return r;
  }
  static jobject newGlobalRef(JNIEnv *env, jobject obj) {
    jobject r = env->NewGlobalRef(obj);
    if (!r) {
      /* null iff outofmem */
      env->ExceptionClear();
      throw std::bad_alloc(); /* ASE THROW */
    }
    return r;
  }
  static ase_string to_ase_string(JNIEnv *env, ase_jni_script_engine *sc,
    jstring str);
  static jstring to_java_string(JNIEnv *env, const char *str,
    ase_size_type len);
  static jstring to_java_string(JNIEnv *env, const ase_string& str);
  static ase_variant to_ase_variant_from_jobject(JNIEnv *env,
    ase_jni_script_engine *sc, jobject jv);
  static void to_jobject_from_ase_variant(JNIEnv *env,
    ase_jni_script_engine *sc, const ase_variant& val, jobjectref& jvref_r);
  static void jthrow_outofmemory(JNIEnv *env);
  static void jthrow_unknownerror(JNIEnv *env, const char *mess);
  static void handleCppException(JNIEnv *env,
    ase_jni_script_engine *sc /* nullable */);

  static void register_native_classdispatchinfo(JNIEnv *env, jclass kl);
  static void register_native_aseproxy(JNIEnv *env, jclass kl);

  static void register_native_jnativeproxy(JNIEnv *env, jclass kl);

};

}; // namespace asejni

#endif

