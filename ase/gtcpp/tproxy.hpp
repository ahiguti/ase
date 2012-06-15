
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_TPROXY_HPP
#define GTCPP_TPROXY_HPP

#include <netinet/in.h>
#include <sys/socket.h>

namespace gtcpp {

int socket_inet_tproxy(int fd, u_long src_addr, u_long local_addr,
  posix_error_callback& ec);
int socket_inet_get_real_destination(int fd, ::sockaddr_in& dest_r,
  posix_error_callback& ec);

};

#endif

