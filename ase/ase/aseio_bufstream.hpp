
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEIO_BUFSTREAM_HPP
#define GENTYPE_ASEIO_BUFSTREAM_HPP

#include <ase/ase.hpp>
#include <ase/aseio_stream.hpp>

struct ase_io_buffered_stream {

  virtual ~ase_io_buffered_stream() { }
  virtual void destroy() = 0;
  virtual void close() = 0;
  virtual void write(const char *buf, ase_size_type size) = 0;
  virtual void read_more() = 0;
    /* should throw in case of eof */
  virtual void get_readbuf(const char *& buf_r, ase_size_type& size_r) = 0;
    /* buf_r and size_r must be valid until read_more() is called */
  virtual void readbuf_consume(ase_size_type size) = 0;
    /* removes from read buffer */

};

void ase_io_buffered_stream_create(
  ase_auto_ptr<ase_io_stream>& base_buf /* move */,
  ase_auto_ptr<ase_io_buffered_stream>& bufstr_r);

#endif

