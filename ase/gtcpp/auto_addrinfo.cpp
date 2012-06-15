
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/auto_addrinfo.hpp>
#include <gtcpp/string_ref.hpp>
#include <errno.h>
#include <sys/un.h>
#include <arpa/inet.h>

namespace gtcpp {

auto_addrinfo::auto_addrinfo()
  : ptr(0)
{
}

auto_addrinfo::~auto_addrinfo()
{
  reset();
}

size_t
auto_addrinfo::size() const
{
  size_t i = 0;
  for (const ::addrinfo *p = ptr; p; p = p->ai_next) {
    ++i;
  }
  return i;
}

void
auto_addrinfo::reset()
{
  if (ptr) {
    ::freeaddrinfo(ptr);
  }
  ptr = 0;
}

namespace {

std::string
af_inet_to_string(const ::sockaddr *addr)
{
  const ::sockaddr_in *const p = reinterpret_cast<const ::sockaddr_in *>(
    addr);
  char buf[INET_ADDRSTRLEN];
  buf[0] = '\0';
  ::inet_ntop(AF_INET, &p->sin_addr, buf, sizeof(buf));
  buf[sizeof(buf) - 1] = '\0';
  return std::string(buf);
}

std::string
af_inet6_to_string(const ::sockaddr *addr)
{
  const ::sockaddr_in6 *const p = reinterpret_cast<const ::sockaddr_in6 *>(
    addr);
  char buf[INET6_ADDRSTRLEN];
  buf[0] = '\0';
  ::inet_ntop(AF_INET6, &p->sin6_addr, buf, sizeof(buf));
  buf[sizeof(buf) - 1] = '\0';
  return std::string(buf);
}

std::string
af_unix_to_string(const ::sockaddr *addr)
{
  const ::sockaddr_un *const p = reinterpret_cast<const ::sockaddr_un *>(
    addr);
  return std::string(p->sun_path);
}

};

std::string
sockaddr_generic::to_string() const
{
  const int family = storage.ss_family;
  switch (family) {
  case AF_INET:
    return af_inet_to_string(addr());
  case AF_INET6:
    return af_inet6_to_string(addr());
  case AF_UNIX:
    return af_unix_to_string(addr());
  }
  return std::string();
}

int
getaddrinfo(auto_addrinfo& ai, const char *node, const char *service,
  int flags, int family, int socktype, int protocol,
  addrinfo_error_callback& ec)
{
  ai.reset();
  ::addrinfo hint;
  hint.ai_flags = flags;
  hint.ai_family = family;
  hint.ai_socktype = socktype;
  hint.ai_protocol = protocol;
  hint.ai_addrlen = 0;
  hint.ai_addr = 0;
  hint.ai_canonname = 0;
  hint.ai_next = 0;
  const int gai_error = ::getaddrinfo(node, service, &hint, &ai.ptr);
  if (gai_error != 0) {
    return ec.on_addrinfo_error(gai_error, __PRETTY_FUNCTION__);
  }
  return 0;
}

void
resolve_address_unix(const char *str, sockaddr_generic& addr_r)
{
  ::sockaddr_un *const p = reinterpret_cast< ::sockaddr_un *>(addr_r.addr());
  p->sun_family = AF_UNIX;
  std::strncpy(p->sun_path, str, sizeof(p->sun_path));
  p->sun_path[sizeof(p->sun_path) - 1] = '\0';
  *(addr_r.length()) = sizeof(p->sun_path);
}

int
resolve_address(const char *str, int flags, int socktype, int protocol,
  sockaddr_generic& addr_r, addrinfo_error_callback& ec)
{
  const char *str_end = str + std::strlen(str);
  int delim = '|';
  const char *c1 = std::strchr(str, '|');
  if (c1 == 0) {
    delim = ':';
    c1 = std::strchr(str, delim);
  }
  if (c1 != 0) {
    int family = AF_UNSPEC;
    const string_ref col1(str, c1);
    const char *const c2 = std::strrchr(c1 + 1, delim);
    if (col1 == "unix" || col1 == "local") {
      resolve_address_unix(c1 + 1, addr_r);
      return 0;
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
    auto_addrinfo ai;
    const char *const hostp = host.empty() ? 0 : host.c_str();
    const int r = getaddrinfo(ai, hostp, port.c_str(), flags, family,
      socktype, protocol, ec);
    if (r < 0) {
      return r;
    }
    if (ai.size()) {
      const ::sockaddr *const ai_addr = ai->ai_addr;
      const socklen_t ai_addrlen = ai->ai_addrlen;
      std::memcpy(addr_r.addr(), ai_addr, ai_addrlen);
      *(addr_r.length()) = ai_addrlen;
    }
    return 0;
  } else {
    /* unix domain */
    resolve_address_unix(str, addr_r);
    return 0;
  }
}

void
throw_addrinfo_error(int e, const char *func)
{
  throw addrinfo_error(e, "AddrinfoError", gai_strerror(e), func);
}

};

