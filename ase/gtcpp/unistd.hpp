
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_UNISTD_HPP
#define GTCPP_UNISTD_HPP

#include <string>
#include <gtcpp/posix_error.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace gtcpp {

std::string gethostname();
int stat(const char *fname, struct stat *sbuf, posix_error_callback& ec);
pid_t fork(posix_error_callback& ec);
int setpgid(pid_t pid, pid_t pgid, posix_error_callback& ec);
pid_t getpgid(pid_t pid, posix_error_callback& ec);

};

#endif

