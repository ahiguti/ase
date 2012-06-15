
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_AHO_CORASICK_HPP
#define GTCPP_AHO_CORASICK_HPP

#include <vector>
#include <string>
#include <boost/scoped_ptr.hpp>
#include <cstdlib>
#include <cassert>

#define AC_AUTOMATON_DBG_TRACE(x)

namespace gtcpp {

struct aho_corasick {

 public:

  aho_corasick(const std::vector<std::string>& needlearr,
    int create_direct_depth);
  ~aho_corasick();
  const unsigned int *get_binary() const;
  unsigned int get_binary_size() const;

  class state;
  class matches;

 private:

  struct bin_format;
  struct impl_type;
  boost::scoped_ptr<impl_type> impl;

  /* noncopyable */
  aho_corasick(const aho_corasick&);
  aho_corasick& operator =(const aho_corasick&);

};

class aho_corasick::state {

 public:

  inline state(const aho_corasick& ac);
  inline state(const unsigned int *bin, unsigned int sz);
  inline bool next(char ch);

 private:

  friend class matches;
  unsigned int offset;
  const unsigned int *const binarr;
  unsigned int binarr_size;

};

class aho_corasick::matches {

 public:

  inline matches(const state& st);
  inline operator bool() const;
  inline unsigned int operator *() const;
  inline void operator ++();

 private:

  inline unsigned int find_suffix(unsigned int offset) const;
  inline unsigned int get_match_id(unsigned int j) const;
  inline unsigned int find_match(unsigned int j) const;

  unsigned int offset;
  const unsigned int *const binarr;

};

struct aho_corasick::bin_format {

  /*
   * A compiled pattern (binary) is a concatenation of variable-length
   * blocks, where each block corresponds to a state. A block is an
   * array of 32-bits unsigned ints, and has the following format.
   *
   * (when has_direct_tbl is not set)
   * code[0]: has_direct_tbl(1), has_match(1), unused(6), suffix(24)
   * code[1]: unused(24), tblsize(8)
   * code[2 + i]: next(24), ch(8)    (0 <= i < tblsize) (sorted by ch)
   * code[2 + tblsize]: match(32)
   *
   * (when has_direct_tbl is set)
   * code[0]: has_direct_tbl(1), has_match(1), unused(6), suffix(24)
   * code[1 + i]: next(24), ch(8)    (0 <= i < 256) (ch == i holds)
   * code[257]: match(32)
   *
   */

  enum {
    mask_zero            = 0U,
    has_direct_tbl_mask  = 0x80000000U,
    has_match_mask       = 0x40000000U,
    suffix_mask          = 0x00ffffffU,
    match_none           = 0xffffffffU,
  };

  inline static unsigned int next_entry(unsigned int ch, unsigned int next);
  inline static unsigned int next_entry_get_next(unsigned int code);
  inline static unsigned int next_entry_get_char(unsigned int code);
  inline static bool has_direct_tbl(unsigned int code);
  inline static bool has_match(unsigned int code);
  inline static unsigned int get_suffix(unsigned int code);
  inline static unsigned int get_next_bin(const unsigned int *bin,
    unsigned int bin_size, unsigned int offset, unsigned char c);
};

unsigned int
aho_corasick::bin_format::next_entry(unsigned int ch, unsigned int next)
{
  return (next << 8) | (ch & 0xffU);
}

unsigned int
aho_corasick::bin_format::next_entry_get_next(unsigned int code) {
  return code >> 8;
}

unsigned int
aho_corasick::bin_format::next_entry_get_char(unsigned int code)
{
  return code & 0xffU;
}

bool
aho_corasick::bin_format::has_direct_tbl(unsigned int code)
{
  return (code & has_direct_tbl_mask) != 0;
}

bool
aho_corasick::bin_format::has_match(unsigned int code)
{
  return (code & has_match_mask) != 0;
}

unsigned int
aho_corasick::bin_format::get_suffix(unsigned int code)
{
  return code & suffix_mask;
}

unsigned int
aho_corasick::bin_format::get_next_bin(const unsigned int *bin,
  unsigned int __attribute__((unused)) bin_size, unsigned int offset,
  unsigned char c)
{
  AC_AUTOMATON_DBG_TRACE(printf("c=%u\n", (unsigned int)c));
  while (true) {
    AC_AUTOMATON_DBG_TRACE(printf("offset=%08x\n", offset));
    assert(offset < bin_size);
    const unsigned int *const code = bin + offset;
    const unsigned int bin0 = code[0];
    if (has_direct_tbl(bin0)) {
      /* direct table */
      AC_AUTOMATON_DBG_TRACE(printf("l:r=%08x\n",
	next_entry_get_next(code[c + 1])));
      return next_entry_get_next(code[c + 1]);
    }
    /* binary search table */
    const unsigned int *const arr = code + 2;
    /* first and last must be signed because they can become negative */
    int first = 0;
    int last = next_entry_get_char(code[1]);
    --last;
    while (first <= last) {
      int mid = first + ((last - first) / 2);
      const unsigned char midc = next_entry_get_char(arr[mid]);
      if (midc < c) {
	first = mid + 1; /* can be lager than last */
      } else if (midc > c) {
	last = mid - 1; /* can be negative */
      } else {
	AC_AUTOMATON_DBG_TRACE(printf("b:r=%08x(pos=%d)\n",
	  next_entry_get_next(arr[mid]), mid));
	return next_entry_get_next(arr[mid]);
      }
    }
    if (offset == 0) {
      return offset;
    }
    offset = get_suffix(bin0);
    assert(offset < bin_size);
  }
}

aho_corasick::state::state(const aho_corasick& ac)
  : offset(0), binarr(ac.get_binary()), binarr_size(ac.get_binary_size())
{
}

aho_corasick::state::state(const unsigned int *bin, unsigned int sz)
  : offset(0), binarr(bin), binarr_size(sz)
{
}

bool
aho_corasick::state::next(char ch)
{
  const unsigned char uc = static_cast<unsigned char>(ch);
  offset = bin_format::get_next_bin(binarr, binarr_size, offset, uc);
  return bin_format::has_match(binarr[offset]);
}

aho_corasick::matches::matches(const state& st)
  : offset(0), binarr(st.binarr)
{
  offset = find_match(st.offset);
}

aho_corasick::matches::operator bool() const
{
  return (offset != 0);
}

unsigned int
aho_corasick::matches::operator *() const
{
  return get_match_id(offset);
}

void
aho_corasick::matches::operator ++()
{
  offset = find_match(find_suffix(offset));
}

unsigned int
aho_corasick::matches::find_suffix(unsigned int offset) const
{
  return bin_format::get_suffix(binarr[offset]);
}

unsigned int
aho_corasick::matches::get_match_id(unsigned int j) const
{
  if (bin_format::has_direct_tbl(binarr[j])) {
    return binarr[j + 256 + 1];
  } else {
    const unsigned int tblsz = binarr[j + 1];
    return binarr[j + tblsz + 2];
  }
}

unsigned int
aho_corasick::matches::find_match(unsigned int j) const
{
  while (true) {
    if (j == 0 || !bin_format::has_match(binarr[j])) {
      /* no more */
      return 0;
    }
    unsigned int match_id = get_match_id(j);
    if (match_id != bin_format::match_none) {
      /* found */
      return j;
    }
    j = find_suffix(j);
  }
}

};

#endif

