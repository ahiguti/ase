
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_SCOPED_SIGACTION_HPP
#define GTCPP_SCOPED_SIGACTION_HPP

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

namespace gtcpp {

class scoped_sigaction : private boost::noncopyable {

 public:

  scoped_sigaction(int signum, bool allow);
  ~scoped_sigaction();

 private:

  struct impl_type;
  boost::scoped_ptr<impl_type> impl;

};

};

#endif

