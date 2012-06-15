
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_POSIX_ERROR_HPP
#define GTCPP_POSIX_ERROR_HPP

#include <boost/scoped_ptr.hpp>
#include <gtcpp/error.hpp>

namespace gtcpp {

struct posix {
};

typedef error<posix> posix_error;

void throw_posix_error(int e, const char *func) __attribute__((noreturn));

struct posix_error_callback {

  virtual ~posix_error_callback() { }
  virtual void on_posix_error(int e, const char *func);

};

struct posix_error_callback_nothrow : public posix_error_callback {

  posix_error_callback_nothrow() : last_posix_error(0),
    last_posix_error_func("") { }
  virtual void on_posix_error(int e, const char *func) {
    last_posix_error = e;
    last_posix_error_func = func;
  }
  int get_last_posix_error() const { return last_posix_error; }
  const char *get_last_posix_error_func() const {
    return last_posix_error_func; }
  void clear_last_posix_error() {
    last_posix_error = 0;
    last_posix_error_func = 0;
  }
  void throw_last_posix_error() {
    const int e = last_posix_error;
    const char *const func = last_posix_error_func;
    clear_last_posix_error();
    throw_posix_error(e, func);
  }

 private:

  int last_posix_error;
  const char *last_posix_error_func;

};

// FIXME: remove
#if 0
struct posix_error_callback_catchlist : public posix_error_callback {

  posix_error_callback_catchlist(const int *list = 0, size_t len = 0);
  ~posix_error_callback_catchlist();
  /* throws if list is not empty and e is not in the list. returns
     otherwise. */
  virtual void on_posix_error(int e, const char *func);

  int get_last_error() const;
  const char *get_last_error_func() const;
  void clear_last_error();

 private:

  struct impl_type;
  boost::scoped_ptr<impl_type> impl;

};
#endif

};

#endif

