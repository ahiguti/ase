
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_ERROR_HPP
#define GTCPP_ERROR_HPP

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <stdexcept>
#include <typeinfo>
#include <string>

namespace gtcpp {

class error_base : public std::exception {

 public:

  explicit error_base(int code, const char *kind, const char *errstr,
    const char *func);
  virtual ~error_base() throw ();
  error_base(const error_base& x);
  error_base& operator =(const error_base& x);
  virtual const char *what() const throw ();
  int get_code() const;
  const std::string& get_kind() const;
  const std::string& get_errstr() const;
  const std::string& get_func() const;

 private:

  struct impl_type;
  boost::scoped_ptr<impl_type> impl;

};

template <typename T>
class error : public error_base {
 
 public:

  explicit error(int e, const char *kind, const char *estr, const char *func)
    : error_base(e, kind, estr, func) { }

};

// FIXME: remove
#if 0
template <typename T> inline T to_negative(T x) {
  return x < 0 ? x : -x;
}
#endif

};

#endif

