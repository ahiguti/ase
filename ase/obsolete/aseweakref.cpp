
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/aseweakref.hpp>
#include <ase/aseatomic.hpp>
#include <ase/asemutex.hpp>
#include <ase/aseflags.hpp>

namespace {

ase_atomic_count ase_weak_impl_count_constr = 0;
ase_atomic_count ase_weak_impl_count_destr = 0;

struct ase_base_varimpl {

  /* create a strong reference */
  ase_base_varimpl(const ase_variant& v)
    : refcount_strong(1), refcount_weak(1), value(v) {
    ASE_VERBOSE_WEAK(ase_weak_impl_count_constr++);
    ASE_VERBOSE_WEAK(fprintf(stderr, "WEAK constr %d\n",
      (int)ase_weak_impl_count_constr));
  }
  ~ase_base_varimpl() /* DCHK */ {
    ASE_VERBOSE_WEAK(ase_weak_impl_count_destr++);
    ASE_VERBOSE_WEAK(fprintf(stderr, "WEAK destr constr=%d destr=%d\n",
      (int)ase_weak_impl_count_constr, (int)ase_weak_impl_count_destr));
  }
  ase_variant create_weak_from_strong();
  ase_variant create_strong_from_weak();

  static ase_base_varimpl *getrawptr(const ase_variant& v) {
    return static_cast<ase_base_varimpl *>(v.get_rep().p);
  }

  static ase_variant extract_reference(const ase_variant& v);

  struct guard {

    guard(const ase_variant& v) : ptr(getrawptr(v)) {
      ptr->weak_mutex.lock();
    }
    ~guard() /* DCHK */ {
      ptr->weak_mutex.unlock();
    }
    ase_base_varimpl *get() { return ptr; }
    ase_base_varimpl *operator ->() { return ptr; }

   private:

    ase_base_varimpl *ptr;

   private:

    /* noncopyable */
    guard(const guard& x);
    guard& operator =(const guard& x);

  };

  ase_atomic_count refcount_strong;
  ase_atomic_count refcount_weak;
  mutable ase_recursive_mutex weak_mutex; 
  ase_variant value;
    /* because 'value' is modified only when refcount_strong becomes 0, there
      is no need to synchronize while a strong reference is alive. however,
      weak_mutex must be locked when 'value' is accessed via a weak
      reference. */

};

struct ase_strong_varimpl : public ase_base_varimpl {

