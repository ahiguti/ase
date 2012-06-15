/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     NUMBER = 258,
     STRING = 259,
     IDENTIFIER = 260,
     INVALIDTOK = 261,
     TOK_KLASS = 262,
     TOK_STATIC = 263,
     TOK_PUBLIC = 264,
     TOK_VAR = 265,
     TOK_WHILE = 266,
     TOK_DO = 267,
     TOK_FOR = 268,
     TOK_IF = 269,
     TOK_ELSE = 270,
     TOK_BREAK = 271,
     TOK_CONTINUE = 272,
     TOK_THROW = 273,
     TOK_TRY = 274,
     TOK_CATCH = 275,
     TOK_RETURN = 276,
     TOK_CPPBEGIN = 277,
     TOK_CPPEND = 278,
     TOK_CPPSTMT = 279,
     ASSIGNOP = 280,
     OROR = 281,
     ANDAND = 282,
     CMPOP = 283,
     SHIFTOP = 284,
     UMINUS = 285,
     DECREMENT = 286,
     INCREMENT = 287
   };
#endif
/* Tokens.  */
#define NUMBER 258
#define STRING 259
#define IDENTIFIER 260
#define INVALIDTOK 261
#define TOK_KLASS 262
#define TOK_STATIC 263
#define TOK_PUBLIC 264
#define TOK_VAR 265
#define TOK_WHILE 266
#define TOK_DO 267
#define TOK_FOR 268
#define TOK_IF 269
#define TOK_ELSE 270
#define TOK_BREAK 271
#define TOK_CONTINUE 272
#define TOK_THROW 273
#define TOK_TRY 274
#define TOK_CATCH 275
#define TOK_RETURN 276
#define TOK_CPPBEGIN 277
#define TOK_CPPEND 278
#define TOK_CPPSTMT 279
#define ASSIGNOP 280
#define OROR 281
#define ANDAND 282
#define CMPOP 283
#define SHIFTOP 284
#define UMINUS 285
#define DECREMENT 286
#define INCREMENT 287




/* Copy the first part of user declarations.  */
#line 3 "aseparser.y"


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "aseparser.h"



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 15 "aseparser.y"
{
  size_t refval;
  asetoken tokval;
}
/* Line 187 of yacc.c.  */
#line 177 "aseparser.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */
#line 20 "aseparser.y"


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



