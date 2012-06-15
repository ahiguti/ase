
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/time.hpp>
#include <gtcpp/auto_file.hpp>
#include <gtcpp/posix_error.hpp>
#include <errno.h>

namespace gtcpp {

int
nanosleep(const struct timespec *req, struct timespec *rem,
  posix_error_callback& ec)
{
  int r = ::nanosleep(req, rem);
  GTCPP_CHECK_POSIX_ERROR_NEGATIVE(r, ec);
}

int
nanosleep(time_t sec, long nsec, posix_error_callback& ec)
{
  struct timespec ts;
  ts.tv_sec = sec;
  ts.tv_nsec = nsec;
  return gtcpp::nanosleep(&ts, 0, ec);
}

int
gettimeofday(struct timeval *tv, posix_error_callback& ec)
{
  const int r = ::gettimeofday(tv, 0);
  GTCPP_CHECK_POSIX_ERROR_NEGATIVE(r, ec);
}

int
gettimeofday(double& time_r, posix_error_callback& ec)
{
  time_r = 0;
  struct timeval tv;
  const int r = ::gettimeofday(&tv, 0);
  if (r < 0) {
    ec.on_posix_error(errno, __PRETTY_FUNCTION__);
    return r;
  }
  const double sec = tv.tv_sec;
  const double usec = tv.tv_usec;
  time_r = sec + (usec / 1000000);
  return r;
}

double
gettimeofday_double()
{
  double r = 0;
  posix_error_callback ec;
  gtcpp::gettimeofday(r, ec);
  return r;
}

};

