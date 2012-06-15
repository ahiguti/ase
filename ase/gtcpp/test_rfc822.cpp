
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/rfc822.hpp>
#include <gtcpp/base64.hpp>
#include <gtcpp/quotedpr.hpp>
#include <gtcpp/ms932.hpp>
#include <gtcpp/file_util.hpp>
#include <gtcpp/auto_iconv.hpp>
#include <iostream>

namespace {

using namespace gtcpp;

void test_rfc822_mem(const char *start, const char *finish)
{
  std::cerr << "hmdone" << std::endl;
  string_ref cte, ct, cs, bnd;
  const char *const bdy_start = rfc822_get_body(start, finish, cte, ct, cs,
    bnd);
  std::cerr << "gbdone" << std::endl;
  string_buffer dec;
  string_ref decref;
  if (cte.equals_strcase("base64")) {
    base64_decode(bdy_start, finish, dec);
    decref.set(dec.begin(), dec.end());
    /*
    std::string s(dec.begin(), dec.end());
    std::cerr << "b6=[" << s << "]" << std::endl;
    */
    std::cerr << "b6done" << std::endl;
  } else if (cte.equals_strcase("quoted-printable")) {
    quoted_printable_decode(bdy_start, finish, dec);
    decref.set(dec.begin(), dec.end());
    /*
    std::string s(dec.begin(), dec.end());
    std::cerr << "qp=[" << s << "]" << std::endl;
    */
    std::cerr << "qpdone" << std::endl;
  } else {
    decref.set(bdy_start, finish);
  }
  if (ct.size() > 10 && std::memcmp(ct.begin(), "multipart/", 10) == 0) {
    std::vector<string_ref> parts;
    string_ref plain;
    if (!rfc822_extract_multipart(decref.begin(), decref.end(), bnd, parts,
      plain)) {
      std::cerr << "mpdone: failed" << std::endl;
    } else {
      std::cerr << "mpdone: " << parts.size() << std::endl;
      for (size_t i = 0; i < parts.size(); ++i) {
	// std::cerr << "part[" << parts[i].to_stdstring() << "]" << std::endl;
	test_rfc822_mem(parts[i].begin(), parts[i].end());
      }
    }
  } else {
    std::cerr << "body[" << decref.to_stdstring() << "]" << std::endl;
  }
}

};

void test_rfc822(int argc, char **argv)
{
  if (argc < 1) {
    throw std::runtime_error("usage: rfc822 FILENAME");
  }
  posix_error_callback ec;
  string_buffer buf;
  read_file_contents(argv[0], buf, ec);
  /*
  std::map<std::string, std::string> m;
  rfc822_headers_map(buf.begin(), buf.end(), m);
  */
  test_rfc822_mem(buf.begin(), buf.end());
}

namespace {

void test_base64_one(const std::string& s)
{
  string_buffer dec;
  base64_decode(s.data(), s.data() + s.size(), dec);
  std::string sd(dec.begin(), dec.end());
  std::cerr << "b6=[" << sd << "]" << std::endl;
}

};

void test_base64(int, char **)
{
  test_base64_one("bGVhc3VyZS4="); /* "leasure." */
  test_base64_one("ZWFzdXJlLg=="); /* "easure." */
  test_base64_one("YXN1cmUu");     /* "asure." */
  test_base64_one("c3VyZS4=");     /* "sure." */
}

void test_iconv(int argc, char **argv)
{
  if (argc < 1) {
    throw std::runtime_error("usage: iconv FILENAME");
  }
  posix_error_callback ec;
  string_buffer buf1, buf2, buf3;
  read_file_contents(argv[0], buf1, ec);
  iso2022jp_to_ms932(buf2, buf1.begin(), buf1.end());
  iconv("UTF8", "MS932", buf3, buf2.begin(), buf2.end());
  fwrite(buf3.begin(), buf3.size(), 1, stdout);
}

namespace {

void test_received_one(const std::string& s)
{
  unsigned long addr = 0;
  string_ref r = rfc822_extract_receivedfrom_ipaddr(s.data(),
    s.data() + s.size(), addr);
  printf("%s %s %08lx\n", r.to_stdstring().c_str(), s.c_str(), addr);
}

};

void test_received(int, char **)
{
  test_received_one("from (1.2.3.4)");
  test_received_one("from a(b.c) (5.6.7.8 9.8.7.6)");
  test_received_one("from 1.2.3.4 (5.6.7.8 9.8.7.6)");
  test_received_one("from 1.2.3.4 (5.6.7.8)");
  test_received_one("from 1.2.3.4");
  test_received_one("from 1.2.3.4 ([5.6.7.8] 9.8.7.6)");
  test_received_one("from 1.2.3.4 (5.6.7.8 [9.8.7.6])");
  test_received_one("from 1.2.3.4 (unknown [9.8.7.6])");
  test_received_one("from");
  test_received_one("from 1.2.3.4 (5.6.7.8 unknown)");
  test_received_one("from 1.2.3.4 (5.6.7.8)(9.9.9.9)");
  test_received_one("foobar xyz from 1.2.3.4 (5.6.7.8)(9.9.9.9)");
  test_received_one("foobar xyz from 1.2.3.4 (5.6.7.8)(9.9.9.9) by (1.1.1.1)");
}

