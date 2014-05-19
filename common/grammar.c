/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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
#define YYBISON_VERSION "2.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse         argus_parse
#define yylex           argus_lex
#define yyerror         argus_error
#define yylval          argus_lval
#define yychar          argus_char
#define yydebug         argus_debug
#define yynerrs         argus_nerrs


/* Copy the first part of user declarations.  */

/* Line 268 of yacc.c  */
#line 1 "grammar.y"

/* Argus Software
 * Copyright (c) 2000-2012 QoSient, LLC
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * $Id: //depot/argus-3.0.6/clients/common/grammar.y#1 $
 * $DateTime: 2012/04/17 12:22:02 $
 * $Change: 2368 $
 */
/*
 * Copyright (c) 1988, 1989, 1990, 1991, 1992, 1993, 1994
 *	The Regents of the University of California.  Af rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that: (1) source code distributions
 * retain the above copyright notice and this paragraph in its entirety, (2)
 * distributions including binary code include the above copyright notice and
 * this paragraph in its entirety in the documentation or other materials
 * provided with the distribution, and (3) af advertising materials mentioning
 * features or use of this software display the fofowing acknowledgement:
 * ``This product includes software developed by the University of California,
 * Lawrence Berkeley Laboratory and its contributors.'' Neither the name of
 * the University nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior
 * written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#ifdef HAVE_CONFIG_H
#include "argus_config.h"
#endif

#include <argus_compat.h>
#include <sys/types.h>

#if defined(HAVE_SOLARIS) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#include <sys/socket.h>
#endif

#include <stdlib.h>
#include <sys/time.h>
#include <net/if.h>

#include <argus_def.h>
#include <argus_out.h>
#include <argus_util.h>

#include <argus_filter.h>

#include <argus_ethertype.h>
#include <argus_compat.h>

#include <syslog.h>

extern void ArgusLog (int, char *, ...);
extern int argus_error (char *);

#define YYDEBUG		2


#define QSET(q, p, d, a) (q).proto = (p),\
			 (q).dir = (d),\
			 (q).addr = (a)

int argus_n_errors = 0;

static struct qual qerr = { Q_UNDEF, Q_UNDEF, Q_UNDEF};



/* Line 268 of yacc.c  */
#line 168 "y.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     START = 258,
     STOP = 259,
     STATUS = 260,
     SHUTDOWN = 261,
     ERROR = 262,
     MAN = 263,
     FAR = 264,
     EVENT = 265,
     INDEX = 266,
     DST = 267,
     SRC = 268,
     HOST = 269,
     INODE = 270,
     GATEWAY = 271,
     IPID = 272,
     TTL = 273,
     TOS = 274,
     DSB = 275,
     SRCID = 276,
     TCPBASE = 277,
     NET = 278,
     AMASK = 279,
     PORT = 280,
     EQUAL = 281,
     LESS = 282,
     GREATER = 283,
     PROTO = 284,
     BYTE = 285,
     PKT = 286,
     APPBYTE = 287,
     TRANS = 288,
     ARP = 289,
     RARP = 290,
     IP = 291,
     IPV4 = 292,
     IPV6 = 293,
     TCP = 294,
     UDP = 295,
     ICMP = 296,
     IGMP = 297,
     ISIS = 298,
     HELLO = 299,
     LSP = 300,
     CSNP = 301,
     PSNP = 302,
     UDT = 303,
     SVC = 304,
     TCPRTT = 305,
     ETHER = 306,
     MPLS = 307,
     VLAN = 308,
     ANON = 309,
     VID = 310,
     VPRI = 311,
     MPLSID = 312,
     SPI = 313,
     ENCAPS = 314,
     RTP = 315,
     RTCP = 316,
     ESP = 317,
     DECNET = 318,
     LAT = 319,
     MOPRC = 320,
     MOPDL = 321,
     TK_BROADCAST = 322,
     TK_MULTICAST = 323,
     FRAG = 324,
     FRAG_ONLY = 325,
     RATE = 326,
     LOAD = 327,
     LOSS = 328,
     PLOSS = 329,
     INTER = 330,
     JITTER = 331,
     CO = 332,
     DUR = 333,
     AVGDUR = 334,
     DELTADUR = 335,
     DELTASTART = 336,
     DELTALAST = 337,
     DELTASPKTS = 338,
     DELTADPKTS = 339,
     SEQ = 340,
     NSTROKE = 341,
     NUM = 342,
     FLOAT = 343,
     INBOUND = 344,
     OUTBOUND = 345,
     LINK = 346,
     AUTH = 347,
     RECURS = 348,
     REQ = 349,
     RSP = 350,
     GEQ = 351,
     LEQ = 352,
     NEQ = 353,
     ID = 354,
     EID = 355,
     HIDV4 = 356,
     HIDV6 = 357,
     STRING = 358,
     LSH = 359,
     RSH = 360,
     LEN = 361,
     OUTOFORDER = 362,
     RETRANS = 363,
     NORMAL = 364,
     WAIT = 365,
     MULTIPATH = 366,
     RESET = 367,
     TIMEDOUT = 368,
     WINSHUT = 369,
     SYN = 370,
     SYNACK = 371,
     ACK = 372,
     PUSH = 373,
     URGENT = 374,
     DATA = 375,
     ECE = 376,
     CWR = 377,
     FIN = 378,
     FINACK = 379,
     ICMPECHO = 380,
     ICMPMAP = 381,
     UNREACH = 382,
     REDIRECT = 383,
     ECN = 384,
     TIMEXED = 385,
     ESTABLISHED = 386,
     CONNECTED = 387,
     CORRELATED = 388,
     RTR = 389,
     MBR = 390,
     LVG = 391,
     COCODE = 392,
     ASN = 393,
     AND = 394,
     OR = 395,
     UMINUS = 396
   };
#endif
/* Tokens.  */
#define START 258
#define STOP 259
#define STATUS 260
#define SHUTDOWN 261
#define ERROR 262
#define MAN 263
#define FAR 264
#define EVENT 265
#define INDEX 266
#define DST 267
#define SRC 268
#define HOST 269
#define INODE 270
#define GATEWAY 271
#define IPID 272
#define TTL 273
#define TOS 274
#define DSB 275
#define SRCID 276
#define TCPBASE 277
#define NET 278
#define AMASK 279
#define PORT 280
#define EQUAL 281
#define LESS 282
#define GREATER 283
#define PROTO 284
#define BYTE 285
#define PKT 286
#define APPBYTE 287
#define TRANS 288
#define ARP 289
#define RARP 290
#define IP 291
#define IPV4 292
#define IPV6 293
#define TCP 294
#define UDP 295
#define ICMP 296
#define IGMP 297
#define ISIS 298
#define HELLO 299
#define LSP 300
#define CSNP 301
#define PSNP 302
#define UDT 303
#define SVC 304
#define TCPRTT 305
#define ETHER 306
#define MPLS 307
#define VLAN 308
#define ANON 309
#define VID 310
#define VPRI 311
#define MPLSID 312
#define SPI 313
#define ENCAPS 314
#define RTP 315
#define RTCP 316
#define ESP 317
#define DECNET 318
#define LAT 319
#define MOPRC 320
#define MOPDL 321
#define TK_BROADCAST 322
#define TK_MULTICAST 323
#define FRAG 324
#define FRAG_ONLY 325
#define RATE 326
#define LOAD 327
#define LOSS 328
#define PLOSS 329
#define INTER 330
#define JITTER 331
#define CO 332
#define DUR 333
#define AVGDUR 334
#define DELTADUR 335
#define DELTASTART 336
#define DELTALAST 337
#define DELTASPKTS 338
#define DELTADPKTS 339
#define SEQ 340
#define NSTROKE 341
#define NUM 342
#define FLOAT 343
#define INBOUND 344
#define OUTBOUND 345
#define LINK 346
#define AUTH 347
#define RECURS 348
#define REQ 349
#define RSP 350
#define GEQ 351
#define LEQ 352
#define NEQ 353
#define ID 354
#define EID 355
#define HIDV4 356
#define HIDV6 357
#define STRING 358
#define LSH 359
#define RSH 360
#define LEN 361
#define OUTOFORDER 362
#define RETRANS 363
#define NORMAL 364
#define WAIT 365
#define MULTIPATH 366
#define RESET 367
#define TIMEDOUT 368
#define WINSHUT 369
#define SYN 370
#define SYNACK 371
#define ACK 372
#define PUSH 373
#define URGENT 374
#define DATA 375
#define ECE 376
#define CWR 377
#define FIN 378
#define FINACK 379
#define ICMPECHO 380
#define ICMPMAP 381
#define UNREACH 382
#define REDIRECT 383
#define ECN 384
#define TIMEXED 385
#define ESTABLISHED 386
#define CONNECTED 387
#define CORRELATED 388
#define RTR 389
#define MBR 390
#define LVG 391
#define COCODE 392
#define ASN 393
#define AND 394
#define OR 395
#define UMINUS 396




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 293 of yacc.c  */
#line 89 "grammar.y"

	int i;
	float f;
	u_long h;
	u_char *e;
	char *s;
	struct stmt *stmt;
	struct arth *a;
	struct {
		struct qual q;
		struct ablock *b;
	} blk;
	struct ablock *rblk;



