
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_AUTO_GLOB_HPP
#define GTCPP_AUTO_GLOB_HPP

#include <gtcpp/error.hpp>
#include <boost/noncopyable.hpp>
#include <glob.h>

namespace gtcpp {

struct glob_error_callback;

class auto_glob : private boost::noncopyable {

 public:

  auto_glob();
  ~auto_glob();
  const ::glob_t& get() const { return globbuf; }
  void reset();

  friend int glob(auto_glob& gl, const char *pattern, int flags,
    glob_error_callback& ec);

 private:

  ::glob_t globbuf;

};

int glob(auto_glob& gl, const char *pattern, int flags,
  glob_error_callback& ec);

typedef error<auto_glob> glob_error;

void throw_glob_error(int e, const char *func) __attribute__((noreturn));

struct glob_error_callback {

  glob_error_callback() : last_glob_error(0), last_glob_error_func("") { }
  virtual ~glob_error_callback() { }
  virtual void on_glob_error(int e, const char *func) {
    if (e == GLOB_NOMATCH) {
      last_glob_error = e;
      last_glob_error_func = func;
    } else {
      throw_glob_error(e, func);
    }
  }

 private:

  int last_glob_error;
  const char *last_glob_error_func;
  

};

};

#endif

