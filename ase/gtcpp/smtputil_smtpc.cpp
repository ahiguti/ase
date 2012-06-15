
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

template <size_t hnamelen> string_ref
get_mail_address_from_header(const char (& hname)[hnamelen],
  const string_ref& message)
{
  rfc822_headers hdrs(message.begin(), message.end());
  while (hdrs.next()) {
    const char *const nbegin = hdrs.name_begin();
    const char *const nend = hdrs.name_end();
    DBG_FROM(printf("h=[%s] l=%d\n", std::string(nbegin, nend).c_str(),
      (int)(nend - nbegin)));
    if (nend - nbegin == hnamelen - 1 &&
      strncasecmp(hname, nbegin, hnamelen - 1) == 0) {
      /* found */
      const char *vbegin = hdrs.value_begin();
      const char *vend = hdrs.value_end();
      DBG_FROM(printf("found [%s]\n", std::string(vbegin, vend).c_str()));
      rfc822_extract_mail_address(vbegin, vend);
      DBG_FROM(printf("ext [%s]\n", std::string(vbegin, vend).c_str()));
      return string_ref(vbegin, vend);
    }
  }
  return string_ref();
}

class bulkmail_shared : public mutex {

 public:

  typedef size_t size_type;
  typedef std::vector<std::string> filenames_type;
  typedef std::vector<int> results_type;

  bulkmail_shared(long mulmax, int v)
    : current(0), fn_size(0), multiply_count(0), multiply_max(mulmax),
      verbose(v), shutdown(false),
      nmails_success(0), nmails_failed(0), nerrors(0), nconns(0)
  {
  }
  void swap_filenames(filenames_type fn)
  {
    filenames.swap(fn);
    results.clear();
    fn_size = filenames.size();
    results.resize(fn_size);
  }
  size_type size() const { return fn_size; }
  bool has_next() volatile const {
    scoped_lock<const bulkmail_shared> lk(*this);
    return (multiply_max < 0) || (multiply_count < multiply_max)
      || (lk->current < lk->fn_size);
  }
  bool get_next(const std::string *& fn_r, size_type& pos_r) volatile
  {
    scoped_lock<bulkmail_shared> lk(*this);
    if (lk->current >= lk->fn_size) {
      return false;
    }
    fn_r = &lk->filenames[lk->current];
    pos_r = lk->current;
    ++(lk->current);
    if ((multiply_max < 0 || multiply_count < multiply_max - 1)
      && lk->current >= lk->fn_size) {
      lk->current = 0;
      if (multiply_max > 0) {
	++multiply_count;
      }
    }
    if (verbose > 1) {
      std::cerr << "pos=" << pos_r << " fn=" << (*fn_r) << std::endl;
    }
    return true;
  }
  void set_result(size_type pos, int value) volatile
  {
    scoped_lock<bulkmail_shared> lk(*this);
    lk->results[pos] = value;
    if (verbose > 1) {
      std::cerr << "pos=" << pos << " r=" << value << std::endl;
    }
    if (smtp_client::is_2xx(value)) {
      ++nmails_success;
    } else {
      ++nmails_failed;
    }
  }
  void set_error_count() volatile
  {
    scoped_lock<bulkmail_shared> lk(*this);
    ++nerrors;
  }
  void set_shutdown() volatile
  {
    scoped_lock<bulkmail_shared> lk(*this);
    shutdown = true;
  }
  void connection_count(int delta) volatile
  {
    scoped_lock<bulkmail_shared> lk(*this);
    nconns += delta;
  }
  bool is_shutdown() const volatile
  {
    scoped_lock<const bulkmail_shared> lk(*this);
    return shutdown;
  }
  void timer_event() volatile
  {
    scoped_lock<bulkmail_shared> lk(*this);
    std::stringstream sstr;
    sstr << "T " << time(0) << " " << nconns << " " << nmails_success << " "
      << nmails_failed << " " << nerrors << std::endl;
    std::cerr << sstr.str();
    nmails_success = 0;
    nmails_failed = 0;
    nerrors = 0;
  }

 private:

  size_type current;
  size_type fn_size;
  filenames_type filenames;
  results_type results;
  long multiply_count;
  long multiply_max;
  int verbose;
  bool shutdown;
  size_type nmails_success;
  size_type nmails_failed;
  size_type nerrors;
  long nconns;

};

struct bulkmail_timer_thr {
  struct arg_type {
    arg_type(volatile bulkmail_shared *s)
      : shared(s) { }
    volatile bulkmail_shared *const shared;
  };
  bulkmail_timer_thr(const arg_type& a) : arg(a) { }
  void operator ()()
  {
    time_t prev_time = 0;
    do {
      usleep(50000);
      time_t n = time(0);
      if (n != prev_time) {
	arg.shared->timer_event();
	prev_time = n;
      }
    } while (arg.shared->has_next() && !arg.shared->is_shutdown());
  }
  arg_type arg;
};

