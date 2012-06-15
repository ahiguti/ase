
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/string_util.hpp>
#include <gtcpp/smtp_server.hpp>
#include <gtcpp/socket.hpp>
#include <gtcpp/file_util.hpp>
#include <boost/static_assert.hpp>
#include <iostream>

#define DBG_SMTP(x)
#define DBG_MEM(x)
#define DBG_CLI(x)

namespace gtcpp {

namespace {

template <size_t n> int
smtp_send_string(const buffered_file& bf, const char (& s)[n],
  posix_error_callback& ec)
{
  return write(bf.get_file(), s, n - 1, ec);
}

int
smtp_send_response(const buffered_file& bf, int code,
  const string_buffer& resp, posix_error_callback& ec)
{
  if (resp.empty()) {
    if (code == 250) {
      return smtp_send_string(bf, "250 Ok\r\n", ec);
    } else if (code == 354) {
      return smtp_send_string(bf, "354 Go ahead\r\n", ec);
    } else if (code == 220) {
      return smtp_send_string(bf,
	"220 ASE ESMTP Service ready\r\n", ec);
    } else if (code == 221) {
      return smtp_send_string(bf, "221 Bye\r\n", ec);
    }
    char buf[5];
    buf[2] = '0' + (code % 10);
    code /= 10;
    buf[1] = '0' + (code % 10);
    code /= 10;
    buf[0] = '0' + (code % 10);
    buf[3] = '\r';
    buf[4] = '\n';
    return write(bf.get_file(), buf, 5, ec);
  } else {
    char buf[4];
    buf[2] = '0' + (code % 10);
    code /= 10;
    buf[1] = '0' + (code % 10);
    code /= 10;
    buf[0] = '0' + (code % 10);
    buf[3] = ' ';
    ::iovec vec[3];
    vec[0].iov_base = buf;
    vec[0].iov_len = 4;
    vec[1].iov_base = const_cast<char *>(resp.begin());
    vec[1].iov_len = resp.size();
    vec[2].iov_base = const_cast<char *>("\r\n");
    vec[2].iov_len = 2;
    return writev(bf.get_file(), vec, 3, ec);
  }
}

int
smtp_read_request(buffered_file& conn, char *& line_r,
  posix_error_callback& ec)
{
  return conn.read_line_crlf(line_r, ec);
}

int
smtp_read_body(buffered_file& conn, posix_error_callback& ec)
{
  char *line = 0;
  while (true) {
    int r = conn.read_line_crlf(line, ec);
    if (r <= 0) {
      return r;
    }
    assert(r >= 2);
    assert(line[r - 2] == '\r');
    assert(line[r - 1] == '\n');
    if (r == 3 && line[0] == '.') {
      return 1;
    }
  }
  return 1;
}

unsigned int
smtp_req_to_uint(const char *req)
{
  const unsigned int *const ureq =
    reinterpret_cast<const unsigned int *>(req);
  return *ureq;
}

template <size_t n> bool
smtp_match_command(const char *line, const char (& cmd)[n])
{
  BOOST_STATIC_ASSERT(n > 1);
  return strncasecmp(line, cmd, n - 1) == 0;
}

bool
is_code_2xx(int x)
{
  return x >= 200 && x < 300;
}

bool
is_code_3xx(int x)
{
  return x >= 300 && x < 400;
}

string_buffer_range
calc_range(const buffered_file& conn, const char *begin, const char *end)
{
  const char *const b = conn.get_buffer().begin();
  return string_buffer_range(begin - b, end - b);
}

string_buffer_range
find_mailaddr(const buffered_file& conn, const char *const begin,
  const char *const end)
{
  const char *const p = find_char(begin, end, '<');
  if (p == end) {
    return string_buffer_range();
  }
  const char *const addr_begin = p + 1; /* next to '<' */
  const char *addr_end = find_char(addr_begin, end, ' ');
  while (true) {
    if (addr_end == addr_begin) {
      return string_buffer_range();
    }
    --addr_end;
    if (*addr_end == '>') {
      break;
    }
  }
  return calc_range(conn, addr_begin, addr_end);
}

string_buffer_range
find_domainname(const buffered_file& conn, const char *const begin,
  const char *const end)
{
  const char *p = begin;
  for (p = begin; p != end; ++p) {
    if (*p != ' ') {
      break;
    }
  }
  const char *q = end;
  while (q != p && (q[-1] == '\r' || q[-1] == '\n')) {
    --q;
  }
  return calc_range(conn, p, q);
}

};

struct smtp_server::impl_type {

  impl_type(smtp_server_callback_factory& scf, const smtp_server::options& o)
    : scfactory(scf), opt(o) { }