/* Line 293 of yacc.c  */
#line 503 "y.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 343 of yacc.c  */
#line 515 "y.tab.c"

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
# if defined YYENABLE_NLS && YYENABLE_NLS
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
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
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
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
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
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1240

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  157
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  33
/* YYNRULES -- Number of rules.  */
#define YYNRULES  206
/* YYNRULES -- Number of states.  */
#define YYNSTATES  258

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   396

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   141,     2,     2,     2,     2,   143,     2,
     150,   149,   146,   144,     2,   145,     2,   147,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   156,     2,
     153,   152,   151,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   154,     2,   155,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   142,     2,     2,     2,     2,     2,
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
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   148
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     6,     8,     9,    11,    15,    19,    23,
      27,    29,    31,    33,    35,    38,    41,    44,    47,    50,
      52,    55,    58,    61,    64,    67,    71,    73,    75,    77,
      81,    85,    87,    91,    93,    95,    98,   100,   102,   104,
     108,   112,   114,   116,   118,   121,   125,   128,   131,   134,
     137,   141,   145,   148,   151,   154,   157,   160,   164,   166,
     168,   170,   174,   178,   180,   182,   183,   185,   187,   191,
     195,   199,   203,   205,   207,   209,   211,   213,   215,   217,
     219,   221,   223,   225,   227,   229,   231,   233,   235,   237,
     239,   241,   243,   245,   247,   249,   251,   253,   255,   257,
     259,   261,   263,   265,   267,   269,   271,   273,   275,   277,
     279,   281,   283,   285,   287,   289,   291,   293,   295,   297,
     299,   301,   303,   305,   307,   309,   311,   313,   315,   317,
     319,   321,   323,   325,   327,   329,   331,   333,   335,   337,
     339,   341,   343,   345,   347,   349,   351,   353,   355,   357,
     359,   361,   363,   365,   367,   369,   371,   373,   375,   377,
     379,   381,   383,   385,   387,   389,   391,   393,   395,   397,
     399,   401,   403,   405,   407,   409,   411,   413,   416,   419,
     421,   423,   425,   427,   429,   431,   433,   435,   437,   439,
     441,   446,   453,   457,   461,   465,   469,   473,   477,   481,
     485,   488,   492,   494,   496,   500,   502
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     158,     0,    -1,   159,   160,    -1,   159,    -1,    -1,   170,
      -1,   160,   161,   170,    -1,   160,   161,   163,    -1,   160,
     162,   170,    -1,   160,   162,   163,    -1,   139,    -1,   140,
      -1,   165,    -1,   188,    -1,    27,   188,    -1,    28,   188,
      -1,    26,   188,    -1,    96,   188,    -1,    97,   188,    -1,
     189,    -1,    27,   189,    -1,    28,   189,    -1,    26,   189,
      -1,    96,   189,    -1,    97,   189,    -1,   167,   168,   149,
      -1,   182,    -1,    99,    -1,   103,    -1,   101,   147,    87,
      -1,   101,    24,   101,    -1,   101,    -1,   102,   147,    87,
      -1,   102,    -1,   100,    -1,   166,   163,    -1,   141,    -1,
     150,    -1,   165,    -1,   169,   161,   163,    -1,   169,   162,
     163,    -1,   188,    -1,   168,    -1,   173,    -1,   166,   170,
      -1,   174,   175,   176,    -1,   174,   175,    -1,   174,   176,
      -1,   174,   177,    -1,   174,   178,    -1,   174,   175,   177,
      -1,   174,   175,   178,    -1,   174,    29,    -1,   174,   179,
      -1,   174,   175,    -1,   171,   163,    -1,   172,   164,    -1,
     167,   160,   149,    -1,   181,    -1,   180,    -1,   164,    -1,
     186,   184,   186,    -1,   186,   185,   186,    -1,   183,    -1,
     181,    -1,    -1,    13,    -1,    12,    -1,    13,   140,    12,
      -1,    12,   140,    13,    -1,    13,   139,    12,    -1,    12,
     139,    13,    -1,    14,    -1,    21,    -1,    15,    -1,    23,
      -1,    25,    -1,    17,    -1,    18,    -1,    19,    -1,    20,
      -1,    77,    -1,   138,    -1,    55,    -1,    56,    -1,    57,
      -1,    30,    -1,    32,    -1,    31,    -1,    33,    -1,    50,
      -1,    22,    -1,    73,    -1,    58,    -1,    59,    -1,    80,
      -1,    81,    -1,    82,    -1,    86,    -1,    85,    -1,    78,
      -1,    79,    -1,    75,    -1,    76,    -1,    71,    -1,    72,
      -1,    74,    -1,    16,    -1,     3,    -1,     4,    -1,     5,
      -1,     6,    -1,     7,    -1,     8,    -1,     9,    -1,    10,
      -1,    11,    -1,    49,    -1,   109,    -1,   110,    -1,   116,
      -1,   124,    -1,   131,    -1,   132,    -1,   133,    -1,   113,
      -1,   126,    -1,   125,    -1,   127,    -1,   128,    -1,   130,
      -1,   134,    -1,   135,    -1,   136,    -1,   137,    -1,    91,
      -1,    51,    -1,    36,    -1,    37,    -1,    38,    -1,    34,
      -1,    35,    -1,    62,    -1,    60,    -1,    61,    -1,    39,
      -1,    40,    -1,    41,    -1,    42,    -1,    52,    -1,    43,
      -1,    53,    -1,    48,    -1,    54,    -1,    63,    -1,    64,
      -1,    66,    -1,    65,    -1,   107,    -1,   108,    -1,   114,
      -1,   115,    -1,   123,    -1,   112,    -1,   117,    -1,   118,
      -1,   119,    -1,   122,    -1,   121,    -1,    69,    -1,    70,
      -1,   129,    -1,   111,    -1,    44,    -1,    45,    -1,    46,
      -1,    47,    -1,   174,    67,    -1,   174,    68,    -1,    89,
      -1,    90,    -1,   151,    -1,    96,    -1,   152,    -1,    26,
      -1,    97,    -1,   153,    -1,    98,    -1,   188,    -1,   187,
      -1,   181,   154,   186,   155,    -1,   181,   154,   186,   156,
      87,   155,    -1,   186,   144,   186,    -1,   186,   145,   186,
      -1,   186,   146,   186,    -1,   186,   147,   186,    -1,   186,
     143,   186,    -1,   186,   142,   186,    -1,   186,   104,   186,
      -1,   186,   105,   186,    -1,   145,   186,    -1,   167,   187,
     149,    -1,   106,    -1,    87,    -1,   167,   188,   149,    -1,
      88,    -1,   167,   189,   149,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   157,   157,   161,   163,   165,   166,   167,   168,   169,
     171,   173,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   190,   192,   193,   195,
     197,   199,   201,   203,   205,   207,   209,   211,   213,   214,
     215,   217,   218,   220,   221,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   234,   237,   238,   239,   240,   241,
     242,   243,   244,   245,   249,   250,   253,   254,   255,   256,
     257,   258,   261,   262,   263,   264,   267,   268,   269,   270,
     271,   272,   273,   274,   275,   276,   277,   278,   279,   280,
     281,   282,   283,   284,   285,   286,   287,   288,   289,   290,
     293,   294,   295,   296,   297,   298,   299,   302,   305,   306,
     307,   308,   309,   310,   311,   312,   313,   314,   315,   316,
     317,   318,   319,   320,   321,   322,   323,   324,   325,   326,
     327,   328,   329,   330,   331,   334,   335,   336,   337,   338,
     339,   340,   341,   342,   343,   344,   345,   346,   347,   348,
     349,   350,   351,   352,   353,   354,   355,   356,   359,   360,
     361,   362,   363,   364,   365,   366,   367,   368,   369,   370,
     371,   372,   373,   374,   375,   376,   377,   379,   380,   381,
     382,   384,   385,   386,   387,   389,   390,   391,   393,   394,
     396,   397,   398,   399,   400,   401,   402,   403,   404,   405,
     406,   407,   408,   410,   411,   413,   414
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "START", "STOP", "STATUS", "SHUTDOWN",
  "ERROR", "MAN", "FAR", "EVENT", "INDEX", "DST", "SRC", "HOST", "INODE",
  "GATEWAY", "IPID", "TTL", "TOS", "DSB", "SRCID", "TCPBASE", "NET",
  "AMASK", "PORT", "EQUAL", "LESS", "GREATER", "PROTO", "BYTE", "PKT",
  "APPBYTE", "TRANS", "ARP", "RARP", "IP", "IPV4", "IPV6", "TCP", "UDP",
  "ICMP", "IGMP", "ISIS", "HELLO", "LSP", "CSNP", "PSNP", "UDT", "SVC",
  "TCPRTT", "ETHER", "MPLS", "VLAN", "ANON", "VID", "VPRI", "MPLSID",
  "SPI", "ENCAPS", "RTP", "RTCP", "ESP", "DECNET", "LAT", "MOPRC", "MOPDL",
  "TK_BROADCAST", "TK_MULTICAST", "FRAG", "FRAG_ONLY", "RATE", "LOAD",
  "LOSS", "PLOSS", "INTER", "JITTER", "CO", "DUR", "AVGDUR", "DELTADUR",
  "DELTASTART", "DELTALAST", "DELTASPKTS", "DELTADPKTS", "SEQ", "NSTROKE",
  "NUM", "FLOAT", "INBOUND", "OUTBOUND", "LINK", "AUTH", "RECURS", "REQ",
  "RSP", "GEQ", "LEQ", "NEQ", "ID", "EID", "HIDV4", "HIDV6", "STRING",
  "LSH", "RSH", "LEN", "OUTOFORDER", "RETRANS", "NORMAL", "WAIT",
  "MULTIPATH", "RESET", "TIMEDOUT", "WINSHUT", "SYN", "SYNACK", "ACK",
  "PUSH", "URGENT", "DATA", "ECE", "CWR", "FIN", "FINACK", "ICMPECHO",
  "ICMPMAP", "UNREACH", "REDIRECT", "ECN", "TIMEXED", "ESTABLISHED",
  "CONNECTED", "CORRELATED", "RTR", "MBR", "LVG", "COCODE", "ASN", "AND",
  "OR", "'!'", "'|'", "'&'", "'+'", "'-'", "'*'", "'/'", "UMINUS", "')'",
  "'('", "'>'", "'='", "'<'", "'['", "']'", "':'", "$accept", "prog",
  "nuf", "expr", "and", "or", "id", "tid", "nid", "not", "paren", "pid",
  "qid", "term", "head", "thead", "rterm", "pqual", "dqual", "aqual",
  "iqual", "fqual", "ndaqual", "sname", "pname", "tname", "other", "relop",
  "irelop", "arth", "narth", "pnum", "fnum", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,    33,   124,    38,    43,    45,    42,    47,   396,    41,
      40,    62,    61,    60,    91,    93,    58
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   157,   158,   158,   159,   160,   160,   160,   160,   160,
     161,   162,   163,   163,   163,   163,   163,   163,   163,   163,
     163,   163,   163,   163,   163,   163,   164,   165,   165,   165,
     165,   165,   165,   165,   165,   165,   166,   167,   168,   168,
     168,   169,   169,   170,   170,   171,   171,   171,   171,   171,
     171,   171,   171,   171,   172,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   174,   174,   175,   175,   175,   175,
     175,   175,   176,   176,   176,   176,   177,   177,   177,   177,
     177,   177,   177,   177,   177,   177,   177,   177,   177,   177,
     177,   177,   177,   177,   177,   177,   177,   177,   177,   177,
     178,   178,   178,   178,   178,   178,   178,   179,   180,   180,
     180,   180,   180,   180,   180,   180,   180,   180,   180,   180,
     180,   180,   180,   180,   180,   180,   180,   180,   180,   180,
     180,   180,   180,   180,   180,   181,   181,   181,   181,   181,
     181,   181,   181,   181,   181,   181,   181,   181,   181,   181,
     181,   181,   181,   181,   181,   181,   181,   181,   182,   182,
     182,   182,   182,   182,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   182,   182,   182,   182,   183,   183,   183,
     183,   184,   184,   184,   184,   185,   185,   185,   186,   186,
     187,   187,   187,   187,   187,   187,   187,   187,   187,   187,
     187,   187,   187,   188,   188,   189,   189
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     1,     0,     1,     3,     3,     3,     3,
       1,     1,     1,     1,     2,     2,     2,     2,     2,     1,
       2,     2,     2,     2,     2,     3,     1,     1,     1,     3,
       3,     1,     3,     1,     1,     2,     1,     1,     1,     3,
       3,     1,     1,     1,     2,     3,     2,     2,     2,     2,
       3,     3,     2,     2,     2,     2,     2,     3,     1,     1,
       1,     3,     3,     1,     1,     0,     1,     1,     3,     3,
       3,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       4,     6,     3,     3,     3,     3,     3,     3,     3,     3,
       2,     3,     1,     1,     3,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,     0,    65,     1,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   140,   141,   137,   138,   139,   145,   146,
     147,   148,   150,   173,   174,   175,   176,   152,   117,   136,
     149,   151,   153,   143,   144,   142,   154,   155,   157,   156,
     169,   170,   203,   179,   180,   135,   202,   158,   159,   118,
     119,   172,   163,   125,   160,   161,   120,   164,   165,   166,
     168,   167,   162,   121,   127,   126,   128,   129,   171,   130,
     122,   123,   124,   131,   132,   133,   134,    36,     0,    37,
       2,    60,    65,    65,     5,     0,     0,    43,     0,    59,
      64,    26,    63,     0,   189,   188,     0,     0,   200,    10,
      11,    65,    65,    44,     0,   189,   188,     0,     0,     0,
     205,     0,     0,    27,    34,    31,    33,    28,    55,    12,
       0,     0,    13,    19,    56,    67,    66,    72,    74,   107,
      77,    78,    79,    80,    73,    91,    75,    76,    52,    86,
      88,    87,    89,    90,    83,    84,    85,    93,    94,   177,
     178,   104,   105,    92,   106,   102,   103,    81,   100,   101,
      95,    96,    97,    99,    98,    82,    54,    47,    48,    49,
      53,     0,   184,   182,   185,   187,     0,     0,     0,     0,
       0,     0,     0,     0,   181,   183,   186,     0,     0,     0,
       7,    65,    65,     6,   188,     9,     8,    57,   201,   204,
       0,    16,    22,    14,    20,    15,    21,    17,    23,    18,
      24,     0,     0,     0,    35,    38,    42,     0,    41,     0,
       0,     0,     0,     0,    45,    50,    51,     0,   198,   199,
     197,   196,   192,   193,   194,   195,    61,    62,    65,   188,
       0,    30,    29,    32,    25,     0,     0,   206,    71,    69,
      70,    68,   190,     0,    39,    40,     0,   191
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,   104,   101,   102,   214,    81,   119,   120,
      96,   216,   217,    84,    85,    86,    87,    88,   166,   167,
     168,   169,   170,    89,    97,    91,    92,   187,   188,    93,
      94,    95,   123
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -131
static const yytype_int16 yypact[] =
{
    -131,    13,   215,  -131,  -131,  -131,  -131,  -131,  -131,  -131,
    -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,
    -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,
    -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,
    -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,
    -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,
    -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,
    -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  1090,  -131,
    -130,  -131,   807,   807,  -131,   656,  1101,  -131,  1037,  -131,
       1,  -131,  -131,    -9,  -131,  -131,  1090,  -126,  -131,  -131,
    -131,   363,   363,  -131,   -93,  -117,  -111,   -29,   -29,   -29,
    -131,   -29,   -29,  -131,  -131,   -22,  -106,  -131,  -131,  -131,
     656,   -33,  -131,  -131,  -131,  -124,   -95,  -131,  -131,  -131,
    -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,
    -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,
    -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,
    -131,  -131,  -131,  -131,  -131,  -131,   941,  -131,  -131,  -131,
    -131,  1090,  -131,  -131,  -131,  -131,  1090,  1090,  1090,  1090,
    1090,  1090,  1090,  1090,  -131,  -131,  -131,  1090,  1090,    87,
    -131,   363,   511,  -131,     8,  -131,  -131,  -131,  -131,  -131,
     -29,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,  -131,
    -131,   -53,   -30,   -16,  -131,  -131,   -84,  -130,  -111,   -74,
      65,    66,    79,    82,  -131,  -131,  -131,    41,   -94,   -94,
     -83,   142,   -73,   -73,  -131,  -131,    87,    87,   659,   -63,
    -111,  -131,  -131,  -131,  -131,   656,   656,  -131,  -131,  -131,
    -131,  -131,  -131,    11,  -131,  -131,   -58,  -131
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -131,  -131,  -131,    99,  -115,  -101,   -82,    38,  -110,     2,
      -2,  -131,  -131,   -76,  -131,  -131,  -131,  -131,  -131,   -40,
     -39,   -37,  -131,  -131,    10,  -131,  -131,  -131,  -131,    -6,
     -69,   -78,   -72
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -59
static const yytype_int16 yytable[] =
{
      83,   -58,   211,   118,    82,   106,   103,   122,   -13,    99,
     100,   215,    90,     3,   105,   220,   221,   172,   106,   190,
     195,   176,   177,   194,   194,   193,   196,   105,   171,   201,
     203,   205,   198,   207,   209,   202,   204,   206,   199,   208,
     210,   213,   122,   218,   222,   223,    99,   100,   241,   219,
     180,   181,   182,   183,    42,   110,   197,   242,    42,   110,
     179,   180,   181,   182,   183,   244,   113,   114,   115,   116,
     117,   243,    98,   182,   183,   247,   -41,   -41,   248,   249,
      83,    83,   215,   121,    82,    82,   199,   173,   174,   175,
     189,   250,    90,    90,   251,   176,   177,   257,   256,   192,
     192,    80,   245,   191,   191,   200,   200,   200,    77,   200,
     200,    90,    90,   194,   239,   103,   246,    79,   121,   200,
     219,    79,   240,   105,   124,   212,   224,   225,   219,   226,
       0,     0,     0,   178,   179,   180,   181,   182,   183,     0,
     -58,   -58,   184,   185,   186,   176,   177,   -13,   -13,     0,
     -58,     0,     0,     0,     0,   171,     0,   -13,     0,     0,
     106,     0,     0,   254,   255,   227,   219,   122,   122,   105,
     228,   229,   230,   231,   232,   233,   234,   235,     0,     0,
       0,   236,   237,   178,   179,   180,   181,   182,   183,   192,
     238,   176,   177,   191,   191,     0,   252,   253,   200,     0,
       0,    90,    90,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    -3,     0,     0,     4,     5,
       6,     7,     8,     9,    10,    11,    12,     0,     0,   178,
     179,   180,   181,   182,   183,     0,   238,     0,     0,     0,
      82,     0,     0,   121,   121,     0,   176,   177,    90,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,     0,    29,    30,    31,    32,
       0,     0,     0,     0,     0,    33,    34,    35,    36,    37,
      38,    39,     0,     0,    40,    41,   180,   181,   182,   183,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    42,     0,    43,    44,    45,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,     0,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,     0,     0,     0,    77,     0,     0,     0,
      78,     0,     0,     0,     0,    79,     4,     5,     6,     7,
       8,     9,    10,    11,    12,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   107,
     108,   109,     0,     0,     0,     0,     0,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,     0,    29,    30,    31,    32,     0,     0,
       0,     0,     0,    33,    34,    35,    36,    37,    38,    39,
       0,     0,    40,    41,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      42,   110,    43,    44,    45,     0,     0,     0,     0,   111,
     112,     0,   113,   114,   115,   116,   117,     0,     0,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,     0,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,     0,     0,     0,    77,     0,     0,     0,    78,     0,
       0,     0,     0,    79,     4,     5,     6,     7,     8,     9,
      10,    11,    12,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,     0,    29,    30,    31,    32,     0,     0,     0,     0,
       0,    33,    34,    35,    36,    37,    38,    39,     0,     0,
      40,    41,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    42,   110,
      43,    44,    45,     0,     0,     0,     0,     0,     0,     0,
     113,   114,   115,   116,   117,     0,     0,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,     0,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,     0,
       0,     0,    77,     0,     0,     0,    78,     0,     0,     0,
       0,    79,     4,     5,     6,     7,     8,     9,    10,    11,
      12,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   107,   108,   109,     0,     0,     0,     0,     0,
       0,     0,     0,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,     0,
      29,    30,    31,    32,     0,     0,     0,     0,     0,    33,
      34,    35,    36,    37,    38,    39,     0,     0,    40,    41,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    42,   110,     0,    42,   110,    43,    44,
      45,     0,   111,   112,     0,   113,   114,   115,   116,   117,
       0,     0,     0,     0,     0,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,     0,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,     0,     0,
      77,     0,     0,     0,    78,     0,    79,     0,     0,    79,
       4,     5,     6,     7,     8,     9,    10,    11,    12,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,     0,    29,    30,
      31,    32,     0,     0,     0,     0,     0,    33,    34,    35,
      36,    37,    38,    39,     0,     0,    40,    41,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    42,     0,    43,    44,    45,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,     0,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,     0,     0,     0,    77,     0,
       0,     0,    78,     0,     0,   127,   128,    79,   130,   131,
     132,   133,   134,   135,   136,     0,   137,   -46,   -46,   -46,
       0,   139,   140,   141,   142,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   143,     0,     0,     0,     0,   144,   145,   146,   147,
     148,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   151,   152,   153,   154,   155,   156,   157,   158,
     159,   160,   161,   162,     0,     0,   163,   164,   -46,   -46,
       0,     0,     0,     0,     0,     0,     0,   -46,   -46,     0,
     -46,   -46,   -46,   -46,   -46,     0,     0,     0,     0,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,     0,   137,     0,     0,     0,   138,   139,   140,   141,
     142,     0,     0,     0,     0,     0,     0,     0,     0,   165,
       0,     0,   -46,     0,     0,     0,     0,   143,     0,     0,
       0,   -46,   144,   145,   146,   147,   148,     0,     0,     0,
       0,     0,     0,     0,   149,   150,     0,     0,   151,   152,
     153,   154,   155,   156,   157,   158,   159,   160,   161,   162,
       0,     0,   163,   164,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,     0,     0,     0,     0,    27,     0,
       0,    29,    30,    31,    32,    23,    24,    25,    26,     0,
      33,    34,    35,    36,    37,    38,    39,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      40,    41,     0,     0,     0,   165,     0,    42,     0,     0,
       0,    45,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    46,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    47,    48,
       0,     0,    51,    52,     0,    54,    55,     0,    57,    58,
      59,     0,    60,    61,    62,     0,     0,     0,     0,     0,
      68,     0,     0,     0,     0,    78,     0,     0,     0,     0,
      79
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-131))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
       2,     0,    24,    85,     2,    83,    82,    85,     0,   139,
     140,   121,     2,     0,    83,   139,   140,    26,    96,   101,
     102,   104,   105,   101,   102,   101,   102,    96,   154,   107,
     108,   109,   149,   111,   112,   107,   108,   109,   149,   111,
     112,   147,   120,   121,   139,   140,   139,   140,   101,   121,
     144,   145,   146,   147,    87,    88,   149,    87,    87,    88,
     143,   144,   145,   146,   147,   149,    99,   100,   101,   102,
     103,    87,    78,   146,   147,   149,   139,   140,    13,    13,
      82,    83,   192,    85,    82,    83,   149,    96,    97,    98,
      96,    12,    82,    83,    12,   104,   105,   155,    87,   101,
     102,     2,   217,   101,   102,   107,   108,   109,   141,   111,
     112,   101,   102,   191,   192,   191,   217,   150,   120,   121,
     192,   150,   200,   192,    86,   147,   166,   166,   200,   166,
      -1,    -1,    -1,   142,   143,   144,   145,   146,   147,    -1,
     139,   140,   151,   152,   153,   104,   105,   139,   140,    -1,
     149,    -1,    -1,    -1,    -1,   154,    -1,   149,    -1,    -1,
     238,    -1,    -1,   245,   246,   171,   238,   245,   246,   238,
     176,   177,   178,   179,   180,   181,   182,   183,    -1,    -1,
      -1,   187,   188,   142,   143,   144,   145,   146,   147,   191,
     192,   104,   105,   191,   192,    -1,   155,   156,   200,    -1,
      -1,   191,   192,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     0,    -1,    -1,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    -1,    -1,   142,
     143,   144,   145,   146,   147,    -1,   238,    -1,    -1,    -1,
     238,    -1,    -1,   245,   246,    -1,   104,   105,   238,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    -1,    51,    52,    53,    54,
      -1,    -1,    -1,    -1,    -1,    60,    61,    62,    63,    64,
      65,    66,    -1,    -1,    69,    70,   144,   145,   146,   147,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    87,    -1,    89,    90,    91,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,    -1,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,    -1,    -1,    -1,   141,    -1,    -1,    -1,
     145,    -1,    -1,    -1,    -1,   150,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    26,
      27,    28,    -1,    -1,    -1,    -1,    -1,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    -1,    51,    52,    53,    54,    -1,    -1,
      -1,    -1,    -1,    60,    61,    62,    63,    64,    65,    66,
      -1,    -1,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      87,    88,    89,    90,    91,    -1,    -1,    -1,    -1,    96,
      97,    -1,    99,   100,   101,   102,   103,    -1,    -1,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,    -1,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,    -1,    -1,    -1,   141,    -1,    -1,    -1,   145,    -1,
      -1,    -1,    -1,   150,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    -1,    51,    52,    53,    54,    -1,    -1,    -1,    -1,
      -1,    60,    61,    62,    63,    64,    65,    66,    -1,    -1,
      69,    70,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    87,    88,
      89,    90,    91,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      99,   100,   101,   102,   103,    -1,    -1,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,    -1,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,    -1,
      -1,    -1,   141,    -1,    -1,    -1,   145,    -1,    -1,    -1,
      -1,   150,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    26,    27,    28,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    -1,
      51,    52,    53,    54,    -1,    -1,    -1,    -1,    -1,    60,
      61,    62,    63,    64,    65,    66,    -1,    -1,    69,    70,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    87,    88,    -1,    87,    88,    89,    90,
      91,    -1,    96,    97,    -1,    99,   100,   101,   102,   103,
      -1,    -1,    -1,    -1,    -1,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,    -1,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   141,    -1,    -1,
     141,    -1,    -1,    -1,   145,    -1,   150,    -1,    -1,   150,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    -1,    51,    52,
      53,    54,    -1,    -1,    -1,    -1,    -1,    60,    61,    62,
      63,    64,    65,    66,    -1,    -1,    69,    70,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    87,    -1,    89,    90,    91,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,    -1,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,    -1,    -1,    -1,   141,    -1,
      -1,    -1,   145,    -1,    -1,    14,    15,   150,    17,    18,
      19,    20,    21,    22,    23,    -1,    25,    26,    27,    28,
      -1,    30,    31,    32,    33,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    50,    -1,    -1,    -1,    -1,    55,    56,    57,    58,
      59,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    -1,    -1,    85,    86,    87,    88,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    96,    97,    -1,
      99,   100,   101,   102,   103,    -1,    -1,    -1,    -1,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    -1,    25,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   138,
      -1,    -1,   141,    -1,    -1,    -1,    -1,    50,    -1,    -1,
      -1,   150,    55,    56,    57,    58,    59,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    67,    68,    -1,    -1,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      -1,    -1,    85,    86,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    -1,    -1,    -1,    -1,    48,    -1,
      -1,    51,    52,    53,    54,    44,    45,    46,    47,    -1,
      60,    61,    62,    63,    64,    65,    66,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      69,    70,    -1,    -1,    -1,   138,    -1,    87,    -1,    -1,
      -1,    91,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   106,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,   108,
      -1,    -1,   111,   112,    -1,   114,   115,    -1,   117,   118,
     119,    -1,   121,   122,   123,    -1,    -1,    -1,    -1,    -1,
     129,    -1,    -1,    -1,    -1,   145,    -1,    -1,    -1,    -1,
     150
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,   158,   159,     0,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    51,
      52,    53,    54,    60,    61,    62,    63,    64,    65,    66,
      69,    70,    87,    89,    90,    91,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   141,   145,   150,
     160,   164,   166,   167,   170,   171,   172,   173,   174,   180,
     181,   182,   183,   186,   187,   188,   167,   181,   186,   139,
     140,   161,   162,   170,   160,   187,   188,    26,    27,    28,
      88,    96,    97,    99,   100,   101,   102,   103,   163,   165,
     166,   167,   188,   189,   164,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    25,    29,    30,
      31,    32,    33,    50,    55,    56,    57,    58,    59,    67,
      68,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    85,    86,   138,   175,   176,   177,   178,
     179,   154,    26,    96,    97,    98,   104,   105,   142,   143,
     144,   145,   146,   147,   151,   152,   153,   184,   185,   186,
     163,   166,   167,   170,   188,   163,   170,   149,   149,   149,
     167,   188,   189,   188,   189,   188,   189,   188,   189,   188,
     189,    24,   147,   147,   163,   165,   168,   169,   188,   189,
     139,   140,   139,   140,   176,   177,   178,   186,   186,   186,
     186,   186,   186,   186,   186,   186,   186,   186,   167,   188,
     188,   101,    87,    87,   149,   161,   162,   149,    13,    13,
      12,    12,   155,   156,   163,   163,    87,   155
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
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
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


