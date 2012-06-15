
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEIO_STREAM_HPP
#define GENTYPE_ASEIO_STREAM_HPP

#include <ase/ase.hpp>

struct ase_io_stream {

  virtual ~ase_io_stream() { }
  virtual void destroy() = 0;
  virtual void close() = 0;
  virtual int write(const char *buf, ase_size_type size) = 0;
  virtual int read(char *buf, int size) = 0;

};

#endif

