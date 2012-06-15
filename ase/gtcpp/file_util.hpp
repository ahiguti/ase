
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_FILE_UTIL_HPP
#define GTCPP_FILE_UTIL_HPP

#include <gtcpp/auto_file.hpp>
#include <gtcpp/string_buffer.hpp>
#include <gtcpp/string_ref.hpp>
#include <set>
#include <string>

namespace gtcpp {

ssize_t read_append(const auto_file& fd, string_buffer& buf, size_t len,
  posix_error_callback& ec);
int read_file_contents(const char *fn, string_buffer& buf,
  posix_error_callback& ec);
int write_file_contents(const char *fn, const string_ref& str, bool do_fsync,
  posix_error_callback& ec);
int writev_file_contents(const char *fn, const string_ref *strs_begin,
  const string_ref *strs_end, bool do_fsync, posix_error_callback& ec);

int read_string_set(const char *fn, std::set<std::string>& m_r,
  posix_error_callback& ec);

};

#endif

