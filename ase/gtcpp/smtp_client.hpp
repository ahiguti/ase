
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_SMTP_CLIENT_HPP
#define GTCPP_SMTP_CLIENT_HPP

#include <gtcpp/auto_file.hpp>
#include <gtcpp/string_ref.hpp>
#include <gtcpp/buffered_file.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

namespace gtcpp {

class smtp_client : private boost::noncopyable {

 public:

  smtp_client();
  ~smtp_client();
  void reset();

  auto_file& get_file();
  buffered_file& get_buffered_file();
  string_ref get_buffer() const;
  string_ref get_response() const;

  int greeting(posix_error_callback& ec);
  int helo(const string_ref& domain, posix_error_callback& ec);
  int ehlo(const string_ref& domain, posix_error_callback& ec);
  int mail(const string_ref& mailfrom, posix_error_callback& ec);
  int rcpt(const string_ref& rcptto, posix_error_callback& ec);
  int data(posix_error_callback& ec);
  int body(const string_ref& bdy, posix_error_callback& ec);
  int body_raw(const string_ref *rawbody_begin, const string_ref *rawbody_end,
    posix_error_callback& ec);
  int rset(posix_error_callback& ec);
  int noop(posix_error_callback& ec);
  int quit(posix_error_callback& ec, bool passive_close);
  int greeting_helo(const string_ref& domain, posix_error_callback& ec);
  int mail_transaction(const string_ref& mailfrom, const string_ref& rcptto,
    const string_ref& bdy, posix_error_callback& ec);

  struct detail {
    static int response_code(const char *str, size_t len);
    static int read_response(buffered_file& conn, string_ref& resp_r,
      posix_error_callback& ec);
    static int write_request(const auto_file& sd, const string_ref *vec,
      int cnt, posix_error_callback& ec);
    static int command(buffered_file& conn, const string_ref *vec, int cnt,
      string_ref& resp_r, posix_error_callback& ec);
  };
  static bool is_2xx(int code) { return code >= 200 && code < 300; }
  static bool is_3xx(int code) { return code >= 300 && code < 400; }
  static bool is_4xx(int code) { return code >= 400 && code < 500; }
  static bool is_5xx(int code) { return code >= 500 && code < 600; }

 private:

  struct impl_type;
  const boost::scoped_ptr<impl_type> impl;

};

};

#endif

