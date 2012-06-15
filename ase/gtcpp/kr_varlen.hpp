
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_KR_VARLEN_HPP
#define GTCPP_KR_VARLEN_HPP

#include <string>
#include <list>
#include <map>
#include <vector>
#include <algorithm>
#include <cstring>

namespace gtcpp {

struct kr_varlen_default_param {

  typedef unsigned int hashval_type;
  enum { stream_hash_prime = 101 };
  typedef std::string needle_type;
  typedef std::list<needle_type> bucket_type;
  static const char *get_pointer(const needle_type& n) { return n.data(); }
  static size_t get_size(const needle_type& n) { return n.size(); }
  static inline bool register_needle(bucket_type& bkt, const needle_type& n);
    /* returns true if registered */
  static inline bool unregister_needle(bucket_type& bkt, const needle_type& n);
    /* returns true if unregistered */
  static inline bucket_type::iterator find(bucket_type& bkt,
    const needle_type& n);
  template <typename Tfobj> static inline bool search(const bucket_type& bkt,
    const char *haystack, size_t haystack_len, size_t offset, Tfobj& fobj);
    /* kr_varlen::search finishes if kr_varlen_default_param::search
     * returns true */

};

template <typename Tparam = kr_varlen_default_param>
class kr_varlen {

 public:

  typedef typename Tparam::hashval_type hashval_type;
  enum { stream_hash_prime = Tparam::stream_hash_prime };
  typedef typename Tparam::needle_type needle_type;
  typedef std::list<needle_type> bucket_type;

  inline kr_varlen(size_t ndl_len, hashval_type hash_table_size);
  inline bool register_needle(const needle_type& n);
  inline bool unregister_needle(const needle_type& n);
  #if 0
  inline bool find_needle(hashval_type hv, const needle_type& n)
    const;
  inline bool find_needle(const needle_type& n) const;
  inline void get_statistics(std::map<std::string, std::string>& stat_r)
    const;
  #endif
  template <typename Tfobj> inline void search(Tfobj& fobj,
    const char *haystack, size_t haystack_len) const;
  inline void resize_buckets(size_t buckets_size);
  inline size_t size() const { return num_entries; }
  inline size_t get_buckets_size() const { return hash_table_size_mask + 1; }

 private:

  /* noncopyable */
  kr_varlen(const kr_varlen&);
  kr_varlen& operator =(const kr_varlen&);

  typedef std::vector<bucket_type> table_type;

  static inline hashval_type stream_hash(hashval_type mul, const char *s,
    unsigned int n);
  static inline hashval_type stream_hash_diff(hashval_type mul,
    hashval_type mul_pow_n, hashval_type hashval, char s0, char sn);
  static inline hashval_type pow_int(hashval_type x, size_t n);
  static inline hashval_type align_pow2(hashval_type x);

