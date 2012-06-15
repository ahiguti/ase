
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/resolver.hpp>
#include <gtcpp/string_util.hpp>
#include <stdexcept>
#include <iostream>

namespace {

std::string to_hex(const std::string& str)
{
  gtcpp::string_buffer buf;
  for (size_t i = 0; i < str.size(); ++i) {
    int c = static_cast<unsigned char>(str[i]);
    gtcpp::to_string_hexadecimal_zero(buf, c, 2);
  }
  return std::string(buf.begin(), buf.end());
}

};

void test_resolver(int argc, char **argv)
{
  if (argc < 2) {
    throw std::runtime_error("usage: resolve TYPE NAME");
  }
  using namespace gtcpp;
  const std::string typ_str(argv[0]);
  const char *const name = argv[1];
  resolver re;
  std::vector<resolver::record> rec;
  int typ_code = 0;
  if (typ_str == "A") {
    typ_code = 1;
  } else if (typ_str == "AAAA") {
    typ_code = 28;
  } else if (typ_str == "CNAME") {
    typ_code = 5;
  } else if (typ_str == "TXT") {
    typ_code = 16;
  } else if (typ_str == "MX") {
    typ_code = 15;
  } else if (typ_str == "ANY") {
    typ_code = 255;
  } else if (std::atoi(typ_str.c_str()) != 0) {
    typ_code = std::atoi(typ_str.c_str());
  } else {
    throw std::runtime_error("unknown type: " + typ_str);
  }
  const int r = re.lookup(name, typ_code, rec);
  std::cout << "resolver: " << r << std::endl;
  for (size_t i = 0; i < rec.size(); ++i) {
    const resolver::record& rr = rec[i];
    std::cout << "TTL=" << rr.rr_ttl << " ";
    switch (rr.rr_type) {
    case 15:
      std::cout << "MX " << rr.mx_pref << " " << rr.data << std::endl;
      break;
    case 16:
      std::cout << "TXT \"" << rr.data << "\" (" << to_hex(rr.data) << ")"
        << std::endl;
      break;
    case 5:
      std::cout << "CNAME " << rr.data << std::endl;
      break;
    case 2:
      std::cout << "NS " << rr.data << std::endl;
      break;
    case 1:
      std::cout << "A " << to_hex(rr.data) << std::endl;
      break;
    case 28:
      std::cout << "AAAA " << to_hex(rr.data) << std::endl;
      break;
    default:
      std::cout << "OTHER(" << rr.rr_type << ") " << to_hex(rr.data)
        << std::endl;
      break;
    }
  }
}

