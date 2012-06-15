
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/kr_varlen.hpp>

using namespace gtcpp;

struct kr_param : public kr_varlen_default_param {
  enum { needle_len = 6U };
};

struct found_fobj {
  found_fobj(const std::string& s) : sref(s) { }
  bool operator ()(size_t offset, const std::string& ndl) {
    printf("match s=%s offset=%d\n", ndl.c_str(), (int)offset);
    return false; /* continue */
  }
  const std::string& sref;
};

void
test()
{
  typedef kr_varlen<kr_param> kr_type;
  kr_type kr(6, 13);
  kr.register_needle("aaabbbc");
  kr.register_needle("aaabbbb");
  kr.register_needle("abcdef");
  kr.register_needle("ABCDEF");
  kr.register_needle("xxxxxx");
  kr.register_needle("xxxxxx"); /* dup */
  kr.register_needle("56abcdef99");
  const std::string haystack("0123456abcdef999aaaaabbbbbb");
  found_fobj fobj(haystack);
  kr.search(fobj, haystack.data(), haystack.size());
  printf("size=%zd buckets=%zd\n", kr.size(), kr.get_buckets_size());
  kr.resize_buckets(1000);
  kr.search(fobj, haystack.data(), haystack.size());
  printf("size=%zd buckets=%zd\n", kr.size(), kr.get_buckets_size());
}

int main()
{
  test();
}

