
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_AUTO_ADDRINFO_HPP
#define GTCPP_AUTO_ADDRINFO_HPP

#include <gtcpp/error.hpp>
#include <boost/noncopyable.hpp>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stddef.h>
#include <string>
#include <cstring>

namespace gtcpp {

struct addrinfo_error_callback;

class auto_addrinfo : private boost::noncopyable {

 public:

  auto_addrinfo();
  ~auto_addrinfo();
  const ::addrinfo& operator *() const { return *ptr; }
  const ::addrinfo *operator ->() const { return ptr; }
  size_t size() const;
  void reset();

  friend int getaddrinfo(auto_addrinfo& ai, const char *node,
    const char *service, int flags, int family, int socktype, int protocol,
    addrinfo_error_callback& ec);

 private:

  ::addrinfo *ptr;

};

class sockaddr_generic {

 public:

  sockaddr_generic() {
    std::memset(&storage, 0, sizeof(storage));
    len = sizeof(storage);
  }
  sockaddr_generic(const ::sockaddr *addr, socklen_t addr_len) {
    std::memcpy(&storage, addr, addr_len);
    len = addr_len;
  }
  const ::sockaddr *addr() const {
    return reinterpret_cast<const ::sockaddr *>(&storage);
  }
  ::sockaddr *addr() {
    return reinterpret_cast< ::sockaddr *>(&storage);
  }
  const socklen_t *length() const {
    return &len;
  }
  socklen_t *length() {
    return &len;
  }
  std::string to_string() const;

 private:

  ::sockaddr_storage storage;
  socklen_t len;

};

typedef error<auto_addrinfo> addrinfo_error;

void throw_addrinfo_error(int e, const char *func) __attribute__((noreturn));

struct addrinfo_error_callback {

  virtual ~addrinfo_error_callback() { }
  /* on_addrinfo_error should return a negative value, or throw an exception */
  virtual int on_addrinfo_error(int e, const char *func) {
    throw_addrinfo_error(e, func);
  }

};

int getaddrinfo(auto_addrinfo& ai, const char *node, const char *service,
  int flags, int family, int socktype, int protocol,
  addrinfo_error_callback& ec);
int resolve_address(const char *str, int flags, int socktype, int protocol,
  sockaddr_generic& addr_r, addrinfo_error_callback& ec);

};

#endif