  const size_t needle_len;
  const hashval_type stream_hash_prime_pow_n;
  hashval_type hash_table_size_mask;
  table_type table;
  size_t num_entries;

};

/* implementation */

bool
kr_varlen_default_param::register_needle(bucket_type& bkt,
  const needle_type& n)
{
  if (std::find(bkt.begin(), bkt.end(), n) == bkt.end()) {
    bkt.push_back(n);
    return true;
  }
  return false;
}

bool
kr_varlen_default_param::unregister_needle(bucket_type& bkt,
  const needle_type& n)
{
  bucket_type::iterator iter = std::find(bkt.begin(), bkt.end(), n);
  if (iter != bkt.end()) {
    bkt.erase(iter);
    return true;
  }
  return false;
}

kr_varlen_default_param::bucket_type::iterator
kr_varlen_default_param::find(bucket_type& bkt, const needle_type& n)
{
  return std::find(bkt.begin(), bkt.end(), n);
}

template <typename Tfobj> bool
kr_varlen_default_param::search(const bucket_type& bkt,
  const char *haystack, size_t haystack_len, size_t offset, Tfobj& fobj)
{
  const char *const hstr = haystack + offset;
  const size_t hlen = haystack_len - offset;
  bucket_type::const_iterator i;
  for (i = bkt.begin(); i != bkt.end(); ++i) {
    const char *const istr = get_pointer(*i);
    size_t ilen = get_size(*i);
    if (hlen >= ilen && std::memcmp(istr, hstr, ilen) == 0) {
      if (fobj(offset, *i)) {
	return true;
      }
    }
  }
  return false;
}

template <typename Tparam>
kr_varlen<Tparam>::kr_varlen(size_t ndl_len, hashval_type hash_table_size)
  : needle_len(ndl_len),
    stream_hash_prime_pow_n(pow_int(stream_hash_prime, needle_len)),
    hash_table_size_mask(align_pow2(hash_table_size) - 1),
    table(hash_table_size_mask + 1),
    num_entries(0)
{
}

template <typename Tparam> bool
kr_varlen<Tparam>::register_needle(const needle_type& n)
{
  const char *const str = Tparam::get_pointer(n);
  const size_t slen = Tparam::get_size(n);
  if (slen < needle_len) {
    return false;
  }
  const hashval_type hashval = stream_hash(stream_hash_prime, str, needle_len);
  const typename table_type::size_type entpos = hashval & hash_table_size_mask;
  bucket_type& bkt = table[entpos];
  const bool r = Tparam::register_needle(bkt, n);
  if (r) {
    ++num_entries;
  }
  return r;
}

template <typename Tparam> bool
kr_varlen<Tparam>::unregister_needle(const needle_type& n)
{
  const char *const str = Tparam::get_pointer(n);
  const size_t slen = Tparam::get_size(n);
  if (slen < needle_len) {
    return false;
  }
  const hashval_type hashval = stream_hash(stream_hash_prime, str, needle_len);
  const typename table_type::size_type entpos = hashval & hash_table_size_mask;
  bucket_type& bkt = table[entpos];
  const bool r = Tparam::unregister_needle(bkt, n);
  if (r) {
    --num_entries;
  }
  return r;
}

template <typename Tparam> template <typename Tfobj> void
kr_varlen<Tparam>::search(Tfobj& fobj, const char *haystack,
  size_t haystack_len) const
{
  if (haystack_len < needle_len) {
    return;
  }
  size_t cur_offset = 0;
  hashval_type hv = stream_hash(stream_hash_prime, haystack, needle_len);
  while (true) {
    const typename table_type::size_type entpos = hv & hash_table_size_mask;
    const bucket_type& bkt = table[entpos];
    if (Tparam::search(bkt, haystack, haystack_len, cur_offset, fobj)) {
      break;
    }
    if (cur_offset + needle_len >= haystack_len) {
      break;
    }
    const char s0 = haystack[cur_offset];
    const char sn = haystack[cur_offset + needle_len];
    ++cur_offset;
    hv = stream_hash_diff(stream_hash_prime, stream_hash_prime_pow_n,
      hv, s0, sn);
  }
}

template <typename Tparam> void
kr_varlen<Tparam>::resize_buckets(size_t buckets_size)
{
  buckets_size = align_pow2(buckets_size);
  const hashval_type nmask = buckets_size - 1;
  table_type ntbl(buckets_size);
  for (typename table_type::const_iterator i = table.begin();
    i != table.end(); ++i) {
    for (typename bucket_type::const_iterator j = i->begin(); j != i->end();
      ++j) {
      const char *const str = Tparam::get_pointer(*j);
      const hashval_type hashval = stream_hash(stream_hash_prime, str,
	needle_len);
      const typename table_type::size_type entpos = hashval & nmask;
      bucket_type& bkt = ntbl[entpos];
      Tparam::register_needle(bkt, *j);
    }
  }
  using std::swap;
  swap(table, ntbl); /* expects koenig lookup */
  hash_table_size_mask = nmask;
}

template <typename Tparam> typename kr_varlen<Tparam>::hashval_type
kr_varlen<Tparam>::stream_hash(hashval_type mul, const char *s,
  unsigned int n)
{
  hashval_type v = 0;
  for (unsigned int i = 0; i < n; ++i) {
    v *= mul; /* modulo hashval_type */
    v += static_cast<unsigned char>(s[i]);
  }
  return v;
}

template <typename Tparam> typename kr_varlen<Tparam>::hashval_type
kr_varlen<Tparam>::stream_hash_diff(hashval_type mul,
  hashval_type mul_pow_n, hashval_type hashval, char s0, char sn)
{
  hashval *= mul;
  hashval -= mul_pow_n * static_cast<unsigned char>(s0);
  hashval += static_cast<unsigned char>(sn);
  return hashval;
}

template <typename Tparam> typename kr_varlen<Tparam>::hashval_type
kr_varlen<Tparam>::pow_int(hashval_type x, size_t n)
{
  hashval_type r = 1;
  for (size_t i = 0; i < n; ++i) {
    r *= x;
  }
  return r;
}

template <typename Tparam> typename kr_varlen<Tparam>::hashval_type
kr_varlen<Tparam>::align_pow2(hashval_type x)
{
  hashval_type i = 1;
  for (i = 1; i < x; i <<= 1) { }
  return i;
}

};

#endif

