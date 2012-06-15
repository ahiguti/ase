
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>
#include <sys/un.h>
#include <ase/aseio_getaddrinfo.hpp>
#include <ase/aseexcept.hpp>

void
ase_io_getaddrinfo(const char *node, const char *service,
  int flags, int family, int socktype, ase_sockaddr_generic& addr_r)
{
  addrinfo hints = { };
  hints.ai_flags = flags;
  hints.ai_family = family;
  hints.ai_socktype = socktype;
  addrinfo *res = 0;
  const int x = ::getaddrinfo(node, service, &hints, &res);
  if (x != 0) {
    ase_throw_exception("GetAddrInfoFailed", ase_string());
  }
  std::memcpy(addr_r.get_sockaddr(), res->ai_addr, res->ai_addrlen);
  addr_r.get_socklen() = res->ai_addrlen;
  ::freeaddrinfo(res);
}

namespace {

void
resolve_address_unix(const char *str, ase_sockaddr_generic& addr_r)
{
  ::sockaddr_un *const p = reinterpret_cast< ::sockaddr_un *>(
    addr_r.get_sockaddr());
  p->sun_family = AF_UNIX;
  std::strncpy(p->sun_path, str, sizeof(p->sun_path));
  p->sun_path[sizeof(p->sun_path) - 1] = '\0';
  addr_r.get_socklen() = sizeof(p->sun_path);
}

};

void
ase_io_hoststring_to_sockaddr(const char *str, ase_sockaddr_generic& addr_r)
{
  const int flags = 0;
  const int socktype = SOCK_STREAM;
  const char *str_end = str + std::strlen(str);
  int delim = '|';
  const char *c1 = std::strchr(str, '|');
  if (c1 == 0) {
    delim = ':';
    c1 = std::strchr(str, delim);
  }
  if (c1 != 0) {
    int family = AF_UNSPEC;
    const std::string col1(str, c1);
    const char *const c2 = std::strrchr(c1 + 1, delim);
    if (col1 == "unix" || col1 == "local") {
      resolve_address_unix(c1 + 1, addr_r);
      return;
    } else if (col1 == "inet" || col1 == "inet4" || col1 == "ipv4") {
      family = AF_INET;
    } else if (col1 == "inet6" || col1 == "ipv6") {
      family = AF_INET6;
    } else {
      family = AF_UNSPEC;
    }
    std::string host, port;
    if (family != AF_UNSPEC && c2 != 0) {
      /* family:host:port */
      host = std::string(c1 + 1, c2);
      port = std::string(c2 + 1, str_end);
    } else {
      /* host:port */
      host = std::string(str, c1);
      port = std::string(c1 + 1, str_end);
    }
    const char *const hostp = host.empty() ? 0 : host.c_str();
    ase_io_getaddrinfo(hostp, port.c_str(), flags, family, socktype, addr_r);
  } else {
    /* unix domain */
    resolve_address_unix(str, addr_r);
  }
}

