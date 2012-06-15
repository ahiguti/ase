
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_BUFFERED_FILE_HPP
#define GTCPP_BUFFERED_FILE_HPP

#include <gtcpp/auto_file.hpp>
#include <gtcpp/string_buffer.hpp>
#include <gtcpp/string_ref.hpp>
#include <boost/noncopyable.hpp>

namespace gtcpp {

class buffered_file : private boost::noncopyable {

 public:

  buffered_file(size_t read_block_size = 0, size_t buffer_size_limit = 0);

  const string_buffer& get_buffer() const { return buffer; }
  const auto_file& get_file() const { return file; }
  auto_file& get_file() { return file; }
  size_t get_cursor_offset() const { return cursor; }
  size_t buffered_size() const { return buffer.size() - cursor; }

  void set_read_block_size(size_t read_block_size = 0);
  void set_buffer_size_limit(size_t buffer_size_limit = 0);
  void reset_cursor();
  void reset() throw ();
  ssize_t read_bytes(size_t len, char *& bytes_r, posix_error_callback& ec);
  ssize_t read_line_lf(char *& line_r, posix_error_callback& ec);
    /* if r > 0, line_r[r - 1] == '\n' holds. */
  ssize_t read_line_crlf(char *& line_r, posix_error_callback& ec);
    /* if r > 0, then r >= 2 && line_r[r - 2] == '\r' &&
       line_r[r - 1] == '\n' hold. */

 private:

  struct impl_type;
  auto_file file;
  string_buffer buffer;
  size_t cursor;
  size_t read_block_size;
  size_t buffer_size_limit;

};

};

#endif

