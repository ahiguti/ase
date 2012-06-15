
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASECCLASS_HPP
#define GENTYPE_ASECCLASS_HPP

#include <ase/asemutex.hpp>
#include <ase/aseatomic.hpp>
#include <ase/aseexcept.hpp>
#include <ase/asealgo.hpp>

#include <memory>

template <typename T> struct ase_cclass;

struct ase_cclass_base {

  template <typename T> friend struct ase_cclass;

  ASELIB_DLLEXT static void terminate();

 private:

  struct cklass_empty_type { };
  typedef void (*cklass_klsfun_type)();
  typedef void (cklass_empty_type::*cklass_memfun_type)();
  typedef ase_variant (*cklass_callmf_type)(const ase_variant& selfv,
    void *self, cklass_memfun_type mfp, const ase_variant *args,
    ase_size_type nargs);
  typedef ase_variant (*cklass_callsf_type)(const ase_variant& selfv,
    cklass_klsfun_type kfp, const ase_variant *args, ase_size_type nargs);
  struct cklass_method_entries_type : private ase_noncopyable {
    ASELIB_DLLEXT cklass_method_entries_type();
    ASELIB_DLLEXT ~cklass_method_entries_type();
    struct impl_type;
    std::auto_ptr<impl_type> impl;
    ASELIB_DLLEXT static void move(std::auto_ptr<impl_type>& x,
      std::auto_ptr<impl_type>& y);
  };
  ASELIB_DLLEXT static void add(cklass_method_entries_type& me, const char *n,
    cklass_callsf_type c, cklass_klsfun_type f);
  ASELIB_DLLEXT static void add(cklass_method_entries_type& me, const char *n,
    cklass_callmf_type c, cklass_memfun_type f);
  ASELIB_DLLEXT static void initialize_vtable(cklass_method_entries_type& me,
    ase_variant_vtable& kvtbl, ase_variant_vtable& ivtbl, ase_int option);
  ASELIB_DLLEXT static void finalize_vtable(ase_variant_vtable& kvtbl,
    ase_variant_vtable& ivtbl);
  ASELIB_DLLEXT static void wrong_num_args(ase_size_type nargs,
    ase_size_type ex);

  ASELIB_DLLEXT static ase_string kGetString(const ase_variant& self);
  ASELIB_DLLEXT static ase_variant get_property(const ase_variant& self,
    const char *name, ase_size_type namelen);
  ASELIB_DLLEXT static ase_variant kInvokeById(const ase_variant& self,
    ase_int id, const ase_variant *args, ase_size_type nargs);
  ASELIB_DLLEXT static ase_variant iInvokeById(const ase_variant& self,
    ase_int id, const ase_variant *args, ase_size_type nargs);
  ASELIB_DLLEXT static ase_variant invoke_by_name(const ase_variant& self,
    const char *name, ase_size_type namelen, const ase_variant *args,
    ase_size_type nargs);
  ASELIB_DLLEXT static ase_variant invoke_self(const ase_variant& self,
    const ase_variant *args, ase_size_type nargs);

  ASELIB_DLLEXT static void register_terminate_hook(
    cklass_method_entries_type *mentries);

  struct impl_type;

};

template <typename T>
struct ase_cclass : public ase_noncopyable {

 public:

  struct initializer;
  static inline ase_variant get_class();

  static inline ase_variant create0();
  template <typename A0>
  static inline ase_variant create1(A0 a0);
  template <typename A0, typename A1>
  static inline ase_variant create2(A0 a0, A1 a1);
  template <typename A0, typename A1, typename A2>
  static inline ase_variant create3(A0 a0, A1 a1, A2 a2);
  template <typename A0, typename A1, typename A2, typename A3>
  static inline ase_variant create4(A0 a0, A1 a1, A2 a2, A3 a3);
  template <typename A0, typename A1, typename A2, typename A3, typename A4>
  static inline ase_variant create5(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4);

  static T& unbox(const ase_variant& x) {
    if (x.get_vtable_address() != &instance_vtable) {
      #ifdef __GNUC__
      ase_throw_type_mismatch(__PRETTY_FUNCTION__);
      #else
      ase_throw_type_mismatch(__FUNCTION__);
      #endif
    }
    return get(x)->value;
  }

 private:

  static inline void initialize_internal(/* non-const */initializer& ini);

