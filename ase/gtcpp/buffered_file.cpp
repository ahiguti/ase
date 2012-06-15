
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/buffered_file.hpp>
#include <gtcpp/file_util.hpp>
#include <gtcpp/string_util.hpp>
#include <errno.h> /* E2BIG */

namespace gtcpp {

buffered_file::buffered_file(size_t rbs, size_t bsl)
  : cursor(0), read_block_size(0), buffer_size_limit(0)
{
  set_read_block_size(rbs);
  set_buffer_size_limit(bsl);
}

void
buffered_file::reset() throw ()
{
  file.reset();
  buffer.reset();
  cursor = 0;
}

void
buffered_file::reset_cursor()
{
  buffer.remove_front(cursor);
  cursor = 0;
}

void
buffered_file::set_read_block_size(size_t rbs)
{
  read_block_size = rbs ? rbs : 1024;
}

void
buffered_file::set_buffer_size_limit(size_t bsl)
{
  buffer_size_limit = bsl;
}

namespace {

template <bool crlf> char *find_lf(char *begin, char *end);

template <> char *
find_lf<true>(char *begin, char *end)
{
  char *p = begin;
  char *q;
  while (p != end) {
    q = memchr_char(p, '\n', end - p);
    if (q == 0) {
      return q;
    }
    if (q != p && q[-1] == '\r') {
      return q;
    }
    p = q + 1;
  }
  return 0;
}

template <> char *
find_lf<false>(char *begin, char *end)
{
  return (end == begin) ? 0 : memchr_char(begin, '\n', end - begin);
}

};

struct buffered_file::impl_type {

  static ssize_t
  read_bytes(buffered_file& bf, size_t len, char *& bytes_r,
    posix_error_callback& ec)
  {
    while (true) {
      const size_t bfsize = bf.buffered_size();
      if (bfsize >= len) {
	/* already buffered */
	bytes_r = bf.buffer.begin() + bf.cursor;
	bf.cursor += len;
	return len;
      }
      size_t read_size = std::max(bf.read_block_size, len - bfsize);
      if (bf.buffer_size_limit > 0) {
	// FIXME: test
	if (bf.buffer.size() >= bf.buffer_size_limit) {
	  bytes_r = 0;
	  ec.on_posix_error(E2BIG, __PRETTY_FUNCTION__);
	  return -1;
	}
	read_size = std::min(read_size,
	  bf.buffer_size_limit - bf.buffer.size());
      }
      const ssize_t r = read_append(bf.get_file(), bf.buffer, read_size, ec);
      if (r <= 0) {
	bytes_r = 0;
	return r;
      }
    }
  }
  
  template <bool crlf> static ssize_t
  read_line(buffered_file& bf, char *& line_r, posix_error_callback& ec)
  {
    size_t offset = bf.cursor;
    while (true) {
      assert(offset <= bf.buffer.size());
      char *const buf_begin = bf.buffer.begin();
      char *const buf_end = bf.buffer.end();
      char *const find_begin = buf_begin + offset;
      char *const lf = find_lf<crlf>(find_begin, buf_end);
      if (lf != 0) {
	/* found */
	const size_t n_cursor = lf + 1 - buf_begin;
	const size_t len = n_cursor - bf.cursor;
	line_r = buf_begin + bf.cursor;
	bf.cursor = n_cursor;
	return len;
      }
      /* need to read more */
      size_t read_size = bf.read_block_size;
      if (bf.buffer_size_limit > 0) {
	// FIXME: test
	if (bf.buffer.size() >= bf.buffer_size_limit) {
	  line_r = 0;
	  ec.on_posix_error(E2BIG, __PRETTY_FUNCTION__);
	  return -1;
	}
	read_size = std::min(read_size,
	  bf.buffer_size_limit - bf.buffer.size());
      }
      offset = buf_end - buf_begin;
      if (crlf && offset > 0) {
	/* we need to decrement because it's possible that
	   buf_begin[offset - 1] == '\r' */
	--offset;
      }
      const ssize_t r = read_append(bf.get_file(), bf.buffer, read_size, ec);
      if (r <= 0) {
	line_r = 0;
	return r;
      }
    }
  };

};

ssize_t
buffered_file::read_bytes(size_t len, char *& bytes_r,
  posix_error_callback& ec)
{
  const ssize_t r = impl_type::read_bytes(*this, len, bytes_r, ec);
  return r;
}

ssize_t
buffered_file::read_line_lf(char *& line_r, posix_error_callback& ec)
{
  const ssize_t r = impl_type::read_line<false>(*this, line_r, ec);
  return r;
}

ssize_t
buffered_file::read_line_crlf(char *& line_r, posix_error_callback& ec)
{
  const ssize_t r = impl_type::read_line<true>(*this, line_r, ec);
  return r;
}

};

