
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/smtp_common.hpp>
#include <gtcpp/string_util.hpp>

namespace gtcpp {

namespace {

bool
ends_with_crlf(const string_buffer& sb)
{
  return (sb.size() >= 2 && sb.end()[-2] == '\r' && sb.end()[-1] == '\n');
}

};

// FIXME: test
void
smtp_escape_body(string_buffer& out, const string_ref& body_unescaped)
{
  out.make_space(body_unescaped.size() + 10);
  const char *begin = body_unescaped.begin();
  const char *const end = body_unescaped.end();
  while (begin != end) {
    const char *p = memchr_char(begin, '\n', end - begin);
    if (p == 0) {
      /* incomplete line */
      out.append(begin, end - begin);
      assert(begin != end); /* because of the 'while' condition */
      if (end[-1] == '\r') {
	out.append("\n", 1);
      } else {
	out.append("\r\n", 2);
      }
      assert(ends_with_crlf(out));
      break;
    }
    /* found */
    if (begin != p && begin[0] == '.') {
      /* the line begins with period. escape it. */
      out.append(".", 1);
    }
    if (begin != p && p[-1] == '\r') {
      /* we found CRLF */
      out.append(begin, p + 1 - begin);
    } else {
      /* we found LF without leading CR */
      out.append(begin, p - begin);
      out.append("\r\n", 2);
    }
    assert(ends_with_crlf(out));
    begin = p + 1;
  }
  if (!ends_with_crlf(out)) {
    /* we reach here only if body_unescaped is empty */
    out.append("\r\n", 2);
  }
  out.append(".\r\n", 3); /* out ends with CRLF . CRLF */
}

namespace {

void
unescape_line(string_buffer& out, const char *start, const char *finish)
{
  if (start != finish && start[0] == '.') {
    ++start;
  }
  out.append(start, finish - start);
}

};

// FIXME: test
void
smtp_unescape_body(string_buffer& out, const string_ref& body_escaped)
{
  out.make_space(body_escaped.size());
  const char *begin = body_escaped.begin();
  const char *const end = body_escaped.end();
  while (begin != end) {
    const char *p = memchr_char(begin, '\n', end - begin);
    if (p == 0) {
      /* incomplete line */
      unescape_line(out, begin, end);
      break;
    }
    if ((p == begin + 1 || (p == begin + 2 && p[-1] == '\r')) &&
      begin[0] == '.') {
      /* LF . (CR) LF */
      /* ignore this line */
    } else {
      unescape_line(out, begin, p + 1);
    }
    begin = p + 1;
  }
}

};

