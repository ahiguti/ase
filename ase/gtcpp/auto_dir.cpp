
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/auto_dir.hpp>
#include <gtcpp/auto_file.hpp>
#include <gtcpp/debugcount.hpp>
#include <errno.h>
#include <unistd.h>

namespace gtcpp {

int dir_constr_count = 0;
int dir_destr_count = 0;

auto_dir::auto_dir()
  : ptr(0)
{
}

auto_dir::~auto_dir() throw ()
{
  reset();
}

void
auto_dir::reset()
{
  if (ptr) {
    #ifndef NDEBUG
    atomic_xadd(dir_destr_count, 1);
    #endif
    ::closedir(ptr);
    ptr = 0;
  }
}

void
auto_dir::reset(::DIR *p)
{
  reset();
  if (p) {
    #ifndef NDEBUG
    atomic_xadd(dir_constr_count, 1);
    #endif
    ptr = p;
  }
}

int
opendir(auto_dir& dir, const char *name, posix_error_callback& ec)
{
  ::DIR *p = ::opendir(name);
  if (p == 0) {
    ec.on_posix_error(errno, __PRETTY_FUNCTION__);
    return -1;
  }
  dir.reset(p);
  return 0;
}

int
readdir(const auto_dir& dir, ::dirent *& ent_r, posix_error_callback& ec)
{
  errno = 0; /* necessary for checking error */
  ent_r = ::readdir(dir.get());
  if (ent_r == 0 && errno != 0) {
    ec.on_posix_error(errno, __PRETTY_FUNCTION__);
    return -1;
  }
  return (ent_r != 0) ? 1 : 0;
}

int
mkdir(const char *name, mode_t mode, posix_error_callback& ec)
{
  const int r = ::mkdir(name, mode);
  GTCPP_CHECK_POSIX_ERROR_NEGATIVE(r, ec);
}

int
unlink(const char *name, posix_error_callback& ec)
{
  const int r = ::unlink(name);
  GTCPP_CHECK_POSIX_ERROR_NEGATIVE(r, ec);
}

};

