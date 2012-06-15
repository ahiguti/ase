
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/smtp_server.hpp>
#include <gtcpp/smtp_proxy.hpp>
#include <gtcpp/smtp_common.hpp>
#include <gtcpp/file_util.hpp>
#include <gtcpp/auto_dir.hpp>
#include <gtcpp/unistd.hpp>
#include <gtcpp/mutex.hpp>
#include <gtcpp/time.hpp>
#include <gtcpp/string_util.hpp>
#include <gtcpp/piggyback_tr.hpp>
#include <vector>
#include <sstream>
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>

#define DBG_FROM(x)
#define DBG_MD(x)
#define DBG(x)

using namespace gtcpp;

namespace {

struct smtp_scf_empty;

struct smtp_sc_counting : public smtp_server_callback_empty {

  smtp_sc_counting(volatile smtp_scf_empty *ref) : backref(ref) { }

  virtual int on_open_connection(const smtp_server_worker&,
    posix_error_callback&);
  virtual int on_close_connection(const smtp_server_worker&) throw ();

  volatile smtp_scf_empty *const backref;

};

struct smtp_scf_empty :
  public mutex,
  public smtp_server_callback_factory {

  smtp_scf_empty() : count(0) { }

  void create_server_callback(std::auto_ptr<smtp_server_callback>& sc_r) {
    std::auto_ptr<smtp_server_callback> c(new smtp_sc_counting(this));
    sc_r = c;
  }
  bool on_timer(time_t now) {
    scoped_lock<smtp_scf_empty> lck(*this);
    std::stringstream sstr;
    sstr << now << " conns=" << lck->count << std::endl;
    std::cerr << sstr.str();
    return true;
  }

  void add_count(int v) volatile {
    scoped_lock<smtp_scf_empty> lck(*this);
    lck->count += v;
  }

  long long count;

};

int
smtp_sc_counting::on_open_connection(const smtp_server_worker&,
    posix_error_callback&)
{
  backref->add_count(1);
  return 0;
}

int
smtp_sc_counting::on_close_connection(const smtp_server_worker&) throw ()
{
  backref->add_count(-1);
  return 0;
}

struct smtp_scf_save_args {
  smtp_scf_save_args() : single_file_mode(false), do_fsync(false),
    piggyback_max(1), pid(-1) { }
  std::string folder_name;
  bool single_file_mode;
  bool do_fsync;
  int piggyback_max;
  int pid;
};

int exp10_int(int x)
{
  int r = 1;
  for (int i = 0; i < x; ++i) {
    r *= 10;
  }
  return r;
}


struct smtp_scf_save;

/* BEGIN single_file_mode */
struct smtp_save_savefile {
  smtp_save_savefile(const string_ref& f1, const string_ref& f2)
    : frag1(f1), frag2(f2), success(false) { }
  const string_ref frag1, frag2;
  bool success;
};
/* END single_file_mode */

struct smtp_scf_save :
  public piggyback_transaction<smtp_save_savefile>,
  public smtp_server_callback_factory {

  smtp_scf_save(const smtp_scf_save_args& a)
    : piggyback_transaction<smtp_save_savefile>(a.piggyback_max), args(a),
    max_digit_log10(3), max_digit(exp10_int(max_digit_log10)) { }
  void create_server_callback(std::auto_ptr<smtp_server_callback>& sc_r);
  bool on_timer(time_t) { return true; }

  int mkdir_hier_if(const std::string& dname,
    posix_error_callback_nothrow& ec) {
    struct stat sbuf;
    int s = stat(dname.c_str(), &sbuf, ec);
    if (s < 0) {
      if (ec.get_last_posix_error() == ENOENT) {
	if ((s = mkdir_hier_parent(dname, ec)) < 0) {
	  return s;
	}
	return mkdir(dname.c_str(), 0755, ec);
      } else {
	return s;
      }
    }
    return 0;
  }
  int mkdir_hier_parent(const std::string& dname,
    posix_error_callback_nothrow& ec) {
    std::string::size_type pos = dname.rfind('/');
    if (pos == dname.npos || pos == 0) {
      return 0;
    }
    return mkdir_hier_if(dname.substr(0, pos), ec);
  }
  int mkdir_hier(const char *filename, posix_error_callback_nothrow& ec)
    volatile {
    scoped_lock<smtp_scf_save> lck(*this);
    return lck->mkdir_hier_parent(filename, ec);
  }

  /* BEGIN multi_file_mode */
  void generate_filename(string_buffer& filename_r) volatile {
    scoped_lock<smtp_scf_save> lck(*this);
    lck->increment_id();
    const size_t ndigits = lck->id.size();
    to_string_decimal(filename_r, static_cast<int>(ndigits));
    for (size_t i = 0; i < ndigits; ++i) {
      const int v = lck->id[ndigits - i - 1];
      filename_r.append_literal("/");
      to_string_decimal_zero(filename_r, v, max_digit_log10);
    }
    struct timeval tv;
    gettimeofday(&tv, lck->pec_throwall);
    const int wlen_max = 64;
    char *const wpos = filename_r.make_space(wlen_max);
    const int wlen = snprintf(wpos, wlen_max, "_%lu_%08lu.%d",
      static_cast<unsigned long>(tv.tv_sec),
      static_cast<unsigned long>(tv.tv_usec),
      args.pid);
    if (wlen > 0) {
      filename_r.append_done(wlen);
    }
    filename_r.append_literal("\0");
  }
  size_t increment_id() {
    const size_t nd = id.size();
    if (nd == 0) {
      id.push_back(1);
      return 0;
    }
    size_t i = 0;
    while (true) {
      if ((++id[i]) < max_digit) {
	break;
      }
      id[i] = 0;
      if ((++i) >= nd) {
	id.push_back(0);
      }
    }
    return i;
  }
  /* END multi_file_mode */

  /* BEGIN single_file_mode */
  void begin_transaction() {
    if (!single_file_fd.is_valid()) {
      open(single_file_fd, (args.folder_name + "mbox").c_str(),
	O_WRONLY | O_CREAT | O_APPEND, 0644, pec_throwall);
    }
  }
  bool commit_transaction() {
    if (!single_file_fd.is_valid()) {
      return false;
    }
    return (fsync(single_file_fd, pec_nothrow) == 0);
  }
  void abort_transaction() {
  }
  void execute_procedure(smtp_save_savefile& proc) {
    proc.success = false;
    const size_t len1 = proc.frag1.size();
    const size_t len2 = proc.frag2.size();
    const ssize_t r1 = write(single_file_fd, proc.frag1.begin(), len1,
      pec_nothrow);
    if (r1 != static_cast<ssize_t>(len1)) {
      return;
    }
    const ssize_t r2 = write(single_file_fd, proc.frag2.begin(), len2,
      pec_nothrow);
    if (r2 != static_cast<ssize_t>(len2)) {
      return;
    }
    proc.success = true;
  }
  bool do_save_file_single_file_mode(smtp_save_savefile& proc) volatile {
    return this->execute_piggyback(proc);
  }
  /* END single_file_mode */

  const smtp_scf_save_args args;
  std::vector<int> id;
  int max_digit_log10;
  int max_digit;
  posix_error_callback pec_throwall;
  posix_error_callback_nothrow pec_nothrow;
  /* BEGIN single_file_mode */
  auto_file single_file_fd;
  /* END single_file_mode */

};

struct smtp_cb_save : public smtp_server_callback {

