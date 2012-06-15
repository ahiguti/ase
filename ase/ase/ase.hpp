
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASE_HPP
#define GENTYPE_ASE_HPP

#if defined(_MSC_VER)
#define ASE_COMPAT_DLLEXPORT __declspec(dllexport)
#define ASE_COMPAT_DLLIMPORT __declspec(dllimport)
#else
#define ASE_COMPAT_DLLEXPORT
#define ASE_COMPAT_DLLIMPORT
#endif

#ifdef ASELIB_EXPORTS
#define ASELIB_DLLEXT ASE_COMPAT_DLLEXPORT
#else
#define ASELIB_DLLEXT ASE_COMPAT_DLLIMPORT
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>  /* std::memcmp  */
#include <string>   /* std::string  */
#include <iostream> /* std::ostream */

#include <ase/asealloca.hpp>
#include <ase/asemutex.hpp>
#include <ase/aseatomic.hpp>

#if __GNUC__ >= 4
/* enable __attribute__ */
#else
#define __attribute__(x)
#endif

#ifdef ASE_DEBUG_VARIANT_COUNT
#define ASE_DEBUG_VARIANT_COUNT_DO(x) x
#else
#define ASE_DEBUG_VARIANT_COUNT_DO(x)
#endif

typedef size_t ase_size_type;
typedef int ase_int;
typedef long long ase_long;

#ifdef _MSC_VER
typedef wchar_t ase_ucchar;
#else
typedef unsigned short ase_ucchar;
#endif

class ase_string;
class ase_variant;

typedef enum {
  ase_vtype_void          = 0,
  ase_vtype_bool          = 1,
  ase_vtype_int           = 2,
  ase_vtype_long          = 3,
  ase_vtype_double        = 4,
  ase_vtype_string        = 5,
  ase_vtype_object        = 6,
} ase_vtype;

typedef enum {
  ase_vtoption_none             = 0,
  ase_vtoption_multithreaded    = 1, /* object can be accessed asynchrously */
  ase_vtoption_remote_by_value  = 2, /* object can be passed by value */
} ase_vtoption;

typedef enum {
  ase_attrib_none         = 0,
  ase_attrib_function     = 1, /* allows invoke_self */
  ase_attrib_array        = 2, /* allows *Element */
  ase_attrib_mutable      = 4, /* allows set* del* */
} ase_attrib;

enum {
  ase_method_id_not_found = -1,   /* get_method_id */
};

struct ase_variant_method_entry {
  const char *u8name;
  ase_size_type u8name_len;
};

struct ase_variant_methods {
  /* must be sorted by ase_variant::compare_method_entry() */
  const ase_variant_method_entry *methods;
  ase_size_type num_methods;
};

struct ase_variant_vtable {
  void *impl_private_data;
  ase_vtype vtype;
  ase_int vtopt; /* ase_vtoption */
  ase_variant_methods methods; /* for statically-typed objects */
  const ase_variant_vtable *instance; /* for class objects only */
  void (*add_ref)(const ase_variant& self);
  void (*release)(const ase_variant& self);
  ase_int (*get_attributes)(const ase_variant& self);
    /* returns ase_attrib */
  bool (*get_boolean)(const ase_variant& self);
  ase_int (*get_int)(const ase_variant& self);
  ase_long (*get_long)(const ase_variant& self);
  double (*get_double)(const ase_variant& self);
  ase_string (*get_string)(const ase_variant& self);
  bool (*is_same_object)(const ase_variant& self, const ase_variant& value);
  ase_variant (*get_property)(const ase_variant& self, const char *name,
    ase_size_type namelen);
  void (*set_property)(const ase_variant& self, const char *name,
    ase_size_type namelen, const ase_variant& value);
  void (*del_property)(const ase_variant& self, const char *name,
    ase_size_type namelen);
  ase_variant (*get_element)(const ase_variant& self, ase_int idx);
    /* for arrays */
  void (*set_element)(const ase_variant& self, ase_int idx,
    const ase_variant& value); /* for arrays */
  ase_int (*get_length)(const ase_variant& self); /* for arrays */
  void (*set_length)(const ase_variant& self, ase_int len); /* for arrays */
  ase_variant (*invoke_by_name)(const ase_variant& self, const char *name, 
    ase_size_type namelen, const ase_variant *args, ase_size_type nargs);
    /* expected to be equvalent to get_property(name).invoke_self(args) */
  ase_variant (*invoke_by_id)(const ase_variant& self, ase_int id, 
    const ase_variant *args, ase_size_type nargs);
    /* invoke_by_id(get_method_id(name), args) is expected to be equivalent to
      invoke_by_name(name, args) */
  ase_variant (*invoke_self)(const ase_variant& self, const ase_variant *args,
    ase_size_type nargs);
    /* for function objects */
  ase_variant (*get_enumerator)(const ase_variant& self);
    /* for containers */
  ase_string (*enum_next)(const ase_variant& self, bool& hasnext_r);
    /* only available if self is a enumerator returned by get_enumerator() */
};

