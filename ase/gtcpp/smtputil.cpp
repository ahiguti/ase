
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/string_util.hpp>
#include <gtcpp/debugcount.hpp>
#include <gtcpp/error.hpp>
#include <iostream>

using namespace gtcpp;

void smtputil_sendmail(int argc, char **argv);
void smtputil_parallel(int argc, char **argv);
void smtputil_fill(int argc, char **argv);
void smtputil_bulkmail(int argc, char **argv);
void smtputil_mailsvr(int argc, char **argv);
void smtputil_emptymta(int argc, char **argv);
void smtputil_smtppr(int argc, char **argv);

struct cmdtbl_type {
  const char *name;
  void (*func)(int, char **);
} cmdtbl[] = {
  { "sendmail", smtputil_sendmail },
  { "parallel", smtputil_parallel},
  { "fill", smtputil_fill},
  { "bulkmail", smtputil_bulkmail },
  { "mailsvr", smtputil_mailsvr },
  { "emptymta", smtputil_emptymta },
  { "smtppr", smtputil_smtppr },
};

#define STRINGIZE(x) STRINGIZE_(x)
#define STRINGIZE_(x) #x

int
main(int argc, char **argv)
{
  int r = 0;
  const size_t cmdtbl_num = sizeof(cmdtbl) / sizeof(cmdtbl[0]);
  if (argc < 2) {
    std::cerr << argv[0] << " rev " STRINGIZE(SVN_REVISION) << std::endl;
    std::cerr << "Usage: " << argv[0] << " COMMAND" << std::endl;
    std::cerr << "available commands are:" << std::endl;
    for (size_t i = 0; i < cmdtbl_num; ++i) {
      std::cerr << " " << cmdtbl[i].name;
    }
    std::cerr << std::endl;
    return 1;
  }
  std::string t = argv[1];
  argv += 2;
  argc -= 2;
  size_t i;
  for (i = 0; i < cmdtbl_num; ++i) {
    if (std::string(cmdtbl[i].name) == t) {
      break;
    }
  }
  if (i < cmdtbl_num) {
    cmdtbl[i].func(argc, argv);
  } else {
    std::cerr << "unknown command: " << t << std::endl;
    return 1;
  }
  dump_debugcount();
  return r;
}