  static void add_ref(const ase_variant& v) {
    ase_base_varimpl *selfp = getrawptr(v);
    selfp->refcount_strong++;
  }
  static void release(const ase_variant& v) {
    ase_base_varimpl *selfp = getrawptr(v);
    if ((selfp->refcount_strong--) == 1) {
      if ((selfp->refcount_weak--) == 1) {
	ase_variant::debug_on_destroy_object(v, "StrongWeakRef");
	delete selfp;
      } else {
	guard g(v);
	g->value = ase_variant();
      }
    }
  }
  static ase_int get_attributes(const ase_variant& v) {
    ase_base_varimpl *selfp = getrawptr(v);
    return selfp->value.get_attributes();
  }
  static bool get_boolean(const ase_variant& v) {
    ase_base_varimpl *selfp = getrawptr(v);
    return selfp->value.get_boolean();
  }
  static ase_int get_int(const ase_variant& v) {
    ase_base_varimpl *selfp = getrawptr(v);
    return selfp->value.get_int();
  }
  static ase_long get_long(const ase_variant& v) {
    ase_base_varimpl *selfp = getrawptr(v);
    return selfp->value.get_long();
  }
  static double get_double(const ase_variant& v) {
    ase_base_varimpl *selfp = getrawptr(v);
    return selfp->value.get_double();
  }
  static ase_string get_string(const ase_variant& v) {
    ase_base_varimpl *selfp = getrawptr(v);
    return selfp->value.get_string();
  }
  static bool is_same_object(const ase_variant& v, const ase_variant& value) {
    ase_base_varimpl *selfp = getrawptr(v);
    return selfp->value.is_same_object(value);
  }
  static ase_variant get_property(const ase_variant& v, const char *name,
    ase_size_type len) {
    ase_base_varimpl *selfp = getrawptr(v);
    return selfp->value.get_property(name, len);
  }
  static void set_property(const ase_variant& v, const char *name,
    ase_size_type len, const ase_variant& value) {
    ase_base_varimpl *selfp = getrawptr(v);
    selfp->value.set_property(name, len, value);
  }
  static void del_property(const ase_variant& v, const char *name,
    ase_size_type len) {
    ase_base_varimpl *selfp = getrawptr(v);
    selfp->value.del_property(name, len);
  }
  static ase_variant get_element(const ase_variant& v, ase_int idx) {
    ase_base_varimpl *selfp = getrawptr(v);
    return selfp->value.get_element(idx);
  }
  static void set_element(const ase_variant& v, ase_int idx,
    const ase_variant& value) {
    ase_base_varimpl *selfp = getrawptr(v);
    selfp->value.set_element(idx, value);
  }
  static ase_int get_length(const ase_variant& v) {
    ase_base_varimpl *selfp = getrawptr(v);
    return selfp->value.get_length();
  }
  static void set_length(const ase_variant& v, ase_int len) {
    ase_base_varimpl *selfp = getrawptr(v);
    selfp->value.set_length(len);
  }
  static ase_int GetMethodId(const ase_variant& v, const char *name,
    ase_size_type namelen) {
    ase_base_varimpl *selfp = getrawptr(v);
    return selfp->value.GetMethodId(name, namelen);
  }
  static ase_int get_instance_method_id(const ase_variant& v, const char *name,
    ase_size_type namelen) {
    ase_base_varimpl *selfp = getrawptr(v);
    return selfp->value.get_instance_method_id(name, namelen);
  }
  static ase_variant invoke_by_name(const ase_variant& v, const char *name,
    ase_size_type namelen, const ase_variant *args, ase_size_type nargs) {
    ase_base_varimpl *selfp = getrawptr(v);
    return selfp->value.invoke_by_name(name, namelen, args, nargs);
  }
  static ase_variant invoke_by_id(const ase_variant& v, ase_int id,
    const ase_variant *args, ase_size_type nargs) {
    ase_base_varimpl *selfp = getrawptr(v);
    return selfp->value.invoke_by_id(id, args, nargs);
  }
  static ase_variant invoke_self(const ase_variant& v, const ase_variant *args,
    ase_size_type nargs) {
    ase_base_varimpl *selfp = getrawptr(v);
    return selfp->value.invoke_self(args, nargs);
  }
  static ase_variant get_enumerator(const ase_variant& v) {
    ase_base_varimpl *selfp = getrawptr(v);
    return selfp->value.get_enumerator();
  }
  static ase_string enum_next(const ase_variant& v, bool& hasnext_r) {
    ase_base_varimpl *selfp = getrawptr(v);
    return selfp->value.enum_next(hasnext_r);
  }
  static ase_variant GetWeak(const ase_variant& v) {
    /* create a weak reference from a strong one */
    ase_base_varimpl *selfp = getrawptr(v);
    return selfp->create_weak_from_strong();
  }
  static ase_variant GetStrong(const ase_variant& v) {
    /* v is already a strong reference */
    return v;
  }

};

struct ase_weak_varimpl : public ase_base_varimpl {

