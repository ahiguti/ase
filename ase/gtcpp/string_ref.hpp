
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_STRING_REF_HPP
#define GTCPP_STRING_REF_HPP

#include <string.h>
#include <strings.h>
#include <assert.h>
#include <string>
#include <cstring>
#include <gtcpp/string_buffer.hpp>
#include <boost/static_assert.hpp>

#include <sys/uio.h>

namespace gtcpp {

class string_ref : public ::iovec {

  typedef size_t size_type;

 public:

  string_ref() { reset(); }
  string_ref(const char *s, size_type len) { set(s, len); }
  string_ref(const char *s, const char *f) { set(s, f); }
  template <size_t n> string_ref(const char (& s)[n]) { set(s, n - 1); }
  string_ref(const std::string& s) { set(s); }
  string_ref(const string_buffer& buffer) {
    set(buffer.begin(), buffer.end());
  }
  string_ref(const string_buffer& buffer, const string_buffer_range& range) {
    assert(range.begin <= buffer.size());
    assert(range.end <= buffer.size());
    set(buffer.begin() + range.begin, range.end - range.begin);
  }
  void reset() {
    iov_base = 0;
    iov_len = 0;
  }
  void set(const char *s, size_type len) {
    iov_base = const_cast<char *>(s);
    iov_len = len;
  }
  void set(const char *s, const char *f) {
    set(s, f - s);
  }
  void set(const string_buffer& s) {
    set(s.begin(), s.size());
  }
  void set(const std::string& s) {
    set(s.data(), s.size());
  }
  const char *begin() const { return static_cast<char *>(iov_base); }
  const char *end() const { return begin() + iov_len; }
  size_type size() const { return iov_len; }
  bool empty() const { return iov_len == 0; }

  // FIXME: should be non-member functions
  std::string to_stdstring() const { return std::string(begin(), end()); }
  bool equals(const char *start, const char *finish) const {
    return (size() == static_cast<size_type>(finish - start))
      && (std::memcmp(begin(), start, (finish - start)) == 0);
  }
  bool equals_strcase(const char *start, const char *finish) const {
    return (size() == static_cast<size_type>(finish - start))
      && (strncasecmp(begin(), start, (finish - start)) == 0);
  }
  template <size_type n> bool equals_strcase(const char (&lit)[n]) const {
    return equals_strcase(lit, lit + n - 1);
  }
  template <size_type n> bool operator ==(const char (&lit)[n]) const {
    return (size() == n - 1) && (std::memcmp(begin(), lit, n - 1) == 0);
  }
  template <size_type n> bool prefix_strcase(const char (&lit)[n]) const {
    return (size() >= n - 1) && (strncasecmp(begin(), lit, n - 1) == 0);
  }

};

/* string_ref[] can be used as ::iovec[] */
BOOST_STATIC_ASSERT((sizeof(string_ref) == sizeof(::iovec)));

};

#endif