class ase_string {

 private:

  struct impl_type {
    ase_atomic_type refcount;
    ase_size_type len;
    char buf[1]; /* variable length. not portable. */
  };
  struct impl_util;

 public:

  friend class ase_variant;
  friend class ase_variant_impl_string;

  ase_string() : rep(&emptyrep) { }
  ase_string(const ase_string& x) : rep(x.rep) { addref_internal(rep); }
  ase_string(const char *str, ase_size_type len) { init_8(str, len); }
  ase_string(const char *s1, ase_size_type len1, const char *s2,
    ase_size_type len2) { init_8(s1, len1, s2, len2); }
  ase_string(const ase_ucchar *str, ase_size_type len) { init_16(str, len); }
  template <ase_size_type n> explicit
  ase_string(const char (& str)[n]) { init_8(str, n - 1); }
  explicit ase_string(const char *s) { init_8(s, std::strlen(s)); }
  ase_string(const std::string& s) { init_8(s.data(), s.size()); }
  template <typename Tbufinit>
  ase_string(ase_size_type len, const Tbufinit& bufinit) {
    bufinit(init_8_retbuf(len), len); }

  inline ase_string& operator =(const ase_string& x);
  ~ase_string() /* DCHK */ { release_internal(rep); }
  inline const char *data() const { return rep->buf; }
  inline ase_size_type size() const { return rep->len; }
  inline bool empty() const { return rep->len == 0; }
  ASELIB_DLLEXT ase_variant to_variant() const;
  
  template <ase_size_type n>
  static inline bool equals(const char *x, ase_size_type xlen,
    const char (& y)[n]);
  template <ase_size_type n> inline bool
  operator ==(const char (& y)[n]) const;
  inline int compare(const ase_string& y) const;

  friend inline ase_string operator +(const ase_string& x,
    const ase_string& y);
  template <ase_size_type n>
  friend inline ase_string operator +(const ase_string& x,
    const char (& y)[n]);
  template <ase_size_type n>
  friend inline ase_string operator +(const char (& x)[n],
    const ase_string& y);

  ASELIB_DLLEXT static void show_statistics();

 private:

  explicit ase_string(impl_type *rep);
  ASELIB_DLLEXT static void addref_internal(impl_type *rep);
  ASELIB_DLLEXT static void release_internal(impl_type *rep);
  ASELIB_DLLEXT void init_8(const char *str, ase_size_type len);
  ASELIB_DLLEXT void init_8(const char *s1, ase_size_type len1,
    const char *s2, ase_size_type len2);
  ASELIB_DLLEXT char *init_8_retbuf(ase_size_type len);
  ASELIB_DLLEXT void init_16(const ase_ucchar *str, ase_size_type len);
  ASELIB_DLLEXT static void init_variant_internal(const char *str,
    ase_size_type len, ase_variant& v);

 private:

  ASELIB_DLLEXT static impl_type emptyrep;

 private:

  impl_type *rep;

};

class ase_variant {

 public:

  union rep_type {
    ase_int i;
    ase_long l;
    double d;
    void *p;
  };

  friend class ase_string;

  inline ase_variant();
  inline ase_variant(bool v);
  template <typename T>
  inline ase_variant(const volatile T *v);
    /* forbid matching ase_variant(bool) */
  inline ase_variant(ase_int v);
  inline ase_variant(ase_long v);
  inline ase_variant(double v);
  template <ase_size_type slen>
  inline ase_variant(const char (& s)[slen]);
  inline ase_variant(const char *str, ase_size_type slen);
  inline static ase_variant create_void();
  inline static ase_variant create_boolean(bool v);
  inline static ase_variant create_int(ase_int v);
  inline static ase_variant create_long(ase_long v);
  inline static ase_variant create_double(double v);
  inline static ase_variant create_object(const ase_variant_vtable *vtbl,
    void *rep, const char *debugid);
  ASELIB_DLLEXT static ase_variant create_from_active_exception() throw();

