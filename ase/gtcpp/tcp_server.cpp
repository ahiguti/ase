
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/tcp_server.hpp>
#include <gtcpp/auto_threadattr.hpp>
#include <gtcpp/mutex.hpp>
#include <gtcpp/scoped_sigaction.hpp>
#include <gtcpp/scoped_sigmask.hpp>
#include <gtcpp/time.hpp>
#include <gtcpp/atomicity.hpp>
#include <gtcpp/unistd.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <set>
#include <sys/types.h>
#include <sys/wait.h>

#define SERIALIZE_ACCEPT 0

#define DBG(x)

namespace gtcpp {

namespace {

class tcpsvr_shared : public mutex {

 public:

  tcpsvr_shared(tcp_server& svr, const tcp_server::options& o)
    : server(svr), opts(o), sock_fd(-1), shut_flag(false) { }
  auto_file& get_file() { return sock; }
  void set_fd() {
    sock_fd = sock.get();
  }
  void worker_process_initialized() volatile {
    scoped_lock<tcpsvr_shared> lk(*this);
    lk->server.worker_process_initialized();
  }
  void create_worker(std::auto_ptr<tcp_server_worker>& wkr_r) volatile {
    scoped_lock<tcpsvr_shared> lk(*this);
    lk->server.create_worker(wkr_r);
  }
  void set_shut() volatile {
    shut_flag = true;
  }
  bool accept(auto_file& sd_r, sockaddr_generic& addr_r,
    posix_error_callback& ec) volatile {
    while (true) {
      #if SERIALIZE_ACCEPT
      scoped_lock<tcpsvr_shared> lk(*this);
      if (shut_flag) {
	return false;
      }
      int r = gtcpp::accept(lk->sock, sd_r, addr_r, ec);
      #else
      int r = gtcpp::accept(sock_fd, sd_r, addr_r, ec);
      #endif
      if (shut_flag) {
	return false;
      }
      if (r >= 0) {
	return true;
      }
      /* retry */
      nanosleep(1, 0, ec);
    }
  }

 private:

  tcp_server& server;
  tcp_server::options opts;
  auto_file sock;
  int sock_fd;
  volatile bool shut_flag;

};

struct tcpsvr_thr {

  struct arg_type {
    arg_type(volatile tcpsvr_shared *s, int to_sec) : shared(s),
      timeout_sec(to_sec){ }
    volatile tcpsvr_shared *shared;
    int timeout_sec;
  };

  tcpsvr_thr(const arg_type& a) : arg(a), finished(false) {
    arg.shared->create_worker(worker);
  }
  void operator ()() {
    {
      thr_main();
      finished = true;
    }
    DBG(fprintf(stderr, "thread finished\n"));
  }
  void thr_main() {
    posix_error_callback ecthrw;
    posix_error_callback_nothrow ecnotrw;
    while (true) {
      int r = 0;
      auto_file conn;
      sockaddr_generic cliaddr;
      DBG(fprintf(stderr, "pre accept\n"));
      bool accepted = arg.shared->accept(conn, cliaddr, ecnotrw);
      if (!accepted) {
	/* shutdown */
	break;
      }
      if (arg.timeout_sec > 0) {
        struct timeval tv;
	tv.tv_sec = arg.timeout_sec;
	tv.tv_usec = 0;
        setsockopt(conn, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv), ecthrw);
	tv.tv_sec = arg.timeout_sec;
	tv.tv_usec = 0;
        setsockopt(conn, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv), ecthrw);
      }
      DBG(fprintf(stderr, "pre run\n"));
      if ((r = worker->run(conn, cliaddr, ecnotrw)) < 0) {
	continue;
      }
    }
  }
  bool get_finished() volatile {
    return finished;
  }
  std::string get_errstr() const {
    return errstr;
  }

 private:

  arg_type arg;
  std::auto_ptr<tcp_server_worker> worker;
  std::string errstr;
  volatile bool finished;

};

struct tcpsvr_threads : private boost::noncopyable {

  typedef boost::shared_ptr< threaded<tcpsvr_thr> > thr_type;
  typedef std::vector<thr_type> thrs_type;

