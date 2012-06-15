
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_AUTO_REGEX_HPP
#define GTCPP_AUTO_REGEX_HPP

#include <gtcpp/error.hpp>
#include <boost/noncopyable.hpp>
#include <sys/types.h>
#include <regex.h>

namespace gtcpp {

struct regex_error_callback;

class auto_regex : private boost::noncopyable {

 public:

  auto_regex();
  ~auto_regex();
  void reset();

  int compile(const char *pattern, int cflags, regex_error_callback& ec);
  int exec(const char *str, ::regmatch_t *match_begin,
    ::regmatch_t *match_end, int eflags, regex_error_callback& ec);

 private:

  ::regex_t re;
  bool compiled;

};

typedef error<auto_regex> regex_error;

void throw_regex_error(int e, const char *func) __attribute__((noreturn));

struct regex_error_callback {

  regex_error_callback() : last_regex_error(0), last_regex_error_func("") { }
  virtual ~regex_error_callback() { }
  virtual void on_regex_error(int e, const char *func) {
    if (e == REG_NOMATCH) {
      last_regex_error = e;
      last_regex_error_func = func;
    } else {
      throw_regex_error(e, func);
    }
  }

 private:

  int last_regex_error;
  const char *last_regex_error_func;

};

};

#endif

