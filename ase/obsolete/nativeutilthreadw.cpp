
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asecpnt.hpp>
#include <ase/aseexcept.hpp>

#include <windows.h>
#include <process.h>

namespace {

struct component_thread_klass {

  component_thread_klass(const ase_string& n, ASE_Component::Library& lib) { }
  
  ase_variant yield(const ase_variant *args, ase_size_type nargs) {
    ::Sleep(0);
    return ase_variant();
  }
  ase_variant sleep(const ase_variant *args, ase_size_type nargs) {
    if (nargs > 0) {
      ase_int msec = args[0].get_int();
      ::Sleep(msec);
    }
    return ase_variant();
  }
  ase_variant noop(const ase_variant *args, ase_size_type nargs) {
    return ase_variant();
  }

  static const ASE_Component::MethodEntry<component_thread_klass>
    ClassMethods[];

};

struct component_thread {

  component_thread(const component_thread_klass& klass,
    const ase_variant *args, ase_size_type nargs) : running(false),
    thrhnd(0), thrid(0) {
    if (nargs > 0) {
      threadmain = args[0];
    }
  }
  
  ase_variant start(const ase_variant *args, ase_size_type nargs) {
    if (running) {
      return ase_variant();
    }
    thrhnd = (HANDLE)_beginthreadex(0, 0, thread_main, this, 0, &thrid);
    if (thrhnd == 0) {
      ase_throw_io_error("failed to create thread");
    }
    running = true;
    return ase_variant();
  }
  ase_variant join(const ase_variant *args, ase_size_type nargs) {
    if (!running) {
      return ase_variant();
    }
    if (WaitForSingleObject(thrhnd, INFINITE) != 0) {
      ase_throw_io_error("failed to join thread");
    }
    CloseHandle(thrhnd);
    thrhnd = 0;
    thrid = 0;
    running = false;
    return retval;
  }

  static const ASE_Component::MethodEntry<component_thread> InstanceMethods[];

 private:

  bool running;
  HANDLE thrhnd;
  unsigned int thrid;
  ase_variant threadmain;
  ase_variant retval;

 private:

  static unsigned __stdcall thread_main(void *p) {
    component_thread *selfp = static_cast<component_thread *>(p);
    try {
      selfp->retval = selfp->threadmain.invoke_self(0, 0);
    } catch (...) {
      /* TODO: show exception */
    }
    return 0;
  }

};

const ASE_Component::MethodEntry<component_thread_klass>
component_thread_klass::ClassMethods[] = {
  { "yield", &component_thread_klass::yield },
  { "sleep", &component_thread_klass::sleep },
  { "noop", &component_thread_klass::noop },
};
const ASE_Component::MethodEntry<component_thread>
component_thread::InstanceMethods[] = {
  { "start", &component_thread::start },
  { "join", &component_thread::join },
};

}; // anonymous namespace

ase_variant
ASE_NewUtilThreadClass(const ase_string& n, ASE_Component::Library& lib)
{
  return ASE_Component::NewClass<
    component_thread_klass, component_thread, true
  >(n, lib);
}

