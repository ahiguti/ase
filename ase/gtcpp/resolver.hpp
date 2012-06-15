
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_RESOLVER_HPP
#define GTCPP_RESOLVER_HPP

#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <vector>
#include <string>

namespace gtcpp {

class resolver : private boost::noncopyable {

 public:

  struct record {
    uint16_t rr_type;
    uint16_t rr_class;
    uint32_t rr_ttl;
    uint16_t mx_pref;
    std::string data;
    record() : rr_type(0), rr_class(0), rr_ttl(0), mx_pref(0) { }
  };

  resolver();
  ~resolver();
  int lookup(const char *name, int typ, std::vector<record>& rec_r);
    /* returns h_errno */

 private:

  struct impl_type;
  const boost::scoped_ptr<impl_type> impl;

};

};

#endif

