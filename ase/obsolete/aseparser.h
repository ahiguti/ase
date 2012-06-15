
#ifndef GENTYPE_ASEPARSER_H
#define GENTYPE_ASEPARSER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  asestag_module,
  asestag_klass,
  asestag_method,
  asestag_args,
  asestag_var,
  asestag_seq,
  asestag_attr,
  asestag_while,
  asestag_dowhile,
  asestag_for,
  asestag_if,
  asestag_break,
  asestag_continue,
  asestag_throw,
  asestag_trycatch,
  asestag_return,
  asestag_lref,
  asestag_gref,
  asestag_let,
  asestag_get,
  asestag_set,
  asestag_minvoke,
  asestag_op,
  asestag_closure,
  asestag_array,
  asestag_map,
  asestag_cpp,
  /* the followings are used by the parser internally */
  asestag_i_string,    /* string literal */
  asestag_i_number,    /* numeric literal */
  asestag_i_list,      /* list cons operator */
  asestag_i_nil,       /* list nil */
} asestag;

typedef struct aseparser_s aseparser;

typedef struct {
  const char *begin, *end;
} asetoken;

typedef struct {
  asestag tag;
  asetoken token;
  size_t *args; /* allocate */
  size_t nargs;
} asepnode;

aseparser *aseparser_create(const char *filename, int *notfound_r);
void aseparser_free(aseparser *cx);
int aseparser_parse(aseparser *cx);
int aseparser_is_oom(aseparser *cx);
size_t aseparser_get_top(aseparser *cx);
const asepnode *aseparser_get_node(aseparser *cx, size_t i);
const char *aseparser_tag2string(asestag t);
void aseparser_print_node(aseparser *cx, size_t n, int indent, FILE *fp);
const char *aseparser_get_error(aseparser *cx, int *line_r, int *col_r);
void aseparser_print_error(aseparser *cx, FILE *fp);

#ifdef __cplusplus
}
#endif

#endif

