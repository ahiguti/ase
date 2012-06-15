/* Infix notation calculator.  */

%{

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "aseparser.h"

%}

%union {
  size_t refval;
  asetoken tokval;
};

%{

static int yyparse(aseparser *cx);
static int yylex(YYSTYPE *lvalp, aseparser *cx);
static void yyerror(aseparser *cx, const char *s);

static size_t aseparser_create_node(aseparser *cx, asestag t,
  const asetoken *tok, size_t *args, size_t nargs);
static size_t aseparser_create_node_0(aseparser *cx, asestag t,
  const asetoken *tok);
static size_t aseparser_create_node_1(aseparser *cx, asestag t,
  const asetoken *tok, size_t arg1);
static size_t aseparser_create_node_2(aseparser *cx, asestag t,
  const asetoken *tok, size_t arg1, size_t arg2);
static size_t aseparser_create_node_3(aseparser *cx, asestag t,
  const asetoken *tok, size_t arg1, size_t arg2, size_t arg3);
static size_t aseparser_create_node_4(aseparser *cx, asestag t,
  const asetoken *tok, size_t arg1, size_t arg2, size_t arg3, size_t arg4);
static void aseparser_set_top(aseparser *cx, size_t i);
static asepnode *aseparser_get_node_wr(aseparser *cx, size_t i);

%}

%error-verbose
%pure-parser /* bison */
%parse-param {aseparser *cx}
%lex-param {aseparser *cx}

%type     <refval> input
%type     <refval> block
%type     <refval> elsecl
%type     <refval> stmtlst
%type     <refval> stmt
%type     <refval> exp
%type     <refval> klslst
%type     <refval> klsdef
%type     <refval> fldlst
%type     <refval> flddef
%type     <refval> fldatrl
%type     <refval> fldattr
%type     <refval> idstr
%type     <refval> mtdargs
%type     <refval> arglst

%token    <tokval> NUMBER
%token    <tokval> STRING
%token    <tokval> IDENTIFIER
%token    <tokval> INVALIDTOK
%token    <tokval> TOK_KLASS
%token    <tokval> TOK_STATIC
%token    <tokval> TOK_PUBLIC
%token    <tokval> TOK_VAR
%token    <tokval> TOK_WHILE
%token    <tokval> TOK_DO
%token    <tokval> TOK_FOR
%token    <tokval> TOK_IF
%token    <tokval> TOK_ELSE
%token    <tokval> TOK_BREAK
%token    <tokval> TOK_CONTINUE
%token    <tokval> TOK_THROW
%token    <tokval> TOK_TRY
%token    <tokval> TOK_CATCH
%token    <tokval> TOK_RETURN
%token    <tokval> TOK_CPPBEGIN
%token    <tokval> TOK_CPPEND
%token    <tokval> TOK_CPPSTMT

%right    <tokval> ','
%right    <tokval> '=' ASSIGNOP
%right    <tokval> '?' ':'
%left     <tokval> OROR
%left     <tokval> ANDAND
%left     <tokval> '|'
%left     <tokval> '^'
%left     <tokval> '&'
%nonassoc <tokval> CMPOP
%left     <tokval> SHIFTOP
%left     <tokval> '+' '-'
%left     <tokval> '*' '/' '%'
%left     <tokval> '!' '~' UMINUS
%nonassoc <tokval> INCREMENT DECREMENT
%left     <tokval> '(' ')' '[' ']'
%left     <tokval> '.'

%%

input	: klslst
	  {
	    $$ = aseparser_create_node_1(cx, asestag_module, NULL, $1);
	    aseparser_set_top(cx, $$);
	  }
;
klslst	: /* empty */
	  { $$ = aseparser_create_node_0(cx, asestag_i_nil, NULL); }
	| klsdef klslst
	  { $$ = aseparser_create_node_2(cx, asestag_i_list, NULL, $1, $2); }
;
klsdef	: TOK_KLASS IDENTIFIER '{' fldlst '}'
	  { $$ = aseparser_create_node_1(cx, asestag_klass, &$2, $4); }
;
fldlst	: /* empty */
	  { $$ = aseparser_create_node_0(cx, asestag_i_nil, NULL); }
	| flddef fldlst
	  { $$ = aseparser_create_node_2(cx, asestag_i_list, NULL, $1, $2); }
