
#include <ase/aseruntime.hpp>
#include <memory>
#include <jni.h>
#include "asejni_sctx.hpp"

namespace asejni {

/* TODO: don't create runtime if it's called from another runtime */

struct asejni_runtime {

  asejni_runtime(JNIEnv *env);
  jobject get_ase_global(JNIEnv *env);

  std::auto_ptr<ase_runtime> rt;
  std::auto_ptr<ase_unified_engine> eng;
  ase_jni_script_engine *sctx;

};

asejni_runtime::asejni_runtime(JNIEnv *env)
  : sctx(0)
{
  JavaVM *jvm = 0;
  env->GetJavaVM(&jvm);
  try {
    const struct ase_preexisting_engine_info pe = { "JavaVM", jvm };
    rt.reset(new ase_runtime("", 0, 0, &pe, 1));
    eng.reset(new ase_unified_engine(*rt));
    sctx = static_cast<ase_jni_script_engine *>(eng->get_real_engine("Java"));
  } catch (...) {
    util::jthrow_unknownerror(env, "failed to initialize asejni runtime");
  }
  if (sctx == 0) {
    util::jthrow_unknownerror(env, "failed to initialize asejni Java engine");
  }
}

jobject
asejni_runtime::get_ase_global(JNIEnv *env)
{
  try {
    util::jobjectref jobj(env);
    const ase_variant gl = eng->get_ase_global_object();
    util::to_jobject_from_ase_variant(env, sctx, gl, jobj);
    return jobj.release();
  } catch (...) {
    util::handleCppException(env, sctx);
  }
  return 0;
}

};

extern "C" {

JNIEXPORT jlong JNICALL
Java_asejni_Runtime_constructImpl(JNIEnv *env, jclass)
{
  return reinterpret_cast<jlong>(new asejni::asejni_runtime(env));
}

JNIEXPORT jobject JNICALL
Java_asejni_Runtime_getGlobalImpl(JNIEnv *env, jclass, jlong priv)
{
  asejni::asejni_runtime *const p =
    reinterpret_cast<asejni::asejni_runtime *>(priv);
  return p->get_ase_global(env);
}

JNIEXPORT jlong JNICALL
Java_asejni_Runtime_finalizeImpl(JNIEnv *, jclass, jlong priv)
{
  asejni::asejni_runtime *p =
    reinterpret_cast<asejni::asejni_runtime *>(priv);
  delete p;
  p = 0;
  return reinterpret_cast<jlong>(p);
}

};

