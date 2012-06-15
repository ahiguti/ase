
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASERMT_CONCURRENT_SERVER_HPP
#define GENTYPE_ASERMT_CONCURRENT_SERVER_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <memory>
#include <string>

#include <ase/aseio_listener.hpp>
#include <ase/aseio_thread.hpp>
#include <ase/aseruntime.hpp>
#include <ase/asermt_protocol.hpp>

struct ase_remote_concurrent_server_options {

  ase_sockaddr_generic address_to_bind;
  int num_procs;
  int num_threads;
  size_t thread_stack_size;
  ase_string serializer;

  ase_remote_concurrent_server_options() : num_procs(0), num_threads(100),
    thread_stack_size(1024 * 1024) { }

};

ASELIB_DLLEXT int ase_remote_concurrent_server_main(ase_io_listener& lsn,
  ase_unified_engine_ref& eng, const ase_variant& topobj,
  const ase_remote_concurrent_server_options& o);

#endif

