
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/buffered_file.hpp>
#include <gtcpp/file_util.hpp>
#include <gtcpp/auto_glob.hpp>
#include <gtcpp/posix_error.hpp>
#include <gtcpp/karp_rabin.hpp>
#include <gtcpp/string_util.hpp>
#include <map>
#include <set>
#include <list>
#include <string>
#include <memory>
#include <iostream>
#include <unistd.h>
#include <strings.h>

#define DBG_REG(x)
#define DBG_FIND(x)
#define DBG_LOAD(x)
#define DBG_SAVE(x)
#define DBG_DROP(x)

using namespace gtcpp;

namespace ngsearch {

class ngram_search : private boost::noncopyable {

 public:

  enum { invalid_needle_id = karp_rabin<>::invalid_needle_id };
  struct specimen_type;
  typedef std::list<specimen_type> specimens_type;
  struct kr_param : public karp_rabin_default_param {
    enum { needle_len = 16 };
    struct needle_base_type {
      needle_base_type() : specimen_next(invalid_needle_id) { }
      specimens_type::iterator specimen_iter;
      needle_id_type specimen_next; /* next needle of the same specimen */
    };
  };
  struct specimen_type {
    specimen_type() : needle_id_first(invalid_needle_id), num_needles(0) { }
    kr_param::needle_id_type needle_id_first;
    unsigned int num_needles;
    std::string specimen_idstr;
  };
  typedef karp_rabin<kr_param> kr_type;
  enum { needle_len = kr_type::needle_len };
  typedef std::map<std::string, specimens_type::iterator> idmap_type;
  typedef std::map<std::string, std::string> options_type;

  ngram_search(const options_type& opt);
  specimens_type::iterator find_resemble(const char *start,
    const char *finish, unsigned int num_threshold, double& tc_max_rat_r);
  void move_to_last(specimens_type::iterator iter);
  const specimen_type *find_specimen(const std::string& idstr) const;
  bool register_specimen(const std::string& idstr, const char *start,
    const char *finish);
  bool unregister_specimen(const std::string& idstr);
  bool deserialize(const char *start, const char *finish, size_t& count_r);
  size_t serialize(string_buffer& sb) const;
  void show_stat();

 private:

  options_type options;
  std::auto_ptr<kr_type> kr;
  specimens_type specimens;
  specimens_type::size_type specimens_size; /* == specimens.size() */
  specimens_type::size_type specimens_size_limit;
  bool allow_dup;
  int debug_level;
  idmap_type idmap;

  enum {
    tag_ng_key = 10,
    tag_ng_value = 11,
  };

  struct search_callback {
    typedef std::map<unsigned int, unsigned int> mc_type;
    mc_type mc;
    void operator ()(size_t offset, kr_type::needle_id_type nid);
  };

