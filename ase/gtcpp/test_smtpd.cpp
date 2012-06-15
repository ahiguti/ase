
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/smtp_server.hpp>
#include <gtcpp/smtp_proxy.hpp>
#include <gtcpp/file_util.hpp>
#include <gtcpp/auto_dir.hpp>
#include <gtcpp/unistd.hpp>
#include <gtcpp/mutex.hpp>
#include <vector>

#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>

using namespace gtcpp;

namespace {

struct smtp_scf_empty : public smtp_server_callback_factory {

  void create_server_callback(std::auto_ptr<smtp_server_callback>& sc_r) {
    std::auto_ptr<smtp_server_callback> c(new smtp_server_callback_empty());
    sc_r = c;
  }
  bool on_timer(time_t) {
    return true;
  }

};

struct smtp_scf_save :
  public mutex,
  public smtp_server_callback_factory {

  smtp_scf_save(const std::string& dname, const std::string& hname,
    bool do_fsync, int pid)
    : dname(dname), hname(hname), do_fsync(do_fsync),
      pid(pid), current_id(0) { }
  void create_server_callback(std::auto_ptr<smtp_server_callback>& sc_r);
  bool on_timer(time_t) {
    return true;
  }

  unsigned long generate_id() volatile {
    scoped_lock<smtp_scf_save> lck(*this);
    int r = lck->current_id;
    ++lck->current_id;
    return r;
  }

  std::string dname;
  std::string hname;
  bool do_fsync;
  int pid;
  unsigned long current_id;

};

struct smtp_cb_save : public smtp_server_callback {

  smtp_cb_save(volatile smtp_scf_save& bref, const std::string& dname,
    const std::string& hname, bool do_fsync, int pid)
    : backref(bref), dname(dname), hname(hname), do_fsync(do_fsync), pec(0),
      got_mailfrom(false), pid(pid) { }

  void clear_transaction() throw () {
    got_mailfrom = false;
    mailfrom = string_buffer_range();
    rcptto.clear();
  }

  virtual int on_open_connection(const smtp_server_worker&,
    posix_error_callback& ec) {
    pec = &ec;
    return 0;
  }
  virtual int on_close_connection(const smtp_server_worker&) throw () {
    clear_transaction();
    pec = 0;
    return 0;
  }
  virtual int on_ehlo(const smtp_server_worker&,
    const string_buffer_range& domain) {
    clear_transaction();
    eh_domain = domain;
    return 250;
  }
  virtual int on_mail(const smtp_server_worker&,
    const string_buffer_range& from) {
    got_mailfrom = true;
    mailfrom = from;
    return 250;
  }
  virtual int on_rcpt(const smtp_server_worker&,
    const string_buffer_range& to) {
    rcptto.push_back(to);
    return 250;
  }
  virtual int on_data(const smtp_server_worker&) {
    return 354;
  }
  virtual int on_body(const smtp_server_worker& svr,
    const string_buffer_range& body_raw, string_buffer&) {
    string_ref str(svr.get_buffer(), body_raw);
    int code = 250;
    struct timeval tv;
    if (::gettimeofday(&tv, 0) == 0) {
      char filename[1024];
      snprintf(filename, sizeof(filename), "%s/%lu_%08lu.%d_%lu",
	dname.c_str(),
	static_cast<unsigned long>(tv.tv_sec),
	static_cast<unsigned long>(tv.tv_usec),
	pid, backref.generate_id());
      int r = 0;
      if ((r = write_file_contents(filename, str, do_fsync, *pec)) < 0) {
	code = 421;
      }
    } else {
      code = 421;
    }
    clear_transaction();
    return code;
  }
  virtual int on_rset(const smtp_server_worker&) {
    clear_transaction();
    return 250;
  }
  virtual int on_noop(const smtp_server_worker&) {
    return 250;
  }
  virtual int on_quit(const smtp_server_worker&) {
    clear_transaction();
    return 221;
  }

  volatile smtp_scf_save& backref;
  std::string dname;
  std::string hname;
  bool do_fsync;
  posix_error_callback *pec;
  string_buffer_range eh_domain;
  bool got_mailfrom;
  int pid;
  string_buffer_range mailfrom;
  std::vector<string_buffer_range> rcptto;

};

void
smtp_scf_save::create_server_callback(
  std::auto_ptr<smtp_server_callback>& sc_r)
{
  std::auto_ptr<smtp_server_callback> c(new smtp_cb_save(*this, dname, hname,
    do_fsync, pid));
  sc_r = c;
}

};

void test_emptymta(int argc, char **argv)
{
  tcp_server::options topt;
  topt.thread_stack_size = 128 * 1024;
  smtp_server::options opt;
  if (argc < 1) {
    throw std::runtime_error(
      "usage: emptymta PORT [NUM_THREADS] [VERBOSE]");
  }
  topt.address_to_bind = argv[0];
  if (argc > 1) {
    topt.num_threads = std::atoi(argv[1]);
  }
  smtp_scf_empty scf;
  smtp_server svr(scf, opt);
  concurrent_tcp_server_main(svr, topt);
}

void test_mailsvr(int argc, char **argv)
{
  tcp_server::options topt;
  if (argc < 1) {
    throw std::runtime_error(
      "usage: mailsvr PORT [NUM_THREADS] [FSYNC]");
  }
  topt.address_to_bind = argv[0];
  if (argc > 1) {
    topt.num_threads = std::atoi(argv[1]);
  }
  bool do_fsync = false;
  if (argc > 2) {
    do_fsync = (std::atoi(argv[2]) > 0);
  }
  std::string dname;
  int pid = static_cast<int>(getpid());
  {
    char buf[1024];
    snprintf(buf, sizeof(buf), "mailsvr.%d", pid);
    dname = std::string(buf);
    posix_error_callback ec;
    mkdir(dname.c_str(), 0755, ec);
  }
  std::string hname;
  smtp_scf_save scf(dname, hname, do_fsync, pid);
  smtp_server::options opt;
  smtp_server svr(scf, opt);
  concurrent_tcp_server_main(svr, topt);
}

void test_smtppr(int argc, char **argv)
{
  tcp_server::options topt;
  if (argc < 3) {
    throw std::runtime_error(
      "usage: smtppr HOST BACKENDHOST [NUM_THREADS]");
  }
  if (argc > 2) {
    topt.num_threads = std::atoi(argv[2]);
  }
  smtp_filter_factory_nochange ff;
  smtp_proxy::args a;
  a.ehlo_domain = gethostname();
  a.backend_address = argv[1];
  smtp_proxy spr(a, ff);
  smtp_server::options opt;
  smtp_server svr(spr, opt);
  concurrent_tcp_server_main(svr, topt);
}

