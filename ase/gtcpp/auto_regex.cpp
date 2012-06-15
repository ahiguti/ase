
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/auto_regex.hpp>

namespace gtcpp {

auto_regex::auto_regex()
  : compiled(false)
{
}

auto_regex::~auto_regex()
{
  reset();
}

void
auto_regex::reset()
{
  if (compiled) {
    ::regfree(&re);
    compiled = false;
  }
}

int
auto_regex::compile(const char *pattern, int flags, regex_error_callback& ec)
{
  reset();
  int r = ::regcomp(&re, pattern, flags);
  if (r != 0) {
    ec.on_regex_error(r, __PRETTY_FUNCTION__);
  } else {
    compiled = true;
  }
  return r;
}

int
auto_regex::exec(const char *str, ::regmatch_t *match_begin,
  ::regmatch_t *match_end, int eflags, regex_error_callback& ec)
{
  assert(compiled);
  int r = ::regexec(&re, str, match_end - match_begin, match_begin, eflags);
  if (r != 0) {
    ec.on_regex_error(r, __PRETTY_FUNCTION__);
  }
  return r;
}

void
throw_regex_error(int e, const char *func)
{
  const char *mess;
  switch (e) {
  case REG_NOMATCH:
    mess = "NOMATCH";
    break;
  case REG_BADBR:
    mess = "BADBR";
    break;
  case REG_BADPAT:
    mess = "BADPAT";
    break;
  case REG_BADRPT:
    mess = "BADRPT";
    break;
  case REG_EBRACE:
    mess = "EBRACE";
    break;
  case REG_EBRACK:
    mess = "EBRACK";
    break;
  case REG_ECOLLATE:
    mess = "ECOLLATE";
    break;
  case REG_ECTYPE:
    mess = "ECTYPE";
    break;
  case REG_EESCAPE:
    mess = "EESCAPE";
    break;
  case REG_EPAREN:
    mess = "EPAREN";
    break;
  case REG_ERANGE:
    mess = "ERANGE";
    break;
  case REG_ESIZE:
    mess = "ESIZE";
    break;
  case REG_ESPACE:
    mess = "ESPACE";
    break;
  case REG_ESUBREG:
    mess = "ESUBREG";
    break;
  default:
    mess = "UNKNOWN";
  }
  throw regex_error(e, "RegExError", mess, func);
}

};

