
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <errno.h>
#include <gtcpp/string_util.hpp>

namespace gtcpp {

const char *search_mem(const char *needle, size_t needle_len,
  const char *haystack, size_t haystack_len)
{
  for (; needle_len <= haystack_len; ++haystack, --haystack_len) {
    if (std::memcmp(haystack, needle, needle_len) == 0) {
      return haystack;
    }
  }
  return 0;
}

const char *search_strcase(const char *needle, size_t needle_len,
  const char *haystack, size_t haystack_len)
{
  for (; needle_len <= haystack_len; ++haystack, --haystack_len) {
    if (strncasecmp(haystack, needle, needle_len) == 0) {
      return haystack;
    }
  }
  return 0;
}

void
split(std::vector<string_ref>& out, const char *start, const char *finish,
  char delim)
{
  while (start < finish) {
    const char *const p = find_char(start, finish, delim);
    out.push_back(string_ref(start, p));
    if (p == finish) {
      break;
    }
    start = p + 1;
  }
}

void
join(string_buffer& buf, const string_ref *start, const string_ref *finish,
  const char *delim_start, const char *delim_finish)
{
  for (; start != finish; ++start) {
    buf.append(start->begin(), start->size());
    if (start + 1 != finish) {
      buf.append(delim_start, delim_finish - delim_start);
    }
  }
}

size_t
to_string_decimal(string_buffer& buf, int value)
{
  const size_t len = 16;
  char *const p = buf.make_space(len);
  const int r = snprintf(p, static_cast<int>(len), "%d", value);
  buf.append_done(r);
  return static_cast<size_t>(r);
}

size_t
to_string_decimal_ll(string_buffer& buf, long long value)
{
  const size_t len = 16;
  char *const p = buf.make_space(len);
  const int r = snprintf(p, static_cast<int>(len), "%lld", value);
  buf.append_done(r);
  return static_cast<size_t>(r);
}

size_t
to_string_decimal_ull(string_buffer& buf, unsigned long long value)
{
  const size_t len = 16;
  char *const p = buf.make_space(len);
  const int r = snprintf(p, static_cast<int>(len), "%llu", value);
  buf.append_done(r);
  return static_cast<size_t>(r);
}

size_t
to_string_decimal_zero(string_buffer& buf, int value, int precision)
{
  const size_t len = 16;
  char *const p = buf.make_space(len);
  const int r = snprintf(p, static_cast<int>(len), "%0*d", precision, value);
  buf.append_done(r);
  return static_cast<size_t>(r);
}

size_t
to_string_decimal_zero_ll(string_buffer& buf, long long value, int precision)
{
  const size_t len = 16;
  char *const p = buf.make_space(len);
  const int r = snprintf(p, static_cast<int>(len), "%0*lld", precision,
    value);
  buf.append_done(r);
  return static_cast<size_t>(r);
}

size_t
to_string_decimal_zero_ull(string_buffer& buf, unsigned long long value,
  int precision)
{
  const size_t len = 16;
  char *const p = buf.make_space(len);
  const int r = snprintf(p, static_cast<int>(len), "%0*llu", precision,
    value);
  buf.append_done(r);
  return static_cast<size_t>(r);
}

size_t
to_string_hexadecimal(string_buffer& buf, unsigned int value)
{
  const size_t len = 16;
  char *const p = buf.make_space(len);
  const int r = snprintf(p, static_cast<int>(len), "%x", value);
  buf.append_done(r);
  return static_cast<size_t>(r);
}

size_t
to_string_hexadecimal_ull(string_buffer& buf, unsigned long long value)
{
  const size_t len = 16;
  char *const p = buf.make_space(len);
  const int r = snprintf(p, static_cast<int>(len), "%llx", value);
  buf.append_done(r);
  return static_cast<size_t>(r);
}

size_t
to_string_hexadecimal_zero(string_buffer& buf, int value, int precision)
{
  const size_t len = 16;
  char *const p = buf.make_space(len);
  const int r = snprintf(p, static_cast<int>(len), "%0*x", precision, value);
  buf.append_done(r);
  return static_cast<size_t>(r);
}

size_t
to_string_hexadecimal_zero_ull(string_buffer& buf, unsigned long long value,
  int precision)
{
  const size_t len = 16;
  char *const p = buf.make_space(len);
  const int r = snprintf(p, static_cast<int>(len), "%0*llx", precision, value);
  buf.append_done(r);
  return static_cast<size_t>(r);
}

std::string
to_stdstring_decimal(int value)
{
  string_buffer buf;
  to_string_decimal(buf, value);
  return std::string(buf.begin(), buf.size());
}

std::string
to_stdstring_decimal_ll(long long value)
{
  string_buffer buf;
  to_string_decimal_ll(buf, value);
  return std::string(buf.begin(), buf.size());
}

std::string
to_stdstring_decimal_ull(long long value)
{
  string_buffer buf;
  to_string_decimal_ull(buf, value);
  return std::string(buf.begin(), buf.size());
}

template <typename T> bool
parse_decimal_tmpl(const char *start, const char *finish, T& v)
{
  v = 0;
  while (start < finish) {
    v *= 10;
    const char c = start[0];
    if (c >= '0' && c <= '9') {
      v += (c - '0');
    } else {
      v = 0;
      return false;
    }
    ++start;
  }
  return true;
}

template <typename T> bool
parse_hexadecimal_tmpl(const char *start, const char *finish, T& v)
{
  v = 0;
  while (start < finish) {
    v *= 16;
    const char c = start[0];
    if (c >= '0' && c <= '9') {
      v += (c - '0');
    } else if (c >= 'a' && c <= 'f') {
      v += (c - 'a' + 10);
    } else if (c >= 'A' && c <= 'F') {
      v += (c - 'A' + 10);
    } else {
      v = 0;
      return false;
    }
    ++start;
  }
  return true;
}

bool
parse_decimal(const char *start, const char *finish, unsigned int& r)
{
  return parse_decimal_tmpl(start, finish, r);
}

bool
parse_decimal_ull(const char *start, const char *finish, unsigned long long& r)
{
  return parse_decimal_tmpl(start, finish, r);
}

bool
parse_hexadecimal(const char *start, const char *finish, unsigned int& r)
{
  return parse_hexadecimal_tmpl(start, finish, r);
}

bool
parse_hexadecimal_ull(const char *start, const char *finish,
  unsigned long long& r)
{
  return parse_hexadecimal_tmpl(start, finish, r);
}

void
parse_argv(int argc, char **argv, std::map<std::string, std::string>& m_r)
{
  for (int i = 0; i < argc; ++i) {
    const char *const s = argv[i];
    size_t len = std::strlen(s);
    const char *p = memchr_char(s, '=', len);
    if (!p) {
      m_r[std::string(s)] = "";
    } else {
      const std::string k(s, p);
      const std::string v(p + 1, len - (p + 1 - s));
      m_r[k] = v;
    }
  }
}

};

