
#include <cstring>
#include <assert.h>
#include <ase/ase.hpp>
#include <ase/asearray.hpp>
#include <ase/asestrmap.hpp>
#include <ase/aseexcept.hpp>
#include <ase/aseasn1.hpp>
#include <ase/aseserialize.hpp>
#include <ase/asestrbuf.hpp>

/*
  短い形式(int, real, obj, str, array, assoc): 型タグ+長さで1バイト、データ
    (上位2bitが11以外)
    (長さ31バイトまで)
    000xxxxx int
    001xxxxx real
    010xxxxx obj
    011xxxxx str
    100xxxxx array
    101xxxxx assoc
  長い形式(str, array, assoc): 型タグ+長さの長さ、長さ、データ
    (上位2bitが11、上位4bitが1111以外)
    (長さ2^64-1バイトまで)
    11000yyy int
    11001yyy real
    11010yyy obj
    11011yyy str
    11100yyy array
    11101yyy assoc
  即値(void, bool, etc.)
    (上位4bitが1111)
    11110000 void
    11110010 false
    11110011 true
    11110100 array_begin
    11110101 array_end
    11110110 assoc_begin
    11110111 assoc_end
    (それ以外の値は予約)

  arrayとassocは二つのシリアライズ方法がある。
    1. 長さがわかっている場合: arrayタグを使う
    2. 長さがわかっていない場合: array_beginとarray_endを使う

*/

void ase_serialize_compact_internal(const ase_variant& v,
  ase_object_serializer *om, ase_string_buffer& buf);
ase_variant ase_deserialize_compact_internal(ase_object_deserializer *ou,
  const unsigned char *& rp, const unsigned char *rp_end);

void
ase_serialize_compact_veryshort_form(unsigned char typetag,
  ase_string_buffer& buf)
{
  buf.append(reinterpret_cast<const char *>(&typetag), 1);
}

void
ase_serialize_compact_void(ase_string_buffer& buf)
{
  ase_serialize_compact_veryshort_form(0xf0, buf);
}

void
ase_serialize_compact_bool(bool v, ase_string_buffer& buf)
{
  ase_serialize_compact_veryshort_form(v ? 0xf3 : 0xf2, buf);
}

void
ase_serialize_compact_short_form(unsigned char typetag,
  const unsigned char *rp, size_t rp_len, ase_string_buffer& buf)
{
  unsigned char *const wp = reinterpret_cast<unsigned char *>(
    buf.make_space(rp_len + 1));
  wp[0] = typetag | (rp_len & 0x1f);
  for (size_t i = 0; i < rp_len; ++i) {
    wp[i + 1] = rp[i];
  }
  buf.append_done(rp_len + 1);
}

size_t
byte_length_unsigned(size_t v)
{
  size_t len = 0;
  while (v != 0) {
    ++len;
    v >>= 8;
  }
  return len;
}

size_t
byte_length_signed_little_endian(const unsigned char *buf, size_t blen)
{
  size_t p = 0;
  for (p = 0; p < blen - 1; ++p) {
    const size_t q = blen - p - 1;
    if (buf[q] == 0xff && (buf[q - 1] & 0x80) == 0x80) {
      /* ok to drop(negative) */
    } else if (buf[q] == 0x00 && (buf[q - 1] & 0x80) == 0x00) {
      /* ok to drop(positive) */
    } else {
      break;
    }
  }
  return blen - p;
}

void
ase_serialize_compact_long_form(unsigned char typetag, const unsigned char *rp,
  size_t rp_len, ase_string_buffer& buf)
{
  const size_t length_len = byte_length_unsigned(rp_len);
  unsigned char *const wp = reinterpret_cast<unsigned char *>(
    buf.make_space(rp_len + length_len + 1));
  wp[0] = 0xc0 | (typetag >> 2) | (length_len & 0x07);
  const unsigned char *const lenbuf =
    reinterpret_cast<const unsigned char *>(&rp_len);
    /* TODO: ENDIANNESS */
  std::memcpy(wp + 1, lenbuf, length_len);
  for (size_t i = 0; i < rp_len; ++i) {
    wp[i + length_len + 1] = rp[i];
  }
  buf.append_done(rp_len + length_len + 1);
}

