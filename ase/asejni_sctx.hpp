
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEJNI_SCTX_HPP
#define GENTYPE_ASEJNI_SCTX_HPP

#include "asejni_util.hpp"
#include "asejni_classinfo.hpp"

namespace asejni {

struct ase_jni_engine_factory;

struct ase_jni_script_engine : public ase_script_engine {

 public:

  typedef std::map<std::string, ase_variant> globals_type;
  typedef std::vector<const char *> global_names_type;

  ase_jni_script_engine(ase_unified_engine_ref& ue, ase_jni_engine_factory *ef,
    JavaVM *vm, util::javastaticdata *sd);
  ~ase_jni_script_engine(); /* DCHK */
  void init();
  void cleanup();

  void destroy() {
    delete this;
  }
  void child_thread_initialized();
  void child_thread_terminated();
  void force_gc();
  void wait_threads();
  void dispose_proxies();
  const char *const *get_global_names();
  ase_variant get_global_object(const char *name);
  void import_ase_global(const ase_variant& val);
  ase_variant load_file(const char *glname, const char *filename);
  ase_script_engine *get_real_engine();

  JavaVM *getJVM() const { return jvm; }
  JNIEnv *getEnv() { return util::getEnv(jvm); }
  jclass getObjectKlass() const { return sdata->objectklass.get(); }
  jclass getClassKlass() const { return sdata->classklass.get(); }
  jclass getCLdrKlass() const { return sdata->cldrklass.get(); }
  jclass getSystemKlass() const { return sdata->systemklass.get(); }
  jclass getConstrKlass() const { return sdata->constrklass.get(); }
  jclass getMethodKlass() const { return sdata->methodklass.get(); }
  jclass getFieldKlass() const { return sdata->fieldklass.get(); }
  jclass getModKlass() const { return sdata->modklass.get(); }
  jclass getArrayKlass() const { return sdata->arrayklass.get(); }
  jclass getRPrxKlass() const { return sdata->rprxklass.get(); }
  jclass getJNPRXKlass() const { return sdata->jnprxklass.get(); }
  jclass getJLateKlass() const { return sdata->jlateklass.get(); }
  jclass getAExcKlass() const { return sdata->aexcklass.get(); }
  jclass getCmpKlass() const { return sdata->cmpklass.get(); }
  jclass getUtilKlass() const { return sdata->utilklass.get(); }
  jclass getVoidKlass() const { return sdata->voidklass.get(); }
  jclass getBoolKlass() const { return sdata->boolklass.get(); }
  jclass getByteKlass() const { return sdata->byteklass.get(); }
  jclass getCharKlass() const { return sdata->charklass.get(); }
  jclass getShortKlass() const { return sdata->shortklass.get(); }
  jclass getIntKlass() const { return sdata->intklass.get(); }
  jclass getLongKlass() const { return sdata->longklass.get(); }
  jclass getFloatKlass() const { return sdata->floatklass.get(); }
  jclass getDoubleKlass() const { return sdata->doubleklass.get(); }
  jclass getStringKlass() const { return sdata->stringklass.get(); }
  /* java.lang.Object */
  jmethodID getMIDGetClass() const { return sdata->mid_getclass; }
  jmethodID getMIDEquals() const { return sdata->mid_equals; }
  /* java.lang.Class */
  jmethodID getMIDForName() const { return sdata->mid_forname; }
  jmethodID getMIDIsPrimitive() const { return sdata->mid_isprimitive; }
  jmethodID getMIDGetName() const { return sdata->mid_getname; }
  jmethodID getMIDGetConstructors() const {
    return sdata->mid_getconstructors; }
  jmethodID getMIDGetMethods() const { return sdata->mid_getmethods; }
  jmethodID getMIDGetFields() const { return sdata->mid_getfields; }
  jmethodID getMIDIsArray() const { return sdata->mid_isarray; }
  jmethodID getMIDGetComponentType() const {
    return sdata->mid_getcomponenttype; }
  jmethodID getMIDIsInterface() const { return sdata->mid_isinterface; }
  jmethodID getMIDNewInstance() const { return sdata->mid_newinstance; }
  /* java.lang.ClassLoader */
  jmethodID getMIDGetSystemClassLoader() const {
    return sdata->mid_getsyscldr; }
  jmethodID getMIDGC() const { return sdata->mid_gc; }
  /* java.lang.reflect.Constructor */
  jmethodID getMIDCGetParameterTypes() const {
    return sdata->mid_cgetparametertypes; }
  /* java.lang.reflect.Method */
  jmethodID getMIDMGetName() const { return sdata->mid_mgetname; }
  jmethodID getMIDMGetModifiers() const { return sdata->mid_mgetmodifiers; }
  jmethodID getMIDMGetReturnType() const { return sdata->mid_mgetreturntype; }
  jmethodID getMIDMGetParameterTypes() const {
    return sdata->mid_mgetparametertypes; }
  /* java.lang.reflect.Field */
  jmethodID getMIDFGetName() const { return sdata->mid_fgetname; }
  jmethodID getMIDFGetModifiers() const { return sdata->mid_fgetmodifiers; }
  jmethodID getMIDFGetType() const { return sdata->mid_fgettype; }
  /* java.lang.reflect.Modifier */
  jmethodID getMIDIsStatic() const { return sdata->mid_isstatic; }
  /* java.lang.reflect.Array */
  jmethodID getMIDANewInstance() const { return sdata->mid_anewinstance; }
  /* java.lang.reflect.Proxy */
  jmethodID getMIDGetInvocationHandler() const {
    return sdata->mid_getinvocationhandler; }
  jmethodID getMIDIsProxyClass() const { return sdata->mid_isproxyclass; }
  /* asejni.NativeProxy */
  jmethodID getMIDNewProxy() const { return sdata->mid_newproxy; }
  jfieldID getFIDNPRXPriv() const { return sdata->fid_nprx_priv; }
  /* asejni.LateBinding */
  jmethodID getMIDLBGetAttributes() const {
    return sdata->mid_lbgetattributes; }
  jmethodID getMIDLBGetString() const { return sdata->mid_lbgetstring; }
  jmethodID getMIDLBGetProperty() const { return sdata->mid_lbgetproperty; }
  jmethodID getMIDLBSetProperty() const { return sdata->mid_lbsetproperty; }
  jmethodID getMIDLBDelProperty() const { return sdata->mid_lbdelproperty; }
  jmethodID getMIDLBGetElement() const { return sdata->mid_lbgetelement; }
  jmethodID getMIDLBSetElement() const { return sdata->mid_lbsetelement; }
  jmethodID getMIDLBGetLength() const { return sdata->mid_lbgetlength; }
  jmethodID getMIDLBSetLength() const { return sdata->mid_lbsetlength; }
  jmethodID getMIDLBInvokeByName() const { return sdata->mid_lbinvokebyname; }
  jmethodID getMIDLBInvokeById() const { return sdata->mid_lbinvokebyid; }
  jmethodID getMIDLBInvokeSelf() const { return sdata->mid_lbinvokeself; }
  jmethodID getMIDLBGetEnumerator() const {
    return sdata->mid_lbgetenumerator; }
  jmethodID getMIDLBEnumNext() const { return sdata->mid_lbenumnext; }
  /* asejni.ASEException */
  jmethodID getMIDAExcConstr() const { return sdata->mid_aexcconstr; }
  jmethodID getMIDGetWrapped() const { return sdata->mid_aexcgetwrapped; }
  jmethodID getMIDInitWrapped() const { return sdata->mid_aexcinitwrapped; }
  /* asejni.Compile */
  jmethodID getMIDCompile() const { return sdata->mid_compile; }
  /* asejni.Util */
  jmethodID getMIDObjToStr() const { return sdata->mid_objtostr; }
  jmethodID getMIDUJoinAll() const { return sdata->mid_ujoinall; }
  jmethodID getMIDLoadClass() const { return sdata->mid_loadclass; }
  jmethodID getMIDLoadClassCurrentDir() const {
    return sdata->mid_loadclasscur; }
  /* java.lang.Boolean */
  jmethodID getMIDBValueOf() const { return sdata->mid_bvalueof; }
  jmethodID getMIDBooleanValue() const { return sdata->mid_booleanvalue; }
  /* java.lang.Byte */
  jmethodID getMIDByteConstr() const { return sdata->mid_byteconstr; }
  jmethodID getMIDByteValue() const { return sdata->mid_bytevalue; }
  /* java.lang.Character */
  jmethodID getMIDCharConstr() const { return sdata->mid_charconstr; }
  jmethodID getMIDCharValue() const { return sdata->mid_charvalue; }
  /* java.lang.Short */
  jmethodID getMIDShortConstr() const { return sdata->mid_shortconstr; }
  jmethodID getMIDShortValue() const { return sdata->mid_shortvalue; }
  /* java.lang.Integer */
  jmethodID getMIDIntConstr() const { return sdata->mid_intconstr; }
  jmethodID getMIDIntValue() const { return sdata->mid_intvalue; }
  /* java.lang.Long */
  jmethodID getMIDLongConstr() const { return sdata->mid_longconstr; }
  jmethodID getMIDLongValue() const { return sdata->mid_longvalue; }
  /* java.lnag.Float */
  jmethodID getMIDFloatConstr() const { return sdata->mid_floatconstr; }
  jmethodID getMIDFloatValue() const { return sdata->mid_floatvalue; }
  /* java.lang.Double */
  jmethodID getMIDDoubleConstr() const { return sdata->mid_doubleconstr; }
  jmethodID getMIDDoubleValue() const { return sdata->mid_doublevalue; }

