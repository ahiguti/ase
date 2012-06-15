
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/scoped_sigaction.hpp>
#include <gtcpp/posix_error.hpp>
#include <signal.h>
#include <string.h>
#include <errno.h>

#define DBG_HANDLER(x)

namespace gtcpp {

namespace {

void
empty_signal_handler(int)
{
  DBG_HANDLER(fprintf(stderr, "empty signal handler called\n"));
}

};

struct scoped_sigaction::impl_type {
  impl_type(int n) : signum(n) { }
  int signum;
  struct sigaction oldact;
};

scoped_sigaction::scoped_sigaction(int signum, bool allow)
  : impl(new impl_type(signum))
{
  struct sigaction act;
  ::memset(&act, 0, sizeof(act));
  if (allow) {
    act.sa_handler = empty_signal_handler;
  } else {
    act.sa_handler = SIG_IGN;
  }
  act.sa_flags = 0;
  if (::sigaction(signum, &act, &impl->oldact) < 0) {
    throw_posix_error(errno, __PRETTY_FUNCTION__);
  }
}

scoped_sigaction::~scoped_sigaction()
{
  ::sigaction(impl->signum, &impl->oldact, 0);
}

};

