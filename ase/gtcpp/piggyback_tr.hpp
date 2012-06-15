
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_PIGGYBACK_TR_HPP
#define GTCPP_PIGGYBACK_TR_HPP

#include <gtcpp/mutex.hpp>
#include <vector>

namespace gtcpp {

template <typename Tproc> struct piggyback_transaction;

struct piggyback_tr_base : public gtcpp::condition {

 private:

  template <typename Tproc> friend struct piggyback_transaction;

  piggyback_tr_base(size_t piggyback_max);
  virtual ~piggyback_tr_base();
  bool execute_piggyback_base(void *proc) volatile;

 private:

  virtual void begin_transaction() = 0;
  virtual bool commit_transaction() = 0;
  virtual void abort_transaction() = 0;
  virtual void execute_procedure_base(void *proc) = 0;

 private:

  struct impl_type;
  boost::scoped_ptr<impl_type> impl;

};

template <typename Tproc>
struct piggyback_transaction : public piggyback_tr_base {

  piggyback_transaction(size_t pgy_max) : piggyback_tr_base(pgy_max) { }
  bool execute_piggyback(Tproc& proc) volatile {
    return execute_piggyback_base(&proc);
  }

 private:

  virtual void execute_procedure(Tproc& proc) = 0;

 private:

  void execute_procedure_base(void *proc) {
    execute_procedure(*static_cast<Tproc *>(proc));
  }

};

};

#endif

