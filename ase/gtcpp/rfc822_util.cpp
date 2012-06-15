
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/string_util.hpp>
#include <gtcpp/horspool.hpp>
#include <gtcpp/ascii.hpp>
#include <gtcpp/ms932.hpp>
#include <gtcpp/rfc822.hpp>
#include <gtcpp/base64.hpp>
#include <gtcpp/quotedpr.hpp>
#include <gtcpp/html.hpp>
#include <gtcpp/posix_error.hpp>
#include <gtcpp/auto_dir.hpp>
#include <gtcpp/file_util.hpp>
#include <gtcpp/auto_iconv.hpp>
#include <gtcpp/buffered_file.hpp>
#include <gtcpp/rfc822_util.hpp>
#include <errno.h>
#include <iostream>
#include <vector>
#include <set>
#include <cctype>

#define DBG(x)
#define DBG_CS(x)
#define DBG_HTML(x)

namespace gtcpp {

namespace {

void
to_utf8(const std::string& fromcs, string_buffer& obuf, const char *start,
  const char *finish)
{
  const std::string fcs = drop_nonalnum(to_lower(fromcs));
  DBG_CS(std::cerr << "fcs=[" << fcs << "]" << std::endl);
  bool suc = true;
  if (fcs.empty() || fcs == "ms932" || fcs == "cs932" || fcs == "shiftjis"
    || fcs == "iso2022jp") {
    string_buffer b1;
    iso2022jp_to_ms932(b1, start, finish);
    suc = iconv("UTF-8", "MS932", obuf, b1.begin(), b1.end());
  } else {
    suc = iconv("UTF-8", fromcs.c_str(), obuf, start, finish);
  }
  if (!suc) {
    /* cs is not supported. obuf is not modified at all */
    obuf.append(start, finish - start);
  }
}

bool
is_url_char(const char c)
{
  if (c >= 'A' && c <= 'Z') {
    return true;
  }
  if (c >= 'a' && c <= 'z') {
    return true;
  }
  if (c >= '0' && c <= '9') {
    return true;
  }
  switch (c) {
  case '_':
  case '-':
  case '.':
  case '!':
  case '~':
  case '*':
  case '\'':
  case '(':
  case ')':
  case ';':
  case '/':
  case '?':
  case ':':
  case '@':
  case '&':
  case '=':
  case '+':
  case '$':
  case ',':
  case '%':
  case '#':
    return true;
  default:
    break;
  }
  return false;
}

void
skip_url(const char *& start, const char *finish)
{
  while (start < finish && is_url_char(*start)) {
    ++start;
  }
}

template <typename T> void
extract_urls(T& urls_r, const char *start,
  const char *finish, bool dom_only)
{
  string_ref p_http("http://");
  horspool_shift_map shiftmap;
  horspool_create_shift(p_http.begin(), p_http.size(), shiftmap);
  const char *p = 0;
  while ((p = horspool_search(p_http.begin(), p_http.size(), shiftmap,
    start, (finish - start))) != 0) {
    start = p;
    skip_url(start, finish);
    const char *const q = start;
    if (dom_only) {
      const char *const dp = p + p_http.size();
      const char *const dq = find_char(dp, find_char(dp, q, '/'), '?');
      urls_r.insert(urls_r.end(), std::string(dp, dq));
    } else {
      urls_r.insert(urls_r.end(), std::string(p, q));
    }
    start = q;
  }
}

};

void
rfc822_to_plain(string_buffer& outbuf, const char *start, const char *finish,
  const rfc822_to_plain_options& opts)
{
  DBG(std::cerr << "hmdone" << std::endl);
  string_ref cte, ct, cs, bnd;
  const char *const bdy_start = rfc822_get_body(start, finish, cte, ct, cs,
    bnd);
  DBG(std::cerr << "gbdone" << std::endl);
  string_buffer dec;
  string_ref decref;
  string_buffer metacs;
  const bool is_multipart = ct.prefix_strcase("multipart/");
    /* ignore content-transfer-encoding if it's a multipart */
  if (!is_multipart && cte.equals_strcase("base64")) {
    const size_t ninvalid = base64_decode(bdy_start, finish, dec);
    const size_t threashold = (finish - start) / 128;
    DBG(std::cerr << "b64 invalid=" << ninvalid << " threashold=" << threashold
      << std::endl);
    if (ninvalid <= threashold) {
      decref.set(dec.begin(), dec.end());
      DBG(std::string s(dec.begin(), dec.end()));
      DBG(std::cerr << "b6=[" << s << "]" << std::endl);
      DBG(std::cerr << "b6done" << std::endl);
    } else {
      /* too many invalid bytes as an base64 encoded body. */
      decref.set(bdy_start, finish);
    }
  } else if (!is_multipart && cte.equals_strcase("quoted-printable")) {
    quoted_printable_decode(bdy_start, finish, dec);
    decref.set(dec.begin(), dec.end());
    DBG(std::string s(dec.begin(), dec.end()));
    DBG(std::cerr << "qp=[" << s << "]" << std::endl);
    DBG(std::cerr << "qpdone" << std::endl);
  } else {
    decref.set(bdy_start, finish);
  }
  if (is_multipart) {
    std::vector<string_ref> parts;
    string_ref plain;
    if (!rfc822_extract_multipart(decref.begin(), decref.end(), bnd, parts,
      plain)) {
      DBG(std::cerr << "mpdone: failed" << std::endl);
    }
    DBG(std::cerr << "mpdone: " << parts.size() << std::endl);
    if (!plain.empty()) {
      outbuf.append(plain.begin(), plain.size());
    }
    for (size_t i = 0; i < parts.size(); ++i) {
      rfc822_to_plain(outbuf, parts[i].begin(), parts[i].end(), opts);
    }
  } else if (ct.empty() || ct.prefix_strcase("text/")) {
    string_buffer htmlu8;
    if (ct.equals_strcase("text/html")) {
      /* html text */
      html_find_meta_charset(metacs, decref.begin(), decref.end());
      if (!metacs.empty()) {
	cs.set(metacs);
      }
      if (opts.to_utf8) {
	to_utf8(cs.to_stdstring(), htmlu8, decref.begin(), decref.end());
      } else {
	htmlu8.append(decref.begin(), decref.size());
      }
      if (opts.html_to_plain) {
	html_to_plain_options opts;
	opts.show_urls = true;
	opts.hide_linked_urls = true;
	html_to_plain(outbuf, htmlu8.begin(), htmlu8.end(), opts);
      } else {
	outbuf.append(htmlu8.begin(), htmlu8.size());
      }
    } else {
      /* plain text */
      if (opts.to_utf8) {
	to_utf8(cs.to_stdstring(), outbuf, decref.begin(), decref.end());
      } else {
	outbuf.append(decref.begin(), decref.size());
      }
    }
  } else {
    /* binary */
    DBG(std::cerr << "bin[" << decref.to_stdstring() << "]" << std::endl);
  }
}

void
rfc822_extract_urls(std::vector<std::string>& urls_r, const char *start,
  const char *finish, bool convert_from_rfc822, bool dom_only)
{
  rfc822_to_plain_options opts;
  string_buffer buf;
  if (convert_from_rfc822) {
    rfc822_to_plain(buf, start, finish, opts);
    start = buf.begin();
    finish = buf.end();
  }
  extract_urls(urls_r, start, finish, dom_only);
}

void
rfc822_extract_urls(std::set<std::string>& urls_r, const char *start,
  const char *finish, bool convert_from_rfc822, bool dom_only)
{
  rfc822_to_plain_options opts;
  string_buffer buf;
  if (convert_from_rfc822) {
    rfc822_to_plain(buf, start, finish, opts);
    start = buf.begin();
    finish = buf.end();
  }
  extract_urls(urls_r, start, finish, dom_only);
}

};
