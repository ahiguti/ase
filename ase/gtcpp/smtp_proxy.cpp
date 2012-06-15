
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/smtp_proxy.hpp>
#include <gtcpp/smtp_common.hpp>
#include <gtcpp/tproxy.hpp>

#define DBG_TPR(x)

namespace gtcpp {

struct smtp_proxy::impl_type : public smtp_proxy::args {

  impl_type(const args& a, smtp_filter_factory& ff)
    : args(a), filter_factory(ff) { }
  sockaddr_generic backend_addr;
  sockaddr_generic tproxy_local_addr;
  smtp_filter_factory& filter_factory;

};

class smtp_proxy_worker : public smtp_server_callback {

 public:

  smtp_proxy_worker(smtp_proxy& backref);
  void reset();
  int on_open_connection(const smtp_server_worker& svr,
    posix_error_callback& ec);
  int on_close_connection(const smtp_server_worker& svr) throw ();
  virtual int on_ehlo(const smtp_server_worker& svr,
    const string_buffer_range& domain);
  virtual int on_mail(const smtp_server_worker& svr,
    const string_buffer_range& from);
  virtual int on_rcpt(const smtp_server_worker& svr,
    const string_buffer_range& to);
  virtual int on_data(const smtp_server_worker& svr);
  virtual int on_body(const smtp_server_worker& svr,
    const string_buffer_range& body, string_buffer&);
  virtual int on_rset(const smtp_server_worker& svr);
  virtual int on_noop(const smtp_server_worker& svr);
  virtual int on_quit(const smtp_server_worker& svr);

 private:

  int proxy_retcode(int code);

 private:

  smtp_proxy& proxy_backref;
  std::auto_ptr<smtp_filter> filter;
  sockaddr_generic client_address;
  smtp_client backend_conn;
  posix_error_callback *pec;

};

smtp_proxy::smtp_proxy(const args& a, smtp_filter_factory& filter_factory)
  : impl(new impl_type(a, filter_factory))
{
  addrinfo_error_callback aec;
  resolve_address(impl->backend_address.c_str(), 0, SOCK_STREAM, 0,
    impl->backend_addr, aec);
  if (!impl->tproxy_local_address.empty()) {
    resolve_address(impl->tproxy_local_address.c_str(), 0, SOCK_STREAM, 0,
      impl->tproxy_local_addr, aec);
  }
}

smtp_proxy::~smtp_proxy()
{
}

void
smtp_proxy::create_server_callback(std::auto_ptr<smtp_server_callback>& sc_r)
{
  sc_r.reset(new smtp_proxy_worker(*this));
}

bool
smtp_proxy::on_timer(time_t)
{
  return true;
}

const sockaddr_generic&
smtp_proxy::get_backend_address() const
{
  return impl->backend_addr;
}

const sockaddr_generic&
smtp_proxy::get_tproxy_local_address() const
{
  return impl->tproxy_local_addr;
}

const std::string&
smtp_proxy::get_ehlo_domain() const
{
  return impl->ehlo_domain;
}

smtp_proxy_worker::smtp_proxy_worker(smtp_proxy& backref)
  : proxy_backref(backref)
{
  reset();
  backref.impl->filter_factory.create_filter(filter);
}

void
smtp_proxy_worker::reset()
{
  backend_conn.reset();
  pec = 0;
}

int
smtp_proxy_worker::on_open_connection(const smtp_server_worker& wkr,
  posix_error_callback& ec)
{
  client_address = wkr.get_client_address();
  pec = &ec;
  int r = 0;
  auto_file& fd = backend_conn.get_file();
  const sockaddr_generic& sa = proxy_backref.get_backend_address();
  if ((r = socket(fd, sa.addr()->sa_family, SOCK_STREAM, 0, ec)) < 0) {
    return r;
  }
  const int v = 1;
  if ((r = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &v, sizeof(v), ec)) < 0) {
    return r;
  }
  const int timeout_sec = proxy_backref.impl->socket_timeout;
  struct timeval tv;
  tv.tv_sec = timeout_sec;
  tv.tv_usec = 0;
  if ((r = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv), ec)) < 0) {
    return r;
  }
  tv.tv_sec = timeout_sec;
  tv.tv_usec = 0;
  if ((r = setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv), ec)) < 0) {
    return r;
  }
  const sockaddr_generic& tpr_local_addr =
    proxy_backref.get_tproxy_local_address();
  if (client_address.addr()->sa_family == AF_INET &&
    tpr_local_addr.addr()->sa_family == AF_INET) {
    DBG_TPR(printf("TPROXY\n"));
    const ::sockaddr_in *const src = reinterpret_cast<const ::sockaddr_in *>(
      client_address.addr());
    const ::sockaddr_in *const tl = reinterpret_cast<const ::sockaddr_in *>(
      tpr_local_addr.addr());
    const u_int32_t src_addr = ntohl(src->sin_addr.s_addr);
    const u_int32_t tpr_local_addr = ntohl(tl->sin_addr.s_addr);
    if ((r = socket_inet_tproxy(fd.get(), src_addr, tpr_local_addr, ec)) < 0) {
      return r;
    }
    ::sockaddr_in dst;
    std::memset(&dst, 0, sizeof(dst));
    if ((r = socket_inet_get_real_destination(wkr.get_file().get(), dst, ec))
      < 0) {
      return r;
    }
    DBG_TPR(printf("REAL: %x\n", ntohl(dst.sin_addr.s_addr)));
    sockaddr_generic dst_generic(reinterpret_cast<const ::sockaddr *>(&dst),
      sizeof(dst));
    if ((r = connect(fd, dst_generic, ec)) < 0) {
      return r;
    }
  } else {
    if ((r = connect(fd, sa, ec)) < 0) {
      return r;
    }
  }
  if (!smtp_client::is_2xx(r = backend_conn.greeting(ec))) {
    return std::min(r, 0);
  }
  return r;
}