  static void add_ref(const ase_variant& v) {
    ase_base_varimpl *selfp = getrawptr(v);
    selfp->refcount_weak++;
  }
  static void release(const ase_variant& v) {
    ase_base_varimpl *selfp = getrawptr(v);
    if ((selfp->refcount_weak--) == 1) {
      ase_variant::debug_on_destroy_object(v, "StrongWeakRef");
      delete selfp;
    }
  }
  static ase_int get_attributes(const ase_variant& v) {
    guard g(v);
    return g->value.get_attributes();
  }
  static bool get_boolean(const ase_variant& v) {
    guard g(v);
    return g->value.get_boolean();
  }
  static ase_int get_int(const ase_variant& v) {
    guard g(v);
    return g->value.get_int();
  }
  static ase_long get_long(const ase_variant& v) {
    guard g(v);
    return g->value.get_long();
  }
  static double get_double(const ase_variant& v) {
    guard g(v);
    return g->value.get_double();
  }
  static ase_string get_string(const ase_variant& v) {
    guard g(v);
    return g->value.get_string();
  }
  static bool is_same_object(const ase_variant& v, const ase_variant& value) {
    guard g(v);
    return g->value.is_same_object(value);
  }
  static ase_variant get_property(const ase_variant& v, const char *name,
    ase_size_type len) {
    guard g(v);
    return g->value.get_property(name, len);
  }
  static void set_property(const ase_variant& v, const char *name,
    ase_size_type len, const ase_variant& value) {
    guard g(v);
    g->value.set_property(name, len, value);
  }
  static void del_property(const ase_variant& v, const char *name,
    ase_size_type len) {
    guard g(v);
    g->value.del_property(name, len);
  }
  static ase_variant get_element(const ase_variant& v, ase_int idx) {
    guard g(v);
    return g->value.get_element(idx);
  }
  static void set_element(const ase_variant& v, ase_int idx,
    const ase_variant& value) {
    guard g(v);
    return g->value.set_element(idx, value);
  }
  static ase_int get_length(const ase_variant& v) {
    guard g(v);
    return g->value.get_length();
  }
  static void set_length(const ase_variant& v, ase_int len) {
    guard g(v);
    return g->value.set_length(len);
  }
  static ase_int GetMethodId(const ase_variant& v, const char *name,
    ase_size_type namelen) {
    guard g(v);
    return g->value.GetMethodId(name, namelen);
  }
  static ase_int get_instance_method_id(const ase_variant& v, const char *name,
    ase_size_type namelen) {
    guard g(v);
    return g->value.get_instance_method_id(name, namelen);
  }
  static ase_variant invoke_by_name(const ase_variant& v, const char *name,
    ase_size_type namelen, const ase_variant *args, ase_size_type nargs) {
    guard g(v);
    return g->value.invoke_by_name(name, namelen, args, nargs);
  }
  static ase_variant invoke_by_id(const ase_variant& v, ase_int id,
    const ase_variant *args, ase_size_type nargs) {
    guard g(v);
    return g->value.invoke_by_id(id, args, nargs);
  }
  static ase_variant invoke_self(const ase_variant& v, const ase_variant *args,
    ase_size_type nargs) {
    guard g(v);
    return g->value.invoke_self(args, nargs);
  }
  static ase_variant get_enumerator(const ase_variant& v) {
    guard g(v);
    return g->value.get_enumerator();
  }
  static ase_string enum_next(const ase_variant& v, bool& hasnext_r) {
    guard g(v);
    return g->value.enum_next(hasnext_r);
  }
  static ase_variant GetWeak(const ase_variant& v) {
    /* v is already a weak reference */
    return v;
  }
  static ase_variant GetStrong(const ase_variant& v) {
    guard g(v);
    if (g->value.is_void()) {
      return ase_variant();
    }
    return g->create_strong_from_weak();
  }

};

ase_variant
ase_base_varimpl::create_weak_from_strong()
{
  refcount_weak++;
  const char *debugid = 0; /* because this is not a newly created object */
  return ase_variant::create_object(
    &ase_variant_impl<ase_weak_varimpl>::vtable,
    this, debugid);
}

ase_variant
ase_base_varimpl::create_strong_from_weak()
{
  refcount_strong++;
  const char *debugid = 0; /* because this is not a newly created object */
  return ase_variant::create_object(
    &ase_variant_impl<ase_strong_varimpl>::vtable,
    this, debugid);
}

ase_variant
ase_base_varimpl::extract_reference(const ase_variant& v)
{
  const void *addr = v.get_vtable_address();
  if (addr == &ase_variant_impl<ase_strong_varimpl>::vtable ||
    addr == &ase_variant_impl<ase_weak_varimpl>::vtable) {
    guard g(v);
    return g->value;
  }
  return v;
}

}; // anonymous namespace

ase_variant
ASE_NewStrongReference(const ase_variant& value)
{
  return ase_variant::create_object(
    &ase_variant_impl<ase_strong_varimpl>::vtable,
    new ase_base_varimpl(value),
    "StrongWeakRef");
}

ase_variant
ASE_ExtractReference(const ase_variant& value)
{
  return ase_base_varimpl::extract_reference(value);
}

