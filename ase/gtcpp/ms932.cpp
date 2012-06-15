
#include <gtcpp/ms932.hpp>
#include <cstdio>

#define DBG_MS932(x)

namespace gtcpp {

const char *
iso2022jp_to_ms932(string_buffer& buf, const char *start, const char *finish)
{
  char *const wpbegin = buf.make_space(finish - start);
  char *wp = wpbegin;
  const char *rp = start;
  int cs = 0; /* 0: ascii 1: jisx0208 2: kana */
  while (rp < finish) {
    const unsigned char a0 = rp[0];
    if (a0 == 0x1b) {
      /* escape */
      if (rp + 2 >= finish) {
	break;
      }
      const unsigned char a1 = rp[1];
      const unsigned char a2 = rp[2];
      if (a1 == 0x24 && a2 == 0x42) {
	cs = 1;
      } else if (a1 == 0x24 && a2 == 0x40) {
	cs = 1;
      } else if (a1 == 0x28 && a2 == 0x49) {
	cs = 2;
      } else {
	cs = 0;
      }
      rp += 3;
    } else if (a0 == 0x0f) {
      cs = 2; /* SO */
      rp += 1;
    } else if (a0 == 0x0e) {
      cs = 0; /* SI */
      rp += 1;
    } else if (a0 >= 0x80) {
      /* non-iso2022jp */
      if ((a0 >= 0x81 && a0 <= 0x9f) || (a0 >= 0xe0 && a0 <= 0xef)) {
	if (rp + 1 >= finish) {
	  break;
	}
	const unsigned char a1 = rp[1];
	wp[0] = a0;
	wp[1] = a1;
	wp += 2;
	rp += 2;
      } else {
	wp[0] = a0;
	wp += 1;
	rp += 1;
      }
    } else if (cs == 1) {
      /* jisx0208 */
      if (rp + 1 >= finish) {
	break;
      }
      const unsigned char a1 = rp[1];
      const unsigned char s0 = ((a0 - 1) >> 1) + ((a0 <= 0x5e) ? 0x71 : 0xb1);
      const unsigned char s1 = a1 +
	((a0 & 1) ? ((a1 < 0x60) ? 0x1f : 0x20) : 0x7e);
      wp[0] = s0;
      wp[1] = s1;
      wp += 2;
      rp += 2;
    } else if (cs == 2) {
      /* kana */
      wp[0] = a0 + 0x80;
      wp += 1;
      rp += 1;
    } else {
      /* ascii */
      wp[0] = a0;
      wp += 1;
      rp += 1;
    }
  }
  buf.append_done(wp - wpbegin);
  DBG_MS932(fprintf(stderr, "size=%d\n", static_cast<int>(wp - wpbegin)));
  return rp;
}

};