  ase_cclass() : refcount(1), value() { }
  template <typename A0>
  ase_cclass(A0 a0)
    : refcount(1), value(a0) { }
  template <typename A0, typename A1>
  ase_cclass(A0 a0, A1 a1)
    : refcount(1), value(a0, a1) { }
  template <typename A0, typename A1, typename A2>
  ase_cclass(A0 a0, A1 a1, A2 a2)
    : refcount(1), value(a0, a1, a2) { }
  template <typename A0, typename A1, typename A2, typename A3>
  ase_cclass(A0 a0, A1 a1, A2 a2, A3 a3)
    : refcount(1), value(a0, a1, a2, a3) { }
  template <typename A0, typename A1, typename A2, typename A3, typename A4>
  ase_cclass(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4)
    : refcount(1), value(a0, a1, a2, a3, a4) { }

  static void add_ref(const ase_variant& v) {
    get(v)->refcount++;
  }
  static void release(const ase_variant& v) {
    ase_cclass<T> *const p = get(v);
    if ((p->refcount--) == 1) {
      ase_variant::debug_on_destroy_object(v, "cklass");
      delete p;
    }
  }
  static inline ase_cclass *get(const ase_variant& v) {
    return static_cast<ase_cclass *>(v.get_rep().p);
  }

 private:

  static ase_variant_vtable klass_vtable;
  static ase_variant_vtable instance_vtable;
  static ase_cclass_base::cklass_method_entries_type *mentries;

 private:

  ase_atomic_count refcount;
  T value;

};

struct ase_marshal {

 protected:

  template <typename T, bool dummy> struct from_v;
  template <typename T, bool dummy> struct from_v {
    static inline T func(const ase_variant& x) { return x; }
  };
  template <bool dummy> struct from_v<bool, dummy> {
    static inline bool func(const ase_variant& x) { return x.get_boolean(); }
  };
  template <bool dummy> struct from_v<int, dummy> {
    static inline int func(const ase_variant& x) { return x.get_int(); }
  };
  template <bool dummy> struct from_v<long long, dummy> {
    static inline long long func(const ase_variant& x) { return x.get_long(); }
  };
  template <bool dummy> struct from_v<long, dummy> {
    static inline long func(const ase_variant& x) { return x.get_long(); }
  };
  template <bool dummy> struct from_v<float, dummy> {
    static inline float func(const ase_variant& x) { return x.get_double(); }
  };
  template <bool dummy> struct from_v<double, dummy> {
    static inline double func(const ase_variant& x) { return x.get_double(); }
  };
  template <bool dummy> struct from_v<const ase_variant&, dummy> {
    static inline const ase_variant& func(const ase_variant& x) {
      return x;
    }
  };
  template <bool dummy> struct from_v<ase_variant, dummy> {
    static inline ase_variant func(const ase_variant& x) {
      return x;
    }
  };
  template <bool dummy> struct from_v<ase_string, dummy> {
    static inline ase_string func(const ase_variant& x) {
      return x.get_string();
    }
  };
  template <bool dummy> struct from_v<std::string, dummy> {
    static inline std::string func(const ase_variant& x) {
      const ase_string s = x.get_string();
      return std::string(s.data(), s.size());
    }
  };
  template <typename T, bool dummy> struct from_v<T&, dummy> {
    static inline T& func(const ase_variant& x) {
      return ase_cclass<T>::unbox(x);
    }
  };
  template <typename T, bool dummy> struct from_v<const T&, dummy> {
    static inline const T& func(const ase_variant& x) {
      return ase_cclass<T>::unbox(x);
    }
  };

  template <typename T, bool dummy> struct to_v {
    static inline ase_variant func(const ase_variant& x) { return x; }
  };
  template <bool dummy> struct to_v<ase_string, dummy> {
    static inline ase_variant func(const ase_string& x) {
      return x.to_variant();
    }
  };
  template <bool dummy> struct to_v<std::string, dummy> {
    static inline ase_variant func(const std::string& x) {
      return ase_string(x.data(), x.size()).to_variant();
    }
  };

  template <typename T, bool dummy> struct adecl {
    typedef T type;
  };
  template <bool dummy> struct adecl<const ase_string&, dummy> {
    typedef ase_string type;
  };
  template <bool dummy> struct adecl<const std::string&, dummy> {
    typedef std::string type;
  };
  template <bool dummy> struct adecl<const ase_variant&, dummy> {
    typedef const ase_variant& type;
  };
  template <bool dummy> struct adecl<ase_variant&, dummy> {
    typedef ase_variant type;
  };

 public:

  template <typename T> struct arg_decl {
    typedef typename adecl<T, true>::type type;
  };
  template <typename T> struct from_variant {
    typedef from_v<typename arg_decl<T>::type, true> type;
  };
  template <typename T> struct to_variant {
    typedef to_v<T, true> type;
  };

};

#define ASE_CCLASS_GEN_INC
#include <ase/asecclass_gen.hpp>
#undef ASE_CCLASS_GEN_INC

template <typename T> ase_cclass_base::cklass_method_entries_type *
ase_cclass<T>::mentries = 0;

