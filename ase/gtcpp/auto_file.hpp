
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_AUTO_FILE_HPP
#define GTCPP_AUTO_FILE_HPP

#include <gtcpp/posix_error.hpp>
#include <boost/noncopyable.hpp>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace gtcpp {

class auto_file : private boost::noncopyable {

 public:

  auto_file();
  explicit auto_file(int fd);
  ~auto_file() throw ();
  int get() const { return fd; }
  bool is_valid() const { return fd >= 0; }
  void reset();
  void reset(int fd);
  void reset(auto_file& x);
  friend int close(auto_file& fd, posix_error_callback& ec);

 private:

  int fd;

};

int close(auto_file& fd, posix_error_callback& ec);
int dup(const auto_file& ofd, auto_file& nfd);
int fsync(const auto_file& fd, posix_error_callback& ec);
int open(auto_file& fd, const char *fn, int flags, posix_error_callback& ec);
int open(auto_file& fd, const char *fn, int flags, mode_t mode,
  posix_error_callback& ec);
int creat(auto_file& fd, const char *fn, mode_t mode,
  posix_error_callback& ec);

ssize_t read(const auto_file& fd, void *buf, size_t count,
  posix_error_callback& ec);
ssize_t write(const auto_file& fd, const void *buf, size_t count,
  posix_error_callback& ec);
ssize_t readv(const auto_file& fd, const ::iovec *vec, int count,
  posix_error_callback& ec);
ssize_t writev(const auto_file& fd, const ::iovec *vec, int count,
  posix_error_callback& ec);

int read_all(const auto_file& fd, void *buf, size_t count,
  size_t& count_r, posix_error_callback& ec);
int write_all(const auto_file& fd, const void *buf, size_t count,
  size_t& count_r, posix_error_callback& ec);
int readv_all(const auto_file& fd, ::iovec *vec, int count, size_t& count_r,
  posix_error_callback& ec);
int writev_all(const auto_file& fd, ::iovec *vec, int count, size_t& count_r,
  posix_error_callback& ec);

typedef ssize_t (*io_function_type)(int, void *, size_t);
typedef ssize_t (*iovec_function_type)(int, const ::iovec *, int);

ssize_t io_function_all(io_function_type func, const auto_file& fd,
  void *buf, size_t count, size_t& count_r, posix_error_callback& ec);
int iovec_function_all(iovec_function_type func, const auto_file& fd,
  ::iovec *vec, int count, size_t& count_r, posix_error_callback& ec);

};

#define GTCPP_CHECK_POSIX_ERROR_FD(fd, autofd, ec) \
  do { \
    if (fd < 0) { ec.on_posix_error(errno, __PRETTY_FUNCTION__); } \
    else { autofd.reset(fd); } \
    return fd; \
  } while (false); 

#define GTCPP_CHECK_POSIX_ERROR_NEGATIVE(r, ec) \
  do { \
    if (r < 0) { ec.on_posix_error(errno, __PRETTY_FUNCTION__); } \
    return r; \
  } while (false); 

#endif