  ase_vtype get_type() const { return vtbl->vtype; }
  bool is_void() const { return get_type() == ase_vtype_void; }
  bool is_numeric() const {
    return get_type() >= ase_vtype_bool && get_type() <= ase_vtype_double; }
  ase_variant(const ase_variant& x) : vtbl(x.vtbl), rep(x.rep) {
    addref_internal(); }
  inline ase_variant& operator =(const ase_variant& x);
  ~ase_variant() /* DCHK */ { release_internal(); }
  friend inline bool operator ==(const ase_variant& x, const ase_variant& y);
  inline ase_int get_attributes() const;
  inline bool get_boolean() const;
  inline ase_int get_int() const;
  inline ase_long get_long() const;
  inline double get_double() const;
  inline ase_string get_string() const;
  inline bool is_same_object(const ase_variant& val) const;
  inline ase_variant get_property(const char *name, ase_size_type namelen)
    const;
  inline ase_variant get_property(const ase_string& name) const;
  inline void set_property(const char *name, ase_size_type namelen,
    const ase_variant& val) const;
  inline void set_property(const ase_string& name, const ase_variant& val)
    const;
  inline void del_property(const char *name, ase_size_type namelen) const;
  inline void del_property(const ase_string& name) const;
  inline ase_variant get_element(ase_int idx) const;
  inline void set_element(ase_int idx, const ase_variant& val) const;
  inline ase_int get_length() const;
  inline ase_int get_length_or_zero() const;
  inline void set_length(ase_int len) const;
  template <ase_size_type n>
  inline ase_int get_method_id(const char (& name)[n]) const;
  inline ase_int get_method_id(const ase_string& name) const;
  template <ase_size_type n>
  inline ase_int get_instance_method_id(const char (& name)[n]) const;
  inline ase_int get_instance_method_id(const ase_string& name) const;
  inline ase_variant invoke_by_name(const char *name, ase_size_type namelen,
    const ase_variant *args, ase_size_type nargs) const;
  inline ase_variant invoke_by_name(const ase_string& name,
    const ase_variant *args, ase_size_type nargs) const;
  inline ase_variant invoke_by_id(ase_int idx, const ase_variant *args,
    ase_size_type nargs) const;
  inline ase_variant invoke_self(const ase_variant *args, ase_size_type nargs)
    const;
  inline ase_variant get_enumerator() const;
  inline ase_string enum_next(bool& hasnext_r) const;

  template <ase_size_type namelen>
  inline ase_variant get(const char (& name)[namelen]) const;
  inline ase_variant get(ase_int idx) const;
  template <ase_size_type namelen>
  inline void set(const char (& name)[namelen], const ase_variant& val) const;
  inline void set(ase_int idx, const ase_variant& val) const;
  template <ase_size_type namelen>
  inline ase_variant minvoke(const char (& name)[namelen]) const;
  template <ase_size_type namelen>
  inline ase_variant minvoke(const char (& name)[namelen],
    const ase_variant& arg0) const;
  template <ase_size_type namelen>
  inline ase_variant minvoke(const char (& name)[namelen],
    const ase_variant& arg0, const ase_variant& arg1) const;
  template <ase_size_type namelen>
  inline ase_variant minvoke(const char (& name)[namelen],
    const ase_variant& arg0, const ase_variant& arg1,
    const ase_variant& arg2) const;
  template <ase_size_type namelen>
  inline ase_variant minvoke(const char (& name)[namelen],
    const ase_variant& arg0, const ase_variant& arg1,
    const ase_variant& arg2, const ase_variant& arg3) const;
  inline ase_variant minvoke(ase_int id) const;
  inline ase_variant minvoke(ase_int id, const ase_variant& arg0) const;
  inline ase_variant minvoke(ase_int id, const ase_variant& arg0,
    const ase_variant& arg1) const;
  inline ase_variant minvoke(ase_int id, const ase_variant& arg0,
    const ase_variant& arg1, const ase_variant& arg2) const;
  inline ase_variant minvoke(ase_int id, const ase_variant& arg0,
    const ase_variant& arg1, const ase_variant& arg2,
    const ase_variant& arg3) const;
  inline ase_variant invoke() const;
  inline ase_variant invoke(const ase_variant& arg0) const;
  inline ase_variant invoke(const ase_variant& arg0,
    const ase_variant& arg1) const;
  inline ase_variant invoke(const ase_variant& arg0,
    const ase_variant& arg1, const ase_variant& arg2) const;
  inline ase_variant invoke(const ase_variant& arg0,
    const ase_variant& arg1, const ase_variant& arg2, const ase_variant& arg3)
    const;

  const void *get_vtable_address() const { return vtbl; }
  const void *get_instance_vtable_address() const { return vtbl->instance; }
  void *get_impl_priv() const { return vtbl->impl_private_data; }
  ase_int get_vtoption() const { return vtbl->vtopt; }
  ase_size_type get_num_methods() const { return vtbl->methods.num_methods; }
  const ase_variant_method_entry *get_methods() const {
    return vtbl->methods.methods; }
  ase_size_type get_num_instance_methods() const {
    return vtbl->instance ? vtbl->instance->methods.num_methods : 0; }
  const ase_variant_method_entry *get_instance_methods() const {
    return vtbl->instance ? vtbl->instance->methods.methods : 0; }
  const rep_type& get_rep() const { return rep; }