;
flddef	: fldatrl TOK_VAR IDENTIFIER ';'
	  {
	    size_t n;
	    n = aseparser_create_node_1(cx, asestag_attr, NULL, $1);
	    $$ = aseparser_create_node_1(cx, asestag_var, &$3, n);
	  }
	| fldatrl IDENTIFIER mtdargs block
	  {
	    size_t n;
	    n = aseparser_create_node_1(cx, asestag_attr, NULL, $1);
	    $$ = aseparser_create_node_3(cx, asestag_method, &$2, n, $3, $4);
	  }
;
fldatrl	: /* empty */
	  { $$ = aseparser_create_node_0(cx, asestag_i_nil, NULL); }
	| fldattr fldatrl
	  { $$ = aseparser_create_node_2(cx, asestag_i_list, NULL, $1, $2); }
;
fldattr	: TOK_PUBLIC
	  { $$ = aseparser_create_node_0(cx, asestag_i_string, &$1); }
	| TOK_STATIC
	  { $$ = aseparser_create_node_0(cx, asestag_i_string, &$1); }
;
idstr	: IDENTIFIER
	  { $$ = aseparser_create_node_0(cx, asestag_i_string, &$1); }
;
mtdargs	: '(' ')'
	  {
	    size_t n;
	    n = aseparser_create_node_0(cx, asestag_i_nil, NULL);
	    $$ = aseparser_create_node_1(cx, asestag_args, &$1, n);
	  }
	| '(' arglst ')'
	  { $$ = aseparser_create_node_1(cx, asestag_args, &$1, $2); }
;
arglst	: idstr
	  {
	    size_t n;
	    n = aseparser_create_node_0(cx, asestag_i_nil, NULL);
	    $$ = aseparser_create_node_2(cx, asestag_i_list, NULL, $1, n);
	  }
	| idstr ',' arglst
	  { $$ = aseparser_create_node_2(cx, asestag_i_list, NULL, $1, $3); }
;
block	: '{' stmtlst '}'
	  { $$ = aseparser_create_node_1(cx, asestag_seq, NULL, $2); }
	| TOK_CPPBEGIN TOK_CPPSTMT TOK_CPPEND
	  { $$ = aseparser_create_node_0(cx, asestag_cpp, &$2); }
;
stmtlst	: /* empty */
	  { $$ = aseparser_create_node_0(cx, asestag_i_nil, NULL); }
	| stmt stmtlst
	  { $$ = aseparser_create_node_2(cx, asestag_i_list, NULL, $1, $2); }
;
stmt	: exp ';'
	  { $$ = $1; }
	| block
	  { $$ = $1; }
	| TOK_WHILE '(' exp ')' block
	  { $$ = aseparser_create_node_2(cx, asestag_while, &$1, $3, $5); }
	| TOK_DO block TOK_WHILE '(' exp ')' ';'
	  { $$ = aseparser_create_node_2(cx, asestag_dowhile, &$1, $2, $5); }
	| TOK_FOR '(' exp ';' exp ';' exp ')' block
	  { $$ = aseparser_create_node_4(cx, asestag_for,
	    &$1, $3, $5, $7, $9); }
	| TOK_RETURN exp ';'
	  { $$ = aseparser_create_node_1(cx, asestag_return, &$1, $2); }
	| TOK_BREAK ';'
	  { $$ = aseparser_create_node_0(cx, asestag_break, &$1); }
	| TOK_CONTINUE ';'
	  { $$ = aseparser_create_node_0(cx, asestag_continue, &$1); }
	| TOK_THROW exp ';'
	  { $$ = aseparser_create_node_1(cx, asestag_throw, &$1, $2); }
	| TOK_TRY block TOK_CATCH '(' IDENTIFIER ')' block
	  { $$ = aseparser_create_node_2(cx, asestag_trycatch, &$5, $2, $7); }
	| TOK_IF '(' exp ')' block elsecl
	  { $$ = aseparser_create_node_3(cx, asestag_if, &$1, $3, $5, $6); }
;
elsecl	: /* empty */
	  { $$ = aseparser_create_node_0(cx, asestag_i_nil, NULL); }
	| TOK_ELSE block
	  { $$ = $2; }
	| TOK_ELSE TOK_IF '(' exp ')' block elsecl
	  { $$ = aseparser_create_node_3(cx, asestag_if, &$2, $4, $6, $7); }
