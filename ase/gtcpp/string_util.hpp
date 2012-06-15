
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_STRING_UTIL_HPP
#define GTCPP_STRING_UTIL_HPP

#include <string.h>
#include <string>
#include <map>
#include <vector>
#include <gtcpp/string_buffer.hpp>
#include <gtcpp/string_ref.hpp>

namespace gtcpp {

inline int compare_mem(const char *x, size_t xlen, const char *y, size_t ylen)
{
  const size_t clen = std::min(xlen, ylen);
  const int c = memcmp(x, y, clen);
  if (c != 0) {
    return c;
  }
  if (xlen == ylen) {
    return 0;
  } else if (xlen < ylen) {
    return -1;
  } else {
    return 1;
  }
}

inline const char *memchr_char(const char *s, int c, size_t n)
{
  return static_cast<const char *>(::memchr(s, c, n));
}

inline char *memchr_char(char *s, int c, size_t n)
{
  return const_cast<char *>(static_cast<const char *>(::memchr(s, c, n)));
}

template <typename T> inline T *find_char(T *s, T *e, int c)
{
  T *r = memchr_char(s, c, e - s);
  return r ? r : e;
}

inline size_t find_char(const string_ref& s, char c)
{
  const char *b = s.begin();
  const char *e = s.end();
  const char *const p = memchr_char(b, c, e - b);
  return p ? (p - b) : (e - b);
}

template <size_t n> inline size_t string_literal_length(const char (&)[n])
{
  return n - 1;
}

const char *search_mem(const char *needle, size_t needle_len,
  const char *haystack, size_t haystack_len);
  /* returns 0 if not found. haystack must not contain nul. */
const char *search_strcase(const char *needle, size_t needle_len,
  const char *haystack, size_t haystack_len);
  /* returns 0 if not found. haystack must not contain nul. */

template <size_t needle_len> inline const char *search_mem(
  const char (&needle)[needle_len], const char *haystack, size_t haystack_len)
{
  return search_mem(needle, needle_len - 1, haystack, haystack_len);
}
template <size_t needle_len> inline const char *search_strcase(
  const char (&needle)[needle_len], const char *haystack, size_t haystack_len)
{
  return search_strcase(needle, needle_len - 1, haystack, haystack_len);
}

void split(std::vector<string_ref>& out, const char *start, const char *finish,
  char delim);
void join(string_buffer& buf, const string_ref *start,
  const string_ref *finish, const char *delim_start, const char *delim_finish);

size_t to_string_decimal(string_buffer& buf, int value);
size_t to_string_decimal_ll(string_buffer& buf, long long value);
size_t to_string_decimal_ull(string_buffer& buf, unsigned long long value);
size_t to_string_decimal_zero(string_buffer& buf, int value, int prec);
size_t to_string_decimal_zero_ll(string_buffer& buf, long long value,
  int prec);
size_t to_string_decimal_zero_ull(string_buffer& buf, unsigned long long value,
  int prec);
size_t to_string_hexadecimal(string_buffer& buf, unsigned int value);
size_t to_string_hexadecimal_ull(string_buffer& buf, unsigned long long value);
size_t to_string_hexadecimal_zero(string_buffer& buf, int value, int prec);
size_t to_string_hexadecimal_zero_ull(string_buffer& buf,
  unsigned long long value, int prec);
std::string to_stdstring_decimal(int value);
std::string to_stdstring_decimal_ll(long long value);
std::string to_stdstring_decimal_ull(long long value);
bool parse_decimal(const char *start, const char *finish, unsigned int& r);
bool parse_decimal_ull(const char *start, const char *finish,
  unsigned long long& r);
bool parse_hexadecimal(const char *start, const char *finish,
  unsigned int& r);
bool parse_hexadecimal_ull(const char *start, const char *finish,
  unsigned long long& r);

void parse_argv(int argc, char **argv,
  std::map<std::string, std::string>& m_r);

};

#endif