  static bool is_value_type(ase_vtype t) { return t < ase_vtype_object; }
  static bool is_reference_type(ase_vtype t) { return t >= ase_vtype_object; }

  void swap(ase_variant& v) {
    std::swap(vtbl, v.vtbl);
    std::swap(rep, v.rep);
  }

  ASELIB_DLLEXT static int compare_method_entry(const char *xname,
    ase_size_type xnamelen, const char *yname, ase_size_type ynamelen);
  ASELIB_DLLEXT ase_int get_method_id(const char *name, ase_size_type namelen)
    const;
  ASELIB_DLLEXT ase_int get_instance_method_id(const char *name,
    ase_size_type namelen) const;

  inline static void debug_on_create_object(const ase_variant& v,
    const char *debugid);
  inline static void debug_on_destroy_object(const ase_variant& v,
    const char *debugid);
  ASELIB_DLLEXT static void show_statistics();

 private:

  static bool need_ref_count(ase_vtype t) { return t >= ase_vtype_string; }
  inline void addref_internal() const;
  inline void release_internal() const;
  inline ase_variant(const ase_variant_vtable *vtbl, void *rep,
    const char *debugid);

  ASELIB_DLLEXT static void debug_createobj_internal(const ase_variant& v,
    const char *debugid);
  ASELIB_DLLEXT static void debug_destroyobj_internal(const ase_variant& v,
    const char *debugid);

 private:

  const ase_variant_vtable *vtbl;
  rep_type rep;

};

class ase_variant_impl_default : public ase_noncopyable {

 public:

  ASELIB_DLLEXT static void add_ref(const ase_variant& v);
  ASELIB_DLLEXT static void release(const ase_variant& v);
  ASELIB_DLLEXT static ase_int get_attributes(const ase_variant& v);
  ASELIB_DLLEXT static bool get_boolean(const ase_variant& v);
  ASELIB_DLLEXT static ase_int get_int(const ase_variant& v);
  ASELIB_DLLEXT static ase_long get_long(const ase_variant& v);
  ASELIB_DLLEXT static double get_double(const ase_variant& v);
  ASELIB_DLLEXT static ase_string get_string(const ase_variant& v);
  ASELIB_DLLEXT static bool is_same_object(const ase_variant& v,
    const ase_variant& val);
  ASELIB_DLLEXT static ase_variant get_property(const ase_variant& v,
    const char *name, ase_size_type len);
  ASELIB_DLLEXT static void set_property(const ase_variant& v,
    const char *name, ase_size_type len, const ase_variant& value);
  ASELIB_DLLEXT static void del_property(const ase_variant& v,
    const char *name, ase_size_type len);
  ASELIB_DLLEXT static ase_variant get_element(const ase_variant& v,
    ase_int idx);
  ASELIB_DLLEXT static void set_element(const ase_variant& v, ase_int idx,
    const ase_variant& value);
  ASELIB_DLLEXT static ase_int get_length(const ase_variant& v);
  ASELIB_DLLEXT static void set_length(const ase_variant& v, ase_int len);
  ASELIB_DLLEXT static ase_variant invoke_by_name(const ase_variant& v,
    const char *name, ase_size_type namelen, const ase_variant *args,
    ase_size_type nargs);
  ASELIB_DLLEXT static ase_variant invoke_by_id(const ase_variant& v,
    ase_int id, const ase_variant *args, ase_size_type nargs);
  ASELIB_DLLEXT static ase_variant invoke_self(const ase_variant& v,
    const ase_variant *args, ase_size_type nargs);
  ASELIB_DLLEXT static ase_variant get_enumerator(const ase_variant& v);
  ASELIB_DLLEXT static ase_string enum_next(const ase_variant& v,
    bool& hasnext_r);

};

template <typename T, ase_int opt = ase_vtoption_none>
class ase_variant_impl {

 public:

  static const ase_variant_vtable vtable;

};

ASELIB_DLLEXT extern const ase_variant_vtable ase_variant_default_vtable;
ASELIB_DLLEXT extern const ase_variant_vtable ase_variant_boolean_vtable;
ASELIB_DLLEXT extern const ase_variant_vtable ase_variant_int_vtable;
ASELIB_DLLEXT extern const ase_variant_vtable ase_variant_long_vtable;
ASELIB_DLLEXT extern const ase_variant_vtable ase_variant_double_vtable;

