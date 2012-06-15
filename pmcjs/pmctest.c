
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utfconv.h"
#include <iconv.h>
#include "poutil_js.h"

static void
pmc_test_utfconv(const char *filename, int is_8to16)
{
  FILE *fp = NULL;
  long len = 0;
  long wlen = 0;
  void *buf = NULL;
  void *wbuf = NULL;
  fp = fopen(filename, "r");
  if (!fp) {
    return;
  }
  fseek(fp, 0, SEEK_END);
  len = ftell(fp);
  wlen = len * 3;
  rewind(fp);
  buf = pmc_malloc_nocx(len + 1, "pmc_test_utfconv");
  wbuf = pmc_malloc_nocx(wlen + 1, "pmc_test_utfconv");
  fread(buf, len, 1, fp);
  if (is_8to16) {
    pmc_u8ch_t *u8buf, *u8end;
    pmc_u16ch_t *u16buf, *u16end, *u16r;
    const pmc_u8ch_t *u8r;
    pmc_uconv_err_t err;
    u8buf = (pmc_u8ch_t *)buf;
    u16buf = (pmc_u16ch_t *)wbuf;
    u8end = u8buf + len;
    u16end = u16buf + (wlen / 2);
    err = pmc_uconv_8to16(u8buf, u8end, u16buf, u16end, &u8r, &u16r);
    if (err) {
      fprintf(stderr, "8to16 failed pos=%lu\n", (unsigned long)(u8r - u8buf));
    } else {
      fwrite(wbuf, (u16r - u16buf) * 2, 1, stdout);
    }
  } else {
    pmc_u16ch_t *u16buf, *u16end;
    pmc_u8ch_t *u8buf, *u8end, *u8r;
    const pmc_u16ch_t *u16r;
    pmc_uconv_err_t err;
    u16buf = (pmc_u16ch_t *)buf;
    u8buf = (pmc_u8ch_t *)wbuf;
    u16end = u16buf + (len / 2);
    u8end = u8buf + wlen;
    err = pmc_uconv_16to8(u16buf, u16end, u8buf, u8end, &u16r, &u8r);
    if (err) {
      fprintf(stderr, "8to16 failed pos=%lu\n",
       	(unsigned long)(u16r - u16buf));
    } else {
      fwrite(wbuf, (u8r - u8buf), 1, stdout);
    }
  }
  pmc_free(wbuf, "pmc_test_utfconv");
  pmc_free(buf, "pmc_test_utfconv");
  fclose(fp);
}

static void
pmc_test_gen32data()
{
  int i;
  unsigned int buf[0x110000];
  for (i = 0; i < 0x110000; ++i) {
    if (i >= 0xd800 && i < 0xe000) {
      buf[i] = 0;
    } else {
      buf[i] = i;
    }
  }
  fwrite(buf, 0x110000, 4, stdout);
}

static void
pmc_test_to16timing(int mod, size_t len, int loop, int use_sysconv)
{
  pmc_u16ch_t *buf16;
  const pmc_u16ch_t *b16;
  pmc_u8ch_t *buf8, *b8;
  int i, l;
  size_t b8len;
  b8len = len * 3;
  printf("to16 len=%lu loop=%d\n", (unsigned long)len, loop);
  buf16 = (pmc_u16ch_t *)pmc_malloc_nocx(len * sizeof(*buf16) + 100,
    "pmc_test_to16timing");
  buf8 = (pmc_u8ch_t *)pmc_malloc_nocx(b8len * sizeof(*buf8) + 100,
    "pmc_test_to16timing");
  for (i = 0; i < len; ++i) {
    pmc_u16ch_t v;
    while (1) {
      v = rand() % mod;
      if (v < 0xd800 || v >= 0xe000) {
	break;
      }
    }
    buf16[i] = v;
  }
  if (pmc_uconv_16to8(buf16, buf16 + len, buf8, buf8 + b8len,
    &b16, &b8) != 0) {
    abort();
  }
  printf("len=%lu u8len=%lu\n", (unsigned long)len,
    (unsigned long)(b8 - buf8));
  if (use_sysconv) {
    iconv_t cd;
    printf("iconv\n");
    cd = iconv_open("UTF16", "UTF8");
    if (cd == (iconv_t)-1) {
      perror("iconv_open");
      abort();
    }
    for (l = 0; l < loop; ++l) {
      char *p, *q;
      size_t plen, qlen;
      size_t r;
      p = (char *)buf8;
      q = (char *)buf16;
      plen = (size_t)(b8 - buf8);
      qlen = len * 2 + 2;
      //printf("%lu\n", (unsigned long)qlen);
      r = iconv(cd, &p, &plen, &q, &qlen);
      if (r == (size_t)-1) {
	perror("iconv");
	abort();
      }
      //printf("%lu\n", (unsigned long)qlen);
    }
  } else {
    for (l = 0; l < loop; ++l) {
      const pmc_u8ch_t *b8c;
      pmc_u16ch_t *b16r;
      if (pmc_uconv_8to16(buf8, b8, buf16, buf16 + len,
	&b8c, &b16r) != 0) {
	abort();
      }
    }
  }
  pmc_free(buf16, "pmc_test_to16timing");
  pmc_free(buf8, "pmc_test_to16timing");
}

