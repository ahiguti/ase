
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asestrmap.hpp>
#include <ase/asetyped.hpp>
#include <ase/asemutex.hpp>
#include <ase/aseexcept.hpp>

#include <map>
#include <vector>

namespace {

typedef std::map<ase_string, ase_variant> strmap_type;

ase_string
ase_strmap_to_string(const strmap_type& m)
{
  std::vector<char> buf;
  buf.push_back('{');
  size_t cnt = 0;
  for (strmap_type::const_iterator i = m.begin(); i != m.end(); ++i, ++cnt) {
    if (cnt != 0) {
      buf.push_back(',');
    }
    const ase_string& k = i->first;
    const ase_string& v = ase_nothrow_get_string(i->second);
    buf.insert(buf.end(), k.data(), k.data() + k.size());
    buf.push_back('=');
    buf.insert(buf.end(), v.data(), v.data() + v.size());
  }
  buf.push_back('}');
  return ase_string(&buf[0], buf.size());
}

struct ase_smenum_varimpl;
ase_variant ase_new_smenum(const ase_variant& mapval /* StringMap */);

struct ase_strmap_varimpl : public ase_variant_impl_typed<ase_strmap_varimpl> {

  friend struct ase_smenum_varimpl;

  ase_strmap_varimpl() {
  }
  ~ase_strmap_varimpl() /* DCHK */ {
  }

  static ase_string get_string(const ase_variant& v) {
    ase_strmap_varimpl *selfp = get(v);
    return ase_strmap_to_string(selfp->strmap);
  }
  static ase_variant get_property(const ase_variant& v, const char *name,
    ase_size_type len) {
    ase_strmap_varimpl *selfp = get(v);
    ase_mutex_guard<ase_fast_mutex> g(selfp->mutex);
    ase_string s(name, len);
    strmap_type::const_iterator iter = selfp->strmap.find(s);
    if (iter == selfp->strmap.end()) {
      return ase_variant();
    }
    return iter->second;
  }
  static void set_property(const ase_variant& v, const char *name,
    ase_size_type len, const ase_variant& value) {
    ase_strmap_varimpl *selfp = get(v);
    ase_string s(name, len);
    ase_mutex_guard<ase_fast_mutex> g(selfp->mutex);
    selfp->strmap[s] = value;
  }
  static ase_variant invoke_by_name(const ase_variant& v, const char *name,
    ase_size_type len, const ase_variant *args, ase_size_type nargs) {
    return get_property(v, name, len).invoke_self(args, nargs);
  }
  static ase_variant get_enumerator(const ase_variant& v) {
    return ase_new_smenum(v);
  }

  ase_fast_mutex mutex; /* TODO: asestrmap need not to be multi-thread safe */
  strmap_type strmap;
  static const char *const debugid;

};

const char *const ase_strmap_varimpl::debugid = "StrMap";

struct ase_strmap_unmod_varimpl : public ase_strmap_varimpl {

  static void set_property(const ase_variant& v, const char *name,
    ase_size_type len, const ase_variant& value) {
    ase_throw_illegal_operation("ASEStrMap: unmodifiable");
  }

};


struct ase_smenum_varimpl : public ase_variant_impl_typed<ase_smenum_varimpl> {

  static ase_string get_string(const ase_variant& v) {
    return ase_string("[ASEStrMapEnum]");
  }
  static ase_string enum_next(const ase_variant& v, bool& hasnext_r) {
    ase_smenum_varimpl *selfp = get(v);
    ase_strmap_varimpl *mapp = ase_strmap_varimpl::get(selfp->mapval);
    hasnext_r = false;
    const strmap_type& sm = mapp->strmap;
    strmap_type::const_iterator iter = sm.upper_bound(selfp->key);
    if (iter == sm.end()) {
      return ase_string();
    }
    selfp->key = iter->first;
    hasnext_r = true;
    return selfp->key;
  }

  ase_variant mapval;
  ase_string key;
  static const char *const debugid;

};

const char *const ase_smenum_varimpl::debugid = "StrMapEnum";

ase_variant
ase_new_smenum(const ase_variant& mapval /* must be a StringMap */)
{
  ase_smenum_varimpl *sme = new ase_smenum_varimpl();
  sme->mapval = mapval;
  return ase_variant::create_object(
    &ase_variant_impl<ase_smenum_varimpl>::vtable,
    sme,
    ase_smenum_varimpl::debugid);
}

const ase_variant_vtable *
get_vtbl(const ase_variant& m)
{
  const void *xvt = m.get_vtable_address();
  const ase_variant_vtable *vtbl = 0;
  if (xvt == &ase_variant_impl<ase_strmap_varimpl>::vtable) {
    vtbl = &ase_variant_impl<ase_strmap_unmod_varimpl>::vtable;
  } else {
    ase_throw_type_mismatch("ASEStrMap: not a strmap object");
  }
  return vtbl;
}

}; // anonymous namespace

ase_variant
ase_new_string_map()
{
  return ase_variant::create_object(
    &ase_variant_impl<ase_strmap_varimpl>::vtable,
    new ase_strmap_varimpl(),
    ase_strmap_varimpl::debugid);
}

ase_variant
ase_string_map_unmodifiable(const ase_variant& m)
{
  const ase_variant_vtable *const vtbl = get_vtbl(m);
  ase_strmap_varimpl *sm = static_cast<ase_strmap_varimpl *>(m.get_rep().p);
  sm->add_ref(m);
  return ase_variant::create_object(vtbl, sm, 0);
}

void
ase_string_map_clear(const ase_variant& m)
{
  get_vtbl(m);
  ase_strmap_varimpl *sm = static_cast<ase_strmap_varimpl *>(m.get_rep().p);
  sm->strmap.clear();
}

