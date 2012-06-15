
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/aho_corasick.hpp>
#include <gtcpp/karp_rabin.hpp>
#include <gtcpp/horspool.hpp>

#include <boost/shared_ptr.hpp>
#include <gtcpp/auto_thread.hpp>
#include <gtcpp/mutex.hpp>

#include <set>
#include <iostream>
#include <sys/time.h>

#define DBG_TESTDATA(x)

namespace {

using namespace gtcpp;

unsigned int my_rand_seed = 0;

void my_srand(int s)
{
  s = my_rand_seed;
}

unsigned int my_rand(unsigned int max)
{
  my_rand_seed = my_rand_seed * 69069 + 1;
  const unsigned int v = my_rand_seed & 0x7fffffffU;
  const unsigned int x = 0x80000000U / max;
  const unsigned int y = v / x;
  return y % max;
}

std::string random_ascii_string(size_t len, int num_chars = 26)
{
  std::string s;
  s.resize(len);
  for (size_t i = 0; i < len; ++i) {
    char c = 'a' + (my_rand(num_chars));
    s[i] = c;
  }
  return s;
}

struct search_callback {

  virtual ~search_callback() { }
  virtual bool on_match(size_t end_offset, int id) = 0;

};

struct testdata : public search_callback, public mutex {

  std::vector<std::string> needles;
  std::set<std::string> needles_set;
  size_t needles_totallen;
  std::vector<std::string> strings;
  bool dump_flag;
  size_t cur_haystack;
  int match_count;

  testdata(bool dump, size_t num_needles, size_t num_haystacks,
    size_t haystack_len, int num_chars, int len_base, int len_rand)
    : needles_totallen(0), dump_flag(dump),
      cur_haystack(0), match_count(0) {
    std::cout << "creating needles: num_chars=" << num_chars << std::endl;
    for (size_t i = 0; i < num_needles; ++i) {
      int len = my_rand(len_rand) + len_base;
      // int len = 3;
      std::string s = random_ascii_string(len, num_chars);
      if (needles_set.find(s) != needles_set.end()) {
	continue;
      }
      DBG_TESTDATA(std::cout << "NDL: " << s << std::endl);
      needles.push_back(s);
      needles_set.insert(s);
      needles_totallen += len;
    }
    for (size_t i = 0; i < num_haystacks; ++i) {
      std::string s = random_ascii_string(haystack_len, num_chars);
      strings.push_back(s);
    }
  }

  void run_iter(aho_corasick& ac) {
    for (size_t i = 0; i < strings.size(); ++i) {
      const std::string& s = strings[i];
      if (dump_flag) {
	std::cout << "STR " << s << std::endl;
      }
      const char *const haystack = s.data();
      const unsigned int haystack_len = s.size();
      aho_corasick::state st(ac);
      for (size_t i = 0; i < haystack_len; ++i) {
	if (st.next(haystack[i])) {
	  for (aho_corasick::matches m(st); m; ++m) {
	    const unsigned int match_id = *m;
	    if (dump_flag) {
	      const std::string& k = needles[match_id];
	      match_type m(i + 1 - k.size(), k);
	      matches.insert(m);
	    }
	    ++match_count;
	  }
	}
      }
      dump_matches();
    }
    dump_count();
  }

  struct karp_rabin_param : public karp_rabin_default_param {
    enum { needle_len = 5 };
  };
  typedef karp_rabin<karp_rabin_param> kr_type;

  void run_karp(const kr_type& kr) {
    for (size_t i = 0; i < strings.size(); ++i) {
      const std::string& s = strings[i];
      if (dump_flag) {
	std::cout << "STR " << s << std::endl;
      }
      for (kr_type::matches m(kr, s.data(), s.size()); m; ++m) {
	const unsigned int match_id = *m;
	const unsigned int offset = m.get_offset();
	if (dump_flag) {
	  const std::string& k = needles[match_id];
	  match_type m(offset, k);
	  // matches.insert(m);
	}
	++match_count;
      }
      dump_matches();
    }
    dump_count();
  }

  const std::string *get_next_haystack() volatile {
    scoped_lock<testdata> lck(*this);
    if (lck->cur_haystack == lck->strings.size()) {
      return 0;
    }
    const size_t i = lck->cur_haystack++;
    return &lck->strings[i];
  }

