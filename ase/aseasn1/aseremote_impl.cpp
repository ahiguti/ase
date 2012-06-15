
#include <ase/asearray.hpp>
#include <ase/asestrmap.hpp>

#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <stdexcept>
#include <asn_internal.h> /* MALLOC */
#include <ASERemoteVariant.h>
#include "INTEGER_ex.hpp"
#include "aseremote_impl.hpp"

namespace aseremote {

ase_remote_variant::ase_remote_variant()
  : ptr(0)
{
}
ase_remote_variant::~ase_remote_variant()
{
  reset();
}

void
ase_remote_variant::set_void()
{
  reallocate();
  ptr->present = ASERemoteVariant_PR_voidval;
}

void
ase_remote_variant::set_bool(bool v)
{
  reallocate();
  ptr->present = ASERemoteVariant_PR_boolval;
  ptr->choice.boolval = static_cast<int>(v);
}

void
ase_remote_variant::set_int(long long v)
{
  reallocate();
  ptr->present = ASERemoteVariant_PR_intval;
  if (asn_integral2INTEGER(&ptr->choice.intval, v) != 0) {
    check_error_internal(__FUNCTION__);
  }
}

void
ase_remote_variant::set_real(double v)
{
  reallocate();
  ptr->present = ASERemoteVariant_PR_realval;
  if (asn_double2REAL(&ptr->choice.realval, v) != 0) {
    check_error_internal(__FUNCTION__);
  }
}

void
ase_remote_variant::set_string(const char *str, size_t len)
{
  reallocate();
  ptr->present = ASERemoteVariant_PR_strval;
  if (OCTET_STRING_fromBuf(&ptr->choice.strval, str, len) != 0) {
    check_error_internal(__FUNCTION__);
  }
}

void
ase_remote_variant::set_object(bool remote, long long id)
{
  reallocate();
  ptr->present = ASERemoteVariant_PR_objval;
  ptr->choice.objval.remote = static_cast<int>(remote);
  if (asn_integral2INTEGER(&ptr->choice.objval.id, id) != 0) {
    check_error_internal(__FUNCTION__);
  }
}

void
ase_remote_variant::set_array()
{
  reallocate();
  ptr->present = ASERemoteVariant_PR_arrval;
}

void
ase_remote_variant::set_map()
{
  reallocate();
  ptr->present = ASERemoteVariant_PR_mapval;
}

void
ase_remote_variant::dump_xer() const
{
  xer_fprint(stdout, &asn_DEF_ASERemoteVariant, ptr);
}

size_t
ase_remote_variant::get_der_size() const
{
  const asn_enc_rval_t r = der_encode(&asn_DEF_ASERemoteVariant, ptr,
    0, 0);
  if (r.encoded <= 0) {
    return 0;
  }
  const size_t s = r.encoded;
  return s;
}

std::string
ase_remote_variant::get_der_stdstring() const
{
  const size_t sz = get_der_size();
  if (sz == 0) {
    return std::string();
  }
  std::string str;
  str.resize(sz);
  const asn_enc_rval_t r2 = der_encode_to_buffer(&asn_DEF_ASERemoteVariant,
    ptr, &str[0], sz);
  if (r2.encoded <= 0) {
    return std::string();
  }
  return str;
}

struct get_size_udata {
  size_t size;
};

int
get_size_cb(const void *buffer, size_t size, void *udata)
{
  get_size_udata *const p = static_cast<get_size_udata *>(udata);
  p->size += size;
  return 0;
}

struct enc_to_buf_udata {
  char *buffer;
  size_t left;
};

int
enc_to_buf_cb(const void *buffer, size_t size, void *udata)
{
  enc_to_buf_udata *const p = static_cast<enc_to_buf_udata *>(udata);
  if (p->left < size) {
    return -1;
  }
  std::memcpy(p->buffer, buffer, size);
  p->buffer += size;
  p->left -= size;
  return 0;
}

size_t
ase_remote_variant::get_xer_size() const
{
  get_size_udata ud;
  ud.size = 0;
  const asn_enc_rval_t r = xer_encode(&asn_DEF_ASERemoteVariant, ptr,
    XER_F_CANONICAL, get_size_cb, &ud);
  if (r.encoded <= 0) {
    return 0;
  }
  return ud.size;
}

std::string
ase_remote_variant::get_xer_stdstring() const
{
  const size_t sz = get_xer_size();
  if (sz == 0) {
    return std::string();
  }
  std::string str;
  str.resize(sz);
  enc_to_buf_udata ud;
  ud.buffer = &str[0];
  ud.left = sz;
  const asn_enc_rval_t r2 = xer_encode(&asn_DEF_ASERemoteVariant,
    ptr, XER_F_CANONICAL, enc_to_buf_cb, &ud);
  if (r2.encoded <= 0) {
    return std::string();
  }
  return str;
}

bool
ase_remote_variant::set_from_ber(const char *str, size_t slen)
{
  reset();
  const asn_dec_rval_t r = asn_DEF_ASERemoteVariant.ber_decoder(0,
    &asn_DEF_ASERemoteVariant, reinterpret_cast<void **>(&ptr),
    str, slen, 0);
  return (r.code == RC_OK);
}

bool
ase_remote_variant::set_from_xer(const char *str, size_t slen)
{
  reset();
  const asn_dec_rval_t r = xer_decode(0, &asn_DEF_ASERemoteVariant,
    reinterpret_cast<void **>(&ptr), str, slen);
  return (r.code == RC_OK);
}

const ASERemoteVariant *
ase_remote_variant::get() const
{
  return ptr;
}

void
ase_remote_variant::reset()
{
  asn_DEF_ASERemoteVariant.free_struct(&asn_DEF_ASERemoteVariant, ptr, 0);
  ptr = 0;
}

void
ase_remote_variant::reallocate()
{
  reset();
  ptr = static_cast<ASERemoteVariant_t *>(MALLOC(sizeof(*ptr)));
  if (ptr == 0) {
    throw std::bad_alloc();
  }
  std::memset(ptr, 0, sizeof(*ptr));
  ptr->present = ASERemoteVariant_PR_NOTHING;
}

void
ase_remote_variant::push_back(ase_remote_variant& v)
{
  if (ptr->present != ASERemoteVariant_PR_arrval &&
    ptr->present != ASERemoteVariant_PR_mapval) {
    return;
  }
  if (asn_sequence_add(&ptr->choice.arrval, v.ptr) != 0) {
    check_error_internal(__FUNCTION__);
  }
  v.ptr = 0;
}

void
ase_remote_variant::check_error_internal(const char *func)
{
  if (errno == ENOMEM) {
    throw std::bad_alloc();
  } else {
    throw std::runtime_error(func);
  }
}

int
get_type(const ASERemoteVariant_t *ptr)
{
  return ptr ? ptr->present : ASERemoteVariant_PR_NOTHING;
}

bool
get_bool(const ASERemoteVariant_t *ptr)
{
  if (get_type(ptr) != ASERemoteVariant_PR_boolval) {
    return false;
  }
  return (ptr->choice.boolval != 0);
}

long long
get_int(const ASERemoteVariant_t *ptr)
{
  if (get_type(ptr) != ASERemoteVariant_PR_intval) {
    return 0;
  }
  long long v = 0;
  if (asn_INTEGER2integral(&ptr->choice.intval, &v) != 0) {
    return 0;
  }
  return v;
}

double
get_real(const ASERemoteVariant_t *ptr)
{
  if (get_type(ptr) != ASERemoteVariant_PR_realval) {
    return 0.0;
  }
  double v = 0;
  if (asn_REAL2double(&ptr->choice.realval, &v) != 0) {
    return 0.0;
  }
  return v;
}

void
get_string(const ASERemoteVariant_t *ptr, uint8_t *& buf_r, int& len_r)
{
  buf_r = 0;
  len_r = 0;
  if (get_type(ptr) != ASERemoteVariant_PR_strval) {
    return;
  }
  buf_r = ptr->choice.strval.buf;
  len_r = ptr->choice.strval.size;
}

void
get_object(const ASERemoteVariant_t *ptr, bool& remote_r, long long& id_r)
{
  remote_r = false;
  id_r = 0;
  if (get_type(ptr) != ASERemoteVariant_PR_objval) {
    return;
  }
  if (asn_INTEGER2integral(&ptr->choice.objval.id, &id_r) != 0) {
    id_r = 0;
  } else {
    remote_r = (ptr->choice.objval.remote != 0);
  }
}

void
get_array(const ASERemoteVariant_t *ptr, ASERemoteVariant_t **& arr_r,
  int& size_r)
{
  arr_r = 0;
  size_r = 0;
  const int t = get_type(ptr);
  if (t == ASERemoteVariant_PR_arrval) {
    arr_r = ptr->choice.arrval.list.array;
    size_r = ptr->choice.arrval.list.count;
  } else if (t == ASERemoteVariant_PR_mapval) {
    arr_r = ptr->choice.mapval.list.array;
    size_r = ptr->choice.mapval.list.count;
  }
}

void
to_remote_variant(const ase_variant& value, ase_object_serializer *om,
  ase_remote_variant& rv_r)
{
  switch (value.get_type()) {
  case ase_vtype_void:
    rv_r.set_void();
    break;
  case ase_vtype_bool:
    rv_r.set_bool(value.get_boolean());
    break;
  case ase_vtype_int:
    rv_r.set_int(value.get_int());
    break;
  case ase_vtype_long:
    rv_r.set_int(value.get_long());
    break;
  case ase_vtype_double:
    rv_r.set_real(value.get_double());
    break;
  case ase_vtype_string:
    {
      const ase_string s = value.get_string();
      rv_r.set_string(s.data(), s.size());
    }
    break;
  case ase_vtype_object:
    {
      bool is_remote = false;
      ase_long id = 0;
      if (om != 0 && om->serialize_object(value, is_remote, id)) {
	rv_r.set_object(is_remote, id);
	break;
      }
      const ase_int sz = value.get_length();
      if (sz >= 0) {
	/* array */
	rv_r.set_array();
	for (ase_int i = 0; i < sz; ++i) {
	  const ase_variant val = value.get_element(i);
	  ase_remote_variant v;
	  to_remote_variant(val, om, v);
	  rv_r.push_back(v);
	}
	break;
      }
      const ase_variant en = value.get_enumerator();
      if (!en.is_void()) {
	/* assoc container */
	rv_r.set_map();
	while (true) {
	  bool hasnext = false;
	  const ase_string key = en.enum_next(hasnext);
	  if (!hasnext) {
	    break;
	  }
	  const ase_variant val = value.get_property(key);
	  ase_remote_variant k, v;
	  k.set_string(key.data(), key.size());
	  to_remote_variant(val, om, v);
	  rv_r.push_back(k);
	  rv_r.push_back(v);
	}
	break;
      }
      rv_r.set_void();
    }
    break;
  }
}

void
to_remote_variant(const ase_variant *start, const ase_variant *finish,
  ase_object_serializer *om, ase_remote_variant& rv_r)
{
  rv_r.set_array();
  while (start != finish) {
    ase_remote_variant v;
    to_remote_variant(*start, om, v);
    rv_r.push_back(v);
    ++start;
  }
}

void
to_remote_variant(const ase_variant *const *start,
  const ase_variant *const *finish, ase_object_serializer *om,
  ase_remote_variant& rv_r)
{
  rv_r.set_array();
  while (start != finish) {
    ase_remote_variant v;
    to_remote_variant(**start, om, v);
    rv_r.push_back(v);
    ++start;
  }
}

ase_variant
from_remote_variant(const ASERemoteVariant *ptr, ase_object_deserializer *ou)
{
  const int t = get_type(ptr);
  switch (t) {
  case ASERemoteVariant_PR_NOTHING:
  case ASERemoteVariant_PR_voidval:
    return ase_variant();
  case ASERemoteVariant_PR_boolval:
    return ase_variant(get_bool(ptr));
  case ASERemoteVariant_PR_intval:
    return ase_variant(get_int(ptr));
  case ASERemoteVariant_PR_realval:
    return ase_variant(get_real(ptr));
  case ASERemoteVariant_PR_strval:
    {
      uint8_t *p = 0;
      int len = 0;
      get_string(ptr, p, len);
      if (p != 0 && len != 0) {
	return ase_string(reinterpret_cast<char *>(p), len).to_variant();
      }
      return ase_string().to_variant();
    }
  case ASERemoteVariant_PR_objval:
    if (ou == 0) {
      return ase_variant();
    } else {
      bool is_remote = false;
      ase_long id = 0;
      get_object(ptr, is_remote, id);
      return ou->deserialize_object(is_remote, id);
    }
  case ASERemoteVariant_PR_arrval:
    {
      ase_variant r = ase_new_array();
      ASERemoteVariant **arr = 0;
      int sz = 0;
      get_array(ptr, arr, sz);
      for (int i = 0; i < sz; ++i) {
	const ase_variant e = from_remote_variant(arr[i], ou);
	r.set_element(i, e);
      }
      return r;
    }
  case ASERemoteVariant_PR_mapval:
    {
      ase_variant r = ase_new_string_map();
      ASERemoteVariant **arr = 0;
      int sz = 0;
      get_array(ptr, arr, sz);
      sz /= 2;
      for (int i = 0; i < sz; ++i) {
	const ase_variant k = from_remote_variant(arr[i * 2], ou);
	const ase_variant v = from_remote_variant(arr[i * 2 + 1], ou);
	r.set_property(k.get_string(), v);
      }
      return r;
    }
  }
  return ase_variant();
}

};

