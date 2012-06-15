
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */


#ifndef GENTYPE_ASEJNI_IMPL_HPP
#define GENTYPE_ASEJNI_IMPL_HPP

#include "asejni_sctx.hpp"

#include <ase/asedllhandle.hpp>

namespace asejni {

struct ase_jni_engine_factory : public ase_script_engine_factory {

  ase_jni_engine_factory(ase_environment& e);
  ~ase_jni_engine_factory();
  void destroy() {
    delete this;
  }
  void init();
  void init_own_vm();
  void init_preexisting(void *preex);
  void clear();
  void new_script_engine(ase_unified_engine_ref& ue,
    ase_script_engine_ptr& ap_r);

  void child_thread_initialized();
  void child_thread_terminated();

 private:

  ase_environment& env;
  ase_auto_dll_handle *asejni_jvm_dll;
  JavaVM *asejni_jvm;
  bool asejni_jvm_preexisting;
  util::javastaticdata *asejni_sdata;

};

}; // namespace asejni

#endif

