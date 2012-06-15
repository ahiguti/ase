
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_STRING_BUFFER_HPP
#define GTCPP_STRING_BUFFER_HPP

#include <algorithm>
#include <boost/noncopyable.hpp>
#include <stddef.h>

namespace gtcpp {

class string_buffer : public boost::noncopyable {

 public:

  typedef size_t size_type;
  typedef char value_type;
  typedef const value_type *const_iterator;
  typedef value_type *iterator;

  string_buffer(size_type initial_alloc_size = 0);
  ~string_buffer();
  void assert_valid() const;

  size_type size() const { return buf_end - bufalloc; }
  size_type alloc_size() const { return bufalloc_end - bufalloc; }
  bool empty() const { return buf_end == bufalloc; }

  const_iterator begin() const { return bufalloc; }
  iterator begin() { return bufalloc; }
  const_iterator end() const { return buf_end; }
  iterator end() { return buf_end; }

  void swap(string_buffer& x) {
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

struct string_buffer_range {

  typedef size_t size_type;

  string_buffer_range()
    : begin(0), end(0) { }
  string_buffer_range(size_type b, size_type e)
    : begin(b), end(e) { }

  bool empty() const { return end == begin; }
  size_type size() const { return end - begin; }

  size_type begin;
  size_type end;

};

};

#endif

