
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEIO_GETADDRINFO_HPP
#define GENTYPE_ASEIO_GETADDRINFO_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>
#include <netdb.h>
#include <ase/ase.hpp>

struct ase_sockaddr_generic {

 public:

  ase_sockaddr_generic() {
    std::memset(&storage, 0, sizeof(storage));
    len = sizeof(storage);
  }
  ase_sockaddr_generic(const ::sockaddr *addr, socklen_t addr_len) {
    std::memcpy(&storage, addr, addr_len);
    len = addr_len;
  }
  const ::sockaddr_storage *get_sockaddr_storage() const {
    return &storage;
  }
  ::sockaddr_storage *get_sockaddr_storage() {
    return &storage;
  }
  const ::sockaddr *get_sockaddr() const {
    return reinterpret_cast<const ::sockaddr *>(&storage);
  }
  ::sockaddr *get_sockaddr() {
    return reinterpret_cast< ::sockaddr *>(&storage);
  }
  socklen_t get_socklen() const {
    return len;
  }
  socklen_t& get_socklen() {
    return len;
  }

 private:

  ::sockaddr_storage storage;
  socklen_t len;

};

ASELIB_DLLEXT void ase_io_getaddrinfo(const char *node, const char *service,
  int flags, int family, int socktype, ase_sockaddr_generic& addr_r);
ASELIB_DLLEXT void ase_io_hoststring_to_sockaddr(const char *str,
  ase_sockaddr_generic& addr_r);

#endif

