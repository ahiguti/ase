
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <vector>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <set>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <ase/asermt_concurrent.hpp>
#include <ase/aseexcept.hpp>

#define DBG(x)

namespace {

class workers_shared : public ase_fast_mutex {

 public:

  workers_shared(ase_io_listener& lsn, ase_unified_engine_ref& eng,
    const ase_variant& topobj, const ase_remote_concurrent_server_options& o)
    : listener(lsn), engine(eng), topobject(topobj), serializer(0), opts(o),
      shut_flag(false) {
    serializer = ase_serializer_table_get(opts.serializer);
  }
  void set_shut() {
    shut_flag = true;
  }
  void accept(ase_auto_ptr<ase_io_stream>& conn_r,
    ase_sockaddr_generic& addr_r) {
    conn_r.reset();
    while (true) {
      try {
	listener.accept(conn_r, addr_r);
      } catch (...) {
	/* ignore */
      }
      if (shut_flag) {
	conn_r.reset();
	return;
      }
      if (conn_r.get() != 0) {
	return;
      }
      /* retry */
      timespec ts = { };
      ts.tv_sec = 1;
      ts.tv_nsec = 0;
      ::nanosleep(&ts, 0);
    }
  }

  ase_io_listener& listener;
  ase_unified_engine_ref& engine;
  ase_variant topobject;
  const ase_serializer_table *serializer;
  ase_remote_concurrent_server_options opts;
  volatile bool shut_flag;

};

struct worker_thread : private ase_noncopyable {

  worker_thread(workers_shared& sh) : shared(sh) { }
  ~worker_thread() { }
  static void thr_main(void *p) {
    worker_thread *const cthis = static_cast<worker_thread *>(p);
    workers_shared& shared = cthis->shared;
    ase_sockaddr_generic cliaddr;
    shared.engine.child_thread_initialized();
    while (true) {
      ase_auto_ptr<ase_io_stream> conn;
      shared.accept(conn, cliaddr);
      if (conn.get() == 0) {
	break; /* shutdown */
      }
      try {
	ase_auto_ptr<ase_io_buffered_stream> bufconn;
	ase_io_buffered_stream_create(conn, bufconn);
	ase_remote_server_execute(bufconn, shared.topobject,
	  shared.serializer);
      } catch (...) {
	/* ignore */
	const ase_variant v = ase_variant::create_from_active_exception();
	DBG(fprintf(stderr, "%s\n", ase_nothrow_get_string(v).data()));
      }
    }
    shared.engine.child_thread_terminated();
  }

  ase_auto_ptr<ase_io_thread> thr;
  workers_shared& shared;

};

template <typename T> struct ptr_array : private ase_noncopyable {

  ptr_array(size_t sz) : data(sz) { }
  ~ptr_array() {
    for (size_t i = 0; i < data.size(); ++i) {
      delete data[i];
    }
  }
  T *& operator [](size_t idx) { return data[idx]; }
  const T *operator [](size_t idx) const { return data[idx]; }
  size_t size() const { return data.size(); }

  typedef size_t size_type;

 private:

  std::vector<T *> data;

};

int
ase_remote_concurrent_server_worker_main(workers_shared& shared,
  const ase_remote_concurrent_server_options& o)
{
  if (o.num_threads <= 0) {
    DBG(fprintf(stderr, "no-threading\n"));
    worker_thread thr(shared);
    thr.thr_main(&thr);
  } else {
    DBG(fprintf(stderr, "threading %d\n", o.num_threads));
    typedef ptr_array<worker_thread> thrs_type;
    thrs_type thrs(o.num_threads);
    for (int i = 0; i < o.num_threads; ++i) {
      thrs[i] = new worker_thread(shared);
      ase_io_thread_create(thrs[i]->thr);
      thrs[i]->thr->start(o.thread_stack_size, worker_thread::thr_main,
	thrs[i]);
    }
  }
  return 0;
}

typedef std::set<pid_t> pids_type;

int
ase_remote_concurrent_server_main_proc(ase_io_listener& lsn,
  ase_unified_engine_ref& eng, const ase_variant& topobj,
  const ase_remote_concurrent_server_options& o)
{
  workers_shared shared(lsn, eng, topobj, o);
  lsn.listen(o.address_to_bind);
  if (o.num_procs <= 0) {
    DBG(fprintf(stderr, "no-fork\n"));
    return ase_remote_concurrent_server_worker_main(shared, o);
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
	ase_remote_concurrent_server_worker_main(shared, o);
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
ase_remote_concurrent_server_main(ase_io_listener& lsn,
  ase_unified_engine_ref& eng, const ase_variant& topobj,
  const ase_remote_concurrent_server_options& o)
{
  return ase_remote_concurrent_server_main_proc(lsn, eng, topobj, o);
}

