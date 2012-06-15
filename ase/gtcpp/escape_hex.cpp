
#include <gtcpp/escape_hex.hpp>

namespace gtcpp {

namespace {

char
hexchar(char c)
{
  return (c >= 10) ? ('a' + (c - 10)) : ('0' + c);
}

};

void
escape_hexadecimal_control(string_buffer& buf, const char *start,
  const char *finish)
{
  char *const wpbegin = buf.make_space((finish - start) * 3);
  char *wp = 0;
  for (wp = wpbegin; start != finish; ++start) {
    const char c = *start;
    if (c < 0x20 || c == 0x7f) {
      wp[0] = '%';
      wp[1] = hexchar((c >> 4) & 0x0f);
      wp[2] = hexchar(c & 0x0f);
      wp += 3;
    } else {
      wp[0] = c;
      ++wp;
    }
  }
  buf.append_done(wp - wpbegin);
}

};