struct bulkmail_thr {
  struct arg_type {
    arg_type(int i, const std::string& h, const std::string& f,
      const std::string& t, int tsfx, int timeo, int sl,
	bool keepc, bool ier, int v, volatile bulkmail_shared *s)
      : id(i), host(h), mailfrom(f), rcptto(t), to_prefix(tsfx),
	timeout_sec(timeo), sleep_sec(sl), keepconn(keepc), ignore_error(ier),
	verbose(v), shared(s) { }
    int id;
    std::string host;
    std::string mailfrom;
    std::string rcptto;
    int to_prefix;
    int timeout_sec;
    int sleep_sec;
    bool keepconn;
    bool ignore_error;
    int verbose;
    volatile bulkmail_shared *const shared;
  };
  bulkmail_thr(const arg_type& a) : arg(a) { }
  void operator ()()
  {
    thr_main();
  }
  void set_smtp_error(int r, const char *mess, const smtp_client& sc,
    std::string& errstr)
  {
    string_ref bufref(sc.get_buffer());
    string_buffer errbuf;
    errbuf.append(mess, std::strlen(mess));
    to_string_decimal(errbuf, r);
    errbuf.append_literal(" buffer=[");
    escape_hexadecimal_control(errbuf, bufref.begin(), bufref.end());
    errbuf.append_literal("]");
    errstr = std::string(errbuf.begin(), errbuf.end());
  }
  void update_conn_state(smtp_client& sc, bool& cur_open)
  {
    if (cur_open && sc.get_file().get() < 0) {
      arg.shared->connection_count(-1);
      cur_open = false;
    } else if (!cur_open && sc.get_file().get() >= 0) {
      arg.shared->connection_count(1);
      cur_open = true;
    }
  }
  bool open_conn(smtp_client& sc, bool& cur_open, std::string& errstr,
    posix_error_callback& ec)
  {
    int r = 0;
    if (sc.get_file().get() >= 0) {
      return true; /* no need to open */
    }
    close_conn(sc, cur_open);
    socket_connect(sc.get_file(), arg.host.c_str(),
      arg.timeout_sec >= 0 ? arg.timeout_sec : 0);
    /* FIXME: */
    if (!smtp_client::is_2xx(r = sc.greeting_helo("localhost", ec))) {
      set_smtp_error(r, "greeting/helo failed r=", sc, errstr);
      return false;
    }
    update_conn_state(sc, cur_open);
    return true;
  }
  void close_conn(smtp_client& sc, bool& cur_open)
  {
    sc.reset();
    update_conn_state(sc, cur_open);
  }
  void thr_main()
  {
    size_t pos = 0;
    const std::string *fn = 0;
    error_callback ec;
    posix_error_callback_nothrow ecnthr;
    auto_file fd;
    string_buffer sb;
    smtp_client sc;
    bool cur_open = false;
    int r = 0;
    int n = 1;
    string_buffer to_buf;
    std::string to_local, to_domain;
    if (arg.to_prefix != 0) {
      for (int i = 0; i < arg.to_prefix; ++i) {
	n *= 10;
      }
      string_ref to(arg.rcptto);
      const size_t atpos = find_char(to, '@');
      to_local = std::string(to.begin(), atpos);
      to_domain = std::string(to.begin() + atpos, to.end());
    }
    while (arg.shared->get_next(fn, pos)) {
      std::string errstr;
      try {
	sb.reset(4000);
	r = read_file_contents(fn->c_str(), sb, ecnthr);
	if (r < 0) {
	  if (ecnthr.get_last_posix_error() == EISDIR) {
	    continue;
	  } else {
	    ecnthr.throw_last_posix_error();
	  }
	}
	const string_ref body(sb.begin(), sb.end());
	string_ref mailfrom(arg.mailfrom);
	if (mailfrom.empty()) {
	  mailfrom = get_mail_address_from_header("return-path", body);
	}
	open_conn(sc, cur_open, errstr, ec);
	for (int i = 0; i < n; ++i) {
	  string_ref rcptto(arg.rcptto);
	  if (n > 1) {
	    to_buf.clear();
	    to_string_decimal(to_buf, i);
	    to_buf.append(to_local.data(), to_local.size());
	    to_buf.append(to_domain.data(), to_domain.size());
	    DBG_TO(printf("to=[%s]\n",
	      std::string(to_buf.begin(), to_buf.end()).c_str()));
	    rcptto.set(to_buf.begin(), to_buf.end());
	  }
	  r = sc.mail_transaction(mailfrom, rcptto, body, ec);
	  arg.shared->set_result(pos, r);
	  if (!smtp_client::is_2xx(r)) {
	    set_smtp_error(r, "mail transaction failed r=", sc, errstr);
	    break;
	  } else if (arg.verbose > 0) {
	    std::stringstream sstr;
	    string_ref resp = sc.get_response();
	    size_t i = 0;
	    for (; i < resp.size() && resp.begin()[i] >= 0x20; ++i) { }
	    sstr << "M " << std::string(resp.begin(), i) << std::endl;
	    std::cerr << sstr.str();
	  }
	  if (arg.sleep_sec > 0) {
	    sleep(arg.sleep_sec);
	  }
	}
	if (!arg.keepconn) {
	  close_conn(sc, cur_open);
	}
	if (r <= 0) {
	  errstr = "connection closed";
	}
      } catch (const std::exception& e) {
	errstr = e.what();
      } catch (...) {
	errstr = "unknown exception";
      }
      if (!errstr.empty()) {
	if (arg.verbose > -1) {
	  std::stringstream sstr;
	  sstr << "E " << time(0) << " thr=" << arg.id << ": " << errstr
	    << std::endl;
	  std::cerr << sstr.str();
	}
	arg.shared->set_error_count();
	close_conn(sc, cur_open);
	if (!arg.ignore_error) {
	  return;
	}
	sleep(5); /* delay */
      }
    }
  }
  arg_type arg;
};

