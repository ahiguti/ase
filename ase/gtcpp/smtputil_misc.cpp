
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/string_util.hpp>
#include <gtcpp/rfc822.hpp>
#include <gtcpp/escape_hex.hpp>
#include <gtcpp/auto_thread.hpp>
#include <gtcpp/auto_threadattr.hpp>
#include <gtcpp/auto_glob.hpp>
#include <gtcpp/smtp_client.hpp>
#include <gtcpp/socket.hpp>
#include <gtcpp/scoped_sigaction.hpp>
#include <gtcpp/mutex.hpp>
#include <gtcpp/file_util.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include <errno.h>
#include <signal.h>
#include <strings.h>

#define DBG_FROM(x)
#define DBG_TO(x)

using namespace gtcpp;

namespace {

class error_callback :
  public posix_error_callback,
  public glob_error_callback,
  public addrinfo_error_callback {
};

int
send_mail(const std::string& host, const std::string& mailfrom,
  const std::string& rcptto, const std::string& body)
{
  using namespace gtcpp;
  posix_error_callback ec;
  scoped_sigaction sa_pipe(SIGPIPE, false);

  smtp_client sc;
  int r = 0;

  socket_connect(sc.get_file(), host.c_str());
  if (!smtp_client::is_2xx(r = sc.greeting_helo("localhost", ec))) {
    return r;
  }
  if (!smtp_client::is_2xx(r = sc.mail_transaction(mailfrom, rcptto, body,
    ec))) {
    return r;
  }
  return r;
}

typedef boost::shared_ptr<smtp_client> conn_type;
struct conn_stat {
  conn_type conn;
  bool need_to_read_greeting;
  conn_stat() : need_to_read_greeting(false) { }
};
typedef std::vector<conn_stat> conns_type;

void
mail_parallel(const std::string& host, int num_conns, bool verbose)
{
  scoped_sigaction sa_pipe(SIGPIPE, false);
  posix_error_callback ec;
  conns_type conns(num_conns);
  while (true) {
    for (conns_type::iterator i = conns.begin(); i != conns.end(); ++i) {
      if (i->conn == 0) {
	conn_type sc(new smtp_client());
	i->conn = sc;
      }
      i->need_to_read_greeting = false;
      const conn_type& sc = i->conn;
      try {
	if (sc->get_file().get() < 0) {
	  i->need_to_read_greeting = true;
	  socket_connect(sc->get_file(), host.c_str());
	}
	if (verbose) {
	  fprintf(stderr, "conn i=%d\n", (int)(i - conns.begin()));
	}
      } catch (const posix_error& e) {
	fprintf(stderr, "mail_parallel: %s\n", e.what());
      }
    }
    for (conns_type::iterator i = conns.begin(); i != conns.end(); ++i) {
      const conn_type& sc = i->conn;
      try {
	if (i->need_to_read_greeting) {
	  sc->greeting(ec);
	}
	string_ref cmd_ehlo("EHLO localhost\r\n");
	if (verbose) {
	  fprintf(stderr, "ehlo i=%d\n", (int)(i - conns.begin()));
	}
	smtp_client::detail::write_request(sc->get_file(), &cmd_ehlo, 1, ec);
      } catch (const posix_error& e) {
	fprintf(stderr, "mail_parallel: greet: %s\n", e.what());
	sc->reset();
      }
    }
    int open_cnt = 0;
    for (conns_type::iterator i = conns.begin(); i != conns.end(); ++i) {
      const conn_type& sc = i->conn;
      try {
	if (sc->get_file().get() >= 0) {
	  sc->get_buffered_file().reset_cursor();
	  string_ref resp;
	  const int r = smtp_client::detail::read_response(
	    sc->get_buffered_file(), resp, ec);
	  if (verbose) {
	    fprintf(stderr, "i=%d r=%d\n", (int)(i - conns.begin()), r);
	  }
	  if (r) {
	    ++open_cnt;
	  }
	}
      } catch (const posix_error& e) {
	fprintf(stderr, "mail_parallel: readresponse: %s\n", e.what());
	sc->reset();
      }
    }
    sleep(5);
    fprintf(stderr, "cnt=%d\n", open_cnt);
  }
}

void
connection_fill(const std::string&, const std::string&, int, bool)
{
}

};

void
smtputil_sendmail(int argc, char **argv)
{
  if (argc < 4) {
    throw std::runtime_error(
      "usage: sendmail HOST FROM TO FILENAME");
  }
  using namespace gtcpp;
  error_callback ec;
  string_buffer sb;
  read_file_contents(argv[3], sb, ec);
  std::string body(sb.begin(), sb.end());
  const int r = send_mail(argv[0], argv[1], argv[2], body);
  std::cerr << "r=" << r << " done" << std::endl;
}

void
smtputil_parallel(int argc, char **argv)
{
  if (argc < 2) {
    throw std::runtime_error(
      "usage: parallel HOST NUM_CONNS [VERBOSE]");
  }
  using namespace gtcpp;
  const std::string host(argv[0]);
  const int num_conns = std::atoi(argv[1]);
  bool verbose = argc > 2 ? std::atoi(argv[2]) : 0;
  mail_parallel(host, num_conns, verbose);
}

void
smtputil_fill(int argc, char **argv)
{
  if (argc < 3) {
    throw std::runtime_error(
      "usage: fill HOST PORT NUM_CONNS [VERBOSE]");
  }
  using namespace gtcpp;
  const std::string host(argv[0]);
  const std::string port(argv[1]);
  const int num_conns = std::atoi(argv[2]);
  bool verbose = argc > 3 ? std::atoi(argv[3]) : 0;
  connection_fill(host, port, num_conns, verbose);
}

