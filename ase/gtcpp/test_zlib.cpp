
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/zlib_util.hpp>
#include <gtcpp/file_util.hpp>
#include <string>
#include <iostream>

namespace {

using namespace gtcpp;

void test_zlib_one(const std::string& s, int level)
{
  string_buffer buf1, buf2;
  zlib_compress(buf1, s.data(), s.data() + s.size(), level);
  std::string x(buf1.begin(), buf1.end());
  zlib_uncompress(buf2, buf1.begin(), buf1.end());
  std::string r(buf2.begin(), buf2.end());
  if (s != r) {
    std::cout << "verification failed" << std::endl;
  } else {
    std::cout << "ok" << std::endl;
  }
  std::cout << s.size() << "->" << x.size() << std::endl;
}

};

void test_zlib(int argc, char **argv)
{
  std::string s;
  int level = 1;
  if (argc > 0) {
    if (argc > 1) {
      level = std::atoi(argv[1]);
    }
    gtcpp::string_buffer buf;
    gtcpp::posix_error_callback ec;
    read_file_contents(argv[0], buf, ec);
    s = std::string(buf.begin(), buf.end());
  } else {
    s = std::string(
      "abcdef-------------------------------------------------");
  }
  test_zlib_one(s, level);
}

