
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_TIME_HPP
#define GTCPP_TIME_HPP

#include <gtcpp/posix_error.hpp>
#include <sys/time.h>
#include <time.h>

namespace gtcpp {

int nanosleep(const struct timespec *req, struct timespec *rem,
  posix_error_callback& ec);
int nanosleep(time_t sec, long nsec, posix_error_callback& ec);
int gettimeofday(struct timeval *tv, posix_error_callback& ec);
int gettimeofday(double& time_r, posix_error_callback& ec);
double gettimeofday_double();

};

#endif