template <typename T> void
ase_serialize_compact_integral(T v, ase_string_buffer& buf)
{
  const unsigned char *const vp =
    reinterpret_cast<const unsigned char *>(&v); /* TODO: ENDIANNESS */
  const size_t vlen = byte_length_signed_little_endian(vp, sizeof(v));
  ase_serialize_compact_short_form(0x00, vp, vlen, buf);
}

void
ase_serialize_compact_double(double v, ase_string_buffer& buf)
{
  const unsigned char *const vp =
    reinterpret_cast<const unsigned char *>(&v); /* TODO: ENDIANNESS */
  const size_t vlen = sizeof(v);
  return ase_serialize_compact_short_form(0x20, vp, vlen, buf);
}

void
ase_serialize_compact_string(const ase_string& s, ase_string_buffer& buf)
{
  const unsigned char *const vp =
    reinterpret_cast<const unsigned char *>(s.data());
  const size_t vlen = s.size();
  if (vlen <= 31) {
    return ase_serialize_compact_short_form(0x60, vp, vlen, buf);
  } else {
    return ase_serialize_compact_long_form(0x60, vp, vlen, buf);
  }
}

void
ase_serialize_compact_array(const ase_variant& v, ase_int len,
  ase_object_serializer *om, ase_string_buffer& buf)
{
  ase_serialize_compact_veryshort_form(0xf4, buf);
  for (ase_int i = 0; i < len; ++i) {
    const ase_variant e = v.get(i);
    ase_serialize_compact_internal(e, om, buf);
  }
  ase_serialize_compact_veryshort_form(0xf5, buf);
}

void
ase_serialize_compact_assoc(const ase_variant& v,
  ase_object_serializer *om, ase_string_buffer& buf)
{
  ase_serialize_compact_veryshort_form(0xf6, buf);
  const ase_variant en = v.get_enumerator();
  while (true) {
    bool has_next = false;
    const ase_string ek = en.enum_next(has_next);
    if (!has_next) {
      break;
    }
    const ase_variant ev = v.get_property(ek);
    ase_serialize_compact_string(ek, buf);
    ase_serialize_compact_internal(ev, om, buf);
  }
  ase_serialize_compact_veryshort_form(0xf7, buf);
}

void
ase_serialize_compact_object_ref(bool is_remote, const ase_long id,
  ase_string_buffer& buf)
{
  unsigned char val[sizeof(ase_long) + 1];
  val[0] = is_remote ? 0xffU : 0;
  std::memcpy(val + 1, &id, sizeof(ase_long)); /* TODO: ENDIANNESS */
  const size_t vlen_id = byte_length_signed_little_endian(val + 1,
    sizeof(ase_long));
  ase_serialize_compact_short_form(0x40, val, vlen_id + 1, buf);
}

void
ase_serialize_compact_object(const ase_variant& v, ase_object_serializer *om,
  ase_string_buffer& buf)
{
  bool is_remote = false;
  ase_long id = 0;
  if (om != 0 && om->serialize_object(v, is_remote, id)) {
    ase_serialize_compact_object_ref(is_remote, id, buf);
  } else {
    const ase_int len = v.get_length();
    if (len >= 0) {
      ase_serialize_compact_array(v, len, om, buf);
    } else {
      ase_serialize_compact_assoc(v, om, buf);
    }
  }
}