inline ase_string&
ase_string::operator =(const ase_string& x)
{
  if (this != &x) {
    release_internal(rep);
    rep = x.rep;
    addref_internal(rep);
  }
  return *this;
}

inline bool
operator ==(const ase_string& x, const ase_string& y)
{
  if (x.size() != y.size()) { return false; }
  return (std::memcmp(x.data(), y.data(), x.size()) == 0);
}

inline bool
operator !=(const ase_string& x, const ase_string& y)
{
  return !(x == y);
}

inline bool
operator <(const ase_string& x, const ase_string& y)
{
  const ase_size_type len = x.size() < y.size() ? x.size() : y.size();
  int c = std::memcmp(x.data(), y.data(), len);
  if (c != 0) {
    return (c < 0);
  }
  return (x.size() < y.size());
}

inline bool
operator >(const ase_string& x, const ase_string& y)
{
  return y < x;
}

inline bool
operator <=(const ase_string& x, const ase_string& y)
{
  return !(y < x);
}

inline bool
operator >=(const ase_string& x, const ase_string& y)
{
  return !(x < y);
}

template <ase_size_type n> bool
ase_string::equals(const char *x, ase_size_type xlen, const char (& y)[n])
{
  return (xlen + 1 == n) && (std::memcmp(x, y, xlen) == 0);
}

template <ase_size_type n> bool
ase_string::operator ==(const char (& y)[n]) const
{
  return equals(data(), size(), y);
}

int
ase_string::compare(const ase_string& y) const
{
  const ase_size_type xlen = size();
  const ase_size_type ylen = y.size();
  if (xlen < ylen) { return -1; }
  if (xlen > ylen) { return 1; }
  return std::memcmp(data(), y.data(), xlen);
}

ase_string
operator +(const ase_string& x, const ase_string& y)
{
  return ase_string(x.data(), x.size(), y.data(), y.size());
}

template <ase_size_type n> ase_string
operator +(const ase_string& x, const char (& y)[n])
{
  return ase_string(x.data(), x.size(), y, n - 1);
}

template <ase_size_type n> ase_string
operator +(const char (& x)[n], const ase_string& y)
{
  return ase_string(x, n - 1, y.data(), y.size());
}

inline std::ostream& operator <<(std::ostream& s, const ase_string& v)
{
  return s << std::string(v.data(), v.size());
}

void
ase_variant::debug_on_create_object(const ase_variant& v, const char *debugid)
{
  /* called when ase_vtype_object/String is created */
  ASE_DEBUG_VARIANT_COUNT_DO(debug_createobj_internal(v, debugid));
}
void
ase_variant::debug_on_destroy_object(const ase_variant& v, const char *debugid)
{
  /* called when ase_vtype_object/String is destroyed */
  ASE_DEBUG_VARIANT_COUNT_DO(debug_destroyobj_internal(v, debugid));
}

void
ase_variant::addref_internal() const
{
  if (need_ref_count(vtbl->vtype)) {
    vtbl->add_ref(*this);
  }
}

void
ase_variant::release_internal() const
{
  if (need_ref_count(vtbl->vtype)) {
    vtbl->release(*this);
  }
}

ase_variant::ase_variant()
{
  vtbl = &ase_variant_default_vtable;
  rep.p = 0;
};

ase_variant::ase_variant(bool v)
{
  vtbl = &ase_variant_boolean_vtable;
  rep.i = v;
}

template <typename T>
ase_variant::ase_variant(const volatile T *p)
{
  T::NonexistentMethod();
  vtbl = &ase_variant_default_vtable;
  rep.p = 0;
}

ase_variant::ase_variant(ase_int v)
{
  vtbl = &ase_variant_int_vtable;
  rep.i = v;
}

ase_variant::ase_variant(ase_long v)
{
  vtbl = &ase_variant_long_vtable;
  rep.l = v;
}

ase_variant::ase_variant(double v)
{
  vtbl = &ase_variant_double_vtable;
  rep.d = v;
}

template <ase_size_type slen>
ase_variant::ase_variant(const char (& s)[slen])
{
  ase_string::init_variant_internal(s, slen - 1, *this);
}

ase_variant::ase_variant(const char *str, ase_size_type slen)
{
  ase_string::init_variant_internal(str, slen, *this);
}

ase_variant::ase_variant(const ase_variant_vtable *vt, void *re,
  const char *debugid)
{
  vtbl = vt;
  rep.p = re;
  debug_on_create_object(*this, debugid);
}

ase_variant
ase_variant::create_void()
{
  return ase_variant();
};

ase_variant
ase_variant::create_boolean(bool v)
{
  return ase_variant(v);
};

