
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/auto_glob.hpp>
#include <iostream>

void test_glob(int argc, char **argv)
{
  if (argc < 1) {
    throw std::runtime_error("usage: glob PATTERN");
  }
  using namespace gtcpp;
  glob_error_callback ec;
  const char *const pattern = argv[0];
  auto_glob gl;
  glob(gl, pattern, 0, ec);
  for (size_t i = 0; i < gl.get().gl_pathc; ++i) {
    std::cout << gl.get().gl_pathv[i] << std::endl;
  }
  std::cerr << "done" << std::endl;
}

