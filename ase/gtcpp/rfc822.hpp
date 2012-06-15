
#ifndef GTCPP_RFC822_HPP
#define GTCPP_RFC822_HPP

#include <gtcpp/string_ref.hpp>
#include <string>
#include <map>
#include <vector>

namespace gtcpp {

class rfc822_headers {

 public:

  rfc822_headers(const char *s, const char *f)
    : start(s), finish(f),
      name_start(s), name_finish(s), value_start(s), value_finish(s)
  {
  }

  bool next();

  const char *get_current() const { return start; }
  const char *name_begin() const { return name_start; }
  const char *name_end() const { return name_finish; }
  const char *value_begin() const { return value_start; }
  const char *value_end() const { return value_finish; }

 private:

  const char *start;
  const char *const finish;
  const char *name_start;
  const char *name_finish;
  const char *value_start;
  const char *value_finish;

};

struct rfc822_is_known_ipaddr_predicate {
  virtual ~rfc822_is_known_ipaddr_predicate() { };
  virtual bool predicate(unsigned long ipv4addr) = 0;
};

void rfc822_trim(const char *& start, const char *& finish);
const char *rfc822_find_end_of_headers(const char *start, const char *finish);
const char *rfc822_find_logical_line(const char *start, const char *finish);
void rfc822_extract_mail_address(const char *& start, const char *& finish);
string_ref rfc822_extract_receivedfrom_ipaddr(const char *start,
  const char *finish, unsigned long& addr_r);
string_ref rfc822_find_sender_ipaddr(const char *start, const char *finish,
  rfc822_is_known_ipaddr_predicate& pred, unsigned long& addr_r);
string_ref rfc822_find_header(const char *start, const char *finish,
  const string_ref& key);
void rfc822_headers_map(const char *start, const char *finish,
  std::map<std::string, std::string>& m_r);
void rfc822_parse_content_type(const char *start, const char *finish,
  string_ref& ct_r, string_ref& charset_r, string_ref& boundary_r);
const char *rfc822_get_body(const char *start, const char *finish,
  string_ref& enc_r, string_ref& ct_r, string_ref& charset_r,
  string_ref& boundary_r);
bool rfc822_extract_multipart(const char *start, const char *finish,
  const string_ref& boundary, std::vector<string_ref>& parts_r,
  string_ref& plain_r);

};

#endif

