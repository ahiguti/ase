
#include <gtcpp/quotedpr.hpp>
#include <stdio.h>

#define DBG(x)

namespace gtcpp {

namespace {

bool
read_hexadecimal_char(char ch, unsigned int& r)
{
  r = 0;
  if (ch >= '0' && ch <= '9') {
    r = ch - '0';
    return true;
  }
  if (ch >= 'A' && ch <= 'F') {
    r = ch - 'A' + 10;
    return true;
  }
  if (ch >= 'a' && ch <= 'f') {
    r = ch - 'a' + 10;
    return true;
  }
  return false;
}

};

size_t
quoted_printable_decode(const char *start, const char *finish,
  string_buffer& buf_append)
{
  char *const wpbegin = buf_append.make_space(finish - start);
  char *wp = wpbegin;
  while (start < finish) {
    const char ch = *start;
    // if (ch == '=' && (finish - start) >= 3) {
    if (ch == '=') {
      unsigned int x1 = 0, x2 = 0;
      if ((finish - start) >= 3 &&
	read_hexadecimal_char(start[1], x1) &&
	read_hexadecimal_char(start[2], x2)) {
	*wp = (x1 << 4 | x2);
	++wp;
      }
      if ((finish - start) >= 2 && start[1] == '\n') {
	/* if the line ends with LF instead of CRLF, don't eat the leading
	 * byte of the next line. */
	start += 2; /* = LF */
      } else if ((finish - start) >= 3) {
	start += 3; /* = X X */
      } else {
	start = finish;
      }
    } else {
      *wp = ch;
      ++wp;
      ++start;
    }
  }
  buf_append.append_done(wp - wpbegin);
  return 0;
}

};