;
exp	: NUMBER
	  { $$ = aseparser_create_node_0(cx, asestag_i_number, &$1);  }
	| STRING
	  { $$ = aseparser_create_node_0(cx, asestag_i_string, &$1);  }
	| IDENTIFIER
	  { $$ = aseparser_create_node_0(cx, asestag_lref, &$1);  }
	| exp '+' exp
	  { $$ = aseparser_create_node_2(cx, asestag_op, &$2, $1, $3); }
	| exp '-' exp
	  { $$ = aseparser_create_node_2(cx, asestag_op, &$2, $1, $3); }
	| exp '*' exp
	  { $$ = aseparser_create_node_2(cx, asestag_op, &$2, $1, $3); }
	| exp '/' exp
	  { $$ = aseparser_create_node_2(cx, asestag_op, &$2, $1, $3); }
	| exp '%' exp
	  { $$ = aseparser_create_node_2(cx, asestag_op, &$2, $1, $3); }
	| exp '|' exp
	  { $$ = aseparser_create_node_2(cx, asestag_op, &$2, $1, $3); }
	| exp '^' exp
	  { $$ = aseparser_create_node_2(cx, asestag_op, &$2, $1, $3); }
	| exp '&' exp
	  { $$ = aseparser_create_node_2(cx, asestag_op, &$2, $1, $3); }
	| exp ASSIGNOP exp
	  { $$ = aseparser_create_node_2(cx, asestag_op, &$2, $1, $3); }
	| exp OROR exp
	  { $$ = aseparser_create_node_2(cx, asestag_op, &$2, $1, $3); }
	| exp ANDAND exp
	  { $$ = aseparser_create_node_2(cx, asestag_op, &$2, $1, $3); }
	| exp CMPOP exp
	  { $$ = aseparser_create_node_2(cx, asestag_op, &$2, $1, $3); }
	| exp SHIFTOP exp
	  { $$ = aseparser_create_node_2(cx, asestag_op, &$2, $1, $3); }
	| exp ',' exp
	  { $$ = aseparser_create_node_2(cx, asestag_op, &$2, $1, $3); }
	| exp '.' idstr
	  { $$ = aseparser_create_node_2(cx, asestag_get, &$2, $1, $3); }
	| '-' exp  %prec UMINUS
	  { $$ = aseparser_create_node_1(cx, asestag_op, &$1, $2); }
	| '+' exp  %prec UMINUS
	  { $$ = aseparser_create_node_1(cx, asestag_op, &$1, $2); }
	| '!' exp  %prec UMINUS
	  { $$ = aseparser_create_node_1(cx, asestag_op, &$1, $2); }
	| '~' exp  %prec UMINUS
	  { $$ = aseparser_create_node_1(cx, asestag_op, &$1, $2); }
	| INCREMENT exp
	  { $$ = aseparser_create_node_1(cx, asestag_op, &$1, $2); }
	| DECREMENT exp
	  { $$ = aseparser_create_node_1(cx, asestag_op, &$1, $2); }
	| '(' exp ')'
	  { $$ = aseparser_create_node_1(cx, asestag_op, NULL, $2); }
	| exp '(' ')'
	  { $$ = aseparser_create_node_1(cx, asestag_op, &$2, $1); }
	| exp '(' exp ')'
	  { $$ = aseparser_create_node_2(cx, asestag_op, &$2, $1, $3); }
	| exp '[' exp ']'
	  { $$ = aseparser_create_node_2(cx, asestag_get, &$2, $1, $3); }
;
 %%

#define ASEPARSER_NODE_PER_BUCKET 1024

#define DBG_TOK(x)
#define DBG_NODE(x)

typedef struct {
  asepnode node[ASEPARSER_NODE_PER_BUCKET];
} asepbucket;

struct aseparser_s {
  int oom_flag : 1;
  int cpp_flag : 1;
  char *filename; /* allocate */
  char *errstr; /* allocate */
  char *bufbegin, *bufend, *bufcur;
  size_t num_nodes;
  asepbucket **bucket; /* allocate */
  size_t top;
};

static aseparser *
aseparser_create_internal(char *filename /* move */,
  char *bufbegin /* move */, char *bufend)
{
  aseparser *cx;
  cx = malloc(sizeof(aseparser));
  if (cx == NULL) {
    free(filename);
    free(bufbegin);
    return NULL;
  }
  memset(cx, 0, sizeof(*cx));
  cx->oom_flag = 0;
  cx->cpp_flag = 0;
  cx->filename = filename;
  cx->errstr = NULL;
  cx->bufbegin = bufbegin;
  cx->bufend = bufend;
  cx->bufcur = bufbegin;
  /* node #0 */
  aseparser_create_node(cx, asestag_module, NULL, NULL, 0);
  if (cx->oom_flag) {
    aseparser_free(cx);
    return NULL;
  }
  return cx;
}