  smtp_cb_save(volatile smtp_scf_save& bref, const smtp_scf_save_args& a)
    : backref(bref), args(a), pec(0), got_mailfrom(false)
  {
    filename_buffer.append(args.folder_name.data(), args.folder_name.size());
  }

  void clear_mail_transaction() throw () {
    got_mailfrom = false;
    mailfrom = string_buffer_range();
    rcptto.clear();
    headers.clear();
    filename_buffer.truncate(args.folder_name.size());
    body.clear();
    bodyraw.reset();
  }

  virtual int on_open_connection(const smtp_server_worker&,
    posix_error_callback& ec) {
    pec = &ec;
    return 0;
  }
  virtual int on_close_connection(const smtp_server_worker&) throw () {
    clear_mail_transaction();
    pec = 0;
    return 0;
  }
  virtual int on_ehlo(const smtp_server_worker&,
    const string_buffer_range& domain) {
    clear_mail_transaction();
    eh_domain = domain;
    return 250;
  }
  virtual int on_mail(const smtp_server_worker&,
    const string_buffer_range& from) {
    got_mailfrom = true;
    mailfrom = from;
    DBG_FROM(string_ref mf(svr.get_buffer(), mailfrom));
    DBG_FROM(printf("mailfrom=[%s]\n",
      std::string(mf.begin(), mf.end()).c_str()));
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
    const string_buffer_range& body_raw, string_buffer& resp) {
    if (args.single_file_mode) {
      return on_body_single_file_mode(svr, body_raw, resp);
    } else {
      return on_body_multi_file_mode(svr, body_raw, resp);
    }
  }
  /* BEGIN single_file_mode */
  int on_body_single_file_mode(const smtp_server_worker& svr,
    const string_buffer_range& body_raw, string_buffer&) {
    const char lit_retpath_pre[] = "Return-Path: <";
    const char lit_retpath_post[] = ">\r\n";
    string_ref mf(svr.get_buffer(), mailfrom);
    headers.append(lit_retpath_pre, sizeof(lit_retpath_pre) - 1);
    headers.append(mf.begin(), mf.size());
    headers.append(lit_retpath_post, sizeof(lit_retpath_post) - 1);
    bodyraw = string_ref(svr.get_buffer(), body_raw);
    int code = 421;
    smtp_save_savefile sf(headers, bodyraw);
    if (backref.do_save_file_single_file_mode(sf) && sf.success) {
      code = 250;
    }
    clear_mail_transaction();
    return code;
  }
  /* END single_file_mode */
  /* BEGIN multi_file_mode */
  int on_body_multi_file_mode(const smtp_server_worker& svr,
    const string_buffer_range& body_raw, string_buffer& resp) {
    string_ref contents[2];
    string_ref& addhdr = contents[0];
    string_ref& msgbody = contents[1];
    const char lit_retpath_pre[] = "Return-Path: <";
    const char lit_retpath_post[] = ">\r\n";
    string_ref mf(svr.get_buffer(), mailfrom);
    headers.append(lit_retpath_pre, sizeof(lit_retpath_pre) - 1);
    headers.append(mf.begin(), mf.size());
    headers.append(lit_retpath_post, sizeof(lit_retpath_post) - 1);
    const string_ref msgbody_raw(svr.get_buffer(), body_raw);
    smtp_unescape_body(body, msgbody_raw);
    addhdr = string_ref(headers.begin(), headers.end());
    msgbody = string_ref(body);
    int code = 421;
    backref.generate_filename(filename_buffer);
      /* filename_buffer is nul-terminated */
    int r = 0;
    for (int retry_count = 0; retry_count < 2; ++retry_count) {
      DBG(printf("retry_count = %d\n", retry_count));
      DBG(printf("wrfc %s\n", filename_buffer.begin()));
      if ((r = writev_file_contents(filename_buffer.begin(), contents,
	contents + 2, args.do_fsync, *pec)) < 0) {
	if (backref.mkdir_hier(filename_buffer.begin(), pec_nothrow) == 0) {
	  continue; /* retry */
	}
      } else {
	resp.append_literal("queued as ");
	resp.append(filename_buffer.begin(), filename_buffer.size() - 1);
	resp.append_literal(" .");
	code = 250;
	break;
      }
    }
    clear_mail_transaction();
    return code;
  }
  /* END multi_file_mode */
  virtual int on_rset(const smtp_server_worker&) {
    clear_mail_transaction();
    return 250;
  }
  virtual int on_noop(const smtp_server_worker&) {
    return 250;
  }
  virtual int on_quit(const smtp_server_worker&) {
    clear_mail_transaction();
    return 221;
  }

