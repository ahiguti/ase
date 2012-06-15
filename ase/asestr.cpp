
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/ase.hpp>
#include <ase/aseexcept.hpp>
#include <ase/aseflags.hpp>

#include "aseutfconv.hpp"

#define DBG_REP(x)
#define DBG_DUMP(x)

namespace {

ase_atomic_count ase_stringrep_count_constr = 0;
ase_atomic_count ase_stringrep_count_destr = 0;

}; // anonymous namespace

struct ase_string::impl_util {

  static impl_type *impl_new(ase_size_type slen);
  static impl_type *impl_resize(impl_type *p);
  static void impl_destroy(impl_type *p);
  static impl_type *impl_new_utf8(const char *s, ase_size_type slen);
  static impl_type *impl_new_utf8(const char *s1, ase_size_type len1,
    const char *s2, ase_size_type len2);
  static void dump(impl_type *p, const char *mess);

};

ase_string::impl_type *
ase_string::impl_util::impl_new(ase_size_type slen)
{
  impl_type *rep = static_cast<impl_type *>(
    malloc(slen + sizeof(impl_type) - 1 + 1));
    /* plus one for terminating nul */
  if (rep) {
    rep->refcount = 1;
    rep->len = slen;
    ASE_DEBUG(ase_stringrep_count_constr++);
  }
  return rep;
}

ase_string::impl_type *
ase_string::impl_util::impl_resize(impl_type *p)
{
  ase_size_type slen = p->len;
  impl_type *np = static_cast<impl_type *>(
    realloc(p, slen + sizeof(impl_type) - 1 + 1));
    /* plus one for terminating nul */
  if (!np) {
    free(p);
  }
  return np;
}

void
ase_string::impl_util::impl_destroy(impl_type *p)
{
  if (p) {
    ASE_DEBUG(ase_stringrep_count_destr++);
  }
  free(p);
}

ase_string::impl_type *
ase_string::impl_util::impl_new_utf8(const char *s, ase_size_type slen)
{
  impl_type *rep = impl_util::impl_new(slen);
  if (!rep) {
    return 0;
  }
  std::memcpy(rep->buf, s, slen);
  rep->buf[slen] = '\0';
  return rep;
}

ase_string::impl_type *
ase_string::impl_util::impl_new_utf8(const char *s1, ase_size_type len1,
  const char *s2, ase_size_type len2)
{
  impl_type *rep = impl_util::impl_new(len1 + len2);
  if (!rep) {
    return 0;
  }
  std::memcpy(rep->buf, s1, len1);
  std::memcpy(rep->buf + len1, s2, len2);
  rep->buf[len1 + len2] = '\0';
  return rep;
}

void
ase_string::impl_util::dump(impl_type *p, const char *mess)
{
  if (!p) {
    return;
  }
  fprintf(stderr, "STRREP %s: %p %s\n", mess, p, p->buf);
}

class ase_variant_impl_string {

 public:

  friend class ase_string;

  static ase_string::impl_type *get(const ase_variant& v);

  static void add_ref(const ase_variant& v);
  static void release(const ase_variant& v);
  static bool get_boolean(const ase_variant& v);
  static ase_int get_int(const ase_variant& v);
  static ase_long get_long(const ase_variant& v);
  static double get_double(const ase_variant& v);
  static ase_string get_string(const ase_variant& v);

 private:

  static const ase_variant_vtable vtable;

};

ase_string::ase_string(impl_type *re)
  : rep(re)
{
  DBG_DUMP(impl_util::impl_new(rep, "create"));
}

void
ase_string::addref_internal(impl_type *rep)
{
  if (rep->refcount != -1) { /* -1 is for emptyrep */
    ase_atomic::locked_xadd(rep->refcount, 1);
  }
}

void
ase_string::release_internal(impl_type *rep)
{
  if (rep->refcount != -1) { /* -1 is for emptyrep */
    if (ase_atomic::locked_xadd(rep->refcount, -1) == 1) {
      DBG_REP(fprintf(stderr, "STR del %p\n", rep));
      DBG_DUMP(impl_util::dump(rep, "destroy"));
      impl_util::impl_destroy(rep);
    }
  }
}

ase_variant
ase_string::to_variant() const
{
  addref_internal(rep);
  const char *const debugid = 0;
    /* because this is not a newly created object, debug_count_* should ignore
     * this */
  return ase_variant::create_object(&ase_variant_impl_string::vtable, rep,
    debugid);
}

ase_string::impl_type *
ase_variant_impl_string::get(const ase_variant& v)
{
  return static_cast<ase_string::impl_type *>(v.get_rep().p);
}

void
ase_variant_impl_string::add_ref(const ase_variant& v)
{
  ase_string::impl_type *p = get(v);
  ase_string::addref_internal(p);
}

void
ase_variant_impl_string::release(const ase_variant& v)
{
  ase_string::impl_type *p = get(v);
  ase_string::release_internal(p);
}

