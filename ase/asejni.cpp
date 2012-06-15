
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/aseio.hpp>
#include <ase/aseflags.hpp>
#include <ase/asedllhandle.hpp>
#include <ase/aseexcept.hpp>

#include "asejni_impl.hpp"

#include <cstdlib>
#include <algorithm>

#ifndef ASE_INSTALLED_JAR_DIR
#define ASE_INSTALLED_JAR_DIR "/usr/share/java"
#endif

#ifndef __GNUC__
#include <windows.h>
#endif


#define DBG_THR(x)

namespace asejni {

namespace {

std::string
ase_jni_get_user_classpath()
{
  ase_string s = ase_getenv_mtunsafe("CLASSPATH");
  return std::string(s.data(), s.size());
}

void
ase_jni_get_javaopts(std::vector<std::string>& opts_append)
{
  typedef std::string::size_type size_type;
  ase_string c = ase_getenv_mtunsafe("JAVA_OPTS");
  if (c.empty()) {
    ASE_JNI_VERBOSE_OPT(fprintf(stderr, "Javaopts notfound\n"));
    return;
  }
  std::string s(c.data(), c.size());
  ASE_JNI_VERBOSE_OPT(fprintf(stderr, "Javaopts [%s]\n", s.c_str()));
  size_type npos = s.npos;
  size_type pos = 0;
  while (true) {
    size_type posd = s.find(' ', pos);
    if (posd != pos) {
      opts_append.push_back(s.substr(pos, posd - pos));
      ASE_JNI_VERBOSE_OPT(fprintf(stderr, "opt [%s]\n",
	opts_append.back().c_str()));
    }
    if (posd == npos) {
      break;
    }
    pos = posd + 1;
  }
}

void
append_path(std::string& pt, const std::string& name)
{
  if (!pt.empty()) {
    pt += ASE_PATH_SEP_STR + name;
  } else {
    pt = name;
  }
}

std::string
ase_jni_find_jarfiles(const char *const *path, util::ase_jni_engines& eng_r)
{
  /* this function need not to be so fast */
  std::string r;
  while (path && *path) {
    std::string dname(*path);
    ase_variant ents;
    ents = ase_read_directory(dname); /* ignore notfound error */
    ase_int nents = ents.get_length();
    for (ase_int i = 0; i < nents; ++i) {
      ase_string e = ents.get(i).get_string();
      const std::string name(e.data(), e.size());
      if (name.size() <= 4 || name.substr(name.size() - 4) != ".jar") {
	continue;
      }
      /* jar file */
      ASE_JNI_VERBOSE_JAR(fprintf(stderr, "found jar %s/%s\n", dname.c_str(),
	name.c_str()));
      std::string bname = name.substr(0, name.size() - 4);
      if (bname == "asejni") {
	append_path(r, dname + ASE_DIR_SEP_STR + name);
      }
      if (bname.size() > 11 && bname.substr(0, 11) == "libase-scr-") {
	append_path(r, dname + ASE_DIR_SEP_STR + name);
	std::string e = bname.substr(11);
	std::string depjar;
	ASE_JNI_VERBOSE_JAR(fprintf(stderr, "found engine %s\n", e.c_str()));
	std::string::size_type h = e.find('-');
	if (h != e.npos) {
	  depjar = e.substr(h + 1);
	  e = e.substr(0, h);
	}
	util::ase_jni_engine eng;
	eng.globalname = e;
	eng.depjar = depjar;
	eng_r.push_back(eng);
	std::string depjarinst = std::string(ASE_INSTALLED_JAR_DIR)
	  + ASE_DIR_SEP_STR + eng.depjar + ".jar";
	std::string depjarloc = dname + ASE_DIR_SEP_STR + eng.depjar + ".jar";
	if (ase_file_exists(depjarinst)) {
	  append_path(r, depjarinst);
	} else if (ase_file_exists(depjarloc)) {
	  append_path(r, depjarloc);
	}
      }
    }
    ++path;
  }
  return r;
}

#ifndef __GNUC__

struct auto_registry_key {
  auto_registry_key(HKEY k) : key(k) {
  }
  ~auto_registry_key() /* DCHK */ {
    RegCloseKey(key);
  }
  const HKEY key;
 private:
  auto_registry_key(const auto_registry_key& x);
  auto_registry_key& operator =(const auto_registry_key& x);
};

std::string
get_reg_str(const HKEY kroot, const char *kname, const char *name)
{
  HKEY hkey = 0;
  DWORD typ = 0;
  DWORD sz = 0;
  if (RegOpenKeyExA(kroot, kname, NULL, KEY_EXECUTE, &hkey)
    == ERROR_SUCCESS) {
    auto_registry_key akey(hkey);
    if (RegQueryValueExA(hkey, name, NULL, &typ, NULL, &sz)
      != ERROR_SUCCESS) {
      return std::string();
    }
    std::vector<char> buf(sz + 1);
    if (RegQueryValueExA(hkey, name, NULL, &typ,
      (BYTE *)&buf[0], &sz) != ERROR_SUCCESS) {
    }
    return std::string(&buf[0], sz);
  }
  return std::string();
}

#endif

ase_string
get_last(const ase_variant& arr)
{
  ase_int len = arr.get_length();
  std::vector<ase_string> vec(len);
  for (int i = 0; i < len; ++i) {
    vec[i] = arr.get(i).get_string();
  }
  std::sort(vec.begin(), vec.end());
  if (len > 0) {
    return vec[len - 1];
  }
  return ase_string();
}

std::string
get_jvmpath()
{
  ase_string c = ase_getenv_mtunsafe("ASEJNI_JVM");
  if (!c.empty()) {
    return std::string(c.data(), c.size());
  }

  #ifdef __GNUC__
  static const char *patterns[] = {
    "/usr/lib/jvm/java/jre/lib/*/server/libjvm.so",
    "/usr/lib/jvm/java/jre/lib/*/client/libjvm.so",
    "/usr/java/default/jre/lib/*/server/libjvm.so",
    "/usr/java/default/jre/lib/*/client/libjvm.so",
    0
  };
  for (const char **pat = patterns; *pat; ++pat) {
    ase_variant arr;
    arr = ase_glob(ase_string(*pat));
    if (arr.get_length() > 0) {
      return std::string(get_last(arr).data());
    }
  }
  #else
  ase_string pattern_pre("C:\\Program Files\\Java");
  ase_variant arr;
  arr = ase_read_directory(pattern_pre);
  std::vector<ase_string> vec(arr.get_length_or_zero());
  for (ase_int i = 0; i < arr.get_length_or_zero(); ++i) {
    vec[i] = arr.get(i).get_string();
  }
  std::sort(vec.rbegin(), vec.rend());
  for (ase_size_type i = 0; i < vec.size(); ++i) {
    ase_string n = vec[i];
    ase_string pstr = pattern_pre + "\\" + n + "\\jre\\bin\\server\\jvm.dll";
    if (ase_file_exists(pstr)) {
      return std::string(pstr.data());
    }
    pstr = pattern_pre + "\\" + n + "\\jre\\bin\\client\\jvm.dll";
    if (ase_file_exists(pstr)) {
      return std::string(pstr.data());
    }
  }
  #endif

  #ifdef _MSC_VER
  std::string kname("Software\\JavaSoft\\Java Runtime Environment");
  std::string curver = get_reg_str(HKEY_LOCAL_MACHINE, kname.c_str(),
    "CurrentVersion");
  if (curver.empty()) {
    ase_throw_error("JNI: JRE not found");
  }
  kname += "\\" + curver;
  std::string dllpstr = get_reg_str(HKEY_LOCAL_MACHINE, kname.c_str(),
    "RuntimeLib");
  if (dllpstr.empty()) {
    ase_throw_error("JNI: JRE not found");
  }
  ASE_JNI_VERBOSE_JNI(fprintf(stderr, "javavm curver=%s\n",
    curver.c_str()));
  ASE_JNI_VERBOSE_JNI(fprintf(stderr, "javavm runtimelib=%s\n",
    dllpstr.c_str()));
  return dllpstr;
  #else
  ase_throw_error("JNI: JRE not found");
  return std::string();
  #endif
}

void
add_option(std::vector<JavaVMOption>& opts, const char *opt)
{
  opts.push_back(JavaVMOption());
  opts.back().optionString = const_cast<char *>(opt);
    /* JNI is not const-aware */
}

}; // anonymous namespace

ase_jni_engine_factory::ase_jni_engine_factory(ase_environment& e)
  : env(e), asejni_jvm_dll(0), asejni_jvm(0), asejni_jvm_preexisting(false),
    asejni_sdata(0)
{
  try {
    init();
  } catch (...) {
    clear();
    throw;
  }
}

ase_jni_engine_factory::~ase_jni_engine_factory() /* DCHK */
{
  clear();
}

void
ase_jni_engine_factory::init()
{
  void *preex = env.get_preexisting_engine("JavaVM");
  if (preex != 0) {
    init_preexisting(preex);
  } else {
    init_own_vm();
  }
}

void
ase_jni_engine_factory::init_own_vm()
{
  ase_init_flags("ASE_JNI_VERBOSE", util::verbose);
  const char *const *path = env.get_path();
  util::ase_jni_engines eng;
  std::string clpath = ase_jni_find_jarfiles(path, eng);
  std::string uclpath = ase_jni_get_user_classpath();
  if (!uclpath.empty()) {
    if (!clpath.empty()) {
      clpath += ASE_PATH_SEP_STR;
    }
    clpath += uclpath;
  }
  ASE_JNI_VERBOSE_JNI(fprintf(stderr, "new jvm classpath=%s\n",
    clpath.c_str()));

  std::string jvmpath = get_jvmpath();
  ASE_JNI_VERBOSE_JNI(fprintf(stderr, "jvmpath=%s\n", jvmpath.c_str()));
  asejni_jvm_dll = new ase_auto_dll_handle(jvmpath, true);
  if (!asejni_jvm_dll->get_error().empty()) {
    ase_string s = "JNI: init: " + asejni_jvm_dll->get_error();
    ase_throw_error(s.data());
  }
  void *sym = asejni_jvm_dll->get_symbol(ase_string("JNI_CreateJavaVM"));
  if (!sym) {
    ase_string s = "JNI: symbol JNI_CreateJavaVM not found in "
      + ase_string(jvmpath.c_str());
    ase_throw_error(s.data());
  }
  typedef jint (JNICALL *jvmini_type)(JavaVM **, void **, void *);
  jvmini_type jvmini = (jvmini_type)(sym);

  std::vector<std::string> java_opts;
  ase_jni_get_javaopts(java_opts);

  JavaVMInitArgs vm_args;
  std::memset(&vm_args, 0, sizeof(vm_args));
  std::vector<JavaVMOption> options;
  std::string c = "-Djava.class.path=" + clpath;
  add_option(options, const_cast<char *>(c.c_str()));
  ASE_JNI_VERBOSE_JNI(add_option(options, "-Xcheck:jni"));
  ASE_JNI_VERBOSE_CLS(add_option(options, "-verbose:class"));
  ASE_JNI_VERBOSE_GC(add_option(options, "-verbose:gc"));
  for (ase_size_type i = 0; i < java_opts.size(); ++i) {
    add_option(options, const_cast<char *>(java_opts[i].c_str()));
  }
  vm_args.version = 0x00010002;
  vm_args.nOptions = static_cast<jint>(options.size());
  vm_args.options = &options[0];
  vm_args.ignoreUnrecognized = JNI_FALSE;
  JNIEnv *env = 0;
  JavaVM *tjvm = 0;
  jint e = (*jvmini)(&tjvm, &reinterpret_cast<void *&>(env), &vm_args);
    /* -fno-strict-aliasing */
  if (e < 0 || env == 0) {
    ase_variant ev(static_cast<ase_int>(e));
    ase_string evs = "failed to create java vm: err=" + ev.get_string();
    ase_throw_error(evs.data());
  }
  asejni_jvm = tjvm;
  asejni_sdata = new util::javastaticdata(asejni_jvm, env, eng);
}

void
ase_jni_engine_factory::init_preexisting(void *preex)
{
  asejni_jvm = static_cast<JavaVM *>(preex);
  util::ase_jni_engines eng;
  JNIEnv *env = util::getEnv(asejni_jvm);
  asejni_sdata = new util::javastaticdata(asejni_jvm, env, eng);
}

void
ase_jni_engine_factory::clear()
{
  if (asejni_sdata != 0) {
    delete asejni_sdata;
  }
  if (!asejni_jvm_preexisting && asejni_jvm != 0) {
    asejni_jvm->DestroyJavaVM();
  }
  if (asejni_jvm_dll != 0) {
    delete asejni_jvm_dll;
  }
  asejni_sdata = 0;
  asejni_jvm_preexisting = false;
  asejni_jvm = 0;
  asejni_jvm_dll = 0;
}

void
ase_jni_engine_factory::child_thread_initialized()
{
}

void
ase_jni_engine_factory::child_thread_terminated()
{
  if (asejni_jvm) {
    DBG_THR(fprintf(stderr, "Detach %p\n", asejni_jvm));
    asejni_jvm->DetachCurrentThread();
  }
}

void
ase_jni_engine_factory::new_script_engine(ase_unified_engine_ref& ue,
  ase_script_engine_ptr& ap_r)
{
  ap_r.reset(new ase_jni_script_engine(ue, this, asejni_jvm, asejni_sdata));
}

}; // namespace asejni

extern "C" void
ASE_NewScriptEngineFactory_Java(ase_environment& env,
  ase_script_engine_factory_ptr& ap_r)
{
  ap_r.reset(new asejni::ase_jni_engine_factory(env));
}

