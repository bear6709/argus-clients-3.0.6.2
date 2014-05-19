/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
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

/* Line 2068 of yacc.c  */
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



/* Line 2068 of yacc.c  */
#line 349 "y.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE argus_lval;


