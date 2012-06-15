
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/socket.hpp>
#include <gtcpp/auto_dir.hpp>
#include <errno.h>

namespace gtcpp {

int
socket(auto_file& sd_r, int domain, int type, int protocol,
  posix_error_callback& ec)
{
  const int sd = ::socket(domain, type, protocol);
  GTCPP_CHECK_POSIX_ERROR_FD(sd, sd_r, ec);
}

int
socket(auto_file& sd_r, const ::addrinfo& ai, posix_error_callback& ec)
{
  const int sd = ::socket(ai.ai_family, ai.ai_socktype, ai.ai_protocol);
  GTCPP_CHECK_POSIX_ERROR_FD(sd, sd_r, ec);
}

int
bind(const auto_file& sd, const sockaddr_generic& addr,
  posix_error_callback& ec)
{
  const ::sockaddr *const p = addr.addr();
  const socklen_t plen = *addr.length();
  int r = ::bind(sd.get(), p, plen);
  GTCPP_CHECK_POSIX_ERROR_NEGATIVE(r, ec);
}

int
bind(const auto_file& sd, const ::addrinfo& ai, posix_error_callback& ec)
{
  int r = ::bind(sd.get(), ai.ai_addr, ai.ai_addrlen);
  GTCPP_CHECK_POSIX_ERROR_NEGATIVE(r, ec);
}

int
listen(const auto_file& sd, int backlog, posix_error_callback& ec)
{
  const int r = ::listen(sd.get(), backlog);
  GTCPP_CHECK_POSIX_ERROR_NEGATIVE(r, ec);
}

int
accept(int sd, auto_file& sd_r, sockaddr_generic& addr_r,
  posix_error_callback& ec)
{
  const int r = ::accept(sd, addr_r.addr(), addr_r.length());
  GTCPP_CHECK_POSIX_ERROR_FD(r, sd_r, ec);
}

int
accept(const auto_file& sd, auto_file& sd_r, sockaddr_generic& addr_r,
  posix_error_callback& ec)
{
  return accept(sd.get(), sd_r, addr_r, ec);
}

int
connect(const auto_file& sd, const sockaddr *addr, socklen_t addrlen,
  posix_error_callback& ec)
{
  const int r = ::connect(sd.get(), addr, addrlen);
  GTCPP_CHECK_POSIX_ERROR_NEGATIVE(r, ec);
}

int
connect(const auto_file& sd, const sockaddr_generic& addr,
  posix_error_callback& ec)
{
  const ::sockaddr *const p = addr.addr();
  const socklen_t plen = *addr.length();
  return connect(sd, p, plen, ec);
}

int
connect(const auto_file& sd, const ::addrinfo& ai, posix_error_callback& ec)
{
  const int r = ::connect(sd.get(), ai.ai_addr, ai.ai_addrlen);
  GTCPP_CHECK_POSIX_ERROR_NEGATIVE(r, ec);
}

int
getsockopt(int sd, int level, int optname, void *optval,
  socklen_t *optlen_io, posix_error_callback& ec)
{
  const int r = ::getsockopt(sd, level, optname, optval, optlen_io);
  GTCPP_CHECK_POSIX_ERROR_NEGATIVE(r, ec);
}

int
getsockopt(const auto_file& sd, int level, int optname, void *optval,
  socklen_t *optlen_io, posix_error_callback& ec)
{
  return getsockopt(sd.get(), level, optname, optval, optlen_io, ec);
}

int
setsockopt(int sd, int level, int optname, const void *optval,
  socklen_t optlen, posix_error_callback& ec)
{
  const int r = ::setsockopt(sd, level, optname, optval, optlen);
  GTCPP_CHECK_POSIX_ERROR_NEGATIVE(r, ec);
}

int
setsockopt(const auto_file& sd, int level, int optname, const void *optval,
  socklen_t optlen, posix_error_callback& ec)
{
  return setsockopt(sd.get(), level, optname, optval, optlen, ec);
}

int
shutdown(const auto_file& sd, int how, posix_error_callback& ec)
{
  const int r = ::shutdown(sd.get(), how);
  GTCPP_CHECK_POSIX_ERROR_NEGATIVE(r, ec);
}

void
socket_listen(auto_file& sd_r, const char *node, int backlog, bool reuseaddr)
{
  posix_error_callback pec;
  addrinfo_error_callback aec;
  sockaddr_generic addr;
  resolve_address(node, AI_PASSIVE, SOCK_STREAM, 0, addr, aec);
  const int family = addr.addr()->sa_family;
  socket(sd_r, family, SOCK_STREAM, 0, pec);
  if (family == AF_INET || family == AF_INET6) {
    if (reuseaddr) {
      const int v = 1;
      setsockopt(sd_r, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v), pec);
    }
    {
      const int v = 1;
      setsockopt(sd_r, IPPROTO_TCP, TCP_NODELAY, &v, sizeof(v), pec);
    }
  } else if (family == AF_UNIX) {
    if (reuseaddr) {
      const ::sockaddr_un *p = reinterpret_cast<const ::sockaddr_un *>(
	addr.addr());
      ::unlink(p->sun_path);
    }
  }
  bind(sd_r, addr, pec);
  listen(sd_r, backlog, pec);
}

void
socket_connect(auto_file& sd_r, const char *node, int timeout_sec)
{
  posix_error_callback pec;
  addrinfo_error_callback aec;
  sockaddr_generic addr;
  resolve_address(node, AI_PASSIVE, SOCK_STREAM, 0, addr, aec);
  const int family = addr.addr()->sa_family;
  socket(sd_r, family, SOCK_STREAM, 0, pec);
  if (family == AF_INET || family == AF_INET6) {
    const int v = 1;
    setsockopt(sd_r, IPPROTO_TCP, TCP_NODELAY, &v, sizeof(v), pec);
  }
  if (timeout_sec > 0) {
    struct timeval tv;
    tv.tv_sec = timeout_sec;
    tv.tv_usec = 0;
    setsockopt(sd_r, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv), pec);
    tv.tv_sec = timeout_sec;
    tv.tv_usec = 0;
    setsockopt(sd_r, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv), pec);
  }
  connect(sd_r, addr, pec);
}

int
close_gentle(const auto_file& sd, posix_error_callback& ec)
{
  char buf[1024];
  int r = 0;
  if ((r = shutdown(sd, SHUT_WR, ec)) < 0) {
    return r;
  }
  while ((r = read(sd, buf, sizeof(buf), ec)) > 0) {
    /* ignore */
  }
  return r;
}

};

