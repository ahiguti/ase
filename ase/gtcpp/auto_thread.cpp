
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/auto_thread.hpp>
#include <gtcpp/auto_threadattr.hpp>
#include <gtcpp/posix_error.hpp>
#include <boost/static_assert.hpp>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <signal.h>

namespace gtcpp {

struct auto_thread::impl_type {

  impl_type() : joined(true) { }
  static void *start_routine(void *arg);
  void start(auto_thread *ptr, const auto_threadattr *attr);

  pthread_t thr;
  bool joined : 1;

};

void *
auto_thread::impl_type::start_routine(void *arg)
{
  auto_thread *p = static_cast<auto_thread *>(arg);
  try {
    p->run();
  } catch (const std::exception& e) {
    BOOST_STATIC_ASSERT((sizeof(unsigned long) == sizeof(pthread_t)));
    fprintf(stderr, "thread %lx: uncaught exception: %s\n",
      (unsigned long)(pthread_self()), e.what());
    abort();
  } catch (...) {
    BOOST_STATIC_ASSERT((sizeof(unsigned long) == sizeof(pthread_t)));
    fprintf(stderr, "thread %lx: uncaught exception\n",
      (unsigned long)(pthread_self()));
    abort();
  }
  return 0;
}

void
auto_thread::impl_type::start(auto_thread *ptr, const auto_threadattr *attr)
{
  if (!joined) {
    throw_posix_error(EINVAL, __PRETTY_FUNCTION__);
  }
  const pthread_attr_t *pattr = attr ? &attr->get() : 0;
  int e = pthread_create(&thr, pattr, start_routine, ptr);
  if (e != 0) {
    throw_posix_error(e, __PRETTY_FUNCTION__);
  }
  joined = false;
}

auto_thread::auto_thread()
  : impl(new impl_type())
{
}

auto_thread::~auto_thread()
{
  assert(impl->joined);
}

void
auto_thread::start(const auto_threadattr& attr)
{
  impl->start(this, &attr);
}

void
auto_thread::start()
{
  impl->start(this, 0);
}

int
auto_thread::kill(int signo)
{
  int e = pthread_kill(impl->thr, signo);
  if (e == ESRCH) {
    return -1;
  }
  if (e != 0) {
    throw_posix_error(e, __PRETTY_FUNCTION__);
  }
  return 0;
}

void
auto_thread::join()
{
  if (impl->joined) {
    throw_posix_error(EINVAL, __PRETTY_FUNCTION__);
  }
  int e = pthread_join(impl->thr, 0);
  if (e != 0) {
    throw_posix_error(e, __PRETTY_FUNCTION__);
  }
  impl->joined = true;
}

bool
auto_thread::joined() const
{
  return impl->joined;
}

};

