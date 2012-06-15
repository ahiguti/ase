
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asecclass.hpp>
#include <ase/asebindmethod.hpp>

#include <vector>
#include <algorithm>
#include <cstring>

#define DBG(x)
#define DBG_INV(x)

struct ase_cclass_base::cklass_method_entries_type::impl_type {

  impl_type() : next(0), registered(false) { }

  static int compare_method_entry(const char *xname, ase_size_type xnamelen,
    const char *yname, ase_size_type ynamelen)
  {
    /* NOTE: this function compares lengths first and memcmp second. */
    if (xnamelen != ynamelen) {
      return static_cast<int>(xnamelen) - static_cast<int>(ynamelen);
    }
    return std::memcmp(xname, yname, xnamelen);
  }

  struct cklass_kmethod_entry {
    const char *name;
    ase_size_type namelen;
    cklass_callsf_type callsf;
    cklass_klsfun_type klsfun;
    cklass_kmethod_entry(const char *n, cklass_callsf_type c,
      cklass_klsfun_type f)
      : name(n), namelen(std::strlen(n)), callsf(c), klsfun(f) { }
    int operator <(const cklass_kmethod_entry& x) const {
      return compare_method_entry(name, namelen, x.name, x.namelen) < 0;
    }
  };
  struct cklass_imethod_entry {
    const char *name;
    ase_size_type namelen;
    cklass_callmf_type callmf;
    cklass_memfun_type memfun;
    cklass_imethod_entry(const char *n, cklass_callmf_type c,
      cklass_memfun_type f)
      : name(n), namelen(std::strlen(n)), callmf(c), memfun(f) { }
    int operator <(const cklass_imethod_entry& x) const {
      return compare_method_entry(name, namelen, x.name, x.namelen) < 0;
    }
  };

  std::vector<cklass_kmethod_entry> kentries;
  std::vector<cklass_imethod_entry> ientries;
  std::vector<ase_variant_method_entry> vt_kentries;
  std::vector<ase_variant_method_entry> vt_ientries;
  cklass_method_entries_type *next;
  bool registered;

};

ase_cclass_base::cklass_method_entries_type::cklass_method_entries_type()
  : impl(new impl_type())
{
}

ase_cclass_base::cklass_method_entries_type::~cklass_method_entries_type()
{
}

void
ase_cclass_base::cklass_method_entries_type::move(std::auto_ptr<impl_type>& x,
  std::auto_ptr<impl_type>& y)
{
  x = y;
}

void
ase_cclass_base::add(cklass_method_entries_type& me, const char *n,
  cklass_callsf_type c, cklass_klsfun_type f)
{
  me.impl->kentries.push_back(
    cklass_method_entries_type::impl_type::cklass_kmethod_entry(n, c, f));
}

void
ase_cclass_base::add(cklass_method_entries_type& me, const char *n,
  cklass_callmf_type c, cklass_memfun_type f)
{
  me.impl->ientries.push_back(
    cklass_method_entries_type::impl_type::cklass_imethod_entry(n, c, f));
}

void
ase_cclass_base::initialize_vtable(cklass_method_entries_type& me,
  ase_variant_vtable& kvtbl, ase_variant_vtable& ivtbl, ase_int option)
{
  std::sort(me.impl->ientries.begin(), me.impl->ientries.end());
  std::sort(me.impl->kentries.begin(), me.impl->kentries.end());
    /* compares length first, memcmp second */
  for (size_t i = 0; i < me.impl->kentries.size(); ++i) {
    ase_variant_method_entry e;
    e.u8name = me.impl->kentries[i].name;
    e.u8name_len = me.impl->kentries[i].namelen;
    me.impl->vt_kentries.push_back(e);
    DBG(std::printf("n=%s\n", me.impl->kentries[i].name));
  }
  for (size_t i = 0; i < me.impl->ientries.size(); ++i) {
    ase_variant_method_entry e;
    e.u8name = me.impl->ientries[i].name;
    e.u8name_len = me.impl->ientries[i].namelen;
    me.impl->vt_ientries.push_back(e);
    DBG(std::printf("n=%s\n", me.impl->ientries[i].name));
  }
  if (me.impl->kentries.size() > 0) {
    kvtbl.methods.methods = &me.impl->vt_kentries[0];
    kvtbl.methods.num_methods = me.impl->vt_kentries.size();
  }
  if (me.impl->ientries.size() > 0) {
    ivtbl.methods.methods = &me.impl->vt_ientries[0];
    ivtbl.methods.num_methods = me.impl->vt_ientries.size();
  }
  kvtbl.vtopt = option;
  ivtbl.vtopt = option;
  kvtbl.impl_private_data = me.impl.get();
  ivtbl.impl_private_data = me.impl.get();
  kvtbl.instance = &ivtbl;
}

