
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <pthread.h>
#include <ase/aseio_thread.hpp>
#include <ase/aseexcept.hpp>

struct ase_io_thread_impl : public ase_io_thread, private ase_noncopyable {

  ase_io_thread_impl();
  ~ase_io_thread_impl();
  void destroy();
  void start(size_t stack_size, void (*f)(void *), void *);

  static void *thr_main(void *x);

  bool running;
  pthread_t thr;
  void (*func)(void *);
  void *arg;

};

ase_io_thread_impl::ase_io_thread_impl()
  : running(false), thr(), func(0), arg(0)
{
}

ase_io_thread_impl::~ase_io_thread_impl()
{
  if (running) {
    pthread_join(thr, 0);
  }
}

void
ase_io_thread_impl::destroy()
{
  delete this;
}

void *
ase_io_thread_impl::thr_main(void *x)
{
  ase_io_thread_impl *const p = static_cast<ase_io_thread_impl *>(x);
  p->func(p->arg);
  return 0;
}

void
ase_io_thread_impl::start(size_t stack_size, void (*f)(void *), void *a)
{
  if (running) {
    ase_throw_exception("ASEIOThreadAlreadyRunning", ase_string());
  }
  this->func = f;
  this->arg = a;
  int r = 0;
  if (stack_size == 0) {
    r = pthread_create(&thr, 0, thr_main, this);
  } else {
    pthread_attr_t attr;
    if (pthread_attr_init(&attr) != 0) {
      ase_throw_exception("ASEIOThreadAttrInit", ase_string());
    }
    pthread_attr_setstacksize(&attr, stack_size);
    r = pthread_create(&thr, &attr, thr_main, this);
    pthread_attr_destroy(&attr);
  }
  if (r != 0) {
    ase_throw_exception("ASEIOThreadStart", ase_string());
  }
  this->running = true;
}

void
ase_io_thread_create(ase_auto_ptr<ase_io_thread>& ptr_r)
{
  ptr_r.reset(new ase_io_thread_impl());
}