static size_t
aseparser_get_num_bucket(size_t num_nodes)
{
  return (num_nodes + ASEPARSER_NODE_PER_BUCKET - 1)
    / ASEPARSER_NODE_PER_BUCKET;
}

void
aseparser_free(aseparser *cx)
{
  size_t num_bucket, i;
  if (cx == NULL) {
    return;
  }
  for (i = 0; i < cx->num_nodes; ++i) {
    asepnode *p;
    p = aseparser_get_node_wr(cx, i);
    free(p->args);
  }
  num_bucket = aseparser_get_num_bucket(cx->num_nodes);
  for (i = 0; i < num_bucket; ++i) {
    free(cx->bucket[i]);
  }
  free(cx->bucket);
  free(cx->bufbegin);
  free(cx->errstr);
  free(cx->filename);
  free(cx);
}

asepnode *
aseparser_get_node_wr(aseparser *cx, size_t i)
{
  const size_t n = cx->num_nodes / ASEPARSER_NODE_PER_BUCKET;
  return cx->bucket[n]->node + i;
}

const asepnode *
aseparser_get_node(aseparser *cx, size_t i)
{
  return aseparser_get_node_wr(cx, i);
}

size_t
aseparser_get_top(aseparser *cx)
{
  return cx->top;
}

void
aseparser_set_top(aseparser *cx, size_t i)
{
  cx->top = i;
}

size_t
aseparser_create_node(aseparser *cx, asestag t, const asetoken *tok,
  size_t *args, size_t nargs)
{
  const size_t num_nodes = cx->num_nodes;
  const size_t num_nodes_new = num_nodes + 1;
  size_t num_bucket, num_bucket_new;
  asepnode *p;
  DBG_NODE(fprintf(stderr, "createnode %s\n", aseparser_tag2string(t)));
  num_bucket = aseparser_get_num_bucket(cx->num_nodes);
  num_bucket_new = aseparser_get_num_bucket(num_nodes_new);
  size_t j;
  if (cx->bucket == NULL || num_bucket != num_bucket_new) {
    asepbucket **bp, *b;
    bp = realloc(cx->bucket, num_bucket_new * sizeof(asepbucket *));
    if (bp == NULL) {
      goto oom;
    }
    cx->bucket = bp;
    b = malloc(sizeof(asepbucket));
    cx->bucket[num_bucket_new - 1] = b;
    if (b == NULL) {
      goto oom;
    }
  }
  p = aseparser_get_node_wr(cx, num_nodes);
  cx->num_nodes = num_nodes_new;
  memset(p, 0, sizeof(*p));
  p->tag = t;
  p->token.begin = NULL;
  p->token.end = NULL;
  p->args = NULL;
  p->nargs = 0;
  if (tok != NULL) {
    p->token = *tok;
  }
  if (nargs != 0) {
    p->args = malloc(nargs * sizeof(size_t));
    if (p->args == NULL) {
      goto oom;
    }
    for (j = 0; j < nargs; ++j) {
      p->args[j] = args[j];
    }
    p->nargs = nargs;
  }
  return num_nodes;

 oom:

  cx->oom_flag = 1;
  return 0;
}

size_t
aseparser_create_node_0(aseparser *cx, asestag t, const asetoken *tok)
{
  return aseparser_create_node(cx, t, tok, NULL, 0);
}

size_t
aseparser_create_node_1(aseparser *cx, asestag t, const asetoken *tok,
  size_t arg1)
{
  return aseparser_create_node(cx, t, tok, &arg1, 1);
}

size_t
aseparser_create_node_2(aseparser *cx, asestag t, const asetoken *tok,
  size_t arg1, size_t arg2)
{
  size_t args[2];
  args[0] = arg1;
  args[1] = arg2;
  return aseparser_create_node(cx, t, tok, args, 2);
}

size_t
aseparser_create_node_3(aseparser *cx, asestag t, const asetoken *tok,
  size_t arg1, size_t arg2, size_t arg3)
{
  size_t args[3];
  args[0] = arg1;
  args[1] = arg2;
  args[2] = arg3;
  return aseparser_create_node(cx, t, tok, args, 3);
}