template <typename T> /* non-const */ ase_variant_vtable
ase_cclass<T>::klass_vtable = {
  0, ase_vtype_object, ase_vtoption_none,
  { 0, 0 } /* methods */, 0,
  ase_variant_impl_default::add_ref,
  ase_variant_impl_default::release,
  ase_variant_impl_default::get_attributes,
  ase_variant_impl_default::get_boolean,
  ase_variant_impl_default::get_int,
  ase_variant_impl_default::get_long,
  ase_variant_impl_default::get_double,
  ase_cclass_base::kGetString,
  ase_variant_impl_default::is_same_object,
  ase_cclass_base::get_property,
  ase_variant_impl_default::set_property,
  ase_variant_impl_default::del_property,
  ase_variant_impl_default::get_element,
  ase_variant_impl_default::set_element,
  ase_variant_impl_default::get_length,
  ase_variant_impl_default::set_length,
  ase_cclass_base::invoke_by_name,
  ase_cclass_base::kInvokeById,
  ase_cclass_base::invoke_self,
  ase_variant_impl_default::get_enumerator,
  ase_variant_impl_default::enum_next,
};

template <typename T> /* non-const */ ase_variant_vtable
ase_cclass<T>::instance_vtable = {
  0, ase_vtype_object, ase_vtoption_none,
  { 0, 0 } /* methods */, 0,
  ase_cclass<T>::add_ref,
  ase_cclass<T>::release,
  ase_variant_impl_default::get_attributes,
  ase_variant_impl_default::get_boolean,
  ase_variant_impl_default::get_int,
  ase_variant_impl_default::get_long,
  ase_variant_impl_default::get_double,
  ase_variant_impl_default::get_string,
  ase_variant_impl_default::is_same_object,
  ase_cclass_base::get_property,
  ase_variant_impl_default::set_property,
  ase_variant_impl_default::del_property,
  ase_variant_impl_default::get_element,
  ase_variant_impl_default::set_element,
  ase_variant_impl_default::get_length,
  ase_variant_impl_default::set_length,
  ase_cclass_base::invoke_by_name,
  ase_cclass_base::iInvokeById,
  ase_cclass_base::invoke_self,
  ase_variant_impl_default::get_enumerator,
  ase_variant_impl_default::enum_next,
};

template <typename T> void
ase_cclass<T>::initialize_internal(
  /* non-const */typename ase_cclass<T>::initializer& ini)
{
  if (mentries == 0) {
    std::auto_ptr<ase_cclass_base::cklass_method_entries_type> m(
      new ase_cclass_base::cklass_method_entries_type());
    ase_cclass_base::cklass_method_entries_type::move(m->impl, ini.me.impl);
    ase_cclass_base::initialize_vtable(*m, klass_vtable, instance_vtable,
      ini.option);
    mentries = m.release();
    ase_cclass_base::register_terminate_hook(mentries);
  }
}

template <typename T> ase_variant
ase_cclass<T>::get_class()
{
  const char *const debugid = 0;
    /* set debugid = 0 because this object need not to be destroyed */
  return ase_variant::create_object(&klass_vtable, 0, debugid);
}

template <typename T> ase_variant
ase_cclass<T>::create0()
{
  return ase_variant::create_object(&instance_vtable, new ase_cclass,
    "cklass_instance");
}

template <typename T>
template <typename A0>
ase_variant
ase_cclass<T>::create1(A0 a0)
{
  return ase_variant::create_object(&instance_vtable,
    new ase_cclass(a0),
    "cklass_instance");
}

template <typename T>
template <typename A0, typename A1>
ase_variant
ase_cclass<T>::create2(A0 a0, A1 a1)
{
  return ase_variant::create_object(&instance_vtable,
    new ase_cclass(a0, a1),
    "cklass_instance");
}

template <typename T>
template <typename A0, typename A1, typename A2>
ase_variant
ase_cclass<T>::create3(A0 a0, A1 a1, A2 a2)
{
  return ase_variant::create_object(&instance_vtable,
    new ase_cclass(a0, a1, a2),
    "cklass_instance");
}

template <typename T>
template <typename A0, typename A1, typename A2, typename A3>
ase_variant
ase_cclass<T>::create4(A0 a0, A1 a1, A2 a2, A3 a3)
{
  return ase_variant::create_object(&instance_vtable,
    new ase_cclass(a0, a1, a2, a3),
    "cklass_instance");
}

template <typename T>
template <typename A0, typename A1, typename A2, typename A3, typename A4>
ase_variant
ase_cclass<T>::create5(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4)
{
  return ase_variant::create_object(&instance_vtable,
    new ase_cclass(a0, a1, a2, a3, a4),
    "cklass_instance");
}

#endif

