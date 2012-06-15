
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_KARP_RABIN_HPP
#define GTCPP_KARP_RABIN_HPP

#include <string>
#include <sstream>
#include <map>
#include <vector>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>

namespace gtcpp {

struct karp_rabin_default_param {

  enum { needle_len = 16U };
  typedef unsigned int hashval_type;
  enum { stream_hash_prime = 101 };
  typedef int needle_id_type;
  struct needle_base_type { };

};

class karp_rabin_detail {

  template <typename Tparam> friend class karp_rabin;

  template <typename T, T x, unsigned int n> struct pow;
  template <typename T, T x> struct pow<T, x, 0> {
    enum { value = 1 };
  };
  template <typename T, T x, unsigned int n> struct pow {
    enum { value = pow<T, x, n - 1>::value * x };
  };

};

template <typename Tparam = karp_rabin_default_param>
class karp_rabin {

 public:

  enum { needle_len = Tparam::needle_len };
  typedef typename Tparam::hashval_type hashval_type;
  enum { stream_hash_prime = Tparam::stream_hash_prime };
  enum { stream_hash_prime_pow_n =
    karp_rabin_detail::pow<hashval_type, stream_hash_prime, needle_len>::value
  };
  typedef typename Tparam::needle_id_type needle_id_type;
  enum { invalid_needle_id = -1 };
  typedef typename Tparam::needle_base_type needle_base_type;
  struct needle_type : public needle_base_type {
    needle_id_type next;
    needle_id_type prev;
    hashval_type hashval;
    char buffer[needle_len];
  };
  typedef std::vector<needle_type> needlearr_type;
  class matches;

  inline karp_rabin(const char *strarr[], size_t strarr_len,
    hashval_type hash_table_size);
    /* strarr[i] (0 <= i < strarr_len) must be a string of length
     * needle_len (need not to be 0-terminated). */
  inline void assert_valid() const;
  inline needle_id_type register_needle(const char *str, bool allow_dup);
    /* str must be a streing of length needle_len (need not to be
     * 0-terminated). */
  inline void unregister_needle(needle_id_type id);
  inline needle_id_type find_needle(hashval_type hv, const char *s) const;
  inline needle_id_type find_needle(const char *s) const;
  inline const needle_type& get_needle(needle_id_type id) const;
  inline needle_base_type& get_needle_base(needle_id_type id);
  inline void get_statistics(std::map<std::string, std::string>& stat_r)
    const;
  template <bool search_dup, typename Tfobj> inline void search(Tfobj& fobj,
    const char *haystack, size_t haystack_len) const;

 private:

  typedef std::vector<needle_id_type> table_type;

  /* noncopyable */
  karp_rabin(const karp_rabin&);
  karp_rabin& operator =(const karp_rabin&);

  inline needle_id_type allocate_needle();
  inline void deallocate_needle(needle_id_type nid);
  static inline hashval_type stream_hash(hashval_type mul, const char *s,
    unsigned int n);
  static inline hashval_type stream_hash_diff(hashval_type mul,
    hashval_type mul_pow_n, hashval_type hashval, char s0, char sn);
  static inline hashval_type align_pow2(hashval_type x);
  static inline std::string to_string(int v);

  const hashval_type hash_table_size_mask;
  table_type table;
  needlearr_type needlearr;
  needle_id_type needle_freelist;

};

template <typename Tparam = karp_rabin_default_param>
class karp_rabin<Tparam>::matches {

 public:

  enum { needle_len = Tparam::needle_len };
  typedef typename Tparam::hashval_type hashval_type;
  typedef typename Tparam::needle_id_type needle_id_type;

  inline matches(const karp_rabin& ref, const char *haystack,
    size_t haystack_len);
  inline operator bool() const;
  inline needle_id_type operator *() const;
  inline void operator ++();
  inline needle_id_type get_match_id() const;
  inline size_t get_offset() const;

 private:

  inline void next_match();
  inline bool advance_offset();

