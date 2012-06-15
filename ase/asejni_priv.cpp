
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include "asejni_priv.hpp"

#define DBG(x)

namespace asejni {

ase_jni_jniproxy_priv::ase_jni_jniproxy_priv(ase_jni_script_engine *sc)
  : base_type(ase_variant()), script_engine(sc), classinfo(0)
{
  DBG(fprintf(stderr, "__jnipr_priv_n0 %p\n", this));
}

ase_jni_jniproxy_priv::ase_jni_jniproxy_priv(const ase_variant& v,
  ase_jni_classinfo *ci, ase_jni_jniproxy_priv& node)
  : base_type(v, node), script_engine(node.script_engine), classinfo(ci)
{
  DBG(fprintf(stderr, "__jnipr_priv_n1 %p\n", this));
}

ase_jni_jniproxy_priv::~ase_jni_jniproxy_priv() /* DCHK */
{
  DBG(fprintf(stderr, "__jnipr_priv_d %p\n", this));
}

}; // namespace asejni

