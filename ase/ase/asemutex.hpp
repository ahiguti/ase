
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEMUTEX_HPP
#define GENTYPE_ASEMUTEX_HPP

#include <stdexcept>
#include <cstdlib>

#ifdef __GNUC__
#include <pthread.h>
#include <errno.h>
#else
#include <windows.h>
#endif

#define ASEMUTEX_DEBUG(x)

struct ase_noncopyable {

  ase_noncopyable() { }

 private:

  ase_noncopyable(const ase_noncopyable&);
  ase_noncopyable& operator =(const ase_noncopyable&);

};

#ifdef __GNUC__

class ase_fast_mutex : public ase_noncopyable {

 public:

  ase_fast_mutex() {
    #ifdef ASE_DEBUG_MUTEX
    /* errorcheck mutex */
    pthread_mutexattr_t mattr;
    pthread_mutexattr_init(&mattr);
    pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_ERRORCHECK);
    int e = pthread_mutex_init(&mutex, &mattr);
    pthread_mutexattr_destroy(&mattr);
    #else
    int e = pthread_mutex_init(&mutex, 0);
    #endif
    if (e != 0) {
      if (e == ENOMEM) {
	throw std::bad_alloc(); /* ASE THROW */
      }
      throw std::runtime_error("failed to initialize mutex"); /* ASE THROW */
    }
  }
  ~ase_fast_mutex() /* DCHK */ {
    pthread_mutex_destroy(&mutex);
  }
  void lock() {
    ASEMUTEX_DEBUG(fprintf(stderr, "f lock %p %ld\n", &mutex,
      (long)pthread_self()));
    int e = pthread_mutex_lock(&mutex);
    if (e != 0) {
      #ifdef ASE_DEBUG_MUTEX
      ::abort();
      #endif
    }
  }
  void unlock() {
    ASEMUTEX_DEBUG(fprintf(stderr, "f unlock %p %ld\n", &mutex,
      (long)pthread_self()));
    int e = pthread_mutex_unlock(&mutex);
    if (e != 0) {
      #ifdef ASE_DEBUG_MUTEX
      ::abort();
      #endif
    }
  }

 private:

  pthread_mutex_t mutex;

};

class ase_recursive_mutex : public ase_noncopyable {

 public:

  ase_recursive_mutex() {
    pthread_mutexattr_t mattr;
    pthread_mutexattr_init(&mattr);
    pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE);
    int e = pthread_mutex_init(&mutex, &mattr);
    pthread_mutexattr_destroy(&mattr);
    if (e != 0) {
      if (e == ENOMEM) {
	throw std::bad_alloc(); /* ASE THROW */
      }
      throw std::runtime_error("failed to initialize mutex"); /* ASE THROW */
    }
  }
  ~ase_recursive_mutex() /* DCHK */ {
    pthread_mutex_destroy(&mutex);
  }
  void lock() {
    ASEMUTEX_DEBUG(fprintf(stderr, "r lock %p %ld\n", &mutex,
      (long)pthread_self()));
    int e = pthread_mutex_lock(&mutex);
    if (e != 0) {
      #ifdef ASE_DEBUG_MUTEX
      ::abort();
      #endif
    }
  }
  void unlock() {
    ASEMUTEX_DEBUG(fprintf(stderr, "r unlock %p %ld\n", &mutex,
      (long)pthread_self()));
    int e = pthread_mutex_unlock(&mutex);
    if (e != 0) {
      #ifdef ASE_DEBUG_MUTEX
      ::abort();
      #endif
    }
  }

 private:

  pthread_mutex_t mutex;

};

#else

class ase_fast_mutex : public ase_noncopyable {

 public:

  ase_fast_mutex() {
    InitializeCriticalSection(&cri);
  }
  ~ase_fast_mutex() /* DCHK */ {
    DeleteCriticalSection(&cri);
  }
  void lock() {
    ASEMUTEX_DEBUG(fprintf(stderr, "lock %p\n", &cri));
    EnterCriticalSection(&cri);
  }
  void unlock() {
    ASEMUTEX_DEBUG(fprintf(stderr, "unlock %p\n", &cri));
    LeaveCriticalSection(&cri);
  }

 private:

  CRITICAL_SECTION cri;

};

class ase_recursive_mutex : public ase_noncopyable {

 public:

  ase_recursive_mutex() {
    mutex = CreateMutex(0, FALSE, 0);
    if (mutex == 0) {
      throw std::runtime_error("failed to initialize mutex"); /* ASE THROW */
    }
  }
  ~ase_recursive_mutex() /* DCHK */ {
    CloseHandle(mutex);
  }
  void lock() {
    ASEMUTEX_DEBUG(fprintf(stderr, "lock %p\n", &mutex));
    WaitForSingleObject(mutex, INFINITE);
  }
  void unlock() {
    ASEMUTEX_DEBUG(fprintf(stderr, "unlock %p\n", &mutex));
    ReleaseMutex(mutex);
  }

 private:

  HANDLE mutex;

};

#endif

class ase_null_mutex : public ase_noncopyable {

 public:

  void lock() {
    ASEMUTEX_DEBUG(fprintf(stderr, "nulllock %p\n", &mutex));
  }
  void unlock() {
    ASEMUTEX_DEBUG(fprintf(stderr, "nullunlock %p\n", &mutex));
  }

};

template <typename Tmtx>
struct ase_mutex_guard {

  ase_mutex_guard(Tmtx& mtx) : mutex(mtx) {
    mutex.lock();
  }
  ~ase_mutex_guard() /* DCHK */ {
    mutex.unlock();
  }

 private:

  Tmtx& mutex;

};

template <typename Tmtx>
struct ase_mutex_unlock_guard {

  ase_mutex_unlock_guard(Tmtx& mtx) : mutex(mtx) {
    mutex.unlock();
  }
  ~ase_mutex_unlock_guard() /* DCHK */ {
    mutex.lock();
  }

 private:

  Tmtx& mutex;

};

template <typename T>
struct ase_synchronized {

  ase_synchronized() : value() { }
  ase_synchronized(const T& v) : value(v) { }
  T get() const {
    ase_mutex_guard<ase_fast_mutex> g(mutex);
    return value;
  }
  void set(const T& v) {
    ase_mutex_guard<ase_fast_mutex> g(mutex);
    value = v;
  }
  void swap(T& v) {
    ase_mutex_guard<ase_fast_mutex> g(mutex);
    value.swap(v);
  }

 private:

  mutable ase_fast_mutex mutex;
  T value;

};

#endif

