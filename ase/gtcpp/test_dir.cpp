
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/auto_dir.hpp>
#include <iostream>

void test_dir(int argc, char **argv)
{
  if (argc < 1) {
    throw std::runtime_error("usage: dir PATH");
  }
  using namespace gtcpp;
  posix_error_callback ec;
  const char *const name = argv[0];
  auto_dir dir;
  ::dirent *e = 0;
  opendir(dir, name, ec);
  while (readdir(dir, e, ec) > 0) {
    std::string s = e->d_name;
    std::cout << "D: " << s << std::endl;
  }
  std::cerr << "done" << std::endl;
}

