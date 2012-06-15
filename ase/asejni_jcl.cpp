
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asebindmethod.hpp>
#include <ase/asearray.hpp>
#include <ase/aseio.hpp>
#include <ase/aseintermed.hpp>

#include <algorithm>

#include "asejni_jcl.hpp"
#include "asejni_jobjpr.hpp"
#include "asejni_classinfo.hpp"

#define DBG(x)
#define DBG_EXEC(x)

namespace asejni {

struct ase_jni_variant_impl_jclasses : public ase_variant_impl_default {

  ase_jni_variant_impl_jclasses(ase_jni_script_engine *sc)
    : refcount(1), script_engine(sc) {
  }

  static ase_jni_variant_impl_jclasses *get(const ase_variant& v) {
    return static_cast<ase_jni_variant_impl_jclasses*>(v.get_rep().p);
  }
  static ase_jni_script_engine *getContext(const ase_variant& v);

  static void add_ref(const ase_variant& v) {
    ase_jni_variant_impl_jclasses *const impl = get(v);
    impl->refcount++;
  }
  static void release(const ase_variant& v) {
    ase_jni_variant_impl_jclasses *const impl = get(v);
    if ((impl->refcount--) == 1) {
      ase_variant::debug_on_destroy_object(v, "JNIJCLass");
      delete impl;
    }
  }

  static ase_string get_string(const ase_variant& v) {
    return ase_string("[JavaClasses]");
  }


  static ase_variant get_property(const ase_variant& v, const char *name,
    ase_size_type namelen) {
    bool is_latebind = false;
    return get_property_internal(v, name, namelen, is_latebind);
  }

  static ase_variant get_property_internal(const ase_variant& v,
    const char *name, ase_size_type namelen, bool& is_latebind) {
    DBG(fprintf(stderr, "jcl getprop name=%s\n",
      ase_string(name, namelen).data()));
    if (ase_string::equals(name, namelen, "ExecuteFile")) {
      return ase_new_variant_bind_method_name(v, ase_string(name, namelen));
    }
    ase_jni_variant_impl_jclasses *const impl = get(v);
    ase_jni_script_engine *const sc = impl->script_engine;
    JNIEnv *env = sc->getEnv();
    jclass utkl = sc->getUtilKlass();
    util::localref<jstring> nstr(env, sc, util::to_java_string(env, name,
      namelen));
    jmethodID loadclass = sc->getMIDLoadClass();
    jvalue args[2];
    args[0].l = nstr.get();
    args[1].l = sc->getSystemClassLoader();
    util::localref<jclass> ncl(env, sc, static_cast<jclass>(
      env->CallStaticObjectMethodA(utkl, loadclass, args)));
    DBG(fprintf(stderr, "jcl getprop ncl=%p\n", ncl.get()));
    if (ncl.get() == 0) {
      /* intermediate */
      return ase_new_intermediate_property(v, ase_string(name, namelen) + ".");
    }
    ase_jni_classinfo *ci = sc->getClassMap().find_type_synchronized(env,
      sc, ncl);
    is_latebind = ci->is_latebind;
    const bool examine_dyntype = false;
    return ase_jni_jobject_new(env, sc, ci, 0, examine_dyntype);
  }

  static ase_variant load_class_current_dir(const ase_variant& v,
    const char *name, ase_size_type namelen, bool& is_latebind) {
    DBG(fprintf(stderr, "jcl load_class_current_dir name=%s\n",
      ase_string(name, namelen).data()));
    ase_jni_variant_impl_jclasses *const impl = get(v);
    ase_jni_script_engine *const sc = impl->script_engine;
    JNIEnv *env = sc->getEnv();
    jclass utkl = sc->getUtilKlass();
    util::localref<jstring> nstr(env, sc, util::to_java_string(env, name,
      namelen));
    jmethodID loadclasscur = sc->getMIDLoadClassCurrentDir();
    jvalue args[2];
    args[0].l = nstr.get();
    args[1].l = sc->getSystemClassLoader();
    util::localref<jclass> ncl(env, sc, static_cast<jclass>(
      env->CallStaticObjectMethodA(utkl, loadclasscur, args)));
    DBG(fprintf(stderr, "jcl load_class_current_dir ncl=%p\n", ncl.get()));
    if (ncl.get() == 0) {
      ase_string s = "JNI: class not found: " + ase_string(name, namelen);
      ase_throw_invalid_arg(s.data());
    }
    ase_jni_classinfo *ci = sc->getClassMap().find_type_synchronized(env,
      sc, ncl);
    is_latebind = ci->is_latebind;
    const bool examine_dyntype = false;
    return ase_jni_jobject_new(env, sc, ci, 0, examine_dyntype);
  }