  tcpsvr_threads(size_t num_threads, size_t thread_stack_size,
    tcpsvr_thr::arg_type& arg)
    : shared(arg.shared), thrs(num_threads), thrstacksize(thread_stack_size) {
    for (thrs_type::size_type i = 0; i < thrs.size(); ++i) {
      thr_type thr(new threaded<tcpsvr_thr>(arg));
      thrs[i] = thr;
    }
  }
  ~tcpsvr_threads() throw() {
    for (thrs_type::size_type i = 0; i < thrs.size(); ++i) {
      thrs[i]->join();
      /* it's safe to access without locking because it's already joined */
      const tcpsvr_thr& bm = thrs[i]->get();
      std::string err = bm.get_errstr();
      if (!err.empty()) {
	DBG(fprintf(stderr, "thread %d: %s\n", (int)i, err.c_str()));
      }
    }
  }
  void start() {
    auto_threadattr attr;
    threadattr_setstacksize(attr, thrstacksize);
    for (thrs_type::size_type i = 0; i < thrs.size(); ++i) {
      thrs[i]->start(attr);
    }
  }

  volatile tcpsvr_shared *shared;
  thrs_type thrs;
  size_t thrstacksize;

};

int
concurrent_tcp_server_worker_main(tcpsvr_shared& shared,
  const tcp_server::options& o)
{
  shared.worker_process_initialized();
  tcpsvr_thr::arg_type arg(&shared, o.worker_socket_timeout_sec);
  if (o.num_threads <= 0) {
    DBG(fprintf(stderr, "no-threading\n"));
    tcpsvr_thr thr(arg);
    thr();
  } else {
    DBG(fprintf(stderr, "threading %d\n", o.num_threads));
    tcpsvr_threads thrs(o.num_threads, o.thread_stack_size, arg);
    thrs.start();
  }
  return 0;
}

typedef std::set<pid_t> pids_type;

int
concurrent_tcp_server_main_proc(tcp_server& svr,
  const tcp_server::options& o)
{
  using namespace gtcpp;
  tcpsvr_shared shared(svr, o);
  socket_listen(shared.get_file(), o.address_to_bind.c_str());
  shared.set_fd();
  if (o.num_procs <= 0) {
    DBG(fprintf(stderr, "no-fork\n"));
    return concurrent_tcp_server_worker_main(shared, o);
  }
  DBG(fprintf(stderr, "fork %d\n", o.num_procs));
  pids_type pids;
  int delay_sec = 1;
  while (true) {
    while (pids.size() < static_cast<size_t>(o.num_procs)) {
      const pid_t pid = ::fork();
      if (pid < 0) {
	struct timespec ts = { 0, 0 };
	ts.tv_sec = 1;
	ts.tv_nsec = 0;
	::nanosleep(&ts, 0);
      } else if (pid == 0) {
	concurrent_tcp_server_worker_main(shared, o);
	_exit(0);
      } else {
	DBG(fprintf(stderr, "fork: pid = %d\n", static_cast<int>(pid)));
	pids.insert(pid);
      }
    }
    int st = 0;
    const pid_t pid = ::waitpid(-1, &st, 0);
    if (pid > 0) {
      DBG(fprintf(stderr, "waitpid: pid = %d\n", static_cast<int>(pid)));
      pids.erase(pid);
    }
    if (WIFSIGNALED(st) && WCOREDUMP(st)) {
      DBG(fprintf(stderr, "pid = %d dumps core\n", static_cast<int>(pid)));
      struct timespec ts = { 0, 0 };
      ts.tv_sec = delay_sec;
      ts.tv_nsec = 0;
      ::nanosleep(&ts, 0);
      delay_sec <<= 1; /* no problem even if it overflows */
    }
  }
  return 0;
}

};

int
concurrent_tcp_server_main(tcp_server& svr, const tcp_server::options& o)
{
  if (o.block_sigpipe) {
    scoped_sigaction sa_pipe(SIGPIPE, false);
    return concurrent_tcp_server_main_proc(svr, o);
  } else {
    return concurrent_tcp_server_main_proc(svr, o);
  }
}

};

