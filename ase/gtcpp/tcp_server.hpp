
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_TCP_SERVER_HPP
#define GTCPP_TCP_SERVER_HPP

#include <gtcpp/posix_error.hpp>
#include <gtcpp/auto_thread.hpp>
#include <gtcpp/socket.hpp>
#include <memory>

namespace gtcpp {

struct tcp_server;
struct tcp_server_worker;

/* implements common tasks for multi-process, multi-thread tcp servers */
struct tcp_server {

  struct options {
    std::string address_to_bind;
    bool block_sigpipe;
    int num_procs;
    int num_threads;
    size_t thread_stack_size;
    int worker_socket_timeout_sec;
    options() : block_sigpipe(true), num_procs(0), num_threads(100),
      thread_stack_size(1024 * 1024), worker_socket_timeout_sec(300) { }
  };

  virtual ~tcp_server() { }
  virtual void worker_process_initialized() = 0;
  virtual void create_worker(std::auto_ptr<tcp_server_worker>& wkr_r) = 0;

};

struct tcp_server_worker {

  virtual ~tcp_server_worker() { }
  virtual int run(auto_file& conn, const sockaddr_generic& client_addr,
    posix_error_callback& ec) = 0;

};

int concurrent_tcp_server_main(tcp_server& svr, const tcp_server::options& o);

};

#endif

