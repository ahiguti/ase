
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/scoped_sigmask.hpp>
#include <gtcpp/posix_error.hpp>
#include <errno.h>

#define DBG_HANDLER(x)

namespace gtcpp {

struct scoped_sigmask::impl_type {
  sigset_t oldset;
};

scoped_sigmask::scoped_sigmask(int how, int signum)
  : impl(new impl_type())
{
  sigset_t sset;
  if (::sigemptyset(&sset) < 0) {
    throw_posix_error(errno, __PRETTY_FUNCTION__);
  }
  if (::sigaddset(&sset, signum) < 0) {
    throw_posix_error(errno, __PRETTY_FUNCTION__);
  }
  int r = ::pthread_sigmask(how, &sset, &impl->oldset);
  if (r != 0) {
    throw_posix_error(r, __PRETTY_FUNCTION__);
  }
}

scoped_sigmask::scoped_sigmask(int how, const sigset_t *sset)
  : impl(new impl_type())
{
  int r = ::pthread_sigmask(how, sset, &impl->oldset);
  if (r != 0) {
    throw_posix_error(r, __PRETTY_FUNCTION__);
  }
}

scoped_sigmask::~scoped_sigmask()
{
  ::pthread_sigmask(SIG_SETMASK, &impl->oldset, 0);
}

};

