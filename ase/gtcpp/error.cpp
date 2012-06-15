
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/error.hpp>
#include <gtcpp/debugcount.hpp>
#include <string>
#include <sstream>
#include <errno.h>

namespace gtcpp {

int error_constr_count = 0;
int error_destr_count = 0;

struct error_base::impl_type {
  impl_type() : code(0) {
    #ifndef NDEBUG
    atomic_xadd(error_constr_count, 1);
    #endif
  }
  ~impl_type() {
    #ifndef NDEBUG
    atomic_xadd(error_destr_count, 1);
    #endif
  }
  int code;
  std::string kind;
  std::string errstr;
  std::string func;
  std::string message;
};

error_base::error_base(int code, const char *kind, const char *errstr,
  const char *func)
  : impl(new impl_type())
{
  impl->code = code;
  impl->kind = std::string(kind);
  impl->errstr = std::string(errstr);
  impl->func = std::string(func);
  std::stringstream ss;
  ss << impl->kind << "(" << code << ", " << impl->errstr << ") at "
    << impl->func << ")";
  impl->message = ss.str();
}

error_base::~error_base() throw ()
{
}

error_base::error_base(const error_base& x)
  : std::exception(x), impl(new impl_type(*x.impl))
{
}

error_base&
error_base::operator =(const error_base& x)
{
  if (this != &x) {
    boost::scoped_ptr<impl_type> p(new impl_type(*x.impl));
    impl.swap(p);
  }
  return *this;
}

const char *
error_base::what() const throw ()
{
  return impl->message.c_str();
}

int
error_base::get_code() const
{
  return impl->code;
}

const std::string&
error_base::get_kind() const
{
  return impl->kind;
}

const std::string&
error_base::get_errstr() const
{
  return impl->errstr;
}

const std::string&
error_base::get_func() const
{
  return impl->func;
}

};

