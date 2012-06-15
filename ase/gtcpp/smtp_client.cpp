
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/string_util.hpp>
#include <gtcpp/smtp_common.hpp>
#include <gtcpp/smtp_client.hpp>
#include <gtcpp/socket.hpp>

#define DBG_SMTP(x)
#define DBG_BUFFER(x)

namespace gtcpp {

namespace {

bool
is_not_digit(char c)
{
  return (c < '0' || c > '9');
}

template <typename T1> int
smtp_command_1(buffered_file& conn, const T1& s1, string_ref& resp_r,
  posix_error_callback& ec)
{
  string_ref req(s1);
  return smtp_client::detail::command(conn, &req, 1, resp_r, ec);
}

template <typename T1, typename T2, typename T3> int
smtp_command_3(buffered_file& conn, const T1& s1, const T2& s2, const T3& s3,
  string_ref& resp_r, posix_error_callback& ec)
{
  string_ref req[3];
  req[0] = s1;
  req[1] = s2;
  req[2] = s3;
  return smtp_client::detail::command(conn, req, 3, resp_r, ec);
}

};

int
smtp_client::detail::write_request(const auto_file& sd, const string_ref *vec,
  int cnt, posix_error_callback& ec)
{
  int r;
  assert(cnt);
  if ((r = writev(sd, vec, cnt, ec)) < cnt) {
    return r >= 0 ? 0 : r; /* <= 0 (unexpected eof or error) */
  }
  return r; /* positive */
}

int
smtp_client::detail::response_code(const char *str, size_t len)
{
  if (len < 3) {
    return 0;
  }
  if (is_not_digit(str[0]) || is_not_digit(str[1]) || is_not_digit(str[2])) {
    return 0;
  }
  return (str[0] - '0') * 100 + (str[1] - '0') * 10 + (str[2] - '0');
}

int
smtp_client::detail::read_response(buffered_file& conn,
  string_ref& resp_r, posix_error_callback& ec)
{
  ssize_t len = 0;
  char *line = 0;
  while (true) {
    len = conn.read_line_crlf(line, ec);
    if (len <= 0) {
      resp_r.reset();
      break;
    }
    DBG_SMTP(std::cout << "R: " << std::string(line, len));
    if (len <= 5 || line[3] == ' ') {
      resp_r.set(line, len);
      return response_code(line, len);
    }
  }
  return len; /* <= 0 (unexpected eof or error) */
}

int
smtp_client::detail::command(buffered_file& conn, const string_ref *vec,
  int cnt, string_ref& resp_r, posix_error_callback& ec)
{
  conn.reset_cursor();
  int r;
  if ((r = write_request(conn.get_file(), vec, cnt, ec)) <= 0) {
    return r; /* unexpected eof or error */
  }
  r = read_response(conn, resp_r, ec);
  return r;
}

struct smtp_client::impl_type {
  impl_type() {
    DBG_BUFFER(conn.set_read_block_size(1));
  }
  buffered_file conn;
  string_buffer work;
  string_ref response;
};

smtp_client::smtp_client()
  : impl(new impl_type())
{
}

smtp_client::~smtp_client()
{
}

void
smtp_client::reset()
{
  impl->conn.reset();
  impl->work.reset(4096);
  impl->response.reset();
}

auto_file&
smtp_client::get_file()
{
  return impl->conn.get_file();
}

buffered_file&
smtp_client::get_buffered_file()
{
  return impl->conn;
}

string_ref
smtp_client::get_response() const
{
  return impl->response;
}

string_ref
smtp_client::get_buffer() const
{
  const string_buffer& bufref = impl->conn.get_buffer();
  return string_ref(bufref.begin(), bufref.end());
}

int
smtp_client::greeting(posix_error_callback& ec)
{
  assert(impl->conn.get_buffer().size() == 0);
  return detail::read_response(impl->conn, impl->response, ec);
}

int
smtp_client::helo(const string_ref& domain, posix_error_callback& ec)
{
  return smtp_command_3(impl->conn, "HELO ", domain, "\r\n", impl->response,
    ec);
}

int
smtp_client::ehlo(const string_ref& domain, posix_error_callback& ec)
{
  return smtp_command_3(impl->conn, "EHLO ", domain, "\r\n", impl->response,
    ec);
}

int
smtp_client::mail(const string_ref& from, posix_error_callback& ec)
{
  return smtp_command_3(impl->conn, "MAIL FROM:<", from, ">\r\n",
    impl->response, ec);
}

int
smtp_client::rcpt(const string_ref& to, posix_error_callback& ec)
{
  return smtp_command_3(impl->conn, "RCPT TO:<", to, ">\r\n",
    impl->response, ec);
}

int
smtp_client::data(posix_error_callback& ec)
{
  int r = smtp_command_1(impl->conn, "DATA\r\n", impl->response, ec);
  if (is_2xx(r)) {
    r = 0;
  }
  return r; /* never returns 2xx */
}

int
smtp_client::body(const string_ref& bdy, posix_error_callback& ec)
{
  impl_type& ir = *impl;
  string_buffer& work = ir.work;
  smtp_escape_body(work, bdy);
  string_ref req = string_ref(work.begin(), work.end());
  int r = smtp_command_1(ir.conn, req, ir.response, ec);
  work.reset(4000);
  return r;
}

int
smtp_client::body_raw(const string_ref *rawbody_begin,
  const string_ref *rawbody_end, posix_error_callback& ec)
{
  return detail::command(impl->conn, rawbody_begin,
    rawbody_end - rawbody_begin, impl->response, ec);
}

int
smtp_client::rset(posix_error_callback& ec)
{
  return smtp_command_1(impl->conn, "RSET\r\n", impl->response, ec);
}

int
smtp_client::noop(posix_error_callback& ec)
{
  return smtp_command_1(impl->conn, "NOOP\r\n", impl->response, ec);
}

int
smtp_client::quit(posix_error_callback& ec, bool passive_close)
{
  impl_type& ir = *impl;
  int r = 0;
  string_ref req("QUIT\r\n");
  if (!is_2xx(r = detail::command(ir.conn, &req, 1, ir.response, ec))) {
    return r;
  }
  if (passive_close) {
    return close_gentle(ir.conn.get_file(), ec);
  }
  ir.conn.reset();
  return r;
}

int
smtp_client::greeting_helo(const string_ref& domain, posix_error_callback& ec)
{
  /* returns 2xx on success */
  int r = 0;
  if (!is_2xx(r = greeting(ec))) {
    return r;
  }
  return helo(domain, ec);
}

int
smtp_client::mail_transaction(const string_ref& mailfrom,
  const string_ref& rcptto, const string_ref& bdy, posix_error_callback& ec)
{
  /* returns 2xx on success */
  int r = 0;
  if (!is_2xx(r = mail(mailfrom, ec))) {
    return r;
  }
  if (!is_2xx(r = rcpt(rcptto, ec))) {
    return r;
  }
  if (!is_3xx(r = data(ec))) {
    return r; /* asserted that r != 2xx */
  }
  return body(bdy, ec);
}

};

