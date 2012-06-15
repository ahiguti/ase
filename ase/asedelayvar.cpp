
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asedelayvar.hpp>
#include <ase/asemutex.hpp>
#include <ase/aseatomic.hpp>

#define DBG_UE(x)

namespace {

struct ase_delay_variant_impl {

  ase_delay_variant_impl(ase_unified_engine_ref& ue,
    ase_delay_load_variant_callback cb, const ase_string& nm, void *userdata);
  ~ase_delay_variant_impl(); /* DCHK */
  static void add_ref(const ase_variant& self);
  static void release(const ase_variant& self);
  static ase_int get_attributes(const ase_variant& self);
  static bool get_boolean(const ase_variant& self);
  static ase_int get_int(const ase_variant& self);
  static ase_long get_long(const ase_variant& self);
  static double get_double(const ase_variant& self);
  static ase_string get_string(const ase_variant& self);
  static bool is_same_object(const ase_variant& self,
    const ase_variant& value);
  static ase_variant get_property(const ase_variant& self,
    const char *name, ase_size_type namelen);
  static void set_property(const ase_variant& self, const char *name,
    ase_size_type namelen, const ase_variant& value);
  static void del_property(const ase_variant& self, const char *name,
    ase_size_type namelen);
  static ase_variant get_element(const ase_variant& self, ase_int idx);
  static void set_element(const ase_variant& self, ase_int idx,
    const ase_variant& value);
  static ase_int get_length(const ase_variant& self);
  static void set_length(const ase_variant& self, ase_int len);
  static ase_variant invoke_by_name(const ase_variant& self,
    const char *name, ase_size_type namelen, const ase_variant *args,
    ase_size_type nargs);
  static ase_variant invoke_by_id(const ase_variant& self, ase_int id,
    const ase_variant *args, ase_size_type nargs);
  static ase_variant invoke_self(const ase_variant& self,
    const ase_variant *args, ase_size_type nargs);
  static ase_variant get_enumerator(const ase_variant& self);
  static ase_string enum_next(const ase_variant& self, bool& hasnext_r);

 private:

  static ase_delay_variant_impl *get(const ase_variant& self);
  static ase_variant& load_real_variant(const ase_variant& self);
  
 private:
  
  ase_atomic_count refcount;
  ase_unified_engine_ref& ueref;
  ase_recursive_mutex& uemutex;
  ase_variant real_variant;
  bool real_variant_loaded : 1;
  ase_delay_load_variant_callback callback;
  ase_string name;
  void *user_data;

 private:

