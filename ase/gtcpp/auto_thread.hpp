
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_AUTO_THREAD_HPP
#define GTCPP_AUTO_THREAD_HPP

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

namespace gtcpp {

class auto_threadattr;

class auto_thread : private boost::noncopyable {

 public:

  virtual ~auto_thread();
  void start();
  void start(const auto_threadattr& attr);
  int kill(int signo); /* returns -1 if ESRCH */
  void join();
  bool joined() const;
 
 private:

  template <typename T> friend class threaded;
  auto_thread();
  virtual void run() = 0;

 private:

  struct impl_type;
  const boost::scoped_ptr<impl_type> impl;

};

template <typename T>
class threaded : public auto_thread {

 public:

  threaded() { }
  template <typename Targ> threaded(const Targ& arg) : obj(arg) { }
  ~threaded() {
    /* thr must be joined before obj is destroyed, or obj will be
      corrupted. */
    if (!joined()) {
      auto_thread::join();
    }
  }
  void run() { obj(); }
  T& get() { return obj; }
  const T& get() const { return obj; }

 private:

  T obj;

};

};

#endif

