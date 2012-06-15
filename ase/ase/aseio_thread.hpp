
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEIO_THREAD_HPP
#define GENTYPE_ASEIO_THREAD_HPP

#include <ase/ase.hpp>

struct ase_io_thread {

  virtual ~ase_io_thread() { }
  virtual void destroy() = 0;
  virtual void start(size_t stack_size, void (*f)(void *), void *arg) = 0;

};

ASELIB_DLLEXT void ase_io_thread_create(ase_auto_ptr<ase_io_thread>& ptr_r);

#endif