  /* noncopyable */
  ase_delay_variant_impl(const ase_delay_variant_impl& x);
  ase_delay_variant_impl& operator =(const ase_delay_variant_impl& x);

};

ase_delay_variant_impl::ase_delay_variant_impl(ase_unified_engine_ref& ue,
  ase_delay_load_variant_callback cb, const ase_string& nm, void *userdata)
  : refcount(1), ueref(ue), uemutex(ue.get_mutex()),
    real_variant_loaded(false), callback(cb), name(nm),
    user_data(userdata)
{
  DBG_UE(fprintf(stderr, "ue=%p\n", &ueref));
  DBG_UE(fprintf(stderr, "uemutex=%p\n", &uemutex));
}

ase_delay_variant_impl::~ase_delay_variant_impl() /* DCHK */
{
}

ase_delay_variant_impl *
ase_delay_variant_impl::get(const ase_variant& self)
{
  return static_cast<ase_delay_variant_impl *>(self.get_rep().p);
}

ase_variant &
ase_delay_variant_impl::load_real_variant(const ase_variant& self)
{
  ase_delay_variant_impl *p = get(self);
  {
    ase_mutex_guard<ase_recursive_mutex> g(p->uemutex);
    if (!p->real_variant_loaded) {
      p->real_variant = p->callback(p->name, p->user_data);
      p->real_variant_loaded = true;
    }
  }
  return p->real_variant;
}

void
ase_delay_variant_impl::add_ref(const ase_variant& self)
{
  ase_delay_variant_impl *p = get(self);
  p->refcount++;
}

void
ase_delay_variant_impl::release(const ase_variant& self)
{
  ase_delay_variant_impl *p = get(self);
  if ((p->refcount--) == 1) {
    ase_variant::debug_on_destroy_object(self, "DelayVariant");
    delete p;
  }
}

ase_int
ase_delay_variant_impl::get_attributes(const ase_variant& self)
{
  return ase_attrib_none;
}

bool
ase_delay_variant_impl::get_boolean(const ase_variant& self)
{
  ase_variant& v = load_real_variant(self);
  return v.get_boolean();
}

ase_int
ase_delay_variant_impl::get_int(const ase_variant& self)
{
  ase_variant& v = load_real_variant(self);
  return v.get_int();
}

ase_long
ase_delay_variant_impl::get_long(const ase_variant& self)
{
  ase_variant& v = load_real_variant(self);
  return v.get_long();
}

double
ase_delay_variant_impl::get_double(const ase_variant& self)
{
  ase_variant& v = load_real_variant(self);
  return v.get_double();
}

ase_string
ase_delay_variant_impl::get_string(const ase_variant& self)
{
  ase_variant& v = load_real_variant(self);
  return v.get_string();
}

bool
ase_delay_variant_impl::is_same_object(const ase_variant& self,
  const ase_variant& value)
{
  ase_variant& v = load_real_variant(self);
  return v.is_same_object(value);
}

ase_variant
ase_delay_variant_impl::get_property(const ase_variant& self,
  const char *name, ase_size_type namelen)
{
  ase_variant& v = load_real_variant(self);
  return v.get_property(name, namelen);
}

void
ase_delay_variant_impl::set_property(const ase_variant& self,
  const char *name, ase_size_type namelen, const ase_variant& value)
{
  ase_variant& v = load_real_variant(self);
  v.set_property(name, namelen, value);
}

void
ase_delay_variant_impl::del_property(const ase_variant& self,
  const char *name, ase_size_type namelen)
{
  ase_variant& v = load_real_variant(self);
  v.del_property(name, namelen);
}

ase_variant
ase_delay_variant_impl::get_element(const ase_variant& self, ase_int idx)
{
  ase_variant& v = load_real_variant(self);
  return v.get_element(idx);
}

void
ase_delay_variant_impl::set_element(const ase_variant& self, ase_int idx,
  const ase_variant& value)
{
  ase_variant& v = load_real_variant(self);
  return v.set_element(idx, value);
}

ase_int
ase_delay_variant_impl::get_length(const ase_variant& self)
{
  ase_variant& v = load_real_variant(self);
  return v.get_length();
}

void
ase_delay_variant_impl::set_length(const ase_variant& self, ase_int len)
{
  ase_variant& v = load_real_variant(self);
  return v.set_length(len);
}

ase_variant
ase_delay_variant_impl::invoke_by_name(const ase_variant& self,
  const char *name, ase_size_type namelen, const ase_variant *args,
  ase_size_type nargs)
{
  ase_variant& v = load_real_variant(self);
  return v.invoke_by_name(name, namelen, args, nargs);
}

ase_variant
ase_delay_variant_impl::invoke_by_id(const ase_variant& self, ase_int id,
  const ase_variant *args, ase_size_type nargs)
{
  ase_variant& v = load_real_variant(self);
  return v.invoke_by_id(id, args, nargs);
}

ase_variant
ase_delay_variant_impl::invoke_self(const ase_variant& self,
  const ase_variant *args, ase_size_type nargs)
{
  ase_variant& v = load_real_variant(self);
  return v.invoke_self(args, nargs);
}

ase_variant
ase_delay_variant_impl::get_enumerator(const ase_variant& self)
{
  ase_variant& v = load_real_variant(self);
  return v.get_enumerator();
}

ase_string
ase_delay_variant_impl::enum_next(const ase_variant& self, bool& hasnext_r)
{
  ase_variant& v = load_real_variant(self);
  return v.enum_next(hasnext_r);
}

}; // anonymous namespace

ase_variant
ase_new_delay_load_variant(ase_unified_engine_ref& ue,
  ase_delay_load_variant_callback cb, const ase_string& name, void *userdata)
{
  return ase_variant::create_object(
    &ase_variant_impl<ase_delay_variant_impl>::vtable,
    new ase_delay_variant_impl(ue, cb, name, userdata),
    "DelayVariant");
}


