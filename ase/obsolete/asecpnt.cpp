
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asetyped.hpp>
#include <ase/asecpnt.hpp>
#include <ase/asebindmethod.hpp>
#include <ase/asealgo.hpp>
#include <ase/aseweakref.hpp>
#include <ase/aseexcept.hpp>

#include <memory>
#include <vector>
#include <map>
#include <algorithm>

#define DBG_BS(x)
#define DBG_CPN(x)
#define DBG_CPNCL(x)
#define DBG_GM(x)
#define DBG_INV(x)
#define DBG_MTD(x)

ase_variant
ASE_Component::NoopImpl::Noop(const ase_variant *args, ase_size_type nargs)
{
  return ase_variant();
}

struct ASE_Component::implpriv {

  struct uuid_type {
    unsigned long long high;
    unsigned long long low;
    uuid_type(unsigned long long h = 0, unsigned long long l = 0)
      : high(h), low(l) { }
  };

  struct argret_type {
    ase_vtype vtype;
    uuid_type clsid;                     /* optional */
    argret_type() : vtype(ase_vtype_void) { }
  };

  struct argret_types : public std::vector<argret_type> {
  };

  struct method_type {
    ase_string name;
    argret_types args;                   /* optional */
    argret_type ret;                     /* optional */
  };

  struct methods_type : public std::vector<method_type> {
  };

  struct typeinfo_type {
    uuid_type clsid;                     /* optional */
    methods_type instance_methods;
    methods_type klass_methods;
  };

  struct method_entry {
    const char *name;        /* refers method_type::name */
    ase_size_type namelen;
    ase_int id;
    method_entry(const char *n, ase_size_type nlen, ase_int i)
      : name(n), namelen(nlen), id(i) { }
    bool operator <(const method_entry& x) const;
    int compare(const char *n, ase_size_type nlen) const;
  };

  struct method_entries : public std::vector<method_entry> {
  };

  struct strref {
    const char *str;
    ase_size_type len;
    strref(const char *s, ase_size_type l) : str(s), len(l) { }
  };

  struct compare_method_entry;

  static ase_int find_method_entry(const method_entries& ents,
    const char *name, ase_size_type namelen);
  static void init_methodss(kibase *selfp,
    bool unlock_engine, const methods_type& mtds,
    method_entries& sorted_methods, std::auto_ptr<ASE_VTable>& vtbl,
    const ase_variant_vtable& vtbase);
  static void create_component_typeinfo(const char *const *klass_methods,
    ase_size_type nkm, const char *const *instance_methods, ase_size_type nim,
    typeinfo_type& info_r);

};

struct ASE_Component::kibase::impl_type {
  implpriv::typeinfo_type info;
  implpriv::method_entries sorted_instance_methods;
  implpriv::method_entries sorted_klass_methods;
  std::auto_ptr<ASE_VTable> instance_vtable;
  std::auto_ptr<ASE_VTable> klass_vtable;
  ase_variant_vtable klass_weak_vtable;
};

bool
ASE_Component::implpriv::method_entry::operator <(const method_entry& x) const
{
  return compare(x.name, x.namelen) < 0;
}

int
ASE_Component::implpriv::method_entry::compare(const char *n,
  ase_size_type nlen) const
{
  /* NOTE: this is NOT the normal string comparison. instead, this function
     compares lengths first and memcmp second. */
  if (namelen != nlen) {
    return static_cast<int>(namelen) - static_cast<int>(nlen);
  }
  return std::memcmp(name, n, nlen);
}

struct ASE_Component::implpriv::compare_method_entry {
  int operator ()(const method_entry& me, const strref& val) {
    return me.compare(val.str, val.len);
  }
};

ase_int
ASE_Component::implpriv::find_method_entry(const method_entries& ents,
  const char *name, ase_size_type namelen)
{
  const method_entry *first = &ents[0];
  const method_entry *last = first + ents.size();
  const strref val(name, namelen);
  const method_entry *const notfound = 0;
  const method_entry *itr = ase_binary_search(first, last, notfound, val,
    compare_method_entry());
  if (itr) {
    return itr->id;
  }
  return ase_method_id_not_found;
}

