
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/buffered_file.hpp>
#include <gtcpp/file_util.hpp>
#include <gtcpp/auto_glob.hpp>
#include <gtcpp/posix_error.hpp>
#include <gtcpp/karp_rabin.hpp>
#include <map>
#include <set>
#include <list>
#include <string>
#include <memory>
#include <iostream>

using namespace gtcpp;

struct pattern {

  typedef karp_rabin<> kr_type;
  enum { needle_len = kr_type::needle_len };

  pattern() { }
  void register_needles(const char *start, const char *finish) {
    if (kr.get()) {
      return;
    }
    const size_t len = finish - start;
    for (size_t i = 0; i + needle_len <= len; i += needle_len) {
      const size_t num_codes = calc_num_codes(start + i,
	start + i + needle_len);
      if (num_codes < needle_len / 2) {
	// printf("noreg nc=%d\n", (int)num_codes);
	continue;
      }
      // printf("reg nc=%d\n", (int)num_codes);
      needles.push_back(start + i);
    }
  }
  static size_t calc_num_codes(const char *start, const char *finish) {
    std::set<unsigned char> cds;
    for (const char *p = start; p != finish; ++p) {
      unsigned char c = static_cast<unsigned char>(*p);
      cds.insert(c);
    }
    return cds.size();
  }
  void search(const std::string& fname, const char *start,
    const char *finish) {
    if (needles.empty()) {
      return;
    }
    init_kr();
    for (kr_type::matches m(*kr, start, finish - start); m; ++m) {
      const unsigned int id = *m;
      const unsigned int offset = m.get_offset();
      printf("%s %u %u\n", fname.c_str(), id, offset);
      // std::string s(start + offset, start + offset + needle_len);
      // printf("%s %u %u [%s]\n", fname.c_str(), id, offset, s.c_str());
    }
  }
  void init_kr() {
    if (kr.get()) {
      return;
    }
    if (needles.empty()) {
      return;
    }
    kr.reset(new kr_type(&needles[0], needles.size(), needles.size() * 4));
    stat();
  }

  void stat() {
    #if 0
    kr_type::hashval_type hts = 0, pn = 0;
    int bkt = 0;
    kr->get_statistics(pn, hts, bkt);
    printf("pn=%lu htsize=%lu bkt=%d\n",
      (unsigned long)pn, (unsigned long)hts, bkt);
    #endif
  }

  std::auto_ptr<kr_type> kr;
  std::vector<const char *> needles;

};

void
find_resemble(const char *needles_pat, const char *haystack_pat)
{
  posix_error_callback ec;
  glob_error_callback gec;
  std::list<std::string> strs;
  pattern pat;
  {
    auto_glob gl;
    glob(gl, needles_pat, 0, gec);
    for (size_t i = 0; i < gl.get().gl_pathc; ++i) {
      const char *const fname = gl.get().gl_pathv[i];
      string_buffer sb;
      try {
	read_file_contents(fname, sb, ec);
	strs.push_back(std::string(sb.begin(), sb.end()));
	const std::string& s = strs.back();
	pat.register_needles(s.data(), s.data() + s.size());
      } catch (const posix_error& e) {
	/* ignore */
      }
    }
  }
  {
    auto_glob gl;
    glob(gl, haystack_pat, 0, gec);
    for (size_t i = 0; i < gl.get().gl_pathc; ++i) {
      const char *const fname = gl.get().gl_pathv[i];
      string_buffer sb;
      try {
	read_file_contents(fname, sb, ec);
	pat.search(std::string(fname), sb.begin(), sb.end());
      } catch (const posix_error& e) {
	/* ignore */
      }
    }
  }
}

int
main(int argc, char **argv)
{
  if (argc < 3) {
    printf("Usage: %s NEEDLE_GLOB_PATTERN HAYSTACK_GLOB_PATTERN\n", argv[0]);
    return 1;
  }
  try {
    find_resemble(argv[1], argv[2]);
  } catch (const std::exception& e) {
    std::cerr << "uncaught exception: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}