static void
pmc_test_set_utf8_2byte(pmc_u32ch_t c, pmc_u8ch_t *w8)
{
  w8[0] = (pmc_u8ch_t)(0xc0 | ((c >> 6) & 0x1f));
  w8[1] = (pmc_u8ch_t)(0x80 | (c & 0x3f));
}

static void
pmc_test_set_utf8_3byte(pmc_u32ch_t c, pmc_u8ch_t *w8)
{
  w8[0] = (pmc_u8ch_t)(0xe0 | ((c >> 12) & 0xf));
  w8[1] = (pmc_u8ch_t)(0x80 | ((c >> 6) & 0x3f));
  w8[2] = (pmc_u8ch_t)(0x80 | (c & 0x3f));
}

static void
pmc_test_set_utf8_4byte(pmc_u32ch_t c, pmc_u8ch_t *w8)
{
  w8[0] = (pmc_u8ch_t)(0xf0 | ((c >> 18) & 0x7));
  w8[1] = (pmc_u8ch_t)(0x80 | ((c >> 12) & 0x3f));
  w8[2] = (pmc_u8ch_t)(0x80 | ((c >> 6) & 0x3f));
  w8[3] = (pmc_u8ch_t)(0x80 | (c & 0x3f));
}

typedef struct {
  size_t bytelen;
  pmc_u32ch_t value;
} pmc_test_u8err_t;

static pmc_test_u8err_t pmc_test_u8data[] = {
  { 2, 0x80 },
  { 2, 0x800 },
  { 3, 0x80 },
  { 3, 0x800 },
  { 3, 0xd800 },
  { 3, 0xe000 },
  { 4, 0x80 },
  { 4, 0x800 },
  { 4, 0xd800 },
  { 4, 0xe000 },
  { 4, 0x10000 },
  { 4, 0x110000 },
  { 0, 0 },
};

void
pmc_test_8to16err()
{
  pmc_u8ch_t buffer[32];
  pmc_u16ch_t u16buf[32];
  pmc_test_u8err_t *td;
  for (td = pmc_test_u8data; ; ++td) {
    void (*f)(pmc_u32ch_t, pmc_u8ch_t *) = NULL;
    pmc_u32ch_t uc = 0;
    pmc_uconv_err_t err = pmc_uconv_err_success;
    switch (td->bytelen) {
    case 2:
      f = pmc_test_set_utf8_2byte;
      break;
    case 3:
      f = pmc_test_set_utf8_3byte;
      break;
    case 4:
      f = pmc_test_set_utf8_4byte;
      break;
    default:
      break;
    }
    if (!f) {
      break;
    }
    for (uc = td->value - 2; uc < td->value + 2; ++uc) {
      const pmc_u8ch_t *r8pos = NULL;
      pmc_u16ch_t *w16pos = NULL;
      (*f)(uc, buffer);
      err = pmc_uconv_8to16(buffer, buffer + td->bytelen, u16buf,
	u16buf + 32, &r8pos, &w16pos);
      printf("%d %06x %d\n", (int)td->bytelen, uc, err);
    }
  }
}

typedef struct {
  size_t chars;
  pmc_u16ch_t value;
} pmc_test_u16err_t;

static pmc_test_u16err_t pmc_test_u16data[] = {
  { 1, 0xd800 },
  { 1, 0xdc00 },
  { 1, 0xe000 },
  { 2, 0xd800 },
  { 2, 0xdc00 },
  { 2, 0xe000 },
  { 0, 0 },
};

void
pmc_test_16to8err()
{
  pmc_u16ch_t buffer[32];
  pmc_u8ch_t u8buf[32];
  pmc_test_u16err_t *td;
  for (td = pmc_test_u16data; td->chars != 0; ++td) {
    pmc_u16ch_t c = 0;
    pmc_uconv_err_t err = pmc_uconv_err_success;
    for (c = td->value - 2; c < td->value + 2; ++c) {
      const pmc_u16ch_t *r16pos = NULL;
      pmc_u8ch_t *w8pos = NULL;
      if (td->chars == 2) {
	buffer[0] = 0xd800;
	buffer[1] = c;
      } else {
	buffer[0] = c;
	buffer[1] = 0xdc00;
      }
      err = pmc_uconv_16to8(buffer, buffer + 2, u8buf,
	u8buf + 32, &r16pos, &w8pos);
      printf("%d %04x %d\n", (int)td->chars, c, err);
    }
  }
}

int
main(int argc, char **argv)
{
  if (argc > 2 && strcmp(argv[1], "16to8") == 0) {
    pmc_test_utfconv(argv[2], 0);
  } else if (argc > 2 && strcmp(argv[1], "8to16") == 0) {
    pmc_test_utfconv(argv[2], 1);
  } else if (argc > 1 && strcmp(argv[1], "gen32") == 0) {
    pmc_test_gen32data();
  } else if (argc > 4 && strcmp(argv[1], "t16t") == 0) {
    pmc_test_to16timing(65536, atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
  } else if (argc > 1 && strcmp(argv[1], "8to16err") == 0) {
    pmc_test_8to16err();
  } else if (argc > 1 && strcmp(argv[1], "16to8err") == 0) {
    pmc_test_16to8err();
  } else {
    fprintf(stderr, "usage: %s testname filename\n", argv[0]);
    return 1;
  }
  return 0;
}

