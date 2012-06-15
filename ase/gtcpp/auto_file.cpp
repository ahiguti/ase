
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

/* nanosleep */
#define _POSIX_C_SOURCE 199309
#include <time.h>

#include <gtcpp/auto_file.hpp>
#include <gtcpp/debugcount.hpp>
#include <errno.h>
#include <unistd.h>

namespace gtcpp {

int file_constr_count = 0;
int file_destr_count = 0;

auto_file::auto_file()
  : fd(-1)
{
  #ifndef NDEBUG
  atomic_xadd(file_constr_count, 1);
  #endif
}

auto_file::auto_file(int fd)
  : fd(fd)
{
  #ifndef NDEBUG
  atomic_xadd(file_constr_count, 1);
  #endif
}

auto_file::~auto_file() throw ()
{
  #ifndef NDEBUG
  atomic_xadd(file_destr_count, 1);
  #endif
  reset();
}

void
auto_file::reset()
{
  if (fd >= 0) {
    do {
      if (::close(fd) == 0) {
	break;
      }
      if (errno == EWOULDBLOCK) {
	/* close() can return EWOULDBLOCK if SO_LINGER is enabled */
	::timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 10 * 1000 * 1000; /* 10 msec */
	nanosleep(&ts, 0);
	continue;
      }
    } while (errno == EINTR); /* ignore other errors */
    fd = -1;
  }
}

void
auto_file::reset(int fd)
{
  reset();
  this->fd = fd;
}

void
auto_file::reset(auto_file& fd)
{
  reset();
  this->fd = fd.fd;
  fd.fd = -1;
}

int
close(auto_file& fd, posix_error_callback& ec)
{
  const int r = ::close(fd.get());
  if (r == 0) {
    fd.fd = -1;
  } else {
    ec.on_posix_error(errno, __PRETTY_FUNCTION__);
  }
  return r;
}

int
dup(const auto_file& ofd, auto_file& nfd, posix_error_callback& ec)
{
  const int fdnum = nfd.get();
  int r = 0;
  if (fdnum < 0) {
    r = ::dup(ofd.get());
  } else {
    r = close(nfd, ec);
    if (r < 0) {
      return r;
    }
    r = ::dup2(ofd.get(), fdnum);
  }
  GTCPP_CHECK_POSIX_ERROR_FD(r, nfd, ec);
}

int
fsync(const auto_file& fd, posix_error_callback& ec)
{
  const int r = ::fsync(fd.get());
  GTCPP_CHECK_POSIX_ERROR_NEGATIVE(r, ec);
}

int
open(auto_file& fd, const char *fn, int flags, posix_error_callback& ec)
{
  const int r = ::open(fn, flags);
  GTCPP_CHECK_POSIX_ERROR_FD(r, fd, ec);
}

int
open(auto_file& fd, const char *fn, int flags, mode_t mode,
  posix_error_callback& ec)
{
  const int r = ::open(fn, flags, mode);
  GTCPP_CHECK_POSIX_ERROR_FD(r, fd, ec);
}

int
creat(auto_file& fd, const char *fn, mode_t mode, posix_error_callback& ec)
{
  const int r = ::creat(fn, mode);
  GTCPP_CHECK_POSIX_ERROR_FD(r, fd, ec);
}


ssize_t
read(const auto_file& fd, void *buf, size_t count, posix_error_callback& ec)
{
  const ssize_t r = ::read(fd.get(), buf, count);
  GTCPP_CHECK_POSIX_ERROR_NEGATIVE(r, ec);
}

ssize_t
write(const auto_file& fd, const void *buf, size_t count,
  posix_error_callback& ec)
{
  const ssize_t r = ::write(fd.get(), buf, count);
  GTCPP_CHECK_POSIX_ERROR_NEGATIVE(r, ec);
}

ssize_t
readv(const auto_file& fd, const ::iovec *vec, int count,
  posix_error_callback& ec)
{
  const ssize_t r = ::readv(fd.get(), vec, count);
  GTCPP_CHECK_POSIX_ERROR_NEGATIVE(r, ec);
}

ssize_t
writev(const auto_file& fd, const ::iovec *vec, int count,
  posix_error_callback& ec)
{
  const ssize_t r = ::writev(fd.get(), vec, count);
  GTCPP_CHECK_POSIX_ERROR_NEGATIVE(r, ec);
}

namespace {

template <typename Func, typename Buf> int
rwop_all(Func func, const auto_file& fd, Buf *buf, size_t count,
  size_t& count_r, posix_error_callback& ec)
{
  count_r = 0;
  while (count > 0) {
    const ssize_t r = func(fd.get(), buf, count);
    if (r == 0) {
      return 0;
    } else if (r < 0) {
      ec.on_posix_error(errno, __PRETTY_FUNCTION__);
      return r;
    }
    count_r += r;
    buf += r;
    count -= r;
  }
  return 1;
}

void
add_iov_base(::iovec& vec, size_t v)
{
  vec.iov_base = static_cast<char *>(vec.iov_base) + v;
}

template <typename Func> int
rwvecop_all(Func func, const auto_file& fd, ::iovec *vec, int count,
  size_t& count_r, posix_error_callback& ec)
{
  count_r = 0;
  while (count > 0) {
    const ssize_t r = func(fd.get(), vec, count);
    if (r == 0) {
      return 0;
    } else if (r < 0) {
      ec.on_posix_error(errno, __PRETTY_FUNCTION__);
      return r;
    }
    size_t ur = r;
    while (ur >= vec[0].iov_len) {
      ur -= vec[0].iov_len;
      add_iov_base(vec[0], vec[0].iov_len);
      vec[0].iov_len = 0;
      ++vec;
      --count;
      if (count <= 0) {
	return 1;
      }
    }
    add_iov_base(vec[0], ur);
    vec[0].iov_len -= ur;
  }
  return 1;
}

};

int
read_all(const auto_file& fd, void *buf, size_t count, size_t& count_r,
  posix_error_callback& ec)
{
  return rwop_all(::read, fd, static_cast<char *>(buf), count, count_r, ec);
}

int
write_all(const auto_file& fd, const void *buf, size_t count, size_t& count_r,
  posix_error_callback& ec)
{
  return rwop_all(::write, fd, static_cast<const char *>(buf), count,
    count_r, ec);
}

int
readv_all(const auto_file& fd, ::iovec *vec, int count, size_t& count_r,
  posix_error_callback& ec)
{
  return rwvecop_all(::readv, fd, vec, count, count_r, ec);
}

int
writev_all(const auto_file& fd, ::iovec *vec, int count, size_t& count_r,
  posix_error_callback& ec)
{
  return rwvecop_all(::writev, fd, vec, count, count_r, ec);
}

ssize_t
io_function_all(io_function_type func, const auto_file& fd,
  void *buf, size_t count, size_t& count_r, posix_error_callback& ec)
{
  return rwop_all(func, fd, static_cast<char *>(buf), count, count_r, ec);
}

int
iovec_function_all(iovec_function_type func, const auto_file& fd,
  ::iovec *vec, int count, size_t& count_r, posix_error_callback& ec)
{
  return rwvecop_all(func, fd, vec, count, count_r, ec);
}

};