ase_variant
ase_variant::create_int(ase_int v)
{
  return ase_variant(v);
};

ase_variant
ase_variant::create_long(ase_long v)
{
  return ase_variant(v);
};

ase_variant
ase_variant::create_double(double v)
{
  return ase_variant(v);
};

ase_variant
ase_variant::create_object(const ase_variant_vtable *vtbl, void *rep,
  const char *debugid) /* nothrow */
{
  return ase_variant(vtbl, rep, debugid);
}

ase_variant&
ase_variant::operator =(const ase_variant& x)
{
  if (this != &x) {
    release_internal();
    vtbl = x.vtbl;
    rep = x.rep;
    addref_internal();
  }
  return *this;
}

bool
operator ==(const ase_variant& x, const ase_variant& y)
{
  return x.vtbl->is_same_object(x, y);
}

ase_int
ase_variant::get_attributes() const
{
  return vtbl->get_attributes(*this);
}

bool
ase_variant::get_boolean() const
{
  return (vtbl->vtype == ase_vtype_bool)
    ? (rep.i != 0)
    : (vtbl->get_boolean(*this));
}

ase_int
ase_variant::get_int() const
{
  return (vtbl->vtype == ase_vtype_int)
    ? (rep.i)
    : vtbl->get_int(*this);
}

ase_long
ase_variant::get_long() const
{
  return (vtbl->vtype == ase_vtype_long)
    ? (rep.l)
    : vtbl->get_long(*this);
}

double
ase_variant::get_double() const
{
  return (vtbl->vtype == ase_vtype_double)
    ? (rep.d)
    : vtbl->get_double(*this);
}

ase_string
ase_variant::get_string() const
{
  return vtbl->get_string(*this);
}

bool
ase_variant::is_same_object(const ase_variant& val) const
{
  return vtbl->is_same_object(*this, val);
}

ase_variant
ase_variant::get_property(const char *name, ase_size_type namelen) const
{
  return vtbl->get_property(*this, name, namelen);
}

ase_variant
ase_variant::get_property(const ase_string& name) const
{
  return get_property(name.data(), name.size());
}

void
ase_variant::set_property(const char *name, ase_size_type namelen,
  const ase_variant& val) const
{
  vtbl->set_property(*this, name, namelen, val);
}

void
ase_variant::set_property(const ase_string& name, const ase_variant& val) const
{
  set_property(name.data(), name.size(), val);
}

void
ase_variant::del_property(const char *name, ase_size_type namelen) const
{
  vtbl->del_property(*this, name, namelen);
}

void
ase_variant::del_property(const ase_string& name) const
{
  del_property(name.data(), name.size());
}

ase_variant
ase_variant::get_element(ase_int idx) const
{
  return vtbl->get_element(*this, idx);
}

void
ase_variant::set_element(ase_int idx, const ase_variant& val) const
{
  vtbl->set_element(*this, idx, val);
}

ase_int
ase_variant::get_length() const
{
  return vtbl->get_length(*this);
}

ase_int
ase_variant::get_length_or_zero() const
{
  ase_int r = vtbl->get_length(*this);
  return r >= 0 ? r : 0;
}

void
ase_variant::set_length(ase_int len) const
{
  vtbl->set_length(*this, len);
}

template <ase_size_type n> ase_int
ase_variant::get_method_id(const char (& name)[n]) const
{
  return get_method_id(name, n - 1);
}

ase_int
ase_variant::get_method_id(const ase_string& name) const
{
  return get_method_id(name.data(), name.size());
}

template <ase_size_type n> ase_int
ase_variant::get_instance_method_id(const char (& name)[n]) const
{
  return get_instance_method_id(name, n - 1);
}

ase_int
ase_variant::get_instance_method_id(const ase_string& name) const
{
  return get_instance_method_id(name.data(), name.size());
}

ase_variant
ase_variant::invoke_by_name(const char *name, ase_size_type namelen,
  const ase_variant *args, ase_size_type nargs) const
{
  return vtbl->invoke_by_name(*this, name, namelen, args, nargs);
}

ase_variant
ase_variant::invoke_by_name(const ase_string& name, const ase_variant *args,
  ase_size_type nargs) const
{
  return invoke_by_name(name.data(), name.size(), args, nargs);
}

ase_variant
ase_variant::invoke_by_id(ase_int idx, const ase_variant *args,
  ase_size_type nargs) const
{
  return vtbl->invoke_by_id(*this, idx, args, nargs);
}

ase_variant
ase_variant::invoke_self(const ase_variant *args, ase_size_type nargs) const
{
  return vtbl->invoke_self(*this, args, nargs);
}

