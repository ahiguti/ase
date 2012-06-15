
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/string_util.hpp>
#include <gtcpp/auto_thread.hpp>
#include <gtcpp/socket.hpp>
#include <gtcpp/debugcount.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <vector>

using namespace gtcpp;

void test_thr(int argc, char **argv);
void test_thrtiming(int argc, char **argv);
void test_ioall(int, char **);
void test_iovecall(int, char **);
void test_sock(int, char **);
void test_sockcli(int, char **);
void test_sockserv(int, char **);
void test_servsink(int, char **);
void test_servclose(int, char **);
void test_clisend(int, char **);
void test_smtpc(int, char **);
void test_smtpd(int, char **);
void test_sendmail(int, char **);
void test_bulkmail(int, char **);
void test_mailsvr(int, char **);
void test_emptymta(int, char **);
void test_smtppr(int, char **);
void test_dir(int, char **);
void test_resolver(int, char **);
void test_glob(int, char **);
void test_regex(int, char **);
void test_rfc822(int, char **);
void test_base64(int, char **);
void test_iconv(int, char **);
void test_received(int, char **);
void test_html(int, char **);
void test_zlib(int, char **);

struct testtbl_type {
  const char *name;
  void (*func)(int, char **);
} testtbl[] = {
  { "thr", test_thr },
  { "thrtiming", test_thrtiming },
  { "ioall", test_ioall },
  { "iovecall", test_iovecall },
  { "sockcli", test_sockcli },
  { "sockserv", test_sockserv },
  { "servsink", test_servsink},
  { "servclose", test_servclose },
  { "clisend", test_clisend },
  { "smtpc", test_smtpc },
  { "smtpd", test_smtpd },
  { "sendmail", test_sendmail },
  { "bulkmail", test_bulkmail },
  { "mailsvr", test_mailsvr },
  { "emptymta", test_emptymta },
  { "smtppr", test_smtppr },
  { "dir", test_dir },
  { "resolver", test_resolver },
  { "glob", test_glob },
  { "regex", test_regex },
  { "rfc822", test_rfc822 },
  { "base64", test_base64 },
  { "iconv", test_iconv },
  { "received", test_received },
  { "html", test_html },
  { "zlib", test_zlib },
};

#define STRINGIZE(x) STRINGIZE_(x)
#define STRINGIZE_(x) #x

int main(int argc, char **argv)
{
  int r = 0;
  try {
    const size_t testtbl_num = sizeof(testtbl) / sizeof(testtbl[0]);
    if (argc < 2) {
      std::cerr << "Usage: " << argv[0] << " TEST" << std::endl;
      std::cerr << "available tests are:" << std::endl;
      for (size_t i = 0; i < testtbl_num; ++i) {
	std::cerr << " " << testtbl[i].name;
      }
      std::cerr << std::endl;
      return 1;
    }
    std::string t = argv[1];
    argv += 2;
    argc -= 2;
    std::cerr << "Revision: " STRINGIZE(SVN_REVISION) << std::endl;
    std::cout << "Test: " << t << std::endl;
    size_t i;
    for (i = 0; i < testtbl_num; ++i) {
      if (std::string(testtbl[i].name) == t) {
	break;
      }
    }
    if (i < testtbl_num) {
      testtbl[i].func(argc, argv);
    } else {
      throw std::runtime_error("unknown test");
    }
  } catch (const std::exception& e) {
    std::cerr << "uncaught exception: " << e.what() << std::endl;
    r = 1;
  }
  dump_debugcount();
  return r;
}