void
ASE_Component::implpriv::init_methodss(
  kibase *selfp, bool unlock_engine,
  const methods_type& mtds, method_entries& sorted,
  std::auto_ptr<ASE_VTable>& vtbl,
  const ase_variant_vtable& vtbase)
{
  DBG_GM(fprintf(stderr, "initmethod size=%d\n",
    static_cast<int>(mtds.size())));
  ase_variant_vtable vtb2 = vtbase;
  vtb2.vtopt = (unlock_engine)
    ? ase_vtoption_multithreaded : ase_vtoption_none;
  // vtb2.vtopt = (ase_vtoption)(vtb2.vtopt | ASE_VTOption_SupportsGetMethodId);
  // FIXME
  vtb2.vtopt = (ase_vtoption)(vtb2.vtopt);
  vtb2.impl_private_data = selfp;
  std::auto_ptr<ASE_VTable> vt(new ASE_VTable(vtb2, mtds.size()));
  vtbl = vt;
  for (ase_size_type i = 0; i < mtds.size(); ++i) {
    const ase_string& name8 = mtds[i].name;
    vtbl->Method(i).u8name = name8.data();
    vtbl->Method(i).u8name_len = name8.size();
    ase_int id = static_cast<ase_int>(i);
    sorted.push_back(method_entry(name8.data(), name8.size(), id));
  }
  std::sort(sorted.begin(), sorted.end());
  DBG_GM(fprintf(stderr, "sort\n"));
  for (ase_size_type i = 0; i < mtds.size(); ++i) {
    DBG_GM(fprintf(stderr, "method %s\n", sorted[i].name));
  }
}

void
ASE_Component::iibase::add_ref(const ase_variant& self)
{
  iibase *selfp = get(self);
  selfp->refcount++;
}

ase_int
ASE_Component::iibase::get_attributes(const ase_variant& self)
{
  return 0;
}

ase_string
ASE_Component::iibase::get_string(const ase_variant& self)
{
  ase_int id = GetMethodId(self, "ToString", 8);
  if (id >= 0) {
    return self.invoke_by_id(id, 0, 0).get_string();
  }
  return ase_string("[ASEComponent]");
}

ase_variant
ASE_Component::iibase::get_property(const ase_variant& self,
  const char *name, ase_size_type namelen)
{
  ase_int id = GetMethodId(self, name, namelen);
  if (id >= 0) {
    return ase_new_variant_bind_method_id(self, id);
  }
  return ase_variant();
}

ase_int
ASE_Component::iibase::GetMethodId(const ase_variant& self,
  const char *name, ase_size_type namelen)
{
  kibase *const klassp = getClass(self);
  return implpriv::find_method_entry(klassp->impl->sorted_instance_methods,
    name, namelen);
}

ase_variant
ASE_Component::iibase::invoke_by_name(const ase_variant& self,
  const char *name, ase_size_type namelen, const ase_variant *args,
  ase_size_type nargs)
{
  ase_int id = GetMethodId(self, name, namelen);
  if (id < 0) {
    return ase_variant(); /* TODO: throw */
  }
  return self.invoke_by_id(id, args, nargs);
}

ASE_Component::kibase::kibase(
  const char *const *kmethods, ase_size_type nkmethods,
  const char *const *imethods, ase_size_type nimethods,
  const ase_variant_vtable& kvtbl,
  const ase_variant_vtable& ivtbl,
  bool unlock_engine)
  : impl(0)
{
  implpriv::typeinfo_type ti;
  implpriv::create_component_typeinfo(kmethods, nkmethods, imethods,
    nimethods, ti);
  std::auto_ptr<impl_type> im(new impl_type());
  impl = im.get();
  im->info = ti;
  DBG_CPNCL(fprintf(stderr, "CPNClass create %p\n", this));
  implpriv::init_methodss(this, unlock_engine, im->info.instance_methods,
    im->sorted_instance_methods, im->instance_vtable, ivtbl);
  implpriv::init_methodss(this, unlock_engine, im->info.klass_methods,
    im->sorted_klass_methods, im->klass_vtable, kvtbl);
  im->klass_vtable->SetInstanceVTable(&im->instance_vtable->get());
  im->klass_weak_vtable = im->klass_vtable->get();
  im->klass_weak_vtable.add_ref = ase_variant_impl_default::add_ref;
  im->klass_weak_vtable.release = ase_variant_impl_default::release;
  im.release();
}

ASE_Component::kibase::~kibase() /* DCHK */
{
  DBG_CPNCL(fprintf(stderr, "CPNClass destroy %p\n", this));
  delete impl;
}

const ase_variant_vtable *
ASE_Component::kibase::get_klass_vtable() const
{
  return &impl->klass_vtable->get();
}

const ase_variant_vtable *
ASE_Component::kibase::get_instance_vtable() const
{
  return &impl->instance_vtable->get();
}

void
ASE_Component::kibase::add_ref(const ase_variant& self)
{
  kibase *selfp = get(self);
  selfp->refcount++;
}

ase_int
ASE_Component::kibase::get_attributes(
  const ase_variant& self)
{
  return 0;
}

ase_string
ASE_Component::kibase::get_string(const ase_variant& self)
{
  ase_int id = GetMethodId(self, "ToString", 8);
  if (id >= 0) {
    return self.invoke_by_id(id, 0, 0).get_string();
  }
  return ase_string("[ASEComponentClass]");
}

