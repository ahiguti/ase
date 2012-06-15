
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <openssl/crypto.h>
#include <pthread.h>

namespace gtcpp {

namespace {

pthread_mutex_t *openssl_locks = 0;

unsigned long
openssl_mt_id_callback()
{
  return static_cast<unsigned long>(pthread_self());
}

void
openssl_mt_locking_callback(int mode, int typ, const char *, int)
{
  if ((mode & CRYPTO_LOCK) != 0) {
    pthread_mutex_lock(openssl_locks + typ);
  } else {
    pthread_mutex_unlock(openssl_locks + typ);
  }
}

};

void
openssl_mt_initialize()
{
  if (openssl_locks != 0) {
    ::abort();
  }
  const int nlcks = CRYPTO_num_locks();
  openssl_locks = new pthread_mutex_t[nlcks];
  for (int i = 0; i < nlcks; ++i) {
    pthread_mutex_init(openssl_locks + i, 0);
  }
  CRYPTO_set_id_callback(openssl_mt_id_callback);
  CRYPTO_set_locking_callback(openssl_mt_locking_callback);
}

void
openssl_mt_terminate()
{
  const int nlcks = CRYPTO_num_locks();
  if (openssl_locks != 0) {
    for (int i = 0; i < nlcks; ++i) {
      pthread_mutex_destroy(openssl_locks + i);
    }
  }
  delete [] openssl_locks;
  openssl_locks = 0;
}

};

