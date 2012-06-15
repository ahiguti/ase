
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/auto_thread.hpp>
#include <gtcpp/mutex.hpp>
#include <pthread.h>
#include <stdio.h>

using namespace gtcpp;

int test_thrtiming(int argc, char **)
{
  if (argc < 2) {
    ::printf("pthread\n");
    // pthread_mutex_t mtx = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
    pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_init(&mtx, 0);
    for (int i = 0; i < 100000000; ++i) {
      pthread_mutex_lock(&mtx);
      pthread_mutex_unlock(&mtx);
    }
    pthread_mutex_destroy(&mtx);
  } else {
    ::printf("gtcpp mutex\n");
    mutex m;
    for (int i = 0; i < 100000000; ++i) {
      scoped_lock<mutex> l(m);
    }
  }

  return 0;
}

