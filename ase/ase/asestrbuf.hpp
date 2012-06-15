
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASESTRBUF_HPP
#define GENTYPE_ASESTRBUF_HPP

#include <algorithm>
#include <ase/ase.hpp>
#include <stddef.h>

class ase_string_buffer : private ase_noncopyable {

 public:

  typedef size_t size_type;
  typedef char value_type;
  typedef const value_type *const_iterator;
  typedef value_type *iterator;

  ase_string_buffer(size_type initial_alloc_size = 0);
  ~ase_string_buffer();
  void assert_valid() const;

  size_type size() const { return buf_end - bufalloc; }
  size_type alloc_size() const { return bufalloc_end - bufalloc; }
  bool empty() const { return buf_end == bufalloc; }
  const_iterator begin() const { return bufalloc; }
  iterator begin() { return bufalloc; }
  const_iterator end() const { return buf_end; }
  iterator end() { return buf_end; }
  ase_string to_string() { return ase_string(begin(), size()); }

  void swap(ase_string_buffer& x) {
    std::swap(bufalloc, x.bufalloc);
    std::swap(buf_end, x.buf_end);
    std::swap(bufalloc_end, x.bufalloc_end);
  }
  void reset(size_type initial_alloc_size = 0);
  void clear();
  value_type *make_space(size_type len);
  void append_done(size_type len_written);
  void append(const value_type *str, size_type len);
  template <size_type n> void append_literal(const value_type (& str)[n]) {
    append(str, n - 1);
  }
  void truncate(size_type len);
  void remove_front(size_type len);

 private:

  value_type *bufalloc, *buf_end, *bufalloc_end;

};

#endif