  struct karp_thr {
    struct arg_type {
      arg_type(testdata& td, const kr_type& kr)
	: td(td), tdconst(td), kr(kr) { }
      volatile testdata& td;
      const testdata& tdconst;
      const kr_type& kr;
    };
    arg_type arg;
    int count;
    karp_thr(const arg_type& arg) : arg(arg), count(0) { }
    void operator ()() {
      int cnt = 0;
      while (true) {
	const std::string *s = arg.td.get_next_haystack();
	if (!s) {
	  break;
	}
	for (kr_type::matches m(arg.kr, s->data(), s->size()); m; ++m) {
	  const unsigned int match_id = *m;
	  const unsigned int offset = m.get_offset();
	  if (arg.td.dump_flag) {
	    const std::string& k = arg.tdconst.needles[match_id];
	    match_type m(offset, k);
	    // matches.insert(m);
	  }
	  ++cnt;
	}
      }
      count = cnt;
    }
  };


  void run_karp_mt(const kr_type& kr, int num_threads) {
    typedef boost::shared_ptr< threaded<karp_thr> > thr_type;
    typedef std::vector<thr_type> thrs_type;
    thrs_type thrs(num_threads);
    for (thrs_type::size_type i = 0; i < thrs.size(); ++i) {
      karp_thr::arg_type arg(*this, kr);
      thr_type thr(new threaded<karp_thr>(arg));
      thrs[i] = thr;
    }
    for (thrs_type::size_type i = 0; i < thrs.size(); ++i) {
      thrs[i]->start();
    }
    int count_sum = 0;
    for (thrs_type::size_type i = 0; i < thrs.size(); ++i) {
      thrs[i]->join();
      const karp_thr& kt = thrs[i]->get();
      count_sum += kt.count;
    }
    match_count = count_sum;
    dump_count();
  }

  void run_horspool() {
    for (size_t i = 0; i < strings.size(); ++i) {
      const std::string& s = strings[i];
      if (dump_flag) {
	std::cout << "STR " << s << std::endl;
      }
      search_horspool(s, *this);
      dump_matches();
    }
    dump_count();
  }

  void run_naive() {
    for (size_t i = 0; i < strings.size(); ++i) {
      const std::string& s = strings[i];
      if (dump_flag) {
	std::cout << "STR " << s << std::endl;
      }
      search_naive(s, *this);
      dump_matches();
    }
    dump_count();
  }

  void search_naive(const std::string& s, search_callback& cb) const {
    const char *const strp = s.c_str();
    for (size_t i = 0; i < needles.size(); ++i) {
      const std::string& ndl = needles[i];
      const char *const ndlp = ndl.c_str();
      const char *p = strp;
      while (true) {
	const char *const q = strstr(p, ndlp);
	if (q == 0) {
	  break;
	}
	if (cb.on_match(q - strp + ndl.size(), i)) {
	  break;
	}
	p = q + 1;
      }
    }
  }

  void search_horspool(const std::string& s, search_callback& cb) const {
    const char *const strp = s.data();
    const char *const strp_end = strp + s.size();
    for (size_t i = 0; i < needles.size(); ++i) {
      const std::string& ndl = needles[i];
      const char *const ndlp = ndl.data();
      const size_t ndl_len = ndl.size();
      const char *p = strp;
      horspool_shift_map shift; /* not optimal */
      horspool_create_shift(ndlp, ndl_len, shift);
      while (true) {
	const char *const q = horspool_search(ndlp, ndl_len, shift, p,
	  strp_end - p);
	if (q == 0) {
	  break;
	}
	if (cb.on_match(q - strp + ndl.size(), i)) {
	  break;
	}
	p = q + 1; /* not optimal */
      }
    }
  }

  typedef std::pair<size_t, std::string> match_type;
  typedef std::set<match_type> matches_type;
  matches_type matches;

  bool on_match(size_t offset, int id) {
    assert(unsigned(id) < needles.size());
    if (dump_flag) {
      const std::string& k = needles[id];
      match_type m(offset - k.size(), k);
      matches.insert(m);
    }
    ++match_count;
    return false;
  }

  void dump_matches() {
    for (matches_type::const_iterator i = matches.begin(); i != matches.end();
      ++i) {
      const match_type& m = *i;
      std::cout << "MATCH offset=" << m.first << ": " << m.second <<
	std::endl;
    }
    matches.clear();
  }

  void dump_count() {
    std::cout << "matches: " << match_count << std::endl;
  }

};

struct timer {

  timer() {
    gettimeofday(&tv, 0);
  }
  ~timer() {
    timeval tvend;
    gettimeofday(&tvend, 0);
    const unsigned long t = get_msec(tvend) - get_msec(tv);
    printf("TIME: %lu.%03lu\n", t / 1000, t % 1000);
  }
  static unsigned long get_msec(const timeval& t) {
    unsigned long r = t.tv_sec * 1000;
    r += t.tv_usec / 1000;
    return r;
  }

 private:

