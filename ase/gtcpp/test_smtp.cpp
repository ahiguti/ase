
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/string_util.hpp>
#include <gtcpp/auto_thread.hpp>
#include <gtcpp/auto_threadattr.hpp>
#include <gtcpp/auto_glob.hpp>
#include <gtcpp/smtp_client.hpp>
#include <gtcpp/socket.hpp>
#include <gtcpp/file_util.hpp>
#include <gtcpp/scoped_sigaction.hpp>
#include <gtcpp/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <vector>
#include <errno.h>
#include <signal.h>

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

class bulkmail_shared : public mutex {

 public:

  typedef size_t size_type;
  typedef std::vector<std::string> filenames_type;
  typedef std::vector<int> results_type;

  bulkmail_shared(bool v)
    : current(0), fn_size(0), verbose(v) { }
  void swap_filenames(filenames_type fn) {
    filenames.swap(fn);
    results.clear();
    fn_size = filenames.size();
    results.resize(fn_size);
  }
  size_type size() const { return fn_size; }
  bool get_next(const std::string *& fn_r, size_type& pos_r) volatile {
    scoped_lock<bulkmail_shared> lk(*this);
    if (lk->current >= lk->fn_size) {
      return false;
    }
    fn_r = &lk->filenames[lk->current];
    pos_r = lk->current;
    ++(lk->current);
    if (verbose) {
      std::cerr << "pos=" << pos_r << " fn=" << (*fn_r) << std::endl;
    }
    return true;
  }
  void set_result(size_type pos, int value) volatile {
    scoped_lock<bulkmail_shared> lk(*this);
    lk->results[pos] = value;
    if (verbose) {
      std::cerr << "pos=" << pos << " r=" << value << std::endl;
    }
  }

 private:

  size_type current;
  size_type fn_size;
  filenames_type filenames;
  results_type results;
  bool verbose;

};

struct bulkmail_thr {
  struct arg_type {
    arg_type(const std::string& h, const std::string& f, const std::string& t,
      bool v, volatile bulkmail_shared *s)
      : host(h), mailfrom(f), rcptto(t), verbose(v), shared(s) { }
    std::string host;
    std::string mailfrom;
    std::string rcptto;
    bool verbose;
    volatile bulkmail_shared *shared;
  };
  bulkmail_thr(const arg_type& a) : arg(a) { }
  void operator ()() {
    try {
      thr_main();
    } catch (const std::exception& e) {
      errstr = e.what();
    }
  }
  void thr_main() {
    size_t pos = 0;
    const std::string *fn = 0;
    error_callback ec;
    posix_error_callback_nothrow ecnthr;
    auto_file fd;
    string_buffer sb;
    smtp_client sc;
    int r = 0;

    socket_connect(sc.get_file(), arg.host.c_str());
    if (!smtp_client::is_2xx(r = sc.greeting_helo("localhost", ec))) {
      errstr = "failed to initialize mail transaction";
      return;
    }
    while (arg.shared->get_next(fn, pos)) {
      sb.reset(4000);
      r = read_file_contents(fn->c_str(), sb, ecnthr);
      if (r < 0) {
	if (ecnthr.get_last_posix_error() == EISDIR) {
	  continue;
	}
	ecnthr.throw_last_posix_error();
      }
      const string_ref body(sb.begin(), sb.end());
      r = sc.mail_transaction(arg.mailfrom, arg.rcptto, body, ec);
      arg.shared->set_result(pos, r);
      if (r <= 0) {
	errstr = "connection closed";
	break;
      }
    }
  }
  arg_type arg;
  std::string errstr;
};

int
bulk_mail(const std::string& host, const std::string& mailfrom,
  const std::string& rcptto, const std::string& glob_pattern,
  int num_threads = 100, bool verbose_flag = false)
{
  using namespace gtcpp;
  error_callback ec;
  scoped_sigaction sa_pipe(SIGPIPE, false);

  auto_glob gl;
  glob(gl, glob_pattern.c_str(), 0, ec);
  std::vector<std::string> fnlist;
  for (size_t i = 0; i < gl.get().gl_pathc; ++i) {
    fnlist.push_back(std::string(gl.get().gl_pathv[i]));
  }
  bulkmail_shared shared(verbose_flag);
  shared.swap_filenames(fnlist);
  bulkmail_thr::arg_type arg(host, mailfrom, rcptto, verbose_flag, &shared);

  typedef boost::shared_ptr< threaded<bulkmail_thr> > thr_type;
  typedef std::vector<thr_type> thrs_type;
  auto_threadattr attr;
  threadattr_setstacksize(attr, 1024 * 1024);
  thrs_type thrs(num_threads);
  for (thrs_type::size_type i = 0; i < thrs.size(); ++i) {
    thr_type thr(new threaded<bulkmail_thr>(arg));
    thrs[i] = thr;
  }
  for (thrs_type::size_type i = 0; i < thrs.size(); ++i) {
    thrs[i]->start(attr);
  }
  for (thrs_type::size_type i = 0; i < thrs.size(); ++i) {
    thrs[i]->join();
    const bulkmail_thr& bm = thrs[i]->get();
    std::string err = bm.errstr;
    if (!err.empty()) {
      std::cerr << "thread " << i << ": " << err << std::endl;
    }
  }

  return 0;
}

};

void test_smtpc(int argc, char **argv)
{
  if (argc < 1) {
    throw std::runtime_error("usage: smtpc HOST");
  }
  std::string host(argv[0]);
  std::string mailfrom("from@example.com");
  std::string rcptto("to@example.com");
  std::string body =
    "From: <hoge>\r\n"
    "To: <nobody+x@example.com>\r\n"
    "\r\n"
    "T-E-S-T\r\n"
    ".T-E-S-T\r\n"
    "..T-E-S-T\r\n";
  const int r = send_mail(host, mailfrom, rcptto, body);
  std::cerr << "r=" << r << " done" << std::endl;
}

void test_smtpd(int argc, char **argv)
{
  if (argc < 1) {
    throw std::runtime_error("usage: smtpd BINDHOST");
  }
  using namespace gtcpp;
  error_callback ec;
  scoped_sigaction sa_pipe(SIGPIPE, false);

  auto_file sd;
  socket_listen(sd, argv[0]);
  sockaddr_generic addr;
  auto_file conn;
  while (true) {
    std::cerr << "accept..." << std::endl;
    accept(sd, conn, addr, ec);
    std::string resp;
    resp = "220 ESMTP ready\r\n";
    ::shutdown(conn.get(), SHUT_RD);
    write(conn, resp.data(), resp.size(), ec);
    close(conn, ec);
    break;
  }
  std::cerr << "done" << std::endl;
}

void test_sendmail(int argc, char **argv)
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

void test_bulkmail(int argc, char **argv)
{
  if (argc < 4) {
    throw std::runtime_error(
      "usage: bulkmail HOST FROM TO GLOB_PATTERN "
      "[NUM_THREADS] [VERBOSE]");
  }
  using namespace gtcpp;
  error_callback ec;
  std::string pattern(argv[3]);
  std::cerr << "pattern: [" << pattern << "]" << std::endl;
  const int num_threads = argc >= 5 ? atoi(argv[4]) : 100;
  const bool verbose_flag = (argc >= 6 && atoi(argv[5]));
  bulk_mail(argv[0], argv[1], argv[2], pattern, num_threads, verbose_flag);
  std::cerr << "done" << std::endl;
}