/* This macro is provided for backward compatibility. */

#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
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

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
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
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
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
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
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

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  YYSIZE_T yysize1;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = 0;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                yysize1 = yysize + yytnamerr (0, yytname[yyx]);
                if (! (yysize <= yysize1
                       && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                  return 2;
                yysize = yysize1;
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  yysize1 = yysize + yystrlen (yyformat);
  if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
    return 2;
  yysize = yysize1;

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

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
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;


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
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

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
      /* Get the current used size of the three stacks, in elements.  */
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
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
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

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
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
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
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

/* Line 1806 of yacc.c  */
#line 158 "grammar.y"
    {
	Argusfinish_parse((yyvsp[(2) - (2)].blk).b);
}
    break;

  case 4:

/* Line 1806 of yacc.c  */
#line 163 "grammar.y"
    { (yyval.blk).q = qerr; }
    break;

  case 6:

/* Line 1806 of yacc.c  */
#line 166 "grammar.y"
    { Argusgen_and((yyvsp[(1) - (3)].blk).b, (yyvsp[(3) - (3)].blk).b); (yyval.blk) = (yyvsp[(3) - (3)].blk); }
    break;

  case 7:

/* Line 1806 of yacc.c  */
#line 167 "grammar.y"
    { Argusgen_and((yyvsp[(1) - (3)].blk).b, (yyvsp[(3) - (3)].blk).b); (yyval.blk) = (yyvsp[(3) - (3)].blk); }
    break;

  case 8:

/* Line 1806 of yacc.c  */
#line 168 "grammar.y"
    { Argusgen_or((yyvsp[(1) - (3)].blk).b, (yyvsp[(3) - (3)].blk).b); (yyval.blk) = (yyvsp[(3) - (3)].blk); }
    break;

  case 9:

/* Line 1806 of yacc.c  */
#line 169 "grammar.y"
    { Argusgen_or((yyvsp[(1) - (3)].blk).b, (yyvsp[(3) - (3)].blk).b); (yyval.blk) = (yyvsp[(3) - (3)].blk); }
    break;

  case 10:

/* Line 1806 of yacc.c  */
#line 171 "grammar.y"
    { (yyval.blk) = (yyvsp[(0) - (1)].blk); }
    break;

  case 11:

/* Line 1806 of yacc.c  */
#line 173 "grammar.y"
    { (yyval.blk) = (yyvsp[(0) - (1)].blk); }
    break;

  case 13:

/* Line 1806 of yacc.c  */
#line 176 "grammar.y"
    { (yyval.blk).b = Argusgen_ncode(NULL, (arg_uint32)(yyvsp[(1) - (1)].i), (yyval.blk).q = (yyvsp[(0) - (1)].blk).q, Q_EQUAL); }
    break;

  case 14:

/* Line 1806 of yacc.c  */
#line 177 "grammar.y"
    { (yyval.blk).b = Argusgen_ncode(NULL, (arg_uint32)(yyvsp[(2) - (2)].i), (yyval.blk).q = (yyvsp[(0) - (2)].blk).q, Q_LESS); }
    break;

  case 15:

/* Line 1806 of yacc.c  */
#line 178 "grammar.y"
    { (yyval.blk).b = Argusgen_ncode(NULL, (arg_uint32)(yyvsp[(2) - (2)].i), (yyval.blk).q = (yyvsp[(0) - (2)].blk).q, Q_GREATER); }
    break;

  case 16:

/* Line 1806 of yacc.c  */
#line 179 "grammar.y"
    { (yyval.blk).b = Argusgen_ncode(NULL, (arg_uint32)(yyvsp[(2) - (2)].i), (yyval.blk).q = (yyvsp[(0) - (2)].blk).q, Q_EQUAL); }
    break;

  case 17:

/* Line 1806 of yacc.c  */
#line 180 "grammar.y"
    { (yyval.blk).b = Argusgen_ncode(NULL, (arg_uint32)(yyvsp[(2) - (2)].i), (yyval.blk).q = (yyvsp[(0) - (2)].blk).q, Q_GEQ); }
    break;

  case 18:

/* Line 1806 of yacc.c  */
#line 181 "grammar.y"
    { (yyval.blk).b = Argusgen_ncode(NULL, (arg_uint32)(yyvsp[(2) - (2)].i), (yyval.blk).q = (yyvsp[(0) - (2)].blk).q, Q_LEQ); }
    break;

  case 19:

/* Line 1806 of yacc.c  */
#line 182 "grammar.y"
    { (yyval.blk).b = Argusgen_fcode(NULL, (float)(yyvsp[(1) - (1)].f), (yyval.blk).q = (yyvsp[(0) - (1)].blk).q, Q_EQUAL); }
    break;

  case 20:

/* Line 1806 of yacc.c  */
#line 183 "grammar.y"
    { (yyval.blk).b = Argusgen_fcode(NULL, (float)(yyvsp[(2) - (2)].f), (yyval.blk).q = (yyvsp[(0) - (2)].blk).q, Q_LESS); }
    break;

  case 21:

/* Line 1806 of yacc.c  */
#line 184 "grammar.y"
    { (yyval.blk).b = Argusgen_fcode(NULL, (float)(yyvsp[(2) - (2)].f), (yyval.blk).q = (yyvsp[(0) - (2)].blk).q, Q_GREATER); }
    break;

  case 22:

/* Line 1806 of yacc.c  */
#line 185 "grammar.y"
    { (yyval.blk).b = Argusgen_fcode(NULL, (float)(yyvsp[(2) - (2)].f), (yyval.blk).q = (yyvsp[(0) - (2)].blk).q, Q_EQUAL); }
    break;

  case 23:

/* Line 1806 of yacc.c  */
#line 186 "grammar.y"
    { (yyval.blk).b = Argusgen_fcode(NULL, (float)(yyvsp[(2) - (2)].f), (yyval.blk).q = (yyvsp[(0) - (2)].blk).q, Q_GEQ); }
    break;

  case 24:

/* Line 1806 of yacc.c  */
#line 187 "grammar.y"
    { (yyval.blk).b = Argusgen_fcode(NULL, (float)(yyvsp[(2) - (2)].f), (yyval.blk).q = (yyvsp[(0) - (2)].blk).q, Q_LEQ); }
    break;

  case 25:

/* Line 1806 of yacc.c  */
#line 188 "grammar.y"
    { (yyval.blk) = (yyvsp[(2) - (3)].blk); }
    break;

  case 26:

/* Line 1806 of yacc.c  */
#line 190 "grammar.y"
    { (yyval.blk).b = Argusgen_tcode((yyvsp[(1) - (1)].i), (yyval.blk).q = (yyvsp[(0) - (1)].blk).q); }
    break;

  case 27:

/* Line 1806 of yacc.c  */
#line 192 "grammar.y"
    { (yyval.blk).b = Argusgen_scode((yyvsp[(1) - (1)].s), (yyval.blk).q = (yyvsp[(0) - (1)].blk).q); }
    break;

  case 28:

/* Line 1806 of yacc.c  */
#line 193 "grammar.y"
    { (yyval.blk).q = (yyvsp[(0) - (1)].blk).q; (yyval.blk).q.type = Q_STRING;
                                  (yyval.blk).b = Argusgen_scode((yyvsp[(1) - (1)].s), (yyval.blk).q); }
    break;

  case 29:

/* Line 1806 of yacc.c  */
#line 195 "grammar.y"
    { (yyval.blk).q = (yyvsp[(0) - (3)].blk).q; (yyval.blk).q.type = Q_IPV4;
                                  (yyval.blk).b = Argusgen_mcode((yyvsp[(1) - (3)].s), NULL, (yyvsp[(3) - (3)].i), (yyval.blk).q); }
    break;

  case 30:

/* Line 1806 of yacc.c  */
#line 197 "grammar.y"
    { (yyval.blk).q = (yyvsp[(0) - (3)].blk).q; (yyval.blk).q.type = Q_IPV4;
                                  (yyval.blk).b = Argusgen_mcode((yyvsp[(1) - (3)].s), (char *)(yyvsp[(3) - (3)].s), 0, (yyval.blk).q); }
    break;

  case 31:

/* Line 1806 of yacc.c  */
#line 199 "grammar.y"
    { (yyval.blk).q = (yyvsp[(0) - (1)].blk).q; (yyval.blk).q.type = Q_IPV4;
                                  (yyval.blk).b = Argusgen_ncode((yyvsp[(1) - (1)].s), 0, (yyval.blk).q, Q_EQUAL); }
    break;

  case 32:

/* Line 1806 of yacc.c  */
#line 201 "grammar.y"
    { (yyval.blk).q = (yyvsp[(0) - (3)].blk).q; (yyval.blk).q.type = Q_IPV6;
                                  (yyval.blk).b = Argusgen_mcode((yyvsp[(1) - (3)].s), NULL, (yyvsp[(3) - (3)].i), (yyval.blk).q); }
    break;

  case 33:

/* Line 1806 of yacc.c  */
#line 203 "grammar.y"
    { (yyval.blk).q = (yyvsp[(0) - (1)].blk).q; (yyval.blk).q.type = Q_IPV6;
                                  (yyval.blk).b = Argusgen_ncode((yyvsp[(1) - (1)].s), 0, (yyval.blk).q, Q_EQUAL); }
    break;

  case 34:

/* Line 1806 of yacc.c  */
#line 205 "grammar.y"
    { (yyval.blk).q = (yyvsp[(0) - (1)].blk).q; (yyval.blk).q.type = Q_IPV6;
				  (yyval.blk).b = Argusgen_ecode((yyvsp[(1) - (1)].e), (yyval.blk).q); }
    break;

  case 35:

/* Line 1806 of yacc.c  */
#line 207 "grammar.y"
    { Argusgen_not((yyvsp[(2) - (2)].blk).b); (yyval.blk) = (yyvsp[(2) - (2)].blk); }
    break;

  case 36:

/* Line 1806 of yacc.c  */
#line 209 "grammar.y"
    { (yyval.blk) = (yyvsp[(0) - (1)].blk); }
    break;

  case 37:

/* Line 1806 of yacc.c  */
#line 211 "grammar.y"
    { (yyval.blk) = (yyvsp[(0) - (1)].blk); }
    break;

  case 39:

/* Line 1806 of yacc.c  */
#line 214 "grammar.y"
    { Argusgen_and((yyvsp[(1) - (3)].blk).b, (yyvsp[(3) - (3)].blk).b); (yyval.blk) = (yyvsp[(3) - (3)].blk); }
    break;

  case 40:

/* Line 1806 of yacc.c  */
#line 215 "grammar.y"
    { Argusgen_or((yyvsp[(1) - (3)].blk).b, (yyvsp[(3) - (3)].blk).b); (yyval.blk) = (yyvsp[(3) - (3)].blk); }
    break;

  case 41:

/* Line 1806 of yacc.c  */
#line 217 "grammar.y"
    { (yyval.blk).b = Argusgen_ncode(NULL, (arg_uint32)(yyvsp[(1) - (1)].i), (yyval.blk).q = (yyvsp[(0) - (1)].blk).q, Q_EQUAL); }
    break;

  case 44:

/* Line 1806 of yacc.c  */
#line 221 "grammar.y"
    { Argusgen_not((yyvsp[(2) - (2)].blk).b); (yyval.blk) = (yyvsp[(2) - (2)].blk); }
    break;

  case 45:

/* Line 1806 of yacc.c  */
#line 223 "grammar.y"
    { QSET((yyval.blk).q, (yyvsp[(1) - (3)].i), (yyvsp[(2) - (3)].i), (yyvsp[(3) - (3)].i)); }
    break;

  case 46:

/* Line 1806 of yacc.c  */
#line 224 "grammar.y"
    { QSET((yyval.blk).q, (yyvsp[(1) - (2)].i), (yyvsp[(2) - (2)].i), Q_DEFAULT); }
    break;

  case 47:

/* Line 1806 of yacc.c  */
#line 225 "grammar.y"
    { QSET((yyval.blk).q, (yyvsp[(1) - (2)].i), Q_DEFAULT, (yyvsp[(2) - (2)].i)); }
    break;

  case 48:

/* Line 1806 of yacc.c  */
#line 226 "grammar.y"
    { QSET((yyval.blk).q, (yyvsp[(1) - (2)].i), Q_DEFAULT, (yyvsp[(2) - (2)].i)); }
    break;

  case 49:

/* Line 1806 of yacc.c  */
#line 227 "grammar.y"
    { QSET((yyval.blk).q, (yyvsp[(1) - (2)].i), Q_DEFAULT, (yyvsp[(2) - (2)].f)); }
    break;

  case 50:

/* Line 1806 of yacc.c  */
#line 228 "grammar.y"
    { QSET((yyval.blk).q, (yyvsp[(1) - (3)].i), (yyvsp[(2) - (3)].i), (yyvsp[(3) - (3)].i)); }
    break;

  case 51:

/* Line 1806 of yacc.c  */
#line 229 "grammar.y"
    { QSET((yyval.blk).q, (yyvsp[(1) - (3)].i), (yyvsp[(2) - (3)].i), (yyvsp[(3) - (3)].f)); }
    break;

  case 52:

/* Line 1806 of yacc.c  */
#line 230 "grammar.y"
    { QSET((yyval.blk).q, (yyvsp[(1) - (2)].i), Q_DEFAULT, Q_PROTO); }
    break;

  case 53:

/* Line 1806 of yacc.c  */
#line 231 "grammar.y"
    { QSET((yyval.blk).q, (yyvsp[(1) - (2)].i), Q_DEFAULT, (yyvsp[(2) - (2)].i)); }
    break;

  case 54:

/* Line 1806 of yacc.c  */
#line 234 "grammar.y"
    { QSET((yyval.blk).q, (yyvsp[(1) - (2)].i), (yyvsp[(2) - (2)].i), Q_DEFAULT); }
    break;

  case 55:

/* Line 1806 of yacc.c  */
#line 237 "grammar.y"
    { (yyval.blk) = (yyvsp[(2) - (2)].blk); }
    break;

  case 56:

/* Line 1806 of yacc.c  */
#line 238 "grammar.y"
    { (yyval.blk) = (yyvsp[(2) - (2)].blk); }
    break;

  case 57:

/* Line 1806 of yacc.c  */
#line 239 "grammar.y"
    { (yyval.blk).b = (yyvsp[(2) - (3)].blk).b; (yyval.blk).q = (yyvsp[(1) - (3)].blk).q; }
    break;

  case 58:

/* Line 1806 of yacc.c  */
#line 240 "grammar.y"
    { (yyval.blk).b = Argusgen_proto_abbrev((yyvsp[(1) - (1)].i)); (yyval.blk).q = qerr; }
    break;

  case 59:

/* Line 1806 of yacc.c  */
#line 241 "grammar.y"
    { (yyval.blk).b = Argusgen_proto_abbrev((yyvsp[(1) - (1)].i)); (yyval.blk).q = qerr; }
    break;

  case 60:

/* Line 1806 of yacc.c  */
#line 242 "grammar.y"
    { (yyval.blk) = (yyvsp[(1) - (1)].blk); }
    break;

  case 61:

/* Line 1806 of yacc.c  */
#line 243 "grammar.y"
    { (yyval.blk).b = Argusgen_relation((yyvsp[(2) - (3)].i), (yyvsp[(1) - (3)].a), (yyvsp[(3) - (3)].a), 0); (yyval.blk).q = qerr; }
    break;

  case 62:

/* Line 1806 of yacc.c  */
#line 244 "grammar.y"
    { (yyval.blk).b = Argusgen_relation((yyvsp[(2) - (3)].i), (yyvsp[(1) - (3)].a), (yyvsp[(3) - (3)].a), 1); (yyval.blk).q = qerr; }
    break;

  case 63:

/* Line 1806 of yacc.c  */
#line 245 "grammar.y"
    { (yyval.blk).b = (yyvsp[(1) - (1)].rblk); (yyval.blk).q = qerr; }
    break;

  case 65:

/* Line 1806 of yacc.c  */
#line 250 "grammar.y"
    { (yyval.i) = Q_DEFAULT; }
    break;

  case 66:

/* Line 1806 of yacc.c  */
#line 253 "grammar.y"
    { (yyval.i) = Q_SRC; }
    break;

  case 67:

/* Line 1806 of yacc.c  */
#line 254 "grammar.y"
    { (yyval.i) = Q_DST; }
    break;

  case 68:

/* Line 1806 of yacc.c  */
#line 255 "grammar.y"
    { (yyval.i) = Q_OR; }
    break;

  case 69:

/* Line 1806 of yacc.c  */
#line 256 "grammar.y"
    { (yyval.i) = Q_OR; }
    break;

  case 70:

/* Line 1806 of yacc.c  */
#line 257 "grammar.y"
    { (yyval.i) = Q_AND; }
    break;

  case 71:

/* Line 1806 of yacc.c  */
#line 258 "grammar.y"
    { (yyval.i) = Q_AND; }
    break;

  case 72:

/* Line 1806 of yacc.c  */
#line 261 "grammar.y"
    { (yyval.i) = Q_HOST; }
    break;

  case 73:

/* Line 1806 of yacc.c  */
#line 262 "grammar.y"
    { (yyval.i) = Q_SRCID; }
    break;

  case 74:

/* Line 1806 of yacc.c  */
#line 263 "grammar.y"
    { (yyval.i) = Q_INODE; }
    break;

  case 75:

/* Line 1806 of yacc.c  */
#line 264 "grammar.y"
    { (yyval.i) = Q_NET; }
    break;

  case 76:

/* Line 1806 of yacc.c  */
#line 267 "grammar.y"
    { (yyval.i) = Q_PORT; }
    break;

  case 77:

/* Line 1806 of yacc.c  */
#line 268 "grammar.y"
    { (yyval.i) = Q_IPID; }
    break;

  case 78:

/* Line 1806 of yacc.c  */
#line 269 "grammar.y"
    { (yyval.i) = Q_TTL; }
    break;

  case 79:

/* Line 1806 of yacc.c  */
#line 270 "grammar.y"
    { (yyval.i) = Q_TOS; }
    break;

  case 80:

/* Line 1806 of yacc.c  */
#line 271 "grammar.y"
    { (yyval.i) = Q_DSB; }
    break;

  case 81:

/* Line 1806 of yacc.c  */
#line 272 "grammar.y"
    { (yyval.i) = Q_CO; }
    break;

  case 82:

/* Line 1806 of yacc.c  */
#line 273 "grammar.y"
    { (yyval.i) = Q_ASN; }
    break;

  case 83:

/* Line 1806 of yacc.c  */
#line 274 "grammar.y"
    { (yyval.i) = Q_VID; }
    break;

  case 84:

/* Line 1806 of yacc.c  */
#line 275 "grammar.y"
    { (yyval.i) = Q_VPRI; }
    break;

  case 85:

/* Line 1806 of yacc.c  */
#line 276 "grammar.y"
    { (yyval.i) = Q_MPLSID; }
    break;

  case 86:

/* Line 1806 of yacc.c  */
#line 277 "grammar.y"
    { (yyval.i) = Q_BYTE; }
    break;

  case 87:

/* Line 1806 of yacc.c  */
#line 278 "grammar.y"
    { (yyval.i) = Q_APPBYTE; }
    break;

  case 88:

/* Line 1806 of yacc.c  */
#line 279 "grammar.y"
    { (yyval.i) = Q_PKT; }
    break;

  case 89:

/* Line 1806 of yacc.c  */
#line 280 "grammar.y"
    { (yyval.i) = Q_TRANS; }
    break;

  case 90:

/* Line 1806 of yacc.c  */
#line 281 "grammar.y"
    { (yyval.i) = Q_TCPRTT; }
    break;

  case 91:

/* Line 1806 of yacc.c  */
#line 282 "grammar.y"
    { (yyval.i) = Q_TCPBASE; }
    break;

  case 92:

/* Line 1806 of yacc.c  */
#line 283 "grammar.y"
    { (yyval.i) = Q_LOSS; }
    break;

  case 93:

/* Line 1806 of yacc.c  */
#line 284 "grammar.y"
    { (yyval.i) = Q_SPI; }
    break;

  case 94:

/* Line 1806 of yacc.c  */
#line 285 "grammar.y"
    { (yyval.i) = Q_ENCAPS; }
    break;

  case 95:

/* Line 1806 of yacc.c  */
#line 286 "grammar.y"
    { (yyval.i) = Q_DELTADUR; }
    break;

  case 96:

/* Line 1806 of yacc.c  */
#line 287 "grammar.y"
    { (yyval.i) = Q_DELTASTART; }
    break;

  case 97:

/* Line 1806 of yacc.c  */
#line 288 "grammar.y"
    { (yyval.i) = Q_DELTALAST; }
    break;

  case 98:

/* Line 1806 of yacc.c  */
#line 289 "grammar.y"
    { (yyval.i) = Q_NSTROKE; }
    break;

  case 99:

/* Line 1806 of yacc.c  */
#line 290 "grammar.y"
    { (yyval.i) = Q_SEQ; }
    break;

  case 100:

/* Line 1806 of yacc.c  */
#line 293 "grammar.y"
    { (yyval.f) = Q_DUR; }
    break;

  case 101:

/* Line 1806 of yacc.c  */
#line 294 "grammar.y"
    { (yyval.f) = Q_AVGDUR; }
    break;

  case 102:

/* Line 1806 of yacc.c  */
#line 295 "grammar.y"
    { (yyval.f) = Q_INTER; }
    break;

  case 103:

/* Line 1806 of yacc.c  */
#line 296 "grammar.y"
    { (yyval.f) = Q_JITTER; }
    break;

  case 104:

/* Line 1806 of yacc.c  */
#line 297 "grammar.y"
    { (yyval.f) = Q_RATE; }
    break;

  case 105:

/* Line 1806 of yacc.c  */
#line 298 "grammar.y"
    { (yyval.f) = Q_LOAD; }
    break;

  case 106:

/* Line 1806 of yacc.c  */
#line 299 "grammar.y"
    { (yyval.f) = Q_PLOSS; }
    break;

  case 107:

/* Line 1806 of yacc.c  */
#line 302 "grammar.y"
    { (yyval.i) = Q_GATEWAY; }
    break;

  case 108:

/* Line 1806 of yacc.c  */
#line 305 "grammar.y"
    { (yyval.i) = Q_START; }
    break;

  case 109:

/* Line 1806 of yacc.c  */
#line 306 "grammar.y"
    { (yyval.i) = Q_STOP; }
    break;

  case 110:

/* Line 1806 of yacc.c  */
#line 307 "grammar.y"
    { (yyval.i) = Q_STATUS; }
    break;

  case 111:

/* Line 1806 of yacc.c  */
#line 308 "grammar.y"
    { (yyval.i) = Q_SHUTDOWN; }
    break;

  case 112:

/* Line 1806 of yacc.c  */
#line 309 "grammar.y"
    { (yyval.i) = Q_ERROR; }
    break;

  case 113:

/* Line 1806 of yacc.c  */
#line 310 "grammar.y"
    { (yyval.i) = Q_MAN; }
    break;

  case 114:

/* Line 1806 of yacc.c  */
#line 311 "grammar.y"
    { (yyval.i) = Q_FAR; }
    break;

  case 115:

/* Line 1806 of yacc.c  */
#line 312 "grammar.y"
    { (yyval.i) = Q_EVENT; }
    break;

  case 116:

/* Line 1806 of yacc.c  */
#line 313 "grammar.y"
    { (yyval.i) = Q_INDEX; }
    break;

  case 117:

/* Line 1806 of yacc.c  */
#line 314 "grammar.y"
    { (yyval.i) = Q_SVC; }
    break;

  case 118:

/* Line 1806 of yacc.c  */
#line 315 "grammar.y"
    { (yyval.i) = Q_NORMAL; }
    break;

  case 119:

/* Line 1806 of yacc.c  */
#line 316 "grammar.y"
    { (yyval.i) = Q_WAIT; }
    break;

  case 120:

/* Line 1806 of yacc.c  */
#line 317 "grammar.y"
    { (yyval.i) = Q_SYNACK; }
    break;

  case 121:

/* Line 1806 of yacc.c  */
#line 318 "grammar.y"
    { (yyval.i) = Q_FINACK; }
    break;

  case 122:

/* Line 1806 of yacc.c  */
#line 319 "grammar.y"
    { (yyval.i) = Q_ESTABLISHED; }
    break;

  case 123:

/* Line 1806 of yacc.c  */
#line 320 "grammar.y"
    { (yyval.i) = Q_CONNECTED; }
    break;

  case 124:

/* Line 1806 of yacc.c  */
#line 321 "grammar.y"
    { (yyval.i) = Q_CORRELATED; }
    break;

  case 125:

/* Line 1806 of yacc.c  */
#line 322 "grammar.y"
    { (yyval.i) = Q_TIMEDOUT; }
    break;

  case 126:

/* Line 1806 of yacc.c  */
#line 323 "grammar.y"
    { (yyval.i) = Q_ICMPMAP; }
    break;

  case 127:

/* Line 1806 of yacc.c  */
#line 324 "grammar.y"
    { (yyval.i) = Q_ECHO; }
    break;

  case 128:

/* Line 1806 of yacc.c  */
#line 325 "grammar.y"
    { (yyval.i) = Q_UNREACH; }
    break;

  case 129:

/* Line 1806 of yacc.c  */
#line 326 "grammar.y"
    { (yyval.i) = Q_REDIRECT; }
    break;

  case 130:

/* Line 1806 of yacc.c  */
#line 327 "grammar.y"
    { (yyval.i) = Q_TIMEXED; }
    break;

  case 131:

/* Line 1806 of yacc.c  */
#line 328 "grammar.y"
    { (yyval.i) = Q_RTR; }
    break;

  case 132:

/* Line 1806 of yacc.c  */
#line 329 "grammar.y"
    { (yyval.i) = Q_MBR; }
    break;

  case 133:

/* Line 1806 of yacc.c  */
#line 330 "grammar.y"
    { (yyval.i) = Q_LVG; }
    break;

  case 134:

/* Line 1806 of yacc.c  */
#line 331 "grammar.y"
    { (yyval.i) = Q_COCODE; }
    break;

  case 135:

/* Line 1806 of yacc.c  */
#line 334 "grammar.y"
    { (yyval.i) = Q_LINK; }
    break;

  case 136:

/* Line 1806 of yacc.c  */
#line 335 "grammar.y"
    { (yyval.i) = Q_ETHER; }
    break;

  case 137:

/* Line 1806 of yacc.c  */
#line 336 "grammar.y"
    { (yyval.i) = Q_IP; }
    break;

  case 138:

/* Line 1806 of yacc.c  */
#line 337 "grammar.y"
    { (yyval.i) = Q_IPV4; }
    break;

  case 139:

/* Line 1806 of yacc.c  */
#line 338 "grammar.y"
    { (yyval.i) = Q_IPV6; }
    break;

  case 140:

/* Line 1806 of yacc.c  */
#line 339 "grammar.y"
    { (yyval.i) = Q_ARP; }
    break;

  case 141:

/* Line 1806 of yacc.c  */
#line 340 "grammar.y"
    { (yyval.i) = Q_RARP; }
    break;

  case 142:

/* Line 1806 of yacc.c  */
#line 341 "grammar.y"
    { (yyval.i) = Q_ESP; }
    break;

  case 143:

/* Line 1806 of yacc.c  */
#line 342 "grammar.y"
    { (yyval.i) = Q_RTP; }
    break;

  case 144:

/* Line 1806 of yacc.c  */
#line 343 "grammar.y"
    { (yyval.i) = Q_RTCP; }
    break;

  case 145:

/* Line 1806 of yacc.c  */
#line 344 "grammar.y"
    { (yyval.i) = Q_TCP; }
    break;

  case 146:

/* Line 1806 of yacc.c  */
#line 345 "grammar.y"
    { (yyval.i) = Q_UDP; }
    break;

  case 147:

/* Line 1806 of yacc.c  */
#line 346 "grammar.y"
    { (yyval.i) = Q_ICMP; }
    break;

  case 148:

/* Line 1806 of yacc.c  */
#line 347 "grammar.y"
    { (yyval.i) = Q_IGMP; }
    break;

  case 149:

/* Line 1806 of yacc.c  */
#line 348 "grammar.y"
    { (yyval.i) = Q_MPLS; }
    break;

  case 150:

/* Line 1806 of yacc.c  */
#line 349 "grammar.y"
    { (yyval.i) = Q_ISIS; }
    break;

  case 151:

/* Line 1806 of yacc.c  */
#line 350 "grammar.y"
    { (yyval.i) = Q_VLAN; }
    break;

  case 152:

/* Line 1806 of yacc.c  */
#line 351 "grammar.y"
    { (yyval.i) = Q_UDT; }
    break;

  case 153:

/* Line 1806 of yacc.c  */
#line 352 "grammar.y"
    { (yyval.i) = Q_ANON; }
    break;

  case 154:

/* Line 1806 of yacc.c  */
#line 353 "grammar.y"
    { (yyval.i) = Q_DECNET; }
    break;

  case 155:

/* Line 1806 of yacc.c  */
#line 354 "grammar.y"
    { (yyval.i) = Q_LAT; }
    break;

  case 156:

/* Line 1806 of yacc.c  */
#line 355 "grammar.y"
    { (yyval.i) = Q_MOPDL; }
    break;

  case 157:

/* Line 1806 of yacc.c  */
#line 356 "grammar.y"
    { (yyval.i) = Q_MOPRC; }
    break;

  case 158:

/* Line 1806 of yacc.c  */
#line 359 "grammar.y"
    { (yyval.i) = Q_OUTOFORDER; }
    break;

  case 159:

/* Line 1806 of yacc.c  */
#line 360 "grammar.y"
    { (yyval.i) = Q_RETRANS; }
    break;

  case 160:

/* Line 1806 of yacc.c  */
#line 361 "grammar.y"
    { (yyval.i) = Q_WINSHUT; }
    break;

  case 161:

/* Line 1806 of yacc.c  */
#line 362 "grammar.y"
    { (yyval.i) = Q_SYN; }
    break;

  case 162:

/* Line 1806 of yacc.c  */
#line 363 "grammar.y"
    { (yyval.i) = Q_FIN; }
    break;

  case 163:

/* Line 1806 of yacc.c  */
#line 364 "grammar.y"
    { (yyval.i) = Q_RESET; }
    break;

  case 164:

/* Line 1806 of yacc.c  */
#line 365 "grammar.y"
    { (yyval.i) = Q_ACK; }
    break;

  case 165:

/* Line 1806 of yacc.c  */
#line 366 "grammar.y"
    { (yyval.i) = Q_PUSH; }
    break;

  case 166:

/* Line 1806 of yacc.c  */
#line 367 "grammar.y"
    { (yyval.i) = Q_URGENT; }
    break;

  case 167:

/* Line 1806 of yacc.c  */
#line 368 "grammar.y"
    { (yyval.i) = Q_CWR; }
    break;

  case 168:

/* Line 1806 of yacc.c  */
#line 369 "grammar.y"
    { (yyval.i) = Q_ECE; }
    break;

  case 169:

/* Line 1806 of yacc.c  */
#line 370 "grammar.y"
    { (yyval.i) = Q_FRAG; }
    break;

  case 170:

/* Line 1806 of yacc.c  */
#line 371 "grammar.y"
    { (yyval.i) = Q_FRAG_ONLY; }
    break;

  case 171:

/* Line 1806 of yacc.c  */
#line 372 "grammar.y"
    { (yyval.i) = Q_ECN; }
    break;

  case 172:

/* Line 1806 of yacc.c  */
#line 373 "grammar.y"
    { (yyval.i) = Q_MULTIPATH; }
    break;

  case 173:

/* Line 1806 of yacc.c  */
#line 374 "grammar.y"
    { (yyval.i) = Q_HELLO; }
    break;

  case 174:

/* Line 1806 of yacc.c  */
#line 375 "grammar.y"
    { (yyval.i) = Q_LSP; }
    break;

  case 175:

/* Line 1806 of yacc.c  */
#line 376 "grammar.y"
    { (yyval.i) = Q_CSNP; }
    break;

  case 176:

/* Line 1806 of yacc.c  */
#line 377 "grammar.y"
    { (yyval.i) = Q_PSNP; }
    break;

  case 177:

/* Line 1806 of yacc.c  */
#line 379 "grammar.y"
    { (yyval.rblk) = Argusgen_broadcast((yyvsp[(1) - (2)].i)); }
    break;

  case 178:

/* Line 1806 of yacc.c  */
#line 380 "grammar.y"
    { (yyval.rblk) = Argusgen_multicast((yyvsp[(1) - (2)].i)); }
    break;

  case 179:

/* Line 1806 of yacc.c  */
#line 381 "grammar.y"
    { (yyval.rblk) = Argusgen_inbound(0); }
    break;

  case 180:

/* Line 1806 of yacc.c  */
#line 382 "grammar.y"
    { (yyval.rblk) = Argusgen_inbound(1); }
    break;

  case 181:

/* Line 1806 of yacc.c  */
#line 384 "grammar.y"
    { (yyval.i) = NFF_JGT; }
    break;

  case 182:

/* Line 1806 of yacc.c  */
#line 385 "grammar.y"
    { (yyval.i) = NFF_JGE; }
    break;

  case 183:

/* Line 1806 of yacc.c  */
#line 386 "grammar.y"
    { (yyval.i) = NFF_JEQ; }
    break;

  case 184:

/* Line 1806 of yacc.c  */
#line 387 "grammar.y"
    { (yyval.i) = NFF_JEQ; }
    break;

  case 185:

/* Line 1806 of yacc.c  */
#line 389 "grammar.y"
    { (yyval.i) = NFF_JGT; }
    break;

  case 186:

/* Line 1806 of yacc.c  */
#line 390 "grammar.y"
    { (yyval.i) = NFF_JGE; }
    break;

  case 187:

/* Line 1806 of yacc.c  */
#line 391 "grammar.y"
    { (yyval.i) = NFF_JEQ; }
    break;

  case 188:

/* Line 1806 of yacc.c  */
#line 393 "grammar.y"
    { (yyval.a) = ArgusLoadI((yyvsp[(1) - (1)].i)); }
    break;

  case 190:

/* Line 1806 of yacc.c  */
#line 396 "grammar.y"
    { (yyval.a) = ArgusLoad((yyvsp[(1) - (4)].i), (yyvsp[(3) - (4)].a), 1); }
    break;

  case 191:

/* Line 1806 of yacc.c  */
#line 397 "grammar.y"
    { (yyval.a) = ArgusLoad((yyvsp[(1) - (6)].i), (yyvsp[(3) - (6)].a), (yyvsp[(5) - (6)].i)); }
    break;

  case 192:

/* Line 1806 of yacc.c  */
#line 398 "grammar.y"
    { (yyval.a) = ArgusArth(NFF_ADD, (yyvsp[(1) - (3)].a), (yyvsp[(3) - (3)].a)); }
    break;

  case 193:

/* Line 1806 of yacc.c  */
#line 399 "grammar.y"
    { (yyval.a) = ArgusArth(NFF_SUB, (yyvsp[(1) - (3)].a), (yyvsp[(3) - (3)].a)); }
    break;

  case 194:

/* Line 1806 of yacc.c  */
#line 400 "grammar.y"
    { (yyval.a) = ArgusArth(NFF_MUL, (yyvsp[(1) - (3)].a), (yyvsp[(3) - (3)].a)); }
    break;

  case 195:

/* Line 1806 of yacc.c  */
#line 401 "grammar.y"
    { (yyval.a) = ArgusArth(NFF_DIV, (yyvsp[(1) - (3)].a), (yyvsp[(3) - (3)].a)); }
    break;

  case 196:

/* Line 1806 of yacc.c  */
#line 402 "grammar.y"
    { (yyval.a) = ArgusArth(NFF_AND, (yyvsp[(1) - (3)].a), (yyvsp[(3) - (3)].a)); }
    break;

  case 197:

/* Line 1806 of yacc.c  */
#line 403 "grammar.y"
    { (yyval.a) = ArgusArth(NFF_OR, (yyvsp[(1) - (3)].a), (yyvsp[(3) - (3)].a)); }
    break;

  case 198:

/* Line 1806 of yacc.c  */
#line 404 "grammar.y"
    { (yyval.a) = ArgusArth(NFF_LSH, (yyvsp[(1) - (3)].a), (yyvsp[(3) - (3)].a)); }
    break;

  case 199:

/* Line 1806 of yacc.c  */
#line 405 "grammar.y"
    { (yyval.a) = ArgusArth(NFF_RSH, (yyvsp[(1) - (3)].a), (yyvsp[(3) - (3)].a)); }
    break;

  case 200:

/* Line 1806 of yacc.c  */
#line 406 "grammar.y"
    { (yyval.a) = ArgusNeg((yyvsp[(2) - (2)].a)); }
    break;

  case 201:

/* Line 1806 of yacc.c  */
#line 407 "grammar.y"
    { (yyval.a) = (yyvsp[(2) - (3)].a); }
    break;

  case 202:

/* Line 1806 of yacc.c  */
#line 408 "grammar.y"
    { (yyval.a) = ArgusLoadLen(); }
    break;

  case 204:

/* Line 1806 of yacc.c  */
#line 411 "grammar.y"
    { (yyval.i) = (yyvsp[(2) - (3)].i); }
    break;

  case 206:

/* Line 1806 of yacc.c  */
#line 414 "grammar.y"
    { (yyval.f) = (yyvsp[(2) - (3)].f); }
    break;



/* Line 1806 of yacc.c  */
#line 3608 "y.tab.c"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
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
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
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
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
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
      if (!yypact_value_is_default (yyn))
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
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

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

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
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



/* Line 2067 of yacc.c  */
#line 416 "grammar.y"


int
yyerror(char *msg)
{
   ++argus_n_errors;
   exit(1);
	/* NOTREACHED */
}

