
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEJNI_PRIV_HPP
#define GENTYPE_ASEJNI_PRIV_HPP

#include "asejni_sctx.hpp"

namespace asejni {

/* priv */
struct ase_jni_jniproxy_priv
  : public ase_list_node< ase_synchronized<ase_variant> > {

 public:

  typedef ase_list_node< ase_synchronized<ase_variant> > base_type;

  ase_jni_jniproxy_priv(ase_jni_script_engine *sc);
  ase_jni_jniproxy_priv(const ase_variant& v, ase_jni_classinfo *ci,
    ase_jni_jniproxy_priv& node);
  ~ase_jni_jniproxy_priv(); /* DCHK */

 public:

  ase_jni_script_engine *const script_engine;
  ase_jni_classinfo *const classinfo;

};

}; // namespace asejni

#endif