void
ase_cclass_base::finalize_vtable(ase_variant_vtable& kvtbl,
  ase_variant_vtable& ivtbl)
{
  kvtbl.methods.methods = 0;
  kvtbl.methods.num_methods = 0;
  kvtbl.impl_private_data = 0;
  ivtbl.methods.methods = 0;
  ivtbl.methods.num_methods = 0;
  ivtbl.impl_private_data = 0;
}

struct ase_cclass_base::impl_type {

  struct compare_method_entry_fobj {
    int operator ()(const ase_variant_method_entry& x,
      const ase_variant_method_entry& y) {
      return cklass_method_entries_type::impl_type::compare_method_entry(
	x.u8name, x.u8name_len, y.u8name, y.u8name_len);
    }
  };

  static const ase_variant_vtable& get_vtable(const ase_variant& self) {
    return *(static_cast<const ase_variant_vtable *>(
      self.get_vtable_address()));
  }
  static const cklass_method_entries_type::impl_type& get_method_entries(
    const ase_variant& self) {
    return *(static_cast<const cklass_method_entries_type::impl_type *>(
      get_vtable(self).impl_private_data));
  }

  static cklass_method_entries_type *thook_first;

};

ase_cclass_base::cklass_method_entries_type *
ase_cclass_base::impl_type::thook_first = 0;

void
ase_cclass_base::wrong_num_args(ase_size_type nargs, ase_size_type ex)
{
  if (nargs > ex) {
    ase_throw_too_many_args("");
  } else if (nargs < ex) {
    ase_throw_missing_arg("");
  }
}

void
ase_cclass_base::register_terminate_hook(cklass_method_entries_type *mentries)
{
  if (!mentries->impl->registered) {
    mentries->impl->next = impl_type::thook_first;
    impl_type::thook_first = mentries;
    mentries->impl->registered = true;
  }
}

void
ase_cclass_base::terminate()
{
  while (impl_type::thook_first) {
    cklass_method_entries_type *const i = impl_type::thook_first;
    impl_type::thook_first = i->impl->next;
    delete i;
  }
}

ase_string
ase_cclass_base::kGetString(const ase_variant& self)
{
  return ase_string("[CClass]");
}

ase_variant
ase_cclass_base::get_property(const ase_variant& self, const char *name,
  ase_size_type namelen)
{
  DBG(std::printf("GP\n"));
  ase_int id = self.get_method_id(name, namelen);
  if (id >= 0) {
    return ase_new_variant_bind_method_id(self, id);
  }
  return ase_variant(); /* TODO: throw */
}

ase_variant
ase_cclass_base::kInvokeById(const ase_variant& self, ase_int id,
  const ase_variant *args, ase_size_type nargs)
{
  const cklass_method_entries_type::impl_type& me =
    impl_type::get_method_entries(self);
  if (id < 0 || id >= static_cast<ase_int>(me.kentries.size())) {
    return ase_variant(); /* TODO: throw */
  }
  const cklass_method_entries_type::impl_type::cklass_kmethod_entry& e =
    me.kentries[id];
  return e.callsf(self, e.klsfun, args, nargs);
}

ase_variant
ase_cclass_base::iInvokeById(const ase_variant& self, ase_int id,
  const ase_variant *args, ase_size_type nargs)
{
  const cklass_method_entries_type::impl_type& me =
    impl_type::get_method_entries(self);
  if (id < 0 || id >= static_cast<ase_int>(me.ientries.size())) {
    return ase_variant(); /* TODO: throw */
  }
  const cklass_method_entries_type::impl_type::cklass_imethod_entry& e =
    me.ientries[id];
  void *const p = self.get_rep().p;
  return e.callmf(self, p, e.memfun, args, nargs);
}

ase_variant
ase_cclass_base::invoke_by_name(const ase_variant& self, const char *name,
  ase_size_type namelen, const ase_variant *args, ase_size_type nargs)
{
  const ase_int id = self.get_method_id(name, namelen);
  if (id < 0) {
    return ase_variant(); /* TODO: throw */
  }
  return self.invoke_by_id(id, args, nargs);
}

ase_variant
ase_cclass_base::invoke_self(const ase_variant& self, const ase_variant *args,
  ase_size_type nargs)
{
  const ase_int id = self.get_method_id("", 0);
  DBG_INV(std::printf("iself id=%d\n", id));
  if (id < 0) {
    return ase_variant(); /* TODO: throw */
  }
  return self.invoke_by_id(id, args, nargs);
}