  static size_t calc_num_unique_chars(const char *start, const char *finish);
  template <typename T> static bool deserialize_nat(const char *& start,
    const char *finish, T& value_r);
  template <typename T> static void serialize_nat(string_buffer& sb,
    T value);
  static bool deserialize_block(const char *& start, const char *finish,
    unsigned int& tag_r, string_buffer& data_r);
  static void serialize_block(string_buffer& sb, unsigned int tag,
    const char *data, size_t len);
  static void xor_block(string_buffer& sb, unsigned int val);
  void concat_needles(string_buffer& sb, kr_type::needle_id_type id,
    unsigned int nndls) const;

};

ngram_search::ngram_search(const options_type& opt)
  : options(opt), specimens_size(0), specimens_size_limit(0),
    allow_dup(false), debug_level(0)
{
  int ht = std::atoi(options["htsize"].c_str());
  if (ht == 0) {
    ht = 10000000;
  }
  kr.reset(new kr_type(0, 0, ht));
  specimens_size_limit = std::atoi(options["limit"].c_str());
  allow_dup = (std::atoi(options["multi"].c_str()) != 0);
  debug_level = std::atoi(options["debug"].c_str());
}

void
ngram_search::search_callback::operator ()(
  size_t __attribute__((unused)) offset, kr_type::needle_id_type nid)
{
  mc[nid] += 1;
}

void
ngram_search::move_to_last(specimens_type::iterator iter)
{
  specimens.splice(specimens.end(), specimens, iter);
}

ngram_search::specimens_type::iterator
ngram_search::find_resemble(const char *start, const char *finish,
  unsigned int num_threshold, double& tc_max_rat_r)
{
  tc_max_rat_r = 0;
  size_t haystack_len = finish - start;
  DBG_FIND(printf("srch: %zu\n", haystack_len));
  if (haystack_len < needle_len) {
    return specimens.end();
  }
  search_callback cb;
  if (allow_dup) {
    kr->search<true>(cb, start, haystack_len);
  } else {
    kr->search<false>(cb, start, haystack_len);
  }
  typedef std::map<unsigned int, unsigned int> tc_type;
  tc_type tc;
  specimens_type::iterator tc_max_ent;
  for (search_callback::mc_type::const_iterator iter = cb.mc.begin();
    iter != cb.mc.end(); ++iter) {
    const specimens_type::iterator speci =
      kr->get_needle(iter->first).specimen_iter;
    const kr_type::needle_id_type k = speci->needle_id_first;
    const unsigned int c = ++tc[k];
    const double rat = static_cast<double>(c) / speci->num_needles;
    DBG_FIND(printf("srch nid=%u mid=%u enndls=%u\n", iter->first, k,
      nndls));
    if (c >= num_threshold && tc_max_rat_r < rat) {
      tc_max_ent = speci;
      tc_max_rat_r = rat;
    }
  }
  return tc_max_ent;
}

const ngram_search::specimen_type *
ngram_search::find_specimen(const std::string& idstr) const
{
  idmap_type::const_iterator i = idmap.find(idstr);
  return (i != idmap.end()) ? (&*i->second) : 0;
}


bool
ngram_search::register_specimen(const std::string& idstr, const char *start,
  const char *finish)
{
  specimens_type::iterator specimen_iter;
  {
    if (idmap.find(idstr) != idmap.end()) {
      return false; /* already exist */
    }
    specimen_type te;
    te.specimen_idstr = idstr;
    te.needle_id_first = invalid_needle_id;
    te.num_needles = 0;
    specimens.push_back(te);
    ++specimens_size;
    specimen_iter = --specimens.end();
    try {
      idmap[idstr] = specimen_iter;
    } catch (...) {
      specimens.pop_back();
      --specimens_size;
      throw;
    }
  }
  specimen_type& e = specimens.back();
  const size_t len = finish - start;
  for (size_t i = 0; i + needle_len <= len; i += needle_len) {
    const size_t num_codes = calc_num_unique_chars(start + i,
      start + i + needle_len);
    if (num_codes * 3 < needle_len * 2) {
      DBG_REG(printf("noreg nc=%d\n", (int)num_codes));
      continue;
    }
    DBG_REG(printf("reg nc=%d\n", (int)num_codes));
    kr_type::needle_id_type ndlid = kr->register_needle(start + i, allow_dup);
    #ifndef NDEBUG
    if ((debug_level & 2) != 0) {
      kr->assert_valid();
    }
    #endif
    if (ndlid != kr_type::invalid_needle_id) {
      DBG_REG(printf("REG [%.16s]\n", start + i));
      kr_type::needle_base_type& nb = kr->get_needle_base(ndlid);
      nb.specimen_iter = specimen_iter;
      nb.specimen_next = e.needle_id_first;
      e.needle_id_first = ndlid;
      ++e.num_needles;
      DBG_REG(printf("reg nc=%d ndlid=%d\n", (int)num_codes, (int)ndlid));
    }
  }
  #ifndef NDEBUG
  if ((debug_level & 1) != 0) {
    kr->assert_valid();
  }
  #endif
  if (specimens_size_limit > 0 && specimens_size > specimens_size_limit) {
    assert(!specimens.empty());
    unregister_specimen(specimens.front().specimen_idstr);
  }
  return true;
}

bool
ngram_search::unregister_specimen(const std::string& idstr)
{
  idmap_type::iterator iter = idmap.find(idstr);
  if (iter == idmap.end()) {
    return false; /* not found */
  }
  specimens_type::iterator& nd = iter->second;
  specimen_type& e = *nd;
  while (e.needle_id_first != invalid_needle_id) {
    kr_type::needle_base_type& nb = kr->get_needle_base(e.needle_id_first);
    const kr_type::needle_id_type next = nb.specimen_next;
    kr->unregister_needle(e.needle_id_first);
    #ifndef NDEBUG
    if ((debug_level & 2) != 0) {
      kr->assert_valid();
    }
    #endif
    e.needle_id_first = next;
  }
  specimens.erase(nd);
  --specimens_size;
  idmap.erase(iter);
  #ifndef NDEBUG
  if ((debug_level & 1) != 0) {
    kr->assert_valid();
  }
  #endif
  return true;
}

size_t
ngram_search::calc_num_unique_chars(const char *start, const char *finish)
{
  size_t cnt = 0;
  for (const char *p = start; p != finish; ++p) {
    const char c = *p;
    const char *const q = p + 1;
    if (q == finish || std::memchr(q, c, finish - q) == 0) {
      ++cnt;
    }
  }
  return cnt;
}

template <typename T> bool
ngram_search::deserialize_nat(const char *& start, const char *finish,
  T& value_r)
{
  value_r = 0;
  unsigned int shift = 0;
  while (true) {
    if (start >= finish) {
      return false;
    }
    const unsigned char b = *reinterpret_cast<const unsigned char *>(start);
    ++start;
    T b7 = b & 0x7fU;
    const unsigned char cf = (b & 0x80U);
    b7 <<= shift;
    value_r |= b7;
    shift += 7;
    if (cf == 0) {
      break;
    }
  }
  return true;
}

template <typename T> void
ngram_search::serialize_nat(string_buffer& sb, T value)
{
  do {
    const unsigned char b7 = value & 0x7fU;
    value >>= 7;
    const unsigned char cf = (value != 0) ? 0x80U : 0;
    const unsigned char b = b7 | cf;
    sb.append(reinterpret_cast<const char *>(&b), 1);
  } while (value != 0);
}

bool
ngram_search::deserialize_block(const char *& start, const char *finish,
  unsigned int& tag_r, string_buffer& data_r)
{
  bool success = true;
  size_t len = 0;
  success &= deserialize_nat(start, finish, tag_r);
  success &= deserialize_nat(start, finish, len);
  const size_t cur_len = finish - start;
  if (cur_len >= len) {
    if (len != 0) {
      data_r.append(start, len);
      start += len;
    }
  } else {
    success = false;
  }
  return success;
}

void
ngram_search::serialize_block(string_buffer& sb, unsigned int tag,
  const char *data, size_t len)
{
  serialize_nat(sb, tag);
  serialize_nat(sb, len);
  sb.append(data, len);
}

bool
ngram_search::deserialize(const char *start, const char *finish,
  size_t& count_r)
{
  count_r = 0;
  unsigned int tag = 0;
  string_buffer work;
  std::string idstr;
  while (start < finish) {
    work.clear();
    if (!deserialize_block(start, finish, tag, work)) {
      return false;
    }
    if (tag == tag_ng_key) {
      idstr = std::string(work.begin(), work.end());
    } else if (tag == tag_ng_value) {
      if (!idstr.empty()) {
	xor_block(work, 0x96);
	DBG_LOAD(printf("load %s %d\n", idstr.c_str(), (int)work.size()));
	register_specimen(idstr, work.begin(), work.end());
	idstr.clear();
	++count_r;
      }
    }
  }
  return true;
}

size_t
ngram_search::serialize(string_buffer& sb) const
{
  size_t count = 0;
  string_buffer work;
  for (specimens_type::const_iterator i = specimens.begin();
    i != specimens.end(); ++i) {
    work.clear();
    const std::string& idstr = i->specimen_idstr;
    serialize_block(sb, tag_ng_key, idstr.data(), idstr.size());
    concat_needles(work, i->needle_id_first, i->num_needles);
    xor_block(work, 0x96);
    serialize_block(sb, tag_ng_value, work.begin(), work.size());
    DBG_SAVE(printf("save %s %d\n", idstr.c_str(), (int)work.size()));
    ++count;
  }
  return count;
}

void
ngram_search::xor_block(string_buffer& sb, unsigned int val)
{
  for (string_buffer::iterator i = sb.begin(); i != sb.end(); ++i) {
    (*i) ^= val;
  }
}

void
ngram_search::concat_needles(string_buffer& sb,
  kr_type::needle_id_type id, unsigned int __attribute__((unused)) nndls)
  const
{
  typedef std::vector<kr_type::needle_id_type> ndls_type;
  ndls_type ndls(nndls);
  unsigned int c = 0;
  while (id != invalid_needle_id) {
    ndls[c] = id;
    const kr_type::needle_type& ndl = kr->get_needle(id);
    ++c;
    id = ndl.specimen_next;
  }
  for (ndls_type::reverse_iterator i = ndls.rbegin(); i != ndls.rend(); ++i) {
    const kr_type::needle_type& ndl = kr->get_needle(*i);
    sb.append(ndl.buffer, needle_len);
  }
  assert(c == nndls);
}

void
ngram_search::show_stat()
{
  typedef std::map<std::string, std::string> strmap_type;
  strmap_type m;
  kr->get_statistics(m);
  printf("STAT:");
  for (strmap_type::const_iterator i = m.begin(); i != m.end(); ++i) {
    printf(" %s=%s", i->first.c_str(), i->second.c_str());
  }
  printf("\n");
  kr->assert_valid();
}

const char *
remove_headers(const char *start, const char *finish)
{
  const char *p, *q;
  for (p = start, q = memchr_char(p, '\n', finish - p); q;
    p = q + 1, q = memchr_char(p, '\n', finish - p)) {
    if (p == q || (p + 1 == q && p[0] == '\r')) {
      return q + 1;
    }
  }
  return start;
}

/* TODO: use gring_util.hpp */
template <size_t n> bool
equal_str_literal(const char (& str)[n], const char *start,
  const char *finish)
{
  return (n - 1 == finish - start) && (std::memcmp(str, start, n - 1) == 0);
}

template <size_t n> bool
equal_str_literal_icase(const char (& str)[n], const char *start,
  const char *finish)
{
  return (n - 1 == finish - start) &&
    (strncasecmp(str, start, n - 1) == 0);
}

template <size_t n> const char *
find_str_literal(const char (& needle)[n], const char *haystack,
  const char *haystack_end)
{
  /* naive string search */
  const size_t needle_len = n - 1;
  while (static_cast<size_t>(haystack_end - haystack) >= needle_len) {
    if (std::memcmp(haystack, needle, needle_len) == 0) {
      return haystack;
    }
    ++haystack;
  }
  return 0;
}

template <size_t n> const char *
find_str_literal_icase(const char (& needle)[n], const char *haystack,
  const char *haystack_end)
{
  /* naive string search */
  const size_t needle_len = n - 1;
  while (static_cast<size_t>(haystack_end - haystack) >= needle_len) {
    if (strncasecmp(haystack, needle, needle_len) == 0) {
      return haystack;
    }
    ++haystack;
  }
  return 0;
}

bool
is_7bit(const char *start, const char *finish)
{
  const unsigned char *const us =
    reinterpret_cast<const unsigned char *>(start);
  const unsigned char *const uf =
    reinterpret_cast<const unsigned char *>(finish);
  for (const unsigned char *p = us; p != uf; ++p) {
    if (*p >= 128) {
      return false;
    }
  }
  return true;
}

/* TODO: implement shift_nonascii() */

void
drop_html_tags(const char *start, const char *finish,
  string_buffer& sb)
{
  while (start != finish) {
    const char *const lt = memchr_char(start, '<', finish - start);
    if (!lt) {
      sb.append(start, finish - start);
      break;
    }
    sb.append(start, lt - start);
    const char *const tok = lt + 1;
    if (finish - tok >= 3 && equal_str_literal("!--", tok, finish)) {
      /* comment */
      start = tok + 3;
      const char *const cmtend = find_str_literal("-->", start, finish);
      if (!cmtend) {
	break;
      }
      start = cmtend + string_literal_length("-->");
      continue;
    }
    if (finish - tok >= 6 && equal_str_literal_icase("style", tok, finish) &&
      (tok[5] <= ' ' || tok[5] == '>' || tok[5] == '/')) {
      /* <style ..> */
      start = tok + 6;
      const char *const stend = find_str_literal_icase("</style>", start,
	finish);
      if (!stend) {
	break;
      }
      start = stend + string_literal_length("</style>");
      continue;
    }
    /* other tags */
    const char *const tokend = memchr_char(tok, '>', finish - tok);
    if (!tokend) {
      break;
    }
    start = tokend + 1;
  }
}

void
find_or_register(ngram_search& ng, const std::string& idstr,
  const char *start, const char *finish, double ratio_threshold,
  unsigned int num_threshold, bool register_if_notfound, bool show_stdout,
  size_t& num_hit, size_t& num_hit_ra)
{
  double tc_max_rat = 0;
  const ngram_search::specimens_type::iterator tc_max_ent =
    ng.find_resemble(start, finish, num_threshold, tc_max_rat);
  if (tc_max_rat > ratio_threshold) {
    ng.move_to_last(tc_max_ent);
    if (show_stdout) {
      printf("F %s(%d) %lf[%u] %s(%d)\n", idstr.c_str(), 0,
	tc_max_rat, tc_max_ent->num_needles,
	tc_max_ent->specimen_idstr.c_str(), tc_max_ent->needle_id_first);
    }
    ++num_hit;
    ++num_hit_ra;
    return;
  } else if (tc_max_rat != 0) {
    if (show_stdout) {
      printf("N %s(%d) %lf[%u] %s(%d)\n", idstr.c_str(), 0,
	tc_max_rat, tc_max_ent->num_needles,
	tc_max_ent->specimen_idstr.c_str(), tc_max_ent->needle_id_first);
    }
  } else {
    if (show_stdout) {
      printf("N %s(%d) %lf\n", idstr.c_str(), 0, tc_max_rat);
    }
  }
  if (register_if_notfound) {
    ng.register_specimen(idstr, start, finish);
  }
}

void
find_or_register(ngram_search& ng, const char *glob_pat,
  bool register_if_notfound, bool show_stdout)
{
  posix_error_callback ec;
  glob_error_callback gec;
  const double ratio_threshold = 0.5;
  unsigned int num_threshold = 5;
  auto_glob gl;
  glob(gl, glob_pat, 0, gec);
  const size_t num = gl.get().gl_pathc;
  printf("START: num=%zu reg=%d\n", num, register_if_notfound ? 1 : 0);
  if (num == 0) {
    printf("DONE: num=%zu\n", num);
    return;
  }
  size_t num_hit = 0;
  size_t num_hit_ra = 0;
  string_buffer sb, work;
  for (size_t i = 0; i < num; ++i) {
    sb.clear();
    work.clear();
    if (i % 1000 == 0) {
      if (show_stdout) {
	printf("X %lf\n", (double)num_hit_ra / 1000);
      }
      num_hit_ra = 0;
    }
    std::string fname(gl.get().gl_pathv[i]);
    if (ng.find_specimen(fname)) {
      continue; /* already exist */
    }
    try {
      read_file_contents(fname.c_str(), sb, ec);
    } catch (const posix_error& e) {
      if (show_stdout) {
	printf("E %s(%d)\n", fname.c_str(), 0);
      }
      continue;
    }
    if (sb.size() > 10 * 1024) {
      if (show_stdout) {
	printf("L %s\n", fname.c_str());
      }
      continue;
    }
    const char *start = remove_headers(sb.begin(), sb.end());
    const char *finish = sb.end();
    #if 0
    {
      drop_html_tags(start, finish, work);
      start = work.begin();
      finish = work.end();
      DBG_DROP(write_file_contents((fname + ".tmp").c_str(),
	string_ref(work.begin(), work.end()), false, ec));
    }
    #endif
    find_or_register(ng, fname, start, finish, ratio_threshold,
      num_threshold, register_if_notfound, show_stdout, num_hit, num_hit_ra);
  }
  printf("DONE: hit=%zu/%zu(%lf)\n", num_hit, num, (double)num_hit / num);
}

};

