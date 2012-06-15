
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEDUMMY_ASEPR_HPP
#define GENTYPE_ASEDUMMY_ASEPR_HPP

#include "asedummy_sctx.hpp"

namespace asedummy {

struct ase_dummy_aseproxy_priv : public ase_variant {

 public:

  friend struct ase_dummy_script_engine;

  struct init_arg {
    init_arg(ase_dummy_script_engine *sc, const ase_variant& v)
      : script_engine(sc), valueref(v) { }
    ase_dummy_script_engine *const script_engine;
    const ase_variant& valueref;
  };

  ase_dummy_aseproxy_priv(const init_arg& ini);
  ~ase_dummy_aseproxy_priv();

  ase_dummy_script_engine *get_script_engine() const {
    return script_engine;
  }

  void dispose(const ase_dummy_script_engine *sc);

 private:

  ase_dummy_script_engine *const script_engine;

 private:

  /* noncopyable */
  ase_dummy_aseproxy_priv(const ase_dummy_aseproxy_priv& x);
  ase_dummy_aseproxy_priv& operator =(const ase_dummy_aseproxy_priv& x);

};

}; // namespace asedummy

#endif

