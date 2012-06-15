
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_RWLOCK_HPP
#define GTCPP_RWLOCK_HPP

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <gtcpp/posix_error.hpp>
#include <pthread.h>
#include <cassert>

namespace gtcpp {

class rwlock {

 public:

  rwlock() {
    int e = pthread_rwlock_init(&rwl, 0);
    if (__builtin_expect(e != 0, false)) {
      throw_posix_error(e, __PRETTY_FUNCTION__);
    }
  }
  virtual ~rwlock() {
    int __attribute__((unused)) e = pthread_rwlock_destroy(&rwl);
    assert(e == 0);
  }

  void rdlock() {
    int e = pthread_rwlock_rdlock(&rwl);
    if (__builtin_expect(e != 0, false)) {
      throw_posix_error(e, __PRETTY_FUNCTION__);
    }
  }
  void wrlock() {
    int e = pthread_rwlock_wrlock(&rwl);
    if (__builtin_expect(e != 0, false)) {
      throw_posix_error(e, __PRETTY_FUNCTION__);
    }
  }
  void unlock() {
    int e = pthread_rwlock_unlock(&rwl);
    if (__builtin_expect(e != 0, false)) {
      throw_posix_error(e, __PRETTY_FUNCTION__);
    }
  }

 private:

  pthread_rwlock_t rwl;

};

template <typename Trwlock>
class scoped_rdlock : private boost::noncopyable {

 public:

  inline explicit scoped_rdlock(volatile Trwlock& oref)
    : objref(const_cast<const Trwlock&>(oref)) { objref.rdlock(); }
  inline ~scoped_rdlock() { objref.unlock(); }
  const Trwlock& operator *() { return objref; }
  const Trwlock *operator ->() { return &objref; }

 private:

  const Trwlock& objref;

};

template <typename Trwlock>
class scoped_wrlock : private boost::noncopyable {

 public:

  inline explicit scoped_wrlock(volatile Trwlock& oref)
    : objref(const_cast<Trwlock&>(oref)) { objref.wrlock(); }
  inline ~scoped_wrlock() { objref.unlock(); }
  Trwlock& operator *() { return objref; }
  Trwlock *operator ->() { return &objref; }

 private:

  Trwlock& objref;

};

};

#endif

