
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASELIBSET_HPP
#define GENTYPE_ASELIBSET_HPP

#include <ase/asecpp.hpp>
#include <ase/asestrmap.hpp>
#include <ase/aseweakref.hpp>
#include <ase/asealgo.hpp>

struct ASE_LibSet {

  struct Entry {
    const char *Name;
    ASE_NewVariantFactoryFunc *Func;
    ASE_VariantFactory *Service;
  };

 private:

  struct init_ent {
    void operator ()(Entry& e) {
      ASE_VariantFactoryPtr ap;
      e.Func(ap);
      e.Service = ap.release();
    }
  };
  struct term_ent {
    void operator ()(Entry& e) {
      e.Service->destroy();
    }
  };

 public:

  template <typename T> struct Service : public ASE_VariantFactory {

   public:

    static void Create(ASE_VariantFactoryPtr& ap_r) {
      ap_r.reset(new Service());
    }
   
   private:

    Service() {
      ase_for_each(ent_begin(), ent_end(), init_ent(), term_ent());
    }
    ~Service() {
      ase_for_each(ent_begin(), ent_end(), term_ent());
    }
    void destroy() {
      delete this;
    }
    ase_variant NewVariant(ase_unified_engine_ref& ue,
      const ase_variant& globals_weak) {
      ase_variant m = ASE_NewStringMap_GetWeak();
      for (Entry *i = ent_begin(); i != ent_end(); ++i) {
	ase_string s(i->Name);
	ASE_VariantFactory *sp = i->Service;
	ase_variant e = sp->NewVariant(ue, globals_weak);
	m.set_property(s, e);
      }
      m = ase_string_map_unmodifiable(m);
      return ASE_NewStrongReference(m); /* must support GetWeak() */
    }

   private:

    static Entry *ent_begin() { return &T::Entries[0]; }
    static Entry *ent_end() { return ent_begin() +
      (sizeof(T::Entries) / sizeof(T::Entries[0])); }

  };

};

#endif