  template <ase_size_type n> static bool match_suffix(const std::string& str,
    const char (& x)[n]) {
    ase_size_type len = n - 1;
    if (str.size() > len && str.substr(str.size() - len) == x) {
      return true;
    }
    return false;
  }

  static void compile_file(ase_jni_script_engine *sc, JNIEnv *env,
    const std::string& src, const std::string& kfile) {
    if (ase_newer_than(kfile, src)) {
      return;
    }
    jmethodID cmp = sc->getMIDCompile();
    util::localref<jstring> str(env, sc, util::to_java_string(env, src.data(),
      src.size()));
    jvalue args[1];
    args[0].l = str.get();
    env->CallStaticVoidMethodA(sc->getCmpKlass(), cmp, args);
    util::checkJavaException(env, sc, "compile");
  }

  static ase_variant invoke_by_name(const ase_variant& v, const char *name,
    ase_size_type namelen, const ase_variant *args, ase_size_type nargs) {
    if (ase_string::equals(name, namelen, "ExecuteFile")) {
      if (nargs < 1) {
	ase_throw_missing_arg("Usage Java.ExecuteFile(\"CLASSNAME.class\")");
      }
      ase_jni_variant_impl_jclasses *const impl = get(v);
      ase_jni_script_engine *const sc = impl->script_engine;
      JNIEnv *env = sc->getEnv();
      ase_string astr = args[0].get_string();
      std::string str(astr.data(), astr.size());
      if (match_suffix(str, ".java")) {
	std::string strc = str.substr(0, str.size() - 5) + ".class";
	compile_file(sc, env, str, strc);
	str = strc;
      }
      if (match_suffix(str, ".class")) {
	str = str.substr(0, str.size() - 6);
      }
      std::replace(str.begin(), str.end(), '/', '.');
      std::replace(str.begin(), str.end(), '\\', '.');
      bool is_latebind = false;
      ase_variant klassobj = load_class_current_dir(v, str.data(), str.size(),
	is_latebind);
      if (is_latebind) {
	ase_variant altargs[2];
	altargs[0] = sc->get_globals_var();
	altargs[1] = ase_new_array();
	for (ase_size_type i = 1; i < nargs; ++i) {
	  altargs[1].set(static_cast<ase_int>(i - 1), args[i]);
	}
	DBG_EXEC(fprintf(stderr, "jc exec %s\n", name));
	return klassobj.invoke_by_name("main", 4, altargs, 2);
      } else {
	ase_int altnargs = static_cast<ase_int>(nargs - 1);
	ase_variant altargs =
	  v.get("java.lang.String").minvoke("NewArray", altnargs);
	for (ase_size_type i = 1; i < nargs; ++i) {
	  altargs.set(static_cast<ase_int>(i - 1),
	    args[i].get_string().to_variant());
	}
	return klassobj.invoke_by_name("main", 4, &altargs, 1);
      }
    }
    return ase_variant();
  }

 private:

  mutable ase_atomic_count refcount;
  ase_jni_script_engine *const script_engine;

  /* noncopyable */
  ase_jni_variant_impl_jclasses(const ase_jni_variant_impl_jclasses& x);
  ase_jni_variant_impl_jclasses& operator =(const
    ase_jni_variant_impl_jclasses& x);


};

ase_variant ase_jni_jclasses_new(ase_jni_script_engine *sc)
{
  return ase_variant::create_object(
    &ase_variant_impl<ase_jni_variant_impl_jclasses>::vtable,
    new ase_jni_variant_impl_jclasses(sc),
    "JNIJClass");
}

}; // namespace asejni