void
ase_serialize_compact_internal(const ase_variant& v, ase_object_serializer *om,
  ase_string_buffer& buf)
{
  const ase_vtype t = v.get_type();
  switch (t) {
  case ase_vtype_void:
    return ase_serialize_compact_void(buf);
  case ase_vtype_bool:
    return ase_serialize_compact_bool(v.get_boolean(), buf);
  case ase_vtype_int:
    return ase_serialize_compact_integral(v.get_int(), buf);
  case ase_vtype_long:
    return ase_serialize_compact_integral(v.get_long(), buf);
  case ase_vtype_double:
    return ase_serialize_compact_double(v.get_double(), buf);
  case ase_vtype_string:
    return ase_serialize_compact_string(v.get_string(), buf);
  case ase_vtype_object:
    return ase_serialize_compact_object(v, om, buf);
  }
}

ase_string
ase_serialize_compact_array_with_length(const ase_string_buffer& buf_array)
{
  const unsigned char typetag = 0x80; /* array type */
  const size_t length = buf_array.size();
  if (length <= 31) {
    /* short format, array type */
    unsigned char buf_hdr = typetag | (length & 0x1f);
    return ase_string(reinterpret_cast<const char *>(&buf_hdr), 1,
      buf_array.begin(), buf_array.size());
  } else {
    const size_t length_len = byte_length_unsigned(length);
    assert(length_len <= sizeof(size_t));
    unsigned char buf_hdr[length_len + 1];
    /* long format, array type */
    buf_hdr[0] = 0xc0 | (typetag >> 2) | (length_len & 0x07);
    const unsigned char *const lenbuf =
      reinterpret_cast<const unsigned char *>(&length);
      /* TODO: ENDIANNESS */
    std::memcpy(buf_hdr + 1, lenbuf, length_len);
    return ase_string(reinterpret_cast<const char *>(buf_hdr), length_len + 1,
      buf_array.begin(), buf_array.size());
  }
}

ase_string
ase_serialize_compact(const ase_variant *start, const ase_variant *finish,
  ase_object_serializer *om)
{
  ase_string_buffer buf_array;
  for (const ase_variant *p = start; p != finish; ++p) {
    ase_serialize_compact_internal(*p, om, buf_array);
  }
  return ase_serialize_compact_array_with_length(buf_array);
}

ase_string
ase_serialize_compact_object_op(int op, bool is_remote, ase_long obj_id)
{
  ase_string_buffer buf_array;
  ase_serialize_compact_integral(op, buf_array);
  ase_serialize_compact_object_ref(is_remote, obj_id, buf_array);
  return ase_serialize_compact_array_with_length(buf_array);
}

template <typename T> void
ase_deserialize_compact_integral(T& v_r, const unsigned char *start,
  const unsigned char *finish)
{
  /* assert(finish - start <= sizeof(T)) */
  const size_t dlen = finish - start;
  std::memcpy(&v_r, start, dlen); /* TODO: ENDIANNESS */
  /* assert(finish - start >= 1) */
  if (dlen != sizeof(v_r) && (finish[-1] & 0x80) == 0x80) {
    /* negative */
    unsigned char *const p = reinterpret_cast<unsigned char *>(&v_r) + dlen;
    std::memset(p, 0xffU, sizeof(v_r) - dlen); /* TODO: ENDIANNESS */
  }
}

ase_variant
ase_deserialize_compact_int(const unsigned char *start,
  const unsigned char *finish)
{
  const size_t dlen = finish - start;
  if (dlen > sizeof(ase_long)) {
    ase_throw_exception("DeserializeIntTooLarge", ase_string());
  }
  if (dlen == 0) {
    return ase_variant(0);
  } else if (dlen <= sizeof(ase_int)) {
    ase_int ival = 0;
    ase_deserialize_compact_integral(ival, start, finish);
    return ase_variant(ival);
  } else {
    ase_long lval = 0;
    ase_deserialize_compact_integral(lval, start, finish);
    return ase_variant(lval);
  }
}

