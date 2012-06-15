
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_AUTO_THREADATTR_HPP
#define GTCPP_AUTO_THREADATTR_HPP

#include <gtcpp/auto_thread.hpp>
#include <pthread.h>

namespace gtcpp {

class auto_threadattr : private boost::noncopyable {

 public:

  auto_threadattr();
  ~auto_threadattr();

  const pthread_attr_t& get() const { return attr; }
  pthread_attr_t& get() { return attr; }

 private:

  pthread_attr_t attr;

};

void threadattr_setstacksize(auto_threadattr& attr, size_t stacksize);

};

#endif

