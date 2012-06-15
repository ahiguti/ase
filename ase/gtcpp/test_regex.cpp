
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/auto_regex.hpp>
#include <iostream>
#include <vector>

void test_regex(int argc, char **argv)
{
  if (argc < 2) {
    throw std::runtime_error("usage: regex PATTERN STR");
  }
  using namespace gtcpp;
  regex_error_callback ec;
  const char *const pattern = argv[0];
  const char *const str = argv[1];
  std::cerr << "pattern = [" << pattern << "]" << std::endl;
  std::cerr << "str = [" << str << "]" << std::endl;
  auto_regex re;
  re.compile(pattern, REG_EXTENDED, ec); 
  std::vector< ::regmatch_t > vec(5);
  re.exec(str, &vec[0], &vec[0] + vec.size(), 0, ec);
  for (size_t i = 0; i < vec.size(); ++i) {
    if (vec[i].rm_so < 0) {
      continue;
    }
    std::string s(str + vec[i].rm_so, str + vec[i].rm_eo);
    std::cerr << i << ": " << s << std::endl;
  }
  std::cerr << "done" << std::endl;
}