ase_variant
ase_deserialize_compact_double(const unsigned char *start,
  const unsigned char *finish)
{
  const size_t dlen = finish - start;
  double v = 0;
  if (dlen != sizeof(v)) {
    ase_throw_exception("DeserializeRealUnsupportedSize", ase_string());
  }
  std::memcpy(&v, start, dlen); /* TODO: ENDIANNESS */
  return ase_variant(v);
}

ase_variant
ase_deserialize_compact_object(ase_object_deserializer *ou,
  const unsigned char *start, const unsigned char *finish)
{
  const size_t dlen = finish - start;
  if (dlen < 1U) {
    ase_throw_exception("DeserializeObjectRefMalformed", ase_string());
  }
  const bool is_remote = (start[0] != 0);
  ase_long id = 0;
  const size_t dlen_id = dlen - 1;
  if (dlen_id == 0) {
    id = 0;
  } else if (dlen_id > sizeof(ase_long)) {
    ase_throw_exception("DeserializeObjectRefUnsupportedSize", ase_string());
  } else {
    ase_deserialize_compact_integral(id, start + 1, finish);
  }
  if (ou != 0) {
    return ou->deserialize_object(is_remote, id);
  }
  return ase_variant();
}

ase_variant
ase_deserialize_compact_string(const unsigned char *start,
  const unsigned char *finish)
{
  const char *const p = reinterpret_cast<const char *>(start);
  size_t plen = finish - start;
  return ase_string(p, plen).to_variant();
}

unsigned char
ase_deserialize_compact_get_tag(const unsigned char *start,
  const unsigned char *finish)
{
  if (static_cast<size_t>(finish - start) < 1U) {
    ase_throw_exception("DeserializeTooShort", ase_string());
  }
  return start[0];
}

ase_variant
ase_deserialize_compact_array(bool expect_end_tag, ase_object_deserializer *ou,
  const unsigned char *& rp, const unsigned char *rp_end)
{
  ase_variant arrval = ase_new_array();
  ase_int idx = 0;
  while (true) {
    if (!expect_end_tag && rp_end == rp) {
      break;
    }
    const unsigned char tag = ase_deserialize_compact_get_tag(rp, rp_end);
    if (tag == 0xf5) {
      if (!expect_end_tag) {
	ase_throw_exception("DeserializeUnexpectedArrayEndTag", ase_string());
      }
      ++rp;
      break;
    }
    const ase_variant ev = ase_deserialize_compact_internal(ou, rp, rp_end);
    arrval.set(idx++, ev);
  }
  return arrval;
}

ase_variant
ase_deserialize_compact_assoc(bool expect_end_tag, ase_object_deserializer *ou,
  const unsigned char *& rp, const unsigned char *rp_end)
{
  ase_variant asval = ase_new_string_map();
  while (true) {
    if (!expect_end_tag && rp_end == rp) {
      break;
    }
    const unsigned char tag = ase_deserialize_compact_get_tag(rp, rp_end);
    if (tag == 0xf7) {
      if (!expect_end_tag) {
	ase_throw_exception("DeserializeUnexpectedAssocEndTag", ase_string());
      }
      ++rp;
      break;
    }
    const ase_variant kv = ase_deserialize_compact_internal(ou, rp, rp_end);
    if (kv.get_type() != ase_vtype_string) {
      ase_throw_exception("DeserializeWrongAssocKey", ase_string());
    }
    const ase_variant ev = ase_deserialize_compact_internal(ou, rp, rp_end);
    asval.set_property(kv.get_string(), ev);
  }
  return asval;
}

