
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/auto_thread.hpp>
#include <gtcpp/socket.hpp>
#include <gtcpp/file_util.hpp>
#include <gtcpp/scoped_sigaction.hpp>
#include <gtcpp/debugcount.hpp>
#include <iostream>
#include <vector>
#include <signal.h>

using namespace gtcpp;

namespace {

class error_callback :
  public posix_error_callback,
  public addrinfo_error_callback {
};

};

void test_sock(int argc, char **argv)
{
  using namespace gtcpp;
  if (argc < 2) {
    throw std::runtime_error("usage: sock HOST SERVICE");
  }
  error_callback ec;
  auto_addrinfo ai;
  getaddrinfo(ai, argv[0], argv[1], 0, AF_INET, SOCK_STREAM, 0, ec);
  auto_file sd;
  socket(sd, *ai, ec);
  connect(sd, *ai, ec);
  std::cerr << "done" << std::endl;
}

void test_sockcli(int argc, char **argv)
{
  using namespace gtcpp;
  if (argc < 1) {
    throw std::runtime_error("usage: sockcli HOST");
  }
  auto_file sd;
  socket_connect(sd, argv[0]);
}

void test_sockserv(int argc, char **argv)
{
  if (argc < 1) {
    throw std::runtime_error("usage: sockserv HOST");
  }
  using namespace gtcpp;
  auto_file sd;
  socket_listen(sd, argv[0]);
}

void test_servsink(int argc, char **argv)
{
  using namespace gtcpp;
  if (argc < 1) {
    throw std::runtime_error("usage: servsink HOST");
  }
  error_callback ec;
  auto_file sd, sdconn;
  socket_listen(sd, argv[0]);
  sockaddr_generic cliaddr;
  accept(sd, sdconn, cliaddr, ec);
  while (true) {
    char buffer[1024];
    int r = 0;
    if ((r = read(sdconn, buffer, 1024, ec)) <= 0) {
      break;
    }
  }
  std::cerr << "done" << std::endl;
}

void test_servclose(int argc, char **argv)
{
  using namespace gtcpp;
  if (argc < 2) {
    throw std::runtime_error("usage: servclose HOST BYTES");
  }
  error_callback ec;
  auto_file sd, sdconn;
  socket_listen(sd, argv[0]);
  int bytes = atoi(argv[1]);
  sockaddr_generic cliaddr;
  while (true) {
    char buffer[bytes]; /* gnu extension */
    accept(sd, sdconn, cliaddr, ec);
    if (bytes > 0) {
      read(sdconn, buffer, bytes, ec);
    }
    #if 0
    size_t len_r;
    read_all(sdconn, buffer, bytes, len_r, ec);
    #endif
    close(sdconn, ec); /* close immediately */
  }
}

void test_clisend(int argc, char **argv)
{
  using namespace gtcpp;
  if (argc < 1) {
    throw std::runtime_error("usage: clisend HOST [IGNORE_SIGPIPE]");
  }
  error_callback ec;
  std::auto_ptr<scoped_sigaction> ssa;
  if (argc > 1 && atoi(argv[1]) != 0) {
    ssa.reset(new scoped_sigaction(SIGPIPE, false));
    std::cerr << "ignore sigpipe" << std::endl;
  }
  auto_file sd;
  socket_connect(sd, argv[0]);
  while (true) {
    std::string str("******************************");
    write(sd, str.data(), str.size(), ec);
    #if 0
    usleep(10);
    #endif
  }
  std::cerr << "done" << std::endl;
}

