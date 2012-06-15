
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEIO_LISTENER_HPP
#define GENTYPE_ASEIO_LISTENER_HPP

#include <ase/ase.hpp>
#include <ase/aseio_stream.hpp>
#include <ase/aseio_getaddrinfo.hpp>

struct ase_io_socket_options {
  int timeout_sec;
  int sndbuf;
  int rcvbuf;
  ase_io_socket_options() : timeout_sec(600), sndbuf(0), rcvbuf(0) { }
};

struct ase_io_listener {

  virtual ~ase_io_listener() { }
  virtual void destroy() = 0;
  virtual void listen(const ase_sockaddr_generic& addr) = 0;
  virtual void accept(ase_auto_ptr<ase_io_stream>& stream_r,
    ase_sockaddr_generic& addr_r) = 0; /* must be multi-thread safe */

};

ASELIB_DLLEXT void ase_io_listener_socket_create(
  ase_auto_ptr<ase_io_listener>& ptr_r, const ase_io_socket_options& opts);
ASELIB_DLLEXT void ase_io_socket_create(
  ase_auto_ptr<ase_io_stream>& ptr_r, const ase_sockaddr_generic& addr,
  const ase_io_socket_options& opts);

#endif

