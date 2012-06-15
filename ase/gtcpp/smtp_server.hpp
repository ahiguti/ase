
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_SMTP_SERVER_HPP
#define GTCPP_SMTP_SERVER_HPP

#include <gtcpp/auto_file.hpp>
#include <gtcpp/string_ref.hpp>
#include <gtcpp/buffered_file.hpp>
#include <gtcpp/tcp_server.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

namespace gtcpp {

class smtp_server;
class smtp_server_worker;
struct smtp_server_callback_factory;
struct smtp_server_callback;

class smtp_server : public tcp_server {

 public:

  struct options {
    size_t buffer_size_limit;
    int send_response_timeout;
    int wait_request_timeout;
    int wait_body_timeout;
    options() : buffer_size_limit(100 * 1024 * 1024),
      send_response_timeout(300), wait_request_timeout(300),
      wait_body_timeout(600) { }
  };

  smtp_server(smtp_server_callback_factory& scf, const options& opt);
  virtual ~smtp_server();
  virtual void worker_process_initialized();
  virtual void create_worker(std::auto_ptr<tcp_server_worker>& wkr_r);

 private:

  struct impl_type;
  const boost::scoped_ptr<impl_type> impl;

};

class smtp_server_worker : public tcp_server_worker {

 public:

  smtp_server_worker(std::auto_ptr<smtp_server_callback>& sc,
    const smtp_server::options& opt);
  virtual ~smtp_server_worker();
  virtual int run(auto_file& conn, const sockaddr_generic& client_addr,
    posix_error_callback& ec);

  auto_file& get_file() const;
    /* returned reference is valid until 'this' is destroyed */
  const string_buffer& get_buffer() const;
    /* returned reference is valid until 'this' is destroyed */
  const sockaddr_generic& get_client_address() const;

 private:

  struct impl_type;
  const boost::scoped_ptr<impl_type> impl;

};

struct smtp_server_callback_factory {

  virtual ~smtp_server_callback_factory() { }
  virtual void create_server_callback(
    std::auto_ptr<smtp_server_callback>& sc_r) = 0;
  virtual bool on_timer(time_t now) = 0;

};

struct smtp_server_callback {

  virtual ~smtp_server_callback() { }
  /* it is asserted that for each time on_open_connection is called,
     corresponding on_close_connection is called even if an exception
     is thrown. the 'ec' argument is valid until on_close_connection is
     returned. */
  virtual int on_open_connection(const smtp_server_worker& svr,
    posix_error_callback& ec) = 0; /* should return 0 on success */
  virtual int on_close_connection(const smtp_server_worker& svr) throw ()
    = 0; /* should return 0 on success */
  /* the 'domain' argument is valid until on_ehlo is returned. */
  virtual int on_ehlo(const smtp_server_worker& svr,
    const string_buffer_range& domain) = 0;
  /* 'from', 'to', and 'body' arguments are valid until the current mail
    transaction is committed or cancelled, i.e., valid until on_ehlo,
    on_body, on_rset, or on_quit is returned. */
  virtual int on_mail(const smtp_server_worker& svr,
    const string_buffer_range& from) = 0;
  virtual int on_rcpt(const smtp_server_worker& svr,
    const string_buffer_range& to) = 0;
  virtual int on_data(const smtp_server_worker& svr) = 0;
  virtual int on_body(const smtp_server_worker& svr,
    const string_buffer_range& body_raw, string_buffer& resp) = 0;
  virtual int on_rset(const smtp_server_worker& svr) = 0;
  virtual int on_noop(const smtp_server_worker& svr) = 0;
  virtual int on_quit(const smtp_server_worker& svr) = 0;

};

struct smtp_server_callback_empty : public smtp_server_callback {

  virtual int on_open_connection(const smtp_server_worker& svr,
    posix_error_callback& ec);
  virtual int on_close_connection(const smtp_server_worker& svr) throw ();
  virtual int on_ehlo(const smtp_server_worker& svr,
    const string_buffer_range& domain);
  virtual int on_mail(const smtp_server_worker& svr,
    const string_buffer_range& from);
  virtual int on_rcpt(const smtp_server_worker& svr,
    const string_buffer_range& to);
  virtual int on_data(const smtp_server_worker& svr);
  virtual int on_body(const smtp_server_worker& svr,
    const string_buffer_range& body_raw, string_buffer& resp);
  virtual int on_rset(const smtp_server_worker& svr);
  virtual int on_noop(const smtp_server_worker& svr);
  virtual int on_quit(const smtp_server_worker& svr);

};

struct smtp_server_callback_verbose : public smtp_server_callback {

  virtual int on_open_connection(const smtp_server_worker& svr,
    posix_error_callback& ec);
  virtual int on_close_connection(const smtp_server_worker& svr) throw ();
  virtual int on_ehlo(const smtp_server_worker& svr,
    const string_buffer_range& domain);
  virtual int on_mail(const smtp_server_worker& svr,
    const string_buffer_range& from);
  virtual int on_rcpt(const smtp_server_worker& svr,
    const string_buffer_range& to);
  virtual int on_data(const smtp_server_worker& svr);
  virtual int on_body(const smtp_server_worker& svr,
    const string_buffer_range& body_raw, string_buffer& resp);
  virtual int on_rset(const smtp_server_worker& svr);
  virtual int on_noop(const smtp_server_worker& svr);
  virtual int on_quit(const smtp_server_worker& svr);

};

};

#endif

