
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/aho_corasick.hpp>

#include <map>
#include <iostream>

#define AC_AUTOMATON_DBG_SIZE(x)
#define AC_AUTOMATON_DBG_OP(x)

namespace gtcpp {

struct aho_corasick::impl_type {

  struct ac_state {

    typedef std::map<unsigned char, int> next_type;

    ac_state() : suffix(0), match(false), match_id(bin_format::match_none),
      bin_offset(0) { }

    next_type next;
    int suffix;
    bool match;
    unsigned int match_id;
    unsigned int bin_offset;

  };

  typedef std::vector<ac_state> stats_type;
  typedef ac_state::next_type next_type;
  typedef std::vector<unsigned int> bin_type;

  impl_type(const std::vector<std::string>& needlearr,
    int create_direct_depth);
  int create_state(stats_type& stats);
  int get_next(stats_type& stats, int state, unsigned char c) const;
  void init_key(stats_type& stats, const std::string& key, int id);
  void create_bin(stats_type& stats, int state, bool direct);
  void update_bin(stats_type& stats, int state);
  void dump(stats_type& stats, const std::vector<std::string>& needlearr)
    const;

  bin_type bin;

};

aho_corasick::aho_corasick(const std::vector<std::string>& needlearr,
  int create_direct_depth)
  : impl(new impl_type(needlearr, create_direct_depth))
{
}

aho_corasick::~aho_corasick()
{
}

const unsigned int *
aho_corasick::get_binary() const
{
  return impl->bin.size() == 0 ? 0 : &impl->bin[0];
}

unsigned int
aho_corasick::get_binary_size() const
{
  return static_cast<unsigned int>(impl->bin.size());
}

aho_corasick::impl_type::impl_type(const std::vector<std::string>& needlearr,
  int create_direct_depth)
{
  AC_AUTOMATON_DBG_SIZE(std::cerr << "s=" << sizeof(ac_state::next_ent)
    << std::endl);

  stats_type stats;

  create_state(stats); /* root */

  for (size_t i = 0; i < needlearr.size(); ++i) {
    init_key(stats, needlearr[i], i);
  }

  typedef std::vector<int> ss_type;
  ss_type ss;
  ss.push_back(0);
  int depth = 0;
  while (!ss.empty()) {
    ss_type ss_next;
    for (ss_type::const_iterator i = ss.begin(); i != ss.end(); ++i) {
      const int x = *i;
      ac_state& st = stats[x];
      for (next_type::const_iterator j = st.next.begin(); j != st.next.end();
	++j) {
	const unsigned char c = j->first;
	const int xnext = j->second;
	const int xsuffix = st.suffix;
	if (x != 0) {
	  const int xsuffixnext = get_next(stats, xsuffix, c);
	  /* set suffix and match */
	  stats[xnext].suffix = xsuffixnext;
	  stats[xnext].match |= stats[xsuffixnext].match;
	}
	/* next depth */
	ss_next.push_back(xnext);
      }
      create_bin(stats, x, (depth <= create_direct_depth));
    }
    ++depth;
    ss.swap(ss_next);
  }

  for (size_t i = 0; i < stats.size(); ++i) {
    update_bin(stats, i);
  }
}

void
aho_corasick::impl_type::create_bin(stats_type& stats, int state, bool direct)
{
  const unsigned int id = static_cast<unsigned int>(bin.size());
  AC_AUTOMATON_DBG_OP(std::cout << "st=" << state << " id=" << id
    << std::endl);
  const bool has_direct_tbl = direct;
  unsigned int code = 0;
  ac_state& st = stats[state];
  st.bin_offset = id;
  code |= (has_direct_tbl ?
    bin_format::has_direct_tbl_mask : bin_format::mask_zero);
  code |= (st.match ?
    bin_format::has_match_mask : bin_format::mask_zero);
  code |= (stats[st.suffix].bin_offset);
  bin.push_back(code);
  if (has_direct_tbl) {
    /* direct table */
    for (unsigned int i = 0; i < 256; ++i) {
      bin.push_back(0); /* set later */
    }
  } else {
    /* binary search table */
    const unsigned int sz = static_cast<unsigned int>(st.next.size());
    bin.push_back(sz);
    for (unsigned int i = 0; i < sz; ++i) {
      bin.push_back(0); /* set later */
    }
  }
  if (st.match) {
    bin.push_back(st.match_id);
  }
}

void
aho_corasick::impl_type::update_bin(stats_type& stats, int state)
{
  const ac_state& st = stats[state];
  const unsigned int id = st.bin_offset;
  const unsigned int bin0 = bin[id];
  AC_AUTOMATON_DBG_OP(printf("offset=%08x st=%d %08x", id, state, bin0));
  unsigned int i = 0;
  if (bin_format::has_direct_tbl(bin0)) {
    /* direct table */
    for (i = 0; i < 256; ++i) {
      const int nextst = get_next(stats, state, i);
      const unsigned int nextid = stats[nextst].bin_offset;
      bin[id + 1 + i] = bin_format::next_entry(i, nextid);
      AC_AUTOMATON_DBG_OP(printf(" %08x", bin[id + 1 + i]));
    }
  } else {
    /* binary search table */
    AC_AUTOMATON_DBG_OP(printf(" ts=%08x", bin[id + 1]));
    next_type::const_iterator j;
    for (i = 1, j = st.next.begin(); j != st.next.end(); ++i, ++j) {
      const int ch = j->first;
      const int nextst = j->second;
      const unsigned int nextid = stats[nextst].bin_offset;
      bin[id + 1 + i] = bin_format::next_entry(ch, nextid);
      AC_AUTOMATON_DBG_OP(printf(" %08x", bin[id + 1 + i]));
    }
  }
  AC_AUTOMATON_DBG_OP(printf("\n"));
}

int
aho_corasick::impl_type::create_state(stats_type& stats)
{
  int r = static_cast<int>(stats.size());
  stats.push_back(ac_state());
  return r;
}

int
aho_corasick::impl_type::get_next(stats_type& stats, int state,
  unsigned char c) const
{
  while (true) {
    const ac_state& st = stats[state];
    const next_type::const_iterator i = st.next.find(c);
    if (i != st.next.end()) {
      return i->second;
    }
    if (state == 0) {
      return state;
    }
    state = st.suffix;
  }
}

void
aho_corasick::impl_type::init_key(stats_type& stats, const std::string& key,
  int id)
{
  int state = 0;
  for (size_t i = 0; i < key.size(); ++i) {
    unsigned char c = static_cast<unsigned char>(key[i]);
    const ac_state& st = stats[state];
    const next_type::const_iterator j = st.next.find(c);
    int next_state = 0;
    if (j != st.next.end()) {
      next_state = j->second;
    } else {
      next_state = create_state(stats);
      stats[state].next[c] = next_state;
    }
    state = next_state;
  }
  stats[state].match = true;
  stats[state].match_id = id;
}

void
aho_corasick::impl_type::dump(stats_type& stats,
  const std::vector<std::string>&) const
{
  for (size_t i = 0; i < stats.size(); ++i) {
    const ac_state& st = stats[i];
    if (st.match) {
      std::cout << "[" << i << "]* ";
    } else {
      std::cout << "[" << i << "]  ";
    }
    next_type::const_iterator i;
    for (i = st.next.begin(); i != st.next.end(); ++i) {
      std::cout << " " << i->first << ":" << i->second;
    }
    std::cout << " (" << st.suffix << ")" << std::endl;
  }
}

};

