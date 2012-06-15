
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include "asejni_util.hpp"
#include "asejni_sctx.hpp"
#include "asejni_jobjpr.hpp"
#include "asejni_priv.hpp"
#include "asejni_classinfo.hpp"
#include "asejni_jcl.hpp"
#include "asejni_impl.hpp"

#include <ase/asedelayvar.hpp>
#include <ase/asestrmap.hpp>
#include <ase/asefuncobj.hpp>
#include <ase/aseio.hpp>

#define DBG_JNI(x)
#define DBG_IMPORT(x)
#define DBG_INIT(x)
#define DBG_CLASS(x)
#define DBG_GLO(x)
#define DBG_EXIT(x)

namespace asejni {

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4355)
#endif

ase_jni_script_engine::ase_jni_script_engine(ase_unified_engine_ref& ue,
  ase_jni_engine_factory *ef, JavaVM *vm, util::javastaticdata *sd)
  : ueref(ue), ef_backref(ef), jvm(vm), sdata(sd),
    jobj_impl_priv(this),
    variant_vtable_for_jobject(
      ase_variant_impl<ase_jni_variant_impl_jobject>::vtable),
    jlobject_classinfo(0), jlclass_classinfo(0), construct_complete(false)
{
  try {
    init();
  } catch (const std::bad_alloc&) {
    cleanup();
    throw;
  } catch (...) {
    cleanup();
    /* we must not rethrow the thrown object because it may refer to
       'this'. */
    ase_throw_error("JNI: failed to initialized script engine");
  }
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

ase_jni_script_engine::~ase_jni_script_engine() /* DCHK */
{
  DBG_EXIT(fprintf(stderr, "jni destr begin\n"));
  cleanup();
  DBG_EXIT(fprintf(stderr, "jni destr end\n"));
}

void
ase_jni_script_engine::cleanup()
{
  dispose_proxies();
  #if defined(_DEBUG) || defined(DEBUG)
  force_gc();
  #endif
}

void
ase_jni_script_engine::init()
{
  variant_vtable_for_jobject.impl_private_data = &jobj_impl_priv;
  variant_vtable_for_jobject.vtopt = ase_vtoption_multithreaded;

  JNIEnv *env = getEnv();

  /* get system class loader */
  util::localref<jobject> scldr(env, 0, env->CallStaticObjectMethod(
    getCLdrKlass(), getMIDGetSystemClassLoader()));
  syscldr.set(jvm, env, scldr);

  /* create global objects */
  jobj_global = ase_jni_jclasses_new(this);
  util::register_native_jnativeproxy(env, getJNPRXKlass());

  jniproxy_endnode.reset(new ase_jni_jniproxy_priv(this));

  classmap.register_primitive_classinfo(env, this, "void",
    &ase_jni_typeinfo_void, getVoidKlass());
  classmap.register_primitive_classinfo(env, this, "boolean",
    &ase_jni_typeinfo_boolean, getBoolKlass());
  classmap.register_primitive_classinfo(env, this, "byte",
    &ase_jni_typeinfo_byte, getByteKlass());
  classmap.register_primitive_classinfo(env, this, "char",
    &ase_jni_typeinfo_char, getCharKlass());
  classmap.register_primitive_classinfo(env, this, "short",
    &ase_jni_typeinfo_short, getShortKlass());
  classmap.register_primitive_classinfo(env, this, "int",
    &ase_jni_typeinfo_int, getIntKlass());
  classmap.register_primitive_classinfo(env, this, "long",
    &ase_jni_typeinfo_long, getLongKlass());
  classmap.register_primitive_classinfo(env, this, "float",
    &ase_jni_typeinfo_float, getFloatKlass());
  classmap.register_primitive_classinfo(env, this, "double",
    &ase_jni_typeinfo_double, getDoubleKlass());
  classmap.register_primitive_classinfo(env, this, "java.lang.String",
    &ase_jni_typeinfo_string, getStringKlass());

  {
    util::localref<jclass> klassk(env, 0, util::findClass(env,
      "java/lang/Class"));
    jlclass_classinfo = classmap.find_type_synchronized(env, this, klassk);
    /* we must initialize j.l.Class first, or j.l.Object class metamethods
      can not be initialized. */
    util::localref<jclass> objectk(env, 0, util::findClass(env,
      "java/lang/Object"));
    jlobject_classinfo = classmap.find_type_synchronized(env, this, objectk);
  }

  globals_var_mod = ase_new_string_map();
  globals_var = ase_string_map_unmodifiable(globals_var_mod);
  globals_var_mod.set("Java", jobj_global);
  {
    /* subengines */
    util::ase_jni_engines::size_type i;
    for (i = 0; i < sdata->engines.size(); ++i) {
      const std::string& name = sdata->engines[i].globalname;
      ase_variant av = ase_new_delay_load_variant(ueref, get_real_global_cb,
	name, this);
      globals[name] = av;
      globals_var_mod.set_property(name.data(), name.size(), av);
    }
  }
  {
    globals_type::size_type n = globals.size();
    global_names.resize(2 + n);
    global_names[0] = "Java";
    globals_type::size_type i;
    globals_type::const_iterator iter;
    for (i = 0, iter = globals.begin(); i < n; ++i, ++iter) {
      global_names[i + 1] = iter->first.c_str();
    }
    global_names[n + 1] = 0;
  }

  construct_complete = true;

  DBG_INIT(fprintf(stderr, "jni init done\n"));
}

ase_variant
ase_jni_script_engine::get_real_global(const ase_string& name)
{
  ase_jni_script_engine *safethis = construct_complete ? this : 0;
  JNIEnv *env = getEnv();
  ase_string klassname = "asejni.engines." + name;
  util::localref<jstring> nstr(env, safethis, util::to_java_string(env,
    klassname));
  jmethodID forname = getMIDForName();
  jvalue args[3];
  args[0].l = nstr.get();
  args[1].z = JNI_TRUE;
  args[2].l = getSystemClassLoader();
  util::localref<jclass> ncl(env, safethis, static_cast<jclass>(
    env->CallStaticObjectMethodA(getClassKlass(), forname, args)));
  ase_jni_classinfo *ci = classmap.find_type_synchronized(env, this, ncl);
  util::localref<jobject> nobj(env, safethis,
    env->CallObjectMethod(ncl.get(), getMIDNewInstance()));
  const bool examine_dyntype = false;
  ase_variant av = ase_jni_jobject_new(env, 0, ci, nobj.get(),
    examine_dyntype);
  /* import external globals */
  av.set("ASE", ase_global);
  /* import 'Java' */
  av.set("Java", jobj_global);
  /* import other java subengines */
  globals_type::const_iterator iter;
  for (iter = globals.begin(); iter != globals.end(); ++iter) {
    if (iter->first == name) {
      continue;
    }
    av.set_property(ase_string(iter->first), iter->second);
  }
  ASE_JNI_VERBOSE_SUB(fprintf(stderr, "Java Engine %s loaded\n",
    name.data()));
  return av;
}

ase_variant
ase_jni_script_engine::get_real_global_cb(const ase_string& name,
  void *userdata)
{
  ase_jni_script_engine *engine =
    static_cast<ase_jni_script_engine *>(userdata);
  return engine->get_real_global(name);
}

void
ase_jni_script_engine::child_thread_initialized()
{
  ef_backref->child_thread_initialized();
}

void
ase_jni_script_engine::child_thread_terminated()
{
  ef_backref->child_thread_terminated();
}

void
ase_jni_script_engine::wait_threads()
{
  sdata->mainthread_env->CallStaticIntMethod(getUtilKlass(),
    getMIDUJoinAll());
  util::checkJavaException(sdata->mainthread_env, this, "joinall");
}

struct dispose_variant {
  void operator ()(ase_synchronized<ase_variant>& value) {
    ase_variant v;
    value.swap(v);
    /* v is released here */
  }
};

void
ase_jni_script_engine::force_gc()
{
  DBG_EXIT(fprintf(stderr, "JNI: gc begin\n"));
  JNIEnv *env = getEnv();
  env->CallStaticVoidMethod(getSystemKlass(), getMIDGC());
  util::checkJavaException(sdata->mainthread_env, this, "gc");
  DBG_EXIT(fprintf(stderr, "JNI: gc end\n"));
}

void
ase_jni_script_engine::dispose_proxies()
{
  DBG_EXIT(fprintf(stderr, "JNI: dispose proxies begin\n"));
  if (jniproxy_endnode.get()) {
    ase_mutex_guard<ase_fast_mutex> g(jniproxy_endnode->get_shared_mutex());
    jniproxy_endnode->for_each_unlocked(dispose_variant());
  }
  ase_global = ase_variant();
  globals_var_mod = ase_variant();
  globals_var = ase_variant();
  DBG_EXIT(fprintf(stderr, "JNI: dispose proxies end\n"));
}

const char *const *
ase_jni_script_engine::get_global_names()
{
  for (global_names_type::size_type i = 0; global_names[i]; ++i) {
    DBG_GLO(fprintf(stderr, "JNI: global %s\n", global_names[i]));
  }
  return &global_names[0];
}

ase_variant
ase_jni_script_engine::get_global_object(const char *name)
{
  std::string n(name);
  if (n == "Java") {
    return jobj_global;
  } else {
    globals_type::const_iterator iter = globals.find(n);
    if (iter != globals.end()) {
      return iter->second;
    }
  }
  ase_throw_invalid_arg("JNI: libase-scr-??.jar not found");
  return ase_variant();
}

void
ase_jni_script_engine::import_ase_global(const ase_variant& val)
{
  {
    DBG_IMPORT(fprintf(stderr, "jni import_ase_global %d %p\n",
      val.get_type(), val.get_rep().p));
    ase_global = val;
    globals_var_mod.set("ASE", ase_global);
  }
}

ase_variant
ase_jni_script_engine::load_file(const char *glname, const char *filename)
{
  ase_variant gl = get_global_object(glname);
  ase_variant func, arg;
  func = gl.get("ExecuteFile");
  if (!func.is_void()) {
    arg = ase_string(filename).to_variant();
  } else {
    func = gl.get("EvalString");
    arg = ase_load_file(ase_string(filename)).to_variant();
  }
  return func.invoke(arg);
}

ase_script_engine *
ase_jni_script_engine::get_real_engine()
{
  return this;
}

}; // namespace asejni

