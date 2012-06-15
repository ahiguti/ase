
#include <gtcpp/base64.hpp>
#include <stdio.h>

#define DBG(x)

namespace gtcpp {

namespace {

bool is_base64_space(char i)
{
  return (i >= 0 && i <= 32);
}

unsigned int decode_char(char i)
{
  if (i >= 'A' && i <= 'Z') {
    return i - 'A';
  }
  if (i >= 'a' && i <= 'z') {
    return i - 'a' + 26;
  }
  if (i >= '0' && i <= '9') {
    return i - '0' + 52;
  }
  if (i == '+') {
    return 62;
  }
  if (i == '/') {
    return 63;
  }
  if (i == '=') {
    return 0;
  }
  return 64; /* space or invalid char */
}

bool
get_next_decode(const char *& start, const char *finish, unsigned int& npad,
  unsigned int& val_r, size_t& invalid_count)
{
  while (start != finish) {
    if (*start == '=') {
      ++npad;
    }
    const char c = *start;
    const unsigned int v = decode_char(c);
    ++start;
    if (v < 64) {
      /* valid */
      val_r = v;
      return true;
    } else {
      if (!is_base64_space(c)) {
	++invalid_count;
      }
    }
  }
  return false;
}

};

#if 0
base64_table::base64_table(void)
{
  unsigned int i;
  for (i = 0; i < 256; i++) table[i] = 64;
  for (i = 'A'; i <= 'Z'; i++) table[i] = i - 'A';
  for (i = 'a'; i <= 'z'; i++) table[i] = i - 'a' + 26;
  for (i = '0'; i <= '9'; i++) table[i] = i - '0' + 52;
  table['+' + 0] = 62;
  table['/' + 0] = 63;
  table['=' + 0] = 0;
}
#endif

size_t
base64_decode(const char *start, const char *finish, string_buffer& buf_append)
{
  size_t invalid_count = 0;
  char *const wpbegin = buf_append.make_space(finish - start + 3);
  char *wp = wpbegin;
  bool success = true;
  unsigned int npad = 0;
  while (true) {
    unsigned int u0 = 0, u1 = 0, u2 = 0, u3 = 0;
    success &= get_next_decode(start, finish, npad, u0, invalid_count);
    success &= get_next_decode(start, finish, npad, u1, invalid_count);
    success &= get_next_decode(start, finish, npad, u2, invalid_count);
    success &= get_next_decode(start, finish, npad, u3, invalid_count);
    if (!success) {
      break;
    }
    const unsigned int val = (u0 << 18) | (u1 << 12) | (u2 << 6) | u3;
    wp[0] = val >> 16;
    wp[1] = (val >> 8) & 0xff;
    wp[2] = (val & 0xff);
    wp += 3;
  }
  size_t len = wp - wpbegin;
  if (npad <= 2 && npad <= len) {
    len -= npad;
  }
  DBG(fprintf(stderr, "b64decode: npad=%u\n", npad));
  buf_append.append_done(len);
  return invalid_count;
}

};

#if 0
bool
base64_table::decode(const aioport::strref& buf_in,
  aioport::cvector& buf_r) const
{
  buf_r.clear();
  size_t len = buf_in.size();
  if ((len & 3) != 0) {
    /* len is not a multiple of 4 */
    return false;
  } else if (len < 4) {
    return true;
  }
  const char *padp = aioport::find_char(buf_in.data(), '=', len);
  if (padp && aioport::sign_cast<size_t>(padp - buf_in.data()) < len - 2) {
    /* pad character is found in a wrong place */
    return false;
  }
  size_t npad = padp ? (buf_in.data() + len - padp) : 0;
  if (npad == 2 && buf_in[len - 1] != '=') {
    /* invalid pad character */
    return false;
  }
  assert(npad <= 2);
  const unsigned char *str =
    reinterpret_cast<const unsigned char *>(buf_in.data());
  for (const unsigned char *rp = str; rp < str + len; rp += 4) {
    unsigned long u0 = table[rp[0]];
    unsigned long u1 = table[rp[1]];
    unsigned long u2 = table[rp[2]];
    unsigned long u3 = table[rp[3]];
    if (u0 > 63 || u1 > 63 || u2 > 63 || u3 > 63) {
      return false;
    }
    unsigned long val = (u0 << 18) | (u1 << 12) | (u2 << 6) | u3;
    buf_r.push_back(aioport::digits_cast<unsigned char>(val >> 16));
    buf_r.push_back(aioport::digits_cast<unsigned char>((val >> 8) & 0xff));
    buf_r.push_back(aioport::digits_cast<unsigned char>(val & 0xff));
  }
  assert(buf_r.size() > 2);
  buf_r.resize(buf_r.size() - npad);
  return true;
}
#endif