size_t
aseparser_create_node_4(aseparser *cx, asestag t, const asetoken *tok,
  size_t arg1, size_t arg2, size_t arg3, size_t arg4)
{
  size_t args[4];
  args[0] = arg1;
  args[1] = arg2;
  args[2] = arg3;
  args[3] = arg4;
  return aseparser_create_node(cx, t, tok, args, 4);
}

const char *
aseparser_tag2string(asestag t)
{
  switch (t) {
  case asestag_module: return "Module";
  case asestag_klass: return "Class";
  case asestag_method: return "Method";
  case asestag_args: return "Args";
  case asestag_var: return "Var";
  case asestag_attr: return "Attr";
  case asestag_seq: return "Seq";
  case asestag_while: return "While";
  case asestag_dowhile: return "DoWhile";
  case asestag_for: return "For";
  case asestag_if: return "If";
  case asestag_break: return "Break";
  case asestag_continue: return "Continue";
  case asestag_throw: return "Throw";
  case asestag_trycatch: return "TryCatch";
  case asestag_return: return "Return";
  case asestag_lref: return "LRef";
  case asestag_gref: return "GRef";
  case asestag_let: return "Let";
  case asestag_get: return "Get";
  case asestag_set: return "Set";
  case asestag_minvoke: return "MInvoke";
  case asestag_op: return "Op";
  case asestag_closure: return "Closure";
  case asestag_array: return "Array";
  case asestag_map: return "Map";
  case asestag_cpp: return "Cpp";
  case asestag_i_string: return "$S";
  case asestag_i_number: return "$N";
  case asestag_i_list: return "$L";
  case asestag_i_nil: return "$E";
  }
  return "";
}

void
aseparser_print_node(aseparser *cx, size_t n, int indent, FILE *fp)
{
  const char *tagstr;
  const asepnode *nd;
  const asetoken *tk;
  int i;
  nd = aseparser_get_node(cx, n);
  tagstr = aseparser_tag2string(nd->tag);
  tk = &nd->token;
  for (i = 0; i < indent; ++i) {
    fputc(' ', fp);
  }
  fprintf(fp, "%s", tagstr);
  if (tk->begin != tk->end) {
    fputc(':', fp);
    fwrite(tk->begin, tk->end - tk->begin, 1, fp);
  }
  if (nd->nargs != 0) {
    size_t j;
    fputs("(\n", fp);
    for (j = 0; j < nd->nargs; ++j) {
      aseparser_print_node(cx, nd->args[j], indent + 1, fp);
    }
    for (i = 0; i < indent; ++i) {
      fputc(' ', fp);
    }
    fputc(')', fp);
  }
  fputc('\n', fp);
}

int
aseparser_peekchar(aseparser *cx)
{
  if (cx->bufcur < cx->bufend) {
    unsigned char c = (unsigned char )(*cx->bufcur);
    return (int)c;
  }
  return EOF;
}

void
aseparser_skip_space(aseparser *cx)
{
  int c;
  while ((c = aseparser_peekchar(cx)) != EOF &&
    (isspace(c) || iscntrl(c))) {
    cx->bufcur++;
  }
}

void
aseparser_skip_numeric(aseparser *cx)
{
  double v = 0;
  int cnt = 0;
  sscanf(cx->bufcur, "%lf%n", &v, &cnt);
  if (cnt > 0) {
    cx->bufcur += cnt;
  }
}

void
aseparser_skip_alnum(aseparser *cx)
{
  int c;
  while ((c = aseparser_peekchar(cx)) != EOF &&
    (isalnum(c) || c == '_')) {
    cx->bufcur++;
  }
}

void
aseparser_skip_strliteral(aseparser *cx)
{
  int qc, c, escaped = 0;
  qc = aseparser_peekchar(cx); /* quote char */
  if (qc == EOF) {
    return;
  }
  cx->bufcur++; /* skip quote char */
  while ((c = aseparser_peekchar(cx)) != EOF) {
    cx->bufcur++;
    if (!escaped) {
      if (c == qc) {
	break;
      }
      escaped = (c == '\\');
    } else {
      escaped = 0;
    }
  }
}

void
aseparser_skip_cpp(aseparser *cx)
{
  int c, p = 0;
  while ((c = aseparser_peekchar(cx)) != EOF && (p != '}' || c != '}')) {
    p = c;
    cx->bufcur++;
  }
  if (c == '}' && p == '}') {
    cx->bufcur--;
  }
}

