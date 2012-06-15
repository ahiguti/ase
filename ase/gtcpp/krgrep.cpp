
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/kr_varlen.hpp>
#include <gtcpp/auto_glob.hpp>
#include <gtcpp/file_util.hpp>
#include <gtcpp/string_util.hpp>
#include <set>
#include <iostream>

using namespace gtcpp;

namespace {

struct kr_param : public kr_varlen_default_param {
};

struct found_fobj {
  found_fobj(const std::string& fn, const string_buffer& haystack)
    : filename(fn), haystack_ref(haystack) { }
  bool operator ()(size_t offset, const std::string& ndl) {
    printf("%s:%zu %s\n", filename.c_str(), offset, ndl.c_str());
    return false; /* continue */
  }
  std::string filename;
  const string_buffer& haystack_ref;
};

void
krgrep_one(const kr_varlen<kr_param>& kr, const std::string& fn)
{
  posix_error_callback ec;
  string_buffer buf;
  read_file_contents(fn.c_str(), buf, ec);
  found_fobj fobj(fn, buf);
  kr.search(fobj, buf.begin(), buf.size());
}

void
krgrep(const std::string& wdfn, const std::vector<std::string>& files)
{
  posix_error_callback ec;
  typedef std::set<std::string> wdset_type;
  wdset_type wdset;
  read_string_set(wdfn.c_str(), wdset, ec);
  typedef kr_varlen<kr_param> kr_type;
  kr_type kr(8, wdset.size() * 2);
  for (wdset_type::const_iterator i = wdset.begin(); i != wdset.end(); ++i) {
    kr.register_needle(*i);
  }
  typedef std::vector<std::string> files_type;
  for (files_type::const_iterator i = files.begin(); i != files.end(); ++i) {
    krgrep_one(kr, *i);
  }
}

void
krgrep_main(int argc, char **argv)
{
  if (argc < 2) {
    throw std::runtime_error("usage: krgrep PATTERN_FILE FILES ...");
  }
  const std::string pattern_file(argv[1]);
  std::vector<std::string> files;
  for (int i = 2; i < argc; ++i) {
    const std::string glob_pattern(argv[i]);
    auto_glob gl;
    glob_error_callback gec;
    glob(gl, glob_pattern.c_str(), 0, gec);
    for (size_t i = 0; i < gl.get().gl_pathc; ++i) {
      files.push_back(std::string(gl.get().gl_pathv[i]));
    }
  }
  krgrep(pattern_file, files);
}

};

int main(int argc, char **argv)
{
  try {
    krgrep_main(argc, argv);
  } catch (const std::exception& e) {
    std::cerr << "uncaught exception: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}