ase_variant
ASE_Component::kibase::get_property(
  const ase_variant& self, const char *name, ase_size_type namelen)
{
  ase_int id = GetMethodId(self, name, namelen);
  if (id >= 0) {
    return ase_new_variant_bind_method_id(self, id);
  }
  if (ase_string::equals(name, namelen, "NewInstance")) {
    return ase_new_variant_bind_method_name(self, ase_string("NewInstance"));
  }
  return ase_variant();
}

ase_int
ASE_Component::kibase::GetMethodId(
  const ase_variant& self, const char *name, ase_size_type namelen)
{
  kibase *const selfp = get(self);
  return implpriv::find_method_entry(selfp->impl->sorted_klass_methods,
    name, namelen);
}

ase_int
ASE_Component::kibase::get_instance_method_id(
  const ase_variant& self, const char *name, ase_size_type namelen)
{
  kibase *const selfp = get(self);
  return implpriv::find_method_entry(
    selfp->impl->sorted_instance_methods, name, namelen);
}

ase_variant
ASE_Component::kibase::invoke_by_name(
  const ase_variant& self, const char *name, ase_size_type namelen,
  const ase_variant *args, ase_size_type nargs)
{
  ase_int id = GetMethodId(self, name, namelen);
  if (id < 0) {
    if (ase_string::equals(name, namelen, "NewInstance")) {
      return self.invoke_self(args, nargs);
    }
    return ase_variant(); /* TODO: throw */
  }
  return self.invoke_by_id(id, args, nargs);
}

ase_variant
ASE_Component::kibase::GetWeak(const ase_variant& self)
{
  kibase *const selfp = get(self);
  const char *const debugid = 0; /* because it's not a new object */
  ase_variant r = ase_variant::create_object(&selfp->impl->klass_weak_vtable,
    selfp, debugid);
  return r;
}

void
ASE_Component::implpriv::create_component_typeinfo(
  const char *const *klass_methods, ase_size_type nkm,
  const char *const *instance_methods, ase_size_type nim,
  typeinfo_type& info_r)
{
  DBG_MTD(fprintf(stderr, "component km=%d im=%d\n", (int)nkm, (int)nim));
  info_r = typeinfo_type();
  {
    for (ase_size_type i = 0; i < nkm; ++i) {
      info_r.klass_methods.push_back(method_type());
      method_type& m = info_r.klass_methods.back();
      const char *s = klass_methods[i];
      m.name = ase_string(s);
    }
  }
  {
    for (ase_size_type i = 0; i < nim; ++i) {
      info_r.instance_methods.push_back(method_type());
      method_type& m = info_r.instance_methods.back();
      const char *s = instance_methods[i];
      m.name = ase_string(s);
    }
  }
}

struct ase_mlib_variant
  : public ase_variant_impl_typed<ase_mlib_variant>,
    public ASE_Component::Library {

  ase_mlib_variant(ase_unified_engine_ref& ue, const ase_variant& glweak)
    : ueref(ue), globals_weak(glweak) {
  }

  static ase_variant get_property(const ase_variant& self, const char *name,
    ase_size_type len) {
    ase_mlib_variant *selfp = get(self);
    ase_string s(name, len);
    klasses_type::const_iterator iter = selfp->klasses.find(s);
    if (iter == selfp->klasses.end()) {
      return ase_variant();
    }
    return iter->second.GetWeak(); /* always returns a weak reference */
  }

  ase_unified_engine_ref& GetUnifiedEngineRef() {
    return ueref;
  }
  ase_variant GetClass(const ase_string& name) {
    klasses_type::const_iterator iter = klasses.find(name);
    if (iter != klasses.end()) {
      return iter->second;
    }
    return ase_variant();
  }
  ase_variant GetGlobals() {
    return globals_weak;
  }

  typedef std::map<ase_string, ase_variant> klasses_type;

  ase_unified_engine_ref& ueref;
  ase_variant globals_weak;
  klasses_type klasses;

  static const char *debugid;

};

const char *ase_mlib_variant::debugid = "CpntLib";

