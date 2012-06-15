
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_SCOPED_SIGMASK_HPP
#define GTCPP_SCOPED_SIGMASK_HPP

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <signal.h>

namespace gtcpp {

class scoped_sigmask : private boost::noncopyable {

 public:

  scoped_sigmask(int how, int signum);
  scoped_sigmask(int how, const sigset_t *sset);
  ~scoped_sigmask();

 private:

  struct impl_type;
  boost::scoped_ptr<impl_type> impl;

};

};

#endif

