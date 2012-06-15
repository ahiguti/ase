
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <errno.h>
#include <gtcpp/file_util.hpp>
#include <gtcpp/string_util.hpp>
#include <vector>

namespace gtcpp {

ssize_t
read_append(const auto_file& fd, string_buffer& buf, size_t len,
  posix_error_callback& ec)
{
  char *const p = buf.make_space(len);
  const ssize_t r = read(fd, p, len, ec);
  if (r <= 0) {
    return r;
  }
  buf.append_done(r);
  return r;
}

int
read_file_contents(const char *fn, string_buffer& buf,
  posix_error_callback& ec)
{
  auto_file fd;
  int r = 0;
  if ((r = open(fd, fn, O_RDONLY, ec)) < 0) {
    return r;
  }
  while (true) {
    if ((r = read_append(fd, buf, 4096, ec)) <= 0) {
      return r;
    }
  }
  return 0;
}

int
writev_file_contents(const char *fn, const string_ref *strs_begin,
  const string_ref *strs_end, bool do_fsync, posix_error_callback& ec)
{
  auto_file fd;
  int r = 0;
  if ((r = open(fd, fn, O_CREAT | O_WRONLY | O_TRUNC, 0644, ec)) < 0) {
    return r;
  }
  for (const string_ref *strsp = strs_begin; strsp != strs_end; ++strsp) {
    const string_ref& str = *strsp;
    const size_t len = str.size();
    size_t wlen = 0;
    if ((r = write_all(fd, str.begin(), len, wlen, ec)) < 0) {
      return r;
    }
    if (wlen < len) {
      /* unexpected eof */
      ec.on_posix_error(EPIPE, __PRETTY_FUNCTION__);
      return -1;
    }
  }
  if (do_fsync && (r = fsync(fd, ec)) < 0) {
    return r;
  }
  if ((r = close(fd, ec)) < 0) {
    return r;
  }
  return 0;
}

int
write_file_contents(const char *fn, const string_ref& str,
  bool do_fsync, posix_error_callback& ec)
{
  return writev_file_contents(fn, &str, &str + 1, do_fsync, ec);
}

int
read_string_set(const char *fn, std::set<std::string>& m_r,
  posix_error_callback& ec)
{
  string_buffer buf;
  int r = read_file_contents(fn, buf, ec);
  if (r < 0) {
    return r;
  }
  std::vector<string_ref> lines;
  split(lines, buf.begin(), buf.end(), '\n');
  for (size_t i = 0; i < lines.size(); ++i) {
    const string_ref& line = lines[i];
    if (line.empty()) {
      continue;
    }
    m_r.insert(std::string(line.begin(), line.end()));
  }
  return r;
}

};