struct aseparser_reserved {
  const char *name;
  int token;
};

static struct aseparser_reserved reserved_tbl[] = {
  { "class", TOK_KLASS },
  { "static", TOK_STATIC },
  { "public", TOK_PUBLIC },
  { "var", TOK_VAR },
  { "while", TOK_WHILE },
  { "do", TOK_DO },
  { "for", TOK_FOR },
  { "if", TOK_IF },
  { "else", TOK_ELSE },
  { "break", TOK_BREAK },
  { "continue", TOK_CONTINUE },
  { "throw", TOK_THROW },
  { "try", TOK_TRY },
  { "catch", TOK_CATCH },
  { "return", TOK_RETURN },
};

static int
aseparser_find_reserved(const char *begin, const char *end)
{
  const char *const tokname = begin;
  const size_t toklen = end - begin;
  const size_t n = sizeof(reserved_tbl) / sizeof(reserved_tbl[0]);
  size_t i;
  for (i = 0; i < n; ++i) {
    const struct aseparser_reserved *const p = reserved_tbl + i;
    if (strncmp(p->name, tokname, toklen) == 0 &&
      p->name[toklen] == '\0') {
      return p->token;
    }
  }
  return IDENTIFIER;
}

int
aseparser_read_operator(aseparser *cx)
{
  int r0 = 0, c0, c1;
  c0 = aseparser_peekchar(cx);
  if (c0 == EOF) {
    return 0;
  }
  cx->bufcur++;
  switch (c0) {
  case ';':
  case ',':
  case '?':
  case ':':
  case '~':
  case '(':
  case ')':
  case '[':
  case ']':
  case '.':
    return c0;
  case '{':
  case '}':
  case '|':
  case '^':
  case '&':
  case '+':
  case '-':
  case '*':
  case '/':
  case '%':
  case '!':
    r0 = c0;
    break;
  case '>':
  case '<':
    r0 = CMPOP;
    break;
  case '=':
    r0 = ASSIGNOP;
    break;
  default:
    return INVALIDTOK;
  }
  c1 = aseparser_peekchar(cx);
  if (c1 == '=') {
    int r1 = 0;
    switch (c0) {
    case '=':
    case '!':
    case '<':
    case '>':
      r1 = CMPOP;
      break;
    case '|':
    case '^':
    case '&':
    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
      r1 = ASSIGNOP;
      break;
    }
    if (r1 != 0) {
      cx->bufcur++;
      return r1;
    }
  }
  if (c0 == c1) {
    int r1 = 0;
    switch (c0) {
    case '|':
      r1 = OROR;
      break;
    case '&':
      r1 = ANDAND;
      break;
    case '<':
    case '>':
      r1 = SHIFTOP;
      break;
    case '+':
      r1 = INCREMENT;
      break;
    case '-':
      r1 = DECREMENT;
      break;
    case '{':
      r1 = TOK_CPPBEGIN;
      break;
    case '}':
      r1 = TOK_CPPEND;
      break;
    }
    if (r1 != 0) {
      cx->bufcur++;
      return r1;
    }
  }
  return r0;
}

void
aseparser_get_location(aseparser *cx, const char *pos, int *line_r,
  int *col_r)
{
  const char *p = cx->bufbegin;
  int line = 1;
  int col = 1;
  (*line_r) = 0;
  (*col_r) = 0;
  while (p != pos) {
    if ((*p) == '\n') {
      ++line;
      col = 1;
    } else {
      ++col;
    }
    ++p;
  }
  (*line_r) = line;
  (*col_r) = col;
}