int
usage(int __attribute__((unused)) argc, char **argv, const char *msg)
{
  printf("%s: %s\n", argv[0], msg);
  printf("Usage: %s\n", argv[0]);
  printf("  fn=FILENAME\n");
  printf("  cmd=(add)\n");
  printf("  pat=GLOB_PATTERN\n");
  printf("  show=(0|1)\n");
  return 1;
}

int
main(int argc, char **argv)
{
  try {
    std::map<std::string, std::string> m;
    parse_argv(argc - 1, argv + 1, m);
    std::string fn = m["fn"];
    std::string cmd = m["cmd"];
    std::string pat = m["pat"];
    std::string show = m["show"];
    const bool show_stdout = std::atoi(show.c_str()) != 0;
    if (cmd.empty()) {
      return usage(argc, argv, "cmd is missing");
    }
    if (pat.empty()) {
      return usage(argc, argv, "fn is missing");
    }
    ngsearch::ngram_search ng(m);
    posix_error_callback ec;
    if (cmd == "add") {
      string_buffer sb;
      if (::access(fn.c_str(), R_OK) == 0) {
	read_file_contents(fn.c_str(), sb, ec);
	size_t count = 0;
	if (!ng.deserialize(sb.begin(), sb.end(), count)) {
	  fprintf(stderr, "warning: failed to deserialize %s\n", fn.c_str());
	}
	printf("LOAD: %s %zu\n", fn.c_str(), count);
      }
      find_or_register(ng, pat.c_str(), true, show_stdout);
      ng.show_stat();
      sb.clear();
      const size_t count = ng.serialize(sb);
      write_file_contents(fn.c_str(),
	string_ref(sb.begin(), sb.end()), false, ec);
      printf("SAVE: %s %zu\n", fn.c_str(), count);
      return 0;
    }
    return usage(argc, argv, "unknown cmd");
  } catch (const std::exception& e) {
    std::cerr << "uncaught exception: " << e.what() << std::endl;
    return 1;
  }
}

