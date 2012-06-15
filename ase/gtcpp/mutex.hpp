
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_MUTEX_HPP
#define GTCPP_MUTEX_HPP

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <gtcpp/posix_error.hpp>
#include <pthread.h>
#include <cassert>

namespace gtcpp {

class mutex {

 public:

  mutex() {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    #ifdef NDEBUG
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
    #else 
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    #endif
    int e = pthread_mutex_init(&mtx, &attr);
    pthread_mutexattr_destroy(&attr);
    if (__builtin_expect(e != 0, false)) {
      throw_posix_error(e, __PRETTY_FUNCTION__);
    }
  }
  virtual ~mutex() {
    int __attribute__((unused)) e = pthread_mutex_destroy(&mtx);
    assert(e == 0);
  }

  void lock() const {
    int e = pthread_mutex_lock(&mtx);
    if (__builtin_expect(e != 0, false)) {
      throw_posix_error(e, __PRETTY_FUNCTION__);
    }
  }
  void unlock() const {
    int e = pthread_mutex_unlock(&mtx);
    if (__builtin_expect(e != 0, false)) {
      throw_posix_error(e, __PRETTY_FUNCTION__);
    }
  }

 private:

  template <typename Tobj> friend class scoped_lock;
  friend class condition;

  mutable pthread_mutex_t mtx;

};

class condition : public mutex {

 public:

  condition() {
    int e = pthread_cond_init(&cond, 0);
    if (__builtin_expect(e != 0, false)) {
      throw_posix_error(e, __PRETTY_FUNCTION__);
    }
  }
  virtual ~condition() {
    int __attribute__((unused)) e = pthread_cond_destroy(&cond);
    assert(e == 0);
  }

  int timedwait(const timespec *abstime) const {
    int e = pthread_cond_timedwait(&cond, &mtx, abstime);
    return e;
  }
  void wait() const {
    int e = pthread_cond_wait(&cond, &mtx);
    if (__builtin_expect(e != 0, false)) {
      throw_posix_error(e, __PRETTY_FUNCTION__);
    }
  }
  void notify_one() const {
    int e = pthread_cond_signal(&cond);
    if (__builtin_expect(e != 0, false)) {
      throw_posix_error(e, __PRETTY_FUNCTION__);
    }
  }
  void notify_all() const {
    int e = pthread_cond_broadcast(&cond);
    if (__builtin_expect(e != 0, false)) {
      throw_posix_error(e, __PRETTY_FUNCTION__);
    }
  }

 private:

  template <typename Tobj> friend class scoped_lock;

  mutable pthread_cond_t cond;

};

template <typename Tmutex>
class scoped_lock : private boost::noncopyable {

 public:

  inline explicit scoped_lock(volatile Tmutex& oref)
    : objref(const_cast<Tmutex&>(oref)) { objref.lock(); }
  inline ~scoped_lock() { objref.unlock(); }
  Tmutex& operator *() { return objref; }
  Tmutex *operator ->() { return &objref; }
  void wait() { objref.wait(); }
  int timedwait(const timespec *abstime) { return objref.timedwait(abstime); }
  void notify_one() { objref.notify_one(); }
  void notify_all() { objref.notify_all(); }

 private:

  Tmutex& objref;

};

};

#endif