int
aseparser_read_token(aseparser *cx, asetoken *tok_r)
{
  int c = 0;
  int r = 0;
  const char *begin = NULL;
  tok_r->begin = 0;
  tok_r->end = 0;
  aseparser_skip_space(cx);
  begin = cx->bufcur;
  c = aseparser_peekchar(cx);
  if (c == EOF) {
    DBG_TOK(fprintf(stderr, "tok eof\n"));
    return 0;
  }
  if (cx->cpp_flag) {
    aseparser_skip_cpp(cx);
    r = TOK_CPPSTMT;
    cx->cpp_flag = 0;
  } else if (isdigit(c)) {
    aseparser_skip_numeric(cx);
    r = NUMBER;
    DBG_TOK(fprintf(stderr, "tok number\n"));
  } else if (c == '\"' || c == '\'') {
    aseparser_skip_strliteral(cx);
    r = STRING;
    DBG_TOK(fprintf(stderr, "tok string\n"));
  } else if (c == '_' || isalpha(c)) {
    aseparser_skip_alnum(cx);
    r = aseparser_find_reserved(begin, cx->bufcur);
    DBG_TOK(fprintf(stderr, "tok identifier-or-reserved\n"));
  } else if (ispunct(c)) {
    r = aseparser_read_operator(cx);
    if (r == TOK_CPPBEGIN) {
      cx->cpp_flag = 1;
    }
    DBG_TOK(fprintf(stderr, "tok punct\n"));
  } else {
    r = INVALIDTOK;
    DBG_TOK(fprintf(stderr, "tok invalid\n"));
  }
  tok_r->begin = begin;
  tok_r->end = cx->bufcur;
  DBG_TOK(fprintf(stderr, "tok %02x ", r));
  DBG_TOK(fwrite(tok_r->begin, tok_r->end - tok_r->begin, 1, stderr));
  DBG_TOK(fprintf(stderr, "\n"));
  return r;
}

const char *
aseparser_get_error(aseparser *cx, int *line_r, int *col_r)
{
  const char *fname = cx->filename;
  if (fname == NULL) {
    fname = "";
  }
  aseparser_get_location(cx, cx->bufcur, line_r, col_r);
  if (cx->oom_flag) {
    return "out of memory";
  }
  if (cx->errstr == NULL) {
    return NULL;
  }
  return cx->errstr;
}

void
aseparser_print_error(aseparser *cx, FILE *fp)
{
  const char *fname = cx->filename;
  if (fname == NULL) {
    fname = "";
  }
  int line = 0;
  int col = 0;
  aseparser_get_location(cx, cx->bufcur, &line, &col);
  if (cx->oom_flag) {
    fprintf(fp, "%s(%d.%d): out of memory\n", fname, line, col);
    return;
  }
  if (cx->errstr == NULL) {
    fprintf(fp, "%s(%d.%d): no error\n", fname, line, col);
    return;
  }
  fprintf(fp, "%s(%d.%d): %s\n", fname, line, col, cx->errstr);
}

int yylex(YYSTYPE *lvalp, aseparser *cx)
{
  return aseparser_read_token(cx, &lvalp->tokval);
}

void
yyerror(aseparser *cx, const char *s)
{
  char *cpy;
  if (cx->errstr != NULL) {
    return;
  }
  cpy = strdup(s);
  if (cpy == NULL) {
    cx->oom_flag = 1;
    return;
  }
  cx->errstr = cpy;
}

static char *
create_buffer_from_file(FILE *fp, size_t *len_r)
{
  char *buf = NULL;
  size_t len = 0;
  size_t n;
  if (len_r != NULL) {
    (*len_r) = 0;
  }
  buf = malloc(1024);
  if (buf == NULL) {
    return NULL;
  }
  while ((n = fread(buf + len, 1, 1024, fp)) > 0) {
    size_t nlen;
    char *nbuf;
    nlen = len + n;
    nbuf = realloc(buf, nlen + 1024);
    if (nbuf == NULL) {
      free(buf);
      return NULL;
    }
    buf = nbuf;
    len = nlen;
  }
  buf[len] = '\0';
  if (len_r != NULL) {
    (*len_r) = len;
  }
  return buf;
}

aseparser *
aseparser_create(const char *filename, int *notfound_r)
{
  char *fname_cpy = NULL;
  FILE *fp = NULL;
  char *buf = NULL;
  size_t buflen = 0;
  *notfound_r = 0;
  fp = fopen(filename, "r");
  if (fp == NULL) {
    *notfound_r = 1;
    return NULL;
  }
  buf = create_buffer_from_file(fp, &buflen);
  fclose(fp);
  fp = NULL;
  if (buf == NULL) {
    return NULL;
  }
  fname_cpy = strdup(filename);
  if (fname_cpy == NULL) {
    free(buf);
    return NULL;
  }
  return aseparser_create_internal(fname_cpy, buf, buf + buflen);
}

int
aseparser_parse(aseparser *cx)
{
  return yyparse(cx);
}

int
aseparser_is_oom(aseparser *cx)
{
  return cx->oom_flag;
}

