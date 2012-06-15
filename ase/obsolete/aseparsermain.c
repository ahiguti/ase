
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aseparser.h"

int
main(int argc, char **argv)
{
  aseparser *cx;
  int r;
  size_t top;
  int notfound = 0;
  if (argc < 2) {
    fprintf(stderr, "%s FILENAME\n", argv[0]);
    return -1;
  }
  cx = aseparser_create(argv[1], &notfound);
  if (cx == NULL) {
    fprintf(stderr, "failed to create parser: %s\n", argv[1]);
  }
  r = aseparser_parse(cx);
  if (r == 0) {
    top = aseparser_get_top(cx);
    aseparser_print_node(cx, top, 0, stderr);
  } else {
    aseparser_print_error(cx, stderr);
  }
  aseparser_free(cx);
  return r;
}