ase_variant
ase_variant::get_enumerator() const
{
  return vtbl->get_enumerator(*this);
}

ase_string
ase_variant::enum_next(bool& hasnext_r) const
{
  return vtbl->enum_next(*this, hasnext_r);
}

template <ase_size_type namelen> ase_variant
ase_variant::get(const char (& name)[namelen]) const
{
  return get_property(name, namelen - 1);
}

ase_variant
ase_variant::get(ase_int idx) const
{
  return get_element(idx);
}

template <ase_size_type namelen> void
ase_variant::set(const char (& name)[namelen], const ase_variant& val) const
{
  set_property(name, namelen - 1, val);
}

void
ase_variant::set(ase_int idx, const ase_variant& val) const
{
  set_element(idx, val);
}

template <ase_size_type namelen> ase_variant
ase_variant::minvoke(const char (& name)[namelen]) const
{
  return invoke_by_name(name, namelen - 1, 0, 0);
}

template <ase_size_type namelen> ase_variant
ase_variant::minvoke(const char (& name)[namelen],
  const ase_variant& arg0) const
{
  return invoke_by_name(name, namelen - 1, &arg0, 1);
}

template <ase_size_type namelen> ase_variant
ase_variant::minvoke(const char (& name)[namelen],
  const ase_variant& arg0, const ase_variant& arg1) const
{
  ASE_ALLOCA(ase_variant, 2, args);
  args[0].vtbl = arg0.vtbl;
  args[0].rep = arg0.rep;
  args[1].vtbl = arg1.vtbl;
  args[1].rep = arg1.rep;
  return invoke_by_name(name, namelen - 1, args, 2);
}

template <ase_size_type namelen> ase_variant
ase_variant::minvoke(const char (& name)[namelen],
  const ase_variant& arg0, const ase_variant& arg1, const ase_variant& arg2)
  const
{
  ASE_ALLOCA(ase_variant, 3, args);
  args[0].vtbl = arg0.vtbl;
  args[0].rep = arg0.rep;
  args[1].vtbl = arg1.vtbl;
  args[1].rep = arg1.rep;
  args[2].vtbl = arg2.vtbl;
  args[2].rep = arg2.rep;
  return invoke_by_name(name, namelen - 1, args, 3);
}

template <ase_size_type namelen> ase_variant
ase_variant::minvoke(const char (& name)[namelen],
  const ase_variant& arg0, const ase_variant& arg1, const ase_variant& arg2,
  const ase_variant& arg3) const
{
  ASE_ALLOCA(ase_variant, 4, args);
  args[0].vtbl = arg0.vtbl;
  args[0].rep = arg0.rep;
  args[1].vtbl = arg1.vtbl;
  args[1].rep = arg1.rep;
  args[2].vtbl = arg2.vtbl;
  args[2].rep = arg2.rep;
  args[3].vtbl = arg3.vtbl;
  args[3].rep = arg3.rep;
  return invoke_by_name(name, namelen - 1, args, 4);
}

ase_variant
ase_variant::minvoke(ase_int id) const
{
  return invoke_by_id(id, 0, 0);
}

ase_variant
ase_variant::minvoke(ase_int id, const ase_variant& arg0) const
{
  return invoke_by_id(id, &arg0, 1);
}

ase_variant
ase_variant::minvoke(ase_int id, const ase_variant& arg0,
  const ase_variant& arg1) const
{
  ASE_ALLOCA(ase_variant, 2, args);
  args[0].vtbl = arg0.vtbl;
  args[0].rep = arg0.rep;
  args[1].vtbl = arg1.vtbl;
  args[1].rep = arg1.rep;
  return invoke_by_id(id, args, 2);
}

ase_variant
ase_variant::minvoke(ase_int id, const ase_variant& arg0,
  const ase_variant& arg1, const ase_variant& arg2) const
{
  ASE_ALLOCA(ase_variant, 3, args);
  args[0].vtbl = arg0.vtbl;
  args[0].rep = arg0.rep;
  args[1].vtbl = arg1.vtbl;
  args[1].rep = arg1.rep;
  args[2].vtbl = arg2.vtbl;
  args[2].rep = arg2.rep;
  return invoke_by_id(id, args, 3);
}

ase_variant
ase_variant::minvoke(ase_int id, const ase_variant& arg0,
  const ase_variant& arg1, const ase_variant& arg2, const ase_variant& arg3)
  const
{
  ASE_ALLOCA(ase_variant, 4, args);
  args[0].vtbl = arg0.vtbl;
  args[0].rep = arg0.rep;
  args[1].vtbl = arg1.vtbl;
  args[1].rep = arg1.rep;
  args[2].vtbl = arg2.vtbl;
  args[2].rep = arg2.rep;
  args[3].vtbl = arg3.vtbl;
  args[3].rep = arg3.rep;
  return invoke_by_id(id, args, 4);
}