int
smtp_proxy_worker::on_close_connection(const smtp_server_worker&) throw ()
{
  reset();
  return 0;
}

int
smtp_proxy_worker::on_ehlo(const smtp_server_worker&,
  const string_buffer_range&)
{
  assert(pec);
  string_ref domain(proxy_backref.get_ehlo_domain());
  int r = backend_conn.ehlo(domain, *pec);
  if (smtp_client::is_5xx(r)) {
    r = backend_conn.helo(domain, *pec);
  }
  return r;
}

int
smtp_proxy_worker::on_mail(const smtp_server_worker& wkr,
  const string_buffer_range& from)
{
  assert(pec);
  return backend_conn.mail(string_ref(wkr.get_buffer(), from), *pec);
}

int
smtp_proxy_worker::on_rcpt(const smtp_server_worker& wkr,
  const string_buffer_range& to)
{
  assert(pec);
  return backend_conn.rcpt(string_ref(wkr.get_buffer(), to), *pec);
}

int
smtp_proxy_worker::on_data(const smtp_server_worker&)
{
  assert(pec);
  return backend_conn.data(*pec);
}

int
smtp_proxy_worker::on_body(const smtp_server_worker& wkr,
  const string_buffer_range& body_raw, string_buffer&)
{
  assert(pec);
  string_ref rawbdy(wkr.get_buffer(), body_raw);
  if (filter.get()) {
    string_buffer body, body_filtered, body_filtered_raw;
    smtp_unescape_body(body, rawbdy);
    int r = filter->filter(body_filtered, string_ref(body));
    body.reset();
    if (!smtp_client::is_2xx(r)) {
      return r;
    }
    smtp_escape_body(body_filtered_raw, string_ref(body_filtered));
    string_ref rawbdy_filtered(body_filtered_raw);
    return backend_conn.body_raw(&rawbdy_filtered, &rawbdy_filtered + 1, *pec);
  } else {
    return backend_conn.body_raw(&rawbdy, &rawbdy + 1, *pec);
  }
}

int
smtp_proxy_worker::on_rset(const smtp_server_worker&)
{
  assert(pec);
  return backend_conn.rset(*pec);
}

int
smtp_proxy_worker::on_noop(const smtp_server_worker&)
{
  assert(pec);
  return backend_conn.noop(*pec);
}

int
smtp_proxy_worker::on_quit(const smtp_server_worker&)
{
  assert(pec);
  const int r = backend_conn.quit(*pec, true);
  return r < 0 ? r : 221;
}

namespace {

class smtp_filter_nochange : public smtp_filter {

  virtual int filter(string_buffer& obuf, const string_ref& message);

};

int
smtp_filter_nochange::filter(string_buffer& obuf, const string_ref& message)
{
  obuf.append(message.begin(), message.size());
  return 250;
}

};

void
smtp_filter_factory_nochange::create_filter(
  std::auto_ptr<smtp_filter>& instance_r)
{
  instance_r.reset(new smtp_filter_nochange());
}

};