int
bulk_mail(const std::string& host, const std::string& mailfrom,
  const std::string& rcptto, int to_prefix, const std::string& glob_pattern,
  int num_threads, int timeout_sec, int sleep_sec, long multiply,
  bool keep_conn, bool ignore_error_flag, int verbose)
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
  bulkmail_shared shared(multiply, verbose);
  shared.swap_filenames(fnlist);

  typedef boost::shared_ptr< threaded<bulkmail_thr> > thr_type;
  typedef std::vector<thr_type> thrs_type;
  auto_threadattr attr;
  threadattr_setstacksize(attr, 128 * 1024);
  thrs_type thrs(num_threads);
  for (thrs_type::size_type i = 0; i < thrs.size(); ++i) {
    bulkmail_thr::arg_type arg(i, host, mailfrom, rcptto, to_prefix,
      timeout_sec, sleep_sec, keep_conn, ignore_error_flag, verbose,
      &shared);
    thr_type thr(new threaded<bulkmail_thr>(arg));
    thrs[i] = thr;
  }
  bulkmail_timer_thr::arg_type targ(&shared);
  boost::shared_ptr< threaded<bulkmail_timer_thr> > timer_thr(
    new threaded<bulkmail_timer_thr>(targ));
  timer_thr->start(attr);
  for (thrs_type::size_type i = 0; i < thrs.size(); ++i) {
    thrs[i]->start(attr);
  }
  for (thrs_type::size_type i = 0; i < thrs.size(); ++i) {
    thrs[i]->join();
  }
  shared.set_shutdown();
  timer_thr->join();

  return 0;
}

};

void
smtputil_bulkmail(int argc, char **argv)
{
  std::map<std::string, std::string> p;
  parse_argv(argc, argv, p);
  int num_threads = std::atoi(p["thr"].c_str());
  int timeout_sec = std::atoi(p["timeo"].c_str());
  const int to_prefix = std::atoi(p["toprefix"].c_str());
  const bool infinite_mails = std::atoi(p["infmails"].c_str());
  long multiply = std::atol(p["multiply"].c_str());
  if (infinite_mails) {
    multiply = -1;
  }
  const bool loop_flag = std::atoi(p["loop"].c_str());
  int sleep_sec = std::atoi(p["sleep"].c_str());
  const bool keep_conn = std::atoi(p["keepconn"].c_str());
  const bool ignore_error = std::atoi(p["ierror"].c_str());
  const int verbose = std::atoi(p["verbose"].c_str());
  std::string host = p["host"];
  std::string port = p["port"];
  std::string from = p["from"];
  std::string to = p["to"];
  std::string msg = p["msg"];
  num_threads = num_threads ? num_threads : 50;
  timeout_sec = timeout_sec ? timeout_sec : 30;
  std::cerr << "host=" << host << std::endl;
  std::cerr << "port=" << port << std::endl;
  std::cerr << "from=" << from << std::endl;
  std::cerr << "to=" << to << std::endl;
  std::cerr << "toprefix=" << to_prefix << std::endl;
  std::cerr << "msg=" << msg << std::endl;
  std::cerr << "thr=" << num_threads << std::endl;
  std::cerr << "timeo=" << timeout_sec << std::endl;
  std::cerr << "sleep=" << sleep_sec << std::endl;
  std::cerr << "keepconn=" << keep_conn << std::endl;
  std::cerr << "ierror=" << ignore_error << std::endl;
  std::cerr << "infmails=" << infinite_mails << std::endl;
  std::cerr << "multiply=" << multiply << std::endl;
  std::cerr << "loop=" << loop_flag << std::endl;
  std::cerr << "verbose=" << verbose << std::endl;
  if (host.empty() || msg.empty()) {
    throw std::runtime_error(
      "usage: bulkmail host=HOST port=PORT msg=GLOB_PATTERN "
      "[from=FROM] [to=TO] [toprefix=NUM] [thr=NUM_THREADS] [verbose=NUM] "
      "[timeo=SEC] [sleep=SEC] [keepconn=(0|1)] [ierror=(0|1)] "
      "[infmails=(0|1)] [multiply=NUM] [loop=(0|1)]");
  }
  if (!port.empty()) {
    host += ":" + port;
  }
  using namespace gtcpp;
  error_callback ec;
  do {
    bulk_mail(host, from, to, to_prefix, msg, num_threads, timeout_sec,
      sleep_sec, multiply, keep_conn, ignore_error, verbose);
  } while (loop_flag);
}

