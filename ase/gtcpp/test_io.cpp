
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/auto_thread.hpp>
#include <gtcpp/socket.hpp>
#include <gtcpp/file_util.hpp>
#include <gtcpp/debugcount.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <vector>

using namespace gtcpp;

namespace {

struct child {
  struct arg {
    std::string s;
    bool t;
  };
  child(const arg& a) : str(a.s), thr(a.t) { }
  void operator ()() {
    write(1, str.c_str(), str.size());
    if (thr) {
      throw std::string("x");
    }
  }
  std::string str;
  bool thr;
};

void test_thr_main(int argc, char **argv)
{
  bool thr = false;
  if (argc > 0 && atoi(argv[0])) {
    thr = true;
  }
  try {
    typedef boost::shared_ptr<auto_thread> thr_type;
    typedef std::vector<thr_type> thrs_type;
    thrs_type thrs(100);
    for (thrs_type::size_type i = 0; i < thrs.size(); ++i) {
      child::arg a = { "abc", thr };
      thr_type p(new threaded<child>(a));
      thrs[i] = p;
    }
    for (thrs_type::size_type i = 0; i < thrs.size(); ++i) {
      thrs[i]->start();
    }
    for (thrs_type::size_type i = 0; i < thrs.size(); ++i) {
      // thrs[i]->join();
    }
  } catch (...) {
    fprintf(stderr, "exc\n");
  }
  write(1, "\n", 1);
  return;
}

int dummy_seed = 100;
int dummy_block_max = 20;
int dummy_write_max = 100;
std::string dummy;
std::string dummy_out;

unsigned int my_rand_seed = 0;

void my_srand(int s)
{
  s = my_rand_seed;
}

int my_rand()
{
  my_rand_seed = my_rand_seed * 69069 + 1;
  return my_rand_seed & 0x7fffffffUL;
}

ssize_t dummy_io_func(int, void *buf, size_t count)
{
  size_t retlen = my_rand() % dummy_write_max + 1;
  retlen = std::min(retlen, count);
  std::string out;
  out += std::string(static_cast<char *>(buf), retlen);
  std::cout << "C: " << count << " B: " << out << std::endl;
  dummy_out += out;
  return retlen;
}

bool test_ioall_one(int s, int w, size_t len)
{
  dummy_seed = s;
  dummy_write_max = w;
  std::cout << "SEED=" << dummy_seed
    << " WR=" << dummy_write_max << std::endl;
  my_srand(dummy_seed);
  dummy.clear();
  dummy_out.clear();
  dummy.resize(len);
  for (size_t i = 0; i < len; ++i) {
    char c = 'a' + (my_rand() % 26);
    dummy[i] = c;
  }
  auto_file fd;
  posix_error_callback ec;
  size_t c = 0;
  io_function_all(dummy_io_func, fd, &dummy[0], dummy.size(), c,
    ec);
  std::cout << "I: " << dummy << std::endl;
  std::cout << "I: " << dummy_out << std::endl;
  if (dummy == dummy_out) {
    std::cout << "OK" << std::endl;
    return true;
  } else {
    std::cout << "FAILED" << std::endl;
    return false;
  }
}

ssize_t dummy_iovec_func(int, const ::iovec *vec, int count)
{
  const int count_orig = count;
  int retlen = my_rand() % dummy_write_max + 1;
  size_t len = retlen;
  std::string out;
  while (count > 0) {
    if (vec[0].iov_len >= len) {
      out += std::string(static_cast<char *>(vec[0].iov_base), len);
      len = 0;
      break;
    }
    out += std::string(static_cast<char *>(vec[0].iov_base),
      vec[0].iov_len);
    len -= vec[0].iov_len;
    --count;
    ++vec;
  }
  std::cout << "C: " << count << "/" << count_orig
    << " B: " << out << std::endl;
  dummy_out += out;
  return retlen;
}

bool test_iovecall_one(int s, int b, int w, size_t len)
{
  dummy_seed = s;
  dummy_block_max = b;
  dummy_write_max = w;
  std::cout << "SEED=" << dummy_seed << " BL=" << dummy_block_max
    << " WR=" << dummy_write_max << std::endl;
  my_srand(dummy_seed);
  dummy.clear();
  dummy_out.clear();
  dummy.resize(len);
  for (size_t i = 0; i < len; ++i) {
    char c = 'a' + (my_rand() % 26);
    dummy[i] = c;
  }
  std::vector< ::iovec > vec;
  for (size_t i = 0; i < len; ) {
    size_t b = my_rand() % dummy_block_max + 1;
    size_t n = std::min(len - i, b);
    ::iovec v;
    v.iov_base = &dummy[i];
    v.iov_len = n;
    vec.push_back(v);
    i += n;
  }
  auto_file fd;
  size_t c = 0;
  posix_error_callback ec;
  iovec_function_all(dummy_iovec_func, fd, &vec[0], vec.size(),
    c, ec);
  std::cout << "I: " << dummy << std::endl;
  std::cout << "I: " << dummy_out << std::endl;
  if (dummy == dummy_out) {
    std::cout << "OK" << std::endl;
    return true;
  } else {
    std::cout << "FAILED" << std::endl;
    return false;
  }
}

};

void
test_thr(int argc, char **argv)
{
  test_thr_main(argc, argv);
}

void test_ioall(int, char **)
{
  bool success = true;
  success &= test_ioall_one(12345, 20, 1000);
  success &= test_ioall_one(67890, 100, 10000);
  success &= test_ioall_one(11111, 300, 10);
  success &= test_ioall_one(99999, 300, 100000);
  if (!success) {
    throw std::runtime_error("test failed");
  }
}

void test_iovecall(int, char **)
{
  bool success = true;
  success &= test_iovecall_one(12345, 20, 100, 1000);
  success &= test_iovecall_one(67890, 100, 10, 10000);
  success &= test_iovecall_one(11111, 300, 300, 10);
  success &= test_iovecall_one(99999, 300, 300, 100000);
  if (!success) {
    throw std::runtime_error("test failed");
  }
}