  volatile smtp_scf_save& backref;
  const smtp_scf_save_args& args;
  posix_error_callback *pec;
  posix_error_callback_nothrow pec_nothrow;
  string_buffer_range eh_domain;
  bool got_mailfrom;
  string_buffer_range mailfrom;
  std::vector<string_buffer_range> rcptto;
  string_buffer headers;
  string_buffer filename_buffer;
  string_buffer body;
  string_ref bodyraw;

};

void
smtp_scf_save::create_server_callback(
  std::auto_ptr<smtp_server_callback>& sc_r)
{
  std::auto_ptr<smtp_server_callback> c(new smtp_cb_save(*this, args));
  sc_r = c;
}

};

void
smtputil_emptymta(int argc, char **argv)
{
  tcp_server::options topt;
  if (argc < 1) {
    throw std::runtime_error(
      "usage: emptymta BINDHOST [NUM_THREADS] [VERBOSE]");
  }
  topt.address_to_bind = argv[0];
  topt.thread_stack_size = 1024 * 1024;
  if (argc > 1) {
    topt.num_threads = std::atoi(argv[1]);
  }
  smtp_scf_empty scf;
  smtp_server::options opt;
  smtp_server svr(scf, opt);
  concurrent_tcp_server_main(svr, topt);
}

void
smtputil_mailsvr(int argc, char **argv)
{
  std::map<std::string, std::string> p;
  parse_argv(argc, argv, p);
  smtp_scf_save_args ssargs;
  tcp_server::options topt;
  topt.address_to_bind = p["host"];
  topt.num_threads = std::atoi(p["thr"].c_str());
  if (topt.num_threads <= 0) {
    topt.num_threads = 100;
  }
  std::string dname = p["save"];
  if (dname.empty()) {
    dname = "mailsvr";
  }
  if (::access(dname.c_str(), X_OK) != 0) {
    posix_error_callback ec;
    mkdir(dname.c_str(), 0755, ec);
  }
  if (dname[dname.size() - 1] != '/') {
    dname += "/";
  }
  ssargs.folder_name = dname;
  ssargs.single_file_mode = std::atoi(p["single"].c_str());
  ssargs.do_fsync = true;
  if (topt.num_threads > 1) {
    ssargs.piggyback_max = topt.num_threads / 2;
  }
  if (!p["pgy"].empty()) {
    ssargs.piggyback_max = std::atoi(p["pgy"].c_str());
  }
  if (!p["fsync"].empty()) {
    ssargs.do_fsync = std::atoi(p["fsync"].c_str());
  }
  std::cerr << "host=" << topt.address_to_bind << std::endl;
  std::cerr << "thr=" << topt.num_threads << std::endl;
  std::cerr << "pgy=" << ssargs.piggyback_max << std::endl;
  std::cerr << "save=" << ssargs.folder_name << std::endl;
  std::cerr << "fsync=" << ssargs.do_fsync << std::endl;
  std::cerr << "single=" << ssargs.single_file_mode << std::endl;
  if (topt.address_to_bind.empty()) {
    throw std::runtime_error("usage: mailsvr host=HOST");
  }
  const int pid = static_cast<int>(getpid());
  ssargs.pid = pid;
  smtp_scf_save scf(ssargs);
  smtp_server::options opt;
  smtp_server svr(scf, opt);
  concurrent_tcp_server_main(svr, topt);
}

