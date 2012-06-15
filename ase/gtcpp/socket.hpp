
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_SOCKET_HPP
#define GTCPP_SOCKET_HPP

#include <gtcpp/auto_addrinfo.hpp>
#include <gtcpp/auto_file.hpp>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/un.h>

namespace gtcpp {

int socket(auto_file& sd_r, int domain, int type, int protocol,
  posix_error_callback& ec);
int socket(auto_file& sd_r, const ::addrinfo& ai, posix_error_callback& ec);

int bind(const auto_file& sd, const sockaddr_generic& addr,
  posix_error_callback& ec);
int bind(const auto_file& sd, const ::addrinfo& ai, posix_error_callback& ec);
int listen(const auto_file& sd, int backlog, posix_error_callback& ec);
int connect(const auto_file& sd, const sockaddr *addr, socklen_t addrlen,
  posix_error_callback& ec);
int connect(const auto_file& sd, const sockaddr_generic& addr,
  posix_error_callback& ec);
int connect(const auto_file& sd, const ::addrinfo& ai,
  posix_error_callback& ec);
int accept(int sd, auto_file& sd_r, sockaddr_generic& addr_r,
  posix_error_callback& ec);
int accept(const auto_file& sd, auto_file& sd_r, sockaddr_generic& addr_r,
  posix_error_callback& ec);
int getsockopt(int sd, int level, int optname, void *optval,
  socklen_t *optlen_io, posix_error_callback& ec);
int getsockopt(const auto_file& sd, int level, int optname, void *optval,
  socklen_t *optlen_io, posix_error_callback& ec);
int setsockopt(int sd, int level, int optname,
  const void *optval, socklen_t optlen, posix_error_callback& ec);
int setsockopt(const auto_file& sd, int level, int optname,
  const void *optval, socklen_t optlen, posix_error_callback& ec);
int shutdown(const auto_file& sd, int how, posix_error_callback& ec);

void socket_listen(auto_file& sd_r, const char *node,
  int backlog = 128, bool reuseaddr = true);
void socket_connect(auto_file& sd_r, const char *node, int timeout_sec = 0);
int close_gentle(const auto_file& sd, posix_error_callback& ec);

};

#endif

