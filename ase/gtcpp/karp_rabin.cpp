
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/karp_rabin.hpp>

using namespace gtcpp;

#if 0
void
test1()
{
  unsigned int p = 101;
  unsigned int p_pow = karp_rabin::stream_hash_pow(p, 15);
  const std::string s1("ab0123456789cde");
  const unsigned int sh1 = karp_rabin::stream_hash(p, s1.data(), s1.size());
  printf("%s %x\n", s1.c_str(), sh1);
  const std::string s2("b0123456789cdef");
  const unsigned int sh2 = karp_rabin::stream_hash(p, s2.data(), s2.size());
  printf("%s %x\n", s2.c_str(), sh2);
  const unsigned int shdiff = karp_rabin::stream_hash_diff(p, p_pow, sh1,
    'a', 'f');
  printf("- %x\n", shdiff);
}
#endif

struct kr_param : public karp_rabin_default_param {
  enum { needle_len = 6U };
};

void
test2()
{
  const char *needles[] = {
    "aaabbb",
    "abcdef",
    "ABCDEF",
    "xxxxxx",
  };
  typedef karp_rabin<kr_param> kr_type;
  kr_type kr(needles, 4, 13);
  const std::string haystack("0123456abcdef999aaaaabbbbbb");
  for (kr_type::matches m(kr, haystack.data(), haystack.size()); m; ++m) {
    unsigned int match_id = *m;
    unsigned int offset = m.get_offset();
    printf("match id=%u offset=%u\n", match_id, offset);
  }
}

int main()
{
  test2();
}