  smtp_server_callback_factory& scfactory;
  smtp_server::options opt;

};

smtp_server::smtp_server(smtp_server_callback_factory& scf, const options& opt)
  : impl(new impl_type(scf, opt))
{
}

smtp_server::~smtp_server()
{
}

void
smtp_server::worker_process_initialized()
{
}

void
smtp_server::create_worker(std::auto_ptr<tcp_server_worker>& wkr_r)
{
  std::auto_ptr<smtp_server_callback> sc;
  impl->scfactory.create_server_callback(sc);
  std::auto_ptr<smtp_server_worker> wkr(new smtp_server_worker(sc, impl->opt));
  wkr_r = wkr;
}

struct smtp_server_worker::impl_type : private boost::noncopyable {

  typedef size_t size_type;

  impl_type(const smtp_server::options& o) : opt(o) {
  }
  void reset_mail_transaction() {
    has_mailfrom = false;
    num_rcpts = 0;
    conn.reset_cursor();
    response_msg.clear();
  }

  struct close_hook;

  std::auto_ptr<smtp_server_callback> callback;
  buffered_file conn;
  string_buffer response_msg;
  bool has_mailfrom;
  size_type num_rcpts;
  sockaddr_generic client_addr;
  smtp_server::options opt;

};

struct smtp_server_worker::impl_type::close_hook
  : private boost::noncopyable {
  close_hook(smtp_server_worker& wkr, auto_file& fd, posix_error_callback& ec)
    : worker(wkr), err(0) {
    worker.impl->conn.get_file().reset(fd);
    err = worker.impl->callback->on_open_connection(worker, ec);
  }
  ~close_hook() {
    worker.impl->callback->on_close_connection(worker); /* ignore retval */
    worker.impl->conn.reset();
  }
  smtp_server_worker& worker;
  int err;
};

smtp_server_worker::smtp_server_worker(
  std::auto_ptr<smtp_server_callback>& sc, const smtp_server::options& o)
  : impl(new impl_type(o))
{
  impl->callback = sc;
}

smtp_server_worker::~smtp_server_worker()
{
}

auto_file&
smtp_server_worker::get_file() const
{
  return impl->conn.get_file();
}

const string_buffer&
smtp_server_worker::get_buffer() const
{
  return impl->conn.get_buffer();
}

const sockaddr_generic&
smtp_server_worker::get_client_address() const
{
  return impl->client_addr;
}

int
smtp_server_worker::run(auto_file& fd, const sockaddr_generic& client_addr,
  posix_error_callback& ec)
{
  int r = 0;
  int code = 0;
  impl_type& ir = *impl;
  ir.client_addr = client_addr;
  smtp_server_callback& sc = *ir.callback;
  ir.reset_mail_transaction();
  impl_type::close_hook chook(*this, fd, ec);
  if (chook.err < 0) {
    return chook.err;
  }
  DBG_CLI(std::cerr << "client: " << client_addr.to_string() << std::endl);
  if ((r = smtp_send_string(ir.conn, "220 ASE ESMTP service ready\r\n",
    ec)) <= 0) {
    return r;
  }
  while (true) {
    char *line = 0;
    bool reset_if_success = false;
    bool close_after_resp = false;
    /* TODO: command-wise timeout is not implemented yet */
    if ((r = smtp_read_request(ir.conn, line, ec)) <= 0) {
      break;
    }
    if (r < 6 || (line[4] != '\r' && line[4] != ' ')) {
      code = 500;
    } else if (smtp_match_command(line, "RCPT")) {
      code = sc.on_rcpt(*this, find_mailaddr(ir.conn, line + 4, line + r));
    } else if (smtp_match_command(line, "MAIL")) {
      code = sc.on_mail(*this, find_mailaddr(ir.conn, line + 4, line + r));
    } else if (smtp_match_command(line, "DATA")) {
      code = sc.on_data(*this);
      if (is_code_3xx(code)) {
	if ((r = smtp_send_response(ir.conn, code, ir.response_msg, ec)) <= 0) {
	  break;
	}
	const size_t body_begin_offset = ir.conn.get_cursor_offset();
	if ((r = smtp_read_body(ir.conn, ec)) <= 0) {
	  break;
	}
	const size_t body_end_offset = ir.conn.get_cursor_offset();
	ir.response_msg.clear();
	code = sc.on_body(*this,
	  string_buffer_range(body_begin_offset, body_end_offset),
	  ir.response_msg);
	reset_if_success = true;
      }
      DBG_MEM(fprintf(stderr, "bs=%d\n",
	static_cast<int>(ir.conn.get_buffer().alloc_size())));
    } else if (smtp_match_command(line, "EHLO")) {
      code = sc.on_ehlo(*this, find_domainname(ir.conn, line + 4, line + r));
      if (code == 250) {
	if ((r = smtp_send_string(ir.conn, "250-Ok\r\n250 8BITMIME\r\n", ec))
	  <= 0) {
	  break;
	}
	continue;
      }
      reset_if_success = true;
    } else if (smtp_match_command(line, "HELO")) {
      code = sc.on_ehlo(*this, find_domainname(ir.conn, line + 4, line + r));
      reset_if_success = true;
    } else if (smtp_match_command(line, "RSET")) {
      code = sc.on_rset(*this);
      reset_if_success = true;
    } else if (smtp_match_command(line, "QUIT")) {
      code = sc.on_quit(*this);
      close_after_resp = true;
    } else {
      code = 500;
    }
    if (code <= 0) {
      break;
    }
    if ((r = smtp_send_response(ir.conn, code, ir.response_msg, ec)) <= 0) {
      break;
    }
    if (reset_if_success && is_code_2xx(code)) {
      ir.reset_mail_transaction();
    }
    if (close_after_resp) {
      break;
    }
  }
  return r;
}

int
smtp_server_callback_empty::on_open_connection(const smtp_server_worker&,
  posix_error_callback&)
{
  return 0;
}

int
smtp_server_callback_empty::on_close_connection(const smtp_server_worker&)
  throw ()
{
  return 0;
}

int
smtp_server_callback_empty::on_ehlo(const smtp_server_worker&,
  const string_buffer_range&)
{
  return 250;
}

int
smtp_server_callback_empty::on_mail(const smtp_server_worker&,
  const string_buffer_range&)
{
  return 250;
}

int
smtp_server_callback_empty::on_rcpt(const smtp_server_worker&,
  const string_buffer_range&)
{
  return 250;
}

int
smtp_server_callback_empty::on_data(const smtp_server_worker&)
{
  return 354;
}

int
smtp_server_callback_empty::on_body(const smtp_server_worker&,
  const string_buffer_range&, string_buffer&)
{
  return 250;
}

int
smtp_server_callback_empty::on_rset(const smtp_server_worker&)
{
  return 250;
}

int
smtp_server_callback_empty::on_noop(const smtp_server_worker&)
{
  return 250;
}

int
smtp_server_callback_empty::on_quit(const smtp_server_worker&)
{
  return 221;
}

int
smtp_server_callback_verbose::on_open_connection(const smtp_server_worker&,
  posix_error_callback&)
{
  std::cout << "SMTP open" << std::endl;
  return 0;
}

int
smtp_server_callback_verbose::on_close_connection(const smtp_server_worker&)
  throw ()
{
  std::cout << "SMTP close" << std::endl;
  return 0;
}

int
smtp_server_callback_verbose::on_ehlo(const smtp_server_worker& svr,
  const string_buffer_range& domain)
{
  const string_buffer& buf = svr.get_buffer();
  std::string s(buf.begin() + domain.begin, buf.begin() + domain.end);
  std::cout << "EHLO " << s << std::endl;
  return 250;
}

int
smtp_server_callback_verbose::on_mail(const smtp_server_worker& svr,
  const string_buffer_range& from)
{
  const string_buffer& buf = svr.get_buffer();
  std::string s(buf.begin() + from.begin, buf.begin() + from.end);
  std::cout << "MAIL " << s << std::endl;
  return 250;
}

int
smtp_server_callback_verbose::on_rcpt(const smtp_server_worker& svr,
  const string_buffer_range& to)
{
  const string_buffer& buf = svr.get_buffer();
  std::string s(buf.begin() + to.begin, buf.begin() + to.end);
  std::cout << "RCPT " << s << std::endl;
  return 250;
}

int
smtp_server_callback_verbose::on_data(const smtp_server_worker&)
{
  std::cout << "DATA" << std::endl;
  return 354;
}

int
smtp_server_callback_verbose::on_body(const smtp_server_worker& svr,
  const string_buffer_range& body_raw, string_buffer&)
{
  const string_buffer& buf = svr.get_buffer();
  std::string s(buf.begin() + body_raw.begin, buf.begin() + body_raw.end);
  std::cout << "BODY [" << s << "]" << std::endl;
  return 250;
}

int
smtp_server_callback_verbose::on_rset(const smtp_server_worker&)
{
  std::cout << "RSET" << std::endl;
  return 250;
}

int
smtp_server_callback_verbose::on_noop(const smtp_server_worker&)
{
  std::cout << "NOOP" << std::endl;
  return 250;
}

int
smtp_server_callback_verbose::on_quit(const smtp_server_worker&)
{
  std::cout << "QUIT" << std::endl;
  return 221;
}

};

