
#include <gtcpp/ascii.hpp>
#include <cctype>

namespace gtcpp {

namespace {

template <typename TF> void
string_tr(string_buffer& buf, const char *start,
  const char *finish, TF f)
{
  char *wp = buf.make_space(finish - start);
  while (start < finish) {
    *wp = f(*start);
    ++wp;
    ++start;
  }
  buf.append_done(finish - start);
}

template <typename TF> std::string
string_tr(const char *start,
  const char *finish, TF f)
{
  std::string r;
  r.resize(finish - start);
  std::string::size_type i = 0;
  const std::string::size_type n = (finish - start);
  for (i = 0; i < n; ++i) {
    r[i] = f(*start);
    ++start;
  }
  return r;
}

};

void to_lower(string_buffer& buf, const char *start, const char *finish)
{
  /* i have no idea why gcc4.3 fails to compile if we use std::tolower */
  string_tr(buf, start, finish, ::tolower);
}

void to_upper(string_buffer& buf, const char *start, const char *finish)
{
  string_tr(buf, start, finish, ::toupper);
}

std::string to_lower(const std::string& str)
{
  return string_tr(str.data(), str.data() + str.size(), ::tolower);
}

std::string to_upper(const std::string& str)
{
  return string_tr(str.data(), str.data() + str.size(), ::toupper);
}

std::string drop_nonalnum(const std::string& str)
{
  std::string r;
  for (std::string::const_iterator i = str.begin(); i != str.end(); ++i) {
    if (std::isalnum(*i)) {
      r.push_back(*i);
    }
  }
  return r;
}

void
fold_space(string_buffer& buf, const char *start, const char *finish)
{
  char *const wpbegin = buf.make_space(finish - start);
  char *wp = wpbegin;
  char chprev = '\0';
  while (start != finish) {
    char ch = *start;
    if (ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t') {
      ch = ' ';
      if (chprev != ' ') {
	*wp = ch;
	++wp;
      }
    } else {
      *wp = ch;
      ++wp;
    }
    chprev = ch;
    ++start;
  }
  buf.append_done(wp - wpbegin);
}

};


