
#include <gtcpp/piggyback_tr.hpp>
#include <cstdio>

#define DBG(x)

namespace gtcpp {

struct piggyback_tr_base::impl_type {

  struct entry_type {
    void *proc;
    bool done_flag : 1;
    bool success : 1;
    entry_type() : proc(0), done_flag(false), success(false) { }
  };
  typedef std::vector<entry_type *> queue_type;

  struct rewind_hook {
    rewind_hook(volatile piggyback_tr_base& r, queue_type& qr)
      : ref(r), q_exec_ref(qr), need_abort(false) { }
    ~rewind_hook() throw();
    volatile piggyback_tr_base& ref;
    queue_type& q_exec_ref;
    bool need_abort;
  };

  queue_type q_waiting;
  bool executing_flag;
  size_t piggyback_max;

  impl_type() : executing_flag(false), piggyback_max(0) { }

};

piggyback_tr_base::impl_type::rewind_hook::~rewind_hook() throw()
{
  scoped_lock<piggyback_tr_base> lck(ref);
  if (need_abort) {
    lck->abort_transaction();
  }
  impl_type& impl = *lck->impl;
  for (impl_type::queue_type::const_iterator i = q_exec_ref.begin();
    i != q_exec_ref.end(); ++i) {
    (*i)->done_flag = true;
  }
  impl.executing_flag = false;
  lck.notify_all();
}

piggyback_tr_base::piggyback_tr_base(size_t piggyback_max)
  : impl(new impl_type())
{
  impl->piggyback_max = piggyback_max;
}

piggyback_tr_base::~piggyback_tr_base()
{
}

bool
piggyback_tr_base::execute_piggyback_base(void *proc) volatile
{
  impl_type::queue_type q_executing;
  impl_type::entry_type ent;
  ent.proc = proc;
  {
    scoped_lock<piggyback_tr_base> lck(*this);
    impl_type& im = *lck->impl;
    bool queued_flag = false;
    while (true) {
      if (!queued_flag && im.q_waiting.size() < im.piggyback_max) {
	/* not queued yet. */
	im.q_waiting.push_back(&ent);
	queued_flag = true;
      }
      if (queued_flag) {
	/* already queued. try to obtain executing_flag */
	if (ent.done_flag) {
	  /* already executed (or aborted) by another thread */
	  return ent.success;
	}
	if (!im.executing_flag) {
	  /* we can execute a transaction now */
	  break;
	}
      }
      lck.wait();
    }
    im.executing_flag = true;
    swap(im.q_waiting, q_executing);
    /* lck is unlocked */
  }
  /* assert(impl->executing_flag) */
  impl_type::rewind_hook h(*this, q_executing);
  {
    piggyback_tr_base *const p = const_cast<piggyback_tr_base *>(this);
    p->begin_transaction();
    h.need_abort = true;
    DBG(std::fprintf(stderr, "piggy %d\n",
      static_cast<int>(q_executing.size())));
    for (impl_type::queue_type::const_iterator i = q_executing.begin();
      i != q_executing.end(); ++i) {
      p->execute_procedure_base((*i)->proc);
    }
    if (p->commit_transaction()) {
      for (impl_type::queue_type::const_iterator i = q_executing.begin();
        i != q_executing.end(); ++i) {
        (*i)->success = true;
      }
    }
    h.need_abort = false;
  }
  return ent.success;
}

};

