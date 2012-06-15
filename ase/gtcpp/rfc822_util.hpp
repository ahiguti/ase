
#ifndef GTCPP_RFC822_UTIL_HPP
#define GTCPP_RFC822_UTIL_HPP

#include <gtcpp/string_buffer.hpp>
#include <gtcpp/string_ref.hpp>
#include <vector>
#include <set>
#include <string>

namespace gtcpp {

struct rfc822_to_plain_options {
  bool to_utf8;
  bool html_to_plain;
  rfc822_to_plain_options() : to_utf8(true), html_to_plain(true) { }
};

void rfc822_to_plain(string_buffer& outbuf, const char *start,
  const char *finish, const rfc822_to_plain_options& opts);
void rfc822_extract_urls(std::vector<std::string>& urls_r, const char *start,
  const char *finish, bool convert_from_rfc822, bool dom_only);
void rfc822_extract_urls(std::set<std::string>& urls_r, const char *start,
  const char *finish, bool convert_from_rfc822, bool dom_only);

};

#endif