struct ase_slib_variant
  : public ase_variant_impl_typed<ase_slib_variant>,
    public ASE_Component::Library {

  ase_slib_variant(ase_unified_engine_ref& ue, const ase_variant& glweak)
    : ueref(ue), globals_weak(glweak) {
  }
  static ase_variant get_single_klass(const ase_variant& self) {
    if (self.get_vtable_address() == &ase_variant_impl<ase_slib_variant>::vtable) {
      ase_slib_variant *selfp = ase_slib_variant::get(self);
      return selfp->single_klass;
    }
    return self;
  }

  static ase_variant get_property(const ase_variant& self, const char *name,
    ase_size_type len) {
    ase_slib_variant *selfp = get(self);
    return selfp->single_klass.get_property(name, len);
  }
  static ase_variant invoke_by_id(const ase_variant& self, ase_int id,
    const ase_variant *args, ase_size_type nargs) {
    ase_slib_variant *selfp = get(self);
    return selfp->single_klass.invoke_by_id(id, args, nargs);
  }
  static ase_int GetMethodId(const ase_variant& self, const char *name,
    ase_size_type namelen) {
    ase_slib_variant *selfp = get(self);
    return selfp->single_klass.GetMethodId(name, namelen);
  }
  static ase_variant invoke_by_name(const ase_variant& self, const char *name,
    ase_size_type namelen, const ase_variant *args, ase_size_type nargs) {
    ase_slib_variant *selfp = get(self);
    return selfp->single_klass.invoke_by_name(name, namelen, args, nargs);
  }
  static ase_variant invoke_self(const ase_variant& self,
    const ase_variant *args, ase_size_type nargs) {
    return invoke_by_name(self, "__funcobj_main__", 16, args, nargs);
  }

  ase_unified_engine_ref& GetUnifiedEngineRef() {
    return ueref;
  }
  ase_variant GetClass(const ase_string& name) {
    if (name.empty()) {
      return single_klass;
    }
    return ase_variant();
  }
  ase_variant GetGlobals() {
    return globals_weak;
  }

  typedef std::map<ase_string, ase_variant> klasses_type;

  ase_unified_engine_ref& ueref;
  ase_variant globals_weak;
  ase_variant single_klass;

  static const char *debugid;

};

const char *ase_slib_variant::debugid = "CpntSingleLib";

struct ase_lib_vfactory : public ASE_VariantFactory {

  static void create(ASE_Component::NewClassFunc *ncfunc,
    ASE_VariantFactoryPtr& ap_r) {
    ap_r.reset(new ase_lib_vfactory(ncfunc));
  }
  static void create(const ASE_Component::LibraryEntry *ents,
    ase_size_type nents, ASE_VariantFactoryPtr& ap_r) {
    ap_r.reset(new ase_lib_vfactory(ents, nents));
  }

  ase_lib_vfactory(ASE_Component::NewClassFunc *ncfunc)
    : single_klass(ncfunc) {
  }
  ase_lib_vfactory(const ASE_Component::LibraryEntry *ents,
    ase_size_type nents) : single_klass(0) {
    for (ase_size_type i = 0; i < nents; ++i) {
      entry_type ent;
      ent.first = ase_string(ents[i].Name);
      ent.second = ents[i].NewClass;
      entries.push_back(ent);
    }
  }

  void destroy() { delete this; }
  ase_variant NewVariant(ase_unified_engine_ref& ue,
    const ase_variant& globals_weak) {
    if (single_klass != 0) {
      /* single klass library */
      ase_slib_variant *p = new ase_slib_variant(ue, globals_weak);
      ase_variant kls = ase_variant::create_object(
	&ase_variant_impl<ase_slib_variant>::vtable,
	p,
	ase_slib_variant::debugid);
      ase_variant e = (*single_klass)(ase_string(), *p);
      p->single_klass = e;
      return ASE_NewStrongReference(kls);
    } else {
      /* multiple klasses library */
      ase_mlib_variant *p = new ase_mlib_variant(ue, globals_weak);
      ase_variant kls = ase_variant::create_object(
	&ase_variant_impl<ase_mlib_variant>::vtable,
	p,
	ase_mlib_variant::debugid);
      for (ase_size_type i = 0; i < entries.size(); ++i) {
	ase_string n = entries[i].first;
	ase_variant e = (*entries[i].second)(n, *p);
	p->klasses[n] = e;
      }
      return ASE_NewStrongReference(kls);
    }
  }

  ASE_Component::NewClassFunc *const single_klass;
  typedef std::pair<ase_string, ASE_Component::NewClassFunc *>
    entry_type;
  typedef std::vector<entry_type> entries_type;
  entries_type entries;

};

void
ASE_Component::NewLibraryFactory(NewClassFunc *ncfunc,
  ASE_VariantFactoryPtr& ap_r)
{
  ase_lib_vfactory::create(ncfunc, ap_r);
}

void
ASE_Component::NewLibraryFactory(const LibraryEntry *ents,
  ase_size_type nents, ASE_VariantFactoryPtr& ap_r)
{
  ase_lib_vfactory::create(ents, nents, ap_r);
}

void
ASE_Component::throw_invalid_method_id()
{
  ase_throw_illegal_operation("ASECPNT: invoke_by_id: invalid method id");
}

ase_variant
ASE_Component::get_class_from_single_class_library(const ase_variant& val)
{
  ase_variant v = ASE_ExtractReference(val);
    /* extract if it's a weak/strong ref */
  return ase_slib_variant::get_single_klass(v);
}

