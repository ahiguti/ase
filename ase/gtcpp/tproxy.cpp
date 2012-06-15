
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/posix_error.hpp>
#include <gtcpp/tproxy.hpp>
#include <gtcpp/socket.hpp>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

#include <limits.h>
#include <linux/netfilter_ipv4.h>
#include "tproxy2/linux/netfilter_ipv4/ip_tproxy.h"

namespace gtcpp {

int
socket_inet_tproxy_bind(int fd, ::sockaddr_in *addr, posix_error_callback& ec)
{
  int r = ::bind(fd, reinterpret_cast< ::sockaddr *>(addr),
    sizeof(::sockaddr_in));
  GTCPP_CHECK_POSIX_ERROR_NEGATIVE(r, ec);
}

int
socket_inet_tproxy(int fd, u_long src_addr, u_long local_addr,
  posix_error_callback& ec)
{
  struct in_tproxy itp;
  std::memset(&itp, 0, sizeof(itp));
  itp.op = TPROXY_VERSION;
  itp.v.version = 0x02000000;
  int r = 0;
  if ((r = setsockopt(fd, SOL_IP, IP_TPROXY, &itp, sizeof(itp), ec)) < 0) {
    return r;
  }
  struct ::sockaddr_in sin;
  std::memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(local_addr);
  sin.sin_port = htons(0);
  if ((r = socket_inet_tproxy_bind(fd, &sin, ec)) < 0) {
    return r;
  }
  std::memset(&itp, 0, sizeof(itp));
  itp.op = TPROXY_ASSIGN;
  itp.v.addr.faddr.s_addr = htonl(src_addr);
  itp.v.addr.fport = htons(0);
  if ((r = setsockopt(fd, SOL_IP, IP_TPROXY, &itp, sizeof(itp), ec)) < 0) {
    return r;
  }
  #if 0
  std::memset(&itp, 0, sizeof(itp));
  itp.op = TPROXY_CONNECT;
  itp.v.addr.faddr.s_addr = htonl(dest_addr);
  itp.v.addr.fport = htons(dest_port);
  if ((r = setsockopt(fd, SOL_IP, IP_TPROXY, &itp, sizeof(itp), ec)) < 0) {
    return r;
  }
  #endif
  std::memset(&itp, 0, sizeof(itp));
  itp.op = TPROXY_FLAGS;
  itp.v.flags = ITP_CONNECT | ITP_ONCE;
  if ((r = setsockopt(fd, SOL_IP, IP_TPROXY, &itp, sizeof(itp), ec)) < 0) {
    return r;
  }
  return 0;
}

int
socket_inet_get_real_destination(int fd, ::sockaddr_in& dest_r,
  posix_error_callback& ec)
{
  int r = 0;
  socklen_t sz = sizeof(::sockaddr_in);
  if ((r = getsockopt(fd, SOL_IP, SO_ORIGINAL_DST, &dest_r, &sz, ec)) < 0) {
    return r;
  }
  return 0;
}

};