  jobject getSystemClassLoader() const { return syscldr.get(); }

  ase_jni_jniproxy_priv *get_jniproxy_endnode() const {
    return jniproxy_endnode.get();
  }
  const ase_variant_vtable *get_variant_vtable_for_jobject() const {
    return &variant_vtable_for_jobject;
  }
  void *GetImplPrivForJObject() { return &jobj_impl_priv; }
  ase_variant get_globals_var() const { return globals_var; }

  ase_jni_classinfo *get_jlobject_classinfo() { return jlobject_classinfo; }
  ase_jni_classinfo *get_jlclass_classinfo() { return jlclass_classinfo; }
  bool isConstructComplete() const { return construct_complete; }
  ase_jni_classmap& getClassMap() { return classmap; }

 private:

  ase_variant get_real_global(const ase_string& name);
  static ase_variant get_real_global_cb(const ase_string& name,
    void *userdata);

 private:

  ase_unified_engine_ref& ueref;
  ase_jni_engine_factory *ef_backref;
  JavaVM *const jvm;
  util::javastaticdata *const sdata;
  util::globalref<jobject> syscldr; /* getSystemClassLoader() */
  /* addresses of j???_impl_priv are used for vtbl.impl_private_data */
  ase_jni_script_engine *jobj_impl_priv; /* always equals to 'this' */
  ase_variant_vtable variant_vtable_for_jobject;
  ase_jni_classmap classmap;
  ase_jni_classinfo *jlobject_classinfo; /* classinfo for java.lang.Object */
  ase_jni_classinfo *jlclass_classinfo; /* classinfo for java.lang.Class */
  ase_variant jobj_global;
  ase_variant globals_var_mod; /* modifiable */
  ase_variant globals_var;     /* unmodifiable */
  globals_type globals;
  global_names_type global_names;
  ase_variant ase_global;
  std::auto_ptr<ase_jni_jniproxy_priv> jniproxy_endnode;
  bool construct_complete;

};

}; // namespace asejni

#endif