  const karp_rabin& ref;
  const char *const haystack;
  const size_t haystack_len;
  size_t cur_offset;
  hashval_type cur_hashval;
  needle_id_type cur_match_id; /* invalid_needle_id if not matched */

};

template <typename Tparam>
karp_rabin<Tparam>::karp_rabin(const char *strarr[],
  size_t strarr_len, hashval_type hash_table_size)
  : hash_table_size_mask(align_pow2(hash_table_size) - 1),
    table(hash_table_size_mask + 1),
    needle_freelist(invalid_needle_id)
{
  needlearr.reserve(table.size());
  for (typename table_type::size_type i = 0; i < table.size(); ++i) {
    table[i] = invalid_needle_id;
  }
  for (size_t i = 0; i < strarr_len; ++i) {
    register_needle(strarr[i], true);
  }
}

template <typename Tparam> void
karp_rabin<Tparam>::assert_valid() const
{
  #ifndef NDEBUG
  size_t cnt = 0;
  for (typename table_type::size_type i = 0; i < table.size(); ++i) {
    needle_id_type prev = invalid_needle_id;
    for (needle_id_type j = table[i]; j != invalid_needle_id;
      j = needlearr[j].next) {
      assert(needlearr[j].prev == prev);
      prev = j;
      ++cnt;
    }
  }
  size_t nf = 0;
  for (needle_id_type i = needle_freelist; i != invalid_needle_id;
    i = needlearr[i].next) {
    ++nf;
  }
  const size_t nndlarr = needlearr.size();
  assert(cnt + nf == nndlarr);
  #endif
}

template <typename Tparam> typename karp_rabin<Tparam>::needle_id_type
karp_rabin<Tparam>::register_needle(const char *str, bool allow_dup)
{
  const hashval_type hashval = stream_hash(stream_hash_prime, str,
    needle_len);
  if (!allow_dup && find_needle(hashval, str) != invalid_needle_id) {
    return invalid_needle_id; /* already exist */
  }
  const typename table_type::size_type entpos
    = hashval & hash_table_size_mask;
  const needle_id_type entfirst = table[entpos];
  needle_id_type nid = allocate_needle();
  needle_type& ndl = needlearr[nid];
  ndl.next = entfirst;
  ndl.prev = invalid_needle_id;
  if (entfirst != invalid_needle_id) {
    needlearr[entfirst].prev = nid;
  }
  ndl.hashval = hashval;
  std::memcpy(ndl.buffer, str, needle_len);
  table[entpos] = nid;
  return nid;
}

template <typename Tparam> void
karp_rabin<Tparam>::unregister_needle(needle_id_type nid)
{
  needle_type& ndl = needlearr[nid];
  const hashval_type hashval = ndl.hashval;
  const typename table_type::size_type entpos
    = hashval & hash_table_size_mask;
  assert(table[entpos] != invalid_needle_id);
  if (table[entpos] == nid) {
    /* nid is the first entry of the bucket */
    needle_id_type entfirst = ndl.next;
    table[entpos] = entfirst;
    if (entfirst != invalid_needle_id) {
      needlearr[entfirst].prev = invalid_needle_id;
    }
  } else {
    /* nid is not the first entry */
    assert(table[entpos] != invalid_needle_id);
    needle_id_type entprev = needlearr[nid].prev;
    needle_id_type entnext = needlearr[nid].next;
    if (entprev != invalid_needle_id) {
      needlearr[entprev].next = entnext;
    }
    if (entnext != invalid_needle_id) {
      needlearr[entnext].prev = entprev;
    }
  }
  deallocate_needle(nid);
}

template <typename Tparam> typename karp_rabin<Tparam>::needle_id_type
karp_rabin<Tparam>::allocate_needle()
{
  if (needle_freelist != invalid_needle_id) {
    const needle_id_type nid = needle_freelist;
    needle_freelist = needlearr[nid].next;
    return nid;
  }
  const needle_id_type nid = needlearr.size();
  needlearr.push_back(needle_type());
  return nid;
}

template <typename Tparam> void
karp_rabin<Tparam>::deallocate_needle(needle_id_type nid)
{
  needlearr[nid].next = needle_freelist;
  needlearr[nid].prev = invalid_needle_id; /* unused */
  needle_freelist = nid;
}

template <typename Tparam> typename karp_rabin<Tparam>::needle_id_type
karp_rabin<Tparam>::find_needle(hashval_type hv, const char *s) const
{
  const typename table_type::size_type entpos = hv & hash_table_size_mask;
  for (needle_id_type i = table[entpos]; i != invalid_needle_id;
    i = needlearr[i].next) { 
    const needle_type& ndl = needlearr[i];
    if (ndl.hashval == hv && std::memcmp(s, ndl.buffer, needle_len) == 0) {
      return i;
    }
  }
  return invalid_needle_id;
}

template <typename Tparam> typename karp_rabin<Tparam>::needle_id_type
karp_rabin<Tparam>::find_needle(const char *s) const
{
  const hashval_type hv = stream_hash(stream_hash_prime, s, needle_len);
  return find_needle(hv, s);
}

template <typename Tparam> const typename karp_rabin<Tparam>::needle_type&
karp_rabin<Tparam>::get_needle(needle_id_type id) const
{
  return needlearr[id];
}

template <typename Tparam> typename karp_rabin<Tparam>::needle_base_type&
karp_rabin<Tparam>::get_needle_base(needle_id_type id)
{
  return needlearr[id];
}

template <typename Tparam> void
karp_rabin<Tparam>::get_statistics(std::map<std::string, std::string>& stat_r)
  const
{
  stat_r["ht_size"] = to_string(hash_table_size_mask + 1);
  size_t cnt = 0;
  int mlen = 0;
  for (typename table_type::size_type i = 0; i < table.size(); ++i) {
    int len = 0;
    for (needle_id_type j = table[i]; j != invalid_needle_id;
      j = needlearr[j].next) {
      ++len;
      ++cnt;
    }
    mlen = std::max(mlen, len);
  }
  size_t nf = 0;
  for (needle_id_type i = needle_freelist; i != invalid_needle_id;
    i = needlearr[i].next) {
    ++nf;
  }
  stat_r["num_needles"] = to_string(cnt);
  stat_r["num_needlearr"] = to_string(needlearr.size());
  stat_r["num_freelist"] = to_string(nf);
  stat_r["max_buckt"] = to_string(mlen);
}

template <typename Tparam> template <bool search_dup, typename Tfobj> void
karp_rabin<Tparam>::search(Tfobj& fobj, const char *haystack,
  size_t haystack_len) const
{
  if (haystack_len < needle_len) {
    return;
  }
  size_t cur_offset = 0;
  hashval_type hv = stream_hash(stream_hash_prime, haystack, needle_len);
  while (true) {
    const char *const s = haystack + cur_offset;
    const typename table_type::size_type entpos = hv & hash_table_size_mask;
    for (needle_id_type i = table[entpos]; i != invalid_needle_id;
      i = needlearr[i].next) { 
      const needle_type& ndl = needlearr[i];
      if (ndl.hashval == hv && std::memcmp(s, ndl.buffer, needle_len) == 0) {
	fobj(cur_offset, i);
	if (search_dup) {
	  break;
	}
      }
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

template <typename Tparam> typename karp_rabin<Tparam>::hashval_type
karp_rabin<Tparam>::stream_hash(hashval_type mul, const char *s,
  unsigned int n)
{
  hashval_type v = 0;
  for (unsigned int i = 0; i < n; ++i) {
    v *= mul; /* mod hashval_type */
    v += static_cast<unsigned char>(s[i]);
  }
  return v;
}

template <typename Tparam> typename karp_rabin<Tparam>::hashval_type
karp_rabin<Tparam>::stream_hash_diff(hashval_type mul,
  hashval_type mul_pow_n, hashval_type hashval, char s0, char sn)
{
  hashval *= mul;
  hashval -= mul_pow_n * static_cast<unsigned char>(s0);
  hashval += static_cast<unsigned char>(sn);
  return hashval;
}

template <typename Tparam> typename karp_rabin<Tparam>::hashval_type
karp_rabin<Tparam>::align_pow2(hashval_type x)
{
  hashval_type i = 1;
  for (i = 1; i < x; i <<= 1) { }
  return i;
}

template <typename Tparam> std::string
karp_rabin<Tparam>::to_string(int v)
{
  /* NOTE: very slow */
  std::stringstream ss;
  ss << v;
  return ss.str();
}

template <typename Tparam>
karp_rabin<Tparam>::matches::matches(const karp_rabin& ref,
  const char *haystack, size_t haystack_len)
  : ref(ref), haystack(haystack), haystack_len(haystack_len),
    cur_offset(0), cur_hashval(0), cur_match_id(karp_rabin::invalid_needle_id)
{
  assert(haystack_len >= needle_len);
  cur_hashval = stream_hash(ref.stream_hash_prime, haystack, needle_len);
  next_match();
}

template <typename Tparam> void
karp_rabin<Tparam>::matches::next_match()
{
  while (true) {
    assert(cur_offset + needle_len <= haystack_len);
    const needle_id_type nid =
      ref.find_needle(cur_hashval, haystack + cur_offset);
    if (nid != invalid_needle_id) {
      cur_match_id = nid;
      return;
    }
    if (!advance_offset()) {
      return;
    }
  }
}

template <typename Tparam> bool
karp_rabin<Tparam>::matches::advance_offset()
{
  if (cur_offset + needle_len >= haystack_len) {
    cur_match_id = invalid_needle_id;
    return false;
  }
  const char s0 = haystack[cur_offset];
  const char sn = haystack[cur_offset + needle_len];
  ++cur_offset;
  cur_hashval = stream_hash_diff(ref.stream_hash_prime,
    ref.stream_hash_prime_pow_n, cur_hashval, s0, sn);
  return true;
}

template <typename Tparam>
karp_rabin<Tparam>::matches::operator bool() const
{
  return cur_match_id != invalid_needle_id;
}

template <typename Tparam>
typename karp_rabin<Tparam>::matches::needle_id_type
karp_rabin<Tparam>::matches::operator *() const
{
  return cur_match_id;
}

template <typename Tparam> void
karp_rabin<Tparam>::matches::operator ++()
{
  /* note: the matches class only finds the first entry of the same key.
   * this is the same behavier as the search() method with
   * search_dup == false. */
  if (!advance_offset()) {
    return;
  }
  next_match();
}

template <typename Tparam>
typename karp_rabin<Tparam>::matches::needle_id_type
karp_rabin<Tparam>::matches::get_match_id() const
{
  return cur_match_id;
}

template <typename Tparam> size_t
karp_rabin<Tparam>::matches::get_offset() const
{
  return cur_offset;
}

};

#endif

