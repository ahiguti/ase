
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_SMTP_PROXY_HPP
#define GTCPP_SMTP_PROXY_HPP

#include <gtcpp/smtp_server.hpp>
#include <gtcpp/smtp_client.hpp>

namespace gtcpp {

struct smtp_filter;
struct smtp_filter_factory;

struct smtp_filter {

  virtual ~smtp_filter() { }
  virtual int filter(string_buffer& obuf, const string_ref& message) = 0;

};

struct smtp_filter_factory {

  virtual ~smtp_filter_factory() { }
  virtual void create_filter(std::auto_ptr<smtp_filter>& instance_r) = 0;

};

class smtp_proxy : public smtp_server_callback_factory {

 public:

  struct args {
    std::string ehlo_domain;
    std::string backend_address;
    std::string tproxy_local_address;
    int socket_timeout; /* sec */
    args() : socket_timeout(600) { }
  };

  smtp_proxy(const smtp_proxy::args& a, smtp_filter_factory& ffactory);
  ~smtp_proxy();
  virtual void create_server_callback(
    std::auto_ptr<smtp_server_callback>& sc_r);
  bool on_timer(time_t now);
  const sockaddr_generic& get_backend_address() const;
  const sockaddr_generic& get_tproxy_local_address() const;
  const std::string& get_ehlo_domain() const;

 private:

  struct impl_type;
  boost::scoped_ptr<impl_type> impl;
  friend class smtp_proxy_worker;

};

class smtp_filter_factory_nochange : public smtp_filter_factory {

  virtual void create_filter(std::auto_ptr<smtp_filter>& instance_r);

};

};

#endif