/* Line 216 of yacc.c.  */
#line 212 "aseparser.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   643

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  55
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  16
/* YYNRULES -- Number of rules.  */
#define YYNRULES  64
/* YYNRULES -- Number of states.  */
#define YYNSTATES  150

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   287

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    42,     2,     2,     2,    41,    34,     2,
      47,    48,    39,    37,    25,    38,    51,    40,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    29,    54,
       2,    26,     2,    28,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    49,     2,    50,    33,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    52,    32,    53,    43,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      27,    30,    31,    35,    36,    44,    45,    46
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    15,    16,    19,    24,
      29,    30,    33,    35,    37,    39,    42,    46,    48,    52,
      56,    60,    61,    64,    67,    69,    75,    83,    93,    97,
     100,   103,   107,   115,   122,   123,   126,   134,   136,   138,
     140,   144,   148,   152,   156,   160,   164,   168,   172,   176,
     180,   184,   188,   192,   196,   200,   203,   206,   209,   212,
     215,   218,   222,   226,   231
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      56,     0,    -1,    57,    -1,    -1,    58,    57,    -1,     7,
       5,    52,    59,    53,    -1,    -1,    60,    59,    -1,    61,
      10,     5,    54,    -1,    61,     5,    64,    66,    -1,    -1,
      62,    61,    -1,     9,    -1,     8,    -1,     5,    -1,    47,
      48,    -1,    47,    65,    48,    -1,    63,    -1,    63,    25,
      65,    -1,    52,    67,    53,    -1,    22,    24,    23,    -1,
      -1,    68,    67,    -1,    70,    54,    -1,    66,    -1,    11,
      47,    70,    48,    66,    -1,    12,    66,    11,    47,    70,
      48,    54,    -1,    13,    47,    70,    54,    70,    54,    70,
      48,    66,    -1,    21,    70,    54,    -1,    16,    54,    -1,
      17,    54,    -1,    18,    70,    54,    -1,    19,    66,    20,
      47,     5,    48,    66,    -1,    14,    47,    70,    48,    66,
      69,    -1,    -1,    15,    66,    -1,    15,    14,    47,    70,
      48,    66,    69,    -1,     3,    -1,     4,    -1,     5,    -1,
      70,    37,    70,    -1,    70,    38,    70,    -1,    70,    39,
      70,    -1,    70,    40,    70,    -1,    70,    41,    70,    -1,
      70,    32,    70,    -1,    70,    33,    70,    -1,    70,    34,
      70,    -1,    70,    27,    70,    -1,    70,    30,    70,    -1,
      70,    31,    70,    -1,    70,    35,    70,    -1,    70,    36,
      70,    -1,    70,    25,    70,    -1,    70,    51,    63,    -1,
      38,    70,    -1,    37,    70,    -1,    42,    70,    -1,    43,
      70,    -1,    46,    70,    -1,    45,    70,    -1,    47,    70,
      48,    -1,    70,    47,    48,    -1,    70,    47,    70,    48,
      -1,    70,    49,    70,    50,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   106,   106,   113,   114,   117,   121,   122,   125,   131,
     139,   140,   143,   145,   148,   151,   157,   160,   166,   169,
     171,   175,   176,   179,   181,   183,   185,   187,   190,   192,
     194,   196,   198,   200,   204,   205,   207,   210,   212,   214,
     216,   218,   220,   222,   224,   226,   228,   230,   232,   234,
     236,   238,   240,   242,   244,   246,   248,   250,   252,   254,
     256,   258,   260,   262,   264
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NUMBER", "STRING", "IDENTIFIER",
  "INVALIDTOK", "TOK_KLASS", "TOK_STATIC", "TOK_PUBLIC", "TOK_VAR",
  "TOK_WHILE", "TOK_DO", "TOK_FOR", "TOK_IF", "TOK_ELSE", "TOK_BREAK",
  "TOK_CONTINUE", "TOK_THROW", "TOK_TRY", "TOK_CATCH", "TOK_RETURN",
  "TOK_CPPBEGIN", "TOK_CPPEND", "TOK_CPPSTMT", "','", "'='", "ASSIGNOP",
  "'?'", "':'", "OROR", "ANDAND", "'|'", "'^'", "'&'", "CMPOP", "SHIFTOP",
  "'+'", "'-'", "'*'", "'/'", "'%'", "'!'", "'~'", "UMINUS", "DECREMENT",
  "INCREMENT", "'('", "')'", "'['", "']'", "'.'", "'{'", "'}'", "';'",
  "$accept", "input", "klslst", "klsdef", "fldlst", "flddef", "fldatrl",
  "fldattr", "idstr", "mtdargs", "arglst", "block", "stmtlst", "stmt",
  "elsecl", "exp", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,    44,    61,   280,    63,    58,
     281,   282,   124,    94,    38,   283,   284,    43,    45,    42,
      47,    37,    33,   126,   285,   286,   287,    40,    41,    91,
      93,    46,   123,   125,    59
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    55,    56,    57,    57,    58,    59,    59,    60,    60,
      61,    61,    62,    62,    63,    64,    64,    65,    65,    66,
      66,    67,    67,    68,    68,    68,    68,    68,    68,    68,
      68,    68,    68,    68,    69,    69,    69,    70,    70,    70,
      70,    70,    70,    70,    70,    70,    70,    70,    70,    70,
      70,    70,    70,    70,    70,    70,    70,    70,    70,    70,
      70,    70,    70,    70,    70
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     5,     0,     2,     4,     4,
       0,     2,     1,     1,     1,     2,     3,     1,     3,     3,
       3,     0,     2,     2,     1,     5,     7,     9,     3,     2,
       2,     3,     7,     6,     0,     2,     7,     1,     1,     1,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     2,     2,     2,     2,
       2,     3,     3,     4,     4
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     0,     2,     3,     0,     1,     4,    10,    13,
      12,     0,    10,     0,    10,     5,     7,     0,     0,    11,
       0,     0,     0,    14,    15,    17,     0,     0,    21,     9,
       8,     0,    16,     0,    37,    38,    39,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    24,     0,    21,     0,    18,    20,     0,
       0,     0,     0,    29,    30,     0,     0,     0,    56,    55,
      57,    58,    60,    59,     0,    19,    22,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    23,     0,     0,     0,     0,    31,
       0,    28,    61,    53,    48,    49,    50,    45,    46,    47,
      51,    52,    40,    41,    42,    43,    44,    62,     0,     0,
      54,     0,     0,     0,     0,     0,    63,    64,    25,     0,
       0,    34,     0,     0,     0,     0,    33,     0,    26,     0,
       0,    35,    32,     0,     0,    27,     0,     0,    34,    36
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,     4,    11,    12,    13,    14,    25,    21,
      26,    53,    54,    55,   136,    56
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -41
static const yytype_int16 yypact[] =
{
      -6,    -1,    24,   -41,    -6,   -24,   -41,   -41,     5,   -41,
     -41,   -19,     5,    10,    22,   -41,   -41,   -14,    30,   -41,
      11,   -20,   -18,   -41,   -41,    32,    12,    41,    50,   -41,
     -41,    65,   -41,    51,   -41,   -41,   -41,    26,   -20,    28,
      29,    23,    25,   102,   -20,   102,   102,   102,   102,   102,
     102,   102,   102,   -41,    27,    50,   125,   -41,   -41,   102,
      67,   102,   102,   -41,   -41,   150,    61,   175,   -22,   -22,
     -22,   -22,   -22,   -22,   250,   -41,   -41,   102,   102,   102,
     102,   102,   102,   102,   102,   102,   102,   102,   102,   102,
     102,    86,   102,    65,   -41,   275,    37,   200,   300,   -41,
      38,   -41,   -41,   452,   475,   496,   516,   535,   553,   570,
     587,   592,    70,    70,   -22,   -22,   -22,   -41,   325,   350,
     -41,   -20,   102,   102,   -20,    81,   -41,   -41,   -41,   377,
     225,    83,    53,    45,   102,     4,   -41,   -20,   -41,   402,
      66,   -41,   -41,   -20,   102,   -41,   427,   -20,    83,   -41
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -41,   -41,   104,   -41,   100,   -41,   101,   -41,    34,   -41,
      87,   -21,    75,   -41,   -28,   -40
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -7
static const yytype_int16 yytable[] =
{
      29,     1,    27,    65,     5,    67,    68,    69,    70,    71,
      72,    73,    74,     9,    10,    17,    23,    60,   140,    95,
      18,    97,    98,    66,     6,    91,    27,    92,     8,    93,
       9,    10,    28,    20,    15,    22,    30,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   118,   119,    34,    35,    36,    28,    31,    -6,    24,
      32,    37,    38,    39,    40,    33,    41,    42,    43,    44,
      23,    45,    27,    59,    58,    61,    62,    63,    96,    64,
      75,   100,   129,   130,   122,   125,   132,    46,    47,    34,
      35,    36,    48,    49,   139,    50,    51,    52,   135,   138,
     128,   137,    28,   131,   146,    34,    35,    36,     7,    88,
      89,    90,    16,   144,   141,    19,   142,    91,    57,    92,
     149,    93,   145,    46,    47,     0,   148,   120,    48,    49,
      76,    50,    51,    52,   117,     0,     0,     0,     0,    46,
      47,     0,     0,     0,    48,    49,     0,    50,    51,    52,
      77,     0,    78,     0,     0,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,     0,     0,     0,
       0,     0,    91,     0,    92,    77,    93,    78,     0,    94,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,     0,     0,     0,     0,     0,    91,     0,    92,
      77,    93,    78,     0,    99,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,     0,     0,     0,
       0,     0,    91,     0,    92,    77,    93,    78,     0,   101,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,     0,     0,     0,     0,     0,    91,     0,    92,
      77,    93,    78,     0,   123,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,     0,     0,     0,
       0,     0,    91,     0,    92,    77,    93,    78,     0,   134,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,     0,     0,     0,     0,     0,    91,   102,    92,
      77,    93,    78,     0,     0,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,     0,     0,     0,
       0,     0,    91,   121,    92,    77,    93,    78,     0,     0,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,     0,     0,     0,     0,     0,    91,   124,    92,
      77,    93,    78,     0,     0,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,     0,     0,     0,
       0,     0,    91,   126,    92,    77,    93,    78,     0,     0,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,     0,     0,     0,     0,     0,    91,     0,    92,
     127,    93,    77,     0,    78,     0,     0,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,     0,
       0,     0,     0,     0,    91,   133,    92,    77,    93,    78,
       0,     0,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,     0,     0,     0,     0,     0,    91,
     143,    92,    77,    93,    78,     0,     0,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,     0,
       0,     0,     0,     0,    91,   147,    92,    77,    93,    78,
       0,     0,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,     0,     0,     0,     0,     0,    91,
       0,    92,    78,    93,     0,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,     0,     0,     0,
       0,     0,    91,     0,    92,     0,    93,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,     0,     0,
       0,     0,     0,    91,     0,    92,     0,    93,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,     0,     0,
       0,     0,     0,    91,     0,    92,     0,    93,    82,    83,
      84,    85,    86,    87,    88,    89,    90,     0,     0,     0,
       0,     0,    91,     0,    92,     0,    93,    83,    84,    85,
      86,    87,    88,    89,    90,     0,     0,     0,     0,     0,
      91,     0,    92,     0,    93,    84,    85,    86,    87,    88,
      89,    90,     0,     0,     0,     0,     0,    91,     0,    92,
       0,    93,    -7,    85,    86,    87,    88,    89,    90,    86,
      87,    88,    89,    90,    91,     0,    92,     0,    93,    91,
       0,    92,     0,    93
};

static const yytype_int16 yycheck[] =
{
      21,     7,    22,    43,     5,    45,    46,    47,    48,    49,
      50,    51,    52,     8,     9,     5,     5,    38,    14,    59,
      10,    61,    62,    44,     0,    47,    22,    49,    52,    51,
       8,     9,    52,    47,    53,     5,    54,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,     3,     4,     5,    52,    25,    53,    48,
      48,    11,    12,    13,    14,    24,    16,    17,    18,    19,
       5,    21,    22,    47,    23,    47,    47,    54,    11,    54,
      53,    20,   122,   123,    47,    47,     5,    37,    38,     3,
       4,     5,    42,    43,   134,    45,    46,    47,    15,    54,
     121,    48,    52,   124,   144,     3,     4,     5,     4,    39,
      40,    41,    12,    47,   135,    14,   137,    47,    31,    49,
     148,    51,   143,    37,    38,    -1,   147,    93,    42,    43,
      55,    45,    46,    47,    48,    -1,    -1,    -1,    -1,    37,
      38,    -1,    -1,    -1,    42,    43,    -1,    45,    46,    47,
      25,    -1,    27,    -1,    -1,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    -1,    -1,    -1,
      -1,    -1,    47,    -1,    49,    25,    51,    27,    -1,    54,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    -1,    -1,    -1,    -1,    -1,    47,    -1,    49,
      25,    51,    27,    -1,    54,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    -1,    -1,    -1,
      -1,    -1,    47,    -1,    49,    25,    51,    27,    -1,    54,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    -1,    -1,    -1,    -1,    -1,    47,    -1,    49,
      25,    51,    27,    -1,    54,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    -1,    -1,    -1,
      -1,    -1,    47,    -1,    49,    25,    51,    27,    -1,    54,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    -1,    -1,    -1,    -1,    -1,    47,    48,    49,
      25,    51,    27,    -1,    -1,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    -1,    -1,    -1,
      -1,    -1,    47,    48,    49,    25,    51,    27,    -1,    -1,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    -1,    -1,    -1,    -1,    -1,    47,    48,    49,
      25,    51,    27,    -1,    -1,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    -1,    -1,    -1,
      -1,    -1,    47,    48,    49,    25,    51,    27,    -1,    -1,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    -1,    -1,    -1,    -1,    -1,    47,    -1,    49,
      50,    51,    25,    -1,    27,    -1,    -1,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    -1,
      -1,    -1,    -1,    -1,    47,    48,    49,    25,    51,    27,
      -1,    -1,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    -1,    -1,    -1,    -1,    -1,    47,
      48,    49,    25,    51,    27,    -1,    -1,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    -1,
      -1,    -1,    -1,    -1,    47,    48,    49,    25,    51,    27,
      -1,    -1,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    -1,    -1,    -1,    -1,    -1,    47,
      -1,    49,    27,    51,    -1,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    -1,    -1,    -1,
      -1,    -1,    47,    -1,    49,    -1,    51,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    -1,    -1,
      -1,    -1,    -1,    47,    -1,    49,    -1,    51,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    -1,    -1,
      -1,    -1,    -1,    47,    -1,    49,    -1,    51,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    -1,    -1,    -1,
      -1,    -1,    47,    -1,    49,    -1,    51,    34,    35,    36,
      37,    38,    39,    40,    41,    -1,    -1,    -1,    -1,    -1,
      47,    -1,    49,    -1,    51,    35,    36,    37,    38,    39,
      40,    41,    -1,    -1,    -1,    -1,    -1,    47,    -1,    49,
      -1,    51,    35,    36,    37,    38,    39,    40,    41,    37,
      38,    39,    40,    41,    47,    -1,    49,    -1,    51,    47,
      -1,    49,    -1,    51
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     7,    56,    57,    58,     5,     0,    57,    52,     8,
       9,    59,    60,    61,    62,    53,    59,     5,    10,    61,
      47,    64,     5,     5,    48,    63,    65,    22,    52,    66,
      54,    25,    48,    24,     3,     4,     5,    11,    12,    13,
      14,    16,    17,    18,    19,    21,    37,    38,    42,    43,
      45,    46,    47,    66,    67,    68,    70,    65,    23,    47,
      66,    47,    47,    54,    54,    70,    66,    70,    70,    70,
      70,    70,    70,    70,    70,    53,    67,    25,    27,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    47,    49,    51,    54,    70,    11,    70,    70,    54,
      20,    54,    48,    70,    70,    70,    70,    70,    70,    70,
      70,    70,    70,    70,    70,    70,    70,    48,    70,    70,
      63,    48,    47,    54,    48,    47,    48,    50,    66,    70,
      70,    66,     5,    48,    54,    15,    69,    48,    54,    70,
      14,    66,    66,    48,    47,    66,    70,    48,    66,    69
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (cx, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, cx)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, value, cx); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, aseparser *cx)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, cx)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    aseparser *cx;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (cx);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, aseparser *cx)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, cx)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    aseparser *cx;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, cx);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule, aseparser *cx)
#else
static void
yy_reduce_print (yyvsp, yyrule, cx)
    YYSTYPE *yyvsp;
    int yyrule;
    aseparser *cx;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       , cx);
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule, cx); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, aseparser *cx)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, cx)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    aseparser *cx;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (cx);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (aseparser *cx);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */






/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (aseparser *cx)
#else
int
yyparse (cx)
    aseparser *cx;
#endif
#endif
{
  /* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;

  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 107 "aseparser.y"
    {
	    (yyval.refval) = aseparser_create_node_1(cx, asestag_module, NULL, (yyvsp[(1) - (1)].refval));
	    aseparser_set_top(cx, (yyval.refval));
	  ;}
    break;

  case 3:
#line 113 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_0(cx, asestag_i_nil, NULL); ;}
    break;

  case 4:
#line 115 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_2(cx, asestag_i_list, NULL, (yyvsp[(1) - (2)].refval), (yyvsp[(2) - (2)].refval)); ;}
    break;

  case 5:
#line 118 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_1(cx, asestag_klass, &(yyvsp[(2) - (5)].tokval), (yyvsp[(4) - (5)].refval)); ;}
    break;

  case 6:
#line 121 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_0(cx, asestag_i_nil, NULL); ;}
    break;

  case 7:
#line 123 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_2(cx, asestag_i_list, NULL, (yyvsp[(1) - (2)].refval), (yyvsp[(2) - (2)].refval)); ;}
    break;

  case 8:
#line 126 "aseparser.y"
    {
	    size_t n;
	    n = aseparser_create_node_1(cx, asestag_attr, NULL, (yyvsp[(1) - (4)].refval));
	    (yyval.refval) = aseparser_create_node_1(cx, asestag_var, &(yyvsp[(3) - (4)].tokval), n);
	  ;}
    break;

  case 9:
#line 132 "aseparser.y"
    {
	    size_t n;
	    n = aseparser_create_node_1(cx, asestag_attr, NULL, (yyvsp[(1) - (4)].refval));
	    (yyval.refval) = aseparser_create_node_3(cx, asestag_method, &(yyvsp[(2) - (4)].tokval), n, (yyvsp[(3) - (4)].refval), (yyvsp[(4) - (4)].refval));
	  ;}
    break;

  case 10:
#line 139 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_0(cx, asestag_i_nil, NULL); ;}
    break;

  case 11:
#line 141 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_2(cx, asestag_i_list, NULL, (yyvsp[(1) - (2)].refval), (yyvsp[(2) - (2)].refval)); ;}
    break;

  case 12:
#line 144 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_0(cx, asestag_i_string, &(yyvsp[(1) - (1)].tokval)); ;}
    break;

  case 13:
#line 146 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_0(cx, asestag_i_string, &(yyvsp[(1) - (1)].tokval)); ;}
    break;

  case 14:
#line 149 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_0(cx, asestag_i_string, &(yyvsp[(1) - (1)].tokval)); ;}
    break;

  case 15:
#line 152 "aseparser.y"
    {
	    size_t n;
	    n = aseparser_create_node_0(cx, asestag_i_nil, NULL);
	    (yyval.refval) = aseparser_create_node_1(cx, asestag_args, &(yyvsp[(1) - (2)].tokval), n);
	  ;}
    break;

  case 16:
#line 158 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_1(cx, asestag_args, &(yyvsp[(1) - (3)].tokval), (yyvsp[(2) - (3)].refval)); ;}
    break;

  case 17:
#line 161 "aseparser.y"
    {
	    size_t n;
	    n = aseparser_create_node_0(cx, asestag_i_nil, NULL);
	    (yyval.refval) = aseparser_create_node_2(cx, asestag_i_list, NULL, (yyvsp[(1) - (1)].refval), n);
	  ;}
    break;

  case 18:
#line 167 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_2(cx, asestag_i_list, NULL, (yyvsp[(1) - (3)].refval), (yyvsp[(3) - (3)].refval)); ;}
    break;

  case 19:
#line 170 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_1(cx, asestag_seq, NULL, (yyvsp[(2) - (3)].refval)); ;}
    break;

  case 20:
#line 172 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_0(cx, asestag_cpp, &(yyvsp[(2) - (3)].tokval)); ;}
    break;

  case 21:
#line 175 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_0(cx, asestag_i_nil, NULL); ;}
    break;

  case 22:
#line 177 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_2(cx, asestag_i_list, NULL, (yyvsp[(1) - (2)].refval), (yyvsp[(2) - (2)].refval)); ;}
    break;

  case 23:
#line 180 "aseparser.y"
    { (yyval.refval) = (yyvsp[(1) - (2)].refval); ;}
    break;

  case 24:
#line 182 "aseparser.y"
    { (yyval.refval) = (yyvsp[(1) - (1)].refval); ;}
    break;

  case 25:
#line 184 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_2(cx, asestag_while, &(yyvsp[(1) - (5)].tokval), (yyvsp[(3) - (5)].refval), (yyvsp[(5) - (5)].refval)); ;}
    break;

  case 26:
#line 186 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_2(cx, asestag_dowhile, &(yyvsp[(1) - (7)].tokval), (yyvsp[(2) - (7)].refval), (yyvsp[(5) - (7)].refval)); ;}
    break;

  case 27:
#line 188 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_4(cx, asestag_for,
	    &(yyvsp[(1) - (9)].tokval), (yyvsp[(3) - (9)].refval), (yyvsp[(5) - (9)].refval), (yyvsp[(7) - (9)].refval), (yyvsp[(9) - (9)].refval)); ;}
    break;

  case 28:
#line 191 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_1(cx, asestag_return, &(yyvsp[(1) - (3)].tokval), (yyvsp[(2) - (3)].refval)); ;}
    break;

  case 29:
#line 193 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_0(cx, asestag_break, &(yyvsp[(1) - (2)].tokval)); ;}
    break;

  case 30:
#line 195 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_0(cx, asestag_continue, &(yyvsp[(1) - (2)].tokval)); ;}
    break;

  case 31:
#line 197 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_1(cx, asestag_throw, &(yyvsp[(1) - (3)].tokval), (yyvsp[(2) - (3)].refval)); ;}
    break;

  case 32:
#line 199 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_2(cx, asestag_trycatch, &(yyvsp[(5) - (7)].tokval), (yyvsp[(2) - (7)].refval), (yyvsp[(7) - (7)].refval)); ;}
    break;

  case 33:
#line 201 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_3(cx, asestag_if, &(yyvsp[(1) - (6)].tokval), (yyvsp[(3) - (6)].refval), (yyvsp[(5) - (6)].refval), (yyvsp[(6) - (6)].refval)); ;}
    break;

  case 34:
#line 204 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_0(cx, asestag_i_nil, NULL); ;}
    break;

  case 35:
#line 206 "aseparser.y"
    { (yyval.refval) = (yyvsp[(2) - (2)].refval); ;}
    break;

  case 36:
#line 208 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_3(cx, asestag_if, &(yyvsp[(2) - (7)].tokval), (yyvsp[(4) - (7)].refval), (yyvsp[(6) - (7)].refval), (yyvsp[(7) - (7)].refval)); ;}
    break;

  case 37:
#line 211 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_0(cx, asestag_i_number, &(yyvsp[(1) - (1)].tokval));  ;}
    break;

  case 38:
#line 213 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_0(cx, asestag_i_string, &(yyvsp[(1) - (1)].tokval));  ;}
    break;

  case 39:
#line 215 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_0(cx, asestag_lref, &(yyvsp[(1) - (1)].tokval));  ;}
    break;

  case 40:
#line 217 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_2(cx, asestag_op, &(yyvsp[(2) - (3)].tokval), (yyvsp[(1) - (3)].refval), (yyvsp[(3) - (3)].refval)); ;}
    break;

  case 41:
#line 219 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_2(cx, asestag_op, &(yyvsp[(2) - (3)].tokval), (yyvsp[(1) - (3)].refval), (yyvsp[(3) - (3)].refval)); ;}
    break;

  case 42:
#line 221 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_2(cx, asestag_op, &(yyvsp[(2) - (3)].tokval), (yyvsp[(1) - (3)].refval), (yyvsp[(3) - (3)].refval)); ;}
    break;

  case 43:
#line 223 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_2(cx, asestag_op, &(yyvsp[(2) - (3)].tokval), (yyvsp[(1) - (3)].refval), (yyvsp[(3) - (3)].refval)); ;}
    break;

  case 44:
#line 225 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_2(cx, asestag_op, &(yyvsp[(2) - (3)].tokval), (yyvsp[(1) - (3)].refval), (yyvsp[(3) - (3)].refval)); ;}
    break;

  case 45:
#line 227 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_2(cx, asestag_op, &(yyvsp[(2) - (3)].tokval), (yyvsp[(1) - (3)].refval), (yyvsp[(3) - (3)].refval)); ;}
    break;

  case 46:
#line 229 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_2(cx, asestag_op, &(yyvsp[(2) - (3)].tokval), (yyvsp[(1) - (3)].refval), (yyvsp[(3) - (3)].refval)); ;}
    break;

  case 47:
#line 231 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_2(cx, asestag_op, &(yyvsp[(2) - (3)].tokval), (yyvsp[(1) - (3)].refval), (yyvsp[(3) - (3)].refval)); ;}
    break;

  case 48:
#line 233 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_2(cx, asestag_op, &(yyvsp[(2) - (3)].tokval), (yyvsp[(1) - (3)].refval), (yyvsp[(3) - (3)].refval)); ;}
    break;

  case 49:
#line 235 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_2(cx, asestag_op, &(yyvsp[(2) - (3)].tokval), (yyvsp[(1) - (3)].refval), (yyvsp[(3) - (3)].refval)); ;}
    break;

  case 50:
#line 237 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_2(cx, asestag_op, &(yyvsp[(2) - (3)].tokval), (yyvsp[(1) - (3)].refval), (yyvsp[(3) - (3)].refval)); ;}
    break;

  case 51:
#line 239 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_2(cx, asestag_op, &(yyvsp[(2) - (3)].tokval), (yyvsp[(1) - (3)].refval), (yyvsp[(3) - (3)].refval)); ;}
    break;

  case 52:
#line 241 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_2(cx, asestag_op, &(yyvsp[(2) - (3)].tokval), (yyvsp[(1) - (3)].refval), (yyvsp[(3) - (3)].refval)); ;}
    break;

  case 53:
#line 243 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_2(cx, asestag_op, &(yyvsp[(2) - (3)].tokval), (yyvsp[(1) - (3)].refval), (yyvsp[(3) - (3)].refval)); ;}
    break;

  case 54:
#line 245 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_2(cx, asestag_get, &(yyvsp[(2) - (3)].tokval), (yyvsp[(1) - (3)].refval), (yyvsp[(3) - (3)].refval)); ;}
    break;

  case 55:
#line 247 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_1(cx, asestag_op, &(yyvsp[(1) - (2)].tokval), (yyvsp[(2) - (2)].refval)); ;}
    break;

  case 56:
#line 249 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_1(cx, asestag_op, &(yyvsp[(1) - (2)].tokval), (yyvsp[(2) - (2)].refval)); ;}
    break;

  case 57:
#line 251 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_1(cx, asestag_op, &(yyvsp[(1) - (2)].tokval), (yyvsp[(2) - (2)].refval)); ;}
    break;

  case 58:
#line 253 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_1(cx, asestag_op, &(yyvsp[(1) - (2)].tokval), (yyvsp[(2) - (2)].refval)); ;}
    break;

  case 59:
#line 255 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_1(cx, asestag_op, &(yyvsp[(1) - (2)].tokval), (yyvsp[(2) - (2)].refval)); ;}
    break;

  case 60:
#line 257 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_1(cx, asestag_op, &(yyvsp[(1) - (2)].tokval), (yyvsp[(2) - (2)].refval)); ;}
    break;

  case 61:
#line 259 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_1(cx, asestag_op, NULL, (yyvsp[(2) - (3)].refval)); ;}
    break;

  case 62:
#line 261 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_1(cx, asestag_op, &(yyvsp[(2) - (3)].tokval), (yyvsp[(1) - (3)].refval)); ;}
    break;

  case 63:
#line 263 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_2(cx, asestag_op, &(yyvsp[(2) - (4)].tokval), (yyvsp[(1) - (4)].refval), (yyvsp[(3) - (4)].refval)); ;}
    break;

  case 64:
#line 265 "aseparser.y"
    { (yyval.refval) = aseparser_create_node_2(cx, asestag_get, &(yyvsp[(2) - (4)].tokval), (yyvsp[(1) - (4)].refval), (yyvsp[(3) - (4)].refval)); ;}
    break;


/* Line 1267 of yacc.c.  */
#line 1968 "aseparser.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (cx, YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (cx, yymsg);
	  }
	else
	  {
	    yyerror (cx, YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, cx);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, cx);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (cx, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, cx);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, cx);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 267 "aseparser.y"


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
  case asestag_get: return "get";
  case asestag_set: return "set";
  case asestag_minvoke: return "minvoke";
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