bool
ase_variant_impl_string::get_boolean(const ase_variant& v)
{
  ase_string::impl_type *p = get(v);
  return p->len != 0;
}

ase_int
ase_variant_impl_string::get_int(const ase_variant& v)
{
  ase_string::impl_type *p = get(v);
  return std::atoi(p->buf);
}

ase_long
ase_variant_impl_string::get_long(const ase_variant& v)
{
  ase_string::impl_type *p = get(v);
#ifdef _MSC_VER
  return _atoi64(p->buf);
#else
  return std::atoll(p->buf);
#endif
}

double
ase_variant_impl_string::get_double(const ase_variant& v)
{
  ase_string::impl_type *p = get(v);
  return std::atof(p->buf);
}

ase_string
ase_variant_impl_string::get_string(const ase_variant& v)
{
  ase_string::impl_type *p = get(v);
  ase_string::addref_internal(p);
  return ase_string(p);
}

const ase_variant_vtable
ase_variant_impl_string::vtable = {
  0, ase_vtype_string, ase_vtoption_none, { 0, 0 }, 0,
  add_ref,
  release,
  ase_variant_impl_default::get_attributes,
  get_boolean,
  get_int,
  get_long,
  get_double,
  get_string,
  ase_variant_impl_default::is_same_object,
  ase_variant_impl_default::get_property,
  ase_variant_impl_default::set_property,
  ase_variant_impl_default::del_property,
  ase_variant_impl_default::get_element,
  ase_variant_impl_default::set_element,
  ase_variant_impl_default::get_length,
  ase_variant_impl_default::set_length,
  ase_variant_impl_default::invoke_by_name,
  ase_variant_impl_default::invoke_by_id,
  ase_variant_impl_default::invoke_self,
  ase_variant_impl_default::get_enumerator,
  ase_variant_impl_default::enum_next,
};

ase_string::impl_type
ase_string::emptyrep = {
  -1,     /* refcount */
  0,      /* len */
  { 0 }   /* buf */
};

void
ase_string::init_8(const char *str, ase_size_type len)
{
  rep = impl_util::impl_new_utf8(str, len);
  if (!rep) {
    rep = &emptyrep;
    throw std::bad_alloc();
  }
}

void
ase_string::init_8(const char *s1, ase_size_type len1, const char *s2,
  ase_size_type len2)
{
  rep = impl_util::impl_new_utf8(s1, len1, s2, len2);
  if (!rep) {
    rep = &emptyrep;
    throw std::bad_alloc();
  }
}

char *
ase_string::init_8_retbuf(ase_size_type len)
{
  rep = impl_util::impl_new(len);
  if (!rep) {
    rep = &emptyrep;
    throw std::bad_alloc();
  }
  return rep->buf;
}

void
ase_string::init_16(const ase_ucchar *str, ase_size_type len)
{
  rep = 0;
  const ase_size_type buflen = len * 3;
  rep = impl_util::impl_new(buflen);
  if (!rep) {
    rep = &emptyrep;
    throw std::bad_alloc();
  }
  rep->len = 0;
  const ase_ucchar *const start = str;
  const ase_size_type u16len = len;
  pmc_u8ch_t *u8buf = reinterpret_cast<pmc_u8ch_t *>(rep->buf);
  const pmc_u16ch_t *u16pos = 0;
  pmc_u8ch_t *u8pos = 0;
  pmc_uconv_err_t err = pmc_uconv_16to8(start, start + u16len, u8buf,
    u8buf + buflen, &u16pos, &u8pos);
  rep->len = (u8pos - u8buf);
  rep->buf[rep->len] = '\0';
  if (err != pmc_uconv_err_success) {
    /* NOTE: this func is called from constr and we must free rep before
      throwing an exception */
    impl_util::impl_destroy(rep);
    rep = &emptyrep;
    throw ase_new_exception(ase_string("UTF8Conv"),
      "(ASERuntime) failed to convert to utf8: " +
      ase_string(pmc_uconv_error_string(err)));
  }
  rep = impl_util::impl_resize(rep);
  if (!rep) {
    rep = &emptyrep;
    throw std::bad_alloc();
  }
}

void
ase_string::show_statistics()
{
  ASE_DEBUG(fprintf(stderr, "STR      c = %d\n",
    (int)ase_stringrep_count_constr));
  ASE_DEBUG(fprintf(stderr, "STR      d = %d\n",
    (int)ase_stringrep_count_destr));
}

void
ase_string::init_variant_internal(const char *str, ase_size_type len,
  ase_variant& v)
{
  v.vtbl = &ase_variant_default_vtable;
  v.rep.p = 0;
  impl_type *srep = impl_util::impl_new_utf8(str, len);
  if (!srep) {
    throw std::bad_alloc();
  }
  DBG_REP(fprintf(stderr, "STR newl1v %p\n", srep));
  /* see ase_variant::create_object */
  v.vtbl = &ase_variant_impl_string::vtable;
  v.rep.p = srep;
}

