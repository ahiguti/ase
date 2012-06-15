
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEFUNCOBJ_HPP
#define GENTYPE_ASEFUNCOBJ_HPP

#include <ase/ase.hpp>
#include <ase/aseatomic.hpp>

struct ase_function_object {

 private:

  struct funcobj_base {
    funcobj_base() : refcount(1) { }
    virtual ~funcobj_base() /* DCHK */ { }
    virtual ase_variant call(const ase_variant *args, ase_size_type nargs) = 0;
    ase_atomic_count refcount;
  };
  template <typename Tf> struct funcobj : public funcobj_base {
    funcobj(const Tf& fo) : fobj(fo) { }
    virtual ase_variant call(const ase_variant *args, ase_size_type nargs) {
      return fobj(args, nargs);
    }
    Tf fobj;
  };
  ASELIB_DLLEXT static ase_variant create_internal(funcobj_base *ptr);

  struct ase_funcobj_varimpl;

 public:

  template <typename Tf>
  static ase_variant create(const Tf& fobj) {
    return create_internal(new funcobj<Tf>(fobj));
  }

};

template <typename Tf>
ase_variant ase_new_function_object(const Tf& fobj)
{
  return ase_function_object::create(fobj);
}

#endif