  timeval tv;

};

void
timing(int method, size_t num_haystacks, size_t haystack_len, bool dump_flag,
  size_t ndl_max, int len_base, int len_rand)
{
  printf("timing method=%d lb=%d lr=%d ndl=%d\n",
    method, len_base, len_rand, (int)ndl_max);
  for (size_t needles = 1; needles <= ndl_max; needles <<= 1) {
    testdata td(dump_flag, needles, num_haystacks, haystack_len, 48,
      len_base, len_rand);
    printf("num_needles=%zu num_haystacks=%zu haystack_len=%zu"
      " needleslen=%zu\n",
      needles, num_haystacks, haystack_len, td.needles_totallen);
    if (method == 0) {
      printf("START\n");
      timer t;
      td.run_naive();
    } else if (method == 1) {
      printf("START\n");
      timer t;
      td.run_horspool();
    } else if (method >= 100 && method < 200) {
      const int create_direct_depth = (method > 100) ? method - 100 : -1;
      aho_corasick ac(td.needles, create_direct_depth);
      printf("START binsize=%d\n", ac.get_binary_size());
      timer t;
      td.run_iter(ac);
    } else if (method >= 200 && method < 300) {
      std::vector<const char *> ndls;
      for (size_t i = 0; i < td.needles.size(); ++i) {
	ndls.push_back(td.needles[i].data());
      }
      unsigned int htsize = ndls.size() * 4;
      htsize = std::max(htsize, 0xfffU);
      testdata::kr_type kr(&ndls[0], ndls.size(), htsize);
      #if 0
      testdata::kr_type::hashval_type hts = 0, pn = 0;
      int bkt = 0;
      kr.get_statistics(pn, hts, bkt);
      printf("pn=%lu htsize=%lu bkt=%d\n",
	(unsigned long)pn, (unsigned long)hts, bkt);
      #endif
      timer t;
      if (method == 200) {
	td.run_karp(kr);
      } else {
	int num_threads = method - 200;
	printf("nthreads=%d\n", num_threads);
	td.run_karp_mt(kr, num_threads);
      }
    }
  }
}

}; // anonymous namespace

int
main(int argc, char **argv)
{
  int len_base = 5;
  int len_rand = 1;
  int ndl_max = 1024;
  if (argc > 2) {
    len_base = atoi(argv[2]);
  }
  if (argc > 3) {
    len_rand = atoi(argv[3]);
  }
  if (argc > 4) {
    ndl_max = atoi(argv[4]);
  }
  if (argc > 1 && strcmp(argv[1], "bin") == 0) {
    timing(100, 10*1000, 10*1000, false, ndl_max, len_base, len_rand);
  } else if (argc > 1 && strcmp(argv[1], "bin0") == 0) {
    timing(100, 10*1000, 10*1000, false, ndl_max, len_base, len_rand);
  } else if (argc > 1 && strcmp(argv[1], "bin1") == 0) {
    timing(101, 10*1000, 10*1000, false, ndl_max, len_base, len_rand);
  } else if (argc > 1 && strcmp(argv[1], "bin2") == 0) {
    timing(102, 10*1000, 10*1000, false, ndl_max, len_base, len_rand);
  } else if (argc > 1 && strcmp(argv[1], "karp") == 0) {
    if (len_rand > 1) {
      std::cerr << "LEN_RAND must be 1 for karp" << std::endl;
    } else if (len_base != 5) {
      std::cerr << "LEN_BASE must be 5 for karp" << std::endl;
    } else {
      timing(200, 10*1000, 10*1000, false, ndl_max, len_base, len_rand);
    }
  } else if (argc > 1 && strncmp(argv[1], "karp", 4) == 0) {
    if (len_rand > 1) {
      std::cerr << "LEN_RAND must be 1 for karp??" << std::endl;
    } else if (len_base != 5) {
      std::cerr << "LEN_BASE must be 5 for karp??" << std::endl;
    } else {
      int num_threads = atoi(argv[1] + 4);
      timing(200 + num_threads, 10*1000, 10*1000, false, ndl_max, len_base,
	len_rand);
    }
  } else if (argc > 1 && strcmp(argv[1], "horspool") == 0) {
    timing(1, 10*1000, 10*1000, false, ndl_max, len_base, len_rand);
  } else if (argc > 1 && strcmp(argv[1], "naive") == 0) {
    timing(0, 10*1000, 10*1000, false, ndl_max, len_base, len_rand);
  } else {
    std::cerr << "Usage: " << argv[0] <<
      " [ bin | bin0 | bin1 | bin2 | karp | karpmt | horspool | naive ] "
	"[LEN_BASE] [LEN_RAND] [NEEDLE_MAX]"
      << std::endl;
  }
  return 0;
}

