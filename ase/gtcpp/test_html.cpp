
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/html.hpp>
#include <gtcpp/file_util.hpp>
#include <gtcpp/auto_iconv.hpp>
#include <iostream>

namespace {

using namespace gtcpp;

template <size_t n> void html_get_entity_code_one(const char (& c)[n])
{
  const int r = html_get_entity_code(c, n - 1);
  printf("%s: %d\n", c, r);
}

void test_html_entities()
{
  html_get_entity_code_one("amp");
  html_get_entity_code_one("hearts");
}

};

void test_html(int argc, char **argv)
{
  if (argc > 0) {
    string_buffer obuf;
    const std::string s(argv[0]);
    html_decode_entities(obuf, s.data(), s.data() + s.size());
    obuf.append_literal("\0");
    printf("%s\n", obuf.begin());
    #if 0
    const int r = html_get_entity_code(argv[0], std::strlen(argv[0]));
    printf("%s: %d\n", argv[0], r);
    #endif
  } else {
    test_html_entities();
  }
}

