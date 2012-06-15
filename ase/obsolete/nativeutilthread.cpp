
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asecpnt.hpp>
#include <ase/aseexcept.hpp>

#include <stdexcept>
#include <pthread.h>

namespace {

struct component_thread_klass {

  component_thread_klass(const ase_string& n, ASE_Component::Library& lib)
    : libref(lib) {
  }
  ase_variant yield(const ase_variant *args, ase_size_type nargs) {
    sched_yield();
    return ase_variant();
  }
  ase_variant sleep(const ase_variant *args, ase_size_type nargs) {
    if (nargs > 0) {
      ase_int msec = args[0].get_int();
      usleep(msec * 1000);
    }
    return ase_variant();
  }
  ase_variant noop(const ase_variant *args, ase_size_type nargs) {
    return ase_variant();
  }

  static const ASE_Component::MethodEntry<component_thread_klass>
    ClassMethods[];

  ASE_Component::Library& libref;

};

struct component_thread {

  component_thread(const component_thread_klass& kl,
    const ase_variant *args, ase_size_type nargs)
    : klass(kl), running(false), thrid(0), retval_is_oom(false) {
    if (nargs > 0) {
      threadmain = args[0];
    }
  }
  ~component_thread() {
    join(0, 0);
  }
  ase_variant start(const ase_variant *args, ase_size_type nargs) {
    if (running) {
      return ase_variant();
    }
    if (pthread_create(&thrid, 0, thread_main, this) != 0) {
      ase_throw_io_error("failed to create thread");
    }
    running = true;
    return ase_variant();
  }
  ase_variant join(const ase_variant *args, ase_size_type nargs) {
    if (!running) {
      return ase_variant();
    }
    if (pthread_join(thrid, 0)) {
      ase_throw_io_error("failed to join thread");
    }
    if (retval_is_oom) {
      throw std::bad_alloc();
    }
    running = false;
    return retval;
  }

  static const ASE_Component::MethodEntry<component_thread> InstanceMethods[];

 private:

  const component_thread_klass& klass;
  bool running;
  pthread_t thrid;
  ase_variant threadmain;
  ase_variant retval;
  bool retval_is_oom;

 private:

  static void *thread_main(void *p) {
    component_thread *selfp = static_cast<component_thread *>(p);
    selfp->klass.libref.GetUnifiedEngineRef().child_thread_initialized();
    try {
      selfp->retval = selfp->threadmain.invoke_self(0, 0);
    } catch (...) {
      selfp->retval = ase_variant::create_from_active_exception(
	selfp->retval_is_oom);
    }
    selfp->klass.libref.GetUnifiedEngineRef().child_thread_terminated();
    return 0;
  }

};

const ASE_Component::MethodEntry<component_thread_klass>
component_thread_klass::ClassMethods[] = {
  { "yield",  &component_thread_klass::yield },
  { "sleep",  &component_thread_klass::sleep },
  { "noop",   &component_thread_klass::noop },
};
const ASE_Component::MethodEntry<component_thread>
component_thread::InstanceMethods[] = {
  { "start",  &component_thread::start },
  { "join",   &component_thread::join },
};

}; // anonymous namespace

ase_variant
ASE_NewUtilThreadClass(const ase_string& name, ASE_Component::Library& lib)
{
  return ASE_Component::NewClass<
    component_thread_klass, component_thread, true
  >(name, lib);
}