ase_variant
ase_deserialize_compact_internal(ase_object_deserializer *ou,
  const unsigned char *& rp, const unsigned char *rp_end)
{
  const unsigned char tag = ase_deserialize_compact_get_tag(rp, rp_end);
  ++rp;
  size_t data_len = 0;
  unsigned char typetag = 0;
  if ((tag & 0xf0) == 0xf0) {
    /* very short form */
    switch (tag) {
    case 0xf0:
      return ase_variant();
    case 0xf2:
      return ase_variant(false);
    case 0xf3:
      return ase_variant(true);
    case 0xf4:
      return ase_deserialize_compact_array(true, ou, rp, rp_end);
    case 0xf6:
      return ase_deserialize_compact_assoc(true, ou, rp, rp_end);
    }
    ase_throw_exception("DeserializeInvalidTag", ase_string());
  } else if ((tag & 0xc0) != 0xc0) {
    /* short form */
    data_len = tag & 0x1f;
    typetag = tag & 0xe0;
  } else {
    /* long form */
    const size_t datalen_len = tag & 0x07;
    if (datalen_len > sizeof(data_len)) {
      ase_throw_exception("DeserializeTooLargeData", ase_string());
    }
    if (static_cast<size_t>(rp_end - rp) < datalen_len) {
      ase_throw_exception("DeserializeTooShort", ase_string());
    }
    std::memcpy(&data_len, rp, datalen_len); /* TODO: ENDIANNESS */
    rp += datalen_len;
    typetag = (tag << 2) & 0xe0;
  }
  if (static_cast<size_t>(rp_end - rp) < data_len) {
    ase_throw_exception("DeserializeTooShort", ase_string());
  }
  const unsigned char *start = rp;
  const unsigned char *finish = start + data_len;
  rp = finish;
  switch (typetag) {
  case 0x00:
    return ase_deserialize_compact_int(start, finish);
  case 0x20:
    return ase_deserialize_compact_double(start, finish);
  case 0x40:
    return ase_deserialize_compact_object(ou, start, finish);
  case 0x60:
    return ase_deserialize_compact_string(start, finish);
  case 0x80:
    return ase_deserialize_compact_array(false, ou, start, finish);
  case 0xa0:
    return ase_deserialize_compact_assoc(false, ou, start, finish);
  }
  ase_throw_exception("DeserializeMalformed", ase_string());
  return ase_variant();
}

ase_variant
ase_deserialize_compact(const char *str, ase_size_type slen,
  ase_object_deserializer *ou)
{
  const unsigned char *ustr =
    reinterpret_cast<const unsigned char *>(str);
  return ase_deserialize_compact_internal(ou, ustr, ustr + slen);
}

bool
ase_deserialize_compact_get_length_internal(const unsigned char *start,
  const unsigned char *finish, ase_size_type& len_r, bool& overflow_r)
{
  len_r = 0;
  overflow_r = false;
  const size_t len = finish - start;
  if (len <= 0) {
    return false;
  }
  const unsigned char tag = start[0];
  if ((tag & 0xf0) == 0xf0) {
    /* very short form */
    len_r = 1;
  } else if ((tag & 0xc0) != 0xc0) {
    /* short form */
    len_r = (tag & 0x1f) + 1;
  } else {
    const size_t datalen_len = tag & 0x07;
    if (len - 1 < datalen_len) {
      return false;
    }
    if (datalen_len > sizeof(size_t)) {
      overflow_r = true;
      return true;
    }
    size_t data_len = 0;
    std::memcpy(&data_len, start + 1, datalen_len); /* TODO: ENDIANNESS */
    len_r = data_len + datalen_len + 1;
  }
  return true;
}

bool
ase_deserialize_compact_get_length(const char *str, ase_size_type slen,
  ase_size_type& len_r, bool& overflow_r)
{
  const unsigned char *const ustr =
    reinterpret_cast<const unsigned char *>(str);
  return ase_deserialize_compact_get_length_internal(ustr, ustr + slen, len_r,
    overflow_r);
}

namespace {

const ase_serializer_table serializer_table_default = {
  ase_serialize_compact,
  ase_serialize_compact_object_op,
  ase_deserialize_compact,
  ase_deserialize_compact_get_length,
};

};

const ase_serializer_table *
ase_serializer_table_default()
{
  return &serializer_table_default;
}