ase_variant
ase_variant::invoke() const
{
  return invoke_self(0, 0);
}

ase_variant
ase_variant::invoke(const ase_variant& arg0) const
{
  return invoke_self(&arg0, 1);
}

ase_variant
ase_variant::invoke(const ase_variant& arg0, const ase_variant& arg1)
  const
{
  ASE_ALLOCA(ase_variant, 2, args);
  args[0].vtbl = arg0.vtbl;
  args[0].rep = arg0.rep;
  args[1].vtbl = arg1.vtbl;
  args[1].rep = arg1.rep;
  return invoke_self(args, 2);
}

ase_variant
ase_variant::invoke(const ase_variant& arg0, const ase_variant& arg1,
  const ase_variant& arg2) const
{
  ASE_ALLOCA(ase_variant, 3, args);
  args[0].vtbl = arg0.vtbl;
  args[0].rep = arg0.rep;
  args[1].vtbl = arg1.vtbl;
  args[1].rep = arg1.rep;
  args[2].vtbl = arg2.vtbl;
  args[2].rep = arg2.rep;
  return invoke_self(args, 3);
}

ase_variant
ase_variant::invoke(const ase_variant& arg0, const ase_variant& arg1,
  const ase_variant& arg2, const ase_variant& arg3) const
{
  ASE_ALLOCA(ase_variant, 4, args);
  args[0].vtbl = arg0.vtbl;
  args[0].rep = arg0.rep;
  args[1].vtbl = arg1.vtbl;
  args[1].rep = arg1.rep;
  args[2].vtbl = arg2.vtbl;
  args[2].rep = arg2.rep;
  args[3].vtbl = arg3.vtbl;
  args[3].rep = arg3.rep;
  return invoke_self(args, 4);
}

inline std::ostream& operator <<(std::ostream& s, const ase_variant& v)
{
  return s << v.get_string();
}

template <typename T, ase_int opt> const ase_variant_vtable
ase_variant_impl<T, opt>::vtable = {
  0, ase_vtype_object, opt, { 0, 0 }, 0,
  T::add_ref,
  T::release,
  T::get_attributes,
  T::get_boolean,
  T::get_int,
  T::get_long,
  T::get_double,
  T::get_string,
  T::is_same_object,
  T::get_property,
  T::set_property,
  T::del_property,
  T::get_element,
  T::set_element,
  T::get_length,
  T::set_length,
  T::invoke_by_name,
  T::invoke_by_id,
  T::invoke_self,
  T::get_enumerator,
  T::enum_next,
};

template <typename T>
struct ase_auto_ptr {

  explicit ase_auto_ptr(T *p = 0) : ptr(p) { }
  ase_auto_ptr(ase_auto_ptr& x) : ptr(x.release()) { }
  ~ase_auto_ptr() { destroy(); }
  ase_auto_ptr& operator =(ase_auto_ptr& x) {
    reset(x.release());
    return *this;
  }
  T& operator *() const { return *ptr; }
  T *operator ->() const { return ptr; }
  T *get() const { return ptr; }
  T *release() {
    T *r = ptr;
    ptr = 0;
    return r;
  }
  void reset(T *p = 0) {
    if (ptr != p) {
      destroy();
      ptr = p;
    }
  }

 private:

  T *ptr;
  void destroy() {
    if (ptr) {
      ptr->destroy();
    }
  }

};

template <typename T>
struct ase_shared_ptr {

  explicit ase_shared_ptr(T *p) : ptr(p) { }
  ase_shared_ptr(T *p, bool addref) : ptr(p) { if (addref) { ptr->add_ref(); } }
  ase_shared_ptr(const ase_shared_ptr& x) : ptr(x.ptr) { ptr->add_ref(); }
  ~ase_shared_ptr() { ptr->release(); }
  ase_shared_ptr& operator =(ase_shared_ptr& x) {
    if (&x != this) {
      ptr->release();
      ptr = x;
      ptr->add_ref();
    }
    return *this;
  }
  T& operator *() const { return *ptr; }
  T *operator ->() const { return ptr; }
  T *get() const { return ptr; }

 private:

  T *ptr;

};

template <typename T>
struct ase_intrusive_count : ase_noncopyable {

  friend struct ase_shared_ptr<T>;

  ase_intrusive_count() : refcount(1) { }

 protected:

  void add_ref() {
    refcount++;
  }

  void release() {
    if ((refcount--) == 1) {
      delete static_cast<T *>(this);
    }
  }

  ase_atomic_count refcount;

};

#endif

