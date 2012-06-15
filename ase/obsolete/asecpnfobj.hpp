
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASECPNT_FUNCTIONOBJECT_HPP
#define GENTYPE_ASECPNT_FUNCTIONOBJECT_HPP

#include <ase/asecpnt.hpp>

#include <iostream>

struct ASE_ComponentFuncObject {

 private:

  template <typename Tf> struct component_impl {

    component_impl(const ase_string& n, ASE_Component::Library& lib)
      : libref(lib) {
      /* create klass */
    }
    component_impl(const component_impl& klass, const ase_variant *args,
      ase_size_type nargs)
      : libref(klass.libref) {
      /* create instance */
    }
    ase_variant funcobj_main(const ase_variant *args, ase_size_type nargs) {
      return Tf()(libref, args, nargs);
    }

    ASE_Component::Library& libref;

    static const ASE_Component::MethodEntry<component_impl> ClassMethods[];
    static const ASE_Component::MethodEntry<component_impl> InstanceMethods[];

  };

 public:

  template <typename Tf, bool UnlockEngine>
  static ase_variant NewClass(const ase_string& name,
    ASE_Component::Library& lib) {
    return ASE_Component::NewClass<
      component_impl<Tf>, component_impl<Tf>, UnlockEngine
    >(name, lib);
  }
  template <typename Tf, bool UnlockEngine>
  static void NewLibraryFactory(ASE_VariantFactoryPtr& klobj_r) {
    ASE_Component::NewLibraryFactory(&NewClass<Tf, UnlockEngine>, klobj_r);
  }

};

template <typename Tf>
const ASE_Component::MethodEntry< ASE_ComponentFuncObject::component_impl<Tf> >
ASE_ComponentFuncObject::component_impl<Tf>::ClassMethods[] = {
  { "__funcobj_main__", &component_impl::funcobj_main },
};

template <typename Tf>
const ASE_Component::MethodEntry< ASE_ComponentFuncObject::component_impl<Tf> >
ASE_ComponentFuncObject::component_impl<Tf>::InstanceMethods[] = {
  /* dummy */
  { "__funcobj_main__", &component_impl::funcobj_main },
};

#endif

