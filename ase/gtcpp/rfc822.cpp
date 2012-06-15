
#include <gtcpp/string_util.hpp>
#include <gtcpp/rfc822.hpp>

#define DBG(x)

namespace gtcpp {

bool
rfc822_is_space(char c)
{
  return c == ' ' || c == '\t';
}

bool
rfc822_is_space_or_crlf(char c)
{
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

void
rfc822_ltrim(const char *& start, const char *finish)
{
  while (start != finish && rfc822_is_space_or_crlf(start[0])) {
    ++start;
  }
}

void
rfc822_rtrim(const char *start, const char *& finish)
{
  while (start != finish && rfc822_is_space_or_crlf(finish[-1])) {
    --finish;
  }
}

void
rfc822_trim(const char *& start, const char *& finish)
{
  rfc822_ltrim(start, finish);
  rfc822_rtrim(start, finish);
}

void
rfc822_ltrim_char(const char *& start, const char *finish, char ch)
{
  while (start != finish && start[0] == ch) {
    ++start;
  }
}

void
rfc822_rtrim_char(const char *start, const char *& finish, char ch)
{
  while (start != finish && finish[-1] == ch) {
    --finish;
  }
}

void
rfc822_trim_char(const char *& start, const char *& finish, char ch)
{
  rfc822_ltrim_char(start, finish, ch);
  rfc822_rtrim_char(start, finish, ch);
}

bool
rfc822_lcut(const char *& start, const char *finish, char c)
{
  const char *const p = memchr_char(start, c, finish - start);
  if (p) {
    start = p + 1;
    return true;
  }
  return false;
}

bool
rfc822_rcut(const char *start, const char *& finish, char c)
{
  const char *const p = memchr_char(start, c, finish - start);
  if (p) {
    finish = p;
    return true;
  }
  return false;
}

void
rfc822_extract_mail_address(const char *& start, const char *& finish)
{
  rfc822_rcut(start, finish, ',');
  if (rfc822_lcut(start, finish, '<')) {
    rfc822_rcut(start, finish, '>');
    return;
  }
  if (rfc822_lcut(start, finish, '"')) {
    rfc822_rcut(start, finish, '"');
    return;
  }
}

namespace {

size_t
rfc822_skip_digit(const char *& start, const char *finish)
{
  size_t c = 0;
  while (start != finish && start[0] >= '0' && start[0] <= '9') {
    ++start;
    ++c;
  }
  return c;
}

bool
rfc822_skip_dot(const char *& start, const char *finish)
{
  bool r = false;
  while (start != finish && start[0] == '.') {
    ++start;
    r = true;
  }
  return r;
}

void
rfc822_extract_addr_if(const char *start, const char *finish,
  string_ref& addrstr, unsigned long& addr_r)
{
  if (rfc822_lcut(start, finish, '[')) {
    rfc822_rcut(start, finish, ']');
  }
  const char *const rval_start = start;
  unsigned long addr = 0;
  for (size_t i = 0; i < 4; ++i) {
    const char *const digit_start = start;
    const size_t c = rfc822_skip_digit(start, finish);
    if (c < 1 || c > 3) {
      return;
    }
    unsigned int d = 0;
    parse_decimal(digit_start, start, d); /* never fails */
    addr <<= 8;
    addr += d;
    if (i != 3 && !rfc822_skip_dot(start, finish)) {
      return;
    }
  }
  if (start != finish) {
    return;
  }
  /* found */
  addr_r = addr;
  addrstr.set(rval_start, finish);
}

};

string_ref
rfc822_extract_receivedfrom_ipaddr(const char *start, const char *finish,
  unsigned long& addr_r)
{
  /* NOTE: not a strict parsing */
  string_ref addrstr;
  const char *const start_initial = start;
  /* find 'from' */
  while (true) {
    const char *const p = search_strcase("from", 4, start, finish - start);
    if (p == 0) {
      start = finish;
      return string_ref();
    }
    start = p + 4;
    if ((p == start_initial || rfc822_is_space_or_crlf(p[-1])) &&
      p + 4 < finish && rfc822_is_space_or_crlf(p[4])) {
      break;
    }
  }
  rfc822_ltrim(start, finish);
  /* read from value */
  const char *const fromval_start = start;
  if (start != finish && start[0] != '(') {
    while (start != finish && !rfc822_is_space_or_crlf(start[0])) {
      ++start;
    }
  }
  const char *const fromval_end = start;
  unsigned long addr = 0;
  rfc822_extract_addr_if(fromval_start, fromval_end, addrstr, addr);
  rfc822_ltrim(start, finish);
  /* read comments */
  while (start != finish && start[0] == '(') {
    ++start;
    const char *const p = memchr_char(start, ')', finish - start);
    if (p == 0) {
      break;
    }
    const char *comment_start = start;
    const char *comment_finish = p;
    rfc822_trim(comment_start, comment_finish);
    while (comment_start != comment_finish) {
      const char *const ws = memchr_char(comment_start, ' ',
	comment_finish - comment_start);
      const char *word_start = comment_start;
      const char *word_finish = ws ? ws : comment_finish;
      if (rfc822_lcut(word_start, word_finish, '[')) {
	rfc822_rcut(word_start, word_finish, ']');
      }
      rfc822_extract_addr_if(word_start, word_finish, addrstr, addr);
      if (!ws) {
	break;
      }
      comment_start = ws + 1;
    }
    start = p + 1;
    rfc822_ltrim(start, finish);
  }
  addr_r = addr;
  return addrstr;
}

string_ref
rfc822_find_sender_ipaddr(const char *start, const char *finish,
  rfc822_is_known_ipaddr_predicate& pred, unsigned long& addr_r)
{
  rfc822_headers hdr(start, finish);
  const string_ref rcvd("received");
  while (hdr.next()) {
    if (rcvd.equals_strcase(hdr.name_begin(), hdr.name_end())) {
      unsigned long addr = 0;
      const string_ref addrstr = rfc822_extract_receivedfrom_ipaddr(
	hdr.value_begin(), hdr.value_end(), addr);
      if (!addrstr.empty() && !pred.predicate(addr)) {
	/* found */
	addr_r = addr;
	return addrstr;
      }
    }
  }
  addr_r = 0;
  return string_ref();
}

const char *
rfc822_find_end_of_headers(const char *start, const char *finish)
{
  while (true) {
    const char *const p = memchr_char(start, '\n', finish - start);
    if (p == 0) {
      return finish;
    }
    if (p == start || (p == start + 1 && start[0] == '\r')) {
      return p + 1;
    }
    start = p + 1;
  }
  return finish;
}

const char *
rfc822_find_logical_line(const char *start, const char *finish)
{
  while (true) {
    const char *const p = memchr_char(start, '\n', finish - start);
    if (p == 0) {
      return finish; /* no LF is found */
    }
    if (p == start || (p == start + 1 && start[0] == '\r')) {
      return p + 1; /* empty line */
    }
    if (p + 1 >= finish || !rfc822_is_space(p[1])) {
      return p + 1; /* next line is a new logical line */
    }
    start = p + 1; /* current logical line continues */
  }
  return finish;
}

bool
rfc822_headers::next()
{
  const char *const nextline = rfc822_find_logical_line(start, finish);
  DBG(fprintf(stderr, "ll: [%s]\n", std::string(start, nextline).c_str()));
  if (nextline - start <= 1 ||
    (nextline - start <= 2 && (start[0] == '\r' || start[0] == '\n'))) {
    start = nextline;
    name_start = name_finish = value_start = value_finish = start;
    return false;
  }
  const char *const hdr_end = memchr_char(start, ':', nextline - start);
  if (hdr_end == 0) {
    /* malformed line */
    start = nextline;
    name_start = name_finish = value_start = value_finish = start;
    return true;
  }
  name_start = start;
  name_finish = hdr_end;
  value_start = hdr_end + 1;
  value_finish = nextline;
  rfc822_trim(value_start, value_finish);
  start = nextline;
  return true;
}

string_ref
rfc822_find_header(const char *start, const char *finish,
  const string_ref& key)
{
  rfc822_headers hdr(start, finish);
  while (hdr.next()) {
    if (key.equals_strcase(hdr.name_begin(), hdr.name_end())) {
      return string_ref(hdr.value_begin(), hdr.value_end());
    }
  }
  return string_ref();
}

void
rfc822_headers_map(const char *start, const char *finish,
  std::map<std::string, std::string>& m_r)
{
  rfc822_headers hdr(start, finish);
  while (hdr.next()) {
    const std::string k(hdr.name_begin(), hdr.name_end());
    const std::string v(hdr.value_begin(), hdr.value_end());
    m_r[k] = v;
  }
}

void
rfc822_parse_content_type(const char *start, const char *finish,
  string_ref& ct_r, string_ref& charset_r, string_ref& boundary_r)
{
  bool read_ct = false;
  while (start != finish) {
    const char *semi = find_char(start, finish, ';');
    const char *tok_begin = start;
    const char *tok_end = semi;
    const char *eq = 0;
    rfc822_trim(tok_begin, tok_end);
    if (!read_ct) {
      ct_r.set(tok_begin, tok_end);
      read_ct = true;
    } else if ((eq = memchr_char(tok_begin, '=', tok_end - tok_begin)) != 0) {
      const string_ref key(tok_begin, eq);
      ++eq;
      rfc822_trim_char(eq, tok_end, '"');
      if (key.equals_strcase("charset")) {
	charset_r.set(eq, tok_end);
      } else if (key.equals_strcase("boundary")) {
	boundary_r.set(eq, tok_end);
      }
    }
    if (semi != finish) {
      ++semi;
    }
    start = semi;
  }
}

const char *
rfc822_get_body(const char *start, const char *finish, string_ref& cte_r,
  string_ref& ct_r, string_ref& charset_r, string_ref& boundary_r)
{
  const string_ref ct_key("content-type");
  const string_ref cte_key("content-transfer-encoding");
  string_ref ctline;
  rfc822_headers hdr(start, finish);
  while (hdr.next()) {
    if (ct_key.equals_strcase(hdr.name_begin(), hdr.name_end())) {
      ctline.set(hdr.value_begin(), hdr.value_end());
    } else if (cte_key.equals_strcase(hdr.name_begin(), hdr.name_end())) {
      cte_r.set(hdr.value_begin(), hdr.value_end());
    }
  }
  DBG(fprintf(stderr, "cte: [%s]\n", cte_r.to_stdstring().c_str()));
  if (ctline.empty()) {
    return hdr.get_current();
  }
  DBG(fprintf(stderr, "mp: [%s]\n", ctline.to_stdstring().c_str()));
  rfc822_parse_content_type(ctline.begin(), ctline.end(), ct_r, charset_r,
    boundary_r);
  DBG(fprintf(stderr, "ct: [%s]\n", ct_r.to_stdstring().c_str()));
  DBG(fprintf(stderr, "cs: [%s]\n", charset_r.to_stdstring().c_str()));
  DBG(fprintf(stderr, "bnd: [%s]\n", boundary_r.to_stdstring().c_str()));
  return hdr.get_current();
}

bool
rfc822_extract_multipart(const char *start, const char *finish,
  const string_ref& boundary, std::vector<string_ref>& parts_r,
  string_ref& plain_r)
{
  parts_r.clear();
  plain_r.reset();
  string_buffer bnd;
  {
    char *const bp = bnd.make_space(boundary.size() + 2);
    bp[0] = bp[1] = '-';
    std::memcpy(bp + 2, boundary.begin(), boundary.size());
    bnd.append_done(boundary.size() + 2);
  }
  const char *part_begin = start;
  while (true) {
    /* 
     * const char *part_end = std::search(part_begin, finish, bnd.begin(),
     *   bnd.end());
     */
    const char *part_end = search_mem(bnd.begin(), bnd.size(), part_begin,
      finish - part_begin);
    if (!part_end) {
      part_end = finish;
    }
    if (part_begin != start) {
      string_ref part(part_begin, part_end);
      parts_r.push_back(part);
    } else {
      plain_r.set(start, part_begin);
    }
    if (part_end == finish) {
      return false; /* malformed */
    }
    part_begin = part_end + bnd.size();
    if ((finish - part_begin) > 2 &&
      part_begin[0] == '-' && part_begin[1] == '-') {
      return true;
    }
    if (part_begin < finish && part_begin[0] == '\r') {
      ++part_begin;
    }
    if (part_begin < finish && part_begin[0] == '\n') {
      ++part_begin;
    }
  }
}

};

