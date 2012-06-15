
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/aseio_listener.hpp>
#include <ase/aseexcept.hpp>
#include <netinet/in.h>
#include <netinet/tcp.h>

struct ase_io_stream_socket : public ase_io_stream, private ase_noncopyable {

  ase_io_stream_socket();
  virtual ~ase_io_stream_socket();
  virtual void destroy();
  virtual void close();
  virtual int write(const char *buf, ase_size_type size);
  virtual int read(char *buf, int size);
  void reset(int fd);

 private:

  const bool is_open() const { return fd >= 0; }
  int fd;

};

ase_io_stream_socket::ase_io_stream_socket()
  : fd(-1)
{
}

ase_io_stream_socket::~ase_io_stream_socket()
{
  close();
}

void
ase_io_stream_socket::destroy()
{
  delete this;
}

void
ase_io_stream_socket::close()
{
  if (is_open()) {
    ::close(fd);
    fd = -1;
  }
}

int
ase_io_stream_socket::write(const char *buf, ase_size_type size)
{
  if (is_open()) {
    return ::write(fd, buf, size);
  } else {
    return -1;
  }
}

int
ase_io_stream_socket::read(char *buf, int size)
{
  if (is_open()) {
    return ::read(fd, buf, size);
  } else {
    return -1;
  }
}

void
ase_io_stream_socket::reset(int f)
{
  close();
  fd = f;
}

struct ase_io_listener_socket
  : public ase_io_listener, private ase_noncopyable  {

  ase_io_listener_socket(const ase_io_socket_options& o);
  virtual ~ase_io_listener_socket();
  virtual void destroy();
  virtual void listen(const ase_sockaddr_generic& addr);
  virtual void accept(ase_auto_ptr<ase_io_stream>& stream_r,
    ase_sockaddr_generic& addr_r);

 private:

  void close();
  void throw_exception(const char *id) {
    close();
    ase_throw_exception(id, ase_string());
  }
  const bool is_open() const { return fd >= 0; }
  int fd;
  const ase_io_socket_options opts;

};

ase_io_listener_socket::ase_io_listener_socket(
  const ase_io_socket_options& o)
  : fd(-1), opts(o)
{
}

ase_io_listener_socket::~ase_io_listener_socket()
{
  close();
}

void
ase_io_listener_socket::destroy()
{
  delete this;
}

void
ase_io_listener_socket::close()
{
  if (is_open()) {
    ::close(fd);
    fd = -1;
  }
}

void
ase_io_listener_socket::listen(const ase_sockaddr_generic& addr)
{
  if (is_open()) {
    throw_exception("ListenerAlreadyListening");
  }
  const int family = addr.get_sockaddr()->sa_family;
  fd = ::socket(family, SOCK_STREAM, 0);
  if (fd < 0) {
    throw_exception("ListenerSocketFailed");
  }
  if (family == AF_INET || family == AF_INET6 || family == AF_UNIX) {
    int v = 1;
    if (::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v)) < 0) {
      throw_exception("ListenerReuseAddrFailed");
    }
  }
  if (family == AF_INET || family == AF_INET6) {
    int v = 1;
    if (::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &v, sizeof(v)) < 0) {
      throw_exception("ListenerNoDelayFailed");
    }
  }
  if (::bind(fd, addr.get_sockaddr(), addr.get_socklen()) < 0) {
    throw_exception("ListenerBindFailed");
  }
  const int backlog_size = 1024;
  if (::listen(fd, backlog_size) < 0) {
    throw_exception("ListenerListenFailed");
  }
}

namespace {

void
set_socket_options(int fd, const ase_io_socket_options& opts)
{
  timeval tv = { };
  tv.tv_sec = opts.timeout_sec;
  tv.tv_usec = 0;
  if (::setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0) {
    ase_throw_exception("SocketSndTimeoFailed", ase_string());
  }
  tv.tv_sec = opts.timeout_sec;
  tv.tv_usec = 0;
  if (::setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
    ase_throw_exception("SocketRcvTimeoFailed", ase_string());
  }
  int v = opts.sndbuf;
  if (v > 0 && ::setsockopt(fd, SOL_SOCKET, SO_SNDBUF,
                 reinterpret_cast<const char *>(&v), sizeof(v)) < 0) {
    ase_throw_exception("SocketSetOptSndBuf", ase_string());
  }
  v = opts.rcvbuf;
  if (v > 0 && ::setsockopt(fd, SOL_SOCKET, SO_RCVBUF,
                 reinterpret_cast<const char *>(&v), sizeof(v)) < 0) {
    ase_throw_exception("SocketSetOptRcvBuf", ase_string());
  }
}

};

void
ase_io_listener_socket::accept(ase_auto_ptr<ase_io_stream>& stream_r,
  ase_sockaddr_generic& addr_r)
{
  if (!is_open()) {
    throw_exception("ListenerNotListening");
  }
  ase_auto_ptr<ase_io_stream_socket> s(new ase_io_stream_socket());
  const int x = ::accept(fd, addr_r.get_sockaddr(), &addr_r.get_socklen());
  if (x < 0) {
    ase_throw_exception("ListnerAcceptFailed", ase_string());
  }
  s->reset(x);
  set_socket_options(x, opts);
  stream_r.reset(s.release());
}

void
ase_io_listener_socket_create(ase_auto_ptr<ase_io_listener>& ptr_r,
  const ase_io_socket_options& opts)
{
  ptr_r.reset(new ase_io_listener_socket(opts));
}

void
ase_io_socket_create(ase_auto_ptr<ase_io_stream>& ptr_r,
  const ase_sockaddr_generic& addr, const ase_io_socket_options& opts)
{
  ase_auto_ptr<ase_io_stream_socket> s(new ase_io_stream_socket());
  const int x = ::socket(addr.get_sockaddr()->sa_family, SOCK_STREAM, 0);
  if (x < 0) {
    ase_throw_exception("SocketFailed", ase_string());
  }
  s->reset(x);
  if (::connect(x, addr.get_sockaddr(), addr.get_socklen()) < 0) {
    ase_throw_exception("SocketConnectFailed", ase_string());
  }
  set_socket_options(x, opts);
  ptr_r.reset(s.release());
}


