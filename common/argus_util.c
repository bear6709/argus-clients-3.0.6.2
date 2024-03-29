/*
 * Argus Software
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
 */

/*
 * Copyright (c) 1988-1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that: (1) source code distributions
 * retain the above copyright notice and this paragraph in its entirety, (2)
 * distributions including binary code include the above copyright notice and
 * this paragraph in its entirety in the documentation or other materials
 * provided with the distribution, and (3) all advertising materials mentioning
 * features or use of this software display the following acknowledgement:
 * ``This product includes software developed by the University of California,
 * Lawrence Berkeley Laboratory and its contributors.'' Neither the name of
 * the University nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior
 * written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/* 
 * $Id: //depot/argus-3.0.6/clients/common/argus_util.c#2 $
 * $DateTime: 2012/06/15 11:08:00 $
 * $Change: 2424 $
 */

#ifdef HAVE_CONFIG_H
#include "argus_config.h"
#endif

#define ArgusUtil

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

#if defined(__NetBSD__)
#include <machine/limits.h>
#endif

#include <syslog.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>

#include <argus_compat.h>

#include <netinet/in.h>
#include <netinet/ip6.h>
#include <arpa/inet.h>

#if defined(HAVE_SOLARIS) || defined(linux)
#include <netinet/icmp6.h>
#endif

#include <string.h>
#include <sys/stat.h>
#include <ctype.h>
#include <math.h>

#include <rpc/types.h>
#include <rpc/xdr.h>

#include <time.h>

#include <argus_int.h>
#include <argus_def.h>
#include <argus_out.h>

#include <argus_util.h>
#include <argus_parser.h>
#include <argus_filter.h>
#include <argus_client.h>
#include <argus_main.h>
#include <argus_label.h>
#include <argus_metric.h>
#include <argus_grep.h>
#include <argus_ethertype.h>
#include <argus_dscodepoints.h>
#include <argus_encapsulations.h>

#if defined(HAVE_FLOWTOOLS)
#include "ftlib.h"
#endif

#ifndef AF_INET6
#define AF_INET6	23
#endif


#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN	46
#endif

int target_flags = 0;
extern void ArgusLog (int, char *, ...);
extern void RaParseComplete (int);

typedef struct {
    uint64_t high, low;
} uint128;
 
void sprint128 (char *, char *, uint128 *);

int ArgusGenerateCanonRecord (struct ArgusRecordStruct *);

void ArgusPrintEspSpi (struct ArgusParserStruct *, char *, struct ArgusRecordStruct *, int, u_int, int);
char *ArgusAbbreviateMetric(struct ArgusParserStruct *, char *, int, double);

void RaClearConfiguration (struct ArgusParserStruct *);

#define ARGUS_RCITEMS                           64

#define RA_ARGUS_SERVER                         0
#define RA_SOURCE_PORT				1
#define RA_CISCONETFLOW_PORT                    2
#define RA_ARGUS_SERVERPORT                     3
#define RA_INPUT_FILE                           4
#define RA_NO_OUTPUT                            5
#define RA_USER_AUTH                            6
#define RA_AUTH_PASS                            7
#define RA_OUTPUT_FILE                          8
#define RA_EXCEPTION_OUTPUT_FILE                9
#define RA_TIMERANGE                            10
#define RA_RUN_TIME                             11
#define RA_NUMBER                               12
#define RA_FLOW_MODEL                           13
#define RA_FIELD_DELIMITER                      14
#define RA_TIME_FORMAT                          15
#define RA_TZ                                   16
#define RA_USEC_PRECISION                       17
#define RA_PRINT_MAN                            18
#define RA_PRINT_EVENT                          19
#define RA_PRINT_LABELS                         20
#define RA_PRINT_SUMMARY                        21
#define RA_PRINT_NAMES                          22
#define RA_PRINT_LOCALONLY                      23
#define RA_PRINT_DOMAINONLY                     24
#define RA_PRINT_RESPONSE_DATA                  25
#define RA_PRINT_TCPSTATES                      26
#define RA_PRINT_TCPFLAGS                       27
#define RAMON_MODE                              28
#define RA_DEBUG_LEVEL                          29
#define RA_USERDATA_ENCODE                      30
#define RA_FILTER                               31
#define RA_FIELD_SPECIFIER                      32
#define RA_MIN_SSF                              33
#define RA_MAX_SSF                              34
#define ARGUS_ARCHIVE                           35
#define RADIUM_DAEMON                           36
#define RADIUM_MONITOR_ID                       37
#define RADIUM_MAR_STATUS_INTERVAL              38
#define RADIUM_ADJUST_TIME                      39
#define RADIUM_ACCESS_PORT                      40
#define RA_CONNECT_TIME                         41
#define RA_UPDATE_INTERVAL                      42
#define RA_FIELD_QUOTED                         43
#define RA_FIELD_WIDTH                          44
#define RA_SET_PID                              45
#define RA_PID_PATH                             46
#define RA_DELEGATED_IP                         47
#define RA_RELIABLE_CONNECT                     48
#define RA_DATABASE				49
#define RA_DB_USER				50
#define RA_DB_PASS				51
#define RA_DB_HOST				52
#define RA_DB_PORT				53
#define RA_DB_TABLE				54
#define RA_AIS_CACHE                            55
#define RA_PRINT_UNIX_TIME                      56
#define RA_TIMEOUT_INTERVAL                     57
#define RA_CIDR_ADDRESS_FORMAT			58
#define RA_SORT_ALGORITHMS			59
#define MYSQL_DB_ENGINE				60
#define RA_PRINT_ETHERNET_VENDORS		61
#define RA_ETHERNET_VENDORS			62
#define RA_SPECIFY_LOCAL			63


char *ArgusResourceFileStr [] = {
   "RA_ARGUS_SERVER=",
   "RA_SOURCE_PORT=",
   "RA_CISCONETFLOW_PORT=",
   "RA_ARGUS_SERVERPORT=",
   "RA_INPUT_FILE=",
   "RA_NO_OUTPUT=",
   "RA_USER_AUTH=",
   "RA_AUTH_PASS=",
   "RA_OUTPUT_FILE=",
   "RA_EXCEPTION_OUTPUT_FILE=",
   "RA_TIMERANGE=",
   "RA_RUN_TIME=",
   "RA_NUMBER=",
   "RA_FLOW_MODEL=",
   "RA_FIELD_DELIMITER=",
   "RA_TIME_FORMAT=",
   "RA_TZ=",
   "RA_USEC_PRECISION=",
   "RA_PRINT_MAN_RECORDS=",
   "RA_PRINT_EVENT_RECORDS=",
   "RA_PRINT_LABELS=",
   "RA_PRINT_SUMMARY=",
   "RA_PRINT_NAMES=",
   "RA_PRINT_LOCALONLY=",
   "RA_PRINT_DOMAINONLY=",
   "RA_PRINT_RESPONSE_DATA=",
   "RA_PRINT_TCPSTATES=",
   "RA_PRINT_TCPFLAGS=",
   "RAMON_MODE=",
   "RA_DEBUG_LEVEL=",
   "RA_USERDATA_ENCODE=",
   "RA_FILTER=",
   "RA_FIELD_SPECIFIER=",
   "RA_MIN_SSF=",
   "RA_MAX_SSF=",
   "ARGUS_ARCHIVE=",
   "RADIUM_DAEMON=",
   "RADIUM_MONITOR_ID=",
   "RADIUM_MAR_STATUS_INTERVAL=",
   "RADIUM_ADJUST_TIME=",
   "RADIUM_ACCESS_PORT=",
   "RA_CONNECT_TIME=",
   "RA_UPDATE_INTERVAL=",
   "RA_FIELD_QUOTED=",
   "RA_FIELD_WIDTH=",
   "RA_SET_PID=",
   "RA_PID_PATH=",
   "RA_DELEGATED_IP=",
   "RA_RELIABLE_CONNECT=",
   "RA_DATABASE=",
   "RA_DB_USER=",
   "RA_DB_PASS=",
   "RA_DB_HOST=",
   "RA_DB_PORT=",
   "RA_DB_TABLE=",
   "RA_AIS_CACHE=",
   "RA_PRINT_UNIX_TIME=",
   "RA_TIMEOUT_INTERVAL=",
   "RA_CIDR_ADDRESS_FORMAT=",
   "RA_SORT_ALGORITHMS=",
   "MYSQL_DB_ENGINE=",
   "RA_PRINT_ETHERNET_VENDORS=",
   "RA_ETHERNET_VENDORS=",
   "RA_SPECIFY_LOCAL=",
};

#include <ctype.h>

const struct tok ethertype_values[] = {
    { ETHERTYPE_IP,             "IPv4" },
    { ETHERTYPE_MPLS,           "MPLS unicast" },
    { ETHERTYPE_MPLS_MULTI,     "MPLS multicast" },
    { ETHERTYPE_IPV6,           "IPv6" },
    { ETHERTYPE_8021Q,          "802.1Q" },
    { ETHERTYPE_VMAN,           "VMAN" },
    { ETHERTYPE_PUP,            "PUP" }, 
    { ETHERTYPE_ARP,            "ARP"},
    { ETHERTYPE_REVARP ,        "Reverse ARP"},
    { ETHERTYPE_NS,             "NS" },
    { ETHERTYPE_SPRITE,         "Sprite" },
    { ETHERTYPE_TRAIL,          "Trail" },
    { ETHERTYPE_MOPDL,          "MOP DL" },
    { ETHERTYPE_MOPRC,          "MOP RC" },
    { ETHERTYPE_DN,             "DN" },
    { ETHERTYPE_LAT,            "LAT" },
    { ETHERTYPE_SCA,            "SCA" },
    { ETHERTYPE_LANBRIDGE,      "Lanbridge" },
    { ETHERTYPE_DECDNS,         "DEC DNS" },
    { ETHERTYPE_DECDTS,         "DEC DTS" },
    { ETHERTYPE_VEXP,           "VEXP" },
    { ETHERTYPE_VPROD,          "VPROD" },
    { ETHERTYPE_ATALK,          "Appletalk" },
    { ETHERTYPE_AARP,           "Appletalk ARP" },
    { ETHERTYPE_IPX,            "IPX" },
    { ETHERTYPE_PPP,            "PPP" },
    { ETHERTYPE_PPPOED,         "PPPoE D" },
    { ETHERTYPE_PPPOES,         "PPPoE S" },
    { ETHERTYPE_LOOPBACK,       "Loopback" },
    { 0, NULL}
};

struct h6namemem h6nametable[HASHNAMESIZE];
struct hnamemem  hnametable[HASHNAMESIZE];
struct hnamemem  tporttable[HASHNAMESIZE];
struct hnamemem  uporttable[HASHNAMESIZE];
struct hnamemem  rporttable[HASHNAMESIZE];
struct hnamemem  eprototable[HASHNAMESIZE];
struct hnamemem  nnametable[HASHNAMESIZE];
struct hnamemem  llcsaptable[HASHNAMESIZE];
struct evendmem  ethervendor[HASHNAMESIZE];
   
void setArguspidflag (struct ArgusParserStruct *, int);
int getArguspidflag (struct ArgusParserStruct *);
const char *tok2str(const struct tok *, const char *, int);
char *bittok2str(const struct tok *, const char *, int); 
int print_unknown_data(const u_char *, const char *, int);
void hex_print_with_offset(const u_char *, const u_char *, u_int, u_int);
void hex_print(const u_char *, const u_char *, u_int);
void relts_print(char *, int);

#include <dirent.h>

int RaSortFileList (const void *, const void *);
void ArgusSortFileList (struct ArgusInput **);


int
RaProcessRecursiveFiles (char *path)
{
   int retn = 1;
   struct stat statbuf;

   if (stat(path, &statbuf) < 0)
      return(0);

   if ((strlen(path) > 1) && ((path[0] == '.') && (path[1] != '/')))
      return (0);

   if ((statbuf.st_mode & S_IFMT) == S_IFDIR) {
      retn = RaDescend (path);
   } else {
      if ((statbuf.st_mode & S_IFMT) == S_IFREG) {
#ifdef ARGUSDEBUG
         ArgusDebug (1, "RaProcessRecursiveFiles: adding %s\n", path);
#endif
         if (!(ArgusAddFileList (ArgusParser, path, ARGUS_DATA_SOURCE, -1, -1)))
            ArgusLog (LOG_ERR, "error: -R file arg %s\n", path);
      }
   }

   ArgusSortFileList (&ArgusParser->ArgusInputFileList);
   return (retn);
}

 
int
RaDescend(char *name)
{
   int retn = 0;
   DIR *dir;
   struct dirent *direntry;
   struct stat statbuf;
   char buf[MAXSTRLEN];
 
   if (stat(name, &statbuf) < 0)
      return(0);
 
   if ((dir = opendir(name)) != NULL) {
      while ((direntry = readdir(dir)) != NULL) {
         if (*direntry->d_name != '.') {
            snprintf (buf, MAXSTRLEN, "%s/%s", name, direntry->d_name);
            if (stat(buf, &statbuf) == 0) {
               if ((statbuf.st_mode & S_IFMT) == S_IFDIR) {
                  retn += RaDescend(buf);
 
               } else {
                  if ((statbuf.st_mode & S_IFMT) == S_IFREG) {
#ifdef ARGUSDEBUG
                     ArgusDebug (1, "RaDescend: adding %s\n", buf);
#endif
                     if (!(ArgusAddFileList (ArgusParser, buf, ARGUS_DATA_SOURCE, -1, -1)))
                        ArgusLog (LOG_ERR, "error: -R file arg %s\n", buf);

                     retn++;
                  }
               }
            }
         }
      }
      closedir(dir);

   }
 
   return(retn);
}



int
RaSortFileList (const void *item1, const void *item2)
{
   struct ArgusInput *input1 = *(struct ArgusInput **) item1;
   struct ArgusInput *input2 = *(struct ArgusInput **) item2;

   return (strcmp (input1->filename, input2->filename));
}


void
ArgusSortFileList (struct ArgusInput **list)
{
   struct ArgusInput *input = NULL;
   void **array = NULL;
   int count = 0, i;

   if ((input = *list) != NULL) {
      while (input != NULL) {
         count++;
         input = (struct ArgusInput *)input->qhdr.nxt;
      }

      if ((array = ArgusCalloc (count, sizeof(input))) == NULL)
         ArgusLog (LOG_ERR, "ArgusSortFileList: ArgusCalloc %s", strerror(errno));

      for (i = 0, input = *list ; i < count; i++) {
          array[i] = input;
          input = (struct ArgusInput *)input->qhdr.nxt;
      }

      qsort (array, count, sizeof(input), RaSortFileList);

      for (i = 0; i < count; i++) {
         ((struct ArgusInput *)array[i])->qhdr.nxt = NULL;
         if (i > 0)
            ((struct ArgusInput *)array[i - 1])->qhdr.nxt = &((struct ArgusInput *)array[i])->qhdr;
      }

      *list = array[0];
      ArgusFree (array);
   }
}



void
ArgusHandleSig (int sig)
{
   int value = 0;

#ifdef ARGUSDEBUG
   ArgusDebug (1, "ArgusHandleSig: received signal %d", sig);
#endif

   switch (sig) {
      case SIGUSR1:
         value = ArgusParser->debugflag;
         ArgusParser->debugflag = (value == 15) ? 15 : value + 1;
         break;

      case SIGUSR2:
         ArgusParser->debugflag = 0;
         break;

      case SIGTERM:
      case SIGQUIT:
      case SIGINT: 
         ArgusParser->RaParseDone++;
         ArgusShutDown(sig);
         break;

      default:
         break;
   }
}

void
ArgusShutDown (int sig)
{
#ifdef ARGUSDEBUG
   ArgusDebug (1, "ArgusShutDown (%d)\n", sig);
#endif

   if (!(ArgusParser->RaShutDown++)) {
      if (ArgusParser->RaPrintOptionIndex > 0) {
         int x, cnt = ArgusParser->RaPrintOptionIndex;
         for (x = 0; x < cnt; x++)
            free(ArgusParser->RaPrintOptionStrings[x]);
      }

      if (ArgusParser->writeDbstr != NULL) {
         free(ArgusParser->writeDbstr);
         ArgusParser->writeDbstr = NULL;
      }
      if (ArgusParser->readDbstr != NULL) {
         free(ArgusParser->readDbstr);
         ArgusParser->readDbstr = NULL;
      }
      if (ArgusParser->dbuserstr != NULL) {
         free(ArgusParser->dbuserstr);
         ArgusParser->dbuserstr = NULL;
      }
      if (ArgusParser->dbpassstr != NULL) {
         free(ArgusParser->dbpassstr);
         ArgusParser->dbpassstr = NULL;
      }
      if (ArgusParser->dbhoststr != NULL) {
         free(ArgusParser->dbhoststr);
         ArgusParser->dbhoststr = NULL;
      }
      if (ArgusParser->dbportstr != NULL) {
         free(ArgusParser->dbportstr);
         ArgusParser->dbportstr = NULL;
      }
      if (ArgusParser->ais != NULL) {
         free(ArgusParser->ais);
         ArgusParser->ais = NULL;
      }

      if (sig >= 0)
         RaParseComplete (0);

      if (ArgusParser->ArgusRemoteHosts != NULL) {
         struct ArgusQueueStruct *queue =  ArgusParser->ArgusRemoteHosts;
         struct ArgusInput *input = NULL;
 
         while (queue->count > 0) {
            if ((input = (struct ArgusInput *) ArgusPopQueue(queue, ARGUS_LOCK)) != NULL) {
               ArgusCloseInput(ArgusParser, input);
               if (input->hostname != NULL)
                  free (input->hostname);
               if (input->filename != NULL)
                  free (input->filename);
#if defined(HAVE_GETADDRINFO)
               if (input->host != NULL)
                  freeaddrinfo (input->host);
#endif
               ArgusFree(input);
            }
         }
         ArgusDeleteQueue(queue);
         ArgusParser->ArgusRemoteHosts = NULL;
      }

      if (ArgusParser->ArgusActiveHosts != NULL) {
         struct ArgusQueueStruct *queue =  ArgusParser->ArgusActiveHosts;
         struct ArgusInput *input = NULL;
#if defined(ARGUS_THREADS)
         void *retn;
#endif
         while ((input = (void *)ArgusPopQueue(queue, ARGUS_LOCK)) != NULL) {
            ArgusCloseInput(ArgusParser, input);
            if (input->hostname != NULL)
               free (input->hostname);
            if (input->filename != NULL)
               free (input->filename);
#if defined(HAVE_GETADDRINFO)
            if (input->host != NULL)
               freeaddrinfo (input->host);
#endif

#if defined(ARGUS_THREADS) 
            if (input->tid != (pthread_t) 0)
               pthread_join(input->tid, &retn);
#endif

            ArgusFree(input);
         }

         ArgusDeleteQueue(queue);
         ArgusParser->ArgusActiveHosts = NULL;
      }

      ArgusWindowClose();

      if (ArgusParser->ArgusExitStatus == 0) {
         if ((ArgusParser->ArgusGrepSource || ArgusParser->ArgusGrepDestination) ||
            ((ArgusParser->ArgusRemoteFilter != NULL) || (ArgusParser->ArgusLocalFilter != NULL))) {
            if (!(ArgusParser->ArgusTotalRecords > 1)) {
               ArgusParser->ArgusExitStatus = 1;
            }
         } else {
            if (ArgusParser->ArgusTotalMarRecords == 0) {
               ArgusParser->ArgusExitStatus = 1;
            }
         }
      }

      if (ArgusParser->pidflag)
         if (ArgusParser->ArgusPidFile)
            ArgusDeletePIDFile (ArgusParser);

      if (ArgusParser->ArgusPrintXml && ArgusParser->RaXMLStarted)
         printf("</ArgusDataStream>\n"); 

#ifdef ARGUSDEBUG
      ArgusDebug (2, "RaParseComplete(caught signal %d)\n", sig);
#endif
      switch (sig) {
         case SIGHUP:
         case SIGINT:
         case SIGTERM:
         case SIGPIPE:
         case SIGQUIT: {
            struct ArgusWfileStruct *wfile = NULL;

            if (ArgusParser->ArgusWfileList != NULL) {
               struct ArgusListObjectStruct *lobj = NULL;
               int i, count = ArgusParser->ArgusWfileList->count;

               if ((lobj = ArgusParser->ArgusWfileList->start) != NULL) {
                  for (i = 0; i < count; i++) {
                     if ((wfile = (struct ArgusWfileStruct *) lobj) != NULL) {
                        if (wfile->fd != NULL) {
#ifdef ARGUSDEBUG
                           ArgusDebug (2, "RaParseComplete: closing %s\n", wfile->filename);
#endif
                           fflush (wfile->fd);
                           fclose (wfile->fd);
                           wfile->fd = NULL;
                        }
                     }
                     lobj = lobj->nxt;
                  }
               }
            }
            exit (ArgusParser->ArgusExitStatus);
            break;
         }

         case -1:
            exit (ArgusParser->ArgusExitStatus);
            break;
      }
   }
}


void
ArgusMainInit (struct ArgusParserStruct *parser, int argc, char **argv)
{
   extern char *optarg;
   extern int optind, opterr;
   int i, cc, noconf = 0;
   time_t tsec;

   char *envstr = NULL;
   struct stat statbuf;
   struct timezone tz;
   static char path[MAXPATHNAMELEN];

   (void) signal (SIGHUP,  (void (*)(int)) RaParseComplete);
   (void) signal (SIGTERM, (void (*)(int)) RaParseComplete);
   (void) signal (SIGQUIT, (void (*)(int)) RaParseComplete);
   (void) signal (SIGINT,  (void (*)(int)) RaParseComplete);
   (void) signal (SIGPIPE,  SIG_IGN);

   parser->RaPrintOptionStrings[parser->RaPrintOptionIndex++] = "stime";
   parser->RaPrintOptionStrings[parser->RaPrintOptionIndex++] = "flgs";
   parser->RaPrintOptionStrings[parser->RaPrintOptionIndex++] = "proto";
   parser->RaPrintOptionStrings[parser->RaPrintOptionIndex++] = "saddr";
   parser->RaPrintOptionStrings[parser->RaPrintOptionIndex++] = "sport";
   parser->RaPrintOptionStrings[parser->RaPrintOptionIndex++] = "dir";
   parser->RaPrintOptionStrings[parser->RaPrintOptionIndex++] = "daddr";
   parser->RaPrintOptionStrings[parser->RaPrintOptionIndex++] = "dport";
   parser->RaPrintOptionStrings[parser->RaPrintOptionIndex++] = "pkts";
   parser->RaPrintOptionStrings[parser->RaPrintOptionIndex++] = "bytes";
   parser->RaPrintOptionStrings[parser->RaPrintOptionIndex++] = "state";

   ArgusProcessSOptions(parser);

   for (i = 0; i < parser->RaPrintOptionIndex; i++)
      if (parser->RaPrintOptionStrings[i] != NULL)
         parser->RaPrintOptionStrings[i] = NULL;

   parser->RaPrintOptionIndex = 0;

   if (!(strncmp(parser->ArgusProgramName, "radium", 6))) {
      parser->ArgusReliableConnection = 1;
      parser->pflag = 6;
   }

   for (i = 0, cc = 0; i < argc; i++)
      cc += strlen(argv[i]);

   if (cc > 0) {
      int len = cc + (argc + 1); 
                      
      if ((parser->ArgusProgramArgs = (char *) ArgusCalloc (len, sizeof(char))) != NULL) { 
         for (i = 0, *parser->ArgusProgramArgs = '\0'; i < argc; i++) { 
            strncat (parser->ArgusProgramArgs, argv[i], (1024 - strlen(parser->ArgusProgramArgs))); 
            strncat (parser->ArgusProgramArgs, " ", (1024 - strlen(parser->ArgusProgramArgs))); 
         }
      } else             
         ArgusLog (LOG_ERR, "ArgusCalloc(%d, %d) failed %s", len, sizeof(char), strerror(errno));
   } 

   if (gettimeofday(&parser->ArgusRealTime, &tz) < 0)
      ArgusLog (LOG_ERR, "gettimeofday failed %s", strerror(errno));

   parser->ArgusGlobalTime = parser->ArgusRealTime;
   thiszone = tz.tz_minuteswest * -60;

   tsec = parser->ArgusRealTime.tv_sec;
   if ((localtime_r (&tsec, &parser->RaTmStruct))) {
      if (parser->RaTmStruct.tm_isdst)
         thiszone += 3600;

   } else {
      ArgusLog (LOG_ERR, "%s: localtime: error %s", *argv, strerror(errno));
   }

   for (i = 1; i < argc; i++)
      if (strstr (argv[i], "-X"))
         noconf++;

   if (!(noconf)) {
      snprintf (path, MAXPATHNAMELEN - 1, "/etc/ra.conf");

      if (stat (path, &statbuf) == 0)
         ArgusParseResourceFile (parser, path);

      if ((RaHomePath = getenv("ARGUSHOME")) != NULL) {
         snprintf (path, MAXPATHNAMELEN - 1, "%s/ra.conf", RaHomePath);
         if (stat (path, &statbuf) == 0) {
            ArgusParseResourceFile (parser, path);
         }
      }

      if ((envstr = getenv("ARGUSPATH")) != NULL) {
         while ((RaHomePath = strtok(envstr, ":")) != NULL) {
            snprintf (path, MAXPATHNAMELEN - 1, "%s/.rarc", RaHomePath);
            if (stat (path, &statbuf) == 0) {
               ArgusParseResourceFile (parser, path);
               break;
            }
            envstr = NULL;
         }

      } else {
         for (i = 0; i < RAENVITEMS; i++) {
            envstr = RaResourceEnvStr[i];
            if ((RaHomePath = getenv(envstr)) != NULL) {
               snprintf (path, MAXPATHNAMELEN, "%s/.rarc", RaHomePath);
               if (stat (path, &statbuf) == 0) {
                  ArgusParseResourceFile (parser, path);
                  break;
               }
            }
         }
      }
   }

   ArgusParseArgs (parser, argc, argv);
   if (parser->ArgusWfileList == NULL)
      (void) signal (SIGPIPE, (void (*)(int)) RaParseComplete);
}

void
ArgusParseArgs (struct ArgusParserStruct *parser, int argc, char **argv)
{
   extern char *optarg;
   extern int optind, opterr;
   int op, retn = 0, rcmdline = 0, Scmdline = 0;
   char *cmdbuf = NULL, *str = NULL;
   char *getoptStr = NULL;
#if defined(HAVE_GETADDRINFO)
   struct addrinfo *host = NULL;
#else
   struct hostent *host = NULL;
#endif

   char *filter = NULL;
   char *tmparg = NULL;

   opterr = 0;

   if ((argv[optind]) != NULL)
      parser->ArgusProgramOptions = ArgusCopyArgv (&argv[optind]);

   if (!(strncmp(parser->ArgusProgramName, "radium", 6)))
      getoptStr = "a:A:bB:c:C:dD:E:e:f:F:g:GhH:iJlL:m:M:nN:OpP:qr:R:S:s:t:T:u:U:Vvw:XzZ:";
   else
   if (!(strncmp(parser->ArgusProgramName, "rahisto", 6)))
      getoptStr = "a:A:bB:cC:dD:E:e:f:F:g:GhH:iJlLm:M:nN:OpP:qr:R:S:s:t:T:u:U:Vvw:XzZ:";
   else
      getoptStr = "a:AbB:c:C:dD:E:e:f:F:GhHiJlL:m:M:nN:Op:P:qQ:r:R:S:s:t:T:uU:Vvw:XzZ:%";

   while ((op = getopt (argc, argv, getoptStr)) != EOF) {
      switch (op) {
         case '%': ++parser->Pctflag; break;
         case 'a': parser->aflag = atoi (optarg); break;
         case 'A': 
            if (!(strncmp(parser->ArgusProgramName, "radium", 6))) {
               setArgusArchive (parser, optarg);
            } else 
               ++parser->Aflag;
            break;

         case 'b': ++parser->bflag; break;
         case 'B': {
            if (!(strncmp(parser->ArgusProgramName, "radium", 6))) {
               parser->ArgusBindAddr = strdup(optarg);
            } else {
               char *ptr;
               parser->Bflag = strtod(optarg, (char **)&ptr);
               if (ptr == optarg)
                  usage ();
               if (isalpha((int) *ptr)) {
                  switch (*ptr) {
                     case 's': break;
                     case 'm': parser->Bflag *= 60.0; break;
                     case 'h': parser->Bflag *= 60.0 * 60.0; break;
                     case 'd': parser->Bflag *= 60.0 * 60.0 * 24.0; break;
                     case 'w': parser->Bflag *= 60.0 * 60.0 * 24.0 * 7.0; break;
                     case 'M': parser->Bflag *= 60.0 * 60.0 * 24.0 * 7.0 * 4.0; break;
                     case 'y': parser->Bflag *= 60.0 * 60.0 * 24.0 * 7.0 * 4.0 * 12.0; break;
                  }
               }
            }
            break;
         }

         case 'c': 
            if (!(strncmp(parser->ArgusProgramName, "radium", 6))) {
               if ((chroot_dir = strdup(optarg)) == NULL)
                   ArgusLog (LOG_ERR, "strdup %s", strerror(errno));
            } else 
            if (!(strncmp(parser->ArgusProgramName, "ragrep", 6))) {
               parser->cflag++;
            } else {
               parser->cflag++;
               if (optarg[0] == '\\') {
                  switch (optarg[1]) {
                     case 't': parser->RaFieldDelimiter = '\t'; break;
                  }

               } else
                  parser->RaFieldDelimiter = *optarg;
               parser->RaFieldWidth = RA_VARIABLE_WIDTH;
            }
            break;

         case 'C':
            ++parser->Cflag;
            ++parser->Sflag;
            if ((!Scmdline++) && (parser->ArgusRemoteHostList != NULL))
               ArgusDeleteHostList(parser);

            if (!(ArgusAddHostList (parser, optarg, ARGUS_CISCO_DATA_SOURCE, IPPROTO_UDP)))
               ArgusLog(LOG_ERR, "%s: host %s unknown", *argv, optarg);
            break;

         case 'D': parser->debugflag = atoi (optarg); break;
         case 'd': parser->dflag = (parser->dflag) ? 0 : 1; break;

         case 'e': {
            parser->estr = strdup(optarg);

            if (!(strncmp(parser->ArgusProgramName, "radium", 6))) {
               if (optarg && (*optarg == '"')) {
                  int slen = strlen(optarg + 1);
                  slen = (slen > 4) ? 4 : slen;
                  parser->ArgusID = 0;
                  bcopy(optarg + 1, (char *)&parser->ArgusID, slen);
                  parser->ArgusIDType = ARGUS_IDIS_STRING;
               } else
               if (optarg && isalnum((int)*optarg)) {
#if defined(HAVE_GETADDRINFO)
                  struct addrinfo *hp = host;
                  if ((retn = getaddrinfo(optarg, NULL, NULL, &host)) == 0) {
                     unsigned int found = 0, addr;
                     while (host && !found) {
                        switch (host->ai_family) {
                           case AF_INET: {
                              struct sockaddr_in *sa = (struct sockaddr_in *) host->ai_addr;

                              if (sa != NULL) {
                                 bcopy ((char *)&sa->sin_addr, (char *)&addr, 4);
                                 parser->ArgusID = (ntohl(addr));
                                 parser->ArgusIDType = ARGUS_IDIS_IPV4;
                                 found++;
                              } else
                                 ArgusLog (LOG_ERR, "Probe ID %s error %s\n", optarg, strerror(errno));
                              break;
                           }

                           default:
                              hp = hp->ai_next;
                              break;
                        }
                     }
                     freeaddrinfo(hp);
                 } else
#else
                 if ((host = gethostbyname(optarg)) != NULL) {
                    if ((host->h_addrtype == 2) && (host->h_length == 4)) {
                       unsigned int addr;
                       bcopy ((char *) *host->h_addr_list, (char *)&addr, host->h_length);
                       parser->ArgusID = (ntohl(addr));
                    } else
                       ArgusLog (LOG_ERR, "Probe ID %s error %s\n", optarg, strerror(errno));

                    parser->ArgusIDType = ARGUS_IDIS_IPV4;

                 } else
#endif
                     if (optarg && isdigit((int)*optarg)) {
                        parser->ArgusID = atoi (optarg);
                        parser->ArgusIDType = ARGUS_IDIS_INT;
                     } else
                        ArgusLog (LOG_ERR, "Probe ID value %s is not appropriate (%s)\n", optarg, strerror(errno));
               } else
                  ArgusLog (LOG_ERR, "Probe ID value %s is not appropriate\n", optarg);

            } else {
               parser->ArgusGrepSource++;
               parser->ArgusGrepDestination++;
  
               if ((parser->estr[0] == 's') && (parser->estr[1] == ':')) {
                  parser->ArgusGrepDestination = 0;
                  parser->estr = &parser->estr[2];
               }
               if ((parser->estr[0] == 'd') && (parser->estr[1] == ':')) {
                  parser->ArgusGrepSource = 0;
                  parser->estr = &parser->estr[2];
               }
            }

            break;
         }

         case 'E':
            parser->exceptfile = strdup(optarg);
            setArgusWfile (parser, optarg, NULL);
            break;

         case 'f': parser->ArgusFlowModelFile = strdup(optarg); break;
         case 'F': 
            if (!(ArgusParseResourceFile (parser, optarg)))
               ArgusLog(LOG_ERR, "%s: %s", optarg, strerror(errno));
            break;

         case 'g': {
            if (!(strncmp(parser->ArgusProgramName, "radium", 6))) {
               struct group *gr;
               if ((gr = getgrnam(optarg)) == NULL)
                   ArgusLog (LOG_ERR, "unknown group \"%s\"\n", optarg);
               new_gid = gr->gr_gid;
               endgrent();
            } else {
            }
            break;
         }

	 case 'G': parser->Gflag++; break;
	 case 'H': {
            if ((strncmp(parser->ArgusProgramName, "rahisto", 7)))
               parser->Hflag = 1;
            else {
               char str[1024], Hstr[1024], *Hptr = Hstr;
               bzero (str, 1024);
               bzero (Hstr, 1024);
               do {
                  if (*optarg == '"') {
                     if (Hptr[1] != '\0')
                        snprintf (Hstr, 1024, "%s", (&Hptr[1]));

                     while ((Hptr = strchr (Hstr, '"')) == NULL) {
                        if ((optarg = argv[optind]) != NULL) {
                           strncat (Hstr, optarg, (1024 - strlen(Hstr)));
                           optind++;
                        } else
                           break;
                     }
                     optarg = Hstr;
                  }

                  snprintf (&str[strlen(str)], (1024 - strlen(str)), "%s ", optarg);

                  if ((optarg = argv[optind]) != NULL)
                     if (*optarg != '-')
                        optind++;
               } while (optarg && (*optarg != '-'));

               parser->Hstr = strdup(str);
            }
            break;
         }

         case 'i': ++parser->iflag; break;
         case 'I': ++parser->Iflag; break;
         case 'j': ++parser->jflag; break;
         case 'J': ++parser->Jflag; break;
         case 'l': ++parser->lflag; break;
         case 'L': 
            if (!(strncmp(parser->ArgusProgramName, "ragrep", 6)))
               parser->Lflag = 1;
            else {
               parser->Lflag = atoi(optarg);
               switch (parser->Lflag) {
                  case  0: parser->Lflag = -1; break;
                  case -1: parser->Lflag =  0; break;
               }
            }
            break;
         case 'm':
            if (!(strncmp(parser->ArgusProgramName, "ragrep", 6)))
               parser->mflag = atoi (optarg);
            else {
               do {
                  if (!(ArgusAddMaskList (parser, optarg)))
                     ArgusLog(LOG_ERR, "%s: error: mask arg %s", *argv, optarg);
               
                  if ((optarg = argv[optind]) != NULL)
                     if (*optarg != '-')
                        optind++;
               } while (optarg && (*optarg != '-'));
            }
            break;

         case 'M': {
            char Mstr[1024], *Mptr = Mstr, *tzptr;
            bzero (Mstr, 1024);
            
            do {
               int ArgusAddMode = 1;
               if (*optarg == '"') {
                  if (Mptr[1] != '\0')
                     snprintf (Mstr, 1024, "%s", (&Mptr[1]));

                  while ((Mptr = strchr (Mstr, '"')) == NULL) {
                     if ((optarg = argv[optind]) != NULL) {
                        strncat (Mstr, optarg, (1024 - strlen(Mstr)));
                        optind++;
                     } else
                        break;
                  }
                  optarg = Mstr;
               }
               if ((tzptr = strstr (optarg, "printer=")) != NULL) {
                  char *option = &optarg[8];
                  if (!(strncasecmp(option, "hex", 3))) {
                     parser->eflag = ARGUS_HEXDUMP;
                     ArgusAddMode = 0;
                  } else
                  if (!(strncasecmp(option, "ascii", 5))) {
                     parser->eflag = ARGUS_ENCODE_ASCII;
                     ArgusAddMode = 0;
                  } else
                  if (!(strncasecmp(option, "obfuscate", 2))) {
                     parser->eflag = ARGUS_ENCODE_OBFUSCATE;
                     ArgusAddMode = 0;
                  } else
                  if (!(strncasecmp(option, "encode64", 8))) {
                     parser->eflag = ARGUS_ENCODE_64;
                     ArgusAddMode = 0;
                  } else
                  if (!(strncasecmp(option, "encode32", 8))) {
                     parser->eflag = ARGUS_ENCODE_32;
                     ArgusAddMode = 0;
                  }
               } else
               if ((tzptr = strstr (optarg, "label=")) != NULL) {
                  parser->ArgusMatchLabel++;
                  ArgusProcessLabelOptions(parser, &optarg[6]);
               } else
               if ((tzptr = strstr (optarg, "dsrs=")) != NULL) {
                  parser->ArgusStripFields++;
                  ArgusProcessStripOptions(parser, &optarg[5]);
                  ArgusAddMode = 0;
               } else
               if ((tzptr = strstr (optarg, "sql=")) != NULL) {
                  if (parser->ArgusSQLStatement != NULL)
                     free (parser->ArgusSQLStatement);
                  parser->ArgusSQLStatement = strdup(&optarg[4]);
               } else
               if (!(strcmp (optarg, "xml"))) {
                  parser->ArgusPrintXml++;
                  parser->Lflag = 0;
                  ArgusAddMode = 0;
               } else
               if (!(strcmp (optarg, "disa"))) {
                  parser->ArgusDSCodePoints = ARGUS_DISA_DSCODES;
                  RaPrintAlgorithmTable[ARGUSPRINTSRCDSBYTE].length = 8;
                  RaPrintAlgorithmTable[ARGUSPRINTDSTDSBYTE].length = 8;
               } else
               if ((tzptr = strstr (optarg, "TZ="))) {
                  if (parser->RaTimeZone != NULL)
                     free (parser->RaTimeZone);
                  parser->RaTimeZone = strdup(optarg);
#if defined(HAVE_SETENV)
                  setenv("TZ", (parser->RaTimeZone + 3), 1);
#else
                  putenv(parser->RaTimeZone);
#endif
                  tzset();
               } else {
#if defined(ARGUS_SASL)
               if ((tzptr = strstr (optarg, "saslmech="))) {
                  extern char *RaSaslMech;
                  if (RaSaslMech)
                     free (RaSaslMech);
                  RaSaslMech=strdup(&optarg[9]);
               }
#endif /* ARGUS_SASL */
               }

               if (ArgusAddMode) 
                  if (!(ArgusAddModeList (parser, optarg)))
                     ArgusLog(LOG_ERR, "%s: error: arg %s", *argv, optarg);

               if ((optarg = argv[optind]) != NULL)
                  if (*optarg != '-')
                     optind++;
            } while (optarg && (*optarg != '-'));
            break;
         }

         case 'n': {
            if (++parser->nflag > 3) 
               parser->nflag = 0;
            break;
         }
         case 'N': {
            char *ptr = NULL;
            int sNflag = 0, eNflag = 0;
            int output = 0;

            if (optarg[0] == 'o') {
               output = 1;
               optarg++;
            }
            if (optarg[0] == 'i') {
               output = 0;
               optarg++;
            }
            if ((ptr = strchr (optarg, '-')) != NULL) {
               char *eptr = ptr + 1;
               sNflag = strtol(optarg, (char **)&ptr, 10);
               if (ptr == optarg)
                  usage ();
               eNflag = strtol(eptr, (char **)&ptr, 10);
               if (ptr == eptr)
                  usage ();

            } else 
            if ((ptr = strchr (optarg, '+')) != NULL) {
               char *eptr = ptr + 1;
               sNflag = strtol(optarg, (char **)&ptr, 10);
               if (ptr == optarg)
                  usage ();
               eNflag = strtol(eptr, (char **)&ptr, 10);
               if (ptr == eptr)
                  usage ();
               eNflag += sNflag;
            } else {
               sNflag = 0;
               eNflag = strtol(optarg, (char **)&ptr, 10);
               if (ptr == optarg)
                  usage ();
            }
            if (output) {
               parser->sNoflag = sNflag;
               parser->eNoflag = eNflag;
            } else {
               parser->sNflag = sNflag;
               parser->eNflag = eNflag;
            }
            break;
         }
         case 'O': parser->Oflag = 0; break;
         case 'p': 
            if (!(strncmp(parser->ArgusProgramName, "radium", 6))) {
               parser->ArgusReliableConnection = 0;
            } else {
               parser->pflag = atoi (optarg); break;
            }
            break;

         case 'P': 
            if (!(strncmp(parser->ArgusProgramName, "radium", 6))) {
               parser->ArgusPortNum = atoi (optarg);
            } else 
            if (!(strncmp(parser->ArgusProgramName, "rampcd", 6))) {
               parser->ArgusPortNum = atoi (optarg);
            } else {
/*
               parser->Pflag++;
               parser->dbstr = strdup(optarg);
*/
            }
            break;

         case 'q': ++parser->qflag; break;
         case 'Q': {
            parser->Qflag = atoi (optarg);
            break;
         }

/* -r [type:]file[::ostart[:ostop]] */

         case 'r': {
            int type = ARGUS_DATA_SOURCE;

            ++parser->rflag; 
            parser->Sflag = 0;

            if (optarg == NULL)
               optarg = "-";
            else {
               if (!(strncmp ("mysql:", optarg, 6))) {
                  if (parser->readDbstr != NULL)
                     free(parser->readDbstr);
                  parser->readDbstr = strdup(optarg);

               } else {
                  if (!(strncmp ("cisco:", optarg, 6))) {
                     parser->Cflag++;
                     type = ARGUS_CISCO_DATA_SOURCE;
                     optarg += 6;
                  } else
                  if (!(strncmp ("jflow:", optarg, 6))) {
                     parser->Cflag++;
                     type = ARGUS_JFLOW_DATA_SOURCE;
                     optarg += 6;
                  } else
                  if (!(strncmp ("ft:", optarg, 3))) {
                     type = ARGUS_FLOW_TOOLS_SOURCE;
                     optarg += 3;
                  } else
                  if (!(strncmp ("sflow:", optarg, 6))) {
                     type = ARGUS_SFLOW_DATA_SOURCE;
                     optarg += 6;
                  }
                  if ((!rcmdline++) && (parser->ArgusInputFileList != NULL))
                     ArgusDeleteFileList(parser);

                  do {
                     long long ostart = -1, ostop = -1;
                     char *ptr, *eptr;

                     if ((ptr = strstr(optarg, "::")) != NULL) {
                        char *endptr;

                        *ptr++ = '\0';
                        ptr++;

                        if ((eptr = strstr(ptr, ":")) != NULL) {
                           ostart = strtol(ptr, (char **)&endptr, 10);
                           if (endptr == optarg)
                              usage ();
                           ostop = strtol((eptr + 1), (char **)&endptr, 10);
                           if (endptr == optarg)
                              usage ();
                        }
                     }

                     if (!(ArgusAddFileList (parser, optarg, type, ostart, ostop)))
                        ArgusLog(LOG_ERR, "%s: error: file arg %s", *argv, optarg);

                     if ((optarg = argv[optind]) != NULL)
                        if (*optarg != '-')
                           optind++;
                  } while (optarg && (*optarg != '-'));
               }
            }
            break;
         }

         case 'R': {
            parser->Sflag = 0;
            if ((!rcmdline++) && (parser->ArgusInputFileList != NULL))
               ArgusDeleteFileList(parser);

            do {
               RaProcessRecursiveFiles (optarg);
               if ((optarg = argv[optind]) != NULL)
                  if (*optarg != '-')
                     optind++;
            } while (optarg && (*optarg != '-'));

            break;
         }

         case 's': 
            parser->sflag = 1;
            do {
               if (parser->RaPrintOptionIndex < ARGUS_MAX_S_OPTIONS) {
                  char *soptstr = strdup(optarg), *sptr;
                  if ((sptr = soptstr) != NULL) {
                     char *cptr;
                     while ((cptr = strtok(sptr, " ,")) != NULL) {
                        parser->RaPrintOptionStrings[parser->RaPrintOptionIndex++] = strdup(cptr);
                        sptr = NULL;
                     }
                  }
                  free (soptstr);
               } else
                  ArgusLog (LOG_ERR, "usage: number of -s options exceeds %d", ARGUS_MAX_S_OPTIONS);

               if ((optarg = argv[optind]) != NULL)
                  if (*optarg != '-')
                     optind++;
            } while (optarg && (*optarg != '-'));
            break;

         case 'S':
            ++parser->Sflag;
            if ((!Scmdline++) && (parser->ArgusRemoteHostList != NULL))
               ArgusDeleteHostList(parser);

            if (!(ArgusAddHostList (parser, optarg, ARGUS_DATA_SOURCE, IPPROTO_TCP)))
               ArgusLog(LOG_ERR, "%s: host %s unknown", *argv, optarg);
            break;

         case 't': {
            parser->timearg = strdup(optarg);
            if (parser->timearg != NULL) {
               if ((retn = ArgusParseTimeArg (&parser->timearg, argv, optind, &parser->RaTmStruct)) < 0) {
                  usage ();
               } else {
                  parser->tflag++; 
                  optind += retn;
               }
            }
            break;
         }

         case 'T': parser->Tflag = atoi(optarg); break;

         case 'u': { 
            if (!(strncmp(parser->ArgusProgramName, "radium", 6))) {
               char login[256];
               struct passwd *pw;  
               sprintf (login, "%s", optarg);
               if ((pw = getpwnam(login)) == NULL)  
                  ArgusLog (LOG_ERR, "unknown user \"%s\"\n", optarg);
               new_uid = pw->pw_uid;
               endpwent();
            } else {
               parser->uflag++;
            }
            break;
         }

         case 'U':
            if (strstr(parser->ArgusProgramName, "sql") != NULL)
               parser->dbuserstr = strdup (optarg);
            else
               parser->ustr = strdup(optarg);
            break;

         case 'v': parser->vflag++; break;
         case 'V': parser->Vflag++; break;
         case 'w':  
            if ((tmparg = optarg) != NULL) {
               if (!(strncmp ("mysql:", tmparg, 6))) {
                   if (parser->writeDbstr != NULL)
                      free(parser->writeDbstr);
                   parser->writeDbstr = strdup(optarg);

               } else
               if ((*tmparg != '-') || ((*tmparg == '-') &&
                                       (!(strcmp (tmparg, "-"))))) {
                  if (argc == optind)
                     filter = NULL;
                  else {
                     filter = argv[optind];
                     if (*filter == '-') {
                        filter = NULL;
                     } else
                        optind++;
                     }
                  setArgusWfile (parser, tmparg, filter);
                  break;
               }
            }
            break;

         case 'X': RaClearConfiguration (parser); break;
	 case 'z': ++parser->zflag; break;
	 case 'Z': parser->Zflag = *optarg; break;
         case 'h':
            if (!(strncmp(parser->ArgusProgramName, "ragrep", 6))) {
               parser->hflag = 1;
               break;
            }
            
            default:  
               usage ();
            /* NOTREACHED */
      }
   }

   if (rcmdline)
      if (parser->ArgusInputFileList == NULL)
         ArgusLog (LOG_ERR, "no input files");
 
   if ((str = argv[optind]) != NULL) {
      if ((strcmp(str, "-") == 0) || (strcmp(str, "--") == 0))
         optind++;
      cmdbuf = ArgusCopyArgv (&argv[optind]);
   }

   if (cmdbuf) {
      if (parser->ArgusLocalFilter != NULL)
         free(parser->ArgusLocalFilter);

      if (parser->ArgusRemoteFilter != NULL)
         free(parser->ArgusRemoteFilter);

      if ((str = strstr (cmdbuf, "local ")) != NULL) {
         *str = '\0';
         parser->ArgusLocalFilter = strdup(&cmdbuf[strlen("local ")]);
      } else 
      if ((str = strstr (cmdbuf, "display ")) != NULL) {
         *str = '\0';
         parser->ArgusDisplayFilter = strdup(&cmdbuf[strlen("display ")]);
      } else 
      if ((str = strstr (cmdbuf, "remote ")) != NULL) {
         *str = '\0';
         parser->ArgusRemoteFilter = strdup(&cmdbuf[strlen("remote ")]);
      } else
         parser->ArgusRemoteFilter = strdup(cmdbuf);

      free(cmdbuf);
   }

   if (parser->RaPrintOptionIndex > 0)
      ArgusProcessSOptions(parser);
 
   if (parser->ArgusRemoteFilter != NULL)
      if (ArgusFilterCompile (&parser->ArgusFilterCode, parser->ArgusRemoteFilter, parser->Oflag) < 0)
         ArgusLog (LOG_ERR, "%s filter syntax error", parser->ArgusRemoteFilter);

   if (parser->ArgusLocalFilter != NULL)
      if (ArgusFilterCompile (&parser->ArgusFilterCode, parser->ArgusLocalFilter, parser->Oflag) < 0)
         ArgusLog (LOG_ERR, "%s filter syntax error", parser->ArgusLocalFilter);

   if (parser->ArgusDisplayFilter != NULL)
      if (ArgusFilterCompile (&parser->ArgusDisplayCode, parser->ArgusDisplayFilter, parser->Oflag) < 0)
         ArgusLog (LOG_ERR, "%s filter syntax error", parser->ArgusDisplayFilter);

   if (parser->bflag) {
      if ((parser->ArgusLocalFilter != NULL) || (parser->ArgusRemoteFilter != NULL)) {
         nff_dump(&parser->ArgusFilterCode, parser->bflag);
         exit (0);
      }
   }

   if (parser->ArgusGrepSource || parser->ArgusGrepDestination)
      ArgusInitializeGrep(parser);

   if (parser->RaParseDone)
      exit (0);
}

int
ArgusParseResourceFile (struct ArgusParserStruct *parser, char *file)
{
   int retn = 0, i, len, Soption = 0, roption = 0, found = 0, lines = 0;
   char strbuf[MAXSTRLEN], *str = strbuf, *optarg = NULL, *ptr = NULL;
   FILE *fd;

   if (file) {
      if ((fd = fopen (file, "r")) != NULL) {
         retn = 1;
         while ((fgets(strbuf, MAXSTRLEN, fd)) != NULL)  {
            lines++;
            str = strbuf;
            while (*str && isspace((int)*str))
                str++;

            if (*str && (*str != '#') && (*str != '\n') && (*str != '!')) {
               found = 0;
               for (i = 0; i < ARGUS_RCITEMS; i++) {
                  len = strlen(ArgusResourceFileStr[i]);
                  if (!(strncmp (str, ArgusResourceFileStr[i], len))) {

                     optarg = &str[len];

                     while (optarg[strlen(optarg) - 1] == '\n')
                        optarg[strlen(optarg) - 1] = '\0';

                     while (*optarg == '\"')
                        optarg++;

                     while (optarg[strlen(optarg) - 1] == '\"')
                        optarg[strlen(optarg) - 1] = '\0';
                        
                     if (*optarg == '\0')
                        optarg = NULL;

                     if (optarg) {
                        switch (i) {
                           case RA_ARGUS_SERVER:
                              ++parser->Sflag;
                              if (!Soption++ && (parser->ArgusRemoteHostList != NULL))
                                 ArgusDeleteHostList(parser);
                              
                              if (!(ArgusAddHostList (parser, optarg, ARGUS_DATA_SOURCE, IPPROTO_TCP))) {
                                 ArgusLog (LOG_ERR, "host %s unknown\n", optarg);
                              }
                              break;

                           case RA_SOURCE_PORT:
                              parser->ArgusSourcePort = atoi (optarg);
                              break;

                           case RA_CISCONETFLOW_PORT:
                              ++parser->Sflag; ++parser->Cflag;
                              if (!Soption++ && (parser->ArgusRemoteHostList != NULL))
                                 ArgusDeleteHostList(parser);
                              
                              if (!(ArgusAddHostList (parser, optarg, ARGUS_CISCO_DATA_SOURCE, IPPROTO_UDP))) {
                                 ArgusLog (LOG_ERR, "host %s unknown\n", optarg);
                              }
                              break;

                           case RA_ARGUS_SERVERPORT:
                              parser->ArgusPortNum = atoi (optarg);
                              break;

                           case RA_INPUT_FILE: {
                              int type = ARGUS_DATA_SOURCE;
                              if (!(strncmp ("cisco:", optarg, 6))) {
                                 parser->Cflag++;
                                 optarg += 6;
                              } else
                              if (!(strncmp ("jflow:", optarg, 6))) {
                                 type = ARGUS_JFLOW_DATA_SOURCE;
                                 parser->Cflag++;
                                 optarg += 6;
                              } else
                              if (!(strncmp ("sflow:", optarg, 6))) {
                                 type = ARGUS_SFLOW_DATA_SOURCE;
                                 optarg += 6;
                              } else
                              if (parser->Cflag)
                                 type = ARGUS_CISCO_DATA_SOURCE;

                              if ((!roption++) && (parser->ArgusInputFileList != NULL))
                                 ArgusDeleteFileList(parser);

                              if (!(ArgusAddFileList (parser, optarg, type, -1, -1)))
                                 ArgusLog (LOG_ERR, "error: file arg %s\n", optarg);
                              break;
                           }

                           case RA_NO_OUTPUT:
                              if (!(strncasecmp(optarg, "yes", 3)))
                                 parser->qflag++;
                              else
                                 parser->qflag = 0;
                              break;

                           case RA_USER_AUTH:
                              if (parser->ustr != NULL)
                                 free(parser->ustr);
                              parser->ustr = strdup(optarg);
                              break;

                           case RA_AUTH_PASS:
                              if (parser->pstr != NULL)
                                 free(parser->pstr);
                              parser->pstr = strdup(optarg);
                              break;

                           case RA_OUTPUT_FILE: {
                              char *filter = NULL, *fptr;
 
                              if ((filter = strchr (optarg, ' ')) != NULL) {
                                 *filter++ = '\0';

                                 if ((fptr = strchr (filter, '"')) != NULL) {
                                    *fptr++ = '\0';
                                    filter = fptr;
                                 }
                              }

                              setArgusWfile(parser, optarg, filter);
                              break;
                           }

                           case RA_EXCEPTION_OUTPUT_FILE:
                              parser->exceptfile = optarg;
                              setArgusWfile(parser, optarg, NULL);
                              break;

                           case RA_TIMERANGE:
                              parser->timearg = strdup(optarg);
                              if ((ArgusParseTimeArg (&parser->timearg, NULL, 0, &parser->RaTmStruct)) < 0)
                                 usage ();
                              break;

                           case RA_RUN_TIME:
                              parser->Tflag = atoi (optarg);
                              break;

                           case RA_FIELD_DELIMITER:
                              ptr = optarg;
                              if ((ptr = strchr (optarg, '\'')) != NULL) {
                                 ptr++;
                                 if (ptr[0] == '\'')
                                    break;
                              }

                              if (ptr[0] == '\\') {
                                 switch (ptr[1]) {
                                    case  'a': parser->RaFieldDelimiter = '\a'; break;
                                    case  'b': parser->RaFieldDelimiter = '\b'; break;
                                    case  't': parser->RaFieldDelimiter = '\t'; break;
                                    case  'n': parser->RaFieldDelimiter = '\n'; break;
                                    case  'v': parser->RaFieldDelimiter = '\v'; break;
                                    case  'f': parser->RaFieldDelimiter = '\f'; break;
                                    case  'r': parser->RaFieldDelimiter = '\r'; break;
                                    case '\\': parser->RaFieldDelimiter = '\\'; break;
                                 }
                                 if (parser->RaFieldDelimiter != '\0')
                                    break;
                              } else
                                 parser->RaFieldDelimiter = *ptr;

                              parser->RaFieldWidth = RA_VARIABLE_WIDTH;
                              break;

                           case RA_FIELD_QUOTED:
                              if (!(strncasecmp(optarg, "double", 6)))
                                 parser->RaFieldQuoted = RA_DOUBLE_QUOTED;
                              if (!(strncasecmp(optarg, "single", 6)))
                                 parser->RaFieldQuoted = RA_SINGLE_QUOTED;
                              break;

                           case RA_FIELD_WIDTH: {
                              if (!(strncasecmp(optarg, "fixed", 5))) {
                                 parser->RaFieldWidth = RA_FIXED_WIDTH;
                              } else 
                              if (!(strncasecmp(optarg, "variable", 3))) {
                                 parser->RaFieldWidth = RA_VARIABLE_WIDTH;
                              }
                              break;
                           }

                           case RA_SET_PID: {
                              if (!(strncasecmp(optarg, "yes", 3)))
                                 setArguspidflag  (parser, 1);
                              else
                                 setArguspidflag  (parser, 0);
                              break;
                           }

                           case RA_PID_PATH: {
                              parser->ArgusPidPath = strdup(optarg);
                              break;
                           }

                           case RA_TIME_FORMAT: {
                              struct timeval tv, *tvp = &tv;
                              char tbuf[256];

                              if (parser->RaTimeFormat != NULL)
                                 free (parser->RaTimeFormat);

                              parser->RaTimeFormat = strdup(optarg);
                              if (strstr(parser->RaTimeFormat, ".\%f"))
                                 parser->ArgusFractionalDate = 1;

                              gettimeofday(tvp, 0L);
                              bzero(tbuf, sizeof(tbuf));
                              ArgusPrintTime(parser, tbuf, tvp);

                              if ((len = strlen(tbuf)) > 0)
                                 if (len > 128)
                                    ArgusLog (LOG_ERR, "ArgusParseResourceFile: date string %s too long", optarg);

                              RaPrintAlgorithmTable[ARGUSPRINTSTARTDATE].length = len - parser->pflag;
                              RaPrintAlgorithmTable[ARGUSPRINTLASTDATE].length  = len - parser->pflag;
                              break;
                           }

                           case RA_TZ: {
#ifdef ARGUSDEBUG
                              char *tzvalue = getenv("TZ");
#endif
                              char tzbuf[128];

                              snprintf(tzbuf, 128, "TZ=%s", optarg);

                              if (parser->RaTimeZone != NULL)
                                 free(parser->RaTimeZone);
                              parser->RaTimeZone = strdup(tzbuf);
#if defined(HAVE_SETENV)
                              setenv("TZ", (parser->RaTimeZone + 3), 1);
#else
                              putenv(parser->RaTimeZone);
#endif
                              tzset();
#ifdef ARGUSDEBUG
                              ArgusDebug (1, "ArgusParseResourceFile: TZ changed from \"%s\" to \"%s\"", tzvalue, optarg);
#endif
                              break;
                           }
 
                           case RA_USEC_PRECISION:
                              parser->pflag = atoi (optarg);
                              break;

                           case RA_PRINT_SUMMARY:
                              if (!(strncasecmp(optarg, "yes", 3)))
                                 parser->aflag = 1;
                              else
                                 parser->aflag = 0;
                              break;
 
                           case RA_PRINT_NAMES:
                              if (!(strncasecmp(optarg, "none", 4)))
                                 parser->nflag = 3;
                              else if (!(strncasecmp(optarg, "proto", 5)))
                                 parser->nflag = 2;
                              else if (!(strncasecmp(optarg, "port", 5)))
                                 parser->nflag = 1;
                              else if (!(strncasecmp(optarg, "all", 5)))
                                 parser->nflag = 0;
                              break;

                           case RA_PRINT_DOMAINONLY:
                              if (!(strncasecmp(optarg, "yes", 3)))
                                 parser->domainonly = 1;
                              else
                                 parser->domainonly = 0;
                              break;

                           case RA_PRINT_LOCALONLY:
                              if (!(strncasecmp(optarg, "yes", 3)))
                                 ++parser->fflag;
                              else
                                 parser->fflag = 0;
                              break;

                           case RA_CIDR_ADDRESS_FORMAT:
                              if (!(strncasecmp(optarg, "yes", 3)))
                                 parser->cidrflag = RA_ENABLE_CIDR_ADDRESS_FORMAT;
                              else
                              if (!(strncasecmp(optarg, "strict", 3)))
                                 parser->cidrflag = RA_STRICT_CIDR_ADDRESS_FORMAT;
                              else
                                 parser->cidrflag = 0;
                              break;

                           case RA_FLOW_MODEL:
                              parser->ArgusFlowModelFile = strdup(optarg);
                              break;

                           case RA_PRINT_MAN:
                              if (!(strncasecmp(optarg, "yes", 3)))
                                 parser->ArgusPrintMan++;
                              else
                                 parser->ArgusPrintMan = 0;
                              break;

                           case RA_PRINT_EVENT:
                              if (!(strncasecmp(optarg, "yes", 3)))
                                 parser->ArgusPrintEvent++;
                              else
                                 parser->ArgusPrintEvent = 0;
                              break;

                           case RA_PRINT_LABELS:
                              parser->Lflag = atoi(optarg);
                              switch (parser->Lflag) {
                                 case  0: parser->Lflag = -1; break;
                                 case -1: parser->Lflag =  0; break;
                              }
                              break;

                           case RA_PRINT_UNIX_TIME:
                              if (!(strncasecmp(optarg, "yes", 3)))
                                 ++parser->uflag;
                              else
                                 parser->uflag = 0;
                              break;

                           case RA_PRINT_TCPSTATES:
                              if (!(strncasecmp(optarg, "yes", 3)))
                                 parser->zflag++;
                              else
                                 parser->zflag = 0;
                              break;

                           case RA_PRINT_TCPFLAGS:
                                 parser->Zflag = *optarg;
                              break;

                           case RAMON_MODE:
                              parser->Mflag = optarg;
                              break;

                           case RA_NUMBER:
                              parser->sNflag = 0;
                              parser->eNflag = atoi (optarg);
                              break;

                           case RA_DEBUG_LEVEL:
                              parser->debugflag = (atoi(optarg));
                              break;

                           case RA_USERDATA_ENCODE:
                              if (!(strncasecmp(optarg, "ascii", 5)))
                                 parser->eflag = ARGUS_ENCODE_ASCII;
                              else
                              if (!(strncasecmp(optarg, "obfuscate", 3)))
                                 parser->eflag = ARGUS_ENCODE_OBFUSCATE;
                              else
                              if (!(strncasecmp(optarg, "hex", 3)))
                                 parser->eflag = ARGUS_HEXDUMP;
                              else
                              if (!(strncasecmp(optarg, "encode32", 8)))
                                 parser->eflag = ARGUS_ENCODE_32;
                              else
                                 parser->eflag = ARGUS_ENCODE_64;
                              break;

                           case RA_FILTER: {
                              char *ptr, *sptr;

                              if (parser->ArgusRemoteFilter != NULL)
                                 free(parser->ArgusRemoteFilter);

                              if ((parser->ArgusRemoteFilter = calloc (1, MAXSTRLEN)) != NULL) {
                                 ptr = parser->ArgusRemoteFilter;
                                 str = optarg;
                                 while (*str) {
                                    if ((*str == '\\') && (str[1] == '\n')) {
                                       if ((sptr = fgets(str, MAXSTRLEN, fd)) != NULL) {
                                          lines++;
                                          while (*str && (isspace((int)*str) && (str[1] && isspace((int)str[1]))))
                                             str++;
                                       } else
                                          ArgusLog (LOG_ERR, "format error line %d for filter in client configuration", lines);
                                    }
                                    
                                    if ((*str != '\n') && (*str != '"'))
                                       *ptr++ = *str++;
                                    else
                                       str++;
                                 }
                              }
#ifdef ARGUSDEBUG
                              ArgusDebug (1, "ArgusParseResourceFile: ArgusFilter \"%s\" \n", parser->ArgusRemoteFilter);
#endif
                              break;
                           }

                           case RA_FIELD_SPECIFIER: {
                              char *tok = NULL;

                              while ((tok = strtok(optarg, " ,")) != NULL) {
                                 parser->RaPrintOptionStrings[parser->RaPrintOptionIndex++] = strdup(tok);
                                 if (parser->RaPrintOptionIndex > ARGUS_MAX_S_OPTIONS)
                                    ArgusLog (LOG_ERR, "usage: number of -s options exceeds %d", ARGUS_MAX_S_OPTIONS);
                                 optarg = NULL;
                              }

                              break;
                           }

                           case RA_MIN_SSF: {
                              if (*optarg != '\0') {
#ifdef ARGUS_SASL
                                 ArgusMinSsf = atoi(optarg);
#ifdef ARGUSDEBUG
                              ArgusDebug (1, "ArgusParseResourceFile: ArgusMinSsf \"%s\" \n", ArgusMinSsf);
#endif
#endif
                              }
                              break;
                           }

                           case RA_MAX_SSF: {
                              if (*optarg != '\0') {
#ifdef ARGUS_SASL
                                 ArgusMaxSsf = atoi(optarg);
#ifdef ARGUSDEBUG
                                 ArgusDebug (1, "ArgusParseResourceFile: ArgusMaxSsf \"%s\" \n", ArgusMaxSsf);
#endif
#endif
                              }
                              break;
                           }

                           case RA_DELEGATED_IP: {
                              parser->ArgusDelegatedIPFile = strdup(optarg);
                              break;
                           }

                           case RA_RELIABLE_CONNECT: {
                              if (!(strncasecmp(optarg, "yes", 3)))
                                 parser->ArgusReliableConnection = 1;
                              else
                                 parser->ArgusReliableConnection = 0;
                              break;
                           }


                           case RADIUM_DAEMON:
                           case RADIUM_MONITOR_ID:
                           case RADIUM_MAR_STATUS_INTERVAL:
                           case RADIUM_ADJUST_TIME:
                           case RADIUM_ACCESS_PORT:
                              ArgusLog (LOG_ERR, "%s: radium directive in client configuration (use -f)", file);
                              break;

                           case ARGUS_ARCHIVE:
                              parser->ais = strdup(optarg);
                              break;

                           case RA_CONNECT_TIME:
                              parser->ArgusConnectTime = atoi (optarg);
                              break;
                        
                           case RA_UPDATE_INTERVAL: {
                              double value = 0.0, ivalue, fvalue;
                              char *endptr = NULL;

                              value = strtod(optarg, &endptr);

                              if (optarg != endptr) {
                                 fvalue = modf(value, &ivalue);

                                 parser->ArgusUpdateInterval.tv_sec  = (int) ivalue;
                                 parser->ArgusUpdateInterval.tv_usec = (int) (fvalue * 1000000.0);

                              } else
                                 ArgusLog (LOG_ERR, "%s: format error for update interval in client configuration (use float)");
                              break;
                           }

                           case RA_TIMEOUT_INTERVAL: {
                              double value = 0.0, ivalue, fvalue;
                              char *endptr = NULL;

                              value = strtod(optarg, &endptr);

                              if (optarg != endptr) {
                                 fvalue = modf(value, &ivalue);

                                 parser->timeout.tv_sec  = (int) ivalue;
                                 parser->timeout.tv_usec = (int) (fvalue * 1000000.0);

                              } else
                                 ArgusLog (LOG_ERR, "%s: format error for timeout interval in client configuration (use float)");
                              break;
                           }

                           case RA_DATABASE:
                              if (parser->readDbstr != NULL)
                                 free(parser->readDbstr);
                              parser->readDbstr = strdup(optarg);
                              break;

                           case RA_DB_USER:
                              if (parser->dbuserstr != NULL)
                                 free(parser->dbuserstr);
                              parser->dbuserstr = strdup(optarg);
                              break;

                           case RA_DB_PASS:
                              if (parser->dbpassstr != NULL)
                                 free(parser->dbpassstr);
                              parser->dbpassstr = strdup(optarg);
                              break;

                           case RA_DB_HOST:
                              if (parser->dbhoststr != NULL)
                                 free(parser->dbhoststr);
                              parser->dbhoststr = strdup(optarg);
                              break;

                           case RA_DB_PORT:
                              if (parser->dbportstr != NULL)
                                 free(parser->dbportstr);
                              parser->dbportstr = strdup(optarg);
                              break;

                           case RA_AIS_CACHE:
                              if (parser->ais != NULL)
                                 free(parser->ais);
                              parser->ais = strdup(optarg);
                              break;

                           case RA_SORT_ALGORITHMS: {
                              char *tok = NULL;
                              while ((tok = strtok(optarg, " ,")) != NULL) {
                                 parser->RaSortOptionStrings[parser->RaSortOptionIndex++] = strdup(tok);
                                 if (parser->RaSortOptionIndex > ARGUS_MAX_S_OPTIONS)
                                    ArgusLog (LOG_ERR, "usage: number of -s options exceeds %d", ARGUS_MAX_S_OPTIONS);
                                 optarg = NULL;
                              }

                              break;
                           }

                           case MYSQL_DB_ENGINE: {
                              if (parser->MySQLDBEngine != NULL)
                                 free(parser->MySQLDBEngine);
                              parser->MySQLDBEngine = strdup(optarg);
                              break;
                           }

                           case RA_PRINT_ETHERNET_VENDORS: {
                              if (!(strncasecmp(optarg, "yes", 3)))
                                 parser->ArgusPrintEthernetVendors = 1;
                              else
                                 parser->ArgusPrintEthernetVendors = 0;
                              break;
                              break;
                           }

                           case RA_ETHERNET_VENDORS: {
                              if (parser->ArgusEthernetVendorFile != NULL)
                                 free (parser->ArgusEthernetVendorFile);

                              parser->ArgusEthernetVendorFile = strdup(optarg);
                              break;
                           }
                        }
                     }
                     found++;
                     break;
                  }
               }

               if (!found)
                  ArgusLog (LOG_ERR, "%s: syntax error line %d", file, lines);
            }
         }

         if (parser->RaPrintOptionIndex > 0) {
            ArgusProcessSOptions(parser);
            for (i = 0; i < parser->RaPrintOptionIndex; i++) {
               if (parser->RaPrintOptionStrings[i] != NULL) {
                  free(parser->RaPrintOptionStrings[i]);
                  parser->RaPrintOptionStrings[i] = NULL;
               }
            }

            parser->RaPrintOptionIndex = 0;
         }

         fclose(fd);

      } else {
#ifdef ARGUSDEBUG
         ArgusDebug (2, "%s: %s\n", file, strerror(errno));
#endif
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (1, "ArgusParseResourceFile (%s) returning %d\n", file, retn);
#endif

   return (retn);
}




int 
ArgusParserWiresharkManufFile (struct ArgusParserStruct *parser, char *file)
{
   int retn = 0;
   struct stat statbuf;
   FILE *fd = NULL;

   if (stat(file, &statbuf) >= 0) {
      if ((fd = fopen(file, "r")) != NULL) {
         char strbuf[MAXSTRLEN], *str = strbuf, *optarg = NULL;
         int lines = 0;

         retn = 1;

         while ((fgets(strbuf, MAXSTRLEN, fd)) != NULL)  {
            lines++;
            str = strbuf;
            while (*str && isspace((int)*str))
                str++;

#define RA_READING_ETHER_PART	0
#define RA_READING_ETHER_VENDOR	1

            if (*str && (*str != '#') && (*str != '\n') && (*str != '!')) {
               int state = RA_READING_ETHER_PART;
               int addr = 0;
               while ((optarg = strtok(str, " \t\n")) != NULL) {
                  switch (state) {
                     case RA_READING_ETHER_PART: {
                        int i, result;
                        for (i = 0; i < 3; i++) {
                           if (sscanf (optarg, "%x", &result) == 1) {
                              addr |= result << (8 * (2 - i));
                           }
                           optarg +=3;
                        }
                        break;
                     }

                     case RA_READING_ETHER_VENDOR: {
                        struct evendmem *p = NULL;
                        int found = 0;

                        p = &ethervendor[addr % (HASHNAMESIZE-1)];
                        for (; p->nxt; p = p->nxt) {
                           if (p->addr == addr) {
                              found++;
                              break;
                           }
                        }
                        if (!found) {
                           p->addr = addr;
                           p->name = savestr(optarg);
                           p->nxt = (struct evendmem *)calloc(1, sizeof (*p));
                        }
                        break;
                     }
                  }
                  str = NULL;
                  if (++state > RA_READING_ETHER_VENDOR)
                     break;
               }
            }
         }
         fclose(fd);
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (1, "ArgusParserWiresharkManufFile (%p, %s) returning %d\n", parser, file, retn);
#endif

   return (retn);
}



void
RaClearConfiguration (struct ArgusParserStruct *parser)
{
   int i;

   parser->aflag = 0;
   parser->Aflag = 0;
   parser->debugflag = 0;
   parser->bflag = 0;
   parser->Bflag = 0.0;
   parser->cflag = 0;
   parser->Cflag = 0;
   parser->dflag = 0;
   parser->Dflag = 0;
   parser->eflag = 0;
   parser->Eflag = 0;
   parser->estr = NULL;
   parser->fflag = 0;
   parser->Fflag = 0;
   parser->gflag = 0;
   parser->Gflag = 0;
   parser->Hflag = 0;
   parser->Hstr = NULL;
   parser->idflag = 0;
   parser->jflag = 0;
   parser->lflag = 0;
   parser->Lflag = -1;
   parser->mflag = 0;
   parser->Mflag = NULL;
   parser->Netflag = 0;
   parser->nflag = 1;
   parser->sNflag = -1;
   parser->eNflag = -1;
   parser->Normflag = 0;
   parser->notNetflag = 0;
   parser->Oflag = 0;
   parser->pflag = 6;
   parser->ArgusReliableConnection = 1;
   parser->Pflag = 0;
   parser->qflag = 0;
   parser->sflag = 0;
   parser->tflag = 0;
   parser->uflag = 0;
   parser->Wflag = 0;

   parser->Uflag = 6;
   parser->vflag = 0;
   parser->Vflag = 0;
   parser->iflag = 0;

   parser->Iflag = 0;
   parser->Tflag = 0;
   parser->rflag = 0;
   parser->Sflag = 0;
   parser->xflag = 0;
   parser->Xflag = 1;
   parser->XMLflag = 0;

   parser->zflag = 0;
   parser->Zflag = 0;

   parser->ArgusPortNum = 0;

   parser->RaCumulativeMerge = 1;

   if (parser->ArgusPidFile) {
      free (parser->ArgusPidFile);
      parser->ArgusPidFile = NULL;
   }

   if (parser->ArgusPidPath) {
      free (parser->ArgusPidPath);
      parser->ArgusPidPath = NULL;
   }

   if (parser->RaFlowModelFile != NULL) {
      free (parser->RaFlowModelFile);
      parser->RaFlowModelFile = NULL;
   }

   if (parser->ArgusDelegatedIPFile != NULL) {
      free(parser->ArgusDelegatedIPFile);
      parser->ArgusDelegatedIPFile = NULL;
   }

   if (parser->ArgusEthernetVendorFile != NULL) {
      free (parser->ArgusEthernetVendorFile);
      parser->ArgusEthernetVendorFile = NULL;
   }

   if (parser->RaTimeFormat) {
      free (parser->RaTimeFormat);
      parser->RaTimeFormat = strdup("%T.%f");
      parser->ArgusFractionalDate = 1;
   }

   parser->ArgusPrintEthernetVendors = 0;
   parser->RaAllocHashTableHeaders = 0;
   parser->RaAllocArgusRecord      = 0;

   parser->ArgusMinuteUpdate = 1;
   parser->ArgusHourlyUpdate = 1;
   parser->RaHistoMetricSeries = 1;

   parser->RaThisActiveIndex = 0;

   parser->ArgusConnectTime = 0;
   parser->RaThisFlowNum = 0;
   parser->RaThisModelNum = 0;
   parser->RaParseError = 0;

   clearArgusWfile(parser);

   if (parser->readDbstr != NULL)
      free (parser->readDbstr);
   parser->readDbstr = NULL;
 
   if (parser->writeDbstr != NULL)
      free (parser->writeDbstr);
   parser->writeDbstr = NULL;
 
   if (parser->dbuserstr != NULL)
      free (parser->dbuserstr);
   parser->dbuserstr = NULL;
 
   if (parser->dbpassstr != NULL)
      free (parser->dbpassstr);
   parser->dbpassstr = NULL;
 
   if (parser->ustr != NULL)
      free (parser->ustr);
   parser->ustr = NULL;
 
   if (parser->pstr != NULL)
      free (parser->pstr);
   parser->pstr = NULL;

   if (parser->timearg != NULL)
      free(parser->timearg);
   parser->timearg = NULL;

   if (parser->ArgusRemoteFilter != NULL)
      free (parser->ArgusRemoteFilter);
   parser->ArgusRemoteFilter = NULL;

   if (parser->ArgusInputFileList != NULL) {
      ArgusDeleteFileList(parser);
      parser->ArgusInputFileList = NULL;
   }

   if (parser->ArgusRemoteHostList != NULL) {
      ArgusDeleteHostList(parser);
      parser->ArgusRemoteHostList = NULL;
   }

   parser->RaPrintOptionIndex = 0;

   parser->RaPrintOptionStrings[parser->RaPrintOptionIndex++] = "stime";
   parser->RaPrintOptionStrings[parser->RaPrintOptionIndex++] = "flgs";
   parser->RaPrintOptionStrings[parser->RaPrintOptionIndex++] = "proto";
   parser->RaPrintOptionStrings[parser->RaPrintOptionIndex++] = "saddr";
   parser->RaPrintOptionStrings[parser->RaPrintOptionIndex++] = "sport";
   parser->RaPrintOptionStrings[parser->RaPrintOptionIndex++] = "dir";
   parser->RaPrintOptionStrings[parser->RaPrintOptionIndex++] = "daddr";
   parser->RaPrintOptionStrings[parser->RaPrintOptionIndex++] = "dport";
   parser->RaPrintOptionStrings[parser->RaPrintOptionIndex++] = "pkts";
   parser->RaPrintOptionStrings[parser->RaPrintOptionIndex++] = "bytes";
   parser->RaPrintOptionStrings[parser->RaPrintOptionIndex++] = "state";

   ArgusProcessSOptions(parser);

   for (i = 0; i < parser->RaPrintOptionIndex; i++)
      if (parser->RaPrintOptionStrings[i] != NULL)
         parser->RaPrintOptionStrings[i] = NULL;

   parser->RaPrintOptionIndex = 0;

#ifdef ARGUSDEBUG
   ArgusDebug (1, "clearArgusConfiguration () returning\n");
#endif 
}


void RaProcessRecord (struct ArgusParserStruct *, struct ArgusRecordStruct *);
struct ArgusCanonRecord RaThisCanonBuf, *RaThisCanon = &RaThisCanonBuf;

int
RaScheduleRecord (struct ArgusParserStruct *parser, struct ArgusRecordStruct *argus)
{
   int retn = 0;

   RaProcessRecord(parser, argus);

#ifdef ARGUSDEBUG
   ArgusDebug (6, "RaScheduleRecord (%p, %p) scheduled\n", parser, argus);
#endif 
   return (retn);
}

int
ArgusHandleDatum (struct ArgusParserStruct *parser, struct ArgusInput *input, struct ArgusRecord *ptr, struct nff_program *filter)
{
   struct ArgusRecordStruct *argus = NULL;
   int retn = 0;

   if (ptr != NULL) {
      int len = ntohs(ptr->hdr.len) * 4;
      struct nff_insn *fcode = filter->bf_insns;

      if (len < sizeof(input->ArgusOriginalBuffer)) {
         bcopy ((char *)ptr, (char *)input->ArgusOriginal, len);
#ifdef _LITTLE_ENDIAN
         ArgusNtoH(ptr);
#endif
         switch (ptr->hdr.type & 0xF0) {
            case ARGUS_MAR:
               parser->ArgusTotalMarRecords++;
               break;

            case ARGUS_EVENT:
               parser->ArgusTotalEventRecords++;
               break;
      
            case ARGUS_NETFLOW:
            case ARGUS_FAR:
               parser->ArgusTotalFarRecords++;
               break;
         }

         if ((argus = ArgusGenerateRecordStruct (parser, input, (struct ArgusRecord *) ptr)) != NULL) {
            if ((retn = ArgusFilterRecord (fcode, argus)) != 0) {

               if (parser->ArgusGrepSource || parser->ArgusGrepDestination)
                  if (ArgusGrepUserData(parser, argus) == 0)
                     return (argus->hdr.len * 4);

               if (parser->ArgusMatchLabel) {
                  struct ArgusLabelStruct *label;
                  if (((label = (void *)argus->dsrs[ARGUS_LABEL_INDEX]) != NULL)) {
                     if (regexec(&parser->lpreg, label->l_un.label, 0, NULL, 0))
                        return (argus->hdr.len * 4);
                  } else
                     return (argus->hdr.len * 4);
               }

               if (!(((ptr->hdr.type & 0xF0) == ARGUS_MAR) && (argus->status & ARGUS_INIT_MAR)))
                  parser->ArgusTotalRecords++;
               else {
#ifdef _LITTLE_ENDIAN
                  ArgusHtoN(ptr);
#endif
               }

               if (parser->sNflag && (parser->sNflag >= parser->ArgusTotalRecords))
                  return (argus->hdr.len * 4);

               if ((retn = ArgusCheckTime (parser, argus)) != 0) {
                  if (parser->ArgusWfileList != NULL) {
                     if (parser->RaWriteOut) {
                        if (parser->ArgusWfileList != NULL) {
                           struct ArgusWfileStruct *wfile = NULL;
                           struct ArgusListObjectStruct *lobj = NULL;
                           int i, count = parser->ArgusWfileList->count;
    
                           if ((lobj = parser->ArgusWfileList->start) != NULL) {
                              for (i = 0; i < count; i++) {
                                 if ((wfile = (struct ArgusWfileStruct *) lobj->list_obj) != NULL) {
                                    if (wfile->filterstr) {
                                       struct nff_insn *wfcode = wfile->filter.bf_insns;
                                       retn = ArgusFilterRecord (wfcode, argus);
                                    }

                                    if (retn != 0) {
                                       if ((parser->exceptfile == NULL) || strcmp(wfile->filename, parser->exceptfile)) {
                                          if (!(((argus->hdr.type & ARGUS_MAR) && ((argus->hdr.cause & 0xF0) == ARGUS_START)))) {

                                             if (argus->status & RA_MODIFIED) {
                                                struct ArgusRecord *ns = NULL;
                                                char rbuf[ARGUS_MAXRECORDSIZE];

                                                if ((ns = ArgusGenerateRecord (argus, 0L, rbuf)) == NULL)
                                                   ArgusLog(LOG_ERR, "RaProcessSQLEvent: ArgusGenerateRecord error %s", strerror(errno));
#ifdef _LITTLE_ENDIAN
                                                ArgusHtoN(ns);
#endif
                                                if (ArgusWriteNewLogfile (parser, input, wfile, ns))
                                                   ArgusLog (LOG_ERR, "ArgusWriteNewLogfile failed. %s", strerror(errno));
                                             } else
                                                if (ArgusWriteNewLogfile (parser, input, wfile, input->ArgusOriginal))
                                                   ArgusLog (LOG_ERR, "ArgusWriteNewLogfile failed. %s", strerror(errno));
                                          }
                                       }
                                    }
                                 }
                                 lobj = lobj->nxt;
                              }
                           }
                        }
    
                     } else
                        RaScheduleRecord (parser, argus);
                  } else
                     RaScheduleRecord (parser, argus);
               }

            } else {
               if (parser->exceptfile) {
                  struct ArgusWfileStruct *wfile = NULL, *start = NULL;

                  if ((wfile = (struct ArgusWfileStruct *)ArgusFrontList(parser->ArgusWfileList)) != NULL) {
                     start = wfile;
                     do {
                        if (!(strcmp(wfile->filename, parser->exceptfile))) {
                           if (!((ptr->hdr.type & ARGUS_MAR) && ((ptr->hdr.cause & 0xF0) == ARGUS_START)))
                              if (ArgusWriteNewLogfile (parser, input, wfile, input->ArgusOriginal))
                                 ArgusLog (LOG_ERR, "ArgusWriteNewLogfile failed. %s", strerror(errno));
                           break;
                        }
    
                        ArgusPopFrontList(parser->ArgusWfileList, ARGUS_LOCK);
                        ArgusPushBackList(parser->ArgusWfileList, (struct ArgusListRecord *)wfile, ARGUS_LOCK);
                        wfile = (struct ArgusWfileStruct *)ArgusFrontList(parser->ArgusWfileList);
    
                     } while (wfile != start);
                  }
               }
            }
      
            retn = 0;
      
            if (ptr->hdr.type & ARGUS_MAR) {
               switch (ptr->hdr.cause & 0xF0) {
                  case ARGUS_STOP:
                  case ARGUS_SHUTDOWN:
                  case ARGUS_ERROR: {
                     if (ptr->argus_mar.argusid == input->ArgusID) {
#ifdef ARGUSDEBUG
                        ArgusDebug (3, "ArgusHandleDatum (%p, %p) received closing Mar\n", ptr, filter);
#endif
                        if (parser->Sflag)
                           retn = 1;
                     }
                     break;
                  }
               }
            }
         } else
            retn = -1;

         if ((parser->eNflag >= 0) && (parser->ArgusTotalRecords > parser->eNflag)) {
               parser->RaParseDone++;
               parser->eNflag = 0;
               retn = -1;
         }

         if (parser->RaPollMode)
            retn = -1;

         if (retn != -1)
            retn = argus->hdr.len * 4;
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (6, "ArgusHandleDatum (%p, %p) returning %d\n", ptr, filter, retn);
#endif

   return (retn);
}


void
ArgusAdjustGlobalTime (struct ArgusParserStruct *parser, struct timeval *now)
{

   if (parser->Sflag) {
      if (now) {
         parser->ArgusGlobalTime = *now;
         parser->ArgusTimeDelta.tv_sec  = 0;
         parser->ArgusTimeDelta.tv_usec = 0;
      }

   } else {
      if (now != NULL) {
         parser->ArgusTimeDelta.tv_sec  = now->tv_sec  - parser->ArgusGlobalTime.tv_sec;
         parser->ArgusTimeDelta.tv_usec = now->tv_usec - parser->ArgusGlobalTime.tv_usec;

         if (parser->ArgusTimeDelta.tv_usec < 0) {
            parser->ArgusTimeDelta.tv_sec--;
            parser->ArgusTimeDelta.tv_usec += 1000000;
         }

      } else {
         parser->ArgusGlobalTime.tv_sec  = parser->ArgusRealTime.tv_sec  - parser->ArgusTimeDelta.tv_sec;
         parser->ArgusGlobalTime.tv_usec = parser->ArgusRealTime.tv_usec - parser->ArgusTimeDelta.tv_usec;

         if (parser->ArgusGlobalTime.tv_usec < 0) {
            parser->ArgusGlobalTime.tv_sec--;
            parser->ArgusGlobalTime.tv_usec += 1000000;
         } else {
            if (parser->ArgusGlobalTime.tv_usec > 1000000) {
               parser->ArgusGlobalTime.tv_sec++;
                  parser->ArgusGlobalTime.tv_usec -= 1000000;
            }
         }
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusAdjustGlobalTime global %d.%06d", parser->ArgusGlobalTime.tv_sec, 
                                                         parser->ArgusGlobalTime.tv_usec);
#endif
}


void
ArgusZeroRecord (struct ArgusRecordStruct *argus)
{
   ArgusZeroRecordWithFlag (argus, 0 /* no flags */);
}

void
ArgusZeroRecordWithFlag (struct ArgusRecordStruct *argus, int flag)
{
   int i;

   argus->status &= ~ARGUS_RECORD_WRITTEN;

   for (i = 0; i < ARGUSMAXDSRTYPE; i++) {
      switch (i) {
         case ARGUS_FLOW_INDEX:
         case ARGUS_MAC_INDEX: 
         case ARGUS_TRANSPORT_INDEX:
         case ARGUS_ENCAPS_INDEX:
         case ARGUS_LABEL_INDEX:
         case ARGUS_VLAN_INDEX:
         case ARGUS_MPLS_INDEX:
         case ARGUS_IPATTR_INDEX:
         case ARGUS_COCODE_INDEX:
         case ARGUS_ASN_INDEX:
            break;

         case ARGUS_NETWORK_INDEX: {
            struct ArgusNetworkStruct *net = (struct ArgusNetworkStruct *)argus->dsrs[ARGUS_NETWORK_INDEX];
            if (net != NULL) {
               switch (net->hdr.subtype) {
                  case ARGUS_TCP_INIT:
                  case ARGUS_TCP_STATUS:
                  case ARGUS_TCP_PERF: {
                     struct ArgusTCPObject *tcp = (struct ArgusTCPObject *)&net->net_union.tcp;
                     bzero ((char *)tcp, sizeof(*tcp));
                     break;
                  }
                  case ARGUS_RTP_FLOW: {
                     break;
                  }
                  case ARGUS_ESP_DSR: {
                     break;
                  }
               }
            }
            break;
         }

         case ARGUS_TIME_INDEX: {
            struct ArgusTimeObject *dtime = (void *)argus->dsrs[ARGUS_TIME_INDEX];
            if (dtime != NULL) {
               dtime->hdr.subtype &= ~( ARGUS_TIME_SRC_START | ARGUS_TIME_SRC_END |
                                        ARGUS_TIME_DST_START | ARGUS_TIME_DST_END);
               bzero ((char *)&dtime->src, sizeof(*dtime) - sizeof(dtime->hdr));
               dtime->hdr.argus_dsrvl8.qual = 0;
            }
            break;
         }

         case ARGUS_METRIC_INDEX: {
            struct ArgusMetricStruct *metric = (void *)argus->dsrs[ARGUS_METRIC_INDEX];
            if (metric != NULL) 
               bzero ((char *)metric, sizeof(*metric));
            break;
         }

         case ARGUS_PSIZE_INDEX: {
            struct ArgusPacketSizeStruct *psize = (void *)argus->dsrs[ARGUS_PSIZE_INDEX];
            if (psize != NULL)
               bzero ((char *)psize, sizeof(*psize));
            break;
         }

         case ARGUS_JITTER_INDEX: {
            struct ArgusJitterStruct *jitter = (void *)argus->dsrs[ARGUS_JITTER_INDEX];
            if (jitter != NULL)
               bzero ((char *)jitter, sizeof(*jitter));
            break;
         }

         case ARGUS_AGR_INDEX: {
            struct ArgusAgrStruct *agr = (void *)argus->dsrs[ARGUS_AGR_INDEX];
            if (agr != NULL)
               bzero ((char *)agr, sizeof(*agr));
            break;
         }
         case ARGUS_SRCUSERDATA_INDEX:
         case ARGUS_DSTUSERDATA_INDEX: {
            if (flag != 0) {     /* if have flag, preserve user data */
                                 /* but mark as not used, so won't write out*/
               argus->dsrindex &= ~(0x01 << i); 
               break;
            }
         }

         case ARGUS_ICMP_INDEX: {
            struct ArgusIcmpStruct *icmp = (void *)argus->dsrs[ARGUS_ICMP_INDEX];
            if (icmp != NULL)
               bzero ((char *)icmp, sizeof(*icmp));
            break;
         }
         case ARGUS_COR_INDEX: {
            struct ArgusCorStruct *cor = (void *)argus->dsrs[ARGUS_COR_INDEX];
            if (cor != NULL)
               bzero ((char *)cor, sizeof(*cor));
            break;
         }
      }
   }

   bzero(&argus->qhdr.lasttime, sizeof(argus->qhdr.lasttime));

   if (argus->correlates != NULL) {
      int i;
      for (i = 0; i < argus->correlates->count; i++)
         ArgusDeleteRecordStruct(ArgusParser, argus->correlates->array[i]);

      ArgusFree(argus->correlates->array);
      argus->correlates->array = NULL;
      ArgusFree(argus->correlates);
      argus->correlates = NULL;
   }

   argus->rank  = 0;

   argus->sload = 0.0;
   argus->dload = 0.0;
   argus->srate = 0.0;
   argus->drate = 0.0;

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusZeroRecord (%p)", argus);
#endif
}


void ArgusUniDirectionalRecord (struct ArgusRecordStruct *argus);

void
ArgusUniDirectionalRecord (struct ArgusRecordStruct *argus)
{
   int i;

   for (i = 0; i < ARGUSMAXDSRTYPE; i++) {
      switch (i) {
         case ARGUS_FLOW_INDEX:
         case ARGUS_MAC_INDEX: 
         case ARGUS_TRANSPORT_INDEX:
         case ARGUS_ENCAPS_INDEX:
         case ARGUS_LABEL_INDEX:
         case ARGUS_VLAN_INDEX:
         case ARGUS_MPLS_INDEX:
         case ARGUS_IPATTR_INDEX:
         case ARGUS_COCODE_INDEX:
         case ARGUS_ASN_INDEX:
            break;

         case ARGUS_NETWORK_INDEX: {
            struct ArgusNetworkStruct *net = (struct ArgusNetworkStruct *)argus->dsrs[ARGUS_NETWORK_INDEX];
            if (net != NULL) {
               switch (net->hdr.subtype) {
                  case ARGUS_TCP_INIT:
                  case ARGUS_TCP_STATUS:
                  case ARGUS_TCP_PERF: {
                     break;
                  }
                  case ARGUS_RTP_FLOW: {
                     break;
                  }
                  case ARGUS_ESP_DSR: {
                     break;
                  }
               }
            }
            break;
         }

         case ARGUS_TIME_INDEX: {
            struct ArgusTimeObject *dtime = (void *)argus->dsrs[ARGUS_TIME_INDEX];
            if (dtime != NULL) {
               dtime->hdr.subtype &= ~( ARGUS_TIME_DST_START | ARGUS_TIME_DST_END );
               bzero ((char *)&dtime->dst, sizeof(dtime->dst));
            }
            break;
         }

         case ARGUS_METRIC_INDEX: {
            struct ArgusMetricStruct *metric = (void *)argus->dsrs[ARGUS_METRIC_INDEX];
            if (metric != NULL) {
               metric->dst.pkts  = 0;
               metric->dst.bytes = 0;
               if (metric->hdr.subtype == ARGUS_METER_PKTS_BYTES_APP) {
                  metric->dst.appbytes = 0;
               }
            }
            break;
         }

         case ARGUS_PSIZE_INDEX: {
            break;
         }

         case ARGUS_JITTER_INDEX: {
            break;
         }

         case ARGUS_AGR_INDEX: {
            break;
         }

         case ARGUS_SRCUSERDATA_INDEX:
         case ARGUS_DSTUSERDATA_INDEX: {
            break;
         }

         case ARGUS_ICMP_INDEX: {
            break;
         }
         case ARGUS_COR_INDEX: {
            break;
         }
      }
   }

   argus->dload = 0.0;
   argus->drate = 0.0;

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusZeroRecord (%p)", argus);
#endif
}


void
ArgusReverseDataRecord (struct ArgusRecordStruct *argus)
{

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusReverseDataRecord (%p)", argus);
#endif
}

char ArgusReverseLabelBuf[MAXSTRLEN];
void ArgusReverseLabel(struct ArgusLabelStruct *);

void
ArgusReverseLabel(struct ArgusLabelStruct *l)
{
   int len = strlen(l->l_un.label), found = 0, replaced = 0;
   char lbuf[MAXSTRLEN], *ptr, *obj, *sptr, *label;

   if (len >= MAXSTRLEN)
      len = MAXSTRLEN - 1;

   bzero(lbuf, MAXSTRLEN);
   bzero(ArgusReverseLabelBuf, MAXSTRLEN);
   bcopy(l->l_un.label, lbuf, len);
   ptr = lbuf;

   while ((obj = strtok(ptr, ":")) != NULL) {
      int slen;
      if ((sptr = strchr(obj, '=')) != NULL) {
         *sptr = '\0';
         label = sptr + 1;
      } else {
         label = obj;
         obj = NULL;
      }

      slen = strlen(ArgusReverseLabelBuf);
      if (found++) {
         sprintf (&ArgusReverseLabelBuf[slen], ":");
         slen++;
      }

      if (obj != NULL) {
         char replace = '\0';
         
         if (*obj == 's') replace = 'd';
         if (*obj == 'd') replace = 's';

         if (replace != '\0') {
            *obj = replace;
            replaced++;
         }
         snprintf (&ArgusReverseLabelBuf[slen], MAXSTRLEN - slen, "%s=%s", obj, label);

      } else
         snprintf (&ArgusReverseLabelBuf[slen], MAXSTRLEN - slen, "%s", obj);
      ptr = NULL;
   }

   if (replaced)
      bcopy(ArgusReverseLabelBuf, l->l_un.label, len);

   return;
}


void
ArgusReverseRecordWithFlag (struct ArgusRecordStruct *argus, int flags)
{
   struct ArgusRecordHeader *hdr = &argus->hdr;
   struct ArgusDSRHeader *dsr = (struct ArgusDSRHeader *) (hdr + 1);
   int i, x, ArgusDataDataSwitched = 0;

   for (i = 0; i < ARGUSMAXDSRTYPE; i++) {
      if ((dsr = argus->dsrs[i]) != NULL) { 
         switch (i) {
            case ARGUS_FLOW_INDEX: {
               struct ArgusFlow *flow = (struct ArgusFlow *) dsr;
               struct ArgusFlow tbuf, *tflow = &tbuf;
               int tlen = flow->hdr.argus_dsrvl8.len * 4;

               bzero ((char *)tflow, sizeof(*tflow));
               tflow->hdr = flow->hdr;
/*
               if (flags || flow->hdr.subtype & ARGUS_REVERSE)
                  flow->hdr.subtype &= ~ARGUS_REVERSE;
               else
                  flow->hdr.subtype |= ARGUS_REVERSE;

               flow->hdr.argus_dsrvl8.qual &= ~ARGUS_DIRECTION;
*/

               switch (flow->hdr.subtype & 0x3F) {
                  case ARGUS_FLOW_CLASSIC5TUPLE: {
                     bcopy((char *)flow, (char *)tflow, tlen);
                     switch ((flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                        case ARGUS_TYPE_IPV4:
                           tflow->ip_flow.ip_dst = flow->ip_flow.ip_src;
                           tflow->ip_flow.ip_src = flow->ip_flow.ip_dst;
                           switch ((tflow->ip_flow.ip_p = flow->ip_flow.ip_p)) {
                              case IPPROTO_TCP:
                              case IPPROTO_UDP:
                                 tflow->ip_flow.sport = flow->ip_flow.dport;
                                 tflow->ip_flow.dport = flow->ip_flow.sport;
                                 break;
                           }
                           break; 

                        case ARGUS_TYPE_IPV6: {
                           for (x = 0; x < 4; x++) {
                              tflow->ipv6_flow.ip_src[x] = flow->ipv6_flow.ip_dst[x];
                              tflow->ipv6_flow.ip_dst[x] = flow->ipv6_flow.ip_src[x];
                           }
                           switch ((tflow->ipv6_flow.ip_p = flow->ipv6_flow.ip_p)) {
                              case IPPROTO_TCP:
                              case IPPROTO_UDP:
                                 flow->ipv6_flow.sport = flow->ipv6_flow.dport;
                                 flow->ipv6_flow.dport = flow->ipv6_flow.sport;
                                 break;
                           }
                           break; 
                        }

                        case ARGUS_TYPE_RARP:
                           bcopy(&flow->arp_flow.arp_spa,&tflow->arp_flow.arp_tpa, 6);
                           bcopy(&flow->arp_flow.arp_tpa,&tflow->arp_flow.arp_spa, 6);
                           break;

                        case ARGUS_TYPE_ARP:
                           bcopy ((char *)flow, (char *)tflow, sizeof(*tflow));
                           tflow->arp_flow.arp_tpa = flow->arp_flow.arp_spa;
                           tflow->arp_flow.arp_spa = flow->arp_flow.arp_tpa;
                           break;

                        case ARGUS_TYPE_ISIS:
                        default: {
                           bcopy ((char *)flow, (char *)tflow, sizeof(*tflow));
                           break;
                        }

                        case ARGUS_TYPE_ETHER: {
                           bcopy ((char *)&flow->mac_flow.mac_union.ether.ehdr.ether_shost, (char *)&tflow->mac_flow.mac_union.ether.ehdr.ether_dhost,
                                  sizeof(tflow->mac_flow.mac_union.ether.ehdr.ether_shost));
                           bcopy ((char *)&flow->mac_flow.mac_union.ether.ehdr.ether_dhost, (char *)&tflow->mac_flow.mac_union.ether.ehdr.ether_shost,
                                  sizeof(tflow->mac_flow.mac_union.ether.ehdr.ether_shost));
                           tflow->mac_flow.mac_union.ether.ehdr.ether_type = flow->mac_flow.mac_union.ether.ehdr.ether_type;
                           tflow->mac_flow.mac_union.ether.dsap = flow->mac_flow.mac_union.ether.ssap;
                           tflow->mac_flow.mac_union.ether.ssap = flow->mac_flow.mac_union.ether.dsap;
                           break;
                        }
                     }
                     break; 
                  }
                  case ARGUS_FLOW_ARP: {
                     switch ((flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                        case ARGUS_TYPE_RARP:
                           bcopy(&flow->arp_flow.arp_spa,&tflow->arp_flow.arp_tpa, 6);
                           bcopy(&flow->arp_flow.arp_tpa,&tflow->arp_flow.arp_spa, 6);
                           break;

                        case ARGUS_TYPE_ARP:
                           bcopy ((char *)flow, (char *)tflow, tlen);
                           tflow->arp_flow.arp_tpa = flow->arp_flow.arp_spa;
                           tflow->arp_flow.arp_spa = flow->arp_flow.arp_tpa;
                           break;
                     }
                  }
               }
               bcopy ((char *)tflow, (char *)flow, tlen);
               break;
            }

            case ARGUS_IPATTR_INDEX: {
               struct ArgusIPAttrStruct *attr = (struct ArgusIPAttrStruct *)argus->dsrs[ARGUS_IPATTR_INDEX];
               struct ArgusIPAttrObject objbuf;
               char qual;

               if (attr != NULL) { 
                  if ((qual = attr->hdr.argus_dsrvl8.qual) != 0) {
                     attr->hdr.argus_dsrvl8.qual &= ~((ARGUS_IPATTR_SRC | ARGUS_IPATTR_SRC_OPTIONS ) |
                                                      (ARGUS_IPATTR_DST | ARGUS_IPATTR_DST_OPTIONS ));

                     if (qual & ARGUS_IPATTR_SRC) attr->hdr.argus_dsrvl8.qual |= ARGUS_IPATTR_DST;
                     if (qual & ARGUS_IPATTR_DST) attr->hdr.argus_dsrvl8.qual |= ARGUS_IPATTR_SRC;
                     if (qual & ARGUS_IPATTR_SRC_OPTIONS) attr->hdr.argus_dsrvl8.qual |= ARGUS_IPATTR_DST_OPTIONS;
                     if (qual & ARGUS_IPATTR_DST_OPTIONS) attr->hdr.argus_dsrvl8.qual |= ARGUS_IPATTR_SRC_OPTIONS;

                     bcopy (&attr->src, &objbuf, sizeof(objbuf));
                     bcopy (&attr->dst, &attr->src, sizeof(attr->dst));
                     bcopy (&objbuf, &attr->dst, sizeof(objbuf));
                  }
               }

               break;
            }

            case ARGUS_TRANSPORT_INDEX:
            case ARGUS_TIME_ADJ_INDEX: 
            case ARGUS_AGR_INDEX:
               break;

            case ARGUS_TIME_INDEX:  {
               struct ArgusTimeObject *time = (struct ArgusTimeObject *) dsr;
               unsigned char subtype = time->hdr.subtype;

               if (subtype & ARGUS_TIME_MASK) {
                  struct ArgusTimeStruct srcbuf;

                  subtype &= ~ARGUS_TIME_MASK;
                  bcopy (&time->src, &srcbuf, sizeof(srcbuf));
                  bcopy (&time->dst, &time->src, sizeof(srcbuf));
                  bcopy (&srcbuf, &time->dst, sizeof(srcbuf));

                  if (time->hdr.subtype & ARGUS_TIME_SRC_START) subtype |= ARGUS_TIME_DST_START;
                  if (time->hdr.subtype & ARGUS_TIME_DST_START) subtype |= ARGUS_TIME_SRC_START;
                  if (time->hdr.subtype & ARGUS_TIME_SRC_END)   subtype |= ARGUS_TIME_DST_END;
                  if (time->hdr.subtype & ARGUS_TIME_DST_END)   subtype |= ARGUS_TIME_SRC_END;

                  time->hdr.subtype = subtype;
               }
              
               break;
            }

            case ARGUS_LABEL_INDEX: {
               struct ArgusLabelStruct *label;
               if (((label = (void *)argus->dsrs[ARGUS_LABEL_INDEX]) != NULL))
                  ArgusReverseLabel(label);
               break;
            }

            case ARGUS_ASN_INDEX: {
               struct ArgusAsnStruct *asn = (struct ArgusAsnStruct *)dsr;
               unsigned int src_as = asn->src_as;
               asn->src_as = asn->dst_as;
               asn->dst_as = src_as;
               break;
            }

            case ARGUS_NETWORK_INDEX: {
               struct ArgusNetworkStruct *net = (struct ArgusNetworkStruct *)dsr;
               switch (net->hdr.subtype) {
                  case ARGUS_TCP_INIT:
                  case ARGUS_TCP_STATUS:
                  case ARGUS_TCP_PERF: {
                     struct ArgusTCPObject *tcp = (struct ArgusTCPObject *)&net->net_union.tcp;
                     struct ArgusTCPObjectMetrics sbuf;
                     bcopy ((char *)&tcp->src, (char *)&sbuf, sizeof (sbuf));
                     bcopy ((char *)&tcp->dst, (char *)&tcp->src, sizeof (sbuf));
                     bcopy ((char *)&sbuf, (char *)&tcp->dst, sizeof (sbuf));
                     break;
                  }
               }
               break;
            }

            case ARGUS_METRIC_INDEX: {
               struct ArgusMetricStruct *metric = (struct ArgusMetricStruct *)argus->dsrs[ARGUS_METRIC_INDEX];
               struct ArgusMetricStruct mbuf, *tmetric = &mbuf;

               if ((metric != NULL) && (!flags)) {
                  bzero ((char *)tmetric, sizeof(*tmetric));

                  tmetric->hdr       = metric->hdr;
                  tmetric->src.pkts  = metric->dst.pkts;
                  tmetric->src.bytes = metric->dst.bytes;
                  tmetric->dst.pkts  = metric->src.pkts;
                  tmetric->dst.bytes = metric->src.bytes;
                  if (metric->hdr.subtype == ARGUS_METER_PKTS_BYTES_APP) {
                     tmetric->src.appbytes = metric->dst.appbytes;
                     tmetric->dst.appbytes = metric->src.appbytes;
                  }

                  bcopy((char *)tmetric,  (char *) metric, sizeof(*tmetric));
               }
               break;
            }

            case ARGUS_PSIZE_INDEX: {
               struct ArgusPacketSizeStruct *psize = (void *)argus->dsrs[ARGUS_PSIZE_INDEX];
               struct ArgusPacketSizeStruct pbuf, *pbptr = &pbuf;
               pbptr->hdr = psize->hdr;

               switch (psize->hdr.argus_dsrvl8.qual & 0x0F) {
                  case ARGUS_SRCDST_SHORT:
                     break;
                  case ARGUS_SRC_SHORT:
                     pbptr->hdr.argus_dsrvl8.qual &= ~ARGUS_SRC_SHORT;
                     pbptr->hdr.argus_dsrvl8.qual |=  ARGUS_DST_SHORT;
                     break;
                  case ARGUS_DST_SHORT:
                     pbptr->hdr.argus_dsrvl8.qual &= ~ARGUS_DST_SHORT;
                     pbptr->hdr.argus_dsrvl8.qual |=  ARGUS_SRC_SHORT;
                     break;
               }

               bcopy(&psize->src, &pbptr->dst, sizeof(psize->src));
               bcopy(&psize->dst, &pbptr->src, sizeof(psize->dst));

               bcopy(pbptr, psize, sizeof(*psize));
               break;
            }

            case ARGUS_MAC_INDEX: {
               struct ArgusMacStruct *mac = (struct ArgusMacStruct *) argus->dsrs[ARGUS_MAC_INDEX];
               struct ArgusMacStruct mbuf, *tmac = &mbuf;
               tmac->hdr   = mac->hdr;

               switch (mac->hdr.subtype & 0x3F) {
                  default:
                  case ARGUS_TYPE_ETHER:
                     bcopy ((char *)&mac->mac.mac_union.ether.ehdr.ether_shost, (char *)&tmac->mac.mac_union.ether.ehdr.ether_dhost, 6);
                     bcopy ((char *)&mac->mac.mac_union.ether.ehdr.ether_dhost, (char *)&tmac->mac.mac_union.ether.ehdr.ether_shost, 6);

                     tmac->mac.mac_union.ether.ehdr.ether_type = mac->mac.mac_union.ether.ehdr.ether_type;
                     tmac->mac.mac_union.ether.dsap            = mac->mac.mac_union.ether.ssap;
                     tmac->mac.mac_union.ether.ssap            = mac->mac.mac_union.ether.dsap;
                     break;
               }

               bcopy ((char *) tmac, (char *) mac, sizeof(*mac));
               break;
            }

            case ARGUS_VLAN_INDEX: {
               struct ArgusVlanStruct *vlan = (struct ArgusVlanStruct *)argus->dsrs[ARGUS_VLAN_INDEX];
               unsigned short tsid = vlan->sid;
               unsigned char qual = vlan->hdr.argus_dsrvl8.qual & ~(ARGUS_SRC_VLAN | ARGUS_DST_VLAN);

                vlan->sid = vlan->did;
                vlan->did = tsid;

               if (vlan->hdr.argus_dsrvl8.qual & ARGUS_SRC_VLAN)
                  qual |= ARGUS_DST_VLAN;
               
               if (vlan->hdr.argus_dsrvl8.qual & ARGUS_DST_VLAN)
                  qual |= ARGUS_SRC_VLAN;

               vlan->hdr.argus_dsrvl8.qual = qual;
               break;
            }

            case ARGUS_MPLS_INDEX: {
               struct ArgusMplsStruct *mpls = (struct ArgusMplsStruct *)argus->dsrs[ARGUS_MPLS_INDEX];
               if (mpls != NULL) {
                  struct ArgusMplsStruct mbuf, *tmpls = &mbuf;
                  tmpls->hdr = mpls->hdr;
                  tmpls->hdr.subtype = 0;
                  if (mpls->hdr.subtype & ARGUS_MPLS_SRC_LABEL) 
                     tmpls->hdr.subtype |= ARGUS_MPLS_DST_LABEL;
                  if (mpls->hdr.subtype & ARGUS_MPLS_DST_LABEL) 
                     tmpls->hdr.subtype |= ARGUS_MPLS_SRC_LABEL;

                  tmpls->dlabel = mpls->slabel;
                  tmpls->slabel = mpls->dlabel;
                  bcopy((char *)tmpls,  (char *) mpls, sizeof(*tmpls));
               }
               break;
            }

            case ARGUS_JITTER_INDEX: {
               struct ArgusJitterStruct *jitter = (void *)argus->dsrs[ARGUS_JITTER_INDEX];

               if (jitter != NULL) {
                  struct ArgusJitterStruct tjitbuf, *tjit = &tjitbuf;
                  bzero((char *)tjit, sizeof(*tjit));

                  bcopy((char *)&jitter->hdr, (char *)&tjit->hdr, sizeof(tjit->hdr));
                  tjit->hdr.argus_dsrvl8.qual &= ~(ARGUS_SRC_ACTIVE_JITTER | ARGUS_DST_ACTIVE_JITTER |
                                                   ARGUS_SRC_IDLE_JITTER   | ARGUS_DST_IDLE_JITTER );

                  if (jitter->hdr.argus_dsrvl8.qual & ARGUS_SRC_ACTIVE_JITTER) {
                     tjit->hdr.argus_dsrvl8.qual |= ARGUS_DST_ACTIVE_JITTER;
                     bcopy((char *)&jitter->src.act, (char *)&tjit->dst.act, sizeof(tjit->dst.act));
                  }
                  if (jitter->hdr.argus_dsrvl8.qual & ARGUS_SRC_IDLE_JITTER) {
                     tjit->hdr.argus_dsrvl8.qual |= ARGUS_DST_IDLE_JITTER;
                     bcopy((char *)&jitter->src.idle, (char *)&tjit->dst.idle, sizeof(tjit->dst.idle));
                  }
                  if (jitter->hdr.argus_dsrvl8.qual & ARGUS_DST_ACTIVE_JITTER) {
                     tjit->hdr.argus_dsrvl8.qual |= ARGUS_SRC_ACTIVE_JITTER;
                     bcopy((char *)&jitter->dst.act, (char *)&tjit->src.act, sizeof(tjit->src.act));
                  }
                  if (jitter->hdr.argus_dsrvl8.qual & ARGUS_DST_IDLE_JITTER) {
                     tjit->hdr.argus_dsrvl8.qual |= ARGUS_SRC_IDLE_JITTER;
                     bcopy((char *)&jitter->dst.idle, (char *)&tjit->src.idle, sizeof(tjit->src.idle));
                  }

                  bcopy((char *)tjit, (char *)jitter, sizeof(*jitter));
               }
               break;
            }

            case ARGUS_COCODE_INDEX: {
               struct ArgusCountryCodeStruct *cocode = (void *)argus->dsrs[ARGUS_COCODE_INDEX];
               char ccbuf[4];

               if (cocode != NULL) {
                  bzero(ccbuf, sizeof(ccbuf));
                  bcopy((char *)&cocode->src, ccbuf, 2);
                  bcopy((char *)&cocode->dst, (char *)&cocode->src, 2);
                  bcopy((char *)ccbuf, (char *)&cocode->dst, 2);
               }
               break;
            }

            case ARGUS_SRCUSERDATA_INDEX:
            case ARGUS_DSTUSERDATA_INDEX: {
               if (!ArgusDataDataSwitched && !flags) {
                  struct ArgusDataStruct *srcuser = (struct ArgusDataStruct *)argus->dsrs[ARGUS_SRCUSERDATA_INDEX];
                  struct ArgusDataStruct *dstuser = (struct ArgusDataStruct *)argus->dsrs[ARGUS_DSTUSERDATA_INDEX];
                  if (srcuser) {
                     argus->dsrs[ARGUS_DSTUSERDATA_INDEX] = (struct ArgusDSRHeader *) srcuser;
                     srcuser->hdr.subtype &= ~ARGUS_SRC_DATA;
                     srcuser->hdr.subtype |= ARGUS_DST_DATA;
                  } else
                     argus->dsrs[ARGUS_DSTUSERDATA_INDEX] = (struct ArgusDSRHeader *) NULL;
                  if (dstuser) {
                     argus->dsrs[ARGUS_SRCUSERDATA_INDEX] = (struct ArgusDSRHeader *) dstuser;
                     dstuser->hdr.subtype &= ~ARGUS_DST_DATA;
                     dstuser->hdr.subtype |= ARGUS_SRC_DATA;
                  } else
                     argus->dsrs[ARGUS_SRCUSERDATA_INDEX] = (struct ArgusDSRHeader *) NULL;

                  ArgusDataDataSwitched++;
               }
               break;
            }
         }
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusReverseRecord (%p, 0x%x)", argus, flags);
#endif
}

void
ArgusReverseRecord (struct ArgusRecordStruct *argus)
{
   ArgusReverseRecordWithFlag (argus,0);
}

u_int
ArgusIndexRecord (struct ArgusRecordStruct *argus)
{
   u_int retn = 0;

   retn = argus->dsrindex;
#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusIndexRecord (%p) returns 0x%x", argus, retn);
#endif

   return (retn);
}



int
ArgusConvertInitialWriteStruct (struct WriteStruct *ws, struct ArgusRecordStruct *argus)
{
   int retn = 0;

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusConvertInitialWriteStruct (%p, %p) returning %d", ws, argus, retn);
#endif

   return (retn);
}

#include <argus/cons_def.h>

#if defined(__OpenBSD__) || defined(__APPLE__) || defined(linux)
#include <netinet/ip_icmp.h>
#include <netinet/icmp6.h>
#endif

extern int ArgusTotalBytes;
extern int ArgusTotalCount;

int
ArgusConvertWriteStruct (struct WriteStruct *ws, struct ArgusRecordStruct *argus)
{
   int retn = 0;

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusConvertWriteStruct (%p, %p) returning %d", ws, argus, retn);
#endif

   return (retn);
}


void ArgusPrintXmlSortAlgorithms(struct ArgusParserStruct *parser);

void
ArgusPrintXmlSortAlgorithms(struct ArgusParserStruct *parser)
{
   int i, dtime = 0, agg = 0, flow = 0, attr = 0,  metrics = 0, trans = 0;
   int mac = 0, encaps = 0, label = 0, state = 0, igmp = 0, psize = 0;
   int vlan = 0, mpls = 0, cor = 0, user = 0, tcp = 0, sfile = 0;

   for (i = 0; i < MAX_PRINT_ALG_TYPES; i++) {
      if (parser->RaPrintAlgorithmList[i] != NULL) {
         switch (parser->RaPrintAlgorithmList[i]->value) {
            case ARGUSPRINTSTARTDATE:			dtime++; break;
            case ARGUSPRINTLASTDATE:			dtime++; break;
            case ARGUSPRINTTRANSACTIONS:		agg++; break;
            case ARGUSPRINTDURATION:			dtime++; break;
            case ARGUSPRINTMEAN:			agg++; break;
            case ARGUSPRINTMIN:				agg++; break;
            case ARGUSPRINTMAX:				agg++; break;
            case ARGUSPRINTSRCADDR:			flow++; break;
            case ARGUSPRINTDSTADDR:			flow++; break;
            case ARGUSPRINTPROTO:			flow++; break;
            case ARGUSPRINTSRCPORT:			flow++; break;
            case ARGUSPRINTDSTPORT:			flow++; break;
            case ARGUSPRINTSRCTOS:			attr++; break;
            case ARGUSPRINTDSTTOS:			attr++; break;
            case ARGUSPRINTSRCDSBYTE:			attr++; break;
            case ARGUSPRINTDSTDSBYTE:			attr++; break;
            case ARGUSPRINTSRCTTL:			attr++; break;
            case ARGUSPRINTDSTTTL:			attr++; break;
            case ARGUSPRINTBYTES:			metrics++; break;
            case ARGUSPRINTSRCBYTES:			metrics++; break;
            case ARGUSPRINTDSTBYTES:			metrics++; break;
            case ARGUSPRINTAPPBYTES:			metrics++; break;
            case ARGUSPRINTSRCAPPBYTES:			metrics++; break;
            case ARGUSPRINTDSTAPPBYTES:			metrics++; break;
            case ARGUSPRINTPACKETS:			metrics++; break;
            case ARGUSPRINTSRCPACKETS:			metrics++; break;
            case ARGUSPRINTDSTPACKETS:			metrics++; break;
            case ARGUSPRINTLOAD:			metrics++; break;
            case ARGUSPRINTSRCLOAD:			metrics++; break;
            case ARGUSPRINTDSTLOAD:			metrics++; break;
            case ARGUSPRINTLOSS:			metrics++; break;
            case ARGUSPRINTSRCLOSS:			metrics++; break;
            case ARGUSPRINTDSTLOSS:			metrics++; break;
            case ARGUSPRINTPERCENTLOSS:			metrics++; break;
            case ARGUSPRINTSRCPERCENTLOSS:		metrics++; break;
            case ARGUSPRINTDSTPERCENTLOSS:		metrics++; break;
            case ARGUSPRINTRATE:			metrics++; break;
            case ARGUSPRINTSRCRATE:			metrics++; break;
            case ARGUSPRINTDSTRATE:			metrics++; break;
            case ARGUSPRINTSOURCEID:			trans++; break;
            case ARGUSPRINTFLAGS:			break;
            case ARGUSPRINTSRCMACADDRESS:		mac++; break;
            case ARGUSPRINTDSTMACADDRESS:		mac++; break;
            case ARGUSPRINTDIR:				flow++; break;
            case ARGUSPRINTSRCINTPKT:			metrics++; break;
            case ARGUSPRINTDSTINTPKT:			metrics++; break;
            case ARGUSPRINTACTSRCINTPKT:		metrics++; break;
            case ARGUSPRINTACTDSTINTPKT:		metrics++; break;
            case ARGUSPRINTIDLESRCINTPKT:		metrics++; break;
            case ARGUSPRINTIDLEDSTINTPKT:		metrics++; break;
            case ARGUSPRINTSRCINTPKTMAX:		metrics++; break;
            case ARGUSPRINTSRCINTPKTMIN:		metrics++; break;
            case ARGUSPRINTDSTINTPKTMAX:		metrics++; break;
            case ARGUSPRINTDSTINTPKTMIN:		metrics++; break;
            case ARGUSPRINTACTSRCINTPKTMAX:		metrics++; break;
            case ARGUSPRINTACTSRCINTPKTMIN:		metrics++; break;
            case ARGUSPRINTACTDSTINTPKTMAX:		metrics++; break;
            case ARGUSPRINTACTDSTINTPKTMIN:		metrics++; break;
            case ARGUSPRINTIDLESRCINTPKTMAX:		metrics++; break;
            case ARGUSPRINTIDLESRCINTPKTMIN:		metrics++; break;
            case ARGUSPRINTIDLEDSTINTPKTMAX:		metrics++; break;
            case ARGUSPRINTIDLEDSTINTPKTMIN:		metrics++; break;
            case ARGUSPRINTSPACER:			break;
            case ARGUSPRINTSRCJITTER:			metrics++; break;
            case ARGUSPRINTDSTJITTER:			metrics++; break;
            case ARGUSPRINTACTSRCJITTER:		metrics++; break;
            case ARGUSPRINTACTDSTJITTER:		metrics++; break;
            case ARGUSPRINTIDLESRCJITTER:		metrics++; break;
            case ARGUSPRINTIDLEDSTJITTER:		metrics++; break;
            case ARGUSPRINTSTATE:			state++; break;
            case ARGUSPRINTDELTADURATION:		cor++; break;
            case ARGUSPRINTDELTASTARTTIME:		cor++; break;
            case ARGUSPRINTDELTALASTTIME:		cor++; break;
            case ARGUSPRINTDELTASRCPKTS:		cor++; break;
            case ARGUSPRINTDELTADSTPKTS:		cor++; break;
            case ARGUSPRINTDELTASRCBYTES:		cor++; break;
            case ARGUSPRINTDELTADSTBYTES:		cor++; break;
            case ARGUSPRINTPERCENTDELTASRCPKTS:		cor++; break;
            case ARGUSPRINTPERCENTDELTADSTPKTS:		cor++; break;
            case ARGUSPRINTPERCENTDELTASRCBYTES:	cor++; break;
            case ARGUSPRINTPERCENTDELTADSTBYTES:	cor++; break;
            case ARGUSPRINTSRCUSERDATA:			user++; break;
            case ARGUSPRINTDSTUSERDATA:			user++; break;
            case ARGUSPRINTTCPEXTENSIONS:		tcp++; break;
            case ARGUSPRINTSRCWINDOW:			tcp++; break;
            case ARGUSPRINTDSTWINDOW:			tcp++; break;
            case ARGUSPRINTJOINDELAY:			igmp++; break;
            case ARGUSPRINTLEAVEDELAY:			igmp++; break;
            case ARGUSPRINTSEQUENCENUMBER:		trans++; break;
            case ARGUSPRINTBINS:			agg++; break;
            case ARGUSPRINTBINNUMBER:			agg++; break;
            case ARGUSPRINTSRCMPLS:			mpls++; break;
            case ARGUSPRINTDSTMPLS:			mpls++; break;
            case ARGUSPRINTSRCVLAN:			vlan++; break;
            case ARGUSPRINTDSTVLAN:			vlan++; break;
            case ARGUSPRINTSRCVID:			vlan++; break;
            case ARGUSPRINTDSTVID:			vlan++; break;
            case ARGUSPRINTSRCVPRI:			vlan++; break;
            case ARGUSPRINTDSTVPRI:			vlan++; break;
            case ARGUSPRINTSRCIPID:			attr++; break;
            case ARGUSPRINTDSTIPID:			attr++; break;
            case ARGUSPRINTSTARTRANGE:			dtime++; break;
            case ARGUSPRINTENDRANGE:			dtime++; break;
            case ARGUSPRINTTCPSRCBASE:			tcp++; break;
            case ARGUSPRINTTCPDSTBASE:			tcp++; break;
            case ARGUSPRINTTCPRTT:			tcp++; break;
            case ARGUSPRINTINODE:			break;
            case ARGUSPRINTSTDDEV:			agg++; break;
            case ARGUSPRINTRELDATE:			dtime++; break;
            case ARGUSPRINTBYTEOFFSET:			sfile++; break;
            case ARGUSPRINTSRCNET:			flow++; break;
            case ARGUSPRINTDSTNET:			flow++; break;
            case ARGUSPRINTSRCDURATION:			dtime++; break;
            case ARGUSPRINTDSTDURATION:			dtime++; break;
            case ARGUSPRINTTCPSRCMAX:			tcp++; break;
            case ARGUSPRINTTCPDSTMAX:			tcp++; break;
            case ARGUSPRINTTCPSYNACK:			tcp++; break;
            case ARGUSPRINTTCPACKDAT:			tcp++; break;
            case ARGUSPRINTSRCSTARTDATE:		dtime++; break;
            case ARGUSPRINTSRCLASTDATE:			dtime++; break;
            case ARGUSPRINTDSTSTARTDATE:		dtime++; break;
            case ARGUSPRINTDSTLASTDATE:			dtime++; break;
            case ARGUSPRINTSRCENCAPS:			encaps++; break;
            case ARGUSPRINTDSTENCAPS:			encaps++; break;
            case ARGUSPRINTSRCMAXPKTSIZE:		metrics++; psize++; break;
            case ARGUSPRINTSRCMINPKTSIZE:		metrics++; psize++; break;
            case ARGUSPRINTDSTMAXPKTSIZE:		metrics++; psize++; break;
            case ARGUSPRINTDSTMINPKTSIZE:		metrics++; psize++; break;
            case ARGUSPRINTSRCCOUNTRYCODE:		label++; break;
            case ARGUSPRINTDSTCOUNTRYCODE:		label++; break;
            case ARGUSPRINTSRCHOPCOUNT:			attr++; break;
            case ARGUSPRINTDSTHOPCOUNT:			attr++; break;
         }
      }
   }
}

void ArgusPrintRecordHeader (struct ArgusParserStruct *, char *, struct ArgusRecordStruct *, int);
void ArgusPrintRecordCloser (struct ArgusParserStruct *, char *, struct ArgusRecordStruct *, int);

int ArgusParseInited = 0;

void
ArgusPrintRecord (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusInput *input = argus->input;
   char *timeFormat = parser->RaTimeFormat;
   char tbuf[0x10000], *tptr;
   extern char version[];
   int slen = 0, dlen = len;

   if (!(ArgusParseInited++)) {
      if (argus->input)
         ArgusInitAddrtoname (parser, argus->input->ArgusLocalNet, argus->input->ArgusNetMask);
      else
         ArgusInitAddrtoname (parser, 0L, 0L);
   }

   if (parser->RaFieldQuoted) {
      tptr = buf;
      bzero (tbuf, 16);
      buf = tbuf;
   }

   if (parser->ArgusPrintXml) {

      parser->RaTimeFormat="%Y-%m-%dT%H:%M:%S.%f";

      if (parser->RaXMLStarted == 0) {
         struct ArgusRecord *rec = (struct ArgusRecord *) &input->ArgusManStart;
         struct ArgusInterfaceStruct *interface = &ArgusInterfaceTypes[0];

         char StartDateBuf[64], CurrentDateBuf[64];
         char ArgusSourceId[64];
         struct timeval tvpbuf, *tvp = &tvpbuf;

         bzero(StartDateBuf, sizeof(StartDateBuf));
         bzero(CurrentDateBuf, sizeof(CurrentDateBuf));
         bzero(ArgusSourceId, sizeof(ArgusSourceId));

         tvp->tv_sec  = rec->argus_mar.startime.tv_sec;
         tvp->tv_usec = rec->argus_mar.startime.tv_usec;

         ArgusPrintTime(parser, StartDateBuf, tvp);
	      
         gettimeofday(tvp, 0L);
         ArgusPrintTime(parser, CurrentDateBuf, tvp);

         snprintf(buf, dlen, "<?xml version =\"1.0\" encoding=\"UTF-8\"?>\n");
         slen = strlen(buf); dlen = len - slen;

         snprintf(&buf[slen], dlen, "<!--Generated by %s(%s) QoSient, LLC-->\n", parser->ArgusProgramName, version);
         slen = strlen(buf); dlen = len - slen;

         snprintf(&buf[slen], dlen, "<ArgusDataStream");
         slen = strlen(buf); dlen = len - slen;

         snprintf(&buf[slen], dlen, "\n  xmlns:xsi = \"http://www.w3.org/2001/XMLSchema-instance\" ");
         slen = strlen(buf); dlen = len - slen;

         snprintf(&buf[slen], dlen, "\n  xsi:noNamespaceSchemaLocation = \"http://qosient.com/argus/Xml/ArgusRecord.3.0.xsd\"");
         slen = strlen(buf); dlen = len - slen;

         snprintf(&buf[slen], dlen, "\n  BeginDate = \"%s\" CurrentDate = \"%s\"", StartDateBuf, CurrentDateBuf);
         slen = strlen(buf); dlen = len - slen;

         snprintf(&buf[slen], dlen, "\n  MajorVersion = \"%d\" MinorVersion = \"%d\" ", input->major_version, input->minor_version);
         slen = strlen(buf); dlen = len - slen;

         while (interface->value >= 0) {
            if (rec->argus_mar.interfaceType == interface->value)
               break;
            interface++;
         }

         snprintf(&buf[slen], dlen, "InterfaceType = \"%s\" InterfaceStatus = \"%s\"\n ", interface->label, "Up");
         slen = strlen(buf); dlen = len - slen;
         
         ArgusPrintSourceID (parser, ArgusSourceId, argus, 32);

         snprintf(&buf[slen], dlen, " Argus%s ", &ArgusSourceId[1]);
         slen = strlen(buf); dlen = len - slen;

         snprintf(&buf[slen], dlen, " NetAddr = \"%s\" ", ipaddr_string(&input->ArgusLocalNet));
         slen = strlen(buf); dlen = len - slen;

         snprintf(&buf[slen], dlen, " NetMask = \"%s\">\n\n", ipaddr_string(&input->ArgusNetMask));
         slen = strlen(buf); dlen = len - slen;

         ArgusPrintXmlSortAlgorithms(parser);
         parser->RaXMLStarted++;
      }

      ArgusPrintRecordHeader (parser, &buf[slen], argus, dlen);
   }

#if defined(ARGUS_THREADS)
      pthread_mutex_lock(&parser->lock); 
#endif

   for (parser->RaPrintIndex = 0; parser->RaPrintIndex < MAX_PRINT_ALG_TYPES; parser->RaPrintIndex++) {
      char tmpbuf[0x10000];

      if (parser->RaPrintAlgorithmList[parser->RaPrintIndex] != NULL) {
         bzero(tmpbuf, 16);
         parser->RaPrintAlgorithm = parser->RaPrintAlgorithmList[parser->RaPrintIndex];

         parser->RaPrintAlgorithm->print(parser, tmpbuf, argus, parser->RaPrintAlgorithm->length);
         if ((slen = strlen(tmpbuf)) > 0) {
            dlen = sizeof(tmpbuf) - slen;

            if (tmpbuf[slen - 1] == ' ') {
               tmpbuf[slen - 1] = '\0';  // remove trailing space
               slen--;
               dlen = sizeof(tmpbuf) - slen;
            }

            if (!(parser->ArgusPrintXml)) {
               if ((parser->RaPrintIndex > 0) && (parser->RaPrintIndex < ARGUS_MAX_PRINT_ALG)) {
                  if ((parser->RaFieldDelimiter == '\0') || (parser->RaFieldDelimiter == ' ')) {
                     int tok = 0, i;

                     for (i = 0; i < strlen(tmpbuf); i++) {
                        if (!isspace(tmpbuf[i])) {
                           tok = 1; break; 
                        } 
                     } 
                     if (tok) {
                        if (((parser->RaPrintAlgorithmList[parser->RaPrintIndex]->print     == ArgusPrintSrcPort) &&
                             (parser->RaPrintAlgorithmList[parser->RaPrintIndex - 1]->print == ArgusPrintSrcAddr)) ||
                            ((parser->RaPrintAlgorithmList[parser->RaPrintIndex]->print     == ArgusPrintDstPort) &&
                             (parser->RaPrintAlgorithmList[parser->RaPrintIndex - 1]->print == ArgusPrintDstAddr))) {

                           if (parser->RaFieldDelimiter == '\0')
                              if (buf[strlen(buf) - 1] == ' ')
                                 buf[strlen(buf) - 1] = '.';
                        }
                     }
                  }
               }

               slen = strlen(tmpbuf); dlen = len - slen;
               snprintf(&tmpbuf[slen], dlen, " ");

            } else {
               slen = strlen(tmpbuf); dlen = len - slen;
               if (slen > 0)
                  if (!(tmpbuf[slen - 1] == '\"'))
                     sprintf(&tmpbuf[slen], "\"");
            }

            if ((parser->RaFieldDelimiter != ' ') && (parser->RaFieldDelimiter != '\0')) {
               if (parser->RaPrintAlgorithm->print != ArgusPrintFlags)
                  while ((strlen(tmpbuf) > 0) && isspace((int)(tmpbuf[strlen(tmpbuf) - 1])))
                     tmpbuf[strlen(tmpbuf) - 1] = '\0';
    
               slen = strlen(buf); dlen = len - slen;
               snprintf(&buf[slen], dlen, "%s%c", tmpbuf, parser->RaFieldDelimiter);
            } else {
               slen = strlen(buf); dlen = len - slen;
               snprintf(&buf[slen], dlen, "%s", tmpbuf);
            }
         }
      }
   }
#if defined(ARGUS_THREADS)
      pthread_mutex_unlock(&parser->lock); 
#endif

   slen = strlen(buf); dlen = len - slen;

   if (!(parser->ArgusPrintXml))
      while (isspace((int)(buf[strlen(buf) - 1]))) {
         buf[slen - 1] = '\0';
         slen--;
      }

   if ((parser->RaFieldDelimiter != ' ') && (parser->RaFieldDelimiter != '\0'))
      if (buf[slen - 1] == parser->RaFieldDelimiter) {
         buf[slen - 1] = '\0';
         slen--;
      }
   
   if (parser->RaFieldQuoted) {
      char *ptr = tptr, sepbuf[8], *sep = sepbuf;
      char *ap, *tstr = buf;
      int i = 0;

      bzero(sep, 8);
      sep[0] = parser->RaFieldDelimiter;

      while ((ap = strtok(tstr, sep)) != NULL) {
         if (i++)
            *ptr++ = parser->RaFieldDelimiter;
         if (*ap != '\0') {
            snprintf (ptr, MAXSTRLEN, "%c%s%c", parser->RaFieldQuoted, ap, parser->RaFieldQuoted);
            ptr += strlen(ptr);
         } else {
            snprintf (ptr, MAXSTRLEN, "%c%c", parser->RaFieldQuoted, parser->RaFieldQuoted);
            ptr += strlen(ptr);
         }
         tstr = NULL;
      }
   }

   slen = strlen(buf); dlen = len - slen;

   if (parser->ArgusPrintXml) {
      ArgusPrintRecordCloser (parser, &buf[slen], argus, dlen);
      parser->RaTimeFormat = timeFormat;
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintRecord (%p, %p, %p, %d)", parser, buf, argus, len);
#endif
}


void
ArgusPrintRecordHeader (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   if (parser->ArgusPrintXml) {
      char ArgusTypeBuf[32], *ArgusTypeStr    = ArgusTypeBuf;

      snprintf (ArgusTypeBuf, 32, " ");

      switch (argus->hdr.type & 0xF0) {
         case ARGUS_MAR:      snprintf (ArgusTypeBuf, 32, "Management"); break;
         case ARGUS_FAR:      snprintf (ArgusTypeBuf, 32, "Flow"); break;
         case ARGUS_NETFLOW:  snprintf (ArgusTypeBuf, 32, "NetFlow"); break;
         case ARGUS_INDEX:    snprintf (ArgusTypeBuf, 32, "Index"); break;
         case ARGUS_DATASUP:  snprintf (ArgusTypeBuf, 32, "Supplement"); break;
         case ARGUS_ARCHIVAL: snprintf (ArgusTypeBuf, 32, "Archive"); break;
         case ARGUS_EVENT:    snprintf (ArgusTypeBuf, 32, "Event"); break;
         default:             snprintf (ArgusTypeBuf, 32, "Unknown"); break;
      }

      snprintf(&buf[strlen(buf)], len, " <Argus%sRecord ", ArgusTypeStr);
   }
}

void
ArgusPrintRecordCloser (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   if (parser->ArgusPrintXml) {
      char ArgusTypeBuf[32], *ArgusTypeStr    = ArgusTypeBuf;

      snprintf (ArgusTypeBuf, 32, " ");

      switch (argus->hdr.type & 0xF0) {
         case ARGUS_MAR:      snprintf (ArgusTypeBuf, 32, "Management"); break;
         case ARGUS_FAR:      snprintf (ArgusTypeBuf, 32, "Flow"); break;
         case ARGUS_NETFLOW:  snprintf (ArgusTypeBuf, 32, "NetFlow"); break;
         case ARGUS_INDEX:    snprintf (ArgusTypeBuf, 32, "Index"); break;
         case ARGUS_DATASUP:  snprintf (ArgusTypeBuf, 32, "Supplement"); break;
         case ARGUS_ARCHIVAL: snprintf (ArgusTypeBuf, 32, "Archive"); break;
         case ARGUS_EVENT:    snprintf (ArgusTypeBuf, 32, "Event"); break;
         default:             snprintf (ArgusTypeBuf, 32, "Unknown"); break;
      }
      snprintf(&buf[strlen(buf)], len, "></Argus%sRecord>", ArgusTypeStr);
   }
}


void
ArgusPrintBssid (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusFlow *flow;
   int type = 0;

   switch (argus->hdr.type & 0xF0) {
      case ARGUS_MAR: 
      case ARGUS_EVENT: 
      case ARGUS_NETFLOW: {
         if (parser->ArgusPrintXml) {
         } else {
            switch (parser->RaFieldWidth) {
               case RA_FIXED_WIDTH:
                  sprintf (buf, "%*.*s ", len, len, "");
                  break;
               default:
                  sprintf (buf, "%s ", "");
                  break;
            }
         }
         break;
      }

      case ARGUS_FAR: {
         char ArgusBssidStr[36];
         bzero(ArgusBssidStr, sizeof(ArgusBssidStr));

         if (((flow = (void *)argus->dsrs[ARGUS_FLOW_INDEX]) != NULL)) {
            switch (flow->hdr.subtype & 0x3F) {
               case ARGUS_FLOW_CLASSIC5TUPLE: 
               case ARGUS_FLOW_LAYER_3_MATRIX: {
                  switch (type = (flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                     case ARGUS_TYPE_WLAN: {
                        char *macstr = etheraddr_string (parser, flow->wlan_flow.bssid);
                        bcopy(macstr, ArgusBssidStr, strlen(macstr));
                        break;
                     }

                     default:
                        break;
                  }
                  break;
               }

               default:
                  break;
            }
         } 

         if (parser->ArgusPrintXml) {
            if (strlen(ArgusBssidStr))
               sprintf (buf, " Bssid = \"%s\"", ArgusBssidStr);
         } else {
            switch (parser->RaFieldWidth) {
               case RA_FIXED_WIDTH: {
                  sprintf (buf, "%*.*s ", len, len, ArgusBssidStr);
                  if (strlen(ArgusBssidStr) > len) {
                     buf[len - 1] = '*';
                     buf[len]     = '\0';
                  }
                  break;
               }
               default:
                  sprintf (buf, "%s ", ArgusBssidStr);
                  break;
            }
         }
         break;
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintBssid (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintSsid (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusFlow *flow;
   int type = 0;

   switch (argus->hdr.type & 0xF0) {
      case ARGUS_MAR: 
      case ARGUS_EVENT: 
      case ARGUS_NETFLOW: {
         if (parser->ArgusPrintXml) {
         } else {
            switch (parser->RaFieldWidth) {
               case RA_FIXED_WIDTH:
                  sprintf (buf, "%*.*s ", len, len, "");
                  break;
               default:
                  sprintf (buf, "%s ", "");
                  break;
            }
         }
         break;
      }

      case ARGUS_FAR: {
         char ArgusSsidStr[36];
         bzero(ArgusSsidStr, sizeof(ArgusSsidStr));

         if (((flow = (void *)argus->dsrs[ARGUS_FLOW_INDEX]) != NULL)) {
            switch (flow->hdr.subtype & 0x3F) {
               case ARGUS_FLOW_CLASSIC5TUPLE: 
               case ARGUS_FLOW_LAYER_3_MATRIX: {
                  switch (type = (flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                     case ARGUS_TYPE_WLAN: {
                        int len = strlen(flow->wlan_flow.ssid);
                        if (len > 32) len = 32;
                        bcopy(flow->wlan_flow.ssid, ArgusSsidStr, len);
                        break;
                     }

                     default:
                        break;
                  }
                  break;
               }

               default:
                  break;
            }
         } 

         if (parser->ArgusPrintXml) {
            if (strlen(ArgusSsidStr))
               sprintf (buf, " Ssid = \"%s\"", ArgusSsidStr);
         } else {
            switch (parser->RaFieldWidth) {
               case RA_FIXED_WIDTH:
                  sprintf (buf, "%*.*s ", len, len, ArgusSsidStr);
                  if (strlen(ArgusSsidStr) > len) {
                     buf[len - 1] = '*';
                     buf[len]     = '\0';
                  }
                  break;
               default:
                  sprintf (buf, "%s ", ArgusSsidStr);
                  break;
            }
         }
         break;
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSsid (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintCause (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char *ArgusCauseStr = NULL;

   switch (argus->hdr.cause & 0xF0) {
      case ARGUS_START:    ArgusCauseStr = "Start"; break;
      case ARGUS_STATUS:   ArgusCauseStr = "Status"; break;
      case ARGUS_STOP:     ArgusCauseStr = "Stop"; break;
      case ARGUS_SHUTDOWN: ArgusCauseStr = "Shutdown"; break;
      case ARGUS_TIMEOUT:  ArgusCauseStr = "Timeout"; break;
      case ARGUS_ERROR:    ArgusCauseStr = "Error"; break;
      default:             ArgusCauseStr = "Unknown"; break;
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " Cause = \"%s\"", ArgusCauseStr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH)
         len = strlen(ArgusCauseStr);
      else {
         if (strlen(ArgusCauseStr) > len) {
            ArgusCauseStr[len - 1] = '*';
            ArgusCauseStr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ArgusCauseStr);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintCause (%p, %p, %d)", buf, argus, len);
#endif
}

void
ArgusPrintStartDate (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct timeval tvpbuf, *tvp = &tvpbuf;
   char tbuf[256];
          
   switch (argus->hdr.type & 0xF0) {
      case ARGUS_MAR: {
         struct ArgusRecord *rec = (struct ArgusRecord *) argus->dsrs[0];
         tvp->tv_sec  = rec->argus_mar.now.tv_sec;
         tvp->tv_usec = rec->argus_mar.now.tv_usec;
         break;
      }

      case ARGUS_EVENT:
      case ARGUS_NETFLOW:
      case ARGUS_FAR: {
         long long stime = ArgusFetchStartuSecTime(argus);

         tvp->tv_sec  = stime / 1000000;
         tvp->tv_usec = stime % 1000000;
      }
   }
             
   bzero(tbuf, sizeof(tbuf));
   ArgusPrintTime(parser, tbuf, tvp);

   if (strchr (tbuf, '.'))
      len += parser->pflag;

   if (parser->ArgusPrintXml) {
      sprintf (buf, " StartTime = \"%s\"", tbuf);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(tbuf);
      } else {
         if (strlen(tbuf) > len) {
            tbuf[len - 1] = '*';
            tbuf[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, tbuf);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintStartDate (%p, %p, %d)", buf, argus, len);
#endif
}

void
ArgusPrintLastDate (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct timeval tvpbuf, *tvp = &tvpbuf;
   char tbuf[256];
 
   len += parser->pflag;

   switch (argus->hdr.type & 0xF0) {
      case ARGUS_MAR: {
         struct ArgusRecord *rec = (struct ArgusRecord *) argus->dsrs[0];
         tvp->tv_sec  = rec->argus_mar.startime.tv_sec;
         tvp->tv_usec = rec->argus_mar.startime.tv_usec;
         break;
      }

      case ARGUS_EVENT:
      case ARGUS_NETFLOW:
      case ARGUS_FAR: {
         long long stime = ArgusFetchLastuSecTime(argus);

         tvp->tv_sec  = stime / 1000000;
         tvp->tv_usec = stime % 1000000;
      }
   }

   bzero(tbuf, sizeof(tbuf));
   ArgusPrintTime(parser, tbuf, tvp);

   if (parser->ArgusPrintXml) {
      sprintf (buf, "  LastTime = \"%s\"", tbuf);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(tbuf);
      } else {
         if (strlen(tbuf) > len) {
            tbuf[len - 1] = '*';
            tbuf[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, tbuf);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintLastDate (%p, %p %d)", buf, argus, len);
#endif
}


void
ArgusPrintSrcStartDate (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct timeval tvpbuf, *tvp = &tvpbuf;
   char tbuf[256];
          
   len += parser->pflag;
   if (argus->hdr.type & ARGUS_MAR) {
      struct ArgusRecord *rec = (struct ArgusRecord *) argus->dsrs[0];
      if (rec != NULL) {
         tvp->tv_sec  = rec->argus_mar.startime.tv_sec;
         tvp->tv_usec = rec->argus_mar.startime.tv_usec;
      }

   } else {
      struct ArgusTimeObject *dtime = (struct ArgusTimeObject *)argus->dsrs[ARGUS_TIME_INDEX];
      if (dtime != NULL) {
         tvp->tv_sec  = dtime->src.start.tv_sec;
         tvp->tv_usec = dtime->src.start.tv_usec;
      }
   }
             
   bzero(tbuf, sizeof(tbuf));
   ArgusPrintTime(parser, tbuf, tvp);

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcStartTime = \"%s\"", tbuf);
      
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(tbuf);
      } else {
         if (strlen(tbuf) > len) {
            tbuf[len - 1] = '*';
            tbuf[len]     = '\0'; 
         }        
      }
      sprintf (buf, "%*.*s ", len, len, tbuf);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcStartDate (%p, %p, %d)", buf, argus, len);
#endif
}


void
ArgusPrintSrcLastDate (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct timeval tvpbuf, *tvp = &tvpbuf;
   char tbuf[256];
 
   len += parser->pflag;
   if (argus->hdr.type & ARGUS_MAR) {
      struct ArgusRecord *rec = (struct ArgusRecord *) argus->dsrs[0];
      if (rec != NULL) {
         tvp->tv_sec  = rec->argus_mar.now.tv_sec;
         tvp->tv_usec = rec->argus_mar.now.tv_usec;
      }
   } else {
      struct ArgusTimeObject *dtime = (struct ArgusTimeObject *)argus->dsrs[ARGUS_TIME_INDEX];
      if (dtime != NULL) {
         tvp->tv_sec  = dtime->src.end.tv_sec;
         tvp->tv_usec = dtime->src.end.tv_usec;
      }
   }

   bzero(tbuf, sizeof(tbuf));
   ArgusPrintTime(parser, tbuf, tvp);

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcLastTime = \"%s\"", tbuf);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(tbuf);
      } else {
         if (strlen(tbuf) > len) {
            tbuf[len - 1] = '*';
            tbuf[len]     = '\0'; 
         }        
      }
      sprintf (buf, "%*.*s ", len, len, tbuf);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcLastDate (%p, %p, %d)", buf, argus, len);
#endif
}


void
ArgusPrintDstStartDate (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct timeval tvpbuf, *tvp = &tvpbuf;
   char tbuf[256];
          
   len += parser->pflag;
   bzero(tvp, sizeof(tvpbuf));

   if (argus->hdr.type & ARGUS_MAR) {
      struct ArgusRecord *rec = (struct ArgusRecord *) argus->dsrs[0];
      if (rec != NULL) {
         tvp->tv_sec  = rec->argus_mar.startime.tv_sec;
         tvp->tv_usec = rec->argus_mar.startime.tv_usec;
      }

   } else {
      struct ArgusTimeObject *dtime = (struct ArgusTimeObject *)argus->dsrs[ARGUS_TIME_INDEX];
      if (dtime != NULL) {
         tvp->tv_sec  = dtime->dst.start.tv_sec;
         tvp->tv_usec = dtime->dst.start.tv_usec;
      }
   }
             
   bzero(tbuf, sizeof(tbuf));
   ArgusPrintTime(parser, tbuf, tvp);

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstStartTime = \"%s\"", tbuf);
      
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(tbuf);
      } else {
         if (strlen(tbuf) > len) {
            tbuf[len - 1] = '*';
            tbuf[len]     = '\0'; 
         }        
      }
      sprintf (buf, "%*.*s ", len, len, tbuf);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstStartDate (%p, %p, %d)", buf, argus, len);
#endif
}


void
ArgusPrintDstLastDate (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct timeval tvpbuf, *tvp = &tvpbuf;
   char tbuf[256];
 
   len += parser->pflag;
   bzero(tvp, sizeof(tvpbuf));
   if (argus->hdr.type & ARGUS_MAR) {
      struct ArgusRecord *rec = (struct ArgusRecord *) argus->dsrs[0];
      if (rec != NULL) {
         tvp->tv_sec  = rec->argus_mar.now.tv_sec;
         tvp->tv_usec = rec->argus_mar.now.tv_usec;
      }

   } else {
      struct ArgusTimeObject *dtime = (struct ArgusTimeObject *)argus->dsrs[ARGUS_TIME_INDEX];
      if (dtime != NULL) {
         tvp->tv_sec  = dtime->dst.end.tv_sec;
         tvp->tv_usec = dtime->dst.end.tv_usec;
      }
   }

   bzero(tbuf, sizeof(tbuf));
   ArgusPrintTime(parser, tbuf, tvp);

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstLastTime = \"%s\"", tbuf);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(tbuf);
      } else {
         if (strlen(tbuf) > len) {
            tbuf[len - 1] = '*';
            tbuf[len]     = '\0'; 
         }        
      }
      sprintf (buf, "%*.*s ", len, len, tbuf);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstLastDate (%p, %p, %d)", buf, argus, len);
#endif
}



void
ArgusPrintRelativeDate (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct timeval tvpbuf, *tvp = &tvpbuf;
   char tbuf[256], *ptr;

   if (argus->hdr.type & ARGUS_MAR) {
      struct ArgusRecord *rec = (struct ArgusRecord *) argus->dsrs[0];
      if (rec != NULL) {
         tvp->tv_sec  = rec->argus_mar.now.tv_sec;
         tvp->tv_usec = rec->argus_mar.now.tv_usec;
      }

   } else {
      struct ArgusTimeObject *dtime = (struct ArgusTimeObject *)argus->dsrs[ARGUS_TIME_INDEX];

      if (parser->ArgusStartTimeVal.tv_sec == 0) {
         parser->ArgusStartTimeVal.tv_sec  = dtime->src.start.tv_sec;
         parser->ArgusStartTimeVal.tv_usec = dtime->src.start.tv_usec;
      }

      if (dtime != NULL)
         RaDiffTime ((struct timeval *)&dtime->src.start, &parser->ArgusStartTimeVal, tvp);
   }

   bzero(tbuf, sizeof(tbuf));

   sprintf (tbuf, "%d", (int) tvp->tv_sec);

   if (parser->pflag) {
      while (isspace((int)tbuf[strlen(tbuf) - 1]))
         tbuf[strlen(tbuf) - 1] = '\0';
      ptr = &tbuf[strlen(tbuf)];
      sprintf (ptr, ".%06u", (int) tvp->tv_usec);
      ptr[parser->pflag] = '\0';
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " RelativeDate = \"%s\"", tbuf);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(tbuf);
      } else {
         if (strlen(tbuf) > len) {
            tbuf[len - 1] = '*';
            tbuf[len]     = '\0'; 
         }        
      }
      sprintf (buf, "%*.*s ", len, len, tbuf);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintRelativeDate (%p, %p, %d)", buf, argus, len);
#endif
}

void
ArgusPrintByteOffset (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tbuf[32];

   bzero(tbuf, sizeof(tbuf));
#if defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__APPLE_CC__) || defined(__APPLE__) || defined(HAVE_SOLARIS)
   sprintf (tbuf, "%llu", argus->offset);
#else
   sprintf (tbuf, "%Lu", argus->offset);
#endif

   if (parser->ArgusPrintXml) {
      sprintf (buf, " ByteOffset = \"%s\"", tbuf);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(tbuf);
      } else {
         if (strlen(tbuf) > len) {
            tbuf[len - 1] = '*';
            tbuf[len]     = '\0'; 
         }        
      }
      sprintf (buf, "%*.*s ", len, len, tbuf);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintByteOffset (%p, %p, %d)", buf, argus, len);
#endif
}

void
ArgusPrintAutoId (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tbuf[32];

   bzero(tbuf, sizeof(tbuf));
#if defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__APPLE_CC__) || defined(__APPLE__) || defined(HAVE_SOLARIS)
   sprintf (tbuf, "%d", argus->autoid);
#else
   sprintf (tbuf, "%d", argus->autoid);
#endif

   if (parser->ArgusPrintXml) {
      sprintf (buf, " AutoId = \"%s\"", tbuf);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(tbuf);
      } else {
         if (strlen(tbuf) > len) {
            tbuf[len - 1] = '*';
            tbuf[len]     = '\0'; 
         }        
      }
      sprintf (buf, "%*.*s ", len, len, tbuf);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintAutoId (%p, %p, %d)", buf, argus, len);
#endif
}



float
RaGetFloatDuration (struct ArgusRecordStruct *argus)
{
   float retn = 0;
   int sec = 0, usec = 0;

   if (argus->hdr.type & ARGUS_MAR) {
      struct ArgusRecord *rec = (struct ArgusRecord *) argus->dsrs[0];
      if (rec != NULL) {
         sec  = rec->argus_mar.now.tv_sec  - rec->argus_mar.startime.tv_sec;
         usec = rec->argus_mar.now.tv_usec - rec->argus_mar.startime.tv_usec;
      }

   } else {
      struct ArgusTimeObject *dtime = (void *)argus->dsrs[ARGUS_TIME_INDEX];
      if (dtime != NULL) {
         struct timeval stbuf, *st = &stbuf;
         struct timeval ltbuf, *lt = &ltbuf;
         struct timeval stimebuf, *stime = &stimebuf;
         struct timeval ltimebuf, *ltime = &ltimebuf;

         unsigned char subtype = dtime->hdr.subtype & (ARGUS_TIME_SRC_START | ARGUS_TIME_DST_START |
                                                       ARGUS_TIME_SRC_END   | ARGUS_TIME_DST_END);
         if (subtype) {
            switch (subtype) {
               case ARGUS_TIME_SRC_START | ARGUS_TIME_DST_START |
                    ARGUS_TIME_DST_END: {
                  st->tv_sec  = dtime->src.start.tv_sec;
                  st->tv_usec = dtime->src.start.tv_usec;
                  lt->tv_sec  = dtime->dst.start.tv_sec;
                  lt->tv_usec = dtime->dst.start.tv_usec;
                  *stime = *RaMinTime(st, lt);

                  st->tv_sec  = dtime->src.start.tv_sec;
                  st->tv_usec = dtime->src.start.tv_usec;
                  lt->tv_sec  = dtime->dst.end.tv_sec;
                  lt->tv_usec = dtime->dst.end.tv_usec;
                  *ltime = *RaMaxTime(st, lt);
                  break;
               }

               case ARGUS_TIME_SRC_START | ARGUS_TIME_DST_START |
                    ARGUS_TIME_SRC_END: {
                  st->tv_sec  = dtime->src.start.tv_sec;
                  st->tv_usec = dtime->src.start.tv_usec;
                  lt->tv_sec  = dtime->dst.start.tv_sec;
                  lt->tv_usec = dtime->dst.start.tv_usec;
                  *stime = *RaMinTime(st, lt);

                  st->tv_sec  = dtime->dst.start.tv_sec;
                  st->tv_usec = dtime->dst.start.tv_usec;
                  lt->tv_sec  = dtime->src.end.tv_sec;
                  lt->tv_usec = dtime->src.end.tv_usec;
                  *ltime = *RaMaxTime(st, lt);
               }

               case ARGUS_TIME_SRC_START | ARGUS_TIME_DST_START |
                    ARGUS_TIME_SRC_END   | ARGUS_TIME_DST_END: {
                  st->tv_sec  = dtime->src.start.tv_sec;
                  st->tv_usec = dtime->src.start.tv_usec;
                  lt->tv_sec  = dtime->dst.start.tv_sec;
                  lt->tv_usec = dtime->dst.start.tv_usec;
                  *stime = *RaMinTime(st, lt);

                  st->tv_sec  = dtime->src.end.tv_sec;
                  st->tv_usec = dtime->src.end.tv_usec;
                  lt->tv_sec  = dtime->dst.end.tv_sec;
                  lt->tv_usec = dtime->dst.end.tv_usec;
                  *ltime = *RaMaxTime(st, lt);
                  break;
               }

               case ARGUS_TIME_SRC_START: {
                  st->tv_sec  = dtime->src.start.tv_sec;
                  st->tv_usec = dtime->src.start.tv_usec;

                  *stime = *st;
                  *ltime = *st;
                  break;
               }

               case ARGUS_TIME_SRC_START | ARGUS_TIME_SRC_END: {
                  st->tv_sec  = dtime->src.start.tv_sec;
                  st->tv_usec = dtime->src.start.tv_usec;
                  lt->tv_sec  = dtime->src.end.tv_sec;
                  lt->tv_usec = dtime->src.end.tv_usec;

                  *stime = *st;
                  *ltime = *lt;
                  break;
               }

               case ARGUS_TIME_DST_START: {
                  st->tv_sec  = dtime->dst.start.tv_sec;
                  st->tv_usec = dtime->dst.start.tv_usec;

                  *stime = *st;
                  *ltime = *st;
                  break;
               }

               case ARGUS_TIME_DST_START | ARGUS_TIME_DST_END: {
                  st->tv_sec  = dtime->dst.start.tv_sec;
                  st->tv_usec = dtime->dst.start.tv_usec;
                  lt->tv_sec  = dtime->dst.end.tv_sec;
                  lt->tv_usec = dtime->dst.end.tv_usec;

                  *stime = *st;
                  *ltime = *lt;
                  break;
               }

               case ARGUS_TIME_SRC_START | ARGUS_TIME_DST_END: {
                  st->tv_sec  = dtime->src.start.tv_sec;
                  st->tv_usec = dtime->src.start.tv_usec;
                  lt->tv_sec  = dtime->dst.end.tv_sec;
                  lt->tv_usec = dtime->dst.end.tv_usec;

                  *stime = *st;
                  *ltime = *lt;
                  break;
               }

               case ARGUS_TIME_DST_START | ARGUS_TIME_SRC_END: {
                  st->tv_sec  = dtime->dst.start.tv_sec;
                  st->tv_usec = dtime->dst.start.tv_usec;
                  lt->tv_sec  = dtime->src.end.tv_sec;
                  lt->tv_usec = dtime->src.end.tv_usec;

                  *stime = *st;
                  *ltime = *lt;
                  break;
               }

               case ARGUS_TIME_SRC_START | ARGUS_TIME_DST_START: {
                  st->tv_sec  = dtime->src.start.tv_sec;
                  st->tv_usec = dtime->src.start.tv_usec;
                  lt->tv_sec  = dtime->dst.start.tv_sec;
                  lt->tv_usec = dtime->dst.start.tv_usec;
                  *stime = *RaMinTime(st, lt);
                  *ltime = *RaMaxTime(st, lt);
                  break;
               }

               default:
                  break;
            }

         } else {
            st->tv_sec  = dtime->src.start.tv_sec;
            st->tv_usec = dtime->src.start.tv_usec;
            lt->tv_sec  = dtime->src.end.tv_sec;
            lt->tv_usec = dtime->src.end.tv_usec;
            stime = st;
            ltime = lt;
         }


         if (stime && ltime) {
            sec  = ltime->tv_sec  - stime->tv_sec;
            usec = ltime->tv_usec - stime->tv_usec;
         }
      }
      retn  = (sec * 1.0) + usec/1000000.0;
   }

   return (retn);
}


float
RaGetFloatSrcDuration (struct ArgusRecordStruct *argus)
{
   float retn = 0;
   int sec = 0, usec = 0;

   if (argus->hdr.type & ARGUS_MAR) {
      struct ArgusRecord *rec = (struct ArgusRecord *) argus->dsrs[0];
      if (rec != NULL) {
         sec  = rec->argus_mar.now.tv_sec  - rec->argus_mar.startime.tv_sec;
         usec = rec->argus_mar.now.tv_usec - rec->argus_mar.startime.tv_usec;
      }

   } else {
      struct ArgusTimeObject *dtime = (void *)argus->dsrs[ARGUS_TIME_INDEX];
      if (dtime != NULL) {
         struct timeval *stime = NULL;
         struct timeval *ltime = NULL;
         struct timeval stbuf, *st = &stbuf;
         struct timeval ltbuf, *lt = &ltbuf;

         st->tv_sec  = dtime->src.start.tv_sec;
         st->tv_usec = dtime->src.start.tv_usec;
         lt->tv_sec  = dtime->src.end.tv_sec;
         lt->tv_usec = dtime->src.end.tv_usec;
         stime = st;
         ltime = lt;

         if (stime && ltime) {
            sec  = ltime->tv_sec  - stime->tv_sec;
            usec = ltime->tv_usec - stime->tv_usec;
         }
      }

      if ((sec == 0) && (usec == 0))
         retn = 0;
      else
         retn  = (sec * 1.0) + usec/1000000.0;
   }
   return (retn);
}


float
RaGetFloatDstDuration (struct ArgusRecordStruct *argus)
{

   float retn = 0;
   int sec = 0, usec = 0;

   if (argus->hdr.type & ARGUS_MAR) {
      struct ArgusRecord *rec = (struct ArgusRecord *) argus->dsrs[0];
      if (rec != NULL) {
         sec  = rec->argus_mar.now.tv_sec  - rec->argus_mar.startime.tv_sec;
         usec = rec->argus_mar.now.tv_usec - rec->argus_mar.startime.tv_usec;
      }

   } else {
      struct ArgusTimeObject *dtime;
      if ((dtime = (void *)argus->dsrs[ARGUS_TIME_INDEX]) != NULL) {
         struct timeval *stime = NULL;
         struct timeval *ltime = NULL;
         struct timeval stbuf, *st = &stbuf;
         struct timeval ltbuf, *lt = &ltbuf;

         st->tv_sec  = dtime->dst.start.tv_sec;
         st->tv_usec = dtime->dst.start.tv_usec;
         lt->tv_sec  = dtime->dst.end.tv_sec;
         lt->tv_usec = dtime->dst.end.tv_usec;
         stime = st;
         ltime = lt;

         if (stime && ltime) {
            sec  = ltime->tv_sec  - stime->tv_sec;
            usec = ltime->tv_usec - stime->tv_usec;
         }
      }

      if ((sec == 0) && (usec == 0))
         retn = 0;
      else
         retn  = (sec * 1.0) + usec/1000000.0;
   }
   return (retn);
}


float
RaGetFloatMean (struct ArgusRecordStruct *argus)
{
   float retn = 0.0;

   if (argus->hdr.type & ARGUS_MAR) {
   } else {
      struct ArgusAgrStruct *agr;
      if ((agr = (struct ArgusAgrStruct *) argus->dsrs[ARGUS_AGR_INDEX]) != NULL)
         retn = agr->act.meanval;
   }

   return (retn);
}

float
RaGetFloatIdleTime (struct ArgusRecordStruct *argus)
{
   float retn = 0.0;
   double value = 0.0;

   if (argus->hdr.type & ARGUS_MAR) {
   } else {
      double qtime = 0.0, rtime = 0.0;

      rtime = (ArgusParser->ArgusRealTime.tv_sec * 1.0) + (ArgusParser->ArgusRealTime.tv_usec / 1000000.0);
      qtime = (argus->qhdr.lasttime.tv_sec * 1.0) + (argus->qhdr.lasttime.tv_usec / 1000000.0);
      value = rtime - qtime;
      retn = value;
   }

   return (retn);
}

float
RaGetFloatSum (struct ArgusRecordStruct *argus)
{
   float retn = 0.0;

   if (argus->hdr.type & ARGUS_MAR) {
   } else {
      struct ArgusAgrStruct *agr;
      if ((agr = (struct ArgusAgrStruct *) argus->dsrs[ARGUS_AGR_INDEX]) != NULL)
         retn = agr->act.meanval * agr->act.n;
   }

   return (retn);
}

float
RaGetFloatMin (struct ArgusRecordStruct *argus)
{
   float retn = 0.0;
 
   if (argus->hdr.type & ARGUS_MAR) { 
   } else {
      struct ArgusAgrStruct *agr;
      if ((agr = (struct ArgusAgrStruct *) argus->dsrs[ARGUS_AGR_INDEX]) != NULL)
         retn = agr->act.minval; 
   }
 
   return (retn); 
}

float
RaGetFloatMax (struct ArgusRecordStruct *argus)
{
   float retn = 0.0;
 
   if (argus->hdr.type & ARGUS_MAR) { 
   } else {
      struct ArgusAgrStruct *agr;
      if ((agr = (struct ArgusAgrStruct *) argus->dsrs[ARGUS_AGR_INDEX]) != NULL)
         retn = agr->act.maxval; 
   }
 
   return (retn); 
}



/*
   There are two types of addresses to parse, IPv4 and IPv6
   addresses.  An address is in the form:
     dd[.:][:][dd]/n

   where n is the number significant bits in the address.
*/
int ArgusNumTokens (char *, char);
   
int
ArgusNumTokens (char *str, char tok)
{
   int retn = 0;
   if (str != NULL) {
      while ((str = strchr(str, tok)) != NULL) {
         retn++;
         str++;
      }
   }
   return (retn);
}


struct ArgusCIDRAddr *
RaParseCIDRAddr (struct ArgusParserStruct *parser, char *addr)
{
   struct ArgusCIDRAddr *retn = NULL;
   char *ptr = NULL, *mask = NULL, strbuf[128], *str = strbuf;
   int opmask = 0;

   snprintf (str, 128, "%s", addr);
   if (parser->ArgusCIDRPtr == NULL)
      parser->ArgusCIDRPtr = &parser->ArgusCIDRBuffer;

   retn = parser->ArgusCIDRPtr;
   retn->type     = 0;
   retn->len      = 0;
   retn->masklen  = 0;
   memset(&retn->addr, 0, sizeof(retn->addr));

   if ((ptr = strchr(str, '!')) != NULL) {
      opmask = ARGUSMONITOR_NOTEQUAL;
      str = ptr + 1;
   }

   if ((mask = strchr (str, '/')) != NULL) {
      *mask++ = '\0';
      retn->masklen = strtol((const char *)mask, (char **)&ptr, 10);
      if (ptr == mask) {
#ifdef ARGUSDEBUG
         ArgusDebug (1, "RaParseCIDRAddr: format error: mask length incorrect.\n", retn);
#endif
         return (NULL);
      }
   }

   if ((ptr = strchr (str, ':')) != NULL)
      retn->type = AF_INET6;
   else
   if ((ptr = strchr (str, '.')) != NULL)
      retn->type = AF_INET;
  
   if (!(retn->type))
      retn->type = (retn->masklen > 32) ? AF_INET6 : AF_INET;
   
   switch (retn->type) {
      case AF_INET: {
         int i, len = sizeof(struct in_addr);
 
         retn->len = len;
         if (retn->masklen == 0)
            retn->masklen = 32;

         for (i = 0; (i < len) && str; i++) {
            long int tval = strtol(str, (char **)&ptr, 10);
            if (ptr != NULL) {
               if (strlen(ptr) > 0) {
                  if (*ptr++ != '.') {
#ifdef ARGUSDEBUG
                     ArgusDebug (1, "RaParseCIDRAddr: format error: IPv4 addr format.\n");
#endif
                     return(NULL);
                  }
               } else
                  ptr = NULL;

               retn->addr[0] |= (tval << ((len - (i + 1)) * 8));
            }
            str = ptr;
         }

         if (retn->masklen > 0)
            retn->mask[0] = 0xFFFFFFFF << (32 - retn->masklen);
         break;
      }

      case AF_INET6: {
         unsigned short *val = (unsigned short *)&retn->addr;
         int ind = 0, len = sizeof(retn->addr)/sizeof(unsigned short);
         int fsecnum = 8, lsecnum = 0, rsecnum = 0, i, masklen;
         char *sstr = NULL, *ipv4addr = NULL;

         retn->len = sizeof(retn->addr);
         if ((sstr = strstr(str, "::")) != NULL) {
            *sstr++ = '\0';
            *sstr++ = '\0';
            if (strlen(str))
               fsecnum = ArgusNumTokens(str,  ':') + 1;
            if (strlen(sstr))
               lsecnum = ArgusNumTokens(sstr, ':') + 1;
         } else
            sstr = str;

         if (retn->masklen == 0)
            retn->masklen = 128;

         if (strchr (sstr, '.')) {
            lsecnum += (lsecnum > 0) ? 1 : 2;
            if ((ipv4addr = strrchr(sstr, ':')) == NULL) {
               ipv4addr = sstr;
               sstr = NULL;
            } else {
               *ipv4addr++ = '\0';
            }
         }

         if (fsecnum + lsecnum) {
            rsecnum = 8 - (fsecnum + lsecnum);
            if (fsecnum) {
               while (str && *str && (ind++ < len)) {
                  *val++ = htons(strtol(str, (char **)&ptr, 16));

                  if (ptr != NULL) {
                     if (strlen(ptr) > 0) {
                        if (*ptr++ != ':') {
#ifdef ARGUSDEBUG
                           ArgusDebug (1, "RaParseCIDRAddr: format error: IPv4 addr format.\n");
#endif
                           return(NULL);
                        }
                     } else
                        ptr = NULL;
                  }
                  str = ptr;
               }
            }

            for (i = 0; i < rsecnum; i++)
               *val++ = 0;
            if (lsecnum) {
               if ((str = sstr) != NULL) {
                  while (str && (ind++ < len)) {
                     *val++ = htons(strtol(str, (char **)&ptr, 16));

                     if (ptr != NULL) {
                        if (strlen(ptr) > 0) {
                           if (*ptr++ != ':') {
#ifdef ARGUSDEBUG
                              ArgusDebug (1, "RaParseCIDRAddr: format error: IPv4 addr format.\n");
#endif
                              return(NULL);
                           }
                        } else
                           ptr = NULL;
                     }
                     str = ptr;
                  }
               }
            }

            if (ipv4addr) {
               unsigned char *cval = (unsigned char *)&retn->addr[3];
               int ind = 0, len = sizeof(struct in_addr);
 
               while (ipv4addr && (ind++ < len)) {
                  *cval++ = strtol(ipv4addr, (char **)&ptr, 10);
                  if (ptr != NULL) {
                     if (strlen(ptr) > 0) {
                        if (*ptr++ != '.') {
#ifdef ARGUSDEBUG
                           ArgusDebug (1, "RaParseCIDRAddr: format error: IPv4 addr format.\n");
#endif
                           return(NULL);
                        }
                     } else
                        ptr = NULL;
                  }
                  ipv4addr = ptr;
               }
               retn->masklen = 128;
            }
         }

         for (i = 0; i < 4; i++) retn->mask[i] = 0;

         if ((masklen = retn->masklen) > 0) {
            unsigned int *mask = &retn->mask[0];

            while (masklen) {
               if (masklen > 32) {
                  *mask++ = 0xFFFFFFFF;
                  masklen -= 32;
               } else {
                  *mask = htonl(0xFFFFFFFF << (32 - masklen));
                  masklen = 0;
               }
            }
         }
         break;
      }

      default:
         break;
   }

#ifdef ARGUSDEBUG
   ArgusDebug (9, "RaParseCIDRAddr: returning %p \n", retn);
#endif
   return (retn);
}

void ArgusPrintResponse (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintIdleTime (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintSrcRate (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintDstRate (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintRate (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintLoss (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintSrcLoad (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintDstLoad (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintLoad (struct ArgusParserStruct *parser, char *,struct ArgusRecordStruct *, int);
void ArgusPrintSrcTTL (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintDstTTL (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintTos (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintSrcTos (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintDstTos (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintDSByte (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintSrcDSByte (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintDstDSByte (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintWindow (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintDuration (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintSrcDuration (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintDstDuration (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintMean (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintMin (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintMax (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintSum (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintRunTime (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintStdDeviation (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintStartRange (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintEndRange (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintTransactions (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintJoinDelay (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintLeaveDelay (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);


void
ArgusPrintTransactions (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusAgrStruct *nsagr, *agr;
   unsigned int count = 0;
   char trans[32];

   if (argus->hdr.type & ARGUS_MAR) {
      snprintf(trans, 32, " ");

   } else {
      if ((agr = (struct ArgusAgrStruct *) argus->dsrs[ARGUS_AGR_INDEX]) != NULL)
         count = agr->count;

      if (parser->Pctflag && parser->ns) {
         nsagr = (struct ArgusAgrStruct *) parser->ns->dsrs[ARGUS_AGR_INDEX];
         snprintf(trans, 32, "%3.*f", parser->pflag, (count * 100.0) / (nsagr->count) * 1.0);

      } else {
         snprintf(trans, 32, "%u", count);
      }
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " Trans = \"%s\"", trans);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(trans);
      } else {
         if (strlen(trans) > len) {
            trans[len - 1] = '*';
            trans[len]     = '\0'; 
         }        
      }
      snprintf(&buf[strlen(buf)], (MAXSTRLEN - strlen(buf)), "%*.*s ", len, len, trans);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintTransactions (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintMean (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusAgrStruct *agr = NULL;
   char avg[32];
 
   bzero (avg, 32);
   if (argus->hdr.type & ARGUS_MAR) {
   } else {
      if ((agr = (struct ArgusAgrStruct *) argus->dsrs[ARGUS_AGR_INDEX]) != NULL) {
         if (agr->count > 0) {
            sprintf (avg, "%.*f", parser->pflag, agr->act.meanval);
         } else 
            sprintf (avg, "%.*f", parser->pflag, 0.0);
      } else
         sprintf (avg, "%.*f", parser->pflag, 0.0);
   }
 
   if (parser->ArgusPrintXml) {
      sprintf (buf, " Mean = \"%s\"", avg);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(avg);
      } else {
         if (strlen(avg) > len) {
            avg[len - 1] = '*';
            avg[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, avg);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintMean (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintSum (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusAgrStruct *agr = NULL;
   char sum[32];

   bzero (sum, 32);
   if (argus->hdr.type & ARGUS_MAR) {
   } else {
      if ((agr = (struct ArgusAgrStruct *) argus->dsrs[ARGUS_AGR_INDEX]) != NULL) {
         if (agr->count > 0) {
            sprintf (sum, "%.*f", parser->pflag, agr->act.meanval * agr->act.n);
         } else
            sprintf (sum, "%.*f", parser->pflag, 0.0);
      } else
         sprintf (sum, "%.*f", parser->pflag, 0.0);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " Sum = \"%s\"", sum);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(sum);
      } else {
         if (strlen(sum) > len) {
            sum[len - 1] = '*';
            sum[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, sum);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSum (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintRunTime (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusAgrStruct *agr = NULL;
   char sum[32];

   bzero (sum, 32);
   if (argus->hdr.type & ARGUS_MAR) {
   } else {
      if ((agr = (struct ArgusAgrStruct *) argus->dsrs[ARGUS_AGR_INDEX]) != NULL) {
         if (agr->count > 0) {
            sprintf (sum, "%.*f", parser->pflag, agr->act.meanval * agr->act.n);
         } else
            sprintf (sum, "%.*f", parser->pflag, 0.0);
      } else
         sprintf (sum, "%.*f", parser->pflag, 0.0);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " Run = \"%s\"", sum);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(sum);
      } else {
         if (strlen(sum) > len) {
            sum[len - 1] = '*';
            sum[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, sum);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintRunTime (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintMin (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusAgrStruct *agr = NULL;
   char minval[32];
 
   bzero (minval, 32);
   if (argus->hdr.type & ARGUS_MAR) {
   } else {
      if ((agr = (struct ArgusAgrStruct *) argus->dsrs[ARGUS_AGR_INDEX]) != NULL)
         sprintf (minval, "%.*f", parser->pflag, agr->act.minval);
   }
 
   if (parser->ArgusPrintXml) {
      sprintf (buf, " Min = \"%s\"", minval);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(minval);
      } else {
         if (strlen(minval) > len) {
            minval[len - 1] = '*';
            minval[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, minval);
   }
 
#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintMinDuration (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintMax (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusAgrStruct *agr = NULL;
   char maxval[32];

   bzero (maxval, 32);
   if (argus->hdr.type & ARGUS_MAR) {
   } else {
      if ((agr = (struct ArgusAgrStruct *) argus->dsrs[ARGUS_AGR_INDEX]) != NULL)
         sprintf (maxval, "%.*f", parser->pflag, agr->act.maxval);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " Max = \"%s\"", maxval);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(maxval);
      } else {
         if (strlen(maxval) > len) {
            maxval[len - 1] = '*';
            maxval[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, maxval);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintMax (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintStdDeviation (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusAgrStruct *agr = NULL;
   char stddev[32];
 
   bzero (stddev, 32);
   if (argus->hdr.type & ARGUS_MAR) {
   } else {
      if ((agr = (struct ArgusAgrStruct *) argus->dsrs[ARGUS_AGR_INDEX]) != NULL)
         sprintf (stddev, "%.*f", parser->pflag, agr->act.stdev);
      else
         ArgusPrintDuration (parser, stddev, argus, len);
   }
 
   if (parser->ArgusPrintXml) {
      sprintf (buf, " StdDev = \"%s\"", stddev);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(stddev);
      } else {
         if (strlen(stddev) > len) {
            stddev[len - 1] = '*';
            stddev[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, stddev);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintStdDeviation (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintIdleTime (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   float tidle = RaGetFloatIdleTime(argus);
   char idle[32];

   bzero (idle, 32);
   if (argus->hdr.type & ARGUS_MAR) {
   } else {
      if (tidle < 0) tidle = 0;
      sprintf (idle, "%.*f", parser->pflag, tidle);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " Idle = \"%s\"", idle);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(idle);
      } else {
         if (strlen(idle) > len) {
            idle[len - 1] = '*';
            idle[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, idle);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintIdleTime (%p, %p)", buf, argus);
#endif
}



void
ArgusPrintStartRange (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char ebuf[32];
   bzero (ebuf, sizeof(ebuf));

   if (argus->hdr.type & ARGUS_MAR) {
   } else {
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " StartRange = \"%s\"", ebuf);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ebuf);
      } else {
         if (strlen(ebuf) > len) {
            ebuf[len - 1] = '*';
            ebuf[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ebuf);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintStartRange (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintEndRange (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char ebuf[32];
   bzero (ebuf, sizeof(ebuf));

   if (argus->hdr.type & ARGUS_MAR) {
   } else {
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstRange = \"%s\"", ebuf);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ebuf);
      } else {
         if (strlen(ebuf) > len) {
            ebuf[len - 1] = '*';
            ebuf[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ebuf);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintEndRange (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintDuration (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   float fdur = RaGetFloatDuration (argus);
   char dur[128];

   bzero(dur, sizeof(dur));
   sprintf (dur, "%0.*f", parser->pflag, fdur);

   if (parser->ArgusPrintXml) {
      sprintf (buf, " Duration = \"%s\"", dur);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(dur);
      } else {
         if (strlen(dur) > len) {
            dur[len - 1] = '*';
            dur[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, dur);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDuration (0x%x, 0x%x, 0x%x)", parser, buf, argus);
#endif
}

void
ArgusPrintSrcDuration (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   float fdur = RaGetFloatSrcDuration (argus);
   char dur[128];

   bzero(dur, sizeof(dur));
   sprintf (dur, "%0.*f", parser->pflag, fdur);

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcDuration = \"%s\"", dur);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(dur);
      } else {
         if (strlen(dur) > len) {
            dur[len - 1] = '*';
            dur[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, dur);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcDuration (0x%x, 0x%x, 0x%x)", parser, buf, argus);
#endif
}

void
ArgusPrintDstDuration (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   float fdur = RaGetFloatDstDuration (argus);
   char dur[128];

   bzero(dur, sizeof(dur));
   sprintf (dur, "%0.*f", parser->pflag, fdur);

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstDuration = \"%s\"", dur);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(dur);
      } else {
         if (strlen(dur) > len) {
            dur[len - 1] = '*';
            dur[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, dur);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstDuration (0x%x, 0x%x, 0x%x)", parser, buf, argus);
#endif
}


void ArgusGetIndicatorString (struct ArgusParserStruct *parser, struct ArgusRecordStruct *, char *);

void
ArgusGetIndicatorString (struct ArgusParserStruct *parser, struct ArgusRecordStruct *argus, char *buf)
{
   int type = 0;
   bzero (buf, 16);

   bcopy ("          ", buf, 9);

   switch (argus->hdr.type & 0xF0) {
      case ARGUS_MAR:
      case ARGUS_EVENT:
         break;

      case ARGUS_NETFLOW:
      case ARGUS_FAR: {
         struct ArgusFlow *flow;
         struct ArgusMacStruct *mac;
         struct ArgusTimeObject *time;
         struct ArgusNetworkStruct *net;
         struct ArgusEncapsStruct *encaps;

         if ((argus->hdr.type & 0xF0) == ARGUS_NETFLOW)
            buf[0] = 'N';

         if ((time = (void *)argus->dsrs[ARGUS_TIME_INDEX]) != NULL)
            if (time->hdr.argus_dsrvl8.qual & ARGUS_TIMEADJUST)
               buf[0] = 'T';

         if ((encaps = (struct ArgusEncapsStruct *)argus->dsrs[ARGUS_ENCAPS_INDEX]) != NULL) {
            unsigned int i, types = encaps->src | encaps->dst, ind = 0;

            for (i = 0; i < ARGUS_ENCAPS_TYPE; i++) {
               if (types & (0x01 << i)) {
                  ind++;
                  switch (0x01 << i) {
                     case ARGUS_ENCAPS_ETHER:  buf[1] = 'e'; break;
                     case ARGUS_ENCAPS_LLC:    buf[1] = 'l'; break;
                     case ARGUS_ENCAPS_MPLS:   buf[1] = 'm'; break;
                     case ARGUS_ENCAPS_8021Q:  buf[1] = 'v'; break;
                     case ARGUS_ENCAPS_PPP:    buf[1] = 'p'; break;
                     case ARGUS_ENCAPS_ISL:    buf[1] = 'i'; break;
                     case ARGUS_ENCAPS_GRE:    buf[1] = 'G'; break;
                     case ARGUS_ENCAPS_AH:     buf[1] = 'a'; break;
                     case ARGUS_ENCAPS_IP:     buf[1] = 'P'; break;
                     case ARGUS_ENCAPS_IPV6:   buf[1] = '6'; break;
                     case ARGUS_ENCAPS_HDLC:   buf[1] = 'H'; break;
                     case ARGUS_ENCAPS_CHDLC:  buf[1] = 'C'; break;
                     case ARGUS_ENCAPS_ATM:    buf[1] = 'A'; break;
                     case ARGUS_ENCAPS_SLL:    buf[1] = 'S'; break;
                     case ARGUS_ENCAPS_FDDI:   buf[1] = 'F'; break;
                     case ARGUS_ENCAPS_SLIP:   buf[1] = 's'; break;
                     case ARGUS_ENCAPS_ARCNET: buf[1] = 'R'; break;
                     case ARGUS_ENCAPS_802_11: buf[1] = 'w'; break;
                     case ARGUS_ENCAPS_PRISM:  buf[1] = 'z'; break;
                     case ARGUS_ENCAPS_AVS:    buf[1] = 'a'; break;
                     case ARGUS_ENCAPS_TEREDO: buf[1] = 'T'; break;
                  }
               }
            }

            if (ind > 1)
               buf[1] = '*';

         } else {
            if (argus->dsrs[ARGUS_MPLS_INDEX] != NULL)
               buf[1] = 'm';
            if (argus->dsrs[ARGUS_MAC_INDEX] != NULL)
               buf[1] = 'e';
            if (argus->dsrs[ARGUS_VLAN_INDEX] != NULL)
               buf[1] = 'v';
         }

         if ((mac = (struct ArgusMacStruct *) argus->dsrs[ARGUS_MAC_INDEX]) != NULL) {
            if (mac->hdr.argus_dsrvl8.qual & ARGUS_MULTIPATH)
               buf[1] = 'M';
         }

         net = (struct ArgusNetworkStruct *) argus->dsrs[ARGUS_NETWORK_INDEX];
         flow = (struct ArgusFlow *)argus->dsrs[ARGUS_FLOW_INDEX];

         if (net && (net->hdr.subtype == ARGUS_UDT_FLOW)) {
            int status = net->net_union.udt.status;

            if (status & ARGUS_OUTOFORDER) {
               buf[3] = 'i'; 
            }
            if (status & (ARGUS_PKTS_RETRANS | ARGUS_PKTS_DROP)) {
               buf[3] = 's';
            }
            if (status & ARGUS_WINDOW_SHUT) {
               buf[4] = 'S'; 
            }
            if (status & ARGUS_ECN_CONGESTED) {
               buf[5] = 'E';
            }

         } else {
            if (flow != NULL) {
               switch (flow->hdr.subtype & 0x3F) {
                  case ARGUS_FLOW_CLASSIC5TUPLE: {
                     switch (type = (flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                        case ARGUS_TYPE_IPV4: {
                           struct ArgusIPAttrStruct *attr = (void *)argus->dsrs[ARGUS_IPATTR_INDEX];
                           if ((attr != NULL) && ((attr->hdr.argus_dsrvl8.qual & 
                                                   (ARGUS_IPATTR_SRC_FRAGMENTS | ARGUS_IPATTR_DST_FRAGMENTS)) ||
                                                   (flow->hdr.argus_dsrvl8.qual & ARGUS_FRAGMENT)))
                               buf[6] = 'F';

                           switch (flow->ip_flow.ip_p) {
                              case  IPPROTO_TCP: {
                                 if (net != NULL) {
                                    struct ArgusTCPObject *tcp = (struct ArgusTCPObject *)&net->net_union.tcp;
                                    unsigned int status = tcp->status;

                                    if (status & ARGUS_PKTS_RETRANS) {
                                       if ((status & ARGUS_SRC_PKTS_RETRANS) && (status & ARGUS_DST_PKTS_RETRANS))
                                          buf[3] =  '*';
                                       else {
                                          if (status & ARGUS_SRC_PKTS_RETRANS)
                                             buf[3] = 's';
                                          if (status & ARGUS_DST_PKTS_RETRANS)
                                             buf[3] = 'd';
                                       }
                                    } else
                                    if ((ArgusFetchSrcGap(argus) > 0) || (ArgusFetchDstGap(argus) > 0)) {
                                       buf[3] =  'g';
                                    } else
                                    if (status & ARGUS_OUTOFORDER) {
                                       if ((status & ARGUS_SRC_OUTOFORDER) && (status & ARGUS_DST_OUTOFORDER))
                                          buf[3] =  '&';
                                       else { 
                                          if (status & ARGUS_SRC_OUTOFORDER)
                                             buf[3] = 'i'; 
                                          if (status & ARGUS_DST_OUTOFORDER)
                                             buf[3] = 'r';
                                       }
                                    }
                                    if (status & ARGUS_WINDOW_SHUT) {
                                       if ((status & ARGUS_SRC_WINDOW_SHUT) && (status & ARGUS_DST_WINDOW_SHUT))
                                          buf[4] = '@';
                                       else {
                                          if (status & ARGUS_SRC_WINDOW_SHUT)
                                             buf[4] = 'S'; 
                                          if (status & ARGUS_DST_WINDOW_SHUT)
                                             buf[4] = 'D';
                                       }
                                    }
                                    if (status & ARGUS_ECN_CONGESTED) {
                                       if ((status & ARGUS_SRC_CONGESTED) && (status & ARGUS_DST_CONGESTED))
                                          buf[5] = 'E';
                                       else { 
                                          if (status & ARGUS_SRC_CONGESTED)
                                             buf[5] = 'x';
                                          if (status & ARGUS_DST_CONGESTED)
                                             buf[5] = 't';
                                       }
                                    }
                                 }
                                 break;
                              }

                              case IPPROTO_UDP: {
                                 if (net != NULL) {
                                    switch (net->hdr.subtype) {
                                       case ARGUS_RTP_FLOW: {
                                          struct ArgusRTPObject *rtp = &net->net_union.rtp;
                                          if (rtp->sdrop && rtp->ddrop) {
                                             buf[3] =  '*';
                                          } else {
                                             if (rtp->sdrop)
                                                buf[3] = 's';
                                             if (rtp->ddrop)
                                                buf[3] = 'd';
                                          }
                                          break;
                                       }
                                       case ARGUS_RTCP_FLOW:
                                          break;
                                    }
                                 }
                                 break;
                              }

                              default:          
                              case IPPROTO_ICMP:
                                 break;

                              case IPPROTO_ESP: {
                                 if (net != NULL) {
                                    unsigned int status = net->net_union.esp.status;
                                    if ((status & ARGUS_PKTS_DROP) && (net->net_union.esp.lostseq)) {
                                       if ((status & ARGUS_SRC_PKTS_DROP) && (status & ARGUS_DST_PKTS_DROP))
                                          buf[3] =  '*';
                                       else {
                                          if (status & ARGUS_SRC_PKTS_DROP)
                                             buf[3] = 's';
                                          if (status & ARGUS_DST_PKTS_DROP)
                                             buf[3] = 'd';
                                       }
                                    }
                                    if (status & ARGUS_OUTOFORDER) {
                                       if ((status & ARGUS_SRC_OUTOFORDER) && (status & ARGUS_DST_OUTOFORDER))
                                          buf[3] =  '&';
                                       else { 
                                          if (status & ARGUS_SRC_OUTOFORDER)
                                             buf[3] = 'i'; 
                                          if (status & ARGUS_DST_OUTOFORDER)
                                             buf[3] = 'r';
                                       }
                                    }
                                 }
                                 break;
                              }
                           }
                           break;
                        }
       
                        case ARGUS_TYPE_IPV6: {
                           struct ArgusIPAttrStruct *ipattr = (void *)argus->dsrs[ARGUS_IPATTR_INDEX];
                           if (((ipattr != NULL) && (ipattr->hdr.argus_dsrvl8.qual & (ARGUS_IPATTR_SRC_FRAGMENTS | ARGUS_IPATTR_DST_FRAGMENTS))) ||
                                (flow->hdr.argus_dsrvl8.qual & ARGUS_FRAGMENT))
                              buf[6] = 'F';

                           switch (flow->ipv6_flow.ip_p) {
                              case  IPPROTO_TCP: {
                                 if (net != NULL) {
                                    struct ArgusTCPObject *tcp = (struct ArgusTCPObject *)&net->net_union.tcp;
                                    if (tcp->src.status & ARGUS_PKTS_RETRANS) {
                                       if ((tcp->status & ARGUS_SRC_PKTS_RETRANS) && (tcp->status & ARGUS_DST_PKTS_RETRANS))
                                          buf[3] =  '*';
                                       else {
                                          if (tcp->status & ARGUS_SRC_PKTS_RETRANS)
                                             buf[3] = 's';
                                          if (tcp->status & ARGUS_DST_PKTS_RETRANS)
                                             buf[3] = 'd';
                                       }
                                    }
                                 }
                                 break;
                              }
                              case IPPROTO_UDP: {
                                 if (net != NULL) {
                                    switch (net->hdr.subtype) {
                                       case ARGUS_RTP_FLOW: {
                                          struct ArgusRTPObject *rtp = &net->net_union.rtp;
                                          if (rtp->sdrop && rtp->ddrop) {
                                             buf[3] =  '*';
                                          } else {
                                             if (rtp->sdrop)
                                                buf[3] = 's';
                                             if (rtp->ddrop)
                                                buf[3] = 'd';
                                          }
                                          break;
                                       }
                                       case ARGUS_RTCP_FLOW:
                                          break;
                                    }
                                 }
                                 break;
                              }
                              case IPPROTO_ICMP:
                                 break;
                              case IPPROTO_IGMP:
                                 break;
                              default:          
                                 break;
                           }

                           break;
                        }
                     }
                     break;
                  }
                  case ARGUS_FLOW_ARP: {
                     break;
                  }
               }
            }
         }


         if (argus->dsrs[ARGUS_ICMP_INDEX] != NULL) {
            struct ArgusIcmpStruct *icmp = (void *)argus->dsrs[ARGUS_ICMP_INDEX];
            if (icmp->hdr.argus_dsrvl8.qual & ARGUS_ICMP_MAPPED) {
               buf[2] = 'I';
               switch (icmp->icmp_type) {
                  case ICMP_UNREACH:  buf[2] = 'U'; break;
                  case ICMP_REDIRECT: buf[2] = 'R'; break;
                  case ICMP_TIMXCEED: buf[2] = 'T'; break;
               }
            }
         }

         if (net != NULL) {
            switch (net->hdr.subtype) { 
               case ARGUS_NETWORK_SUBTYPE_FRAG:
                  buf[6] = 'f';

                  if (net->hdr.argus_dsrvl8.qual & ARGUS_FRAGOVERLAP) {
                     switch (type = (flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                        default:
                           buf[6] = 'V';
                           break;
                     }
                  }
                  break;

               case ARGUS_RTP_FLOW:
                  if (net->hdr.argus_dsrvl8.qual & (ARGUS_RTP_SRCSILENCE | ARGUS_RTP_DSTSILENCE)) {
                     if ((net->hdr.argus_dsrvl8.qual & (ARGUS_RTP_SRCSILENCE | ARGUS_RTP_DSTSILENCE)) == 
                                                       (ARGUS_RTP_SRCSILENCE | ARGUS_RTP_DSTSILENCE))
                        buf[4] = '*';
                     if (net->hdr.argus_dsrvl8.qual & ARGUS_RTP_DSTSILENCE)
                        buf[4] = 's';
                     if (net->hdr.argus_dsrvl8.qual & ARGUS_RTP_DSTSILENCE)
                        buf[4] = 'd';
                  }
                  break;
            }
         }

         if (argus->dsrs[ARGUS_IPATTR_INDEX]) {
            struct ArgusIPAttrStruct *attr = (struct ArgusIPAttrStruct *)argus->dsrs[ARGUS_IPATTR_INDEX];
            unsigned char options = attr->src.options | attr->dst.options;
            if (attr) {
               switch (options) {
                  case ARGUS_RTRALERT:    buf[7] = 'A'; break;
                  case ARGUS_TIMESTAMP:   buf[7] = 'T'; break;
                  case ARGUS_RECORDROUTE: buf[7] = 'R'; break;
                  case ARGUS_SECURITY:    buf[7] = '+'; break;
                  case ARGUS_LSRCROUTE:   buf[7] = 'L'; break;
                  case ARGUS_SSRCROUTE:   buf[7] = 'S'; break;
                  case ARGUS_SATID:       buf[7] = 'D'; break;
                  default:  {
                     unsigned char v = options, c;
                     for (c = 0; v; c++) 
                       v &= v - 1;
                     if (c > 1)
                        buf[7] = 'O';
                     else
                        buf[7] = 'U';
                     break;
                  }
                  case 0:                 break;
               }
            }
         }
         if ((argus->correlates != NULL) || (argus->dsrs[ARGUS_COR_INDEX])) {
            struct ArgusCorrelateStruct *cor = (void *)argus->dsrs[ARGUS_COR_INDEX];
            if (argus->correlates != NULL)
               sprintf(&buf[8], "%d",  argus->correlates->count);
            if (cor != NULL) {
               int count = (cor->hdr.argus_dsrvl8.len - 1)/(sizeof(struct ArgusCorMetrics)/4);
               sprintf(&buf[8], "%d", count);
            }
         }
      }
      break;
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusGetIndicatorString (0x%x, 0x%x)", argus, buf);
#endif
   return;
}


char argus_strbuf[MAXSTRLEN];
u_short ArgusThisProto;

void
ArgusPrintSourceID (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char strbuf[64], *value = strbuf;

   if (argus->hdr.type & ARGUS_MAR) {
      struct ArgusRecord *rec = (struct ArgusRecord *) argus->dsrs[0];

      if (rec != NULL) {
         void *pid;

         switch (argus->hdr.cause & 0xF0) {
            case ARGUS_START:     pid = &rec->argus_mar.thisid; break;
            default:
            case ARGUS_STATUS:
            case ARGUS_STOP:
            case ARGUS_SHUTDOWN:
            case ARGUS_ERROR:     pid = &rec->argus_mar.argusid; break;
         }

         switch (rec->argus_mar.status & (ARGUS_IDIS_STRING | ARGUS_IDIS_INT | ARGUS_IDIS_IPV4)) {
            case ARGUS_IDIS_STRING: value = ArgusGetString(parser, (u_char *)pid, 4); break;
            case ARGUS_TYPE_INT: snprintf (value, sizeof(strbuf), "%d", *(unsigned int *)pid); break; 
            default:
            case ARGUS_IDIS_IPV4: value = ArgusGetName(parser, pid); break;
         }
      }

   } else {
      struct ArgusTransportStruct *trans = (struct ArgusTransportStruct *) argus->dsrs[ARGUS_TRANSPORT_INDEX];
      if (trans != NULL) {
         switch (trans->hdr.argus_dsrvl8.qual) {
            case ARGUS_TYPE_INT:    snprintf (value, sizeof(strbuf), "%d", trans->srcid.a_un.value); break;
            case ARGUS_TYPE_STRING: value = ArgusGetString(parser, (u_char *)&trans->srcid.a_un.str, 4); break;

            default:
            case ARGUS_TYPE_IPV4:   value =   ArgusGetName(parser, (u_char *)&trans->srcid.a_un.ipv4); break;
/*
            case ARGUS_TYPE_IPV6:   value = ArgusGetV6Name(parser, (u_char *)&trans->srcid.ipv6); break;
            case ARGUS_TYPE_ETHER:  value = ArgusGetEtherName(parser, (u_char *)&trans->srcid.ether); break;
*/
         }
      }
   }

   if (value == NULL)
      value = "";

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SourceId = \"%s\"", value);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(value);
      } else {
         if (strlen(value) > len) {
            value[len - 1] = '*';
            value[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, value);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSourceID (%p, %p)", buf, argus);
#endif
}

void ArgusPrintRank (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintBinNumber (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);
void ArgusPrintBins (struct ArgusParserStruct *parser, char *, struct ArgusRecordStruct *, int);

void
ArgusPrintRank (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus , int len)
{
   char rank[32];

   bzero (rank, sizeof(rank));
   if (argus->rank > 0) {
      sprintf (rank, "%d", argus->rank);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " Rank = \"%s\"", rank);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(rank);
      } else {
         if (strlen(rank) > len) {
            rank[len - 1] = '*';
            rank[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, rank);
   }
}

void
ArgusPrintBinNumber (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus , int len)
{
   char binbuf[32];

   bzero (binbuf, sizeof(binbuf));

   if (parser->ArgusPrintXml) {
      sprintf (buf, " BinNum = \"%s\"", binbuf);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(binbuf);
      } else {
         if (strlen(binbuf) > len) {
            binbuf[len - 1] = '*';
            binbuf[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, binbuf);
   }
}

void
ArgusPrintBins (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char binbuf[32];

   bzero (binbuf, sizeof(binbuf));

   if (parser->ArgusPrintXml) {
      sprintf (buf, " Bins = \"%s\"", binbuf);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(binbuf);
      } else {
         if (strlen(binbuf) > len) {
            binbuf[len - 1] = '*';
            binbuf[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, binbuf);
   }
}

void
ArgusPrintSequenceNumber (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char value[128];

   bzero(value, sizeof(value));

   if (argus->hdr.type & ARGUS_MAR) {
      struct ArgusRecord *rec = (struct ArgusRecord *) argus->dsrs[0];
      if (rec != NULL)
         sprintf (value, "%u", rec->argus_mar.nextMrSequenceNum);
 
   } else {
      struct ArgusTransportStruct *trans;

      if ((trans = (void *)argus->dsrs[ARGUS_TRANSPORT_INDEX]) != NULL)
         sprintf (value, "%u", trans->seqnum);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SeqNumber = \"%s\"", value);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(value);
      } else {
         if (strlen(value) > len) {
            value[len - 1] = '*';
            value[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, value);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSequenceNumber (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintFlags (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char flags[32];
   bzero (flags, 32);
   ArgusGetIndicatorString (parser, argus, flags);

   if (parser->ArgusPrintXml) {
      char xmlflags[64];
      int i, len = strlen(flags);

      bzero(xmlflags, sizeof(xmlflags));
      for (i = 0; i < len; i++) {
         if (flags[i] == '&') {
            sprintf(&xmlflags[strlen(xmlflags)], "&amp;");
         } else
            xmlflags[strlen(xmlflags)] = flags[i];
      }

      sprintf (buf, " Flags = \"%s\"", xmlflags);

   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(flags);
      } else {
         if (strlen(flags) > len) {
            flags[len - 1] = '*';
            flags[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, flags);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintFlags (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintSrcMacAddress (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusMacStruct *mac = (struct ArgusMacStruct *) argus->dsrs[ARGUS_MAC_INDEX];
   char *macstr = NULL;

   if (mac != NULL) {
      switch (mac->hdr.subtype & 0x3F) {
         default:
         case ARGUS_TYPE_ETHER:
            macstr = etheraddr_string (parser, (unsigned char *)&mac->mac.mac_union.ether.ehdr.ether_shost);
            break;
      }
   }

   if (macstr == NULL)
      macstr = "";

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcMacAddr = \"%s\"", macstr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(macstr);
      } else {
         if (strlen(macstr) > len) {
            macstr[len - 1] = '*';
            macstr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, macstr);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcMacAddress (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintDstMacAddress (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusMacStruct *mac = (struct ArgusMacStruct *) argus->dsrs[ARGUS_MAC_INDEX];
   char *macstr = NULL;

   if (mac != NULL) {
      switch (mac->hdr.subtype & 0x3F) {
         default:
         case ARGUS_TYPE_ETHER:
            macstr = etheraddr_string (parser, (unsigned char *)&mac->mac.mac_union.ether.ehdr.ether_dhost);
            break;
      }
   }

   if (macstr == NULL)
      macstr = "";
   
   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstMacAddr = \"%s\"", macstr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(macstr);
      } else {
         if (strlen(macstr) > len) {
            macstr[len - 1] = '*';
            macstr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, macstr);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstMacAddress (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintSrcOui (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusMacStruct *mac = (struct ArgusMacStruct *) argus->dsrs[ARGUS_MAC_INDEX];
   char *oui = NULL;

   if (mac != NULL) {
      switch (mac->hdr.subtype & 0x3F) {
         default:
         case ARGUS_TYPE_ETHER:
            oui = etheraddr_oui (parser, (unsigned char *)&mac->mac.mac_union.ether.ehdr.ether_shost);
            break;
      }
   }

   if (oui == NULL)
      oui = "";

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcOui = \"%s\"", oui);
   } else {
      char strbuf[16];
      strncpy(strbuf, oui, 16);

      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(strbuf);
      } else {
         if (strlen(strbuf) > len) {
            strbuf[len - 1] = '*';
            strbuf[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, strbuf);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcOui (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintDstOui (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusMacStruct *mac = (struct ArgusMacStruct *) argus->dsrs[ARGUS_MAC_INDEX];
   char *oui = NULL;

   if (mac != NULL) {
      switch (mac->hdr.subtype & 0x3F) {
         default:
         case ARGUS_TYPE_ETHER: {
            oui = etheraddr_oui (parser, (unsigned char *)&mac->mac.mac_union.ether.ehdr.ether_dhost);
            break;
         }
      }
   }

   if (oui == NULL)
      oui = "";

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstOui = \"%s\"", oui);
   } else {
      char strbuf[16];
      strncpy(strbuf, oui, 16);

      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(strbuf);
      } else {
         if (strlen(strbuf) > len) {
            strbuf[len - 1] = '*';
            strbuf[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, strbuf);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstOui (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintProto (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusFlow *flow;
   char protoStrBuf[16], *protoStr = NULL;
   u_short eproto;
   u_char proto; 
 
   bzero (protoStrBuf, sizeof(protoStrBuf));
   protoStr = protoStrBuf;
    
   switch (argus->hdr.type & 0xF0) {
      case ARGUS_MAR:
         sprintf (protoStrBuf, "man");
         break;

      case ARGUS_EVENT:
         sprintf (protoStrBuf, "evt");
         break;
         
      case ARGUS_NETFLOW:
      case ARGUS_FAR: {
         if (((flow = (void *)argus->dsrs[ARGUS_FLOW_INDEX]) != NULL)) {
            switch (flow->hdr.subtype & 0x3F) {
               case ARGUS_FLOW_CLASSIC5TUPLE: {
                  struct ArgusNetworkStruct *net = (struct ArgusNetworkStruct *)argus->dsrs[ARGUS_NETWORK_INDEX];

                     switch ((flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                        case ARGUS_TYPE_IPV4:
                           switch (proto = flow->ip_flow.ip_p) {
                              case IPPROTO_UDP: {
                                 if (parser->nflag > 2) {
                                    sprintf (protoStr, "%u", proto); 
                                    break;
                                 } else {
                                    if (net && (net->hdr.subtype == ARGUS_RTP_FLOW)) {
                                       protoStr = "rtp";
                                       break;
                                    } else 
                                    if (net && (net->hdr.subtype == ARGUS_RTCP_FLOW)) {
                                       protoStr = "rtcp";
                                       break;
                                    } else
                                    if (net && (net->hdr.subtype == ARGUS_UDT_FLOW)) {
                                       protoStr = "udt";
                                       break;
                                    }
                                 }
                              }
                              default:
                                 if (ip_proto_string[proto] == NULL)
                                    ip_proto_string[proto] = "unas";

                                 if (parser->nflag > 2)
                                    sprintf (protoStr, "%u", proto); 
                                 else
                                    sprintf (protoStr, "%s", ip_proto_string[proto]); 
                                 break;
                           }
                           break;

                        case ARGUS_TYPE_IPV6:
                           switch (proto = flow->ipv6_flow.ip_p) {
                              case IPPROTO_UDP: {
                                 if (parser->nflag > 2) {
                                    sprintf (protoStr, "%u", proto); 
                                    break;
                                 } else {
                                    if (net && (net->hdr.subtype == ARGUS_RTP_FLOW)) {
                                       protoStr = "rtp";
                                       break;
                                    } else {
                                       if (net && (net->hdr.subtype == ARGUS_RTCP_FLOW)) {
                                          protoStr = "rtcp";
                                          break;
                                       }
                                    }
                                 }
                              }
                              default:
                                 if (ip_proto_string[proto] == NULL)
                                    ip_proto_string[proto] = "unas";

                                 protoStr = protoStrBuf;
                                 if (parser->nflag > 2)
                                    sprintf (protoStr, "%u", proto); 
                                 else
                                    sprintf (protoStr, "%s", ip_proto_string[proto]); 
                                 break;
                           }
                           break;

                        case ARGUS_TYPE_RARP:
                           protoStr = (parser->nflag > 2) ? "" : "rarp";
                           break;
                        case ARGUS_TYPE_ARP:
                           protoStr = (parser->nflag > 2) ? "2054" : "arp";
                           break;
    
                        case ARGUS_TYPE_ISIS:
                           protoStr = "isis"; break;
                           break;

                        case ARGUS_TYPE_WLAN:
                           protoStr = "wlan"; break;
                           break;

                        case ARGUS_TYPE_ETHER:
                           eproto = flow->mac_flow.mac_union.ether.ehdr.ether_type;
                           if (parser->nflag > 2) {
                              sprintf (protoStr, "%u", eproto); 
                           }  else { 
                              char *pstr = ArgusEtherProtoString(parser, eproto);
                              if (pstr !=  NULL) {
                                 sprintf (protoStr, "%s", pstr); 
                              } else
                                 sprintf (protoStr, "%u", eproto); 
                           }
                           break;
                     }
                  break;
               }

               case ARGUS_FLOW_ARP: {
                  switch ((flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                     case ARGUS_TYPE_RARP:
                        protoStr = (parser->nflag > 2) ? "" : "rarp";
                        break;
                     case ARGUS_TYPE_ARP:
                        protoStr = (parser->nflag > 2) ? "2054" : "arp";
                        break;
                  }
               }

               default:
                  switch ((flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                     case ARGUS_TYPE_IPV4:
                     case ARGUS_TYPE_IPV6:
                        protoStr = "ip ";
                        break;
                     case ARGUS_TYPE_RARP:
                        protoStr = "rarp";
                        break;
                     case ARGUS_TYPE_ARP:
                        protoStr = "arp";
                        break;
                     case ARGUS_TYPE_WLAN:
                        protoStr = "wlan";
                        break;
                     case ARGUS_TYPE_ETHER:
                        protoStr = "ether";
                        break;
                  }
                  break;
            }
         }
      }
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " Proto = \"%s\"", protoStr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(protoStr);
      } else {
         if (strlen(protoStr) > len) {
            protoStr[len - 1] = '*';
            protoStr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, protoStr);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintProto (%p, %p)", buf, argus);
#endif
}

int ArgusPrintNet = 0;

void
ArgusPrintSrcNet (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusFlow *flow;
   unsigned int naddr;
   void *addr = NULL;
   int objlen, type = 0;
   char masklen = 32;

   if (argus->hdr.type & ARGUS_MAR) {
      struct ArgusRecord *rec = (struct ArgusRecord *) argus->dsrs[0];
      unsigned int value = 0;
      char pbuf[32];

      if (rec != NULL) {
         value = rec->argus_mar.queue;
         sprintf (pbuf, "%u", value);
      } else
         bzero(pbuf, sizeof(pbuf));

      if (parser->ArgusPrintXml) {
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
            len = strlen(pbuf);
         } else {
            if (strlen(pbuf) > len) {
               pbuf[len - 1] = '*';
               pbuf[len]     = '\0';
            }
         }
         sprintf (buf, "%*.*s ", len, len, pbuf);
      }
 
   } else {
      if (((flow = (void *)argus->dsrs[ARGUS_FLOW_INDEX]) != NULL)) {
         switch (flow->hdr.subtype & 0x3F) {

            case ARGUS_FLOW_CLASSIC5TUPLE: 
            case ARGUS_FLOW_LAYER_3_MATRIX: {
               switch (type = (flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                  case ARGUS_TYPE_IPV4:
                     naddr = flow->ip_flow.ip_src;
                     naddr &= ipaddrtonetmask(naddr);
                     addr = &naddr;
                     objlen = 4;
                     break;
                  case ARGUS_TYPE_IPV6:
                     addr = &flow->ipv6_flow.ip_src;
                     objlen = 16;
                     break;

                  case ARGUS_TYPE_RARP: {
                     type = ARGUS_TYPE_ETHER;
                     addr = &flow->lrarp_flow.tareaddr;
                     objlen = 6;
                     break;
                  }
                  case ARGUS_TYPE_ARP: {
                     type = ARGUS_TYPE_IPV4;
                     addr = &flow->larp_flow.arp_spa;
                     objlen = 4;
                     break;
                  }
                  case ARGUS_TYPE_ETHER:
                     addr = &flow->mac_flow.mac_union.ether.ehdr.ether_shost;
                     objlen = 6;
                     break;
                  case ARGUS_TYPE_WLAN:
                     addr = &flow->wlan_flow.shost;
                     objlen = 6;
                     break;
               }
               break;
            }

            case ARGUS_FLOW_ARP: {
               switch (type = (flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                  case ARGUS_TYPE_RARP:
                     type = ARGUS_TYPE_ETHER;
                     addr = &flow->rarp_flow.shaddr;
                     objlen = 6;
                     break;
                  case ARGUS_TYPE_ARP:
                     type = ARGUS_TYPE_IPV4;
                     addr = &flow->arp_flow.haddr;
                     objlen = 4;
                     break;
               }
               break;
            }
 
            default:
               break;
         }
      } 

      ArgusPrintAddr (parser, buf, type, addr, objlen, masklen, len, ARGUS_SRC);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcNet (%p, %p)", buf, argus);
#endif
}


#if !defined(ETHER_ADDR_LEN)
#define ETHER_ADDR_LEN		6
#endif
 
#define SYSTEM_ID_LEN   ETHER_ADDR_LEN
#define NODE_ID_LEN     SYSTEM_ID_LEN+1
#define LSP_ID_LEN      SYSTEM_ID_LEN+2

void
ArgusPrintSrcAddr (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusFlow *flow;
   void *addr = NULL;
   int objlen, type = 0;
   char masklen = 32;

   switch (argus->hdr.type & 0xF0) {
      case ARGUS_MAR: {
         struct ArgusRecord *rec = (struct ArgusRecord *) argus->dsrs[0];
         unsigned int value;
         char pbuf[32];

         if (rec != NULL) {
            value = rec->argus_mar.queue;
            sprintf (pbuf, "%u", value);
         } else
            bzero(pbuf, sizeof(pbuf));

         if (parser->ArgusPrintXml) {
         } else {
            if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
               len = strlen(pbuf);
            } else {
               if (strlen(pbuf) > len) {
                  pbuf[len - 1] = '*';
                  pbuf[len]     = '\0';
               }
            }
            sprintf (buf, "%*.*s ", len, len, pbuf);
         }
         break;
      }

      case ARGUS_EVENT: {
         struct ArgusTransportStruct *trans = (void *) argus->dsrs[ARGUS_TRANSPORT_INDEX];
         char strbuf[64], *value = strbuf;

         if (trans != NULL) {
            switch (trans->hdr.argus_dsrvl8.qual) {
               case ARGUS_TYPE_INT:    {
                  snprintf (value, sizeof(strbuf), "%d", trans->srcid.a_un.value);
                  value = strbuf;
                  break;
               }
               case ARGUS_TYPE_IPV4: {
                  if (parser->RaPrintAlgorithmList[parser->RaPrintIndex] != NULL) {
                     char *format = parser->RaPrintAlgorithmList[parser->RaPrintIndex]->format;

                     if ((format != NULL) && (strlen(format) > 0)) {
                        unsigned int naddr = *(unsigned int *)&trans->srcid.a_un.ipv4;
                        snprintf (strbuf, 64, format, naddr);
                        value = strbuf;

                     } else
                        value = ArgusGetName(parser, (u_char *)&trans->srcid.a_un.ipv4);
                  }
                  break;
               }

               case ARGUS_TYPE_STRING: value = ArgusGetString(parser, (u_char *)&trans->srcid.a_un.str, 4); break;
/*
               case ARGUS_TYPE_IPV6:   value = ArgusGetV6Name(parser, (u_char *)&trans->srcid.ipv6); break;
               case ARGUS_TYPE_ETHER:  value = ArgusGetEtherName(parser, (u_char *)&trans->srcid.ether); break;
*/
            }
         }

         if (parser->ArgusPrintXml) {
            sprintf (buf, " SrcAddr = \"%s\"", value);
         } else {
            switch (parser->RaFieldWidth) {
               case RA_FIXED_WIDTH:
                  if (strlen(value) > len) {
                     value[len - 1] = '*';
                     value[len]     = '\0';
                  }
                  sprintf (buf, "%*.*s ", len, len, value);
                  break;
               default:
                  sprintf (buf, "%s ", value);
                  break;
            }
         }

         break;
      }

      case ARGUS_NETFLOW:
      case ARGUS_FAR: {
         if (((flow = (void *)argus->dsrs[ARGUS_FLOW_INDEX]) != NULL)) {
            switch (flow->hdr.subtype & 0x3F) {

               case ARGUS_FLOW_CLASSIC5TUPLE: 
               case ARGUS_FLOW_LAYER_3_MATRIX: {
                  switch (type = (flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                     case ARGUS_TYPE_IPV4:
                        addr = &flow->ip_flow.ip_src;
                        if (flow->hdr.argus_dsrvl8.qual & ARGUS_FRAGMENT)
                           masklen = 32;
                        else
                           masklen = flow->ip_flow.smask;
                        objlen = 4;
                        break;
                     case ARGUS_TYPE_IPV6:
                        addr = &flow->ipv6_flow.ip_src;
                        objlen = 16;
                        break;

                     case ARGUS_TYPE_RARP:
                        type = ARGUS_TYPE_ETHER;
                        addr = &flow->lrarp_flow.tareaddr;
                        objlen = 6;
                        break;
                     case ARGUS_TYPE_ARP:
                        type = ARGUS_TYPE_IPV4;
                        addr = &flow->larp_flow.arp_spa;
                        objlen = 4;
                        break;
                     case ARGUS_TYPE_ETHER:
                        addr = &flow->mac_flow.mac_union.ether.ehdr.ether_shost;
                        objlen = 6;
                        break;
                     case ARGUS_TYPE_WLAN:
                        type = ARGUS_TYPE_ETHER;
                        addr = &flow->wlan_flow.shost;
                        objlen = 6;
                        break;

                     case ARGUS_TYPE_ISIS: {
                        type = ARGUS_TYPE_ISIS;
                        switch (flow->isis_flow.pdu_type) {
                           case L1_LAN_IIH:
                           case L2_LAN_IIH:
                              type = ARGUS_TYPE_ISIS;
                              addr = &flow->isis_flow.isis_un.hello.srcid;
                              objlen = SYSTEM_ID_LEN;
                              break;

                           case L1_CSNP:
                           case L2_CSNP:
                              type = ARGUS_TYPE_ISIS;
                              addr = &flow->isis_flow.isis_un.csnp.srcid;
                              objlen = NODE_ID_LEN;
                              break;

                           case L1_PSNP:
                           case L2_PSNP:
                              type = ARGUS_TYPE_ISIS;
                              addr = &flow->isis_flow.isis_un.psnp.srcid;
                              objlen = NODE_ID_LEN;
                              break;

                           case L1_LSP:
                           case L2_LSP:
                              type = ARGUS_TYPE_ISIS;
                              addr = &flow->isis_flow.isis_un.lsp.lspid;
                              objlen = LSP_ID_LEN;
                              break;
                        }
                        break;
                     }
                  }
                  break;
               }

               case ARGUS_FLOW_ARP: {
                  switch (type = (flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                     case ARGUS_TYPE_RARP:
                        type = ARGUS_TYPE_ETHER;
                        addr = &flow->rarp_flow.dhaddr;
                        objlen = 6;
                        break;

                     case ARGUS_TYPE_ARP:
                        type = ARGUS_TYPE_IPV4;
                        addr = &flow->arp_flow.arp_spa;
                        objlen = 4;
                        break;
                  }
                  break;
               }

               default:
                  break;
            }
         } 

         ArgusPrintAddr (parser, buf, type, addr, objlen, masklen, len, ARGUS_SRC);
         break;
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcAddr (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintDstNet (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusFlow *flow;
   unsigned int naddr;
   void *addr = NULL;
   int objlen, type = 0;
   char masklen = 32;

   if (argus->hdr.type & ARGUS_MAR) {
      struct ArgusRecord *rec = (struct ArgusRecord *) argus->dsrs[0];
      unsigned int value = 0;
      char pbuf[32];

      if (rec != NULL) {
         value = rec->argus_mar.queue;
         sprintf (pbuf, "%u", value);
      } else
         bzero(pbuf, sizeof(pbuf));

      if (parser->ArgusPrintXml) {
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
            len = strlen(pbuf);
         } else {
            if (strlen(pbuf) > len) {
               pbuf[len - 1] = '*';
               pbuf[len]     = '\0';
            }
         }
         sprintf (buf, "%*.*s ", len, len, pbuf);
      }
 
   } else {
      if (((flow = (void *)argus->dsrs[ARGUS_FLOW_INDEX]) != NULL)) {
         switch (flow->hdr.subtype & 0x3F) {

            case ARGUS_FLOW_CLASSIC5TUPLE: 
            case ARGUS_FLOW_LAYER_3_MATRIX: {
               switch (type = (flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                  case ARGUS_TYPE_IPV4:
                     naddr = flow->ip_flow.ip_dst;
                     naddr &= ipaddrtonetmask(naddr);
                     addr = &naddr;
                     objlen = 4;
                     break;
                  case ARGUS_TYPE_IPV6:
                     addr = &flow->ipv6_flow.ip_dst;
                     objlen = 16;
                     break;
                  case ARGUS_TYPE_RARP:
                     type = ARGUS_TYPE_ETHER;
                     addr = &flow->lrarp_flow.srceaddr;
                     objlen = 6;
                     break;
                  case ARGUS_TYPE_ARP:
                     type = ARGUS_TYPE_IPV4;
                     addr = &flow->larp_flow.arp_tpa;
                     objlen = 4;
                     break;
                  case ARGUS_TYPE_ETHER:
                     addr = &flow->mac_flow.mac_union.ether.ehdr.ether_dhost;
                     objlen = 6;
                     break;
                  case ARGUS_TYPE_WLAN:
                     type = ARGUS_TYPE_ETHER;
                     addr = &flow->wlan_flow.dhost;
                     objlen = 6;
                     break;
               }
               break;
            }
            case ARGUS_FLOW_ARP: {
               switch (type = (flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                  case ARGUS_TYPE_RARP:
                     type = ARGUS_TYPE_ETHER;
                     addr = &flow->rarp_flow.shaddr;
                     objlen = 6;
                     break;

                  case ARGUS_TYPE_ARP:
                     type = ARGUS_TYPE_IPV4;
                     addr = &flow->arp_flow.haddr;
                     objlen = 4;
                     break;
               }
               break;
            }

            default:
               break;
         }
      } 

      ArgusPrintAddr (parser, buf, type, addr, objlen, masklen, len, ARGUS_DST);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstNet (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintDstAddr (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusFlow *flow;
   void *addr = NULL;
   int objlen, type = 0;
   char masklen = 32;

   switch (argus->hdr.type & 0xF0) {
      case ARGUS_MAR: {
         struct ArgusRecord *rec = (struct ArgusRecord *) argus->dsrs[0];
         unsigned int value = 0;
         char pbuf[32];
         if (rec != NULL) {
            value = rec->argus_mar.bufs;
            sprintf (pbuf, "%u", value);
         } else
            bzero(pbuf, sizeof(pbuf));

         if (parser->ArgusPrintXml) {
         } else {
            if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
               len = strlen(pbuf);
            } else {
               if (strlen(pbuf) > len) {
                  pbuf[len - 1] = '*';
                  pbuf[len]     = '\0';
               }
            }
            sprintf (buf, "%*.*s ", len, len, pbuf);
         }
         break;
      }

      case ARGUS_EVENT: {
         break;
      }

      case ARGUS_NETFLOW:
      case ARGUS_FAR: {
         if (((flow = (void *)argus->dsrs[ARGUS_FLOW_INDEX]) != NULL)) {
            switch (flow->hdr.subtype & 0x3F) {
               case ARGUS_FLOW_CLASSIC5TUPLE: 
               case ARGUS_FLOW_LAYER_3_MATRIX: {
                  switch (type = (flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                     case ARGUS_TYPE_IPV4:
                        addr = &flow->ip_flow.ip_dst;
                        if (flow->hdr.argus_dsrvl8.qual & ARGUS_FRAGMENT)
                           masklen = 32;
                        else
                           masklen = flow->ip_flow.dmask;
                        objlen = 4;
                        break;
                     case ARGUS_TYPE_IPV6:
                        addr = &flow->ipv6_flow.ip_dst;
                        objlen = 16;
                        break;
                     case ARGUS_TYPE_RARP:
                        type = ARGUS_TYPE_ETHER;
                        addr = &flow->lrarp_flow.srceaddr;
                        objlen = 6;
                        break;
                     case ARGUS_TYPE_ARP:
                        type = ARGUS_TYPE_IPV4;
                        addr = &flow->larp_flow.arp_tpa;
                        objlen = 4;
                        break;
                     case ARGUS_TYPE_ETHER:
                        addr = &flow->mac_flow.mac_union.ether.ehdr.ether_dhost;
                        objlen = 6;
                        break;
                     case ARGUS_TYPE_WLAN:
                        type = ARGUS_TYPE_ETHER;
                        addr = &flow->wlan_flow.dhost;
                        objlen = 6;
                        break;

                     case ARGUS_TYPE_ISIS: {
                        switch (flow->isis_flow.pdu_type) {
                           case L1_LAN_IIH:
                           case L2_LAN_IIH:
                              type = ARGUS_TYPE_ISIS;
                              addr = &flow->isis_flow.isis_un.hello.lanid;
                              objlen = NODE_ID_LEN;
                              break;

                           case L1_LSP:
                           case L2_LSP:
                              type = ARGUS_TYPE_INT;
                              addr = &flow->isis_flow.isis_un.lsp.seqnum;
                              objlen = 4;
                              break;

                           case L1_CSNP:
                           case L2_CSNP:
                           case L1_PSNP:
                           case L2_PSNP:
                              addr = NULL;
                              break;
                        }
                        break;
                     }
                  }
                  break;
               }
               case ARGUS_FLOW_ARP: {
                  switch (type = (flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                     case ARGUS_TYPE_RARP:
                        type = ARGUS_TYPE_ETHER;
                        addr = &flow->rarp_flow.shaddr;
                        objlen = 6;
                        break;

                     case ARGUS_TYPE_ARP:
                        type = ARGUS_TYPE_IPV4;
                        addr = &flow->arp_flow.arp_tpa;
                        objlen = 4;
                        break;
                  }
                  break;
               }
            }
         }

         ArgusPrintAddr (parser, buf, type, addr, objlen, masklen, len, ARGUS_DST);
         break;
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstAddr (%p, %p)", buf, argus);
#endif
}

/* shared routine for printing system, node and lsp-ids */
static char *
isis_print_id(const u_int8_t *cp, int id_len)
{
   int i;
   static char id[sizeof("xxxx.xxxx.xxxx.yy-zz")];
   char *pos = id;

   for (i = 1; i <= SYSTEM_ID_LEN; i++) {
      snprintf(pos, sizeof(id) - (pos - id), "%02x", *cp++);

   pos += strlen(pos);
   if (i == 2 || i == 4)
      *pos++ = '.';
   }
   if (id_len >= NODE_ID_LEN) {
      snprintf(pos, sizeof(id) - (pos - id), ".%02x", *cp++);
      pos += strlen(pos);
   }
   if (id_len == LSP_ID_LEN)
      snprintf(pos, sizeof(id) - (pos - id), "-%02x", *cp);

   return (id);
}


#define ARGUS_INODE	0x03

void
ArgusPrintAddr (struct ArgusParserStruct *parser, char *buf, int type, void *addr, int objlen, char masklen, int len, int dir)
{
   char addrbuf[128], *addrstr = NULL;
   char *dirstr;

   switch (dir) {
      case ARGUS_SRC:   dirstr = "Src"; break;
      case ARGUS_DST:   dirstr = "Dst"; break;
      case ARGUS_INODE: dirstr = "Inode"; break;
   }
    
   if (addr != NULL) {
      switch (type) {
         case ARGUS_TYPE_IPV4:
            if (parser->status & ARGUS_PRINTNET) {
               unsigned int naddr = (*(unsigned int *)addr & ipaddrtonetmask(*(unsigned int *)addr));

               if (parser->RaPrintAlgorithmList[parser->RaPrintIndex] != NULL) {
                  char *format = parser->RaPrintAlgorithmList[parser->RaPrintIndex]->format;

                  if ((format != NULL) && (strlen(format) > 0)) {
                     snprintf (addrbuf, 128, format, naddr);
                     addrstr = addrbuf;

                  } else
                     addrstr = ArgusGetName (parser, (unsigned char *)&naddr);
               }
            } else  {
               if (parser->RaPrintAlgorithmList[parser->RaPrintIndex] != NULL) {
                  char *format = parser->RaPrintAlgorithmList[parser->RaPrintIndex]->format;

                  if ((format != NULL) && (strlen(format) > 0)) {
                     unsigned int naddr = *(unsigned int *)addr;
                     snprintf (addrbuf, 128, format, naddr);
                     addrstr = addrbuf;

                  } else
                     addrstr = ArgusGetName (parser, (unsigned char *)addr);
               }
               switch (parser->cidrflag) {
                  case RA_ENABLE_CIDR_ADDRESS_FORMAT:
                     if ((masklen == 32) || (masklen == 0))
                        break;

//  deliberately fall through
                  case RA_STRICT_CIDR_ADDRESS_FORMAT:
                     sprintf(addrbuf, "%s/%d", addrstr, masklen);
                     addrstr = addrbuf;
               }
            }
            break;

         case ARGUS_TYPE_IPV6: {
            if (parser->RaPrintAlgorithmList[parser->RaPrintIndex] != NULL) {
               char *format = parser->RaPrintAlgorithmList[parser->RaPrintIndex]->format;

               if ((format != NULL) && (strlen(format) > 0)) {
                  char bbuf[64], *tptr = bbuf;
                  sprint128(tptr, format, (uint128 *)addr);
                  snprintf (addrbuf, 128, "%s", tptr);
                  addrstr = addrbuf;
               } else
                  addrstr = ArgusGetV6Name (parser, (unsigned char *)addr);
            }
            break;
         }

         case ARGUS_TYPE_ARP:
         case ARGUS_TYPE_RARP:
         case ARGUS_TYPE_ETHER:
            addrstr = etheraddr_string (parser, (unsigned char *) addr);
            break;

         case ARGUS_TYPE_INT:
            addrstr = addrbuf;
            sprintf (addrstr, "0x%08x", *(unsigned int *)addr);
            break;


         case ARGUS_TYPE_ISIS:
            addrstr = isis_print_id ((unsigned char *) addr, objlen);
            break;
      }
   }

   if (parser->domainonly) {
      char *tptr = addrstr;
      while (tptr && (strlen(tptr) > len))
         if ((tptr = strchr(tptr, (int) '.')) != NULL)
            tptr++;
      if (tptr)
         addrstr = tptr;
   }
             
   if (parser->ArgusPrintXml) {
      sprintf (buf, " %sAddr = \"%s\"", dirstr, addrstr);
   } else {
      if (len != 0) {
         switch (parser->RaFieldWidth) {
            case RA_FIXED_WIDTH:
               if (addrstr && (len < strlen(addrstr))) {
                  if (parser->domainonly) {
                     char *tptr = addrstr;
                     while (tptr && (strlen(tptr) > len))
                        if ((tptr = strchr(tptr, (int) '.')) != NULL)
                           tptr++;
                     if (tptr)
                        sprintf (buf, "%*.*s ", len, len, tptr);
                     else
                        sprintf (buf, "%*.*s* ", len-1, len-1, (addrstr != NULL ? addrstr : ""));
                  } else
                     sprintf (buf, "%*.*s* ", len-1, len-1, (addrstr != NULL ? addrstr : ""));
               } else
                  sprintf (buf, "%*.*s ", len, len, (addrstr != NULL ? addrstr : ""));
               break;
            default:
               sprintf (buf, "%s ", addrstr);
               break;
         }

      } else 
         sprintf (buf, "%s ", (addrstr != NULL ? addrstr : ""));
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintAddr (0x%x, 0x%x, %d, 0x%x, %d, %d)", parser, buf, type, addr, objlen, len, dir);
#endif
}


void
ArgusPrintSrcPort (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   if (argus->hdr.type & ARGUS_MAR) {
      struct ArgusRecord *rec = (struct ArgusRecord *) argus->dsrs[0];
      unsigned int value = 0;
      char pbuf[32];

      if (rec != NULL) {
         value = rec->argus_mar.dropped;
         sprintf (pbuf, "%u", value);
      } else
         bzero(pbuf, sizeof(pbuf));

      if (parser->ArgusPrintXml) {
      } else {
         switch (parser->RaFieldWidth) {
            case RA_FIXED_WIDTH:
               if (strlen(pbuf) > len) {
                  pbuf[len - 1] = '*';
                  pbuf[len]     = '\0';
               }
               sprintf (buf, "%*.*s ", len, len, pbuf);
               break;
            default:
               sprintf (buf, "%u ", value);
               break;
         }
      }

   } else {
      struct ArgusFlow *flow;
      int type, done = 0;
      u_char proto;
 
      if (((flow = (void *)argus->dsrs[ARGUS_FLOW_INDEX]) != NULL)) {
         switch (flow->hdr.subtype & 0x3F) {
            case ARGUS_FLOW_CLASSIC5TUPLE: {
               switch (type = (flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                  case ARGUS_TYPE_IPV4:
                     proto = flow->ip_flow.ip_p;
                     switch (proto) {
                        case IPPROTO_TCP:
                        case IPPROTO_UDP:
                        case IPPROTO_ICMP:
                           ArgusPrintPort (parser, buf, argus, type, proto, flow->ip_flow.sport, len, ARGUS_SRC);
                           done++;
                           break;
                     }
                     break;

                  case ARGUS_TYPE_IPV6:
                     switch (proto = flow->ipv6_flow.ip_p) {
                        case IPPROTO_TCP:
                        case IPPROTO_UDP:
                        case IPPROTO_ICMPV6:
                           ArgusPrintPort (parser, buf, argus, type, proto, flow->ipv6_flow.sport, len, ARGUS_SRC);
                           done++;
                           break;
                     }
                     break;

                  case ARGUS_TYPE_ETHER:
                     ArgusPrintPort (parser, buf, argus, type, ARGUS_TYPE_ETHER, flow->mac_flow.mac_union.ether.ssap, len, ARGUS_SRC);
                     done++;
                     break;

                  case ARGUS_TYPE_ARP:
                  case ARGUS_TYPE_RARP: {
                     if (parser->ArgusPrintXml) {
                     } else
                        sprintf (buf, "%-*.*s ", len, len, " ");
                     done++;
                     break;
                  }
               }
               break;
            }
         }
      }
 
      if (!done) {
         if (parser->ArgusPrintXml) {
            sprintf (buf, " SrcPort = \"\"");
         } else
            sprintf (buf, "%*s ", len, " ");
      }
   }
 
#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcPort (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintDstPort (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   if (argus->hdr.type & ARGUS_MAR) {
      struct ArgusRecord *rec = (struct ArgusRecord *) argus->dsrs[0];
      unsigned int value = 0;
      char pbuf[32];

      if (rec != NULL) {
         value = rec->argus_mar.clients;
         sprintf (pbuf, "%u", value);
      } else
         bzero(pbuf, sizeof(pbuf));

      if (parser->ArgusPrintXml) {
      } else {
         switch (parser->RaFieldWidth) {
            case RA_FIXED_WIDTH:
               if (strlen(pbuf) > len) {
                  pbuf[len - 1] = '*';
                  pbuf[len]     = '\0';
               }
               sprintf (buf, "%*.*s ", len, len, pbuf);
               break;
            default:
               sprintf (buf, "%u ", value);
               break;
         }
      }
   } else {
      struct ArgusFlow *flow; 
      int type, done = 0;
      u_char proto;
   
      if (((flow = (void *)argus->dsrs[ARGUS_FLOW_INDEX]) != NULL)) {
         switch (flow->hdr.subtype & 0x3F) {
            case ARGUS_FLOW_CLASSIC5TUPLE: {
               switch (type = (flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                  case ARGUS_TYPE_IPV4:
                     proto = flow->ip_flow.ip_p;
                     switch (flow->ip_flow.ip_p) {
                        case IPPROTO_TCP:
                        case IPPROTO_UDP: 
                        case IPPROTO_ICMP:
                           ArgusPrintPort (parser, buf, argus, type, proto, flow->ip_flow.dport, len, ARGUS_DST);
                           done++; 
                           break; 

                        case IPPROTO_ESP: 
                           ArgusPrintEspSpi (parser, buf, argus, type, flow->esp_flow.spi, len);
                           done++; 
                           break; 
                     }
                     break; 

                  case ARGUS_TYPE_IPV6: 
                     proto = flow->ipv6_flow.ip_p;
                     switch (flow->ipv6_flow.ip_p) {
                        case IPPROTO_TCP:
                        case IPPROTO_UDP: 
                        case IPPROTO_ICMPV6: 
                           ArgusPrintPort (parser, buf, argus, type, proto,
                                          flow->ipv6_flow.dport, len, ARGUS_DST);
                           done++; 
                           break; 
                        case IPPROTO_ESP: 
                           ArgusPrintEspSpi (parser, buf, argus, type, flow->esp_flow.spi, len);
                           done++; 
                           break; 
                     }
                     break; 
                            
                  case ARGUS_TYPE_ETHER:
                     ArgusPrintPort (parser, buf, argus, type, ARGUS_TYPE_ETHER, flow->mac_flow.mac_union.ether.dsap, len, ARGUS_DST);
                     done++;
                     break;

                  case ARGUS_TYPE_ARP:
                  case ARGUS_TYPE_RARP:
                     if (parser->ArgusPrintXml) {
                     } else {
                        if (parser->RaFieldWidth != RA_FIXED_WIDTH)
                           len = strlen(" ");
                        sprintf (buf, "%*.*s ", len, len, " ");
                     }
                     done++;
                     break;

                  case ARGUS_TYPE_ISIS: {
                     char chksum[32];
                     sprintf (chksum, "0x%x", flow->flow_un.isis.chksum);

                     if ((parser->RaFieldDelimiter != ' ') && (parser->RaFieldDelimiter != '\0')) {
                        switch (parser->RaFieldWidth) {
                           case RA_FIXED_WIDTH:
                              if (strlen(chksum) > len) {
                                 chksum[len - 1] = '*';
                                 chksum[len]     = '\0';
                              }
                              sprintf (buf, "%-*.*s%c", len, len, chksum, parser->RaFieldDelimiter);
                              break;
                           default:
                              sprintf (buf, "0x%x%c", flow->flow_un.isis.chksum, parser->RaFieldDelimiter);
                              break;
                        }

                     } else {
                        if (strlen(chksum) > len)
                           chksum[len - 1] = '*';
                        chksum[len]     = '\0';
                        sprintf (buf, "%-*.*s ", len, len, chksum);
                     }
                     done++;
                     break;
                  }
               }
               break; 
            }
         }
      }      
                      
      if (!done) {
         if (parser->ArgusPrintXml) {
            sprintf (buf, " DstPort = \"\"");
         } else
            sprintf (buf, "%*.*s ", len, len, " ");
      }
   }            

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstPort (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintPort (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus,
                int type, u_char proto, u_int port, int len, int dir)
{
   char *dirstr = (dir == ARGUS_SRC) ? "Src" : "Dst";

   if (parser->nflag > 1) {
      char upbuf[32], *upstr = upbuf;

      switch (proto) {
         case IPPROTO_ICMP:
            sprintf(upstr, "0x%4.4x", port);
            break;

         default:
            sprintf(upstr, "%d", port);
            break;
      }

      if (parser->ArgusPrintXml) {
         sprintf (buf, " %sPort = \"%s\"", dirstr, upstr);
      } else {
         switch (parser->RaFieldWidth) {
            case RA_FIXED_WIDTH:
               sprintf (buf, "%-*s ", len, upstr);
               break;
            default:
               sprintf (buf, "%-s ", upstr);
               break;
         }
      }

   } else {
      switch (type) {
         case ARGUS_TYPE_IPV4:
         case ARGUS_TYPE_IPV6:
            break;

         case ARGUS_TYPE_ETHER: {
            char *llcstr = llcsap_string((unsigned char) port);

            if (parser->ArgusPrintXml) {
               sprintf (buf, " %sEtherLlcSap = \"%s\"", dirstr, llcstr);
            } else {
               switch (parser->RaFieldWidth) {
                  case RA_FIXED_WIDTH:
                     if (strlen(llcstr) > len) {
                        sprintf (buf, "%-*.*s* ", len-1 , len-1, llcstr);
                     } else
                        sprintf (buf, "%-*.*s ", len , len, llcstr);
                     break;
                  default:
                     sprintf (buf, "%s ", llcstr);
                     break;
               }
            }
            return;
         }
      }

      switch (proto) {
         case IPPROTO_TCP: {
            char *tpstr = tcpport_string(port);
            if (parser->ArgusPrintXml) {
               sprintf (buf, " %sPort = \"%s\"", dirstr, tpstr);
            } else {
               switch (parser->RaFieldWidth) {
                  case RA_FIXED_WIDTH:
                     if (strlen(tpstr) > len) {
                        sprintf (buf, "%-*.*s* ", len-1, len-1, tpstr);
                     } else
                        sprintf (buf, "%-*.*s ", len, len, tpstr);
                     break;
                  default:
                     sprintf (buf, "%s ", tpstr);
                     break;
               }
            }
            break; 
         }
         case IPPROTO_UDP: {
            char *upstr = udpport_string(port);
            if (parser->ArgusPrintXml) {
               sprintf (buf, " %sPort = \"%s\"", dirstr, upstr);
            } else
               switch (parser->RaFieldWidth) {
                  case RA_FIXED_WIDTH:
                     if (strlen(upstr) > len) {
                        sprintf (buf, "%-*.*s* ", len-1, len-1, upstr);
                     } else
                        sprintf (buf, "%-*.*s ", len, len, upstr);
                     break;
                  default:
                     sprintf (buf, "%s ", upstr);
                     break;
               }
            break; 
         }
         case IPPROTO_ICMP: {
            char upbuf[32], *upstr = upbuf;
            sprintf(upstr, "0x%4.4x", port);

            if (parser->ArgusPrintXml) {
               sprintf (buf, " %sPort = \"%s\"", dirstr, upstr);
            } else   
               switch (parser->RaFieldWidth) {
                  case RA_FIXED_WIDTH:
                     if (strlen(upstr) > len) {
                        sprintf (buf, "%-*.*s* ", len-1, len-1, upstr);
                     } else
                        sprintf (buf, "%-*.*s ", len, len, upstr);
                     break;
                  default:
                     sprintf (buf, "%s ", upstr);
                     break;
               }  
            break; 
         } 

         default:
            if (parser->ArgusPrintXml) {
               sprintf (buf, " %sPort = \"%u\"", dirstr, port);
            } else
               switch (parser->RaFieldWidth) {
                  case RA_FIXED_WIDTH:
                     sprintf (buf, "%-*u ", len, port);
                     break;
                  default:
                     sprintf (buf, "%u ", port);
                     break;
               }
            break; 
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintPort (0x%x, 0x%x, %d, %d, %d)", buf, argus, port, len, dir);
#endif
}



void
ArgusPrintEspSpi (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int type, u_int spi, int len)
{
   if (argus->hdr.type & ARGUS_MAR) {
 
   } else {
      struct ArgusFlow *flow;

      if (((flow = (void *)argus->dsrs[ARGUS_FLOW_INDEX]) != NULL)) {
         char spibuf[32];

         sprintf (spibuf, "0x%8.8x", spi);
         if (strlen(spibuf) > len) {
            spibuf[len - 1] = '*';
            spibuf[len]     = '\0';
         }

         if (parser->RaPrintIndex > 0) {
            if (parser->RaPrintAlgorithmList[parser->RaPrintIndex - 1]) {
               if ((parser->RaPrintAlgorithmList[parser->RaPrintIndex - 1]->print == ArgusPrintSrcAddr) ||
                   (parser->RaPrintAlgorithmList[parser->RaPrintIndex - 1]->print == ArgusPrintDstAddr))
                  if (parser->RaFieldDelimiter == '\0')
                     if (buf[strlen(buf) - 1] == ' ') 
                        buf[strlen(buf) - 1] = '.';
            }
         }
      
         if (parser->ArgusPrintXml) {
            sprintf (buf, "  EspSpi = \"%s\"", spibuf);
         } else {
            switch (parser->RaFieldWidth) {
               case RA_FIXED_WIDTH: 
                  if (strlen(spibuf) > len) {
                     sprintf (buf, "%-*.*s* ", len-1, len-1, spibuf);
                  } else 
                     sprintf (buf, "%-*.*s ", len, len, spibuf);
                  break;
               default:
                  sprintf (buf, "0x%x ", spi);
                  break;
            }
         }
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintEspSpi (0x%x, 0x%x, %d, %d)", buf, argus, spi, len);
#endif
}

void                       
ArgusPrintSrcIpId (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{                          
   struct ArgusIPAttrStruct *attr = (struct ArgusIPAttrStruct *)argus->dsrs[ARGUS_IPATTR_INDEX];
   char ipidbuf[8];

   bzero (ipidbuf, sizeof(ipidbuf));

   if (attr != NULL) 
      if (attr->hdr.argus_dsrvl8.qual & ARGUS_IPATTR_SRC)
         sprintf (ipidbuf, "0x%04x", attr->src.ip_id);

   if (parser->ArgusPrintXml) {
      sprintf (buf, "  SrcIpId = \"%s\"", ipidbuf);
   } else
      switch (parser->RaFieldWidth) {
         case RA_FIXED_WIDTH:
            if (strlen(ipidbuf) > len) {
               sprintf (buf, "%*.*s* ", len-1, len-1, ipidbuf);
            } else
               sprintf (buf, "%*.*s ", len, len, ipidbuf);
            break;
         default:
            sprintf (buf, "%s ", ipidbuf);
            break;
      }

#ifdef ARGUSDEBUG           
   ArgusDebug (10, "ArgusPrintSrcIpId (%p, %p)", buf, argus);
#endif               
}

void                       
ArgusPrintDstIpId (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusIPAttrStruct *attr = (struct ArgusIPAttrStruct *)argus->dsrs[ARGUS_IPATTR_INDEX];
   char ipidbuf[8];

   bzero (ipidbuf, sizeof(ipidbuf));

   if (attr != NULL)
      if (attr->hdr.argus_dsrvl8.qual & ARGUS_IPATTR_DST)
         sprintf (ipidbuf, "0x%04x", attr->dst.ip_id);

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstIpId = \"%s\"", ipidbuf);
   } else
      switch (parser->RaFieldWidth) {
         case RA_FIXED_WIDTH:
            if (strlen(ipidbuf) > len) {
               sprintf (buf, "%*.*s* ", len-1, len-1, ipidbuf);
            } else
               sprintf (buf, "%*.*s ", len, len, ipidbuf);
            break;
         default:
            sprintf (buf, "%s ", ipidbuf);
            break;
      }
                        
#ifdef ARGUSDEBUG           
   ArgusDebug (10, "ArgusPrintDstIpId (%p, %p)", buf, argus);
#endif               
}


char *argus_dscodes[0x100];

void                       
ArgusPrintSrcDSByte (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   int tos;
   struct ArgusIPAttrStruct *attr = (struct ArgusIPAttrStruct *)argus->dsrs[ARGUS_IPATTR_INDEX];
   char obuf[32];

   bzero(obuf, sizeof(obuf));
   if ((argus->hdr.type & ARGUS_MAR) || (attr == NULL)) {
   } else {
      if (attr->hdr.argus_dsrvl8.qual & ARGUS_IPATTR_SRC) {
         tos = (attr->src.tos >> 2);
         if (!(parser->nflag > 2) && (argus_dscodes[tos] != NULL)) {
            sprintf (obuf, "%s", argus_dscodes[tos]);
         } else {
            sprintf (obuf, "%2d", tos);
         }
      }
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcDSByte = \"%s\"", obuf);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH)
         len = strlen(obuf);
      else {
         if (strlen(obuf) > len) {
            obuf[len - 1] = '*';
            obuf[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, obuf);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcDSByte (%p, %p)", buf, argus);
#endif
}

void                       
ArgusPrintDstDSByte (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   int tos;
   struct ArgusIPAttrStruct *attr = (struct ArgusIPAttrStruct *)argus->dsrs[ARGUS_IPATTR_INDEX];
   char obuf[32];

   bzero (obuf, sizeof(obuf));
   if ((argus->hdr.type & ARGUS_MAR) || (attr == NULL)) {
   } else {
      if (attr->hdr.argus_dsrvl8.qual & ARGUS_IPATTR_DST) {
         tos = (attr->dst.tos >> 2);
         if (!(parser->nflag > 2) && (argus_dscodes[tos] != NULL)) {
            sprintf (obuf, "%s", argus_dscodes[tos]);
         } else {
            sprintf (obuf, "%2d", tos);
         }
      }
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstDSByte = \"%s\"", obuf);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH)
         len = strlen(obuf);
      else {
         if (strlen(obuf) > len) {
            obuf[len - 1] = '*';
            obuf[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, obuf);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstDSByte (%p, %p)", buf, argus);
#endif
}

void                       
ArgusPrintSrcTos (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{                          
   struct ArgusIPAttrStruct *attr = (struct ArgusIPAttrStruct *)argus->dsrs[ARGUS_IPATTR_INDEX];
   char obuf[32];

   bzero(obuf, sizeof(obuf));
   if ((argus->hdr.type & ARGUS_MAR) || (attr == NULL)) {
   } else {
      if (attr->hdr.argus_dsrvl8.qual & ARGUS_IPATTR_SRC)
         sprintf (obuf, "%d", attr->src.tos);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcTos = \"%s\"", obuf);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH)
         len = strlen(obuf);
      else {
         if (strlen(obuf) > len) {
            obuf[len - 1] = '*';
            obuf[len]     = '\0';
         } 
      } 
      sprintf (buf, "%*.*s ", len, len, obuf);
   }

#ifdef ARGUSDEBUG           
   ArgusDebug (10, "ArgusPrintSrcTos (%p, %p)", buf, argus);
#endif               
}

void                       
ArgusPrintDstTos (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusIPAttrStruct *attr = (struct ArgusIPAttrStruct *)argus->dsrs[ARGUS_IPATTR_INDEX];
   char obuf[32]; 

   bzero(obuf, sizeof(obuf));
   if ((argus->hdr.type & ARGUS_MAR) || (attr == NULL)) {
   } else {
      if (attr->hdr.argus_dsrvl8.qual & ARGUS_IPATTR_DST)
         sprintf (obuf, "%d", attr->dst.tos);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstTos = \"%s\"", obuf);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH)
         len = strlen(obuf);
      else {
         if (strlen(obuf) > len) {
            obuf[len - 1] = '*';
            obuf[len]     = '\0';
         } 
      } 
      sprintf (buf, "%*.*s ", len, len, obuf);
   }

#ifdef ARGUSDEBUG           
   ArgusDebug (10, "ArgusPrintDstTos (%p, %p)", buf, argus);
#endif               
}

/*
void
ArgusPrintTos (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   ArgusPrintSrcTos (parser, buf, argus);
   ArgusPrintDstTos (parser, buf, argus);

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintTos (%p, %p)", buf, argus);
#endif
}
*/


void                       
ArgusPrintSrcTtl (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusIPAttrStruct *attr = (struct ArgusIPAttrStruct *)argus->dsrs[ARGUS_IPATTR_INDEX];
   char obuf[32];

   bzero(obuf, sizeof(obuf));
   if ((argus->hdr.type & ARGUS_MAR) || (attr == NULL)) {
   } else {
      if (attr->hdr.argus_dsrvl8.qual & ARGUS_IPATTR_SRC)
         sprintf (obuf, "%d", attr->src.ttl);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcTtl = \"%s\"", obuf);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH)
         len = strlen(obuf);
      else {
         if (strlen(obuf) > len) {
            obuf[len - 1] = '*';
            obuf[len]     = '\0';
         } 
      } 
      sprintf (buf, "%*.*s ", len, len, obuf);
   }

#ifdef ARGUSDEBUG           
   ArgusDebug (10, "ArgusPrintSrcTtl (%p, %p)", buf, argus);
#endif               
}

void                       
ArgusPrintDstTtl (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusIPAttrStruct *attr = (struct ArgusIPAttrStruct *)argus->dsrs[ARGUS_IPATTR_INDEX];
   char obuf[32];

   bzero(obuf, sizeof(obuf));
   if ((argus->hdr.type & ARGUS_MAR) || (attr == NULL)) {
   } else {
      if (attr->hdr.argus_dsrvl8.qual & ARGUS_IPATTR_DST)
         sprintf (obuf, "%d", attr->dst.ttl);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstTtl = \"%s\"", obuf);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH)
         len = strlen(obuf);
      else {
         if (strlen(obuf) > len) {
            obuf[len - 1] = '*';
            obuf[len]     = '\0';
         } 
      } 
      sprintf (buf, "%*.*s ", len, len, obuf);
   }

#ifdef ARGUSDEBUG           
   ArgusDebug (10, "ArgusPrintDstTtl (%p, %p)", buf, argus);
#endif               
}


void                       
ArgusPrintSrcHopCount (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   int esthops = 1;
   struct ArgusIPAttrStruct *attr = (struct ArgusIPAttrStruct *)argus->dsrs[ARGUS_IPATTR_INDEX];
   char obuf[32];

   bzero(obuf, sizeof(obuf));
   if ((argus->hdr.type & ARGUS_MAR) || (attr == NULL)) {
   } else {
      if (attr->hdr.argus_dsrvl8.qual & ARGUS_IPATTR_SRC)
         while (esthops < attr->src.ttl)
            esthops = esthops * 2;
         sprintf (obuf, "%d", (esthops - attr->src.ttl));
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcHops = \"%s\"", obuf);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH)
         len = strlen(obuf);
      else {
         if (strlen(obuf) > len) {
            obuf[len - 1] = '*';
            obuf[len]     = '\0';
         } 
      } 
      sprintf (buf, "%*.*s ", len, len, obuf);
   }

#ifdef ARGUSDEBUG           
   ArgusDebug (10, "ArgusPrintSrcHopCount (%p, %p)", buf, argus);
#endif               
}

void                       
ArgusPrintDstHopCount (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   int esthops = 1;
   struct ArgusIPAttrStruct *attr = (struct ArgusIPAttrStruct *)argus->dsrs[ARGUS_IPATTR_INDEX];
   char obuf[32];
 
   bzero(obuf, sizeof(obuf));
   if ((argus->hdr.type & ARGUS_MAR) || (attr == NULL)) {
   } else {
      if (attr->hdr.argus_dsrvl8.qual & ARGUS_IPATTR_DST) {
         while (esthops < attr->dst.ttl)
            esthops = esthops * 2;
         sprintf (obuf, "%d", (esthops - attr->dst.ttl));
      }
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstHops = \"%s\"", obuf);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH)
         len = strlen(obuf);
      else {
         if (strlen(obuf) > len) {
            obuf[len - 1] = '*';
            obuf[len]     = '\0';
         } 
      } 
      sprintf (buf, "%*.*s ", len, len, obuf);
   }

#ifdef ARGUSDEBUG           
   ArgusDebug (10, "ArgusPrintDstHopCount (%p, %p)", buf, argus);
#endif               
}


void                       
ArgusPrintInode (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char masklen = 32, obuf[32];
   int objlen = 0;
 
   bzero(obuf, sizeof(obuf));

   if (argus->hdr.type & ARGUS_MAR) {
      sprintf (buf, "%*.*s ", len, len, "");
   } else {
      struct ArgusIcmpStruct *icmp = (void *)argus->dsrs[ARGUS_ICMP_INDEX];

      if (icmp != NULL) {
         if (icmp->hdr.argus_dsrvl8.qual & ARGUS_ICMP_MAPPED) {
            struct ArgusFlow *flow = (void *)argus->dsrs[ARGUS_FLOW_INDEX];
            void *addr = NULL;
            int type = 0;

            if (flow != NULL) {
               switch (flow->hdr.subtype & 0x3F) {
                  case ARGUS_FLOW_CLASSIC5TUPLE: 
                  case ARGUS_FLOW_LAYER_3_MATRIX: {
                     switch (type = (flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                        case ARGUS_TYPE_IPV4:
                           objlen = 4;
                           break;
                        case ARGUS_TYPE_IPV6:
                           objlen = 16;
                           break;
                     }
                     break;
                  }
       
                  default:
                     break;
               }
            }

            if (objlen > 0)
               addr = &icmp->osrcaddr;

            ArgusPrintAddr (parser, buf, type, addr, objlen, masklen, len, ARGUS_INODE);

         } else
            sprintf (buf, "%*.*s ", len, len, "");

      } else
         sprintf (buf, "%*.*s ", len, len, "");
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintInode (%p, %p)", buf, argus);
#endif
}


void                       
ArgusPrintKeyStrokeNStroke (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char obuf[32];
 
   bzero(obuf, sizeof(obuf));

   if (argus->hdr.type & ARGUS_MAR) {
      sprintf (buf, "%*.*s ", len, len, "");
   } else {
      struct ArgusBehaviorStruct *actor = (void *)argus->dsrs[ARGUS_BEHAVIOR_INDEX];

      if (actor != NULL) {
         sprintf (obuf, "%d ", actor->keyStroke.src.n_strokes + actor->keyStroke.dst.n_strokes);
      } else
         sprintf (obuf, "%*.*s", len, len, "");
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " KeyStrokeNStroke = \"%s\"", obuf);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH)
         len = strlen(obuf);
      else {
         if (strlen(obuf) > len) {
            obuf[len - 1] = '*';
            obuf[len]     = '\0';
         } 
      } 
      sprintf (buf, "%*.*s ", len, len, obuf);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintKeyStrokeNStroke (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintKeyStrokeSrcNStroke (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char obuf[32];

   bzero(obuf, sizeof(obuf));

   if (argus->hdr.type & ARGUS_MAR) {
      sprintf (buf, "%*.*s ", len, len, "");
   } else {
      struct ArgusBehaviorStruct *actor = (void *)argus->dsrs[ARGUS_BEHAVIOR_INDEX];

      if (actor != NULL) {
         sprintf (obuf, "%d ", actor->keyStroke.src.n_strokes);
      } else
         sprintf (obuf, "%*.*s", len, len, "");
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " KeyStrokeSrcNStroke = \"%s\"", obuf);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH)
         len = strlen(obuf);
      else {
         if (strlen(obuf) > len) {
            obuf[len - 1] = '*';
            obuf[len]     = '\0';
         } 
      } 
      sprintf (buf, "%*.*s ", len, len, obuf);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintKeyStrokeSrcNStroke (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintKeyStrokeDstNStroke (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char obuf[32];

   bzero(obuf, sizeof(obuf));

   if (argus->hdr.type & ARGUS_MAR) {
      sprintf (buf, "%*.*s ", len, len, "");
   } else {
      struct ArgusBehaviorStruct *actor = (void *)argus->dsrs[ARGUS_BEHAVIOR_INDEX];

      if (actor != NULL) {
         sprintf (obuf, "%d ", actor->keyStroke.dst.n_strokes);
      } else
         sprintf (obuf, "%*.*s", len, len, "");
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " KeyStrokeDstNStroke = \"%s\"", obuf);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH)
         len = strlen(obuf);
      else {
         if (strlen(obuf) > len) {
            obuf[len - 1] = '*';
            obuf[len]     = '\0';
         } 
      } 
      sprintf (buf, "%*.*s ", len, len, obuf);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintKeyStrokeDstNStroke (%p, %p)", buf, argus);
#endif
}


char *ArgusProcessStr = NULL;

void
ArgusPrintDir (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   if (argus->hdr.type & ARGUS_MAR) {
      if (parser->ArgusPrintXml) {
      } else {
         sprintf (buf, "%*.*s ", len, len, " ");
      }
 
   } else {
      struct ArgusMetricStruct *metric = (struct ArgusMetricStruct *)argus->dsrs[ARGUS_METRIC_INDEX];
      struct ArgusFlow *flow = (struct ArgusFlow *)argus->dsrs[ARGUS_FLOW_INDEX];
      struct ArgusNetworkStruct *net = (struct ArgusNetworkStruct *)argus->dsrs[ARGUS_NETWORK_INDEX];
      int type, src_count = 0, dst_count = 0;
      char dirStr[8];

         
      if (metric == NULL) {
         sprintf (dirStr, "%s", "   ");
         if (parser->ArgusPrintXml) {
         } else {
            if (parser->RaFieldWidth != RA_FIXED_WIDTH)
               len = strlen(dirStr);
            else {
               if (strlen(dirStr) > len) {
                  dirStr[len - 1] = '*';
                  dirStr[len]     = '\0';
               } 
            } 
            sprintf (buf, "%*.*s ", len, len, dirStr);
         }

      } else {
         sprintf (dirStr, "%s", "<->");

         if ((dst_count = metric->dst.pkts) == 0)
            dirStr[0] = ' ';
         if ((src_count = metric->src.pkts) == 0)
            dirStr[2] = ' ';
         if ((src_count == 0) && (dst_count == 0))
            dirStr[1] = ' ';
 
         if (flow != NULL) {
            switch (flow->hdr.subtype & 0x3F) {
               case ARGUS_FLOW_CLASSIC5TUPLE: {
                  switch (type = (flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                     case ARGUS_TYPE_IPV4:
                        switch (flow->ip_flow.ip_p) {
                           case IPPROTO_TCP: {
                              if (net != NULL) {
                                 struct ArgusTCPObject *tcp = (struct ArgusTCPObject *)&net->net_union.tcp;
                                 if (!((tcp->status & ARGUS_SAW_SYN) || (tcp->status & ARGUS_SAW_SYN_SENT))) {
                                    dirStr[1] = '?';
                                 }
                                 if ((tcp->status & ARGUS_SAW_SYN) || (tcp->status & ARGUS_SAW_SYN_SENT)) {
                                    dirStr[0] = ' ';
                                    dirStr[2] = '>';
                                 }
                              }
                           }
                           break;
                        }
                        break;  

                     case ARGUS_TYPE_IPV6:
                        switch (flow->ipv6_flow.ip_p) {
                           case IPPROTO_TCP: {
                              if (net != NULL) {
                                 struct ArgusTCPObject *tcp = (struct ArgusTCPObject *)&net->net_union.tcp;
                                 if (!((tcp->status & ARGUS_SAW_SYN) || (tcp->status & ARGUS_SAW_SYN_SENT))) {
                                    dirStr[1] = '?';
                                 } else {
                                    if (tcp->status & ARGUS_SAW_SYN) {
                                       dirStr[0] = ' ';
                                    } else {
                                       if (tcp->status & ARGUS_SAW_SYN_SENT) {
                                          dirStr[2] = ' ';
                                       }
                                    }
                                 }
                              }
                           }
                           break;
                        }
                        break;  

                     case ARGUS_TYPE_RARP:
                        sprintf (dirStr, "tel");
                        break;

                     case ARGUS_TYPE_ARP:
                        sprintf (dirStr, "who");
                        break;
                  } 
                  break;
               }

               case ARGUS_FLOW_ARP: {
                  sprintf (dirStr, "who");
                  break;
               }
            }
         }

         if (parser->ArgusPrintXml) {
            char ndirStr[16], *dptr = dirStr;
            int i, len;

            bzero(ndirStr, 16);
            for (i = 0, len = strlen(dirStr); i < len; i++) {
               if (*dptr == '<')
                  sprintf (&ndirStr[strlen(ndirStr)], "&lt;");
               else if (*dptr == '>')
                  sprintf (&ndirStr[strlen(ndirStr)], "&gt;");
               else 
                  sprintf (&ndirStr[strlen(ndirStr)], "%c", *dptr);
               dptr++;
            }
            sprintf (buf, " Dir = \"%s\"", ndirStr);

         } else {
            if (parser->RaFieldWidth != RA_FIXED_WIDTH)
               len = strlen(dirStr);
            else {
               if (strlen(dirStr) > len) {
                  dirStr[len - 1] = '*';
                  dirStr[len]     = '\0';
               }
            }
            sprintf (buf, "%*.*s ", len, len, dirStr);
         }
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDir (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintPackets (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusMetricStruct *metric, *nsmetric;

   if (argus->hdr.type & ARGUS_MAR) {
      struct ArgusRecord *rec = (struct ArgusRecord *) argus->dsrs[0];
      unsigned long long value = 0;
      char pbuf[32];

      if (rec != NULL) {
         value = rec->argus_mar.pktsRcvd;
#if defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__APPLE_CC__) || defined(__APPLE__) || defined(HAVE_SOLARIS)
         sprintf (pbuf, "%llu", value);
#else
         sprintf (pbuf, "%Lu", value);
#endif
      } else
         bzero(pbuf, sizeof(pbuf));

      if (parser->ArgusPrintXml) {
         sprintf (buf, " Pkts = \"%s\"", pbuf);

      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH)
            len = strlen(pbuf);
         else {
            if (strlen(pbuf) > len) {
               pbuf[len - 1] = '*';
               pbuf[len]     = '\0';
            }
         }
         sprintf (buf, "%*.*s ", len, len, pbuf);
      }

   } else {
      if (argus && ((metric = (void *)argus->dsrs[ARGUS_METRIC_INDEX]) != NULL)) {
         long long value = metric->src.pkts + metric->dst.pkts;
         float fvalue = 0.0;
         char pbuf[32];

         if (parser->Pctflag && parser->ns) {
            if ((nsmetric = (void *)parser->ns->dsrs[ARGUS_METRIC_INDEX]) != NULL) {
               if (value > 0)
                  fvalue = (value * 100.0) / ((nsmetric->src.pkts + nsmetric->dst.pkts) * 1.0);
            }
         }

         if (parser->Pctflag && parser->ns) {
            sprintf (pbuf, "%3.*f", parser->pflag, fvalue);
         } else {
            double tvalue = value * 1.0;
            if (parser->Hflag) {
               ArgusAbbreviateMetric(parser, pbuf, 32, tvalue);
            } else {
#if defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__APPLE_CC__) || defined(__APPLE__) || defined(HAVE_SOLARIS)
               sprintf (pbuf, "%llu", value);
#else
               sprintf (pbuf, "%Lu", value);
#endif
            }
         }

         if (parser->ArgusPrintXml) {
            sprintf (buf, " Pkts = \"%s\"", pbuf);
         } else {
            if (parser->RaFieldWidth != RA_FIXED_WIDTH)
               len = strlen(pbuf);
            else {
               if (strlen(pbuf) > len) {
                  pbuf[len - 1] = '*';
                  pbuf[len]     = '\0';
               }
            }
            sprintf (buf, "%*.*s ", len, len, pbuf);
         }
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintPackets (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintSrcPackets (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusMetricStruct *metric, *nsmetric;
 
   if (argus->hdr.type & ARGUS_MAR) {
      struct ArgusRecord *rec = (struct ArgusRecord *) argus->dsrs[0];
      long long value = 0;
      char pbuf[32];

      if (rec != NULL) {
         value = rec->argus_mar.pktsRcvd;
#if defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__APPLE_CC__) || defined(__APPLE__) || defined(HAVE_SOLARIS)
         sprintf (pbuf, "%llu", value);
#else
         sprintf (pbuf, "%Lu", value);
#endif
      } else
         bzero(pbuf, sizeof(pbuf));

      if (parser->ArgusPrintXml) {
         sprintf (buf, " PktsRcvd = \"%s\"", pbuf);
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH)
            len = strlen(pbuf);
         else {
            if (strlen(pbuf) > len) {
               pbuf[len - 1] = '*';
               pbuf[len]     = '\0';
            }
         }
         sprintf (buf, "%*.*s ", len, len, pbuf);
      }
 
   } else {
      char pbuf[32];
      bzero (pbuf, 4);

      if (argus && ((metric = (void *)argus->dsrs[ARGUS_METRIC_INDEX]) != NULL)) {
         long long value = metric->src.pkts;
         float fvalue = 0.0;

         if (parser->Pctflag && parser->ns) {
            if ((nsmetric = (void *)parser->ns->dsrs[ARGUS_METRIC_INDEX]) != NULL) {
               if (nsmetric->src.pkts > 0)
                  fvalue = (metric->src.pkts * 100.0) / (nsmetric->src.pkts * 1.0);
            }
            sprintf (pbuf, "%.*f", parser->pflag, fvalue);

         } else {
            double tvalue = value * 1.0;
            if (parser->Hflag) {
               ArgusAbbreviateMetric(parser, pbuf, 32, tvalue);
            } else {
#if defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__APPLE_CC__) || defined(__APPLE__) || defined(HAVE_SOLARIS)
               sprintf (pbuf, "%llu", value);
#else
               sprintf (pbuf, "%Lu", value);
#endif
            }
         }
      }

      if (parser->ArgusPrintXml) {
         sprintf (buf, " SrcPkts = \"%s\"", pbuf);
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH)
            len = strlen(pbuf);
         else {
            if (strlen(pbuf) > len) {
               pbuf[len - 1] = '*';
               pbuf[len]     = '\0';
            }  
         } 
         sprintf (buf, "%*.*s ", len, len, pbuf);
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcPackets (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintDstPackets (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusMetricStruct *metric, *nsmetric;
 
   if (argus->hdr.type & ARGUS_MAR) {
      struct ArgusRecord *rec = (struct ArgusRecord *) argus->dsrs[0];
      unsigned int value = 0;
      char pbuf[32];

      if (rec != NULL) {
         value = rec->argus_mar.records;
#if defined(__APPLE_CC__) || defined(__APPLE__) || defined(HAVE_SOLARIS)
         sprintf (pbuf, "%u", value);
#else
         sprintf (pbuf, "%u", value);
#endif
      } else
         bzero(pbuf, sizeof(pbuf));

      if (parser->ArgusPrintXml) {
         sprintf (buf, " Records = \"%s\"", pbuf);
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH)
            len = strlen(pbuf);
         else {
            if (strlen(pbuf) > len) {
               pbuf[len - 1] = '*';
               pbuf[len]     = '\0';
            }  
         } 
         sprintf (buf, "%*.*s ", len, len, pbuf);
      }

   } else {
      char pbuf[32];
      bzero (pbuf, 4);

      if (argus && ((metric = (void *)argus->dsrs[ARGUS_METRIC_INDEX]) != NULL)) {
         long long value = metric->dst.pkts;
         float fvalue = 0.0;

         if (parser->Pctflag && parser->ns) {
            if ((nsmetric = (void *)parser->ns->dsrs[ARGUS_METRIC_INDEX]) != NULL) {
               if (nsmetric->dst.pkts > 0)
                  fvalue = (metric->dst.pkts * 100.0) / (nsmetric->dst.pkts * 1.0);
               sprintf (pbuf, "%.*f", parser->pflag, fvalue);
            }

         } else {
            double tvalue = value * 1.0;
            if (parser->Hflag) {
               ArgusAbbreviateMetric(parser, pbuf, 32, tvalue);
            } else {
#if defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__APPLE_CC__) || defined(__APPLE__) || defined(HAVE_SOLARIS)
               sprintf (pbuf, "%llu", value);
#else
               sprintf (pbuf, "%Lu", value);
#endif
            }
         }
      }

      if (parser->ArgusPrintXml) {
         sprintf (buf, " DstPkts = \"%s\"", pbuf);
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH)
            len = strlen(pbuf);
         else {
            if (strlen(pbuf) > len) {
               pbuf[len - 1] = '*';
               pbuf[len]     = '\0';
            }  
         } 
         sprintf (buf, "%*.*s ", len, len, pbuf);
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstPackets (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintBytes (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusMetricStruct *metric, *nsmetric;
   char pbuf[32];

   bzero(pbuf, 4);

   if (argus->hdr.type & ARGUS_MAR) {
      struct ArgusRecord *rec = (struct ArgusRecord *) argus->dsrs[0];
      long long value = 0;
      char pbuf[32];

      if (rec != NULL) {
         value = rec->argus_mar.bytes;
#if defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__APPLE_CC__) || defined(__APPLE__) || defined(HAVE_SOLARIS)
         sprintf (pbuf, "%llu", value);
#else
         sprintf (pbuf, "%Lu", value);
#endif
      } else
         bzero(pbuf, sizeof(pbuf));


      if (parser->ArgusPrintXml) {
         sprintf (buf, " Bytes = \"%s\"", pbuf);
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH)
            len = strlen(pbuf);
         else {
            if (strlen(pbuf) > len) {
               pbuf[len - 1] = '*';
               pbuf[len]     = '\0';
            }  
         } 
         sprintf (buf, "%*.*s ", len, len, pbuf);
      }
 
   } else {
      if (argus && ((metric = (void *)argus->dsrs[ARGUS_METRIC_INDEX]) != NULL)) {
         long long value = metric->src.bytes + metric->dst.bytes;
         float fvalue = 0.0;

         if (parser->Pctflag && parser->ns) {
            if ((nsmetric = (void *)parser->ns->dsrs[ARGUS_METRIC_INDEX]) != NULL) {
               if (value > 0)
                  fvalue = (value * 100.0) / ((nsmetric->src.bytes + nsmetric->dst.bytes) * 1.0);
            }
         }

         if (parser->Pctflag && parser->ns) {
            sprintf (pbuf, "%.*f", parser->pflag, fvalue);
            sprintf (buf, "%*.*s ", len, len, pbuf);
         } else {
            double tvalue = value * 1.0;
            if (parser->Hflag) {
               ArgusAbbreviateMetric(parser, pbuf, 32, tvalue);
            } else {
#if defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__APPLE_CC__) || defined(__APPLE__) || defined(HAVE_SOLARIS)
               sprintf (pbuf, "%llu", value);
#else
               sprintf (pbuf, "%Lu", value);
#endif
            }
         }
      }

      if (parser->ArgusPrintXml) {
         sprintf (buf, " Bytes = \"%s\"", pbuf);
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH)
            len = strlen(pbuf);
         else {
            if (strlen(pbuf) > len) {
               pbuf[len - 1] = '*';
               pbuf[len]     = '\0';
            }  
         } 
         sprintf (buf, "%*.*s ", len, len, pbuf);
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintBytes (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintSrcBytes (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusMetricStruct *metric, *nsmetric;
 
   if (argus->hdr.type & ARGUS_MAR) {
      struct ArgusRecord *rec = (struct ArgusRecord *) argus->dsrs[0];
      long long value = 0;
      char pbuf[32];

      if (rec != NULL) {
         value = rec->argus_mar.bytesRcvd;
#if defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__APPLE_CC__) || defined(__APPLE__) || defined(HAVE_SOLARIS)
         sprintf (pbuf, "%llu", value);
#else
         sprintf (pbuf, "%Lu", value);
#endif
      } else
         bzero(pbuf, sizeof(pbuf));

      if (parser->ArgusPrintXml) {
         sprintf (buf, " BytesRcvd = \"%s\"", pbuf);
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
            len = strlen(pbuf);
         } else {
            if (strlen(pbuf) > len) {
               pbuf[len - 1] = '*';
               pbuf[len]     = '\0'; 
            }        
         }
         sprintf (buf, "%*.*s ", len, len, pbuf);
      }

   } else {
      char pbuf[32];
      bzero(pbuf, 4);

      if (argus && ((metric = (void *)argus->dsrs[ARGUS_METRIC_INDEX]) != NULL)) {
         long long value = metric->src.bytes;
         float fvalue = 0.0;

         if (parser->Pctflag && parser->ns) {
            if ((nsmetric = (void *)parser->ns->dsrs[ARGUS_METRIC_INDEX]) != NULL) {
               if (nsmetric->src.bytes > 0)
                  fvalue = (metric->src.bytes * 100.0) / (nsmetric->src.bytes * 1.0);
            }
         }

         if (parser->Pctflag && parser->ns) {
            sprintf (pbuf, "%.*f", parser->pflag, fvalue);
         } else {
            double tvalue = value * 1.0;
            if (parser->Hflag) {
               ArgusAbbreviateMetric(parser, pbuf, 32, tvalue);
            } else {
#if defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__APPLE_CC__) || defined(__APPLE__) || defined(HAVE_SOLARIS)
               sprintf (pbuf, "%llu", value);
#else
               sprintf (pbuf, "%Lu", value);
#endif
            }
         }
      }

      if (parser->ArgusPrintXml) {
         sprintf (buf, " SrcBytes = \"%s\"", pbuf);
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
            len = strlen(pbuf);
         } else {
            if (strlen(pbuf) > len) {
               pbuf[len - 1] = '*';
               pbuf[len]     = '\0'; 
            }        
         }
         sprintf (buf, "%*.*s ", len, len, pbuf);
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcBytes (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintDstBytes (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusMetricStruct *metric, *nsmetric;
 
   if (argus->hdr.type & ARGUS_MAR) {
      struct ArgusRecord *rec = (struct ArgusRecord *) argus->dsrs[0];
      long long value = 0;
      char pbuf[32];

      if (rec != NULL) {
         value = rec->argus_mar.dropped;
#if defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__APPLE_CC__) || defined(__APPLE__) || defined(HAVE_SOLARIS)
         sprintf (pbuf, "%llu", value);
#else
         sprintf (pbuf, "%Lu", value);
#endif
      } else
         bzero(pbuf, sizeof(pbuf));

      if (parser->ArgusPrintXml) {
         sprintf (buf, " PktsDropped = \"%s\"", pbuf);
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
            len = strlen(pbuf);
         } else {
            if (strlen(pbuf) > len) {
               pbuf[len - 1] = '*';
               pbuf[len]     = '\0';
            }
         }
         sprintf (buf, "%*.*s ", len, len, pbuf);
      }
 
   } else {
      char pbuf[32];
      bzero(pbuf, 4);

      if (argus && ((metric = (void *)argus->dsrs[ARGUS_METRIC_INDEX]) != NULL)) {
         long long value = metric->dst.bytes;
         float fvalue = 0.0;

         if (parser->Pctflag && parser->ns) {
            if ((nsmetric = (void *)parser->ns->dsrs[ARGUS_METRIC_INDEX]) != NULL) {
               if (nsmetric->dst.bytes > 0)
                  fvalue = (metric->dst.bytes * 100.0) / (nsmetric->dst.bytes * 1.0);
            }
         }

         if (parser->Pctflag && parser->ns) {
            sprintf (pbuf, "%.*f", parser->pflag, fvalue);
         } else {
            double tvalue = value * 1.0;
            if (parser->Hflag) {
               ArgusAbbreviateMetric(parser, pbuf, 32, tvalue);
            } else {
#if defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__APPLE_CC__) || defined(__APPLE__) || defined(HAVE_SOLARIS)
               sprintf (pbuf, "%llu", value);
#else
               sprintf (pbuf, "%Lu", value);
#endif
            }
         }
      }

      if (parser->ArgusPrintXml) {
         sprintf (buf, " DstBytes = \"%s\"", pbuf);
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
            len = strlen(pbuf);
         } else {
            if (strlen(pbuf) > len) {
               pbuf[len - 1] = '*';
               pbuf[len]     = '\0';
            }
         }
         sprintf (buf, "%*.*s ", len, len, pbuf);
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstBytes (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintAppBytes (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusMetricStruct *metric, *nsmetric;

   if (argus->hdr.type & ARGUS_MAR) {
      struct ArgusRecord *rec = (struct ArgusRecord *) argus->dsrs[0];
      long long value = 0;

      if (rec != NULL) 
         value = rec->argus_mar.bytes;

      if (parser->ArgusPrintXml) {
      } else {
         char pbuf[32];
#if defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__APPLE_CC__) || defined(__APPLE__) || defined(HAVE_SOLARIS)
         sprintf (pbuf, "%llu", value);
#else
         sprintf (pbuf, "%Lu", value);
#endif

         if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
            len = strlen(pbuf);
         } else {
            if (strlen(pbuf) > len) {
               pbuf[len - 1] = '*';
               pbuf[len]     = '\0';
            }
         }
         sprintf (buf, "%*.*s ", len, len, pbuf);
      }
 
 
   } else {
      char pbuf[32];
      bzero(pbuf, 4);
      if (argus && ((metric = (void *)argus->dsrs[ARGUS_METRIC_INDEX]) != NULL)) {
         long long value = metric->src.appbytes + metric->dst.appbytes;
         float fvalue = 0.0;

         if (parser->Pctflag && parser->ns) {
            if ((nsmetric = (void *)parser->ns->dsrs[ARGUS_METRIC_INDEX]) != NULL) {
               if (value > 0)
                  fvalue = (value * 100.0) / ((nsmetric->src.appbytes + nsmetric->dst.appbytes) * 1.0);
            }
         }

         if (parser->Pctflag && parser->ns) {
            sprintf (pbuf, "%.*f", parser->pflag, fvalue);
            sprintf (buf, "%*.*s ", len, len, pbuf);
         } else {
            double tvalue = value * 1.0;
            if (parser->Hflag) {
               ArgusAbbreviateMetric(parser, pbuf, 32, tvalue);
            } else {
#if defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__APPLE_CC__) || defined(__APPLE__) || defined(HAVE_SOLARIS)
               sprintf (pbuf, "%llu", value);
#else
               sprintf (pbuf, "%Lu", value);
#endif
            }
         }
      }

      if (parser->ArgusPrintXml) {
         sprintf (buf, " AppBytes = \"%s\"", pbuf);
      } else  {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
            len = strlen(pbuf);
         } else {
            if (strlen(pbuf) > len) {
               pbuf[len - 1] = '*';
               pbuf[len]     = '\0';
            }
         }
         sprintf (buf, "%*.*s ", len, len, pbuf);
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintAppBytes (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintSrcAppBytes (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusMetricStruct *metric, *nsmetric;
 
   if (argus->hdr.type & ARGUS_MAR) {
      struct ArgusRecord *rec = (struct ArgusRecord *) argus->dsrs[0];
      long long value = 0;
      char pbuf[32];

      if (rec != NULL) {
         value = rec->argus_mar.bufs;
#if defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__APPLE_CC__) || defined(__APPLE__) || defined(HAVE_SOLARIS)
         sprintf (pbuf, "%llu", value);
#else
         sprintf (pbuf, "%Lu", value);
#endif
      } else
         bzero(pbuf, sizeof(pbuf));

      if (parser->ArgusPrintXml) {
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
            len = strlen(pbuf);
         } else {
            if (strlen(pbuf) > len) {
               pbuf[len - 1] = '*';
               pbuf[len]     = '\0';
            }
         }
         sprintf (buf, "%*.*s ", len, len, pbuf);
      }

   } else {
      char pbuf[32];
      bzero(pbuf, 4);

      if (argus && ((metric = (void *)argus->dsrs[ARGUS_METRIC_INDEX]) != NULL)) {
         long long value = metric->src.appbytes;
         float fvalue = 0.0;

         if (parser->Pctflag && parser->ns) {
            if ((nsmetric = (void *)parser->ns->dsrs[ARGUS_METRIC_INDEX]) != NULL) {
               if (nsmetric->src.appbytes > 0)
                  fvalue = (metric->src.appbytes * 100.0) / (nsmetric->src.appbytes * 1.0);
            }
         }

         if (parser->Pctflag && parser->ns) {
            sprintf (pbuf, "%.*f", parser->pflag, fvalue);
         } else {
            double tvalue = value * 1.0;
            if (parser->Hflag) {
               ArgusAbbreviateMetric(parser, pbuf, 32, tvalue);
            } else {
#if defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__APPLE_CC__) || defined(__APPLE__) || defined(HAVE_SOLARIS)
               sprintf (pbuf, "%llu", value);
#else
               sprintf (pbuf, "%Lu", value);
#endif
            }
         }
      }

      if (parser->ArgusPrintXml) {
         sprintf (buf, " SrcAppBytes = \"%s\"", pbuf);
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
            len = strlen(pbuf);
         } else {
            if (strlen(pbuf) > len) {
               pbuf[len - 1] = '*';
               pbuf[len]     = '\0';
            }
         }
         sprintf (buf, "%*.*s ", len, len, pbuf);
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcAppBytes (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintDstAppBytes (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusMetricStruct *metric, *nsmetric;
 
   if (argus->hdr.type & ARGUS_MAR) {
      struct ArgusRecord *rec = (struct ArgusRecord *) argus->dsrs[0];
      long long value = 0;
      char pbuf[32];

      if (rec != NULL) {
         value = rec->argus_mar.queue;
#if defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__APPLE_CC__) || defined(__APPLE__) || defined(HAVE_SOLARIS)
         sprintf (pbuf, "%llu", value);
#else
         sprintf (pbuf, "%Lu", value);
#endif
      } else
         bzero(pbuf, sizeof(pbuf));

      if (parser->ArgusPrintXml) {
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
            len = strlen(pbuf);
         } else {
            if (strlen(pbuf) > len) {
               pbuf[len - 1] = '*';
               pbuf[len]     = '\0';
            }
         }
         sprintf (buf, "%*.*s ", len, len, pbuf);
      }
 
   } else {
      char pbuf[32];
      bzero(pbuf, 4);

      if (argus && ((metric = (void *)argus->dsrs[ARGUS_METRIC_INDEX]) != NULL)) {
         long long value = metric->dst.appbytes;
         float fvalue = 0.0;

         if (parser->Pctflag && parser->ns) {
            if ((nsmetric = (void *)parser->ns->dsrs[ARGUS_METRIC_INDEX]) != NULL) {
               if (nsmetric->dst.appbytes > 0)
                  fvalue = (metric->dst.appbytes * 100.0) / (nsmetric->dst.appbytes * 1.0);
            }
         }

         if (parser->Pctflag && parser->ns) {
            sprintf (pbuf, "%.*f", parser->pflag, fvalue);
         } else {
            double tvalue = value * 1.0;
            if (parser->Hflag) {
               ArgusAbbreviateMetric(parser, pbuf, 32, tvalue);
            } else {
#if defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__APPLE_CC__) || defined(__APPLE__) || defined(HAVE_SOLARIS)
               sprintf (pbuf, "%llu", value);
#else
               sprintf (pbuf, "%Lu", value);
#endif
            }
         }
      }

      if (parser->ArgusPrintXml) {
         sprintf (buf, " DstAppBytes = \"%s\"", pbuf);
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
            len = strlen(pbuf);
         } else {
            if (strlen(pbuf) > len) {
               pbuf[len - 1] = '*';
               pbuf[len]     = '\0';
            }
         }
         sprintf (buf, "%*.*s ", len, len, pbuf);
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstAppBytes (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintSrcPktSize (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusPacketSizeStruct *psize;
   char value[128];

   bzero(value, sizeof(value));

   if (argus->hdr.type & ARGUS_MAR) {

   } else {
      if ((psize = (struct ArgusPacketSizeStruct *)argus->dsrs[ARGUS_PSIZE_INDEX]) != NULL) {
         if (psize->hdr.subtype & ARGUS_PSIZE_HISTO) {
            int i, tpkts[8], count = 0, max = 0, tlen, tmax;

            for (i = 0; i < 8; i++) {
               tpkts[i] = psize->src.psize[i];
               count   += psize->src.psize[i];
               max = (max < psize->src.psize[i]) ? psize->src.psize[i] : max;
            }

            tlen  = ((len == 8) || (len == 16)) ? len : ((len < 16) ? 8 : 16);
            tmax  = ((tlen == 8)) ? 15 : 255;

            if (max > tmax)
               for (i = 0; i < 8; i++) {
                  if (tpkts[i]) {
                     tpkts[i] = (tpkts[i] * tmax) / max;
                     if (tpkts[i] == 0)
                        tpkts[i] = 1;
                  }
               } 

            switch (tlen) {
               case  8:
                  for (i = 0; i < 8; i++)
                     sprintf (&value[strlen(value)], "%1.1x", tpkts[i]);
                  break;

               case 16:
                  for (i = 0; i < 8; i++)
                     sprintf (&value[strlen(value)], "%2.2x", tpkts[i]);
                  break;
            }
         } else
            sprintf (value, " ");
      }
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcPktSize = \"%s\"", value);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(value);
      } else {
         if (strlen(value) > len) {
            value[len - 1] = '*';
            value[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, value);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcPktSize (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintSrcMaxPktSize (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusPacketSizeStruct *psize;
   char value[128];

   bzero(value, sizeof(value));

   if (argus->hdr.type & ARGUS_MAR) {

   } else {
      if ((psize = (struct ArgusPacketSizeStruct *)argus->dsrs[ARGUS_PSIZE_INDEX]) != NULL) {
         if (psize->src.psizemax > 0)
            sprintf (value, "%d", psize->src.psizemax);
         else
            sprintf (value, " ");
      }
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcMaxPktSize = \"%s\"", value);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(value);
      } else {
         if (strlen(value) > len) {
            value[len - 1] = '*';
            value[len]     = '\0';
         }
      }

      sprintf (buf, "%*.*s ", len, len, value);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcMaxPktSize (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintSrcMinPktSize (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusPacketSizeStruct *psize;
   char value[128];

   bzero(value, sizeof(value));

   if (argus->hdr.type & ARGUS_MAR) {

   } else {
      if ((psize = (struct ArgusPacketSizeStruct *)argus->dsrs[ARGUS_PSIZE_INDEX]) != NULL) {
         if (psize->src.psizemin > 0) 
            sprintf (value, "%d", psize->src.psizemin);
         else
            sprintf (value, " ");
      }
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcMinPktSize = \"%s\"", value);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(value);
      } else {
         if (strlen(value) > len) {
            value[len - 1] = '*';
            value[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, value);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcMinPktSize (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintSrcMeanPktSize (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusMetricStruct *metric = (struct ArgusMetricStruct *) argus->dsrs[ARGUS_METRIC_INDEX];
   char tmpbuf[128], *ptr = tmpbuf;
   long long pkts = 0, bytes = 0;
   float value = 0.0;
 
   if (argus->hdr.type & ARGUS_MAR) {

   } else {
      if (metric != NULL) {
         pkts  = metric->src.pkts;
         bytes = metric->src.bytes;
      }
   }

   if (pkts > 0) 
      value = (float)(bytes * 1.0)/(pkts * 1.0);

   if (parser->Hflag) {
      ArgusAbbreviateMetric(parser, ptr, 128, value);
   } else
      sprintf (ptr, "%.*f", parser->pflag, value);

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcMeanPktSize = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 
 
#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcMeanPktSize (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintDstPktSize (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusPacketSizeStruct *psize;
   char value[128];

   bzero(value, sizeof(value));

   if (argus->hdr.type & ARGUS_MAR) {

   } else {
      if ((psize = (struct ArgusPacketSizeStruct *)argus->dsrs[ARGUS_PSIZE_INDEX]) != NULL) {
         if (psize->hdr.subtype & ARGUS_PSIZE_HISTO) {
            int i, tpkts[8], count = 0, max = 0, tlen, tmax;

            for (i = 0; i < 8; i++) {
               tpkts[i] = psize->dst.psize[i];
               count   += psize->dst.psize[i];
               max = (max < psize->dst.psize[i]) ? psize->dst.psize[i] : max;
            }

            tlen  = ((len == 8) || (len == 16)) ? len : ((len < 16) ? 8 : 16);
            tmax  = ((tlen == 8)) ? 15 : 255;

            if (max > tmax)
               for (i = 0; i < 8; i++) {
                  if (tpkts[i]) {
                     tpkts[i] = (tpkts[i] * tmax) / max;
                     if (tpkts[i] == 0)
                        tpkts[i] = 1;
                  }
               } 

            switch (tlen) {
               case  8:
                  for (i = 0; i < 8; i++)
                     sprintf (&value[strlen(value)], "%1.1x", tpkts[i]);
                  break;

               case 16:
                  for (i = 0; i < 8; i++)
                     sprintf (&value[strlen(value)], "%2.2x", tpkts[i]);
                  break;
            }
         } else
            sprintf (value, " ");
      }
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstPktSize = \"%s\"", value);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(value);
      } else {
         if (strlen(value) > len) {
            value[len - 1] = '*';
            value[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, value);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstPktSize (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintDstMaxPktSize (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusPacketSizeStruct *psize;
   char value[128];

   bzero(value, sizeof(value));

   if (argus->hdr.type & ARGUS_MAR) {

   } else {
      if ((psize = (struct ArgusPacketSizeStruct *)argus->dsrs[ARGUS_PSIZE_INDEX]) != NULL) {
         if (psize->dst.psizemax > 0) 
            sprintf (value, "%d", psize->dst.psizemax);
         else
            sprintf (value, " ");
      }
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstMaxPktSize = \"%s\"", value);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(value);
      } else {
         if (strlen(value) > len) {
            value[len - 1] = '*';
            value[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, value);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstMaxPktSize (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintDstMinPktSize (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusPacketSizeStruct *psize;
   char value[128];

   bzero(value, sizeof(value));

   if (argus->hdr.type & ARGUS_MAR) {

   } else {
      if ((psize = (struct ArgusPacketSizeStruct *)argus->dsrs[ARGUS_PSIZE_INDEX]) != NULL) {
         if (psize->dst.psizemin > 0) 
            sprintf (value, "%d", psize->dst.psizemin);
         else
            sprintf (value, " ");
      }
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstMinPktSize = \"%s\"", value);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(value);
      } else {
         if (strlen(value) > len) {
            value[len - 1] = '*';
            value[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, value);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstMinPktSize (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintDstMeanPktSize (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusMetricStruct *metric = (struct ArgusMetricStruct *) argus->dsrs[ARGUS_METRIC_INDEX];
   char tmpbuf[128], *ptr = tmpbuf;
   long long pkts = 0, bytes = 0;
   float value = 0.0;
 
   if (argus->hdr.type & ARGUS_MAR) {

   } else {
      if (metric != NULL) {
         pkts  = metric->dst.pkts;
         bytes = metric->dst.bytes;
      }
   }

   if (pkts > 0) 
      value = (float)(bytes * 1.0)/(pkts * 1.0);

   if (parser->Hflag) {
      ArgusAbbreviateMetric(parser, ptr, 128, value);
   } else
      sprintf (ptr, "%.*f", parser->pflag, value);

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstMeanPktSize = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 
 
#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstMeanPktSize (%p, %p)", buf, argus);
#endif
}

#include <math.h>


void
ArgusPrintSrcIntPkt (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter = NULL; 
   char value[128];

   bzero(value, sizeof(value));
 
   if (argus->hdr.type & ARGUS_MAR) {
 
   } else {
      float meanval = 0.0;
      unsigned int n;

      if ((jitter = (struct ArgusJitterStruct *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL) {
         if ((n = (jitter->src.act.n + jitter->src.idle.n)) > 0) {
            meanval += ((jitter->src.act.meanval  * jitter->src.act.n) +
                        (jitter->src.idle.meanval * jitter->src.idle.n));
            meanval = meanval / n;
         }
         sprintf (value, "%.*f", parser->pflag, meanval/1000.0);   
      }
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcIntPkt = \"%s\"", value);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(value);
      } else {
         if (strlen(value) > len) {
            value[len - 1] = '*';
            value[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, value);
   } 
   
#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcIntPkt (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintSrcIntPktDist (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter;
   char value[128];

   bzero(value, sizeof(value));

   if (argus->hdr.type & ARGUS_MAR) {

   } else {
      if ((jitter = (struct ArgusJitterStruct *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL) {
         switch (jitter->hdr.subtype & (ARGUS_HISTO_EXP | ARGUS_HISTO_LINEAR)) {
            case ARGUS_HISTO_EXP: {
               int i, tpkts[8], count = 0, max = 0, tlen, tmax;

               for (i = 0; i < 8; i++) {
                  tpkts[i] = jitter->src.act.dist_union.fdist[i] + jitter->src.idle.dist_union.fdist[i];
                  count   += tpkts[i];
                  max = (max < tpkts[i]) ? tpkts[i] : max;
               }

               tlen  = ((len == 8) || (len == 16)) ? len : ((len < 16) ? 8 : 16);
               tmax  = ((tlen == 8)) ? 15 : 255;

               if (max > tmax)
                  for (i = 0; i < 8; i++) {
                     if (tpkts[i]) {
                        tpkts[i] = (tpkts[i] * tmax) / max;
                        if (tpkts[i] == 0)
                           tpkts[i] = 1;
                     }
                  }

               switch (tlen) {
                  case  8:
                     for (i = 0; i < 8; i++)
                        sprintf (&value[strlen(value)], "%1.1x", tpkts[i]);
                     break;

                  case 16:
                     for (i = 0; i < 8; i++)
                        sprintf (&value[strlen(value)], "%2.2x", tpkts[i]);
                     break;
               }
               break;
            }

            case ARGUS_HISTO_LINEAR: {
               struct ArgusHistoObject *ahist = &jitter->src.act.dist_union.linear;
               struct ArgusHistoObject *ihist = &jitter->src.idle.dist_union.linear;

               int i, tpkts[256], count = 0, max = 0;
               int tlen = ahist->bins, tmax = 8;;

               bzero(&tpkts, sizeof(tpkts));

               if (ahist->data)
                  for (i = 0; i < tlen; i++) {
                     tpkts[i] += ahist->data[i];
                     count   +=  ahist->data[i];
                     max = (max < tpkts[i]) ? tpkts[i] : max;
                  }

               if (ihist->data)
                  for (i = 0; i < tlen; i++) {
                     tpkts[i] += ihist->data[i];
                     count   += ihist->data[i];
                     max = (max < tpkts[i]) ? tpkts[i] : max;
                  }

               if (ahist->bits == 4)      tmax = 15;
               else if (ahist->bits == 8) tmax = 255;

               if (max > tmax)
                  for (i = 0; i < tlen; i++) {
                     if (tpkts[i]) {
                        tpkts[i] = (tpkts[i] * tmax) / max;
                        if (tpkts[i] == 0)
                           tpkts[i] = 1;
                     }
                  }

               switch (ahist->bits) {
                  case  4:
                     for (i = 0; i < tlen; i++)
                        sprintf (&value[strlen(value)], "%1.1x", tpkts[i]);
                     break;

                  case 8:
                     for (i = 0; i < tlen; i++)
                        sprintf (&value[strlen(value)], "%2.2x", tpkts[i]);
                     break;
               }
               break;
            }

            default:
               sprintf (value, " ");
               break;
         }
      }
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcIntDist = \"%s\"", value);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(value);
      } else {
         if (strlen(value) > len) {
            value[len - 1] = '*';
            value[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, value);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcIntPktDist (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintActiveSrcIntPktDist (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter;
   char value[128];

   bzero(value, sizeof(value));

   if (argus->hdr.type & ARGUS_MAR) {

   } else {
      if ((jitter = (struct ArgusJitterStruct *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL) {
         if (jitter->hdr.subtype & ARGUS_HISTO_EXP) {
            int i, tpkts[8], count = 0, max = 0, tlen, tmax;

            for (i = 0; i < 8; i++) {
               tpkts[i] = jitter->src.act.dist_union.fdist[i];
               count   += tpkts[i];
               max = (max < tpkts[i]) ? tpkts[i] : max;
            }

            tlen  = ((len == 8) || (len == 16)) ? len : ((len < 16) ? 8 : 16);
            tmax  = ((tlen == 8)) ? 15 : 255;

            if (max > tmax)
               for (i = 0; i < 8; i++) {
                  if (tpkts[i]) {
                     tpkts[i] = (tpkts[i] * tmax) / max;
                     if (tpkts[i] == 0)
                        tpkts[i] = 1;
                  }
               }

            switch (tlen) {
               case  8:
                  for (i = 0; i < 8; i++)
                     sprintf (&value[strlen(value)], "%1.1x", tpkts[i]);
                  break;

               case 16:
                  for (i = 0; i < 8; i++)
                     sprintf (&value[strlen(value)], "%2.2x", tpkts[i]);
                  break;
            }
         } else
            sprintf (value, " ");
      }
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcIntDist = \"%s\"", value);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(value);
      } else {
         if (strlen(value) > len) {
            value[len - 1] = '*';
            value[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, value);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintActiveSrcIntPktDist (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintIdleSrcIntPktDist (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter;
   char value[128];

   bzero(value, sizeof(value));

   if (argus->hdr.type & ARGUS_MAR) {

   } else {
      if ((jitter = (struct ArgusJitterStruct *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL) {
         if (jitter->hdr.subtype & ARGUS_HISTO_EXP) {
            int i, tpkts[8], count = 0, max = 0, tlen, tmax;

            for (i = 0; i < 8; i++) {
               tpkts[i] = jitter->src.idle.dist_union.fdist[i];
               count   += tpkts[i];
               max = (max < tpkts[i]) ? tpkts[i] : max;
            }

            tlen  = ((len == 8) || (len == 16)) ? len : ((len < 16) ? 8 : 16);
            tmax  = ((tlen == 8)) ? 15 : 255;

            if (max > tmax)
               for (i = 0; i < 8; i++) {
                  if (tpkts[i]) {
                     tpkts[i] = (tpkts[i] * tmax) / max;
                     if (tpkts[i] == 0)
                        tpkts[i] = 1;
                  }
               } 

            switch (tlen) {
               case  8:
                  for (i = 0; i < 8; i++)
                     sprintf (&value[strlen(value)], "%1.1x", tpkts[i]);
                  break;

               case 16:
                  for (i = 0; i < 8; i++)
                     sprintf (&value[strlen(value)], "%2.2x", tpkts[i]);
                  break;
            }
         } else
            sprintf (value, " ");
      }
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcIntDist = \"%s\"", value);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(value);
      } else {
         if (strlen(value) > len) {
            value[len - 1] = '*';
            value[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, value);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintIdleSrcIntPktDist (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintDstIntPkt (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter;
   char value[128];

   bzero(value, sizeof(value));

   if (argus->hdr.type & ARGUS_MAR) {
 
   } else {
      float meanval = 0.0;
      unsigned int n;

      if ((jitter = (struct ArgusJitterStruct *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL) {
         if ((n = (jitter->dst.act.n + jitter->dst.idle.n)) > 0) {
            if (jitter->dst.act.n && jitter->dst.idle.n) {
               meanval  = ((jitter->dst.act.meanval * jitter->dst.act.n) +
                          (jitter->dst.idle.meanval * jitter->dst.idle.n)) / n;
            } else {
               meanval = (jitter->dst.act.n) ? jitter->dst.act.meanval : jitter->dst.idle.meanval;
            } 

            sprintf (value, "%.*f", parser->pflag, meanval/1000.0);   
         }
      }
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstIntPkt = \"%s\"", value);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(value);
      } else {
         if (strlen(value) > len) {
            value[len - 1] = '*';
            value[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, value);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstIntPkt (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintDstIntPktDist (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter;
   char value[128];

   bzero(value, sizeof(value));

   if (argus->hdr.type & ARGUS_MAR) {

   } else {
      if ((jitter = (struct ArgusJitterStruct *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL) {
         switch (jitter->hdr.subtype & (ARGUS_HISTO_EXP | ARGUS_HISTO_LINEAR)) {
            case ARGUS_HISTO_EXP: {
               int i, tpkts[8], count = 0, max = 0, tlen, tmax;

               for (i = 0; i < 8; i++) {
                  tpkts[i] = jitter->dst.act.dist_union.fdist[i] + jitter->dst.idle.dist_union.fdist[i];
                  count   += tpkts[i];
                  max = (max < tpkts[i]) ? tpkts[i] : max;
               }

               tlen  = ((len == 8) || (len == 16)) ? len : ((len < 16) ? 8 : 16);
               tmax  = ((tlen == 8)) ? 15 : 255;

               if (max > tmax)
                  for (i = 0; i < 8; i++) {
                     if (tpkts[i]) {
                        tpkts[i] = (tpkts[i] * tmax) / max;
                        if (tpkts[i] == 0)
                           tpkts[i] = 1;
                     }
                  }

               switch (tlen) {
                  case  8:
                     for (i = 0; i < 8; i++)
                        sprintf (&value[strlen(value)], "%1.1x", tpkts[i]);
                     break;

                  case 16:
                     for (i = 0; i < 8; i++)
                        sprintf (&value[strlen(value)], "%2.2x", tpkts[i]);
                     break;
               }
               break;
            }

            case ARGUS_HISTO_LINEAR: {
               struct ArgusHistoObject *ahist = &jitter->dst.act.dist_union.linear;
               struct ArgusHistoObject *ihist = &jitter->dst.idle.dist_union.linear;

               int i, tpkts[256], count = 0, max = 0;
               int tlen = ahist->bins, tmax = 8;;

               bzero(&tpkts, sizeof(tpkts));

               if (ahist->data)
                  for (i = 0; i < tlen; i++) {
                     tpkts[i] += ahist->data[i];
                     count   +=  ahist->data[i];
                     max = (max < tpkts[i]) ? tpkts[i] : max;
                  }

               if (ihist->data)
                  for (i = 0; i < tlen; i++) {
                     tpkts[i] += ihist->data[i];
                     count   += ihist->data[i];
                     max = (max < tpkts[i]) ? tpkts[i] : max;
                  }

               if (ahist->bits == 4)      tmax = 15;
               else if (ahist->bits == 8) tmax = 255;

               if (max > tmax)
                  for (i = 0; i < tlen; i++) {
                     if (tpkts[i]) {
                        tpkts[i] = (tpkts[i] * tmax) / max;
                        if (tpkts[i] == 0)
                           tpkts[i] = 1;
                     }
                  }

               switch (ahist->bits) {
                  case  4:
                     for (i = 0; i < tlen; i++)
                        sprintf (&value[strlen(value)], "%1.1x", tpkts[i]);
                     break;

                  case 8:
                     for (i = 0; i < tlen; i++)
                        sprintf (&value[strlen(value)], "%2.2x", tpkts[i]);
                     break;
               }
               break;
            }

            default:
               sprintf (value, " ");
               break;
         }
      }
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcIntDist = \"%s\"", value);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(value);
      } else {
         if (strlen(value) > len) {
            value[len - 1] = '*';
            value[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, value);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstIntPktDist (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintActiveDstIntPktDist (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter;
   char value[128];

   bzero(value, sizeof(value));

   if (argus->hdr.type & ARGUS_MAR) {

   } else {
      if ((jitter = (struct ArgusJitterStruct *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL) {
         if (jitter->hdr.subtype & ARGUS_HISTO_EXP) {
            int i, tpkts[8], count = 0, max = 0, tlen, tmax;

            for (i = 0; i < 8; i++) {
               tpkts[i] = jitter->dst.act.dist_union.fdist[i];
               count   += tpkts[i];
               max = (max < tpkts[i]) ? tpkts[i] : max;
            }

            tlen  = ((len == 8) || (len == 16)) ? len : ((len < 16) ? 8 : 16);
            tmax  = ((tlen == 8)) ? 15 : 255;

            if (max > tmax)
               for (i = 0; i < 8; i++) {
                  if (tpkts[i]) {
                     tpkts[i] = (tpkts[i] * tmax) / max;
                     if (tpkts[i] == 0)
                        tpkts[i] = 1;
                  }
               } 

            switch (tlen) {
               case  8:
                  for (i = 0; i < 8; i++)
                     sprintf (&value[strlen(value)], "%1.1x", tpkts[i]);
                  break;

               case 16:
                  for (i = 0; i < 8; i++)
                     sprintf (&value[strlen(value)], "%2.2x", tpkts[i]);
                  break;
            }
         } else
            sprintf (value, " ");
      }
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcIntDist = \"%s\"", value);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(value);
      } else {
         if (strlen(value) > len) {
            value[len - 1] = '*';
            value[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, value);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintActiveDstIntPktDist (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintIdleDstIntPktDist (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter;
   char value[128];

   bzero(value, sizeof(value));

   if (argus->hdr.type & ARGUS_MAR) {

   } else {
      if ((jitter = (struct ArgusJitterStruct *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL) {
         if (jitter->hdr.subtype & ARGUS_HISTO_EXP) {
            int i, tpkts[8], count = 0, max = 0, tlen, tmax;

            for (i = 0; i < 8; i++) {
               tpkts[i] = jitter->dst.idle.dist_union.fdist[i];
               count   += tpkts[i];
               max = (max < tpkts[i]) ? tpkts[i] : max;
            }

            tlen  = ((len == 8) || (len == 16)) ? len : ((len < 16) ? 8 : 16);
            tmax  = ((tlen == 8)) ? 15 : 255;

            if (max > tmax)
               for (i = 0; i < 8; i++) {
                  if (tpkts[i]) {
                     tpkts[i] = (tpkts[i] * tmax) / max;
                     if (tpkts[i] == 0)
                        tpkts[i] = 1;
                  }
               } 

            switch (tlen) {
               case  8:
                  for (i = 0; i < 8; i++)
                     sprintf (&value[strlen(value)], "%1.1x", tpkts[i]);
                  break;

               case 16:
                  for (i = 0; i < 8; i++)
                     sprintf (&value[strlen(value)], "%2.2x", tpkts[i]);
                  break;
            }
         } else
            sprintf (value, " ");
      }
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcIntDist = \"%s\"", value);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(value);
      } else {
         if (strlen(value) > len) {
            value[len - 1] = '*';
            value[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, value);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintIdleDstIntPktDist (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintActiveSrcIntPkt (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter; 
   char value[128];
   bzero(value, sizeof(value));
 
   if (argus->hdr.type & ARGUS_MAR) {
 
   } else {
      if ((jitter = (struct ArgusJitterStruct *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL)
         sprintf (value, "%.*f", parser->pflag, jitter->src.act.meanval/1000.0);   
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcActiveIntPkt = \"%s\"", value);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(value);
      } else {
         if (strlen(value) > len) {
            value[len - 1] = '*';
            value[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, value);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintActiveSrcIntPkt (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintActiveDstIntPkt (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter;
   char value[128];

   bzero(value, sizeof(value));

   if (argus->hdr.type & ARGUS_MAR) {
 
   } else {
      if ((jitter = (struct ArgusJitterStruct *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL)
         sprintf (value, "%.*f", parser->pflag, jitter->dst.act.meanval/1000.0);   
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstActiveIntPkt = \"%s\"", value);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(value);
      } else {
         if (strlen(value) > len) {
            value[len - 1] = '*';
            value[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, value);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintActiveDstIntPkt (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintIdleSrcIntPkt (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter; 
   char value[128];

   bzero(value, sizeof(value));
 
   if (argus->hdr.type & ARGUS_MAR) {
   } else {
      if ((jitter = (struct ArgusJitterStruct *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL)
         sprintf (value, "%.*f", parser->pflag, jitter->src.idle.meanval/1000.0);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcIdleIntPkt = \"%s\"", value);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(value);
      } else {
         if (strlen(value) > len) {
            value[len - 1] = '*';
            value[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, value);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintIdleSrcIntPkt (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintIdleDstIntPkt (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter;
   char value[128];

   bzero (value, sizeof(value));

   if (argus->hdr.type & ARGUS_MAR) {
      if (parser->ArgusPrintXml) {
      } else {
         sprintf (buf, "%*.*s ", len, len, " ");   
      }
 
   } else  {
      if ((jitter = (struct ArgusJitterStruct *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL)
         sprintf (value, "%.*f", parser->pflag, jitter->dst.idle.meanval/1000.0);   

      if (parser->ArgusPrintXml) {
         sprintf (buf, " DstIdleIntPkt = \"%s\"", value);
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
            len = strlen(value);
         } else {
            if (strlen(value) > len) {
               value[len - 1] = '*';
               value[len]     = '\0';
            }
         }
         sprintf (buf, "%*.*s ", len, len, value);
      } 
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintIdleDstIntPkt (%p, %p)", buf, argus);
#endif
}

/*
struct ArgusStatObject {
   int n;
   unsigned int minval;
   float meanval;
   float stdev;
   unsigned int maxval;
}; 
*/

void
ArgusPrintSrcIntPktMax (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter;
   char value[128];

   bzero (value, sizeof(value));

   if (argus->hdr.type & ARGUS_MAR) {
      if (parser->ArgusPrintXml) {
      } else {
         sprintf (buf, "%*.*s ", len, len, " ");
      }

   } else {
      if ((jitter = (struct ArgusJitterStruct *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL) {
         float maxval = (jitter->src.act.maxval > jitter->src.idle.maxval) ?
                         jitter->src.act.maxval : jitter->src.idle.maxval;
         sprintf (value, "%.*f", parser->pflag, maxval/1000.0);
      }

      if (parser->ArgusPrintXml) {
         sprintf (buf, " SrcIntPktMax = \"%s\"", value);
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
            len = strlen(value);
         } else {
            if (strlen(value) > len) {
               value[len - 1] = '*';
               value[len]     = '\0';
            }
         }
         sprintf (buf, "%*.*s ", len, len, value);
      } 
   }
#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcIntPktMax (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintSrcIntPktMin (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter;
   char value[128];

   bzero (value, sizeof(value));

   if (argus->hdr.type & ARGUS_MAR) {
      if (parser->ArgusPrintXml) {
      } else {
         sprintf (buf, "%*.*s ", len, len, " ");
      }

   } else {
      if ((jitter = (struct ArgusJitterStruct *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL) {
         float minval = (jitter->src.act.minval > jitter->src.idle.minval) ?
                         jitter->src.act.minval : jitter->src.idle.minval;
         sprintf (value, "%.*f", parser->pflag, minval/1000.0);
      }

      if (parser->ArgusPrintXml) {
         sprintf (buf, " SrcIntPktMin = \"%s\"", value);
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
            len = strlen(value);
         } else {
            if (strlen(value) > len) {
               value[len - 1] = '*';
               value[len]     = '\0';
            }
         }
         sprintf (buf, "%*.*s ", len, len, value);
      } 
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcIntPktMin (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintDstIntPktMax (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter;
   char value[128]; 
   bzero(value, sizeof(value));

   if (argus->hdr.type & ARGUS_MAR) {

   } else {
      if ((jitter = (struct ArgusJitterStruct *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL)
         sprintf (value, "%.*f", parser->pflag, jitter->dst.act.maxval/1000.0);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstIntPktMax = \"%s\"", value);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(value);
      } else {
         if (strlen(value) > len) {
            value[len - 1] = '*';
            value[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, value);
   } 
   
#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstIntPktMax (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintDstIntPktMin (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter;
   char value[128]; 
   bzero(value, sizeof(value));

   if (argus->hdr.type & ARGUS_MAR) {

   } else {
      if ((jitter = (struct ArgusJitterStruct *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL)
         sprintf (value, "%.*f", parser->pflag, jitter->dst.act.minval/1000.0);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstIntPktMin = \"%s\"", value);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(value);
      } else {
         if (strlen(value) > len) {
            value[len - 1] = '*';
            value[len]     = '\0';
         }  
      } 
      sprintf (buf, "%*.*s ", len, len, value);
   } 
   
#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstIntPktMin (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintActiveSrcIntPktMax (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter;
   char value[128];
   bzero(value, sizeof(value));

   if (argus->hdr.type & ARGUS_MAR) {

   } else {
      if ((jitter = (struct ArgusJitterStruct *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL)
         sprintf (value, "%.*f", parser->pflag, jitter->src.act.maxval/1000.0);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcActIntPktMax = \"%s\"", value);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(value);
      } else {
         if (strlen(value) > len) {
            value[len - 1] = '*';
            value[len]     = '\0';
         }  
      } 
      sprintf (buf, "%*.*s ", len, len, value);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintActiveSrcIntPktMax (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintActiveSrcIntPktMin (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter;
   char value[128];
   bzero(value, sizeof(value));

   if (argus->hdr.type & ARGUS_MAR) {

   } else {
      if ((jitter = (struct ArgusJitterStruct *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL)
         sprintf (value, "%.*f", parser->pflag, jitter->src.act.minval/1000.0);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcActIntPktMin = \"%s\"", value);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(value);
      } else {
         if (strlen(value) > len) {
            value[len - 1] = '*';
            value[len]     = '\0';
         }  
      } 
      sprintf (buf, "%*.*s ", len, len, value);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintActiveSrcIntPktMin (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintActiveDstIntPktMax (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter;
   char value[128];
   bzero(value, sizeof(value));

   if (argus->hdr.type & ARGUS_MAR) {

   } else {
      if ((jitter = (struct ArgusJitterStruct *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL)
         sprintf (value, "%.*f", parser->pflag, jitter->dst.act.maxval/1000.0);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstActIntPktMax = \"%s\"", value);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(value);
      } else {
         if (strlen(value) > len) {
            value[len - 1] = '*';
            value[len]     = '\0';
         }  
      } 
      sprintf (buf, "%*.*s ", len, len, value);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintActiveDstIntPktMax (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintActiveDstIntPktMin (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter;
   char value[128];
   bzero(value, sizeof(value));

   if (argus->hdr.type & ARGUS_MAR) {

   } else {
      if ((jitter = (struct ArgusJitterStruct *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL)
         sprintf (value, "%.*f", parser->pflag, jitter->dst.act.minval/1000.0);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstActIntPktMin = \"%s\"", value);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(value);
      } else {
         if (strlen(value) > len) {
            value[len - 1] = '*';
            value[len]     = '\0';
         }  
      } 
      sprintf (buf, "%*.*s ", len, len, value);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintActiveDstIntPktMin (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintIdleSrcIntPktMax (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter;
   char value[128];
   bzero(value, sizeof(value));

   if (argus->hdr.type & ARGUS_MAR) {

   } else {
      if ((jitter = (struct ArgusJitterStruct *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL)
         sprintf (value, "%.*f", parser->pflag, jitter->src.idle.maxval/1000.0);
   }

   if (parser->ArgusPrintXml) {
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(value);
      } else {
         if (strlen(value) > len) {
            value[len - 1] = '*';
            value[len]     = '\0';
         }  
      } 
      sprintf (buf, "%*.*s ", len, len, value);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintIdleSrcIntPktMax (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintIdleSrcIntPktMin (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter;
   char value[128];
   bzero(value, sizeof(value));

   if (argus->hdr.type & ARGUS_MAR) {

   } else {
      if ((jitter = (struct ArgusJitterStruct *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL)
         sprintf (value, "%.*f", parser->pflag, jitter->src.idle.minval/1000.0);
   }

   if (parser->ArgusPrintXml) {
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(value);
      } else {
         if (strlen(value) > len) {
            value[len - 1] = '*';
            value[len]     = '\0';
         }  
      } 
      sprintf (buf, "%*.*s ", len, len, value);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintIdleSrcIntPktMin (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintIdleDstIntPktMax (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter;
   char value[128];
   bzero(value, sizeof(value));

   if (argus->hdr.type & ARGUS_MAR) {

   } else {
      if ((jitter = (struct ArgusJitterStruct *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL)
         sprintf (value, "%.*f", parser->pflag, jitter->dst.idle.maxval/1000.0);
   }

   if (parser->ArgusPrintXml) {
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(value);
      } else {
         if (strlen(value) > len) {
            value[len - 1] = '*';
            value[len]     = '\0';
         }  
      } 
      sprintf (buf, "%*.*s ", len, len, value);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintIdleDstIntPktMax (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintIdleDstIntPktMin (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter;
   char value[128];
   bzero(value, sizeof(value));

   if (argus->hdr.type & ARGUS_MAR) {

   } else {
      if ((jitter = (struct ArgusJitterStruct *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL)
         sprintf (value, "%.*f", parser->pflag, jitter->dst.idle.minval/1000.0);
   }

   if (parser->ArgusPrintXml) {
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(value);
      } else {
         if (strlen(value) > len) {
            value[len - 1] = '*';
            value[len]     = '\0';
         }  
      } 
      sprintf (buf, "%*.*s ", len, len, value);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintIdleDstIntPktMin (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintSrcJitter (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter;

   if (argus->hdr.type & ARGUS_MAR) {
      if (parser->ArgusPrintXml) {
      } else {
         sprintf (buf, "%*.*s ", len, len, " ");   
      }
 
   } else {
      char value[128];
      bzero(value, sizeof(value));

      if (argus && ((jitter = (void *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL)) {
         double stdev = 0.0, sumsqrd1 = 0.0, sumsqrd2 = 0.0, sumsqrd;
         unsigned int n;
         float meanval;

         if ((n = (jitter->src.act.n + jitter->src.idle.n)) > 0) {
            if (jitter->src.act.n && jitter->src.idle.n) {
               meanval  = ((jitter->src.act.meanval * jitter->src.act.n) +
                          (jitter->src.idle.meanval * jitter->src.idle.n)) / n;

               if (jitter->src.act.n) {
                  stdev = jitter->src.act.stdev;
                  sumsqrd1 = (jitter->src.act.n * pow(stdev, 2.0)) +
                              pow((jitter->src.act.meanval * jitter->src.act.n), 2.0)/jitter->src.act.n;
               }

               if (jitter->src.idle.n) {
                  stdev = jitter->src.idle.stdev;
                  sumsqrd2 = (jitter->src.idle.n * pow(stdev, 2.0)) +
                              pow((jitter->src.idle.meanval * jitter->src.idle.n), 2.0)/jitter->src.idle.n;
               }

               sumsqrd = sumsqrd1 + sumsqrd2;
               sumsqrd = sumsqrd / 1000;
               meanval = meanval / 1000.0;
               stdev   = ((sqrt ((sumsqrd/n) - pow (meanval, 2.0))) * 1);

            } else {
               stdev = (jitter->src.act.n) ? jitter->src.act.stdev : jitter->src.idle.stdev;
               stdev = stdev / 1000;
            }

            sprintf (value, "%.*f", parser->pflag, stdev);
         }
      }

      if (parser->ArgusPrintXml) {
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
            len = strlen(value);
         } else {
            if (strlen(value) > len) {
               value[len - 1] = '*';
               value[len]     = '\0';
            }  
         } 
         sprintf (buf, "%*.*s ", len, len, value);
      } 
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcJitter (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintDstJitter (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter;

   if (argus->hdr.type & ARGUS_MAR) {
      if (parser->ArgusPrintXml) {
      } else {
         sprintf (buf, "%*.*s ", len, len, " ");   
      }
 
   } else {
      double stdev = 0.0, sumsqrd1 = 0.0, sumsqrd2 = 0.0, sumsqrd;
      unsigned int n;
      float meanval;
      char sbuf[128];
      bzero(sbuf, 32);

      if (argus && ((jitter = (void *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL)) {
         if ((n = (jitter->dst.act.n + jitter->dst.idle.n)) > 0) {
            if (jitter->dst.act.n && jitter->dst.idle.n) {
               meanval  = ((jitter->dst.act.meanval * jitter->dst.act.n) +
                          (jitter->dst.idle.meanval * jitter->dst.idle.n)) / n;

               if (jitter->dst.act.n) {
                  stdev = jitter->dst.act.stdev;
                  sumsqrd1 = (jitter->dst.act.n * pow(stdev, 2.0)) +
                              pow((jitter->dst.act.meanval * jitter->dst.act.n), 2.0)/jitter->dst.act.n;
               }

               if (jitter->dst.idle.n) {
                  stdev = jitter->dst.idle.stdev;
                  sumsqrd2 = (jitter->dst.idle.n * pow(jitter->dst.idle.stdev, 2.0)) +
                              pow((jitter->dst.idle.meanval * jitter->dst.idle.n), 2.0)/jitter->dst.idle.n;
               }

               sumsqrd = sumsqrd1 + sumsqrd2;
               sumsqrd = sumsqrd / 1000;
               meanval = meanval / 1000.0;
               stdev   = ((sqrt ((sumsqrd/n) - pow (meanval, 2.0))) * 1);

            } else {
               stdev = (jitter->dst.act.n) ? jitter->dst.act.stdev : jitter->dst.idle.stdev;
               stdev = stdev / 1000.0;
            }

            sprintf (sbuf, "%.*f", parser->pflag, stdev);
         }
      }

      if (parser->ArgusPrintXml) {
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
            len = strlen(sbuf);
         } else {
            if (strlen(sbuf) > len) {
               sbuf[len - 1] = '*';
               sbuf[len]     = '\0';
            }  
         } 
         sprintf (buf, "%*.*s ", len, len, sbuf);
      } 
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstJitter (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintActiveSrcJitter (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter; 
 
   if (argus->hdr.type & ARGUS_MAR) {
      if (parser->ArgusPrintXml) {
      } else {
         sprintf (buf, "%*.*s ", len, len, " ");
      }  
 
   } else {
      char sbuf[128];
      bzero(sbuf, 4);

      if (argus && ((jitter = (void *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL)) {
         double stdev = 0;
         if (jitter->src.act.n > 0) {
            stdev = jitter->src.act.stdev/1000.0;
            sprintf (sbuf, "%.*f", parser->pflag, stdev);
         }
      }

      if (parser->ArgusPrintXml) {
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
            len = strlen(sbuf);
         } else {
            if (strlen(sbuf) > len) {
               sbuf[len - 1] = '*';
               sbuf[len]     = '\0';
            }
         }
         sprintf (buf, "%*.*s ", len, len, sbuf);
      } 
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintActiveSrcJitter (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintActiveDstJitter (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter;

   if (argus->hdr.type & ARGUS_MAR) {
      if (parser->ArgusPrintXml) {
      } else 
         sprintf (buf, "%*.*s ", len, len, " ");   
 
   } else {
      char sbuf[128];
      bzero(sbuf, 4);

      if (argus && ((jitter = (void *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL)) {
         double stdev = 0;

         if (jitter->dst.act.n > 0)  {
            stdev = jitter->dst.act.stdev/1000.0;
            sprintf (sbuf, "%.*f", parser->pflag, stdev);
         }
      }

      if (parser->ArgusPrintXml) {
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
            len = strlen(sbuf);
         } else {
            if (strlen(sbuf) > len) {
               sbuf[len - 1] = '*';
               sbuf[len]     = '\0';
            }
         }
         sprintf (buf, "%*.*s ", len, len, sbuf);
      } 
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintActiveDstJitter (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintIdleSrcJitter (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter; 
 
   if (argus->hdr.type & ARGUS_MAR) {
      if (parser->ArgusPrintXml) {
      } else {
         sprintf (buf, "%*.*s ", len, len, " ");
      }  
 
   } else {
      char sbuf[128];
      bzero(sbuf, 4);

      if (argus && ((jitter = (void *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL)) {
         double stdev = 0;
         if (jitter->src.idle.n > 0) {
            stdev = jitter->src.idle.stdev/1000.0;
            sprintf (sbuf, "%.*f", parser->pflag, stdev);
         }
      }

      if (parser->ArgusPrintXml) {
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
            len = strlen(sbuf);
         } else {
            if (strlen(sbuf) > len) {
               sbuf[len - 1] = '*';
               sbuf[len]     = '\0';
            }
         }
         sprintf (buf, "%*.*s ", len, len, sbuf);
      } 
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintIdleSrcJitter (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintIdleDstJitter (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusJitterStruct *jitter;

   if (argus->hdr.type & ARGUS_MAR) {
      if (parser->ArgusPrintXml) {
      } else {
         sprintf (buf, "%*.*s ", len, len, " ");   
      }
 
   } else {
      char sbuf[128];
      bzero(sbuf, 4);

      if (argus && ((jitter = (void *)argus->dsrs[ARGUS_JITTER_INDEX]) != NULL)) {
         double stdev = 0;

         if (jitter->dst.idle.n > 0) {
            stdev = jitter->dst.idle.stdev/1000.0;
            sprintf (sbuf, "%.*f", parser->pflag, stdev);
         }
      }

      if (parser->ArgusPrintXml) {
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
            len = strlen(sbuf);
         } else {
            if (strlen(sbuf) > len) {
               sbuf[len - 1] = '*';
               sbuf[len]     = '\0';
            }
         }
         sprintf (buf, "%*.*s ", len, len, sbuf);
      } 
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstJitter (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintSrcRate (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusMetricStruct *metric = (struct ArgusMetricStruct *) argus->dsrs[ARGUS_METRIC_INDEX];
   char tmpbuf[128], *ptr = tmpbuf;
   float seconds = 0.0, load = 0.0;
   long long count = 0;
 
   if (argus->hdr.type & ARGUS_MAR) {

   } else {
      if (metric != NULL) {
         if ((seconds = RaGetFloatSrcDuration(argus)) == 0.0)
            seconds = RaGetFloatDuration(argus);
         count = metric->src.pkts - 1;
      }
   }

   if ((count > 0) && (seconds > 0))
      load = (float)(count/seconds);

   if (parser->Hflag) {
      ArgusAbbreviateMetric(parser, ptr, 128, load);
   } else
      sprintf (ptr, "%.*f", parser->pflag, load);

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcRate = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 
 
#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcRate (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintDstRate (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusMetricStruct *metric = (struct ArgusMetricStruct *) argus->dsrs[ARGUS_METRIC_INDEX];
   char tmpbuf[128], *ptr = tmpbuf;
   float seconds = 0.0, load = 0.0;
   long long count = 0;
 
   if (argus->hdr.type & ARGUS_MAR) {

   } else {
      if (metric != NULL) {
         if ((seconds = RaGetFloatDstDuration(argus)) == 0.0)
            seconds = RaGetFloatDuration(argus);
         count = metric->dst.pkts - 1;
      }
   }

   if ((count > 0) && (seconds > 0.0))
      load = (float)(count/seconds);

   if (parser->Hflag) {
      ArgusAbbreviateMetric(parser, ptr, 128, load);
   } else
      sprintf (ptr, "%.*f", parser->pflag, load);
 
   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstRate = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 
 
#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstRate (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintRate (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusMetricStruct *metric = (struct ArgusMetricStruct *) argus->dsrs[ARGUS_METRIC_INDEX];
   char tmpbuf[128], *ptr = tmpbuf;
   float seconds = 0.0, load = 0.0;
   long long pkts = 0;
 
   if (argus->hdr.type & ARGUS_MAR) {

   } else {
      if (metric != NULL) {
         seconds = RaGetFloatDuration(argus);
         pkts = (metric->src.pkts + metric->dst.pkts) - 1;
      }
   }

   if ((pkts > 0) && (seconds > 0))
      load = (double)(pkts/seconds);

   if (parser->Hflag) {
      ArgusAbbreviateMetric(parser, ptr, 128, load);
   } else
      sprintf (ptr, "%.*f", parser->pflag, load);
 
   if (parser->ArgusPrintXml) {
      sprintf (buf, " Rate = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 
 
#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintRate (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintSrcLoss (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;

   bzero(tmpbuf, sizeof(tmpbuf));

   if (parser->Pctflag) {
      double ploss = ArgusFetchPercentSrcLoss(argus);
      sprintf (ptr, "%.*f", parser->pflag, ploss);

   } else {
      double ploss = ArgusFetchSrcLoss(argus);
      int loss = ploss;
      sprintf (ptr, "%d", loss);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcLoss = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcLoss (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintDstLoss (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;

   bzero(tmpbuf, sizeof(tmpbuf));

   if (parser->Pctflag) {
      double ploss = ArgusFetchPercentDstLoss(argus);
      sprintf (ptr, "%3.*f", parser->pflag, ploss);

   } else {
      double ploss = ArgusFetchDstLoss(argus);
      int loss = ploss;
      sprintf (ptr, "%d", loss);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstLoss = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstLoss (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintLoss (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;

   bzero(tmpbuf, sizeof(tmpbuf));

   if (parser->Pctflag) {
      double ploss = ArgusFetchPercentLoss(argus);
      sprintf (ptr, "%3.*f", parser->pflag, ploss);

   } else {
      double ploss = ArgusFetchLoss(argus);
      int loss = ploss;
      sprintf (ptr, "%d", loss);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " Loss = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintLoss (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintSrcRetrans (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;

   bzero(tmpbuf, sizeof(tmpbuf));

   if (parser->Pctflag) {
      double pretrans = ArgusFetchPercentSrcRetrans(argus);
      sprintf (ptr, "%.*f", parser->pflag, pretrans);

   } else {
      double pretrans = ArgusFetchSrcRetrans(argus);
      int retrans = pretrans;
      sprintf (ptr, "%d", retrans);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcRetrans = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcRetrans (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintDstRetrans (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;

   bzero(tmpbuf, sizeof(tmpbuf));

   if (parser->Pctflag) {
      double pretrans = ArgusFetchPercentDstRetrans(argus);
      sprintf (ptr, "%3.*f", parser->pflag, pretrans);

   } else {
      double pretrans = ArgusFetchDstRetrans(argus);
      int retrans = pretrans;
      sprintf (ptr, "%d", retrans);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstRetrans = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstRetrans (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintRetrans (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;

   bzero(tmpbuf, sizeof(tmpbuf));

   if (parser->Pctflag) {
      double pretrans = ArgusFetchPercentRetrans(argus);
      sprintf (ptr, "%3.*f", parser->pflag, pretrans);

   } else {
      double pretrans = ArgusFetchRetrans(argus);
      int retrans = pretrans;
      sprintf (ptr, "%d", retrans);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " Retrans = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintRetrans (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintPercentSrcRetrans (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;
   double pretrans = ArgusFetchPercentSrcRetrans(argus);

   bzero(tmpbuf, sizeof(tmpbuf));
   sprintf (ptr, "%3.*f", parser->pflag, pretrans);

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcPctRetrans = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintPercentSrcRetrans (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintPercentDstRetrans (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;
   double pretrans = ArgusFetchPercentDstRetrans(argus);

   bzero(tmpbuf, sizeof(tmpbuf));
   sprintf (ptr, "%3.*f", parser->pflag, pretrans);

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstPctRetrans = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintPercentDstRetrans (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintPercentRetrans (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;
   double pretrans = ArgusFetchPercentRetrans(argus);

   bzero(tmpbuf, sizeof(tmpbuf));
   sprintf (ptr, "%3.*f", parser->pflag, pretrans);

   if (parser->ArgusPrintXml) {
      sprintf (buf, " PctRetrans = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintPercentRetrans (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintSrcNacks (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;

   bzero(tmpbuf, sizeof(tmpbuf));

   if (parser->Pctflag) {
      double pnacks = ArgusFetchPercentSrcNacks(argus);
      sprintf (ptr, "%.*f", parser->pflag, pnacks);

   } else {
      double pnacks = ArgusFetchSrcNacks(argus);
      int nacks = pnacks;
      sprintf (ptr, "%d", nacks);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcNacks = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcNacks (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintDstNacks (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;

   bzero(tmpbuf, sizeof(tmpbuf));

   if (parser->Pctflag) {
      double pnacks = ArgusFetchPercentDstNacks(argus);
      sprintf (ptr, "%3.*f", parser->pflag, pnacks);

   } else {
      double pnacks = ArgusFetchDstNacks(argus);
      int nacks = pnacks;
      sprintf (ptr, "%d", nacks);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstNacks = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstNacks (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintNacks (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;

   bzero(tmpbuf, sizeof(tmpbuf));

   if (parser->Pctflag) {
      double pnacks = ArgusFetchPercentNacks(argus);
      sprintf (ptr, "%3.*f", parser->pflag, pnacks);

   } else {
      double pnacks = ArgusFetchNacks(argus);
      int nacks = pnacks;
      sprintf (ptr, "%d", nacks);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " Nacks = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintNacks (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintPercentSrcNacks (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;
   double pnacks = ArgusFetchPercentSrcNacks(argus);

   bzero(tmpbuf, sizeof(tmpbuf));
   sprintf (ptr, "%3.*f", parser->pflag, pnacks);

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcPctNacks = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintPercentSrcNacks (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintPercentDstNacks (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;
   double pnacks = ArgusFetchPercentDstNacks(argus);

   bzero(tmpbuf, sizeof(tmpbuf));
   sprintf (ptr, "%3.*f", parser->pflag, pnacks);

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstPctNacks = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintPercentDstNacks (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintPercentNacks (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;
   double pnacks = ArgusFetchPercentNacks(argus);

   bzero(tmpbuf, sizeof(tmpbuf));
   sprintf (ptr, "%3.*f", parser->pflag, pnacks);

   if (parser->ArgusPrintXml) {
      sprintf (buf, " PctNacks = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintPercentNacks (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintSrcSolo (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;

   bzero(tmpbuf, sizeof(tmpbuf));

   if (parser->Pctflag) {
      double psolo = ArgusFetchPercentSrcSolo(argus);
      sprintf (ptr, "%.*f", parser->pflag, psolo);

   } else {
      double psolo = ArgusFetchSrcSolo(argus);
      int solo = psolo;
      sprintf (ptr, "%d", solo);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcSolo = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcSolo (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintDstSolo (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;

   bzero(tmpbuf, sizeof(tmpbuf));

   if (parser->Pctflag) {
      double psolo = ArgusFetchPercentDstSolo(argus);
      sprintf (ptr, "%3.*f", parser->pflag, psolo);

   } else {
      double psolo = ArgusFetchDstSolo(argus);
      int solo = psolo;
      sprintf (ptr, "%d", solo);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstSolo = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstSolo (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintSolo (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;

   bzero(tmpbuf, sizeof(tmpbuf));

   if (parser->Pctflag) {
      double psolo = ArgusFetchPercentSolo(argus);
      sprintf (ptr, "%3.*f", parser->pflag, psolo);

   } else {
      double psolo = ArgusFetchSolo(argus);
      int solo = psolo;
      sprintf (ptr, "%d", solo);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " Solo = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSolo (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintPercentSrcSolo (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;
   double psolo = ArgusFetchPercentSrcSolo(argus);

   bzero(tmpbuf, sizeof(tmpbuf));
   sprintf (ptr, "%3.*f", parser->pflag, psolo);

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcPctSolo = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintPercentSrcSolo (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintPercentDstSolo (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;
   double psolo = ArgusFetchPercentDstSolo(argus);

   bzero(tmpbuf, sizeof(tmpbuf));
   sprintf (ptr, "%3.*f", parser->pflag, psolo);

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstPctSolo = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintPercentDstSolo (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintPercentSolo (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;
   double psolo = ArgusFetchPercentSolo(argus);

   bzero(tmpbuf, sizeof(tmpbuf));
   sprintf (ptr, "%3.*f", parser->pflag, psolo);

   if (parser->ArgusPrintXml) {
      sprintf (buf, " PctSolo = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintPercentSolo (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintSrcFirst (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;

   bzero(tmpbuf, sizeof(tmpbuf));

   if (parser->Pctflag) {
      double pfirst = ArgusFetchPercentSrcFirst(argus);
      sprintf (ptr, "%.*f", parser->pflag, pfirst);

   } else {
      double pfirst = ArgusFetchSrcFirst(argus);
      int first = pfirst;
      sprintf (ptr, "%d", first);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcFirst = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcFirst (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintDstFirst (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;

   bzero(tmpbuf, sizeof(tmpbuf));

   if (parser->Pctflag) {
      double pfirst = ArgusFetchPercentDstFirst(argus);
      sprintf (ptr, "%3.*f", parser->pflag, pfirst);

   } else {
      double pfirst = ArgusFetchDstFirst(argus);
      int first = pfirst;
      sprintf (ptr, "%d", first);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstFirst = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstFirst (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintFirst (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;

   bzero(tmpbuf, sizeof(tmpbuf));

   if (parser->Pctflag) {
      double pfirst = ArgusFetchPercentFirst(argus);
      sprintf (ptr, "%3.*f", parser->pflag, pfirst);

   } else {
      double pfirst = ArgusFetchFirst(argus);
      int first = pfirst;
      sprintf (ptr, "%d", first);
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " First = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintFirst (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintPercentSrcFirst (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;
   double pfirst = ArgusFetchPercentSrcFirst(argus);

   bzero(tmpbuf, sizeof(tmpbuf));
   sprintf (ptr, "%3.*f", parser->pflag, pfirst);

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcPctFirst = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintPercentSrcFirst (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintPercentDstFirst (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;
   double pfirst = ArgusFetchPercentDstFirst(argus);

   bzero(tmpbuf, sizeof(tmpbuf));
   sprintf (ptr, "%3.*f", parser->pflag, pfirst);

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstPctFirst = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintPercentDstFirst (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintPercentFirst (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;
   double pfirst = ArgusFetchPercentFirst(argus);

   bzero(tmpbuf, sizeof(tmpbuf));
   sprintf (ptr, "%3.*f", parser->pflag, pfirst);

   if (parser->ArgusPrintXml) {
      sprintf (buf, " PctFirst = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintPercentFirst (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintPercentSrcLoss (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;
   double ploss = ArgusFetchPercentSrcLoss(argus);

   bzero(tmpbuf, sizeof(tmpbuf));
   sprintf (ptr, "%3.*f", parser->pflag, ploss);

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcPctLoss = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintPercentSrcLoss (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintPercentDstLoss (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;
   double ploss = ArgusFetchPercentDstLoss(argus);

   bzero(tmpbuf, sizeof(tmpbuf));
   sprintf (ptr, "%3.*f", parser->pflag, ploss);

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstPctLoss = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintPercentDstLoss (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintPercentLoss (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[64], *ptr = tmpbuf;
   double ploss = ArgusFetchPercentLoss(argus);

   bzero(tmpbuf, sizeof(tmpbuf));
   sprintf (ptr, "%3.*f", parser->pflag, ploss);

   if (parser->ArgusPrintXml) {
      sprintf (buf, " PctLoss = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintPercentLoss (%p, %p)", buf, argus);
#endif
}



void
ArgusPrintSrcLoad (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[128], *ptr = tmpbuf;
   float load = 0.0;
 
   if (argus->hdr.type & ARGUS_MAR) {

   } else 
      load = argus->sload;

   if (parser->Hflag) {
      ArgusAbbreviateMetric(parser, ptr, 128, load);
   } else
      sprintf (ptr, "%.*f", parser->pflag, load);
 
   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcLoad = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 
 
#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcLoad (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintDstLoad (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[128], *ptr = tmpbuf;
   float load = 0.0;
 
   if (argus->hdr.type & ARGUS_MAR) {

   } else 
      load = argus->dload;

   if (parser->Hflag) {
      ArgusAbbreviateMetric(parser, ptr, 128, load);
   } else
      sprintf (ptr, "%.*f", parser->pflag, load);
 
   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstLoad = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstLoad (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintLoad (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tmpbuf[128], *ptr = tmpbuf;
   float load = 0.0;
 
   if (argus->hdr.type & ARGUS_MAR) {

   } else {
      float sdur = RaGetFloatSrcDuration(argus);
      float ddur = RaGetFloatDstDuration(argus);

      if (!(sdur > 0)) sdur = argus->dur;
      if (!(ddur > 0)) ddur = argus->dur;

      if (argus->dur > 0.0)
         load = ((argus->sload * sdur) + (argus->dload * ddur)) / argus->dur;
   }

   if (parser->Hflag) {
      ArgusAbbreviateMetric(parser, ptr, 128, load);
   } else
      sprintf (ptr, "%.*f", parser->pflag, load);
 
   if (parser->ArgusPrintXml) {
      sprintf (buf, " Load = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 
 
#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintLoad (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintSrcVID (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusVlanStruct *vlan = (struct ArgusVlanStruct *)argus->dsrs[ARGUS_VLAN_INDEX];
   char vlanbuf[32];

   bzero(vlanbuf, sizeof(vlanbuf));
   if (vlan != NULL) 
      if (vlan->hdr.argus_dsrvl8.qual & ARGUS_SRC_VLAN)
         sprintf (vlanbuf, "%d", (vlan->sid & 0x0FFF));

   if (parser->ArgusPrintXml) {
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(vlanbuf);
      } else {
         if (strlen(vlanbuf) > len) {
            vlanbuf[len - 1] = '*';
            vlanbuf[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, vlanbuf);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcVID (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintDstVID (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusVlanStruct *vlan = (struct ArgusVlanStruct *)argus->dsrs[ARGUS_VLAN_INDEX];
   char vlanbuf[32];

   bzero(vlanbuf, sizeof(vlanbuf));
   if (vlan != NULL)
      if (vlan->hdr.argus_dsrvl8.qual & ARGUS_DST_VLAN)
         sprintf (vlanbuf, "%d", (vlan->did & 0x0FFF));

   if (parser->ArgusPrintXml) {
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(vlanbuf);
      } else {
         if (strlen(vlanbuf) > len) {
            vlanbuf[len - 1] = '*';
            vlanbuf[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, vlanbuf);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstVID (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintSrcVPRI (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{

   if (parser->ArgusPrintXml) {
   } else
      sprintf (buf, "%*.*s ", len, len, " ");

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcVPRI (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintDstVPRI (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{

   if (parser->ArgusPrintXml) {
   } else
      sprintf (buf, "%*.*s ", len, len, " ");

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstVPRI (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintSrcVlan (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusVlanStruct *vlan = (struct ArgusVlanStruct *)argus->dsrs[ARGUS_VLAN_INDEX];
   char vstr[16];
                                                                                                           
   bzero(vstr, sizeof(vstr));
   if (vlan != NULL)
      if ((vlan->hdr.argus_dsrvl8.qual & ARGUS_SRC_VLAN) || (vlan->sid > 0))
         sprintf (vstr, "0x%04x", vlan->sid);
                                                                                                           
   if (parser->ArgusPrintXml) {
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(vstr);
      } else {
         if (strlen(vstr) > len) {
            vstr[len - 1] = '*';
            vstr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, vstr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcVlan (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintDstVlan (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusVlanStruct *vlan = (struct ArgusVlanStruct *)argus->dsrs[ARGUS_VLAN_INDEX];
   char vstr[16];

   bzero(vstr, sizeof(vstr));
   if (vlan != NULL)
      if ((vlan->hdr.argus_dsrvl8.qual & ARGUS_DST_VLAN) || (vlan->did > 0))
         sprintf (vstr, "0x%04x", vlan->did);

   if (parser->ArgusPrintXml) {
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(vstr);
      } else {
         if (strlen(vstr) > len) {
            vstr[len - 1] = '*';
            vstr[len]     = '\0'; 
         }        
      }
      sprintf (buf, "%*.*s ", len, len, vstr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstVlan (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintSrcMpls (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusMplsStruct *mpls = (struct ArgusMplsStruct *)argus->dsrs[ARGUS_MPLS_INDEX];
   unsigned int label;
   char tbuf[32];

   bzero (tbuf, sizeof(tbuf));
   if (mpls != NULL) {
      if (mpls->hdr.subtype & ARGUS_MPLS_SRC_LABEL) {
         label = mpls->slabel >> 12;
         sprintf (tbuf, "%d", label);
      }
   }

   if (parser->ArgusPrintXml) {
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(tbuf);
      } else {
         if (strlen(tbuf) > len) {
            tbuf[len - 1] = '*';
            tbuf[len]     = '\0'; 
         }        
      }
      sprintf (buf, "%*.*s ", len, len, tbuf);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcMpls (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintDstMpls (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusMplsStruct *mpls = (struct ArgusMplsStruct *)argus->dsrs[ARGUS_MPLS_INDEX];
   unsigned int label;
   char tbuf[32];

   bzero (tbuf, sizeof(tbuf));
   if (mpls != NULL) {
      if (mpls->hdr.subtype & ARGUS_MPLS_DST_LABEL) {
         label = mpls->dlabel >> 12;
         sprintf (tbuf, "%d", label);
      }
   }

   if (parser->ArgusPrintXml) {
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(tbuf);
      } else {
         if (strlen(tbuf) > len) {
            tbuf[len - 1] = '*';
            tbuf[len]     = '\0'; 
         }        
      }
      sprintf (buf, "%*.*s ", len, len, tbuf);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstMpls (%p, %p)", buf, argus);
#endif
}

/*
void
ArgusPrintMpls (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   ArgusPrintSrcMpls (parser, buf, argus);
   ArgusPrintDstMpls (parser, buf, argus);

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintMpls (%p, %p)", buf, argus);
#endif
}
*/

#include <netinet/igmp.h>

void
ArgusPrintJoinDelay (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   if (parser->ArgusPrintXml) {
   } else
      sprintf (buf, "%*.*s ", len, len, " ");

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintJoinDelay (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintLeaveDelay (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   if (parser->ArgusPrintXml) {
   } else
      sprintf (buf, "%*.*s ", len, len, " ");

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintLeaveDelay (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintSrcWindow (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusNetworkStruct *net = (struct ArgusNetworkStruct *)argus->dsrs[ARGUS_NETWORK_INDEX];
   char winbuf[32];

   bzero(winbuf, sizeof(winbuf));

   if (net != NULL) {
      struct ArgusFlow *flow = (struct ArgusFlow *)argus->dsrs[ARGUS_FLOW_INDEX];
      struct ArgusMetricStruct *metric = (struct ArgusMetricStruct *) argus->dsrs[ARGUS_METRIC_INDEX];

      if (net && (net->hdr.subtype == ARGUS_UDT_FLOW)) {
         unsigned int win = net->net_union.udt.src.bsize;
         if (parser->Hflag)
            ArgusAbbreviateMetric(parser, winbuf, 32, win);
         else
            snprintf (winbuf, 32, "%u", win);
      } else {
         if ((flow != NULL)  && ((metric != NULL) && (metric->src.pkts > 0))) {
            switch (flow->hdr.subtype & 0x3F) {
               case ARGUS_FLOW_CLASSIC5TUPLE: {
                  switch ((flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                     case ARGUS_TYPE_IPV4:
                        switch (flow->ip_flow.ip_p) {
                           case  IPPROTO_TCP: {
                              struct ArgusTCPObject *tcp = (struct ArgusTCPObject *)&net->net_union.tcp;

                              unsigned int win = tcp->src.win << tcp->src.winshift;
                              if (parser->Hflag)
                                 ArgusAbbreviateMetric(parser, winbuf, 32, win);
                              else
                                 sprintf (winbuf, "%u", win);
                              break;
                           }
                           default:
                              break;
                        }
                        break;

                     case ARGUS_TYPE_IPV6:
                        switch (flow->ipv6_flow.ip_p) {
                           case  IPPROTO_TCP: {
                              struct ArgusTCPObject *tcp = (struct ArgusTCPObject *)&net->net_union.tcp;
                              unsigned int win = tcp->src.win << tcp->src.winshift;
                              if (parser->Hflag)
                                 ArgusAbbreviateMetric(parser, winbuf, 32, win);
                              else
                                 sprintf (winbuf, "%u", win);
                              break;
                           }
                           default:
                              break;
                        }
                        break;
                  }
                  break;
               }

               default: 
                  break;
            }
         }
      }
   }

   if (parser->ArgusPrintXml) {
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(winbuf);
      } else {
         if (strlen(winbuf) > len) {
            winbuf[len - 1] = '*';
            winbuf[len]     = '\0'; 
         }        
      }
      sprintf (buf, "%*.*s ", len, len, winbuf);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcWindow (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintDstWindow (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusNetworkStruct *net = (struct ArgusNetworkStruct *)argus->dsrs[ARGUS_NETWORK_INDEX];
   char winbuf[32];

   bzero(winbuf, sizeof(winbuf));

   if (net != NULL) {
      struct ArgusTCPObject *tcp = (struct ArgusTCPObject *)&net->net_union.tcp;
      struct ArgusFlow *flow = (struct ArgusFlow *)argus->dsrs[ARGUS_FLOW_INDEX];
      struct ArgusMetricStruct *metric = (struct ArgusMetricStruct *) argus->dsrs[ARGUS_METRIC_INDEX];

      if ((flow != NULL)  && ((metric != NULL) && (metric->src.pkts > 0))) {
         if (metric->dst.pkts > 0) {
            switch (flow->hdr.subtype & 0x3F) {
               case ARGUS_FLOW_CLASSIC5TUPLE: {
                  switch ((flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                     case ARGUS_TYPE_IPV4:
                        switch (flow->ip_flow.ip_p) {
                           case  IPPROTO_TCP: {
                              unsigned int win = tcp->dst.win << tcp->dst.winshift;
                              if (parser->Hflag)
                                 ArgusAbbreviateMetric(parser, winbuf, 32, win);
                              else
                                 snprintf (winbuf, 32, "%u", win);
                              break;
                           }
                           default:
                              break;
                        }
                        break;

                     case ARGUS_TYPE_IPV6:
                        switch (flow->ipv6_flow.ip_p) {
                           case  IPPROTO_TCP: {
                              unsigned int win = tcp->dst.win << tcp->dst.winshift;
                              if (parser->Hflag)
                                 ArgusAbbreviateMetric(parser, winbuf, 32, win);
                              else
                                 sprintf (winbuf, "%u", win);
                              break;
                           }
                           default:
                              break;
                        }
                        break;
                  }
                  break;
               }

               default: 
                  break;
            }
         }
      }
   }

   if (parser->ArgusPrintXml) {
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(winbuf);
      } else {
         if (strlen(winbuf) > len) {
            winbuf[len - 1] = '*';
            winbuf[len]     = '\0'; 
         }        
      }
      sprintf (buf, "%*.*s ", len, len, winbuf);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstWindow (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintTCPRTT (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char tbuf[32], *ptr = tbuf;
   double rtt = 0.0;

   rtt = ArgusFetchTcpRtt(argus);
   if (parser->Hflag)
      ArgusAbbreviateMetric(parser, ptr, 32, rtt);
   else
      snprintf (ptr, 32, "%.*f", parser->pflag, rtt);

   if (parser->ArgusPrintXml) {
      if (rtt > 0.0)
         sprintf (buf, " TcpRtt = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0'; 
         }        
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintTCPRTT (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintTCPSynAck (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   double rtt = ArgusFetchTcpSynAck(argus);
   char tbuf[32], *ptr = tbuf;

   if (parser->Hflag)
      ArgusAbbreviateMetric(parser, ptr, 32, rtt);
   else
      snprintf (ptr, 32, "%.*f", parser->pflag, rtt);

   if (parser->ArgusPrintXml) {
      sprintf (buf, " TcpSynAck = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0'; 
         }        
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintTCPSynAck (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintTCPAckDat (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   double rtt = ArgusFetchTcpAckDat(argus);
   char tbuf[32], *ptr = tbuf;

   if (parser->Hflag)
      ArgusAbbreviateMetric(parser, ptr, 32, rtt);
   else
      snprintf (ptr, 32, "%.*f", parser->pflag, rtt);

   if (parser->ArgusPrintXml) {
      sprintf (buf, " TcpAckDat = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0'; 
         }        
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintTCPAckDat (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintTCPSrcMax (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   double value = ArgusFetchSrcTcpMax(argus);
   char tbuf[32], *ptr = tbuf;

   if (parser->Hflag)
      ArgusAbbreviateMetric(parser, ptr, 32, value);
   else
      sprintf (ptr, "%.*f", parser->pflag, value);

   if (parser->ArgusPrintXml) {
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0'; 
         }        
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintTCPSrcMax (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintTCPDstMax (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   double value = ArgusFetchDstTcpMax(argus);
   char tbuf[32], *ptr = tbuf;
 
   if (parser->Hflag)
      ArgusAbbreviateMetric(parser, ptr, 32, value);
   else
      sprintf (ptr, "%.*f", parser->pflag, value);
 
   if (parser->ArgusPrintXml) {
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0'; 
         }        
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintTCPDstMax (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintSrcGap (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   double value = ArgusFetchSrcGap(argus);
   int tcpgap = value;
   char tbuf[32], *ptr = tbuf;

   bzero(tbuf, sizeof(tbuf));

   if (parser->Hflag)
      ArgusAbbreviateMetric(parser, ptr, 32, value);
   else {
      if (tcpgap != -1)
         sprintf (ptr, "%d", tcpgap);
   }

   if (parser->ArgusPrintXml) {
      if (strlen(ptr))
         sprintf (buf, " SrcGap = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else {
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ptr);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcGap (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintDstGap (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   double value = ArgusFetchDstGap(argus);
   int tcpgap = value;
   char tbuf[32], *ptr = tbuf;

   bzero(tbuf, sizeof(tbuf));

   if (parser->Hflag)
      ArgusAbbreviateMetric(parser, ptr, 32, value);
   else {
      if (tcpgap != -1)
         sprintf (ptr, "%d", tcpgap);
   }

   if (parser->ArgusPrintXml) {
      if (strlen(ptr))
         sprintf (buf, " DstGap = \"%s\"", ptr);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ptr);
      } else { 
         if (strlen(ptr) > len) {
            ptr[len - 1] = '*';
            ptr[len]     = '\0';
         }        
      }              
      sprintf (buf, "%*.*s ", len, len, ptr);
   }                       
                           
#ifdef ARGUSDEBUG       
   ArgusDebug (10, "ArgusPrintDstGap (%p, %p)", buf, argus);
#endif               
}


void
ArgusPrintTCPSrcBase (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusNetworkStruct *net;
   struct ArgusTCPObject *tcp;
   struct ArgusFlow *flow;
   char pbuf[32];

   bzero(pbuf, sizeof(pbuf));

   if ((flow = (struct ArgusFlow *)argus->dsrs[ARGUS_FLOW_INDEX]) != NULL) {
      if ((net = (struct ArgusNetworkStruct *)argus->dsrs[ARGUS_NETWORK_INDEX]) != NULL) {
         tcp = (struct ArgusTCPObject *)&net->net_union.tcp;
         switch (flow->hdr.subtype & 0x3F) {
            case ARGUS_FLOW_CLASSIC5TUPLE: {
               switch ((flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                  case ARGUS_TYPE_IPV4:
                     switch (flow->ip_flow.ip_p) {
                        case  IPPROTO_TCP:
                           sprintf (pbuf, "%u", tcp->src.seqbase);
                           break;
                        default:
                           break;
                     }
                     break;

                  case ARGUS_TYPE_IPV6:
                     switch (flow->ipv6_flow.ip_p) {
                        case  IPPROTO_TCP:
                           sprintf (pbuf, "%u", tcp->src.seqbase);
                           break;
                        default:
                           break;
                     }
                     break;
               }
               break;
            }

            default: 
               break;
         }
      }
   }

   if (parser->ArgusPrintXml) {
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(pbuf);
      } else {
         if (strlen(pbuf) > len) {
            pbuf[len - 1] = '*';
            pbuf[len]     = '\0'; 
         }        
      }
      sprintf (buf, "%*.*s ", len, len, pbuf);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintTCPSrcBase (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintTCPDstBase (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusNetworkStruct *net;
   struct ArgusTCPObject *tcp;
   struct ArgusFlow *flow;
   char pbuf[32];

   bzero(pbuf, sizeof(pbuf));

   if ((flow = (struct ArgusFlow *)argus->dsrs[ARGUS_FLOW_INDEX]) != NULL) {
      if ((net = (struct ArgusNetworkStruct *)argus->dsrs[ARGUS_NETWORK_INDEX]) != NULL) {
         tcp = (struct ArgusTCPObject *)&net->net_union.tcp;

         switch (flow->hdr.subtype & 0x3F) {
            case ARGUS_FLOW_CLASSIC5TUPLE: {
               switch ((flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                  case ARGUS_TYPE_IPV4:
                     switch (flow->ip_flow.ip_p) {
                        case  IPPROTO_TCP:
                           sprintf (pbuf, "%u", tcp->dst.seqbase);
                           break;
                        default:
                           break;
                     }
                     break;

                  case ARGUS_TYPE_IPV6:
                     switch (flow->ipv6_flow.ip_p) {
                        case  IPPROTO_TCP:
                           sprintf (pbuf, "%u", tcp->dst.seqbase);
                           break;
                        default:
                           break;
                     }
                     break;
               }
               break;
            }

            default: 
               sprintf (pbuf, "%s", "");
               break;
         }
      }
   }

   if (parser->ArgusPrintXml) {
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(pbuf);
      } else {
         if (strlen(pbuf) > len) {
            pbuf[len - 1] = '*';
            pbuf[len]     = '\0'; 
         }        
      }
      sprintf (buf, "%*.*s ", len, len, pbuf);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintTCPDstBase (%p, %p)", buf, argus);
#endif
}

/*
void
ArgusPrintTCPBase (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   ArgusPrintTCPSrcBase(parser, &buf[strlen(buf)], argus);
   ArgusPrintTCPDstBase(parser, &buf[strlen(buf)], argus);

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintTCPBase (%p, %p)", buf, argus);
#endif
}
*/

void
ArgusPrintTCPOptions (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusNetworkStruct *net;
   struct ArgusTCPObject *tcp = NULL;
   struct ArgusFlow *flow;
   char options[16];

   strcpy(options, "            ");

   if ((flow = (struct ArgusFlow *)argus->dsrs[ARGUS_FLOW_INDEX]) != NULL) {
      if ((net = (struct ArgusNetworkStruct *)argus->dsrs[ARGUS_NETWORK_INDEX]) != NULL) {
         switch (flow->hdr.subtype & 0x3F) {
            case ARGUS_FLOW_CLASSIC5TUPLE: {
               switch ((flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                  case ARGUS_TYPE_IPV4:
                     switch (flow->ip_flow.ip_p) {
                        case  IPPROTO_TCP:
                           tcp = (struct ArgusTCPObject *)&net->net_union.tcp;
                           break;
                        default:
                           break;
                     }
                     break;

                  case ARGUS_TYPE_IPV6:
                     switch (flow->ipv6_flow.ip_p) {
                        case  IPPROTO_TCP:
                           tcp = (struct ArgusTCPObject *)&net->net_union.tcp;
                           break;
                        default:
                           break;
                     }
                     break;
               }
               break;
            }

            default:
               break;
         }
      }
   }

#define ARGUSTCPOPTNUM	12

   if (tcp != NULL) {
      unsigned int opt, tcpoptions = tcp->options & ARGUS_TCP_OPTIONS;
      char *optr = options;
      int i;
      if (tcpoptions != 0) {
         for (i = 0, opt = ARGUS_TCP_MAXSEG; i < ARGUSTCPOPTNUM; i++) {
            if (tcpoptions & opt) {
               switch (opt) {
                  case ARGUS_TCP_MAXSEG:    *optr = 'M'; break;
                  case ARGUS_TCP_WSCALE:    *optr = 'w'; break;
                  case ARGUS_TCP_SACKOK:    *optr = 's'; break;
                  case ARGUS_TCP_SACK:      *optr = 'S'; break;
                  case ARGUS_TCP_ECHO:      *optr = 'e'; break;
                  case ARGUS_TCP_ECHOREPLY: *optr = 'E'; break;
                  case ARGUS_TCP_TIMESTAMP: *optr = 'T'; break;
                  case ARGUS_TCP_CC:        *optr = 'c'; break;
                  case ARGUS_TCP_CCNEW:     *optr = 'N'; break;
                  case ARGUS_TCP_CCECHO:    *optr = 'O'; break;
                  case ARGUS_TCP_SRC_ECN:   *optr = 'S'; break;
                  case ARGUS_TCP_DST_ECN:   *optr = 'D'; break;
               }
            }
            opt = opt << 1;
            optr++;
         }
      }
   }

   if (parser->ArgusPrintXml) {
      if (tcp != NULL)
         sprintf (buf, " TcpOptions = \"%s\"", options);
   } else
      sprintf (buf, "%*.*s ", len, len, options);

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintTCPOptions (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintTCPExtensions (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   if (parser->ArgusPrintXml) {
   } else
      sprintf (buf, "%*.*s ", len, len, " ");
                                                                                                           
#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintTCPExtentions (%p, %p)", buf, argus);
#endif
}

char *ArgusGetManStatus (struct ArgusParserStruct *parser, struct ArgusRecordStruct *);
char *ArgusGetTCPStatus (struct ArgusParserStruct *parser, struct ArgusRecordStruct *);
char *ArgusGetIGMPStatus (struct ArgusParserStruct *parser, struct ArgusRecordStruct *);
char *ArgusGetICMPStatus (struct ArgusParserStruct *parser, struct ArgusRecordStruct *);
char *ArgusGetICMPv6Status (struct ArgusParserStruct *parser, struct ArgusRecordStruct *);
char *ArgusGetIPStatus (struct ArgusParserStruct *parser, struct ArgusRecordStruct *);

char *ArgusTCPFlags [] = {
   "F", "S", "R", "P", "A", "U", "E", "C"
};


void
ArgusPrintState (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char ArgusProcessBuf[256], *ArgusProcessStr = ArgusProcessBuf;
   struct ArgusMetricStruct *metric = NULL;
   struct ArgusFlow *flow = NULL;
   int type;

   sprintf (ArgusProcessStr, "UNK");

   if (argus->hdr.type & ARGUS_MAR) {
      ArgusProcessStr = ArgusGetManStatus (parser, argus);
   } else {
      if (((flow = (struct ArgusFlow *)argus->dsrs[ARGUS_FLOW_INDEX]) != NULL)) {
         metric = (struct ArgusMetricStruct *)argus->dsrs[ARGUS_METRIC_INDEX];

         switch (flow->hdr.subtype & 0x3F) {
            case ARGUS_FLOW_CLASSIC5TUPLE: {
               switch (type = (flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                  case ARGUS_TYPE_IPV4:
                     switch (flow->ip_flow.ip_p) {
                        case  IPPROTO_TCP: ArgusProcessStr = ArgusGetTCPStatus (parser, argus); break;
                        case IPPROTO_ICMP: ArgusProcessStr = ArgusGetICMPStatus (parser, argus); break;
                        case IPPROTO_IGMP: ArgusProcessStr = ArgusGetIPStatus (parser, argus); break;
                        default:           ArgusProcessStr = ArgusGetIPStatus (parser, argus); break;
                     }
                     break;
 
                  case ARGUS_TYPE_IPV6:
                     switch (flow->ipv6_flow.ip_p) {
                        case  IPPROTO_TCP: ArgusProcessStr = ArgusGetTCPStatus (parser, argus); break;
                        case IPPROTO_ICMPV6: ArgusProcessStr = ArgusGetICMPv6Status (parser, argus); break;
                        case IPPROTO_IGMP: ArgusProcessStr = ArgusGetIPStatus (parser, argus); break;
                        default:           ArgusProcessStr = ArgusGetIPStatus (parser, argus); break;
                     }
                     break;
 
                  case ARGUS_TYPE_RARP: 
                  case ARGUS_TYPE_ARP: {
                     if (metric != NULL) {
                        if (metric->src.pkts && metric->dst.pkts)
                           sprintf(ArgusProcessStr, "%s", "CON");
                        else
                           if ((metric->src.pkts) || (parser->RaMonMode)) {
                              if (argus->hdr.cause & ARGUS_START)
                                 sprintf(ArgusProcessStr, "%s", "INT");
                              else
                                 sprintf(ArgusProcessStr, "%s", "REQ");
                           } else
                              sprintf(ArgusProcessStr, "%s", "RSP");
                     } else
                        sprintf(ArgusProcessStr, "%s", "INT");
                     break;
                  }

                  case ARGUS_TYPE_ETHER: 
                  default: {
                     ArgusProcessStr = ArgusGetIPStatus(parser, argus);
                  }

                  case ARGUS_TYPE_ISIS: {
                     switch (flow->isis_flow.pdu_type) {
                        case L1_LAN_IIH: sprintf(ArgusProcessStr, "%s", "L1-IIH"); break;
                        case L2_LAN_IIH: sprintf(ArgusProcessStr, "%s", "L2-IIH"); break;
                        case L1_LSP:     sprintf(ArgusProcessStr, "%s", "L1-LSP"); break;
                        case L2_LSP:     sprintf(ArgusProcessStr, "%s", "L2-LSP"); break;
                        case L1_CSNP:    sprintf(ArgusProcessStr, "%s", "L1-CSNP"); break;
                        case L2_CSNP:    sprintf(ArgusProcessStr, "%s", "L2-CSNP"); break;
                        case L1_PSNP:    sprintf(ArgusProcessStr, "%s", "L1-PSNP"); break;
                        case L2_PSNP:    sprintf(ArgusProcessStr, "%s", "L2-PSNP"); break;
                     }
                     break;
                  }
               }
               break;
            }

            case ARGUS_FLOW_ARP: {
               if (metric != NULL) {
                  if (metric->src.pkts && metric->dst.pkts)
                     sprintf(ArgusProcessStr, "%s", "CON");
                  else
                     if ((metric->src.pkts) || (parser->RaMonMode)) {
                        if (argus->hdr.type & ARGUS_START)
                           sprintf(ArgusProcessStr, "%s", "INT");
                        else
                           sprintf(ArgusProcessStr, "%s", "REQ");
                     } else
                        sprintf(ArgusProcessStr, "%s", "RSP");
               } else 
                  sprintf(ArgusProcessStr, "%s", "INT");
               break;
            }
         }

      } else 
         ArgusProcessStr =  "   ";
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " State = \"%s\"", ArgusProcessStr);
      
   } else {
      int slen = strlen(ArgusProcessStr);
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = slen;
      } else {
         if (slen > len) {
            ArgusProcessStr[len - 1] = '*';
            ArgusProcessStr[len]     = '\0';
         }
      }
      sprintf (buf, "%*.*s ", len, len, ArgusProcessStr);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintState (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintDeltaDuration (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusCorrelateStruct *cor = (void *)argus->dsrs[ARGUS_COR_INDEX];
   char deltadur[128];
   float ddur = 0.0;

   bzero (deltadur, sizeof(deltadur));
   if (cor != NULL) {
      ddur = cor->metrics.deltaDur/1000.0;
      sprintf (deltadur, "%.*f", parser->pflag, ddur);
   } else {
      sprintf (deltadur, " ");
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DeltaDuration = \"%s\"", deltadur);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(deltadur);
      } else {
         if (strlen(deltadur) > len) {
            deltadur[len - 1] = '*';
            deltadur[len]     = '\0'; 
         }        
      }
      sprintf (buf, "%*.*s ", len, len, deltadur);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDeltaDuration (%p, %p)", buf, argus);
#endif
}
 

void
ArgusPrintDeltaStartTime (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusCorrelateStruct *cor = (void *)argus->dsrs[ARGUS_COR_INDEX];
   char deltastart[128];
   float dstart = 0.0;

   bzero (deltastart, sizeof(deltastart));
   if (cor != NULL) {
      dstart = cor->metrics.deltaStart/1000000.0;
      sprintf (deltastart, "%.*f", parser->pflag, dstart);
   } else {
      sprintf (deltastart, " ");
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DeltaStartTime = \"%s\"", deltastart);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(deltastart);
      } else {
         if (strlen(deltastart) > len) {
            deltastart[len - 1] = '*';
            deltastart[len]     = '\0'; 
         }        
      }
      sprintf (buf, "%*.*s ", len, len, deltastart);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDeltaStartTime (%p, %p)", buf, argus);
#endif
}
 
 
void
ArgusPrintDeltaLastTime (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusCorrelateStruct *cor = (void *)argus->dsrs[ARGUS_COR_INDEX];
   char deltalast[128];
   float dlast = 0.0;

   bzero (deltalast, sizeof(deltalast));
   if (cor != NULL) {
      dlast = cor->metrics.deltaLast/1000000.0;
      sprintf (deltalast, "%.*f", parser->pflag, dlast);
   } else {
      sprintf (deltalast, " ");
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DeltaLastTime = \"%s\"", deltalast);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(deltalast);
      } else {
         if (strlen(deltalast) > len) {
            deltalast[len - 1] = '*';
            deltalast[len]     = '\0'; 
         }        
      }
      sprintf (buf, "%*.*s ", len, len, deltalast);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDeltaLastTime (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintDeltaSrcPkts (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusCorrelateStruct *cor = (void *)argus->dsrs[ARGUS_COR_INDEX];
   char deltaspkts[128];
   int dspkts = 0;

   bzero (deltaspkts, sizeof(deltaspkts));
   if (cor != NULL) {
      dspkts = cor->metrics.deltaSrcPkts;
      sprintf (deltaspkts, "%d", dspkts);
   } else {
      sprintf (deltaspkts, " ");
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DeltaSrcPkts = \"%s\"", deltaspkts);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(deltaspkts);
      } else {
         if (strlen(deltaspkts) > len) {
            deltaspkts[len - 1] = '*';
            deltaspkts[len]     = '\0'; 
         }        
      }
      sprintf (buf, "%*.*s ", len, len, deltaspkts);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDeltaSrcPkts (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintDeltaDstPkts (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusCorrelateStruct *cor = (void *)argus->dsrs[ARGUS_COR_INDEX];
   char deltadpkts[128];
   int ddpkts = 0;
      
   bzero (deltadpkts, sizeof(deltadpkts));
   if (cor != NULL) {
      ddpkts = cor->metrics.deltaDstPkts;
      sprintf (deltadpkts, "%d", ddpkts);
   } else { 
      sprintf (deltadpkts, " ");
   }     
            
   if (parser->ArgusPrintXml) {
      sprintf (buf, " DeltaSrcPkts = \"%s\"", deltadpkts);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(deltadpkts);
      } else {
         if (strlen(deltadpkts) > len) {
            deltadpkts[len - 1] = '*';
            deltadpkts[len]     = '\0'; 
         }        
      }
      sprintf (buf, "%*.*s ", len, len, deltadpkts);
   } 

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDeltaSrcPkts (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintDeltaSrcBytes (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DeltaSrcBytes = \"%s\"", " ");
   } else
      sprintf (buf, "%*.*s ", len, len, " ");

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDeltaSrcBytes (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintDeltaDstBytes (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DeltaDstBytes = \"%s\"", " ");
   } else
      sprintf (buf, "%*.*s ", len, len, " ");

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDeltaDstBytes (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintPercentDeltaSrcPkts (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{

   if (parser->ArgusPrintXml) {
      sprintf (buf, " PctDeltaSrcPkts = \"%s\"", " ");
   } else
      sprintf (buf, "%*.*s ", len, len, " ");

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintPercentDeltaSrcPkts (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintPercentDeltaDstPkts (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{

   if (parser->ArgusPrintXml) {
      sprintf (buf, " PctDeltaDstPkts = \"%s\"", " ");
   } else
      sprintf (buf, "%*.*s ", len, len, " ");

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintPercentDeltaDstPkts (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintPercentDeltaSrcBytes (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{

   if (parser->ArgusPrintXml) {
      sprintf (buf, " PctDeltaSrcBytes = \"%s\"", " ");
   } else
      sprintf (buf, "%*.*s ", len, len, " ");

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintPercentDeltaSrcBytes (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintPercentDeltaDstBytes (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{

   if (parser->ArgusPrintXml) {
      sprintf (buf, " PctDeltaDstBytes = \"%s\"", " ");
   } else
      sprintf (buf, "%*.*s ", len, len, " ");

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintPercentDeltaDstBytes (%p, %p)", buf, argus);
#endif
}


char ArgusIPStatus[32];

void
ArgusPrintIPStatus (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{

   if (parser->ArgusPrintXml) {
      sprintf (buf, " IPStatus = \"%s\"", " ");
   } else
      sprintf (buf, "%*.*s ", len, len, " ");

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintIPStatus (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintManStatus (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{

   if (parser->ArgusPrintXml) {
      sprintf (buf, " ManStatus = \"%s\"", " ");
   } else
      sprintf (buf, "%*.*s ", len, len, " ");

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintManStatus (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintIGMPStatus (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{

   if (parser->ArgusPrintXml) {
      sprintf (buf, " IGMPStatus = \"%s\"", " ");
   } else
      sprintf (buf, "%*.*s ", len, len, " ");

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintIGMPStatus (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintICMPStatus (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{

   if (parser->ArgusPrintXml) {
      sprintf (buf, " ICMPStatus = \"%s\"", " ");
   } else
      sprintf (buf, "%*.*s ", len, len, " ");

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintICMPStatus (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintSrcEncaps (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusEncapsStruct *encaps = NULL;
   char ebuf[32];

   bzero(ebuf, sizeof(ebuf));
   if ((encaps = (struct ArgusEncapsStruct *)argus->dsrs[ARGUS_ENCAPS_INDEX]) != NULL) {
      unsigned int i, types = encaps->src, ind = 0;

      for (i = 0; i < ARGUS_ENCAPS_TYPE; i++) {
         if (types & (0x01 << i)) {
            switch (0x01 << i) {
                  case ARGUS_ENCAPS_ETHER:  ebuf[ind++] = 'e'; break;
                  case ARGUS_ENCAPS_LLC:    ebuf[ind++] = 'l'; break;
                  case ARGUS_ENCAPS_MPLS:   ebuf[ind++] = 'm'; break;
                  case ARGUS_ENCAPS_8021Q:  ebuf[ind++] = 'v'; break;
                  case ARGUS_ENCAPS_PPP:    ebuf[ind++] = 'p'; break;
                  case ARGUS_ENCAPS_ISL:    ebuf[ind++] = 'i'; break;
                  case ARGUS_ENCAPS_GRE:    ebuf[ind++] = 'G'; break;
                  case ARGUS_ENCAPS_AH:     ebuf[ind++] = 'a'; break;
                  case ARGUS_ENCAPS_IP:     ebuf[ind++] = 'P'; break;
                  case ARGUS_ENCAPS_IPV6:   ebuf[ind++] = '6'; break;
                  case ARGUS_ENCAPS_HDLC:   ebuf[ind++] = 'H'; break;
                  case ARGUS_ENCAPS_CHDLC:  ebuf[ind++] = 'C'; break;
                  case ARGUS_ENCAPS_ATM:    ebuf[ind++] = 'A'; break;
                  case ARGUS_ENCAPS_SLL:    ebuf[ind++] = 'S'; break;
                  case ARGUS_ENCAPS_FDDI:   ebuf[ind++] = 'F'; break;
                  case ARGUS_ENCAPS_SLIP:   ebuf[ind++] = 's'; break;
                  case ARGUS_ENCAPS_ARCNET: ebuf[ind++] = 'R'; break;
                  case ARGUS_ENCAPS_802_11: ebuf[ind++] = 'w'; break;
                  case ARGUS_ENCAPS_PRISM:  ebuf[ind++] = 'z'; break;
                  case ARGUS_ENCAPS_AVS:    ebuf[ind++] = 'a'; break;
                  case ARGUS_ENCAPS_TEREDO: ebuf[ind++] = 'T'; break;
            }
         }
      }
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcEncaps = \"%s\"", ebuf);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ebuf);
      } else {
         if (strlen(ebuf) > len) {
            ebuf[len - 1] = '*';
            ebuf[len]     = '\0'; 
         }        
      }
      sprintf (buf, "%*.*s ", len, len, ebuf);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcEncaps (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintDstEncaps (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusEncapsStruct *encaps = NULL;
   char ebuf[32];

   bzero(ebuf, sizeof(ebuf));
   if ((encaps = (struct ArgusEncapsStruct *)argus->dsrs[ARGUS_ENCAPS_INDEX]) != NULL) {
      unsigned int i, types = encaps->dst, ind = 0;

      for (i = 0; i < ARGUS_ENCAPS_TYPE; i++) {
         if (types & (0x01 << i)) {
            switch (0x01 << i) {
                  case ARGUS_ENCAPS_ETHER:  ebuf[ind++] = 'e'; break;
                  case ARGUS_ENCAPS_LLC:    ebuf[ind++] = 'l'; break;
                  case ARGUS_ENCAPS_MPLS:   ebuf[ind++] = 'm'; break;
                  case ARGUS_ENCAPS_8021Q:  ebuf[ind++] = 'v'; break;
                  case ARGUS_ENCAPS_PPP:    ebuf[ind++] = 'p'; break;
                  case ARGUS_ENCAPS_ISL:    ebuf[ind++] = 'i'; break;
                  case ARGUS_ENCAPS_GRE:    ebuf[ind++] = 'G'; break;
                  case ARGUS_ENCAPS_AH:     ebuf[ind++] = 'a'; break;
                  case ARGUS_ENCAPS_IP:     ebuf[ind++] = 'P'; break;
                  case ARGUS_ENCAPS_IPV6:   ebuf[ind++] = '6'; break; 
                  case ARGUS_ENCAPS_HDLC:   ebuf[ind++] = 'H'; break;
                  case ARGUS_ENCAPS_CHDLC:  ebuf[ind++] = 'C'; break;
                  case ARGUS_ENCAPS_ATM:    ebuf[ind++] = 'A'; break;
                  case ARGUS_ENCAPS_SLL:    ebuf[ind++] = 'S'; break;
                  case ARGUS_ENCAPS_FDDI:   ebuf[ind++] = 'F'; break;
                  case ARGUS_ENCAPS_SLIP:   ebuf[ind++] = 's'; break;
                  case ARGUS_ENCAPS_ARCNET: ebuf[ind++] = 'R'; break;
                  case ARGUS_ENCAPS_802_11: ebuf[ind++] = 'w'; break;
                  case ARGUS_ENCAPS_PRISM:  ebuf[ind++] = 'z'; break;
                  case ARGUS_ENCAPS_AVS:    ebuf[ind++] = 'a'; break;
                  case ARGUS_ENCAPS_TEREDO: ebuf[ind++] = 'T'; break;
            }
         }
      }
   }
 
   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstEncaps = \"%s\"", ebuf);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
         len = strlen(ebuf);
      } else {
         if (strlen(ebuf) > len) {
            ebuf[len - 1] = '*';
            ebuf[len]     = '\0'; 
         }        
      }
      sprintf (buf, "%*.*s ", len, len, ebuf);
   }
 
#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstEncaps (%p, %p)", buf, argus);
#endif
}


char RaPrecisionPad[128], RaTimePad[128], RaDateBuf[128];


char *
ArgusGenerateLabel(struct ArgusParserStruct *parser, struct ArgusRecordStruct *argus) 
{
   int i, x;

   bzero (RaDateBuf, sizeof (RaDateBuf));
   bzero (RaTimePad, sizeof (RaTimePad));
   bzero (RaPrecisionPad, sizeof (RaPrecisionPad));
   bzero (parser->RaLabelStr, sizeof(parser->RaLabelStr));

   parser->RaLabel = parser->RaLabelStr;

   for (i = 0; i < MAX_PRINT_ALG_TYPES; i++) {
      if (parser->RaPrintAlgorithmList[i] != NULL) {
         for (x = 0; x < MAX_PRINT_ALG_TYPES; x++) {
            if ((void *) parser->RaPrintAlgorithmList[i]->print == (void *) RaPrintAlgorithmTable[x].print) {
               char labelbuf[MAXSTRLEN], *lptr = labelbuf;
               RaPrintAlgorithmTable[x].label(parser, labelbuf, parser->RaPrintAlgorithmList[i]->length);
               if (parser->RaFieldWidth != RA_FIXED_WIDTH) {
                  lptr = ArgusTrimString(labelbuf);
                  sprintf(&parser->RaLabel[strlen(parser->RaLabel)], "%s ", lptr);
               } else
                  sprintf(&parser->RaLabel[strlen(parser->RaLabel)], "%s", lptr);
               break;
            }
         }
      } else
         break;

   }

   if (parser->ArgusPrintXml) {
   } else
   if ((parser->RaFieldDelimiter != ' ') && (parser->RaFieldDelimiter != '\0')) {
      switch (parser->RaFieldWidth) {
         case RA_FIXED_WIDTH: {
            char tmpbuf[128], *ptr = tmpbuf, *str = parser->RaLabel, lastchr = ' ';
            bzero (tmpbuf, sizeof(tmpbuf));
            lastchr = parser->RaFieldDelimiter;
            while (*str) {
               if (*str == ' ') {
                  if (lastchr != parser->RaFieldDelimiter)
                     *ptr++ = parser->RaFieldDelimiter;
                  while (isspace((int)*str)) str++;
               }
               lastchr = *str;
               *ptr++ = *str++;
            }
            bzero (parser->RaLabel, MAXSTRLEN);
            if (tmpbuf[strlen(tmpbuf) - 1] == parser->RaFieldDelimiter)
               tmpbuf[strlen(tmpbuf) - 1] = '\0';
            if (parser->RaFieldQuoted) {
               char *ptr = parser->RaLabel, sepbuf[8], *sep = sepbuf;
               char *ap, *tstr = tmpbuf;
               int i = 0;
               bzero(sep, 8);
               sep[0] = parser->RaFieldDelimiter;
               while ((ap = strtok(tstr, sep)) != NULL) {
                  if (i++)
                     *ptr++ = parser->RaFieldDelimiter;
                  if (*ap != '\0') {
                     sprintf (ptr, "%c%s%c", parser->RaFieldQuoted, ap, parser->RaFieldQuoted);
                     ptr += strlen(ptr);
                  } else {
                     sprintf (ptr, "%c%c", parser->RaFieldQuoted, parser->RaFieldQuoted);
                  }
                  tstr = NULL;
               }
            } else
               bcopy (tmpbuf, parser->RaLabel, strlen(tmpbuf));
         }
         break;

         default: {
            char tmpbuf[0x10000], *ptr = tmpbuf, *str = parser->RaLabel, lastchr = ' ';
            bzero (tmpbuf, sizeof(tmpbuf));
            lastchr = parser->RaFieldDelimiter;
            while (*str) {
               if (*str == ' ') {
                  if (lastchr != parser->RaFieldDelimiter)
                     *ptr++ = parser->RaFieldDelimiter;
                  while (isspace((int)*str)) str++;
               }
               lastchr = *str;
               *ptr++ = *str++;
            }
            bzero (parser->RaLabel, sizeof(parser->RaLabelStr));
            if (tmpbuf[strlen(tmpbuf) - 1] == parser->RaFieldDelimiter)
               tmpbuf[strlen(tmpbuf) - 1] = '\0';
            if (parser->RaFieldQuoted) {
               char *ptr = parser->RaLabel, sepbuf[8], *sep = sepbuf;
               char *ap, *tstr = tmpbuf;
               int i = 0;
               bzero(sep, 8);
               sep[0] = parser->RaFieldDelimiter;
               while ((ap = strtok(tstr, sep)) != NULL) {
                  if (i++)
                     *ptr++ = parser->RaFieldDelimiter;
                  if (*ap != '\0') {
                     sprintf (ptr, "%c%s%c", parser->RaFieldQuoted, ap, parser->RaFieldQuoted);
                     ptr += strlen(ptr);
                  } else {
                     sprintf (ptr, "%c%c", parser->RaFieldQuoted, parser->RaFieldQuoted);
                  }
                  tstr = NULL;
               }
            } else
               bcopy (tmpbuf, parser->RaLabel, strlen(tmpbuf));
         }
      }
   }
   return (parser->RaLabel);
}

void
ArgusPrintCauseLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "Cause");
}

void
ArgusPrintBssidLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "Bssid");
}
 
void
ArgusPrintSsidLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "Ssid");
}
 
void
ArgusPrintStartDateLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->ArgusFractionalDate)
      len += parser->pflag;
   sprintf (buf, "%*.*s ", len, len, "StartTime");
}
 
void
ArgusPrintLastDateLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->ArgusFractionalDate)
      len += parser->pflag;
   sprintf (buf, "%*.*s ", len, len, "LastTime");
}

void
ArgusPrintSrcStartDateLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->ArgusFractionalDate)
      len += (parser->pflag + 1);
   sprintf (buf, "%*.*s ", len, len, "SrcStartTime");
}
 
void
ArgusPrintSrcLastDateLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->ArgusFractionalDate)
      len += (parser->pflag + 1);
   sprintf (buf, "%*.*s ", len, len, "SrcLastTime");
}
void
ArgusPrintDstStartDateLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->ArgusFractionalDate)
      len += (parser->pflag + 1);
   sprintf (buf, "%*.*s ", len, len, "DstStartTime");
}
 
void
ArgusPrintDstLastDateLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->ArgusFractionalDate)
      len += (parser->pflag + 1);
   sprintf (buf, "%*.*s ", len, len, "DstLastTime");
}

void
ArgusPrintRelativeDateLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "RelTime");
}

void
ArgusPrintSourceIDLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "SrcId");
}

void
ArgusPrintFlagsLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "Flgs");
}

void
ArgusPrintSrcMacAddressLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->RaMonMode) {
      sprintf (buf, "%*.*s ", len, len, "Mac");
   } else {
      sprintf (buf, "%*.*s ", len, len, "SrcMac");
   }
}

void
ArgusPrintDstMacAddressLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "DstMac");
}

void
ArgusPrintSrcOuiLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->RaMonMode) {
      sprintf (buf, "%*.*s ", len, len, "Oui");
   } else {
      sprintf (buf, "%*.*s ", len, len, "SrcOui");
   }
}

void
ArgusPrintDstOuiLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "DstOui");
}

void
ArgusPrintProtoLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "Proto");
}

void
ArgusPrintSrcNetLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->RaMonMode) {
      sprintf (buf, "%*.*s ", len, len, "Net");
   } else {
      sprintf (buf, "%*.*s ", len, len, "SrcNet");
   }
}

void
ArgusPrintSrcAddrLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->RaMonMode) {
      sprintf (buf, "%*.*s ", len, len, "Host");
   } else {
      if (parser->domainonly && (!parser->nflag)) {
         sprintf (buf, "%*.*s ", len, len, "SrcDomain");
      } else {
         sprintf (buf, "%*.*s ", len, len, "SrcAddr");
      }
   }
}

void
ArgusPrintDstNetLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "DstNet");
}

void
ArgusPrintDstAddrLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->domainonly && (!parser->nflag)) {
      sprintf (buf, "%*.*s ", len, len, "DstDomain");
   } else {
      sprintf (buf, "%*.*s ", len, len, "DstAddr");
   }
}

void
ArgusPrintSrcPortLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "Sport");
}

void
ArgusPrintDstPortLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "Dport");
}

void
ArgusPrintSrcIpIdLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "sIpId");
}

void
ArgusPrintDstIpIdLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "dIpId");
}
/*
ArgusPrintIpIdLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   ArgusPrintSrcIpIdLabel (parser, buf);
   ArgusPrintDstIpIdLabel (parser, buf);
}
*/
void
ArgusPrintSrcDSByteLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "sDSb");
}

void
ArgusPrintDstDSByteLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "dDSb");
}

void
ArgusPrintSrcTosLabel (struct ArgusParserStruct *parser, char *buf, int len)
{  
   sprintf (buf, "%*.*s ", len, len, "sTos");
}

void
ArgusPrintDstTosLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "dTos");
}
   
void
ArgusPrintSrcTtlLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "sTtl");
}

void
ArgusPrintDstTtlLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "dTtl");
}

void
ArgusPrintSrcHopCountLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "sHops");
}

void
ArgusPrintDstHopCountLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "dHops");
}

void
ArgusPrintDirLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "Dir");
}

void
ArgusPrintInodeLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "Inode");
}

void
ArgusPrintKeyStrokeNStrokeLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "NStrok");
}

void
ArgusPrintKeyStrokeSrcNStrokeLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "sNStrok");
}

void
ArgusPrintKeyStrokeDstNStrokeLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "dNStrok");
}


void
ArgusPrintPacketsLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->Pctflag)
      sprintf (buf, "%*.*s ", len, len, "pTotPkts");
   else
      sprintf (buf, "%*.*s ", len, len, "TotPkts");
}

void
ArgusPrintSrcPacketsLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->Pctflag) {
      if (parser->RaMonMode)
         sprintf (buf, "%*.*s ", len, len, "pOutPkts");
      else
         sprintf (buf, "%*.*s ", len, len, "pSrcPkts");
   } else {
      if (parser->RaMonMode)
         sprintf (buf, "%*.*s ", len, len, "OutPkts");
      else
         sprintf (buf, "%*.*s ", len, len, "SrcPkts");
   }
}

void
ArgusPrintDstPacketsLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->Pctflag) {
      if (parser->RaMonMode)
         sprintf (buf, "%*.*s ", len, len, "pInPkts");
      else
         sprintf (buf, "%*.*s ", len, len, "pDstPkts");
   } else {
      if (parser->RaMonMode)
         sprintf (buf, "%*.*s ", len, len, "InPkts");
      else
         sprintf (buf, "%*.*s ", len, len, "DstPkts");
   }
}

void
ArgusPrintBytesLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->Pctflag)
      sprintf (buf, "%*.*s ", len, len, "pTotBytes");
   else
      sprintf (buf, "%*.*s ", len, len, "TotBytes");
}

void
ArgusPrintSrcBytesLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->RaMonMode) {
      if (parser->Pctflag) {
         sprintf (buf, "%*.*s ", len, len, "pOutBytes");
      } else {
         sprintf (buf, "%*.*s ", len, len, "OutBytes");
      }
   } else {
      if (parser->Pctflag) {
         sprintf (buf, "%*.*s ", len, len, "pSrcBytes");
      } else {
         sprintf (buf, "%*.*s ", len, len, "SrcBytes");
      }
   }
}

void
ArgusPrintDstBytesLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->RaMonMode) {
      if (parser->Pctflag) {
         sprintf (buf, "%*.*s ", len, len, "pInBytes");
      } else {
         sprintf (buf, "%*.*s ", len, len, "InBytes");
      }
   } else {
      if (parser->Pctflag) {
         sprintf (buf, "%*.*s ", len, len, "pDstBytes");
      } else {
         sprintf (buf, "%*.*s ", len, len, "DstBytes");
      }
   }
}

void
ArgusPrintAppBytesLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->Pctflag)
      sprintf (buf, "%*.*s ", len, len, "pAppBytes");
   else
      sprintf (buf, "%*.*s ", len, len, "TotAppBytes");
}

void
ArgusPrintSrcAppBytesLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->RaMonMode) {
      if (parser->Pctflag) {
         sprintf (buf, "%*.*s ", len, len, "pOAppBytes");
      } else {
         sprintf (buf, "%*.*s ", len, len, "OAppBytes");
      } 
   } else {
      if (parser->Pctflag) {
         sprintf (buf, "%*.*s ", len, len, "pSAppBytes");
      } else {
         sprintf (buf, "%*.*s ", len, len, "SAppBytes");
      }
   }
}

void
ArgusPrintDstAppBytesLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->RaMonMode) {
      if (parser->Pctflag) {
         sprintf (buf, "%*.*s ", len, len, "pIAppBytes");
      } else {
         sprintf (buf, "%*.*s ", len, len, "IAppBytes");
      }
   } else {
      if (parser->Pctflag) {
         sprintf (buf, "%*.*s ", len, len, "pDAppBytes");
      } else {
         sprintf (buf, "%*.*s ", len, len, "DAppBytes");
      }
   }
}

void
ArgusPrintSrcIntPktLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "SIntPkt");
}

void
ArgusPrintSrcIntPktDistLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "SIntDist");
}
 
void
ArgusPrintDstIntPktLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "DIntPkt");
}

void
ArgusPrintDstIntPktDistLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "DIntDist");
}
 
void
ArgusPrintActiveSrcIntPktLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "SIntPktAct");
}
 
void
ArgusPrintActiveSrcIntPktDistLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "SIntActDist");
}
 
void
ArgusPrintActiveDstIntPktLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "DIntPktAct");
}
 
void
ArgusPrintActiveDstIntPktDistLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "DIntActDist");
}
 
void
ArgusPrintIdleSrcIntPktLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "SIntPktIdl");
}
 
void
ArgusPrintIdleSrcIntPktDistLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "SIntIdlDist");
}
 
void
ArgusPrintIdleDstIntPktLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "DIntPktIdl");
}
 
void
ArgusPrintIdleDstIntPktDistLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "DIntIdlDist");
}
 
void
ArgusPrintSrcIntPktMaxLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "SIntPktMax");
}

void
ArgusPrintSrcIntPktMinLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "SIntPktMin");
}

void
ArgusPrintDstIntPktMaxLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "DIntPktMax");
}

void
ArgusPrintDstIntPktMinLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "DIntPktMin");
}

void
ArgusPrintActiveSrcIntPktMaxLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "SIPActMax");
}

void
ArgusPrintActiveSrcIntPktMinLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "SIPActMin");
}

void
ArgusPrintActiveDstIntPktMaxLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "DIPActMax");
}

void
ArgusPrintActiveDstIntPktMinLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "DIPActMin");
}

void
ArgusPrintIdleSrcIntPktMaxLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "SIPIdlMax");
}

void
ArgusPrintIdleSrcIntPktMinLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "SIPIdlMin");
}

void
ArgusPrintIdleDstIntPktMaxLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "DIPIdlMax");
}

void
ArgusPrintIdleDstIntPktMinLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "DIPIdlMin");
}

void
ArgusPrintSrcJitterLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "SrcJitter");
}

void
ArgusPrintDstJitterLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "DstJitter");
}

void
ArgusPrintActiveSrcJitterLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "SrcJitAct");
}

void
ArgusPrintActiveDstJitterLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "DstJitAct");
}

void
ArgusPrintIdleSrcJitterLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "SrcJitIdl");
}

void
ArgusPrintIdleDstJitterLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "DstJitIdl");
}

void
ArgusPrintStateLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "State");
}

void
ArgusPrintTCPSrcBaseLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "SrcTCPBase");
}

void
ArgusPrintTCPDstBaseLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "DstTCPBase");
}

void
ArgusPrintTCPRTTLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "TcpRtt");
}

void
ArgusPrintTCPSynAckLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "SynAck");
}

void
ArgusPrintTCPAckDatLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "AckDat");
}

void
ArgusPrintTCPSrcMaxLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "STcpMax");
}

void
ArgusPrintTCPDstMaxLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "DTcpMax");
}

void
ArgusPrintSrcGapLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "SrcGap");
}

void
ArgusPrintDstGapLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "DstGap");
}


void
ArgusPrintDeltaDurationLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "dlDur");
}

void
ArgusPrintDeltaStartTimeLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "dlsTime");
}

void
ArgusPrintDeltaLastTimeLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "dllTime");
}

void
ArgusPrintDeltaSrcPktsLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "dsPkts");
}

void
ArgusPrintDeltaDstPktsLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "ddPkts");
}

void
ArgusPrintDeltaSrcBytesLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "dsBytes");
}

void
ArgusPrintDeltaDstBytesLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "ddBytes");
}

void
ArgusPrintPercentDeltaSrcPktsLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "pdsPkt");
}

void
ArgusPrintPercentDeltaDstPktsLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "pddPkt");
}

void
ArgusPrintPercentDeltaSrcBytesLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "pdsByte");
}

void
ArgusPrintPercentDeltaDstBytesLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "pddByte");
}

void
ArgusPrintSrcUserDataLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   int slen = 0;
 
   if (len > 0) {
      switch (parser->eflag) {
         case ARGUS_HEXDUMP:
            return;

         case ARGUS_ENCODE_OBFUSCATE:
         case ARGUS_ENCODE_ASCII:
            slen = len;
            break;
 
         case ARGUS_ENCODE_32:
         case ARGUS_ENCODE_64:
            slen = len * 2;
            break;
      }
 
      if (len > 10) slen++;
      sprintf (buf, "%*ssrcUdata%*s ", (slen)/2, " ", (slen)/2, " ");
      if (slen & 0x01)
         sprintf (&buf[strlen(buf)], " ");
   }
}

void
ArgusPrintDstUserDataLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   int slen = 0;

   if (len > 0) {
      switch (parser->eflag) {
         case ARGUS_HEXDUMP:
            return;

         case ARGUS_ENCODE_OBFUSCATE:
         case ARGUS_ENCODE_ASCII:
            slen = len;
            break;

         case ARGUS_ENCODE_32:
         case ARGUS_ENCODE_64:
            slen = len * 2;
            break;
      }

      if (len > 10) slen++;
      sprintf (buf, "%*sdstUdata%*s ", (slen)/2, " ", (slen)/2, " ");
      if (slen & 0x01)
         sprintf (&buf[strlen(buf)], " ");
   }
}

/*
void
ArgusPrintUserDataLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   ArgusPrintSrcUserDataLabel (parser, buf);
   ArgusPrintDstUserDataLabel (parser, buf);
}
*/

void
ArgusPrintTCPOptionsLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "TcpOpt");
}

void
ArgusPrintTCPExtensionsLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
}

void
ArgusPrintSrcRateLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "SrcRate");
}

void
ArgusPrintDstRateLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "DstRate");
}

void
ArgusPrintRateLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "Rate");
}


void
ArgusPrintSrcLossLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->Pctflag)
      sprintf (buf, "%*.*s ", len, len, "pSrcLoss");
   else
      sprintf (buf, "%*.*s ", len, len, "SrcLoss");
}

void
ArgusPrintDstLossLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->Pctflag)
      sprintf (buf, "%*.*s ", len, len, "pDstLoss");
   else
      sprintf (buf, "%*.*s ", len, len, "DstLoss");
}

void
ArgusPrintLossLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->Pctflag)
      sprintf (buf, "%*.*s ", len, len, "pLoss");
   else
      sprintf (buf, "%*.*s ", len, len, "Loss");
}

void
ArgusPrintSrcRetransLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->Pctflag)
      sprintf (buf, "%*.*s ", len, len, "pSrcRetrans");
   else
      sprintf (buf, "%*.*s ", len, len, "SrcRetrans");
}

void
ArgusPrintDstRetransLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->Pctflag)
      sprintf (buf, "%*.*s ", len, len, "pDstRetrans");
   else
      sprintf (buf, "%*.*s ", len, len, "DstRetrans");
}

void
ArgusPrintRetransLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->Pctflag)
      sprintf (buf, "%*.*s ", len, len, "pRetrans");
   else
      sprintf (buf, "%*.*s ", len, len, "Retrans");
}

void
ArgusPrintSrcSoloLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->Pctflag)
      sprintf (buf, "%*.*s ", len, len, "pSrcSolo");
   else
      sprintf (buf, "%*.*s ", len, len, "SrcSolo");
}

void
ArgusPrintDstSoloLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->Pctflag)
      sprintf (buf, "%*.*s ", len, len, "pDstSolo");
   else
      sprintf (buf, "%*.*s ", len, len, "DstSolo");
}

void
ArgusPrintSoloLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->Pctflag)
      sprintf (buf, "%*.*s ", len, len, "pSolo");
   else
      sprintf (buf, "%*.*s ", len, len, "Solo");
}

void
ArgusPrintPercentSrcSoloLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "pSrcSolo");
}

void
ArgusPrintPercentDstSoloLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "pDstSolo");
}


void
ArgusPrintPercentSoloLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "pSolo");
}


void
ArgusPrintSrcFirstLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->Pctflag)
      sprintf (buf, "%*.*s ", len, len, "pSrcFirst");
   else
      sprintf (buf, "%*.*s ", len, len, "SrcFirst");
}

void
ArgusPrintDstFirstLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->Pctflag)
      sprintf (buf, "%*.*s ", len, len, "pDstFirst");
   else
      sprintf (buf, "%*.*s ", len, len, "DstFirst");
}

void
ArgusPrintFirstLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->Pctflag)
      sprintf (buf, "%*.*s ", len, len, "pFirst");
   else
      sprintf (buf, "%*.*s ", len, len, "First");
}

void
ArgusPrintPercentSrcFirstLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "pSrcFirst");
}

void
ArgusPrintPercentDstFirstLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "pDstFirst");
}


void
ArgusPrintPercentFirstLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "pFirst");
}

void
ArgusPrintPercentSrcLossLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "pSrcLoss");
}

void
ArgusPrintPercentDstLossLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "pDstLoss");
}


void
ArgusPrintPercentLossLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "pLoss");
}

void
ArgusPrintPercentSrcRetransLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "pSrcRetrans");
}

void
ArgusPrintPercentDstRetransLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "pDstRetrans");
}


void
ArgusPrintPercentRetransLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "pRetrans");
}


void
ArgusPrintSrcNacksLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->Pctflag)
      sprintf (buf, "%*.*s ", len, len, "pSrcNacks");
   else
      sprintf (buf, "%*.*s ", len, len, "SrcNacks");
}

void
ArgusPrintDstNacksLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->Pctflag)
      sprintf (buf, "%*.*s ", len, len, "pDstNacks");
   else
      sprintf (buf, "%*.*s ", len, len, "DstNacks");
}

void
ArgusPrintNacksLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   if (parser->Pctflag)
      sprintf (buf, "%*.*s ", len, len, "pNacks");
   else
      sprintf (buf, "%*.*s ", len, len, "Nacks");
}


void
ArgusPrintPercentSrcNacksLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "pSrcNacks");
}

void
ArgusPrintPercentDstNacksLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "pDstNacks");
}


void
ArgusPrintPercentNacksLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "pNacks");
}


void
ArgusPrintSrcLoadLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   char *ptr;
   if (parser->Aflag)
      ptr = "SrcAppLoad";
   else
      ptr = "SrcLoad";

   sprintf (buf, "%*.*s ", len, len, ptr);
}

void
ArgusPrintDstLoadLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   char *ptr;
   if (parser->Aflag)
      ptr = "DstAppLoad";
   else
      ptr = "DstLoad";

   sprintf (buf, "%*.*s ", len, len, ptr);
}

void
ArgusPrintLoadLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   char *ptr;
   if (parser->Aflag)
      ptr = "AppLoad";
   else 
      ptr = "Load";
   sprintf (buf, "%*.*s ", len, len, ptr);
}

void
ArgusPrintSrcMplsLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "sMpls");
}

void
ArgusPrintDstMplsLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "dMpls");
}

void
ArgusPrintSrcVlanLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "sVlan");
}

void
ArgusPrintDstVlanLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "dVlan");
}


void
ArgusPrintSrcVIDLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "sVid");
}

void
ArgusPrintDstVIDLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "dVid");
}


void
ArgusPrintSrcVPRILabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "sVpri");
}

void
ArgusPrintDstVPRILabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "dVpri");
}

void
ArgusPrintJoinDelayLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "JDelay");
}

void
ArgusPrintLeaveDelayLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "LDelay");
}


void
ArgusPrintSrcWindowLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "SrcWin");
}

void
ArgusPrintDstWindowLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "DstWin");
}

void
ArgusPrintDurationLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "Dur");
}

void
ArgusPrintSrcDurationLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "SrcDur");
}

void
ArgusPrintDstDurationLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "DstDur");
}

void
ArgusPrintMeanLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "Mean");
}

void
ArgusPrintSumLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "Sum");
}

void
ArgusPrintIdleTimeLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "IdleTime");
}

void
ArgusPrintRunTimeLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "RunTime");
}

void
ArgusPrintMinLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "Min");
}

void
ArgusPrintMaxLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "Max");
}

void
ArgusPrintStdDeviationLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "StdDev");
}

void
ArgusPrintStartRangeLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "SRange");
}

void
ArgusPrintEndRangeLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "ERange");
}

void
ArgusPrintTransactionsLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   char *ptr;
   if (parser->Pctflag)
      ptr = "pTrans";
   else
      ptr = "Trans";
   sprintf (buf, "%*.*s ", len, len, ptr);
}

void
ArgusPrintSequenceNumberLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*sSeq%*s ", (len - 3)/2, " ", (len - 3)/2, " ");
   if (!(len & 0x01)) sprintf (&buf[strlen(buf)], " ");
}

void
ArgusPrintRankLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*sRank%*s", (len - 4)/2, " ", (len - 4)/2, " ");
   if (!(len & 0x01)) sprintf (&buf[strlen(buf)], " ");
}

void
ArgusPrintBinNumberLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*sBin%*s ", (len - 3)/2, " ", (len - 3)/2, " ");
   if (!(len & 0x01)) sprintf (&buf[strlen(buf)], " ");
}

void
ArgusPrintBinsLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*sBins%*s ", (len - 4)/2, " ", (len - 4)/2, " ");
   if (!(len & 0x01)) sprintf (&buf[strlen(buf)], " ");
}

void
ArgusPrintByteOffsetLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "Offset");
}

void
ArgusPrintAutoIdLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "AutoId");
}

void
ArgusPrintSrcEncapsLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "sEnc");
}

void
ArgusPrintDstEncapsLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "dEnc");
}

void
ArgusPrintSrcPktSizeLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "sPktSz");
}

void
ArgusPrintSrcMaxPktSizeLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "sMaxPktSz");
}

void
ArgusPrintSrcMinPktSizeLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "sMinPktSz");
}

void
ArgusPrintSrcMeanPktSizeLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "sMeanPktSz");
}


void
ArgusPrintDstPktSizeLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "dPktSz");
}

void
ArgusPrintDstMaxPktSizeLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "dMaxPktSz");
}

void
ArgusPrintDstMinPktSizeLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "dMinPktSz");
}

void
ArgusPrintDstMeanPktSizeLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "dMeanPktSz");
}

void
ArgusPrintSrcCountryCodeLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "sCo");
}

void
ArgusPrintDstCountryCodeLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "dCo");
}

void
ArgusPrintSrcAsnLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "sAS");
}

void
ArgusPrintDstAsnLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "dAS");
}

void
ArgusPrintInodeAsnLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "iAS");
}

void
ArgusPrintIcmpIdLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "IcmpId");
}

void
ArgusPrintResponseLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "Response");
}

void
ArgusPrintLabelLabel (struct ArgusParserStruct *parser, char *buf, int len)
{
   sprintf (buf, "%*.*s ", len, len, "Label");
}


void ArgusDump (const u_char *, int, char *);

void
ArgusPrintSrcUserData (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusDataStruct *user = NULL;
   char strbuf[MAXSTRLEN], *str = strbuf;
   char conbuf[MAXSTRLEN], *con = conbuf;
   int slen = 0, exlen = len;
   char delim = ' ';

   if ((parser->RaFieldDelimiter != ' ') && (parser->RaFieldDelimiter != '\0'))
      delim = parser->RaFieldDelimiter;

   bzero (conbuf, sizeof(conbuf));
   bzero (strbuf, sizeof(strbuf));
   bzero (buf, len);

   if (len > 0) {
      switch (parser->eflag) {
         case ARGUS_HEXDUMP:
            return;
            break;

         case ARGUS_ENCODE_ASCII:
         case ARGUS_ENCODE_OBFUSCATE:
            exlen = len;
            break;

         case ARGUS_ENCODE_32:
         case ARGUS_ENCODE_64:
            exlen = len * 2;
            break;
      }
      exlen += 8;
      if (len >= 10) exlen++;
      if (len >= 100) exlen++;

      if ((user = (struct ArgusDataStruct *)argus->dsrs[ARGUS_SRCUSERDATA_INDEX]) != NULL) {
         unsigned short *sptr = &user->hdr.argus_dsrvl16.len;
         slen = (*sptr - 2 ) * 4;

         slen = (user->count < len) ? user->count : slen;
         slen = (slen > len) ? len : slen;

         bzero (strbuf, sizeof(strbuf));

         if ((slen = ArgusEncode (parser, (const char *)&user->array, NULL, slen, str, sizeof(strbuf))) > 0) {
            if (parser->ArgusPrintXml) {
               switch (parser->eflag) {
                  case ARGUS_ENCODE_OBFUSCATE:
                  case ARGUS_ENCODE_ASCII: {
                     char xmlbuf[MAXSTRLEN], *dptr = str;
                     int i, dlen;

                     bzero(xmlbuf, sizeof(xmlbuf));
                     for (i = 0; i < slen; i++) {
                        if (*dptr == '&')
                           sprintf(&xmlbuf[strlen(xmlbuf)], "&amp;");
                        else if (*dptr == '"')
                           sprintf(&xmlbuf[strlen(xmlbuf)], "&quot;");
                        else if (*dptr == '\'')
                           sprintf(&xmlbuf[strlen(xmlbuf)], "&#39;");
                        else if (*dptr == '<')
                           sprintf(&xmlbuf[strlen(xmlbuf)], "&lt;");
                        else if (*dptr == '>')
                           sprintf(&xmlbuf[strlen(xmlbuf)], "&gt;");
                        else
                           sprintf(&xmlbuf[strlen(xmlbuf)], "%c", *dptr);
                        dptr++;
                     }
                     dlen = strlen(xmlbuf);
                     sprintf (con, "%*.*s", dlen, dlen, xmlbuf);
                     break;
                  }
                  default:
                     sprintf (con, "%s", str);
                     break;
               }
            } else {
               sprintf (con, "s[%d]=%s", slen, str);
            }
         }
      }
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " SrcUserData = \"%s\"", con);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH)
         exlen = strlen(con);
      sprintf (buf, "%-*.*s ", exlen, exlen, con);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcUserData (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintDstUserData (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusDataStruct *user = NULL;
   char strbuf[MAXSTRLEN], *str = strbuf;
   char conbuf[MAXSTRLEN], *con = conbuf;
   int slen = 0, exlen = len;
   char delim = ' ';

   if ((parser->RaFieldDelimiter != ' ') && (parser->RaFieldDelimiter != '\0'))
      delim = parser->RaFieldDelimiter;

   bzero (conbuf, sizeof(conbuf));
   bzero (buf, len);

   if (len > 0) {
      switch (parser->eflag) {
         case ARGUS_HEXDUMP:
            return;
            break;

         case ARGUS_ENCODE_OBFUSCATE:
         case ARGUS_ENCODE_ASCII:
            exlen = len;
            break;

         case ARGUS_ENCODE_32:
         case ARGUS_ENCODE_64:
            exlen = len * 2;
            break;
      }
      exlen += 8;
      if (len > 10) exlen++;

      if ((user = (struct ArgusDataStruct *)argus->dsrs[ARGUS_DSTUSERDATA_INDEX]) != NULL) {
         unsigned short *sptr = &user->hdr.argus_dsrvl16.len;
         slen = (*sptr - 2 ) * 4;
         slen = (user->count < slen) ? user->count : slen;
         slen = (slen > len) ? len : slen;
     
         bzero (strbuf, sizeof(strbuf));

         if ((slen = ArgusEncode (parser, (const char *)&user->array, NULL, slen, str, sizeof(strbuf))) > 0) {
            if (parser->ArgusPrintXml) {
               switch (parser->eflag) {
                  case ARGUS_ENCODE_OBFUSCATE:
                  case ARGUS_ENCODE_ASCII: {
                     char xmlbuf[MAXSTRLEN], *dptr = str;
                     int i, dlen;

                     bzero(xmlbuf, sizeof(xmlbuf));
                     for (i = 0; i < slen; i++) {
                        if (*dptr == '&')
                           sprintf(&xmlbuf[strlen(xmlbuf)], "&amp;");
                        else if (*dptr == '<')
                           sprintf(&xmlbuf[strlen(xmlbuf)], "&lt;");
                        else if (*dptr == '>')
                           sprintf(&xmlbuf[strlen(xmlbuf)], "&gt;");
                        else
                           sprintf(&xmlbuf[strlen(xmlbuf)], "%c", *dptr);
                        dptr++;
                     }
                     dlen = strlen(xmlbuf);
                     sprintf (con, "%*.*s", dlen, dlen, xmlbuf);
                     break;
                  }
                  default:
                     sprintf (con, "%s", str);
                     break;
               }
            } else {
               sprintf (con, "d[%d]=%s", slen, str);
            } 
         }
      }
   }

   if (parser->ArgusPrintXml) {
      sprintf (buf, " DstUserData = \"%s\"", con);
   } else {
      if (parser->RaFieldWidth != RA_FIXED_WIDTH)
         exlen = strlen(con);
      sprintf (buf, "%-*.*s ", exlen, exlen, con);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstUserData (%p, %p)", buf, argus);
#endif
}

/*
void
ArgusPrintUserData (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   ArgusPrintSrcUserData(parser, buf, argus);
   ArgusPrintDstUserData(parser, buf, argus);

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintUserData (%p, %p)", buf, argus);
#endif
}
*/


static char basis_64[] =
   "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/???????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????";


int
ArgusEncode (struct ArgusParserStruct *parser, const char *ptr, const char *mask, int len, char *str, int slen)
{
   int retn = 0, i;

   switch (parser->eflag) {
      case ARGUS_ENCODE_32:
         retn = ArgusEncode32(parser, ptr, len, &str[strlen(str)], slen - strlen(str));
         if (mask != NULL) {
            for (i = 0; i < len; i++) {
               if ((mask[i/8] & (0x80 >> (i % 8)))) {
                  str[((2*i))]     = ' ';
                  str[((2*i)) + 1] = ' ';
               }
            }
         }
         break;

      case ARGUS_ENCODE_64:
         retn = ArgusEncode64(parser, ptr, len, &str[strlen(str)], slen - strlen(str));
         break;

      case ARGUS_ENCODE_OBFUSCATE:
      case ARGUS_ENCODE_ASCII:
         retn = ArgusEncodeAscii(parser, ptr, len, &str[strlen(str)], slen - strlen(str));
         if (mask != NULL) {
            for (i = 0; i < len; i++)
               if ((mask[i/8] & (0x80 >> (i % 8))))
                  str[i] = ' ';
         }
         break;

      default:
         break;
   }


   return (retn);
}

static char basis_16[] = "0123456789ABCDEF";

int
ArgusEncode32 (struct ArgusParserStruct *parser, const char *ptr, int len, char *str, int slen)
{
   int retn = 0, i;
   u_char *buf = (u_char *) str;
   unsigned newlen;

   if (ptr && ((newlen = (((len + 1) & ~0x01) * 2)) < slen)) {
      for (i = 0; i < len; i++) {
         *buf++ = basis_16[((ptr[i] & 0xF0) >> 4)];
         *buf++ = basis_16[((ptr[i] & 0x0F))];
      }

      retn = newlen;
   }
   
   return (retn);
}


int
ArgusEncode64 (struct ArgusParserStruct *parser, const char *ptr, int len, char *str, int slen)
{
   int retn = 0;
   const u_char *in = (const u_char *)ptr;
   u_char *buf = (u_char *) str;
   u_char oval;
   unsigned newlen;

   if (ptr && ((newlen = (len + 2) / 3 * 4) < slen)) {
      while (len >= 3) {
          *buf++ = basis_64[in[0] >> 2];
          *buf++ = basis_64[((in[0] << 4) & 0x30) | (in[1] >> 4)];
          *buf++ = basis_64[((in[1] << 2) & 0x3c) | (in[2] >> 6)];
          *buf++ = basis_64[in[2] & 0x3f];
          in += 3;
          len -= 3;
      }
      if (len > 0) {
          *buf++ = basis_64[in[0] >> 2];
          oval = (in[0] << 4) & 0x30;
          if (len > 1) oval |= in[1] >> 4;
          *buf++ = basis_64[oval];
          *buf++ = (len < 2) ? '=' : basis_64[(in[1] << 2) & 0x3c];
          *buf++ = '=';
      }

      retn = newlen;
   }
   
   return (retn);
}

#include <ctype.h>

int
ArgusEncodeAscii (struct ArgusParserStruct *parser, const char *ptr, int len, char *str, int slen)
{
   int retn = 0, newlen = len;
   u_char *buf = (u_char *) str;

   if (ptr && (len < slen)) {
      int blen = len;
      while (blen > 0) {
         if (isascii((int)*ptr) && isprint((int)*ptr))
            *buf = *ptr;
         else
            *buf = '.';
         buf++;
         ptr++;
         blen--;
      }

      if (parser->eflag == ARGUS_ENCODE_OBFUSCATE) {
         int blen = len;
         while ((blen > 0) && ((buf = (u_char *) strstr (str, "PASS")) != NULL)) {
            buf += 5;
            blen -= 5;
            while (((void *)(str + newlen) > (void *) buf) && ((*buf != ' ') && (*buf != '.'))) {
               *buf++ = 'x';
               blen--;
            }
            str = (char *) buf;
         }
      }

      retn = newlen;
   }
   
   return (retn);
}


struct ArgusQueueStruct *
ArgusNewQueue ()
{
   struct ArgusQueueStruct *retn =  NULL;

   if ((retn = (struct ArgusQueueStruct *) ArgusCalloc (1, sizeof (struct ArgusQueueStruct))) != NULL) {
      retn->count = 0;
#if defined(ARGUS_THREADS)
      pthread_mutex_init(&retn->lock, NULL);
#endif
      retn->start = NULL;
      retn->end   = NULL;
   }

#ifdef ARGUSDEBUG
   ArgusDebug (4, "ArgusNewQueue () returning 0x%x\n", retn);
#endif

   return (retn);
}

void
ArgusDeleteQueue (struct ArgusQueueStruct *queue)
{
   struct ArgusQueueHeader *obj = NULL;

   if (queue != NULL) {
#if defined(ARGUS_THREADS)
      pthread_mutex_lock(&queue->lock); 
#endif

      while ((obj = ArgusPopQueue(queue, ARGUS_NOLOCK)))
         ArgusFree(obj);

      if (queue->array != NULL) {
         ArgusFree(queue->array);
         queue->array = NULL;
      }

#if defined(ARGUS_THREADS)
      pthread_mutex_destroy(&queue->lock);
#endif
      ArgusFree(queue);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (4, "ArgusDeleteQueue (0x%x) returning\n", queue);
#endif
}



int
ArgusGetQueueCount(struct ArgusQueueStruct *queue)
{

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusGetQueueCount (0x%x) returning %d\n", queue, queue->count);
#endif

   return (queue->count);
}


// Add object to start of queue

void
ArgusPushQueue(struct ArgusQueueStruct *queue, struct ArgusQueueHeader *obj, int type)
{
   int retn = 0;

#if defined(ARGUS_THREADS)
   if (type == ARGUS_LOCK)
      pthread_mutex_lock(&queue->lock); 
#endif
   if ((retn = ArgusAddToQueue (queue, obj, ARGUS_NOLOCK)) > 0) {
      queue->start = queue->start->prv;
      queue->end   = queue->start->prv;
   }

#if defined(ARGUS_THREADS)
   if (type == ARGUS_LOCK)
      pthread_mutex_unlock(&queue->lock); 
#endif

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPushQueue (0x%x, 0x%x) returning\n", queue, obj);
#endif
}


// Add object to end of queue

int
ArgusAddToQueue(struct ArgusQueueStruct *queue, struct ArgusQueueHeader *obj, int type)
{
   int retn = 0;

   if (queue && obj) {
      if (obj->queue == NULL) {
#if defined(ARGUS_THREADS)
         if (type == ARGUS_LOCK)
            pthread_mutex_lock(&queue->lock); 
#endif
         if (queue->start != NULL) {
            obj->prv = queue->start->prv;
            queue->start->prv = obj;
            obj->nxt = queue->start;
            obj->prv->nxt = obj;
         } else {
            queue->start = obj;
            obj->nxt = obj;
            obj->prv = obj;
         }
         queue->end = obj;
         queue->count++;
#if defined(ARGUS_THREADS)
         if (type == ARGUS_LOCK)
            pthread_mutex_unlock(&queue->lock); 
#endif
         obj->queue = queue;

         if (ArgusParser->status & ARGUS_REAL_TIME_PROCESS) {
            obj->lasttime = ArgusParser->ArgusGlobalTime;
         } else {
            gettimeofday(&obj->lasttime, 0L);
         }
         retn = 1;

      } else
         ArgusLog (LOG_ERR, "ArgusAddToQueue (%p, %p) obj in queue %p\n", queue, obj, obj->queue);
   } else
      ArgusLog (LOG_ERR, "ArgusAddToQueue (%p, %p) parameter error\n", queue, obj);

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusAddToQueue (%p, %p) returning %d\n", queue, obj, retn);
#endif

   return (retn);
}


struct ArgusQueueHeader *
ArgusPopQueue (struct ArgusQueueStruct *queue, int type)
{
   struct ArgusQueueHeader *retn = NULL;
   struct ArgusQueueHeader *obj = NULL;

   if (queue) { 
#if defined(ARGUS_THREADS)
      if (type == ARGUS_LOCK)
         pthread_mutex_lock(&queue->lock); 
#endif
      if (queue->count) {
         if ((obj = (struct ArgusQueueHeader *) queue->start) != NULL) {
            queue->count--;

            if (queue->count) {
               if (queue->start == obj)
                  queue->start = obj->nxt;

               obj->prv->nxt = obj->nxt;
               obj->nxt->prv = obj->prv;

               queue->end    = queue->start->prv;

            } else {
               queue->start = NULL;
               queue->end   = NULL;
            }
         }
         if (obj != NULL) {
            obj->prv = NULL;
            obj->nxt = NULL;
            obj->queue = NULL;
            retn = obj;
         }
      }
#if defined(ARGUS_THREADS)
      if (type == ARGUS_LOCK)
         pthread_mutex_unlock(&queue->lock); 
#endif
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPopQueue (%p) returning %p\n", queue, retn);
#endif
   
   return(retn);
}


struct ArgusQueueHeader *
ArgusRemoveFromQueue(struct ArgusQueueStruct *queue, struct ArgusQueueHeader *obj, int type)
{
   struct ArgusQueueHeader *retn = NULL;

   if ((queue != NULL) && (obj != NULL)) {
      if (obj->queue == queue) {
#if defined(ARGUS_THREADS)
         if (type == ARGUS_LOCK)
            pthread_mutex_lock(&queue->lock); 
#endif
         if (queue->count) {
            queue->count--;

            if (queue->count) {
               if (queue->start == obj)
                  queue->start = obj->nxt;

               obj->prv->nxt = obj->nxt;
               obj->nxt->prv = obj->prv;

               queue->end    = queue->start->prv;

            } else {
               queue->start = NULL;
               queue->end   = NULL;
            }
         }
#if defined(ARGUS_THREADS)
         if (type == ARGUS_LOCK)
            pthread_mutex_unlock(&queue->lock); 
#endif
         obj->prv = NULL;
         obj->nxt = NULL;
         obj->queue = NULL;
         retn = obj;

      } else
         ArgusLog (LOG_ERR, "ArgusRemoveFromQueue(%p, %p) obj not in queue\n", queue, obj);
   } else
      ArgusLog (LOG_ERR, "ArgusRemoveFromQueue(%p, %p) parameter error\n", queue, obj);

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusRemoveFromQueue (%p, %p) returning %p\n", queue, obj, obj);
#endif

   return (retn);
}


struct ArgusListStruct *
ArgusNewList ()
{
   struct ArgusListStruct *retn = NULL;
 
   if ((retn = (struct ArgusListStruct *) ArgusCalloc (1, sizeof (struct ArgusListStruct))) != NULL) {
#if defined(ARGUS_THREADS)
      pthread_mutex_init(&retn->lock, NULL);
      pthread_cond_init(&retn->cond, NULL);
#endif
   }

#ifdef ARGUSDEBUG
   ArgusDebug (4, "ArgusNewList () returning %p\n", retn);
#endif

   return (retn);
}

void
ArgusDeleteList (struct ArgusListStruct *list, int type)
{
   if (list) {
      while (list->start) {
         struct ArgusListRecord *retn = ArgusPopFrontList(list, ARGUS_LOCK);
         switch (type) {
             case ARGUS_RFILE_LIST: {
                struct ArgusRfileStruct *rfile = (struct ArgusRfileStruct *) retn;
                if (rfile->name != NULL)
                   free(rfile->name);
                ArgusFree(retn);
                break;
             }

             case ARGUS_WFILE_LIST: {
                struct ArgusWfileStruct *wfile = (struct ArgusWfileStruct *) retn;
                if (wfile->filename != NULL)
                   free(wfile->filename);
                if (wfile->filterstr != NULL)
                   free(wfile->filterstr);
                ArgusFree(retn);
                break;
             }

             case ARGUS_OUTPUT_LIST:
                ArgusDeleteRecordStruct(ArgusParser, (struct ArgusRecordStruct *)retn);
                break;
         }
      }

      ArgusFree (list);
   }

#ifdef ARGUSDEBUG
   ArgusDebug (3, "ArgusDeleteList (%p, %d) returning\n", list, type);
#endif
}

int
ArgusListEmpty (struct ArgusListStruct *list)
{
   return (list->start == NULL);
}

int
ArgusGetListCount(struct ArgusListStruct *list)
{
   return (list->count);
}


int
ArgusPushFrontList(struct ArgusListStruct *list, struct ArgusListRecord *rec, int lstat)
{
   int retn = 0;

   if (list && rec) {
#if defined(ARGUS_THREADS)
      if (lstat)
         pthread_mutex_lock(&list->lock);
#endif
      if (list->start) {
         rec->nxt = list->start;
      } else {
         rec->nxt = NULL;
      }
      list->start = (struct ArgusListObjectStruct *) rec;
      if (list->end == NULL)
         list->end = (struct ArgusListObjectStruct *) rec;
      list->count++;
#if defined(ARGUS_THREADS)
      if (lstat)
         pthread_mutex_unlock(&list->lock);
#endif
      retn++;
   }

#ifdef ARGUSDEBUG
   ArgusDebug (6, "ArgusPushFrontList (0x%x, 0x%x, %d) returning 0x%x\n", list, rec, lstat);
#endif

   return (retn);
}

int
ArgusPushBackList(struct ArgusListStruct *list, struct ArgusListRecord *rec, int lstat)
{
   int retn = 0;

   if (list && rec) {
      rec->nxt = NULL;
   
#if defined(ARGUS_THREADS)
      if (lstat)
         pthread_mutex_lock(&list->lock);
#endif
      if (list->end) {
         list->end->nxt = (struct ArgusListObjectStruct *) rec;
      } else {
         list->start = (struct ArgusListObjectStruct *) rec;
      }
      list->end = (struct ArgusListObjectStruct *) rec;
      list->count++;
#if defined(ARGUS_THREADS)
      if (lstat)
         pthread_mutex_unlock(&list->lock);
#endif
      retn++;
   }
#ifdef ARGUSDEBUG
   ArgusDebug (6, "ArgusPushBackList (0x%x, 0x%x, %d) returning %d\n", list, rec, lstat, retn);
#endif

   return (retn);
}

void ArgusLoadList(struct ArgusListStruct *, struct ArgusListStruct *);


void
ArgusLoadList(struct ArgusListStruct *l1, struct ArgusListStruct *l2)
{
   if (l1 && l2) {
#if defined(ARGUS_THREADS)
      pthread_mutex_lock(&l1->lock);
      pthread_mutex_lock(&l2->lock);
#endif
   
      if (l2->start == NULL)
         l2->start = l1->start;
      else
         l2->end->nxt = l1->start;

      l2->end = l1->end;
      l2->count += l1->count;

      l1->start = NULL;
      l1->end = NULL;
      l1->count = 0;

#if defined(ARGUS_THREADS)
      pthread_mutex_unlock(&l2->lock);
      pthread_mutex_unlock(&l1->lock);
#endif  
   }
}

struct ArgusListRecord *
ArgusFrontList(struct ArgusListStruct *list)
{
   return ((struct ArgusListRecord *) list->start);
}

struct ArgusListRecord *
ArgusPopFrontList(struct ArgusListStruct *list, int lstat)
{
   struct ArgusListRecord *retn = NULL;

#if defined(ARGUS_THREADS)
   if (lstat)
      pthread_mutex_lock(&list->lock);
#endif
   if ((retn = (struct ArgusListRecord *) list->start)) {
      if (--list->count == 0) {
         list->start = NULL;
         list->end = NULL;
      } else 
         list->start = retn->nxt;
   }
#if defined(ARGUS_THREADS)
   if (lstat)
      pthread_mutex_unlock(&list->lock);
#endif

#ifdef ARGUSDEBUG
   ArgusDebug (9, "ArgusPopFrontList (0x%x) returning\n", retn);
#endif

   return (retn);
}


#ifdef NOVFPRINTF
/*
 * Stock 4.3 doesn't have vfprintf. 
 * This routine is due to Chris Torek.
 */

int
vfprintf(f, fmt, args)
FILE *f;
char *fmt;
va_list args;
{
   int ret;
 
   if ((f->_flag & _IOWRT) == 0) {
      if (f->_flag & _IORW)
         f->_flag |= _IOWRT;
      else
         return EOF;
   }
   ret = _doprnt(fmt, args, f);
   return ferror(f) ? EOF : ret;
}
#endif


/* A replacement for strdup() that cuts down on malloc() overhead */
char *
savestr(const char *str)
{
   u_int size;
   char *p;
   static char *strptr = NULL;
   static u_int strsize = 0;

   size = strlen(str) + 1;
   if (size > strsize) {
      strsize = 1024;
      if (strsize < size)
         strsize = size;
      strptr = (char *) malloc(strsize);
      if (strptr == NULL)
         ArgusLog(LOG_ERR, "savestr: malloc %s", strerror(errno));
   }
   (void)strncpy(strptr, str, size);
   p = strptr;
   strptr += size;
   strsize -= size;
   return (p);
}



/*
 * Copy arg vector into a new argus_strbuffer, concatenating arguments with spaces.
 */
char *
copy_argv(argv)
char **argv;
{
   char **p;
   int len = 0;
   char *argus_strbuf;
   char *src, *dst;

   p = argv;
   if (*p == 0)
      return 0;

   while (*p)
      len += strlen(*p++) + 1;

   argus_strbuf = (char *) malloc (len);

   p = argv;
   dst = argus_strbuf;
   while ((src = *p++) != NULL) {
      while ((*dst++ = *src++) != '\0')
         ;
      dst[-1] = ' ';
   }
   dst[-1] = '\0';

   return argus_strbuf;
}


/*
 * Left justify 'addr' and return its resulting network mask.

u_int
net_mask(addr)
u_int *addr;
{
   u_int m = 0xffffffff;

   if (*addr)
      while ((*addr & 0xff000000) == 0)
         *addr <<= 8, m <<= 8;

   return m;
}
 */

u_int
ipaddrtonetmask(addr)
u_int addr;
{
   if (IN_CLASSA (addr)) return IN_CLASSA_NET;
   if (IN_CLASSB (addr)) return IN_CLASSB_NET;
   if (IN_CLASSC (addr)) return IN_CLASSC_NET;
   if (IN_CLASSD (addr)) return 0xFFFFFFFF;
   else return 0;
}


u_int
getnetnumber(addr)
u_int addr;
{
   if (IN_CLASSA (addr)) return (addr >> 24 );
   if (IN_CLASSB (addr)) return (addr >> 16 );
   if (IN_CLASSC (addr)) return (addr >>  8 );
   if (IN_CLASSD (addr)) return (addr >>  0 );
   else return 0;
}


#ifndef ArgusAddrtoName
#define ArgusAddrtoName
#endif

#include <sys/socket.h>
#include <signal.h>
#include <netdb.h>

#include <argus_namedb.h>
#include <argus_ethernames.h>

static SIGRET nohostname(int);
#ifdef ETHER_SERVICE
struct ether_addr;

#if defined(HAVE_ETHER_HOSTTON) && !defined(__OpenBSD__)
extern int ether_ntohost(char *, const struct ether_addr *);
#else
#if defined(HAVE_SOLARIS)
/*
extern int ether_ntohost(char *, struct ether_addr *);
extern int ether_hostton(char *, struct ether_addr *);
*/
#endif
#endif
#endif

struct enamemem enametable[HASHNAMESIZE];
struct enamemem nsaptable[HASHNAMESIZE];
struct enamemem bytestringtable[HASHNAMESIZE];

struct protoidmem protoidtable[HASHNAMESIZE];

/*
 * A faster replacement for inet_ntoa().
 */
char *
intoa(u_int addr)
{
   char *cp;
   u_int byte;
   int n;
   static char buf[sizeof(".xxx.xxx.xxx.xxx")];
/*
   addr = htonl(addr);
*/
   cp = &buf[sizeof buf];
   *--cp = '\0';

   n = 4;
   do {
      byte = addr & 0xff;
      *--cp = byte % 10 + '0';
      byte /= 10;
      if (byte > 0) {
         *--cp = byte % 10 + '0';
         byte /= 10;
         if (byte > 0)
            *--cp = byte + '0';
      }
      *--cp = '.';
      addr >>= 8;
   } while (--n > 0);

   return cp + 1;
}

static u_int f_netmask;
static u_int f_localnet;
u_int netmask;

/*
 * "ArgusGetName" is written in this atrocious way to make sure we don't
 * wait at all trying to get hostnames from any facility.
 */

#define ARGUS_PENDING	1
#include <setjmp.h>

jmp_buf getname_env;

static SIGRET
nohostname(int signo)
{
   longjmp(getname_env, 1);
}

#if defined(ARGUS_THREADS)
void * ArgusDNSProcess (void *);

void *
ArgusDNSProcess (void *arg)
{
   struct timespec tsbuf = {1, 0}, *ts = &tsbuf;
   sigset_t blocked_signals;

   sigfillset(&blocked_signals);
   pthread_sigmask(SIG_BLOCK, &blocked_signals, NULL);

#ifdef ARGUSDEBUG
   ArgusDebug (2, "ArgusDNSProcess() starting");
#endif

   while (!(ArgusParser->RaParseDone)) {
      if (ArgusParser->ArgusNameList == NULL) {
         nanosleep(ts, NULL);

      } else {
         struct timespec ts = {0, 250000000};
         while (!ArgusListEmpty(ArgusParser->ArgusNameList)) {
            struct ArgusListObjectStruct *list = ArgusParser->ArgusNameList->start;
            if (list != NULL) {
               u_int addr = list->list_val;
               static struct hnamemem *p;      /* static for longjmp() */
               struct hostent *hp;
               int found = 0;
   
               ArgusPopFrontList(ArgusParser->ArgusNameList, ARGUS_LOCK);
               ArgusFree(list);
   
               p = &hnametable[addr % (HASHNAMESIZE-1)];
               for (; p->nxt; p = p->nxt) {
                  if (p->addr == addr) {
                     found++;
                     break;
                  }
               }
   
               if (found && (p->name == NULL)) {
                  addr = htonl(addr);
#ifdef ARGUSDEBUG
                  ArgusDebug (2, "ArgusDNSProcess() query %s pending requests %d", p->nname, ArgusParser->ArgusNameList->count);
#endif
                  hp = gethostbyaddr((char *)&addr, 4, AF_INET);
                  if (hp) {
                     p->name = savestr(hp->h_name);
#ifdef ARGUSDEBUG
                     ArgusDebug (2, "ArgusDNSProcess() query %s returned %s", p->nname, p->name);
#endif
                  } else {
                     switch (h_errno) {
                        case TRY_AGAIN:
                           break;

                        case HOST_NOT_FOUND:
                        case NO_RECOVERY:
                        case NO_DATA:
                           p->name = (char *)-1;
                           break;
                     }
#ifdef ARGUSDEBUG
                     ArgusDebug (2, "ArgusDNSProcess() query %s not resolved", p->nname);
#endif
                  }

                  p->status = 0;
               }
            }
         }

         nanosleep(&ts, NULL);
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (2, "ArgusDNSProcess() done!");
#endif

   return (NULL);
}
#endif


/*
 * Return a name for the IP address pointed to by ap.  This address
 * is assumed to be in network byte order.
 */
char *
ArgusGetName(struct ArgusParserStruct *parser, u_char *ap)
{
   static struct hnamemem *p;      /* static for longjmp() */
   struct hostent *hp;
   int found = 0;
   u_int addr;

#ifndef TCPDUMP_ALIGN
   addr = *(const u_int *)ap;
#else
   /*
    * Deal with alignment.
    */
   switch ((int)ap & 3) {

   case 0:
      addr = *(u_int *)ap;
      break;

   case 2:
      addr = ((u_int)*(u_short *)ap << 16) |
         (u_int)*(u_short *)(ap + 2);
      break;

   default:
      addr = ((u_int)ap[3] << 24) |
         ((u_int)ap[2] << 16) |
         ((u_int)ap[1] << 8) |
         (u_int)ap[0];
      break;
   }
#endif
   p = &hnametable[addr % (HASHNAMESIZE-1)];
   for (; p->nxt; p = p->nxt) {
      if (p->addr == addr) {
         found++;
         break;
      }
   }
   if (!found) {
      p->addr = addr;
      addr = htonl(addr);
      p->nname = savestr(inet_ntoa(*(struct in_addr *)&addr));
      addr = ntohl(addr);
      p->nxt = (struct hnamemem *)calloc(1, sizeof (*p));
   }

   /*
    * Only print names when:
    *   (1) -n was not given.
    *   (3) The host portion is not 0 (i.e., a network address).
    *   (4) The host portion is not broadcast.
    */

   if (!(parser->nflag)) {
      if ((addr & f_netmask) == f_localnet) {
         if ((addr &~ netmask) != 0) {
            if ((addr | netmask) != 0xffffffff) {
#if defined(ARGUS_THREADS)
               if (ArgusParser->NonBlockingDNS) {
                  if (ArgusParser->ArgusNameList == NULL) {
                     pthread_attr_t attrbuf, *attr = &attrbuf;

                     pthread_attr_init(attr);
                     pthread_attr_setdetachstate(attr, PTHREAD_CREATE_JOINABLE);

                     if (getuid() == 0)
                        pthread_attr_setschedpolicy(attr, SCHED_RR);
                     else
                        attr = NULL;

                     ArgusParser->ArgusNameList = ArgusNewList();
                     if ((pthread_create(&ArgusParser->dns, attr, ArgusDNSProcess, NULL)) != 0)
                        ArgusLog (LOG_ERR, "ArgusGetName() pthread_create error %s\n", strerror(errno));
                  }

               } else
#endif
               {
                  if (p->name == NULL) {
#ifdef ARGUSDEBUG
                     ArgusDebug (2, "ArgusDNSProcess() query %s ", p->nname);
#endif
                     addr = htonl(addr);
                     hp = gethostbyaddr((char *)&addr, 4, AF_INET);
                     addr = ntohl(addr);

                     if (hp && (hp->h_name != NULL)) {
                        if (parser->domainonly) {
                           char *tptr, *dptr, *hptr, *hstr = strdup(hp->h_name);
                           int periods = 0;

                           hptr = hstr;
                           while ((tptr = strrchr(hptr, (int) '.')) != NULL) {
                              *tptr = ' ';
                              dptr = tptr + 1;
                              periods++;
                           }
                           
                           if (periods > 0) {
                              char *sptr = dptr;
                              while (*sptr != '\0') {
                                 if (*sptr == ' ')
                                    *sptr = '.';
                                 sptr++;
                              }
                              p->name = savestr(dptr);
                              free(hstr);
                           } else
                              p->name = savestr(hp->h_name);
                        } else
                           p->name = savestr(hp->h_name);
#ifdef ARGUSDEBUG
                        ArgusDebug (2, "ArgusDNSProcess() query %s returned %s", p->nname, p->name);
#endif
                     } else {
                        p->name = (char *)-1;
#ifdef ARGUSDEBUG
                        ArgusDebug (2, "ArgusDNSProcess() query %s not resolved", p->nname);
#endif
                     }
                     p->status = 0;
                  }
               }
   
               if (p->name) {
                  if (p->name != (char *) -1)
                     return (p->name);
               } else {
                  if (ArgusParser->NonBlockingDNS) {
                     if (p->status != ARGUS_PENDING) {
                        struct ArgusListObjectStruct *list;
                        if ((list = ArgusCalloc(1, sizeof(*list))) == NULL)
                           ArgusLog(LOG_ERR, "ArgusCalloc: error %s", strerror(errno));

                        list->list_val = addr;
                        ArgusPushBackList(ArgusParser->ArgusNameList, (struct ArgusListRecord *)list, ARGUS_LOCK);
                        p->status = ARGUS_PENDING;
                     }
                  }
               }
            }
         }
      }
   }

   if (parser->domainonly && !(parser->nflag))
      return ("not resolved");
   else
      return (p->nname);
}

/*
 * Return a null terminated string buffer, for the buf pointed to by ap, no longer than len.
 * The *ap is not assumed to be null terminated.
 *
 */

char *
ArgusGetString(struct ArgusParserStruct *parser, u_char *ap, int len)
{
   char *retn  = NULL;

   if ((ap != NULL) && (len > 0)) {
      char buf[MAXSTRLEN];
      int slen = strlen((const char *)ap);

      slen = (slen > len) ? len : slen;
      bcopy(ap, buf, slen);
      buf[slen] = 0;
      if (strlen(buf) > 0) 
         retn = strdup(buf);
   }

   return (retn);
}


#include <sys/socket.h>
#include <arpa/inet.h>

char *
ArgusGetV6Name(struct ArgusParserStruct *parser, u_char *ap)
{
   struct hostent *hp;
   struct in6_addr addr;
   char ntop_buf[INET6_ADDRSTRLEN];
   struct h6namemem *p;      /* static for longjmp() */
   const char *cp;

   memcpy(&addr, ap, sizeof(addr));

   p = &h6nametable[*(unsigned short *)&addr.s6_addr[14] & (HASHNAMESIZE-1)];
   for (; p->nxt; p = p->nxt) {
      if (memcmp(&p->addr, &addr, sizeof(addr)) == 0)
         return (p->name);
   }
   p->addr = addr;
   p->nxt = (struct h6namemem *)calloc(1, sizeof (*p));

   /*
    * Only print names when:
    *   (1) -n was not given.
    *   (2) Address is foreign and -f was given.  If -f was not
    *       present, f_netmask and f_local are 0 and the second
    *       test will succeed.
    *   (3) The host portion is not 0 (i.e., a network address).
    *   (4) The host portion is not broadcast.
    */
   if (!(parser->nflag)) {
      if (!setjmp(getname_env)) {
         (void)signal(SIGALRM, nohostname);
         (void)alarm(5);
         hp = gethostbyaddr((char *)&addr, sizeof(addr), AF_INET6);
         (void)alarm(0);
         if (hp) {
            if ((p->name = savestr(hp->h_name)) != NULL)
               return (p->name);
         }
      }
   }

   if ((cp = inet_ntop(AF_INET6, (const void *) &addr, ntop_buf, sizeof(ntop_buf))) != NULL)
      p->name = strdup(cp);

   return (p->name);
}

static char hex[] = "0123456789abcdef";


/* Find the hash node that corresponds the ether address 'ep'. */

static inline struct enamemem *
lookup_emem(const u_char *ep)
{
   u_int i, j, k;
   struct enamemem *tp;

   k = (ep[0] << 8) | ep[1];
   j = (ep[2] << 8) | ep[3];
   i = (ep[4] << 8) | ep[5];

   tp = &enametable[(i ^ j) % (HASHNAMESIZE-1)];
   while (tp->e_nxt)
      if (tp->e_addr0 == i &&
          tp->e_addr1 == j &&
          tp->e_addr2 == k)
         return tp;
      else
         tp = tp->e_nxt;
   tp->e_addr0 = i;
   tp->e_addr1 = j;
   tp->e_addr2 = k;
   tp->e_nxt = (struct enamemem *)calloc(1, sizeof(*tp));

   return tp;
}

/*
 * Find the hash node that corresponds to the bytestring 'bs'
 * with length 'nlen'
 */

static inline struct enamemem *
lookup_bytestring(register const u_char *bs, const unsigned int nlen)
{
   struct enamemem *tp;
   register u_int i, j, k;

   if (nlen >= 6) {
      k = (bs[0] << 8) | bs[1];
      j = (bs[2] << 8) | bs[3];
      i = (bs[4] << 8) | bs[5];
   } else if (nlen >= 4) {
      k = (bs[0] << 8) | bs[1];
      j = (bs[2] << 8) | bs[3];
      i = 0;
   } else
      i = j = k = 0;

   tp = &bytestringtable[(i ^ j) & (HASHNAMESIZE-1)];
   while (tp->e_nxt)
      if (tp->e_addr0 == i && tp->e_addr1 == j && tp->e_addr2 == k &&
            memcmp((const char *)bs, (const char *)(tp->e_bs), nlen) == 0)
         return tp;
      else
         tp = tp->e_nxt;

   tp->e_addr0 = i;
   tp->e_addr1 = j;
   tp->e_addr2 = k;

   tp->e_bs = (u_char *) calloc(1, nlen + 1);
   memcpy(tp->e_bs, bs, nlen);
   tp->e_nxt = (struct enamemem *)calloc(1, sizeof(*tp));

   return tp;
}


/* Find the hash node that corresponds the NSAP 'nsap'. */

static inline struct enamemem *
lookup_nsap(const u_char *nsap)
{
   u_int i, j, k;
   int nlen = *nsap;
   struct enamemem *tp;
   const u_char *ensap = nsap + nlen - 6;

   if (nlen > 6) {
      k = (ensap[0] << 8) | ensap[1];
      j = (ensap[2] << 8) | ensap[3];
      i = (ensap[4] << 8) | ensap[5];
   }
   else
      i = j = k = 0;

   tp = &nsaptable[(i ^ j) % (HASHNAMESIZE-1)];
   while (tp->e_nxt)
      if (tp->e_addr0 == i &&
          tp->e_addr1 == j &&
          tp->e_addr2 == k &&
          tp->e_nsap[0] == nlen &&
          bcmp((char *)&(nsap[1]),
         (char *)&(tp->e_nsap[1]), nlen) == 0)
         return tp;
      else
         tp = tp->e_nxt;
   tp->e_addr0 = i;
   tp->e_addr1 = j;
   tp->e_addr2 = k;
   tp->e_nsap = (u_char *) calloc(1, nlen + 1);
   bcopy(nsap, tp->e_nsap, nlen + 1);
   tp->e_nxt = (struct enamemem *)calloc(1, sizeof(*tp));

   return tp;
}

/* Find the hash node that corresponds the protoid 'pi'. */

static inline struct protoidmem *
lookup_protoid(const u_char *pi)
{
   u_int i, j;
   struct protoidmem *tp = NULL;

   /* 5 octets won't be aligned */
   i = (((pi[0] << 8) + pi[1]) << 8) + pi[2];
   j =   (pi[3] << 8) + pi[4];
   /* XXX should be endian-insensitive, but do big-endian testing  XXX */

   tp = &protoidtable[(i ^ j) % (HASHNAMESIZE-1)];
   if (tp->p_nxt != NULL) {
      while (tp->p_nxt)
         if (tp->p_oui == i && tp->p_proto == j)
            return tp;
         else
            tp = tp->p_nxt;
   }
   tp->p_oui = i;
   tp->p_proto = j;
   tp->p_nxt = (struct protoidmem *)calloc(1, sizeof(*tp));

   return tp;
}

char *
etheraddr_string(struct ArgusParserStruct *parser, u_char *ep)
{
   char *retn = NULL;
   struct enamemem *tp;
   u_char *lp = ep;
   char *cp, *oui;
   u_int i, j;

   tp = lookup_emem(ep);
   if (tp->e_name) {
      if (parser->ArgusPrintEthernetVendors)
         return (tp->e_ouiname);
      else
         return (tp->e_name);
   }
#if defined(HAVE_ETHER_HOSTTON) && !defined(linux) && !defined(CYGWIN)
   if (!parser->nflag) {
      char buf[128];
      if (ether_ntohost(buf, (struct ether_addr *)ep) == 0) {
         tp->e_name = savestr(buf);
         return (tp->e_name);
      }
   }
#endif
   tp->e_name = cp = (char *)calloc(1, sizeof("00:00:00:00:00:00"));

   j = *lp >> 4;
   *cp++ = hex[j];
   *cp++ = hex[*lp++ & 0xf];
   for (i = 5; (int)--i >= 0;) {
      *cp++ = ':';
      j = *lp >> 4;
      *cp++ = hex[j];
      *cp++ = hex[*lp++ & 0xf];
   }
   *cp = '\0';

   oui = etheraddr_oui(parser, ep);
   if (oui != NULL) {
      char vendor[16];
      sprintf (vendor, "%*.*s", 8, 8, oui);

      if (tp->e_oui == NULL)
         tp->e_oui = oui;

      if (tp->e_ouiname != NULL)
         free(tp->e_ouiname);

      tp->e_ouiname = strdup(tp->e_name);
      bcopy(vendor, tp->e_ouiname, 8);
      tp->e_ouiname[8] = '_';
   }

   retn =  (parser->ArgusPrintEthernetVendors) ? tp->e_ouiname : tp->e_name;
   return (retn);
}

char *
linkaddr_string(struct ArgusParserStruct *parser, const unsigned char *ep, unsigned int len)
{
   register u_int i;
   register char *cp; 
   register struct enamemem *tp;

   if (len == 6)   /* XXX not totally correct... */
      return etheraddr_string(parser, (u_char *) ep);

   tp = lookup_bytestring(ep, len);
   if (tp->e_name)
      return (tp->e_name);

   tp->e_name = cp = (char *)malloc(len*3);
   *cp++ = hex[*ep >> 4];
   *cp++ = hex[*ep++ & 0xf];
   for (i = len-1; i > 0 ; --i) {
      *cp++ = ':';
      *cp++ = hex[*ep >> 4];
      *cp++ = hex[*ep++ & 0xf];
   }
   *cp = '\0';
   return (tp->e_name);
}

char *
etheraddr_oui(struct ArgusParserStruct *parser, u_char *ep)
{
   int i, addr = 0;
   struct evendmem *p = NULL;
   int found = 0;
   struct enamemem *tp;

   tp = lookup_emem(ep);
   if (tp->e_oui != NULL)
      return(tp->e_oui);

   if (tp->e_name == NULL)
      etheraddr_string(parser, ep);

   for (i = 0; i < 3; i++)
      addr |= ep[i] << (8 * (2 - i));

   p = &ethervendor[addr % (HASHNAMESIZE-1)];
   for (; p->nxt; p = p->nxt) {
      if (p->addr == addr) {
         found++;
         break;
      }
   }

   tp->e_oui = p->name;
   return (tp->e_oui);
}


#define ARGUS_MAXEPROTODB   0x10000

struct ArgusEtherTypeStruct *argus_eproto_db[ARGUS_MAXEPROTODB];

char *
ArgusEtherProtoString(struct ArgusParserStruct *parser, u_short port)
{
   struct ArgusEtherTypeStruct *p;
   char *retn = NULL, *cp = NULL;

   if ((p = argus_eproto_db[port]) != NULL) {
      retn =  p->tag;
   } else {
      if ((p = (struct ArgusEtherTypeStruct *) calloc (1, sizeof(*p))) != NULL) {
         if (parser->nflag < 2) 
            p->tag = "unknown";
         else {
           p->tag = cp = (char *)malloc(sizeof("000000"));
           sprintf (cp, "%d", port);
         }
      
         p->range = cp;
   
         argus_eproto_db[port] = p;
         retn = p->tag;
      }
   }

   return (retn);
}

char *
protoid_string(const u_char *pi)
{
   u_int i, j;
   char *cp;
   struct protoidmem *tp;

   tp = lookup_protoid(pi);
   if (tp->p_name)
      return tp->p_name;

   tp->p_name = cp = (char *)malloc(sizeof("00:00:00:00:00"));

   if ((j = *pi >> 4) != 0)
      *cp++ = hex[j];
   *cp++ = hex[*pi++ & 0xf];
   for (i = 4; (int)--i >= 0;) {
      *cp++ = ':';
      if ((j = *pi >> 4) != 0)
         *cp++ = hex[j];
      *cp++ = hex[*pi++ & 0xf];
   }
   *cp = '\0';
   return (tp->p_name);
}

char *
llcsap_string(u_char sap)
{
   char *cp;
   struct hnamemem *tp;
   u_int i = sap;

   if (sap != '\0') {
      for (tp = &llcsaptable[i % (HASHNAMESIZE-1)]; tp->nxt; tp = tp->nxt)
         if (tp->addr == i)
            return (tp->name);

      tp->name = cp = (char *)malloc(sizeof("00000"));
      tp->addr = i;
      tp->nxt = (struct hnamemem *)calloc(1, sizeof (*tp));

      *cp++ = '0';
      *cp++ = 'x';
      *cp++ = hex[sap >> 4 & 0xf];
      *cp++ = hex[sap & 0xf];
      *cp++ = '\0';
      return (tp->name);
   } else
      return (" ");
}

#define ISONSAP_MAX_LENGTH 20
char *
isonsap_string(const u_char *nsap, int nsap_length)
{       
   register u_int nsap_idx;
   register char *cp;
   register struct enamemem *tp;

   if (nsap_length < 1 || nsap_length > ISONSAP_MAX_LENGTH)
      return ("isonsap_string: illegal length");

   tp = lookup_nsap(nsap);
   if (tp->e_name)
      return tp->e_name;

   tp->e_name = cp = (char *)malloc(sizeof("xx.xxxx.xxxx.xxxx.xxxx.xxxx.xxxx.xxxx.xxxx.xxxx.xx"));

   for (nsap_idx = 0; nsap_idx < nsap_length; nsap_idx++) {
      *cp++ = hex[*nsap >> 4];
      *cp++ = hex[*nsap++ & 0xf];
      if (((nsap_idx & 1) == 0) && (nsap_idx + 1 < nsap_length)) {
         *cp++ = '.';
      }
   }
   *cp = '\0';
   return (tp->e_name); 
}

char *
tcpport_string(arg_uint16 port)
{
   struct hnamemem *tp;
   u_int i = port;

   if (port) {
      for (tp = &tporttable[i % (HASHNAMESIZE-1)]; tp->nxt; tp = tp->nxt)
         if (tp->addr == i)
            return (tp->name);

      tp->name = (char *)malloc(sizeof("00000"));
      tp->addr = i;
      tp->nxt = (struct hnamemem *)calloc(1, sizeof (*tp));

      (void)sprintf (tp->name, "%d", i);
      return (tp->name);
   } else
      return ("*");
}

char *
udpport_string(u_short port)
{
   struct hnamemem *tp;
   u_int i = port;

   if (port) {
      for (tp = &uporttable[i % (HASHNAMESIZE-1)]; tp->nxt; tp = tp->nxt)
         if (tp->addr == i)
            return (tp->name);

      tp->name = (char *)calloc(1, sizeof("000000"));
      tp->addr = i;
      tp->nxt = (struct hnamemem *)calloc(1, sizeof(*tp));

      (void)sprintf (tp->name, "%d", i);

      return (tp->name);
   } else
      return ("*");
}

void
ArgusInitServarray(struct ArgusParserStruct *parser)
{
#if !defined(CYGWIN)
   struct servent *sv = NULL;
   struct hnamemem *table = NULL;
   int i = 0;

   if (parser->nflag > 1)
      return;

   setservent(1);

   while ((sv = getservent()) != NULL) {
      int port = ntohs(sv->s_port);
      i = port % (HASHNAMESIZE-1);
      if (strcmp(sv->s_proto, "tcp") == 0)
         table = &tporttable[i];
      else if (strcmp(sv->s_proto, "udp") == 0)
         table = &uporttable[i];
      else
         continue;

      if (table) {
         while (table->name)
            table = table->nxt;
         if (parser->nflag > 1) {
            char buf[32];

            (void)sprintf (buf, "%d", port);
            table->name = savestr(buf);
         } else
            table->name = savestr(sv->s_name);
         table->addr = port;
         table->nxt = (struct hnamemem *)calloc(1, sizeof(*table));
      }
   }
   endservent();
#endif
}


void
ArgusInitEprotoarray(void)
{
   struct ArgusEtherTypeStruct *p = argus_ethertype_names;

   bzero ((char *)argus_eproto_db, sizeof (argus_eproto_db));

   while (p->range != NULL) {
      int i, start, end;
      char *ptr;
      
      start = atoi(p->range);

      if ((ptr = strchr(p->range, '-')) != NULL)
         end = atoi(ptr + 1);
      else
         end = start;

      for (i = start; i < (end + 1); i++)
         argus_eproto_db[i] = p;

      p++;
   }
}


/*
 * SNAP proto IDs with org code 0:0:0 are actually encapsulated Ethernet
 * types.
 */
void
ArgusInitProtoidarray(void)
{
   struct ArgusEtherTypeStruct *p;
   struct protoidmem *tp;
   u_char protoid[5];
   int i;

   bzero ((char *)protoidtable, sizeof (protoidtable));
   bzero (protoid, sizeof(protoid));

   for (i = 0; i < ARGUS_MAXEPROTODB; i++) {
      if ((p = argus_eproto_db[i]) != NULL) {
         protoid[3] = i;
         tp = lookup_protoid(protoid);
         tp->p_name = p->tag;
      }
   }
}

static struct etherlist {
   u_char addr[6];
   char *name;
} etherlist[] = {
   {{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }, "Broadcast" },
   {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, NULL }
};

/*
 * Initialize the ethers hash table.  We take two different approaches
 * depending on whether or not the system provides the ethers name
 * service.  If it does, we just wire in a few names at startup,
 * and etheraddr_string() fills in the table on demand.  If it doesn't,
 * then we suck in the entire /etc/ethers file at startup.  The idea
 * is that parsing the local file will be fast, but spinning through
 * all the ethers entries via NIS & next_etherent might be very slow.
 *
 * XXX argus_next_etherent doesn't belong in the pcap interface, but
 * since the pcap module already does name-to-address translation,
 * it's already does most of the work for the ethernet address-to-name
 * translation, so we just argus_next_etherent as a convenience.
 */
void
ArgusInitEtherarray(void)
{
   int i, found = 0;

   if (ArgusParser != NULL) {
      for (i = 0; !found && (i < ARGUS_MAX_PRINT_ALG); i++) {
         struct ArgusPrintFieldStruct *pfield;
         if ((pfield = ArgusParser->RaPrintAlgorithmList[i]) != NULL) {
            if ((pfield->print == ArgusPrintSrcMacAddress) || (pfield->print == ArgusPrintDstMacAddress))
               found++;
         } else
            break;
      }
   }

   if (found) {
      struct etherlist *el;
      struct enamemem *tp;
#ifndef ETHER_SERVICE
      struct argus_etherent *ep;
      FILE *fp;

      /* Suck in entire ethers file */
      fp = fopen(PCAP_ETHERS_FILE, "r");
      if (fp != NULL) {
         while ((ep = argus_next_etherent(fp)) != NULL) {
            tp = lookup_emem(ep->addr);
            tp->e_name = savestr(ep->name);
         }
         (void)fclose(fp);
      }
#endif
      /* Hardwire some ethernet names */
      for (el = etherlist; el->name != NULL; ++el) {
#if defined(ETHER_SERVICE) && !defined(linux) && !defined(CYGWIN)
      /* Use yp/nis version of name if available */
         char wrk[256];
         if (ether_ntohost(wrk, (struct ether_addr *)el->addr) == 0) {
            tp = lookup_emem(el->addr);
            tp->e_name = savestr(wrk);
         }
#else
         /* install if not already present */
         tp = lookup_emem(el->addr);
         if (tp->e_name == NULL)
            tp->e_name = el->name;
#endif
      }
      if (ArgusParser->ArgusEthernetVendorFile != NULL)
         ArgusParserWiresharkManufFile(ArgusParser, ArgusParser->ArgusEthernetVendorFile);
   }
}

void
ArgusInitLlcsaparray(void)
{
   int i;
   struct hnamemem *table;

   for (i = 0; llcsap_db[i].s != NULL; i++) {
      table = &llcsaptable[llcsap_db[i].v];
      while (table->name)
         table = table->nxt;
      table->name = llcsap_db[i].s;
      table->addr = llcsap_db[i].v;
      table->nxt = (struct hnamemem *)calloc(1, sizeof(*table));
   }
}

char *argus_dscodes[0x100];
void ArgusInitDSCodepointarray(void);
struct ArgusDSCodePointStruct *ArgusSelectDSCodesTable(struct ArgusParserStruct *);

struct ArgusDSCodePointStruct *
ArgusSelectDSCodesTable(struct ArgusParserStruct *parser)
{
   struct ArgusDSCodePointStruct *retn = NULL;

   switch (parser->ArgusDSCodePoints) {
      case ARGUS_IANA_DSCODES: retn = argus_dscodepoints; break;
      case ARGUS_DISA_DSCODES: retn = argus_disa_dscodepoints; break;
   }
   return (retn);
}

void
ArgusInitDSCodepointarray()
{
   struct ArgusDSCodePointStruct *argus_dsctable = argus_dscodepoints;
   int i;

   bzero (&argus_dscodes, sizeof(argus_dscodes));

   if ((argus_dsctable = ArgusSelectDSCodesTable(ArgusParser)) != NULL) {
      for (i = 0; argus_dsctable[i].label != NULL; i++)
         argus_dscodes[(int)argus_dsctable[i].code] = argus_dsctable[i].label;
   }
}

/*
 * Initialize the address to name translation machinery.  We map all
 * non-local IP addresses to numeric addresses if fflag is true (i.e.,
 * to prevent blocking on the nameserver).  localnet is the IP address
 * of the local network.  mask is its subnet mask.
 */

void ArgusInitAddrtoname(struct ArgusParserStruct *, u_int, u_int);

void
ArgusInitAddrtoname(struct ArgusParserStruct *parser, u_int localnet, u_int mask)
{
   netmask = mask;
   if (parser->fflag) {
      f_localnet = localnet;
      f_netmask = mask;
   }

   if (parser->nflag > 2)
      return;

   ArgusInitEtherarray();
   ArgusInitServarray(parser);
   ArgusInitEprotoarray();
   ArgusInitLlcsaparray();

   ArgusInitProtoidarray();
   ArgusInitDSCodepointarray();

#ifdef ARGUSDEBUG
   ArgusDebug (1, "ArgusInitAddrtoname (0x%x, 0x%x, 0x%x)\n", parser, localnet, mask);
#endif
}


#ifndef __GNUC__
#define inline
#endif

/*
 * Convert a port name to its port and protocol numbers.
 * We assume only TCP or UDP.
 * Return 0 upon failure.
 */
int
argus_nametoport(char *name, int *port, int *proto)
{
   struct protoent *pp = NULL;
   struct servent *sp = NULL;
   char *pname = NULL, *other;

#ifdef ARGUSDEBUG
   ArgusDebug (8, "argus_nametoport (%s, .., ..) starting\n", name);
#endif

   if ((proto != NULL) && (*proto != -1)) {
#ifdef ARGUSDEBUG
      ArgusDebug (8, "argus_nametoport (%s, .., %d) calling getprotobynumber\n", name, *proto);
#endif
      if ((pp = getprotobynumber(*proto)) != NULL) {
         pname = pp->p_name;
      } else
         return 0;
   }

   if (name != NULL) {
#ifdef ARGUSDEBUG
      ArgusDebug (8, "argus_nametoport: calling getservbyname(%s, %s)\n", name, pname);
#endif
      sp = getservbyname(name, pname);

#ifdef ARGUSDEBUG
      ArgusDebug (8, "argus_nametoport: getservbyname() returned 0x%x\n", sp);
#endif
   }

   if (sp != NULL) {
#ifdef ARGUSDEBUG
      ArgusDebug (8, "argus_nametoport: sp is 0x%x\n", sp);
#endif
      *port = ntohs(sp->s_port);

#ifdef ARGUSDEBUG
      ArgusDebug (8, "argus_nametoport (%s, .., ..) calling argus_nametoproto(%s)\n", sp->s_proto);
#endif

      *proto = argus_nametoproto(sp->s_proto);
      /*
       * We need to check /etc/services for ambiguous entries.
       * If we find the ambiguous entry, and it has the
       * same port number, change the proto to PROTO_UNDEF
       * so both TCP and UDP will be checked.
       */
      if (*proto == IPPROTO_TCP)
         other = "udp";
      else
         other = "tcp";

      sp = getservbyname(name, other);
      if (sp != 0) {
         if (*port != ntohs(sp->s_port))
            return 0;
         *proto = PROTO_UNDEF;
      }

#ifdef ARGUSDEBUG
      ArgusDebug (8, "argus_nametoport (%s, %d, %d)\n", name, *port, *proto);
#endif
      return 1;
   }

#if defined(ultrix) || defined(__osf__)
   /* Special hack in case NFS isn't in /etc/services */
   if (strcmp(name, "nfs") == 0) {
      *port = 2049;
      *proto = PROTO_UNDEF;
      return 1;
   }
#endif

#ifdef ARGUSDEBUG
   ArgusDebug (8, "argus_nametoport (%s, %d, %d)\n", name, *port, *proto);
#endif

   return 0;
}

int
argus_nametoproto(char *str)
{
   struct protoent *p;

   p = getprotobyname(str);
   if (p != 0)
      return p->p_proto;
   else
      return PROTO_UNDEF;
}


int
argus_nametoeproto(char *s)
{
   struct ArgusEtherTypeStruct *p = argus_ethertype_names;

   while (p->tag != 0) {
      if (strcmp(p->tag, s) == 0) {
         return atoi(p->range);
      }
      p += 1;
   }

   return PROTO_UNDEF;
}

u_int
__argus_atoin(char *s, u_int *addr)
{
   u_int n;
   int len;

   *addr = 0;
   len = 0;
   while (1) {
      n = 0;
      while (*s && *s != '.')
         n = n * 10 + *s++ - '0';
      *addr <<= 8;
      *addr |= n & 0xff;
      len += 8;
      if (*s == '\0') {
         *addr = *addr;
         return len;
      }
      ++s;
   }
   /* NOTREACHED */
}

u_int
__argus_atodn(char *s)
{
#define AREASHIFT 10
#define AREAMASK 0176000
#define NODEMASK 01777

   u_int addr = 0;
   u_int node, area;

   if (sscanf((char *)s, "%d.%d", (int *) &area, (int *) &node) != 2)
      ArgusLog (LOG_ERR,"malformed decnet address '%s'", s);

   addr = (area << AREASHIFT) & AREAMASK;
   addr |= (node & NODEMASK);

   return(addr);
}

/*
 * Convert 's' which has the form "xx:xx:xx:xx:xx:xx" into a new
 * ethernet address.  Assumes 's' is well formed.
 */

extern int xdtoi(int);

u_char *
argus_ether_aton(char *s)
{
   register u_char *ep, *e;
   register u_int d;

   e = ep = (u_char *)malloc(6);

   while (*s) {
      if (*s == ':')
         s += 1;
      d = xdtoi(*s++);
      if (isxdigit((int)*s)) {
         d <<= 4;
         d |= xdtoi(*s++);
      }
      *ep++ = d;
   }

   return (e);
}

#if !defined(ETHER_SERVICE) || defined(linux)  || defined(CYGWIN)
/* Roll our own */

u_char *
argus_ether_hostton(char *name)
{
   register struct argus_etherent *ep;
   register u_char *ap;
   static FILE *fp = NULL;
   static int init = 0;

   if (!init) {
      fp = fopen(PCAP_ETHERS_FILE, "r");
      ++init;
      if (fp == NULL)
         return (NULL);
   } else if (fp == NULL)
      return (NULL);
   else
      rewind(fp);
   
   while ((ep = argus_next_etherent(fp)) != NULL) {
      if (strcmp(ep->name, name) == 0) {
         ap = (u_char *)malloc(6);
         if (ap != NULL) {
            memcpy(ap, ep->addr, 6);
            return (ap);
         }
         break;
      }
   }
   return (NULL);
}
#else

#if defined(HAVE_ETHER_HOSTTON) && !defined(__APPLE_CC__) && !defined(__APPLE__)
extern int ether_hostton(const char *, struct ether_addr *);
#endif

/* Use the os supplied routines */
u_char *
argus_ether_hostton(char *name)
{
   register u_char *ap;
   u_char a[6];

   ap = NULL;
   if (ether_hostton((char*)name, (struct ether_addr *)a) == 0) {
      ap = (u_char *)malloc(6);
      if (ap != NULL)
         memcpy(ap, a, 6);
   }
   return (ap);
}
#endif

u_short
__argus_nametodnaddr(char *name)
{
#ifndef   DECNETLIB
   return(0);
#else
   struct nodeent *getnodebyname();
   struct nodeent *nep;
   u_short res = 0;

   if ((nep = getnodebyname(name)) != NULL)
      memcpy((char *)&res, (char *)nep->n_addr, sizeof(u_short));

   return(res);
#endif
}



#include <stdarg.h>

void
ArgusPrintTime(struct ArgusParserStruct *parser, char *buf, struct timeval *tvp)
{
   char timeFormatBuf[128], *tstr = timeFormatBuf;
   char timeZoneBuf[32], *ptr;
   struct tm tmbuf, *tm = &tmbuf;
   time_t tsec = tvp->tv_sec;
 
   bzero (timeZoneBuf, sizeof(timeZoneBuf));
   bzero (timeFormatBuf, sizeof(timeFormatBuf));

   if ((tm = localtime_r (&tsec, &tmbuf)) == NULL)
      return;

   if (parser->uflag) {
      sprintf (tstr, "%u", (int) tvp->tv_sec);
      if (parser->pflag) {
         ptr = &tstr[strlen(tstr)];
         sprintf (ptr, ".%06u", (int) tvp->tv_usec);
         ptr[parser->pflag + 1] = '\0';
      }
      sprintf (buf, "%s", tstr);
      return;
   }

   strncpy(timeFormatBuf, parser->RaTimeFormat, 128);

   for (ptr=tstr; *ptr; ptr++) {
      if (*ptr != '%') {
         buf[strlen(buf)] = *ptr;
      } else {
         switch (*++ptr) {
            case 'f': {
               if (parser->pflag) {
                  char *p;
                  int i;

                  while (isspace((int)buf[strlen(buf) - 1]))
                     buf[strlen(buf) - 1] = '\0';
                  p = &buf[strlen(buf)];
                  sprintf (p, "%06u", (int) tvp->tv_usec);
                  for (i = parser->pflag; i < 6; i++)
                     p[i] = '\0';
               } else {
                  if ((buf[strlen(buf) - 1] == '.'))
                     buf[strlen(buf) - 1] = '\0';
               }
               break;
            }

            case '%': {
               buf[strlen(buf)] = '%';
               break;
            }

            case 'E':
            case 'O': {
               char sbuf[8];
               sprintf (sbuf, "%%%.2s", ptr++);
               strftime (&buf[strlen(buf)], 64, sbuf, tm);
               break;
            }

            case 'z': {
               if (parser->ArgusPrintXml) {
                  char sbuf[16];
                  int len, i;
                  bzero (sbuf, 16);
                  if ((strftime ((char *) sbuf, 16, "%z", tm)) == 0)
                     ArgusLog (LOG_ERR, "ArgusPrintTime: strftime() error\n");
                  if (strstr(sbuf, "0000")) {
                     sprintf (sbuf, "Z");
                  } else {
                     if ((len = strlen(sbuf)) > 0) {
                        for (i = 0; i < 2; i++)
                           sbuf[len - i] = sbuf[len - (i + 1)];
                        sbuf[len - 2] = ':';
                     }
                  }
                  sprintf(&buf[strlen(buf)], "%s", sbuf);
                  break;
               }
               /* Fall through to default if %z and not parser->ArgusPrintXml */
            }
            default: {
               char sbuf[8];
               sprintf (sbuf, "%%%c", *ptr);
               strftime (&buf[strlen(buf)], 64, sbuf, tm);
               break;
            }
         }
      }
   }
/*
   if (tvp->tv_sec == 0) {
      int len = strlen(buf);
      sprintf (buf, "%*.*s", len, len, " ");
   }
*/
}

void ArgusPrintCountryCode (struct ArgusParserStruct *, struct ArgusRecordStruct *, unsigned int *, int, int, char *);
extern struct RaAddressStruct *RaFindAddress (struct ArgusParserStruct *, struct RaAddressStruct *, struct RaAddressStruct *, int);

void
ArgusPrintCountryCode (struct ArgusParserStruct *parser, struct ArgusRecordStruct *argus, unsigned int *addr, int type, int len, char *buf)
{
   struct ArgusLabelerStruct *labeler;
   struct RaAddressStruct *raddr;

   if ((labeler = parser->ArgusLabeler) == NULL) {
      parser->ArgusLabeler = ArgusNewLabeler(parser, ARGUS_LABELER_COCODE);
      labeler = parser->ArgusLabeler;
   } else {
      if (labeler->ArgusAddrTree == NULL) {
         if (parser->ArgusDelegatedIPFile) {
            if (!(RaReadAddressConfig (parser, parser->ArgusLabeler, parser->ArgusDelegatedIPFile) > 0))
               ArgusLog (LOG_ERR, "ArgusNewLabeler: RaReadAddressConfig error");
         }
      }
   }

   if (labeler->ArgusAddrTree != NULL) {
      switch (type) {
         case ARGUS_TYPE_IPV4: {
            struct RaAddressStruct **ArgusAddrTree = labeler->ArgusAddrTree;
            struct RaAddressStruct node;
            bzero ((char *)&node, sizeof(node));

            node.addr.type = AF_INET;
            node.addr.len = 4;
            node.addr.addr[0] = *addr;
            node.addr.masklen = 32;

            if ((raddr = RaFindAddress (parser, ArgusAddrTree[AF_INET], &node, ARGUS_LONGEST_MATCH)) != NULL) {
               if (raddr->label != NULL)
                  snprintf (buf, 3, "%s", raddr->label);
               else
                  snprintf (buf, 3, "  ");
            }
            break;
         }

         case ARGUS_TYPE_IPV6:
            break;
      }

   } else
      snprintf (buf, 2, "  ");

#ifdef ARGUSDEBUG
   ArgusDebug (5, "ArgusPrintCountryCode (0x%x, 0x%x, 0x%x, %d, %d, 0x%x) returning\n", parser, argus, addr, type, len, buf);
#endif
}


void
ArgusPrintSrcCountryCode (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   int objlen;
   char ccbuf[4];
   struct ArgusFlow *flow;
   void *addr = NULL;
   int type = 0;

   bzero(ccbuf, sizeof(ccbuf));

   if (argus->hdr.type & ARGUS_MAR) {
      if (parser->ArgusPrintXml) {
      } else 
         sprintf (buf, "%*.*s ", len, len, " ");
 
   } else {
      struct ArgusCountryCodeStruct *cocode = (void *)argus->dsrs[ARGUS_COCODE_INDEX];

      if (cocode != NULL) {
         bcopy((char *)&cocode->src, ccbuf, 2);
      } else
      if (((flow = (void *)argus->dsrs[ARGUS_FLOW_INDEX]) != NULL)) {
         switch (flow->hdr.subtype & 0x3F) {

            case ARGUS_FLOW_CLASSIC5TUPLE: 
            case ARGUS_FLOW_LAYER_3_MATRIX: {
               switch (type = (flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                  case ARGUS_TYPE_IPV4:
                     addr = &flow->ip_flow.ip_src;
                     objlen = 4;
                     break;
                  case ARGUS_TYPE_IPV6:
                     addr = &flow->ipv6_flow.ip_src;
                     objlen = 16;
                     break;

                  case ARGUS_TYPE_RARP:
                     type = ARGUS_TYPE_ETHER;
                     addr = &flow->lrarp_flow.tareaddr;
                     objlen = 6;
                     break;
                  case ARGUS_TYPE_ARP:
                     type = ARGUS_TYPE_IPV4;
                     addr = &flow->larp_flow.arp_spa;
                     objlen = 4;
                     break;

                  case ARGUS_TYPE_ETHER:
                     addr = &flow->mac_flow.mac_union.ether.ehdr.ether_shost;
                     objlen = 6;
                     break;
               }
               break;
            }

            case ARGUS_FLOW_ARP: {
               switch (type = (flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                  case ARGUS_TYPE_RARP:
                     type = ARGUS_TYPE_ETHER;
                     addr = &flow->rarp_flow.dhaddr;
                     objlen = 6;
                     break;

                  case ARGUS_TYPE_ARP:
                     type = ARGUS_TYPE_IPV4;
                     addr = &flow->arp_flow.arp_spa;
                     objlen = 4;
                     break;
               }
               break;
            }

            default:
               break;
         }

         ArgusPrintCountryCode (parser, argus, addr, type, len, ccbuf);
      }

      if (parser->ArgusPrintXml) {
         sprintf (buf, " SrcCoCode = \"%s\"", ccbuf);
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH)
            len = strlen(ccbuf);

         if (len != 0) {
            if (len < strlen(ccbuf))
               sprintf (buf, "%*.*s* ", len-1, len-1, ccbuf);
            else
               sprintf (buf, "%*.*s ", len, len, ccbuf);
         } else
            sprintf (buf, "%s ", ccbuf);
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcCountryCode (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintDstCountryCode (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   int objlen;
   char ccbuf[4];
   struct ArgusFlow *flow;
   void *addr = NULL;
   int type = 0;

   bzero(ccbuf, sizeof(ccbuf));
   if (argus->hdr.type & ARGUS_MAR) {
      if (parser->ArgusPrintXml) {
      } else 
         sprintf (buf, "%*.*s ", len, len, " ");
 
   } else {
      struct ArgusCountryCodeStruct *cocode = (void *)argus->dsrs[ARGUS_COCODE_INDEX];

      if (cocode != NULL) {
         bcopy((char *)&cocode->dst, ccbuf, 2);
      } else
      if (((flow = (void *)argus->dsrs[ARGUS_FLOW_INDEX]) != NULL)) {
         switch (flow->hdr.subtype & 0x3F) {

            case ARGUS_FLOW_CLASSIC5TUPLE: 
            case ARGUS_FLOW_LAYER_3_MATRIX: {
               switch (type = (flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                  case ARGUS_TYPE_IPV4:
                     addr = &flow->ip_flow.ip_dst;
                     objlen = 4;
                     break;
                  case ARGUS_TYPE_IPV6:
                     addr = &flow->ipv6_flow.ip_dst;
                     objlen = 16;
                     break;

                  case ARGUS_TYPE_RARP:
                     type = ARGUS_TYPE_ETHER;
                     addr = &flow->lrarp_flow.tareaddr;
                     objlen = 6;
                     break;
                  case ARGUS_TYPE_ARP:
                     type = ARGUS_TYPE_IPV4;
                     addr = &flow->larp_flow.arp_tpa;
                     objlen = 4;
                     break;

                  case ARGUS_TYPE_ETHER:
                     addr = &flow->mac_flow.mac_union.ether.ehdr.ether_dhost;
                     objlen = 6;
                     break;
               }
               break;
            }

            case ARGUS_FLOW_ARP: {
               switch (type = (flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                  case ARGUS_TYPE_RARP:
                     type = ARGUS_TYPE_ETHER;
                     addr = &flow->rarp_flow.dhaddr;
                     objlen = 6;
                     break;

                  case ARGUS_TYPE_ARP:
                     type = ARGUS_TYPE_IPV4;
                     addr = &flow->arp_flow.arp_tpa;
                     objlen = 4;
                     break;
               }
               break;
            }

            default:
               break;
         }

         ArgusPrintCountryCode (parser, argus, addr, type, len, ccbuf);
      }

      if (parser->ArgusPrintXml) {
         sprintf (buf, " DstCoCode = \"%s\"", ccbuf);
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH)
            len = strlen(ccbuf);

         if (len != 0) {
            if (len < strlen(ccbuf))
               sprintf (buf, "%*.*s* ", len-1, len-1, ccbuf);
            else
               sprintf (buf, "%*.*s ", len, len, ccbuf);
         } else
            sprintf (buf, "%s ", ccbuf);
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstCountryCode (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintSrcAsn (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char asnbuf[16];

   bzero(asnbuf, sizeof(asnbuf));
   if (argus->hdr.type & ARGUS_MAR) {
      if (parser->ArgusPrintXml) {
      } else 
         sprintf (buf, "%*.*s ", len, len, " ");
 
   } else {
      struct ArgusAsnStruct *asn = (void *)argus->dsrs[ARGUS_ASN_INDEX];

      if ((asn != NULL) && (asn->src_as != 0)) {
         if (asn->src_as > 65535) {
            unsigned short sasn[2];
            sasn[0] = (asn->src_as & 0x0000FFFF);
            sasn[1] = (asn->src_as >> 16);
            sprintf(asnbuf, "%d.%d", sasn[1], sasn[0]);
         } else
            sprintf(asnbuf, "%d", asn->src_as);
      } else
         sprintf(asnbuf, "  ");

      if (parser->ArgusPrintXml) {
         sprintf (buf, " SrcASNum = \"%s\"", asnbuf);
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH)
            len = strlen(asnbuf);

         if (len != 0) {
            if (len < strlen(asnbuf))
               sprintf (buf, "%*.*s* ", len-1, len-1, asnbuf);
            else
               sprintf (buf, "%*.*s ", len, len, asnbuf);
         } else
            sprintf (buf, "%s ", asnbuf);
      }
   }
#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintSrcAsn (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintDstAsn (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char asnbuf[16];

   bzero(asnbuf, sizeof(asnbuf));
   if (argus->hdr.type & ARGUS_MAR) {
      if (parser->ArgusPrintXml) {
      } else
         sprintf (buf, "%*.*s ", len, len, " ");

   } else {
      struct ArgusAsnStruct *asn = (void *)argus->dsrs[ARGUS_ASN_INDEX];

      if ((asn != NULL) && (asn->dst_as != 0)) {
         if (asn->dst_as > 65535) {
            unsigned short sasn[2];
            sasn[0] = (asn->dst_as & 0x0000FFFF);
            sasn[1] = (asn->dst_as >> 16);
            sprintf(asnbuf, "%d.%d", sasn[1], sasn[0]);
         } else
            sprintf(asnbuf, "%d", asn->dst_as);
      } else
         sprintf(asnbuf, "  ");

      if (parser->ArgusPrintXml) {
         sprintf (buf, " DstASNum = \"%s\"", asnbuf);
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH)
            len = strlen(asnbuf);

         if (len != 0) {
            if (len < strlen(asnbuf))
               sprintf (buf, "%*.*s* ", len-1, len-1, asnbuf);
            else
               sprintf (buf, "%*.*s ", len, len, asnbuf);
         } else
            sprintf (buf, "%s ", asnbuf);
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintDstAsn (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintInodeAsn (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   char asnbuf[16];

   bzero(asnbuf, sizeof(asnbuf));
   if (argus->hdr.type & ARGUS_MAR) {
      if (parser->ArgusPrintXml) {
      } else
         sprintf (buf, "%*.*s ", len, len, " ");

   } else {
      struct ArgusAsnStruct *asn = (void *)argus->dsrs[ARGUS_ASN_INDEX];

      if (asn != NULL) {
         int alen = asn->hdr.argus_dsrvl8.len;
         if ((alen > 3) && (asn->inode_as != 0)) {
            if (asn->inode_as > 65535) {
               unsigned short sasn[2];
               sasn[0] = (asn->inode_as & 0x0000FFFF);
               sasn[1] = (asn->inode_as >> 16);
               sprintf(asnbuf, "%d.%d", sasn[1], sasn[0]);
            } else
               sprintf(asnbuf, "%d", asn->inode_as);
         }
      } else
         sprintf(asnbuf, "  ");

      if (parser->ArgusPrintXml) {
         sprintf (buf, " InodeASNum = \"%s\"", asnbuf);
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH)
            len = strlen(asnbuf);

         if (len != 0) {
            if (len < strlen(asnbuf))
               sprintf (buf, "%*.*s* ", len-1, len-1, asnbuf);
            else
               sprintf (buf, "%*.*s ", len, len, asnbuf);
         } else
            sprintf (buf, "%s ", asnbuf);
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintInodeAsn (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintIcmpId (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusFlow *flow;
   char idbuf[12];
   int type = 0;

   bzero(idbuf, sizeof(idbuf));
   if (argus->hdr.type & ARGUS_MAR) {
      if (parser->ArgusPrintXml) {
      } else 
         sprintf (buf, "%*.*s ", len, len, " ");
 
   } else {
      unsigned short id = 0;
      int found = 0;

      if (((flow = (void *)argus->dsrs[ARGUS_FLOW_INDEX]) != NULL)) {
         switch (flow->hdr.subtype & 0x3F) {
            case ARGUS_FLOW_CLASSIC5TUPLE: 
            case ARGUS_FLOW_LAYER_3_MATRIX: {
               switch (type = (flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                  case ARGUS_TYPE_IPV4:
                     if (flow->ip_flow.ip_p == IPPROTO_ICMP) {
                        id = flow->icmp_flow.id;
                        found++;
                     }
                     break;
                  case ARGUS_TYPE_IPV6:
                     if (flow->ipv6_flow.ip_p == IPPROTO_ICMPV6) {
                        id = flow->icmp6_flow.id;
                        found++;
                     }
                     break;
               }
               break;
            }
         }
      }

      if (found) {
         sprintf (idbuf, "%d", id);
      } else {
         sprintf (idbuf, " ");
      }

      if (parser->ArgusPrintXml) {
         sprintf (buf, " IcmpId = \"%s\"", idbuf);
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH)
            len = strlen(idbuf);

         if (len != 0) {
            if (len < strlen(idbuf))
               sprintf (buf, "%*.*s* ", len-1, len-1, idbuf);
            else
               sprintf (buf, "%*.*s ", len, len, idbuf);
         } else
            sprintf (buf, "%s ", idbuf);
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintIcmpId (%p, %p)", buf, argus);
#endif
}

void
ArgusPrintResponse (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusFlow *flow;
   char *resp = NULL; 

   if (argus->hdr.type & ARGUS_MAR) {
      if (parser->ArgusPrintXml) {
      } else
         sprintf (buf, "%*.*s ", len, len, " ");

   } else {
      if (((flow = (void *)argus->dsrs[ARGUS_FLOW_INDEX]) != NULL)) {
         switch (flow->hdr.subtype & 0x3F) {
            case ARGUS_FLOW_CLASSIC5TUPLE:
            case ARGUS_FLOW_LAYER_3_MATRIX: {
               int type;
               switch (type = (flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                  case ARGUS_TYPE_RARP: {
                     break;
                  }

                  case ARGUS_TYPE_ARP: {
                     break;
                  }
               }
               break;
            }

            case ARGUS_FLOW_ARP: {
               switch (flow->hdr.argus_dsrvl8.qual & 0x1F) {
                  case ARGUS_TYPE_RARP: {
                     break;
                  }
                  case ARGUS_TYPE_ARP: {
                     struct ArgusNetworkStruct *net = (struct ArgusNetworkStruct *)argus->dsrs[ARGUS_NETWORK_INDEX];
                     if (net != NULL) 
                        resp = etheraddr_string(parser, (unsigned char *)&net->net_union.arp.respaddr);
                     break;
                  }
               }
               break;
            }
         }
      }

      if (resp != NULL) {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH)
            len = strlen(resp);

         if (len != 0) {
            if (len < strlen(resp))
               sprintf (buf, "%*.*s* ", len-1, len-1, resp);
            else
               sprintf (buf, "%*.*s ", len, len, resp);
         } else
            sprintf (buf, "%s ", resp);
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintResponse (%p, %p)", buf, argus);
#endif
}


void
ArgusPrintLabel (struct ArgusParserStruct *parser, char *buf, struct ArgusRecordStruct *argus, int len)
{
   struct ArgusLabelStruct *label;
   char *labelbuf = "";

   if (argus->hdr.type & ARGUS_MAR) {
      if (parser->ArgusPrintXml) {
      } else 
         sprintf (buf, "%*.*s ", len, len, " ");
 
   } else {
      if (((label = (void *)argus->dsrs[ARGUS_LABEL_INDEX]) != NULL))
         labelbuf = label->l_un.label;

      if (parser->ArgusPrintXml) {
         sprintf (buf, " Label = \"%s\"", labelbuf);
      } else {
         if (parser->RaFieldWidth != RA_FIXED_WIDTH)
            len = strlen(labelbuf);

         if (len != 0) {
            if (len < strlen(labelbuf))
               sprintf (buf, "%*.*s* ", len-1, len-1, labelbuf);
            else
               sprintf (buf, "%*.*s ", len, len, labelbuf);
         } else
            sprintf (buf, "%s ", labelbuf);
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (10, "ArgusPrintLabel (%p, %p)", buf, argus);
#endif
}

static char ArgusStatusBuf[32];

char *
ArgusGetManStatus (struct ArgusParserStruct *parser, struct ArgusRecordStruct *argus)
{
   bzero (ArgusStatusBuf, 32);

   switch (argus->hdr.cause & 0xF0) {
      case ARGUS_START:         sprintf (ArgusStatusBuf, "STA"); break;
      case ARGUS_STATUS:        sprintf (ArgusStatusBuf, "CON"); break;
      case ARGUS_STOP:          sprintf (ArgusStatusBuf, "STP"); break;
      case ARGUS_SHUTDOWN:      sprintf (ArgusStatusBuf, "SHT"); break;
      case ARGUS_ERROR: {
         switch (argus->hdr.cause & 0x0F) {
            case ARGUS_ACCESSDENIED:  sprintf (ArgusStatusBuf, "ADN"); break;
            case ARGUS_MAXLISTENEXCD: sprintf (ArgusStatusBuf, "MAX"); break;
            default:                  sprintf (ArgusStatusBuf, "ERR"); break;
         }
      }
   }

   return(ArgusStatusBuf);
}


char *
ArgusGetTCPStatus (struct ArgusParserStruct *parser, struct ArgusRecordStruct *argus)
{
   struct ArgusMetricStruct *metric;
   struct ArgusNetworkStruct *net;
   unsigned int status = 0;
   unsigned char sflags, dflags;

   if ((net = (struct ArgusNetworkStruct *)argus->dsrs[ARGUS_NETWORK_INDEX]) != NULL) {
      switch (net->hdr.subtype) {
         case ARGUS_TCP_INIT:
         case ARGUS_TCP_STATUS:
         case ARGUS_TCP_PERF: {
            struct ArgusTCPObject *tcp = (struct ArgusTCPObject *)&net->net_union.tcp;
            status = tcp->status;
            sflags = tcp->src.flags;
            dflags = tcp->dst.flags;
            break;
         }
      }
   }
   
   *ArgusStatusBuf = '\0';

   if (parser->zflag || parser->Zflag) {
      if (parser->zflag) {
         if (status & ARGUS_SAW_SYN)         strncat (ArgusStatusBuf, "s", (32 - strlen(ArgusStatusBuf)));
         if (status & ARGUS_SAW_SYN_SENT)    strncat (ArgusStatusBuf, "S", (32 - strlen(ArgusStatusBuf)));
         if (status & ARGUS_CON_ESTABLISHED) strncat (ArgusStatusBuf, "E", (32 - strlen(ArgusStatusBuf)));
         if (status & ARGUS_FIN)             strncat (ArgusStatusBuf, "f", (32 - strlen(ArgusStatusBuf)));
         if (status & ARGUS_FIN_ACK)         strncat (ArgusStatusBuf, "F", (32 - strlen(ArgusStatusBuf)));
         if (status & ARGUS_NORMAL_CLOSE)    strncat (ArgusStatusBuf, "C", (32 - strlen(ArgusStatusBuf)));
         if (status & ARGUS_RESET)           strncat (ArgusStatusBuf, "R", (32 - strlen(ArgusStatusBuf)));

      } else {
         if (parser->Zflag) {
            char SrcTCPFlagsStr[16], DstTCPFlagsStr[16], tmp[16];
            int i, index;

            bzero(SrcTCPFlagsStr, sizeof(SrcTCPFlagsStr));
            bzero(DstTCPFlagsStr, sizeof(DstTCPFlagsStr));
            bzero(tmp, sizeof(tmp));

            for (i = 0, index = 1; i < 8; i++) {
               if (sflags & index) {
                  strncat (SrcTCPFlagsStr, ArgusTCPFlags[i], (16 - strlen(SrcTCPFlagsStr)));
               }
               if (dflags & index) {
                  strncat (DstTCPFlagsStr, ArgusTCPFlags[i], (16 - strlen(SrcTCPFlagsStr)));
               }
               index <<= 1;
            }

            if ((metric = (void *)argus->dsrs[ARGUS_METRIC_INDEX]) != NULL) {
               if ((strlen(SrcTCPFlagsStr) && (metric->src.pkts == 0)) ||
                   (strlen(DstTCPFlagsStr) && (metric->dst.pkts == 0))) {
                  bcopy(SrcTCPFlagsStr, tmp, sizeof(tmp));
                  bcopy(DstTCPFlagsStr, SrcTCPFlagsStr, sizeof(tmp));
                  bcopy(tmp, DstTCPFlagsStr, sizeof(tmp));
               }
            }

            switch (parser->Zflag) {
               case 'b':
                  sprintf (ArgusStatusBuf, "%s_%s", SrcTCPFlagsStr, DstTCPFlagsStr);
                  break;
               case 's':
                  sprintf (ArgusStatusBuf, "%s", SrcTCPFlagsStr);
                  break;
               case 'd':
                  sprintf (ArgusStatusBuf, "%s", DstTCPFlagsStr);
                  break;
            }
         }
      }

   } else {
      if (status) {
         if (status & ARGUS_RESET)             sprintf (ArgusStatusBuf, "RST"); else
         if (status & ARGUS_FIN)               sprintf (ArgusStatusBuf, "FIN"); else
         if (status & ARGUS_FIN_ACK)           sprintf (ArgusStatusBuf, "FIN"); else
         if (status & ARGUS_NORMAL_CLOSE)      sprintf (ArgusStatusBuf, "CLO"); else
         if (argus->hdr.cause & ARGUS_TIMEOUT) sprintf (ArgusStatusBuf, "TIM"); else
         if (status & ARGUS_CON_ESTABLISHED)   sprintf (ArgusStatusBuf, "CON"); else
         if (status & ARGUS_SAW_SYN_SENT)      sprintf (ArgusStatusBuf, "ACC"); else
         if (status & ARGUS_SAW_SYN)           sprintf (ArgusStatusBuf, "REQ"); else
                                               sprintf (ArgusStatusBuf, "CON");
      } else {
         if ((metric = (void *)argus->dsrs[ARGUS_METRIC_INDEX]) != NULL) {
            if (metric->src.pkts && metric->dst.pkts)
               sprintf (ArgusStatusBuf, "CON");
            else
               sprintf (ArgusStatusBuf, "INT");
         } else
            sprintf (ArgusStatusBuf, "INT");
      }
   }
   return (ArgusStatusBuf);
}

char *
ArgusGetIGMPStatus (struct ArgusParserStruct *parser, struct ArgusRecordStruct *argus)
{
   return (ArgusStatusBuf);
}

char *
ArgusGetICMPv6Status (struct ArgusParserStruct *parser, struct ArgusRecordStruct *argus)
{
   struct ArgusICMPv6Flow *icmp;
   struct ArgusFlow *flow;
   char *retn = "UNK";

   if ((flow = (struct ArgusFlow *)argus->dsrs[ARGUS_FLOW_INDEX]) != NULL) {
      icmp = (void *) &flow->icmp6_flow;
      if (icmp->type & ICMP6_INFOMSG_MASK) {
         switch (icmp->type) {
            case ICMP6_ECHO_REQUEST:
               retn = icmptypestr[8];
               break;
            case ICMP6_ECHO_REPLY:
               retn = icmptypestr[0];
               break;
            case ICMP6_MEMBERSHIP_QUERY:
               retn = icmptypestr[35];
               break;
            case ICMP6_MEMBERSHIP_REPORT:
               retn = icmptypestr[32];
               break;
            case ND_ROUTER_SOLICIT:
               retn = icmptypestr[41];
               break;
            case ND_ROUTER_ADVERT:
               retn = icmptypestr[42];
               break;
            case ND_NEIGHBOR_SOLICIT:
               retn = icmptypestr[43];
               break;
            case ND_NEIGHBOR_ADVERT:
               retn = icmptypestr[44];
               break;
            case ND_REDIRECT:
               retn = icmptypestr[5];
               break;
         }
      } else {
         switch (icmp->type) {
            case ICMP6_DST_UNREACH:
               retn = icmptypestr[3];
               break;
            case ICMP6_PACKET_TOO_BIG:
               retn = icmptypestr[45];
               break;
            case ICMP6_TIME_EXCEEDED:
               retn = icmptypestr[11];
               break;
            case ICMP6_PARAM_PROB:
               retn = icmptypestr[12];
               break;
         }
      }
   }

   return (retn);
}

char *
ArgusGetICMPStatus (struct ArgusParserStruct *parser, struct ArgusRecordStruct *argus)
{
   struct ArgusFlow *flow;
   char ArgusResponseString[32];
   char icmptype[32];

   bzero (ArgusResponseString, 32);
   bzero (icmptype, 32);

   if ((flow = (struct ArgusFlow *)argus->dsrs[ARGUS_FLOW_INDEX]) != NULL) {
      struct ArgusMetricStruct *metric = (void *)argus->dsrs[ARGUS_METRIC_INDEX];
      struct ArgusIcmpStruct *icmp = (void *)argus->dsrs[ARGUS_ICMP_INDEX];
      struct ArgusICMPFlow *icmpFlow = &flow->icmp_flow;

      unsigned char ra_icmp_type = 0, ra_icmp_code = 0;
      unsigned short ra_icmp_data = 0;
      unsigned int  ra_src_addr = 0, ra_dst_addr = 0, ra_gw_addr = 0;
      char *typestr = "UNK";
      
      if (icmp) {
         ra_src_addr  = icmp->isrcaddr;
         ra_dst_addr  = icmp->idstaddr;
         ra_gw_addr   = icmp->igwaddr;
         ra_icmp_type = icmp->icmp_type;
         ra_icmp_code = icmp->icmp_code;
      } else {
         ra_icmp_type = icmpFlow->type;
         ra_icmp_code = icmpFlow->code;
      }

      ra_icmp_data = icmpFlow->id;
      
      if (ra_icmp_type < (unsigned char) (ICMP_MAXTYPE + 1)) {
         if (icmptypestr[ra_icmp_type] != NULL)
            typestr = icmptypestr[ra_icmp_type];
      }
      strncpy (icmptype, typestr, 32);

      switch (ra_icmp_type) {
         case ICMP_UNREACH:
            switch (ra_icmp_code) {
               case ICMP_UNREACH_NET:
                  strncat (icmptype, "N", (32 - strlen(icmptype)));
                  if (ra_dst_addr) {
                     u_long addr = ra_dst_addr;
                     sprintf (ArgusResponseString, "net %s", ArgusGetName (ArgusParser, (unsigned char *)&addr));
                  }
                  break;
               case ICMP_UNREACH_HOST:
                  strncat (icmptype, "H", (32 - strlen(icmptype)));

                  if (ra_dst_addr)
                     sprintf (ArgusResponseString, "host %s", ArgusGetName (ArgusParser, (unsigned char *)&ra_dst_addr));
                  break;

               case ICMP_UNREACH_PROTOCOL:
                  strncat (icmptype, "O", (32 - strlen(icmptype)));
                  if (ra_icmp_data && (ra_icmp_data < IPPROTOSTR))
                     sprintf (ArgusResponseString,"proto %s",
                        ip_proto_string[ra_icmp_data]);
                  break;

               case ICMP_UNREACH_PORT: {
                  int index = icmpFlow->tp_p;
                  strncat (icmptype, "P", (32 - strlen(icmptype)));

                  if ((ra_icmp_data && ((index < IPPROTOSTR)) && (index > 0))) {
                     sprintf (ArgusResponseString, "%s_port     %d", ip_proto_string[index], ra_icmp_data);

                  } else if (ra_icmp_data)
                     sprintf (ArgusResponseString, "port     %d", ra_icmp_data);
                  break;
               }
               case ICMP_UNREACH_NEEDFRAG:
                  strncat (icmptype, "F", (32 - strlen(icmptype))); break;
               case ICMP_UNREACH_SRCFAIL:
                  strncat (icmptype, "S", (32 - strlen(icmptype))); break;

#ifndef ICMP_UNREACH_NET_UNKNOWN
#define ICMP_UNREACH_NET_UNKNOWN        6
#endif
               case ICMP_UNREACH_NET_UNKNOWN:
                  strncat (icmptype, "NU", (32 - strlen(icmptype)));
                  sprintf (ArgusResponseString, "dst_net unknown"); break;
               
#ifndef ICMP_UNREACH_HOST_UNKNOWN
#define ICMP_UNREACH_HOST_UNKNOWN       7
#endif
               case ICMP_UNREACH_HOST_UNKNOWN:
                  strncat (icmptype, "HU", (32 - strlen(icmptype)));
                  sprintf (ArgusResponseString, "dst_host unknown"); break;

#ifndef ICMP_UNREACH_ISOLATED
#define ICMP_UNREACH_ISOLATED           8
#endif
               case ICMP_UNREACH_ISOLATED:
                  strncat (icmptype, "ISO", (32 - strlen(icmptype)));
                  sprintf (ArgusResponseString, "src_host isolated"); break;

#ifndef ICMP_UNREACH_NET_PROHIB
#define ICMP_UNREACH_NET_PROHIB         9
#endif
               case ICMP_UNREACH_NET_PROHIB:
                  strncat (icmptype, "NPRO", (32 - strlen(icmptype)));
                  sprintf (ArgusResponseString, "admin_net prohib"); break;

#ifndef ICMP_UNREACH_HOST_PROHIB
#define ICMP_UNREACH_HOST_PROHIB        10
#endif
               case ICMP_UNREACH_HOST_PROHIB:
                  strncat (icmptype, "HPRO", (32 - strlen(icmptype)));
                  sprintf (ArgusResponseString, "admin_host prohib"); break;

#ifndef ICMP_UNREACH_TOSNET
#define ICMP_UNREACH_TOSNET             11
#endif
               case ICMP_UNREACH_TOSNET:
                  strncat (icmptype, "NTOS", (32 - strlen(icmptype)));
                  sprintf (ArgusResponseString, "tos_net prohib"); break;

#ifndef ICMP_UNREACH_TOSHOST
#define ICMP_UNREACH_TOSHOST            12
#endif
               case ICMP_UNREACH_TOSHOST:
                  strncat (icmptype, "HTOS", (32 - strlen(icmptype)));
                  sprintf (ArgusResponseString, "tos_host prohib"); break;
    
#ifndef ICMP_UNREACH_FILTER_PROHIB
#define ICMP_UNREACH_FILTER_PROHIB      13
#endif
               case ICMP_UNREACH_FILTER_PROHIB:
                  strncat (icmptype, "FIL", (32 - strlen(icmptype)));
                  sprintf (ArgusResponseString, "admin_filter prohib"); break;

#ifndef ICMP_UNREACH_HOST_PRECEDENCE
#define ICMP_UNREACH_HOST_PRECEDENCE    14
#endif
               case ICMP_UNREACH_HOST_PRECEDENCE:
                  strncat (icmptype, "PRE", (32 - strlen(icmptype)));
                  sprintf (ArgusResponseString, "precedence violation"); break;

#ifndef ICMP_UNREACH_PRECEDENCE_CUTOFF
#define ICMP_UNREACH_PRECEDENCE_CUTOFF  15
#endif
               case ICMP_UNREACH_PRECEDENCE_CUTOFF:
                  strncat (icmptype, "CUT", (32 - strlen(icmptype)));
                  sprintf (ArgusResponseString, "precedence cutoff"); break;

            }
            break;

         case ICMP_MASKREPLY:
            if (ra_src_addr)
               sprintf (ArgusResponseString, "mask 0x%08x", ra_src_addr);
            break;

         case ICMP_REDIRECT:
            switch (ra_icmp_code) {
            case ICMP_REDIRECT_NET:
               (void) sprintf (ArgusResponseString, "net %s",
                         ArgusGetName (ArgusParser, (unsigned char *)&ra_gw_addr));
               break;

            case ICMP_REDIRECT_HOST:
               (void) sprintf (ArgusResponseString, "host %s",
                         ArgusGetName (ArgusParser, (unsigned char *)&ra_gw_addr));
               break;

            case ICMP_REDIRECT_TOSNET:
               (void) sprintf (ArgusResponseString, "tosN %s",
                         ArgusGetName (ArgusParser, (unsigned char *)&ra_gw_addr));
               break;

            case ICMP_REDIRECT_TOSHOST:
               (void) sprintf (ArgusResponseString, "tosH %s",
                         ArgusGetName (ArgusParser, (unsigned char *)&ra_gw_addr));
               break;
            }
            break;

#ifndef ICMP_ROUTERADVERT
#define ICMP_ROUTERADVERT               9       
#endif
         case ICMP_ROUTERADVERT:
            sprintf (ArgusResponseString, "router advertisement"); break;

#ifndef ICMP_ROUTERSOLICIT
#define ICMP_ROUTERSOLICIT              10     
#endif
         case ICMP_ROUTERSOLICIT:
            sprintf (ArgusResponseString, "router solicitation"); break;


         case ICMP_ECHOREPLY:
         case ICMP_TSTAMPREPLY:
         case ICMP_IREQREPLY: {
            long long sbytes = 0, dbytes = 0;
            if (metric != NULL) {
               sbytes = metric->src.bytes;
               dbytes = metric->dst.bytes;
            }
#if defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__APPLE_CC__) || defined(__APPLE__) || defined(HAVE_SOLARIS)
            sprintf (ArgusResponseString, "%-6lld      %-6lld", sbytes, dbytes);
#else
            sprintf (ArgusResponseString, "%-6Ld      %-6Ld", sbytes, dbytes);
#endif
            break;
         }

         case ICMP_TIMXCEED:
               (void) sprintf (ArgusResponseString, "timexceed %s",
                         ra_icmp_code ? "reassembly" : "in-transit");
               break;

         case ICMP_PARAMPROB:
         case ICMP_SOURCEQUENCH:
         case ICMP_ECHO:
         case ICMP_TSTAMP:
         case ICMP_IREQ:
         case ICMP_MASKREQ:
         default: {
            long long sbytes = 0, dbytes = 0;
            if (metric != NULL) {
               sbytes = metric->src.bytes;
               dbytes = metric->dst.bytes;
            }
#if defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__APPLE_CC__) || defined(__APPLE__) || defined(HAVE_SOLARIS)
            sprintf (ArgusResponseString, "%-6lld      %-6lld", sbytes, dbytes);
#else
            sprintf (ArgusResponseString, "%-6Ld      %-6Ld", sbytes, dbytes);
#endif
            break;
         }
      }

      if (!(parser->Rflag)) {
         long long sbytes = 0, dbytes = 0;
         if (metric != NULL) {
            sbytes = metric->src.bytes;
            dbytes = metric->dst.bytes;
         }
#if defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__APPLE_CC__) || defined(__APPLE__) || defined(HAVE_SOLARIS)
            sprintf (ArgusResponseString, "%-6lld      %-6lld", sbytes, dbytes);
#else
            sprintf (ArgusResponseString, "%-6Ld      %-6Ld", sbytes, dbytes);
#endif
      }
   }

   strncpy (ArgusStatusBuf, icmptype, 32);
   return (ArgusStatusBuf);
}


char *
ArgusGetIPStatus (struct ArgusParserStruct *parser, struct ArgusRecordStruct *argus)
{
   struct ArgusMetricStruct *metric;

   if ((metric = (struct ArgusMetricStruct *)argus->dsrs[ARGUS_METRIC_INDEX]) != NULL) {
      if (metric->src.pkts && metric->dst.pkts)
         sprintf (ArgusStatusBuf, "CON");
      else {
         if ((metric->src.pkts) || (parser->RaMonMode)) {
            if (argus->hdr.cause & ARGUS_START)
               sprintf (ArgusStatusBuf, "INT");
            else
               sprintf (ArgusStatusBuf, "REQ");
         } else
            sprintf (ArgusStatusBuf, "RSP");
      }
   }
   return (ArgusStatusBuf);
}


#ifdef ARGUSDEBUG
void
ArgusDebug (int d, char *fmt, ...)
{
   va_list ap;
   char buf[MAXSTRLEN], *ptr;
   struct timeval tvp;

   if (ArgusParser && (d <= ArgusParser->debugflag)) {
      gettimeofday (&tvp, 0L);
      bzero(buf, MAXSTRLEN);
#if defined(ARGUS_THREADS)
      {
         pthread_t ptid;
         char pbuf[128];
         int i;

         bzero(pbuf, sizeof(pbuf));
         ptid = pthread_self();
         for (i = 0; i < sizeof(ptid); i++)
            snprintf (&pbuf[i*2], 3, "%02hhx", ((char *)&ptid)[i]);

         (void) snprintf (buf, MAXSTRLEN, "%s[%d.%s]: ", ArgusParser->ArgusProgramName, (int)getpid(), pbuf);
      }
#else
      (void) snprintf (buf, MAXSTRLEN, "%s[%d]: ", ArgusParser->ArgusProgramName, (int)getpid());
#endif
      ArgusPrintTime(ArgusParser, &buf[strlen(buf)], &tvp);
      ptr = &buf[strlen(buf)];
      *ptr++ = ' ';

#if defined(__STDC__)
      va_start(ap, fmt);
#else
      va_start(ap);
#endif

      (void) vsnprintf (ptr, (MAXSTRLEN - strlen(buf)), fmt, ap);
      va_end (ap);

      while (buf[strlen(buf) - 1] == '\n')
         buf[strlen(buf) - 1] = '\0';

      ptr = &buf[strlen(buf)];

      if (ArgusParser->RaCursesMode) {
#if defined(ARGUS_THREADS)
         pthread_mutex_lock(&ArgusParser->lock);
#endif
         snprintf (ArgusParser->RaDebugString, MAXSTRLEN, "%s\n", buf);
#if defined(ARGUS_THREADS)
         pthread_mutex_unlock(&ArgusParser->lock);
#endif
      } else {
         fprintf (stderr, "%s\n", buf);
      }
   }
}
#endif

#if !defined(HAVE_STRTOF)
float strtof (char *, char **);

float
strtof (char *str, char **ptr)
{
   double ipart = 0.0, fpart = 0.0, multi = 0.0;
   float retn = 0.0;
   char *dptr;
   int i;

   if ((dptr = strchr (str, '.')) != NULL) {
      int len = 0;
      *dptr++ = 0;
      len = strlen(dptr);
      i = atoi(dptr);
      multi = pow(10.0, len * 1.0);
      fpart = i * 1.0/multi;
   }

   ipart = atoi(str);

   retn = ipart + fpart;
   return(retn);
}
#endif


#if !defined(ntohll)
#if defined(_LITTLE_ENDIAN)
#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#include <argus/extract.h>
#define ntohll(x) EXTRACT_64BITS(&x)
#define htonll(x) EXTRACT_64BITS(&x)
#else
#include <byteswap.h>
#define ntohll(x) bswap_64(x)
#define htonll(x) bswap_64(x)
#endif
#else
#define ntohll(x) x
#define htonll(x) x
#endif
#endif


void
ArgusNtoH (struct ArgusRecord *argus)
{
#if defined(_LITTLE_ENDIAN)
   struct ArgusRecordHeader *hdr = &argus->hdr;
   struct ArgusDSRHeader *dsr = (struct ArgusDSRHeader *) (hdr + 1);

   hdr->len = ntohs(hdr->len);

   switch (argus->hdr.type & 0xF0) {
      case ARGUS_MAR: {
         if (argus->hdr.len == sizeof (*argus)/4) {
            argus->argus_mar.status            = ntohl(argus->argus_mar.status);
            argus->argus_mar.argusid           = ntohl(argus->argus_mar.argusid);
            argus->argus_mar.localnet          = ntohl(argus->argus_mar.localnet);
            argus->argus_mar.netmask           = ntohl(argus->argus_mar.netmask);
            argus->argus_mar.nextMrSequenceNum = ntohl(argus->argus_mar.nextMrSequenceNum);
            argus->argus_mar.startime.tv_sec   = ntohl(argus->argus_mar.startime.tv_sec);
            argus->argus_mar.startime.tv_usec  = ntohl(argus->argus_mar.startime.tv_usec);
            argus->argus_mar.now.tv_sec        = ntohl(argus->argus_mar.now.tv_sec);
            argus->argus_mar.now.tv_usec       = ntohl(argus->argus_mar.now.tv_usec);
            argus->argus_mar.reportInterval    = ntohs(argus->argus_mar.reportInterval);
            argus->argus_mar.argusMrInterval   = ntohs(argus->argus_mar.argusMrInterval);

            argus->argus_mar.pktsRcvd          = ntohll(argus->argus_mar.pktsRcvd);
            argus->argus_mar.bytesRcvd         = ntohll(argus->argus_mar.bytesRcvd);
            argus->argus_mar.drift             = ntohll(argus->argus_mar.drift);

            argus->argus_mar.records           = ntohl(argus->argus_mar.records);
            argus->argus_mar.flows             = ntohl(argus->argus_mar.flows);
            argus->argus_mar.dropped           = ntohl(argus->argus_mar.dropped);
            argus->argus_mar.queue             = ntohl(argus->argus_mar.queue);
            argus->argus_mar.output            = ntohl(argus->argus_mar.output);
            argus->argus_mar.clients           = ntohl(argus->argus_mar.clients);
            argus->argus_mar.bufs              = ntohl(argus->argus_mar.bufs);
            argus->argus_mar.bytes             = ntohl(argus->argus_mar.bytes);

            argus->argus_mar.suserlen          = ntohs(argus->argus_mar.suserlen);
            argus->argus_mar.duserlen          = ntohs(argus->argus_mar.duserlen);

            argus->argus_mar.thisid            = ntohl(argus->argus_mar.thisid);
            argus->argus_mar.record_len        = ntohl(argus->argus_mar.record_len);
         }
         break;
      }


      case ARGUS_EVENT:
      case ARGUS_NETFLOW:
      case ARGUS_FAR: {
         if (hdr->len > 1) {
            char *end = (char *)argus + (hdr->len * 4);
            int cnt;
            while ((char *) dsr < end) {
               cnt = (((dsr->type & ARGUS_IMMEDIATE_DATA) ? 1 :
                      ((dsr->subtype & ARGUS_LEN_16BITS)  ? ntohs(dsr->argus_dsrvl16.len) :
                                                                  dsr->argus_dsrvl8.len))) * 4;
               if (cnt == 0)
                  break;

               if (end < ((char *)dsr + cnt))
                  break;

               switch (dsr->type & 0x7F) {
                  case ARGUS_FLOW_DSR: {
                     struct ArgusFlow *flow = (struct ArgusFlow *) dsr;

                     switch (flow->hdr.subtype & 0x3F) {
                        case ARGUS_FLOW_CLASSIC5TUPLE: {
                           switch ((flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                              case ARGUS_TYPE_IPV4: {
                                 flow->ip_flow.ip_src = ntohl(flow->ip_flow.ip_src);
                                 flow->ip_flow.ip_dst = ntohl(flow->ip_flow.ip_dst);
                                 switch (flow->ip_flow.ip_p) {
                                    case IPPROTO_TCP:
                                    case IPPROTO_UDP:
                                       flow->ip_flow.sport = ntohs(flow->ip_flow.sport);
                                       flow->ip_flow.dport = ntohs(flow->ip_flow.dport);
                                       break;
                                    case IPPROTO_ESP:
                                       flow->esp_flow.spi = ntohl(flow->esp_flow.spi);
                                       break;
                                    case IPPROTO_IGMP:
                                       flow->igmp_flow.ip_id = ntohs(flow->igmp_flow.ip_id);
                                       break;
                                    case IPPROTO_ICMP:
                                       flow->icmp_flow.id    = ntohs(flow->icmp_flow.id);
                                       flow->icmp_flow.ip_id = ntohs(flow->icmp_flow.ip_id);
                                       break;
                                 }
                                 break; 
                              }

                              case ARGUS_TYPE_IPV6: {
                                 unsigned int *iptr = (unsigned int *)&flow->ipv6_flow;
                                 iptr[8] = ntohl(iptr[8]);
                                 switch (flow->ipv6_flow.ip_p) {
                                    case IPPROTO_TCP:
                                    case IPPROTO_UDP:
                                       flow->ipv6_flow.sport = ntohs(flow->ipv6_flow.sport);
                                       flow->ipv6_flow.dport = ntohs(flow->ipv6_flow.dport);
                                       break;
                                 }
                                 break; 
                              }

                              case ARGUS_TYPE_ETHER: {
                                 struct ArgusMacFlow *mac = (struct ArgusMacFlow *) &flow->mac_flow;
                                 mac->mac_union.ether.ehdr.ether_type = ntohs(mac->mac_union.ether.ehdr.ether_type);
                                 break;
                              }

                              case ARGUS_TYPE_RARP: {
                                 struct ArgusLegacyRarpFlow *rarp = (struct ArgusLegacyRarpFlow *) &flow->rarp_flow;
                                 rarp->arp_tpa = ntohl(rarp->arp_tpa);
                                 break;
                              }

                              case ARGUS_TYPE_ARP: {
                                 struct ArgusLegacyArpFlow *arp = (struct ArgusLegacyArpFlow *) &flow->flow_un;
                                 arp->arp_spa = ntohl(arp->arp_spa);
                                 arp->arp_tpa = ntohl(arp->arp_tpa);
                                 break;
                              }

                              case ARGUS_TYPE_ISIS: {
                                 struct ArgusIsisFlow *isis = (struct ArgusIsisFlow *) &flow->isis_flow;
                                 switch (isis->pdu_type = ntohl(isis->pdu_type)) {
                                    case L1_LAN_IIH:
                                    case L2_LAN_IIH:
                                       break;

                                    case L1_CSNP:
                                    case L2_CSNP:
                                       break;

                                    case L1_PSNP:
                                    case L2_PSNP:
                                       break;

                                    case L1_LSP:
                                    case L2_LSP:
                                       isis->isis_un.lsp.seqnum = ntohl(isis->isis_un.lsp.seqnum);
                                       break;
                                 }
                                 isis->chksum = ntohl(isis->chksum);
                                 break;
                              }
                           }
                           break; 
                        }

                        case ARGUS_FLOW_ARP: {
                           switch (flow->hdr.argus_dsrvl8.qual & 0x1F) {
                              case ARGUS_TYPE_RARP: {
                                 struct ArgusRarpFlow *rarp = (struct ArgusRarpFlow *) &flow->rarp_flow;
                                 rarp->hrd = ntohs(rarp->hrd);
                                 rarp->pro = ntohs(rarp->pro);
                                 rarp->op  = ntohs(rarp->op);
                                 if (rarp->pln == 4) {
                                    rarp->arp_tpa = ntohl(rarp->arp_tpa);
                                 }
                                 break; 
                              }
                              case ARGUS_TYPE_ARP: {
                                 struct ArgusArpFlow *arp = (struct ArgusArpFlow *) &flow->arp_flow;
                                 arp->hrd = ntohs(arp->hrd);
                                 arp->pro = ntohs(arp->pro);
                                 arp->op  = ntohs(arp->op);
                                 if (arp->pln == 4) {
                                    arp->arp_spa = ntohl(arp->arp_spa);
                                    arp->arp_tpa = ntohl(arp->arp_tpa);
                                 }
                                 break; 
                              }
                              default: {
                                 struct ArgusInterimArpFlow *arp = (void *) &flow->iarp_flow;
                                 arp->pro = ntohs(arp->pro);
                                 arp->arp_spa = ntohl(arp->arp_spa);
                                 arp->arp_tpa = ntohl(arp->arp_tpa);
                              }
                           }
                           break; 
                        }
                     }
                     break;
                  }

                  case ARGUS_ENCAPS_DSR: {
                     struct ArgusEncapsStruct *encaps = (struct ArgusEncapsStruct *) dsr;
                     encaps->src = ntohl(encaps->src);
                     encaps->dst = ntohl(encaps->dst);
                     break;
                  }

                  case ARGUS_ASN_DSR: {
                     struct ArgusAsnStruct *asn = (struct ArgusAsnStruct *) dsr;
                     asn->src_as = ntohl(asn->src_as);
                     asn->dst_as = ntohl(asn->dst_as);
                     if (cnt > 12)
                        asn->inode_as = ntohl(asn->inode_as);
                     break;
                  }

                  case ARGUS_IPATTR_DSR: {
                     struct ArgusIPAttrStruct *attr = (struct ArgusIPAttrStruct *) dsr;
                     unsigned int *dsrptr = (unsigned int *)(dsr + 1);

                     if (attr->hdr.argus_dsrvl8.qual & ARGUS_IPATTR_SRC) {
                        struct ArgusIPAttrObject *aobj = (struct ArgusIPAttrObject *) dsrptr;
                        aobj->ip_id = ntohs(aobj->ip_id);
                        dsrptr++;
                     }
                     if (attr->hdr.argus_dsrvl8.qual & ARGUS_IPATTR_SRC_OPTIONS) {
                        *dsrptr = ntohl(*dsrptr);
                        dsrptr++;
                     }
                     if (attr->hdr.argus_dsrvl8.qual & ARGUS_IPATTR_DST) {
                        struct ArgusIPAttrObject *aobj = (struct ArgusIPAttrObject *) dsrptr;
                        aobj->ip_id = ntohs(aobj->ip_id);
                        dsrptr++;
                     }
                     if (attr->hdr.argus_dsrvl8.qual & ARGUS_IPATTR_DST_OPTIONS) {
                        *dsrptr = ntohl(*dsrptr);
                        dsrptr++;
                     }
                     break;
                  }

                  case ARGUS_TRANSPORT_DSR: {
                     struct ArgusTransportStruct *trans = (struct ArgusTransportStruct *) dsr;

                     if (trans->hdr.subtype & ARGUS_SEQ)
                        trans->seqnum = ntohl(trans->seqnum);

                     if (trans->hdr.subtype & ARGUS_SRCID) {
                        switch (trans->hdr.argus_dsrvl8.qual) {
                           case ARGUS_TYPE_INT:
                              trans->srcid.a_un.value = ntohl(trans->srcid.a_un.value);
                              break;
                           case ARGUS_TYPE_IPV4:
                              trans->srcid.a_un.value = ntohl(trans->srcid.a_un.value);
                              break;

                           case ARGUS_TYPE_IPV6:
                           case ARGUS_TYPE_ETHER:
                           case ARGUS_TYPE_STRING:
                              break;
                        }
                     }
                     break;
                  }

                  case ARGUS_TIME_DSR: {
                     unsigned int i, *dtime = (unsigned int *) dsr;

                     for (i = 1; i < dsr->argus_dsrvl8.len; i++)
                        dtime[i] = ntohl(dtime[i]);
                     break;
                  }

                  case ARGUS_METER_DSR: {
                     if (dsr->subtype & ARGUS_METER_PKTS_BYTES) {
                        switch (dsr->argus_dsrvl8.qual & 0x0F) {
                           case ARGUS_SRCDST_BYTE:
                           case ARGUS_SRC_BYTE:
                           case ARGUS_DST_BYTE:
                              break;
                           case ARGUS_SRCDST_SHORT:
                              ((unsigned short *)(dsr + 1))[0] = ntohs(((unsigned short *)(dsr + 1))[0]);
                              ((unsigned short *)(dsr + 1))[1] = ntohs(((unsigned short *)(dsr + 1))[1]);
                              ((unsigned short *)(dsr + 1))[2] = ntohs(((unsigned short *)(dsr + 1))[2]);
                              ((unsigned short *)(dsr + 1))[3] = ntohs(((unsigned short *)(dsr + 1))[3]);
                              break;
                           case ARGUS_SRCDST_INT:
                              ((unsigned int *)(dsr + 1))[0] = ntohl(((unsigned int *)(dsr + 1))[0]);
                              ((unsigned int *)(dsr + 1))[1] = ntohl(((unsigned int *)(dsr + 1))[1]);
                              ((unsigned int *)(dsr + 1))[2] = ntohl(((unsigned int *)(dsr + 1))[2]);
                              ((unsigned int *)(dsr + 1))[3] = ntohl(((unsigned int *)(dsr + 1))[3]);
                              break;
                           case ARGUS_SRCDST_LONGLONG:
                              ((long long *)(dsr + 1))[0] = ntohll(((long long *)(dsr + 1))[0]);
                              ((long long *)(dsr + 1))[1] = ntohll(((long long *)(dsr + 1))[1]);
                              ((long long *)(dsr + 1))[2] = ntohll(((long long *)(dsr + 1))[2]);
                              ((long long *)(dsr + 1))[3] = ntohll(((long long *)(dsr + 1))[3]);
                              break;
                           case ARGUS_SRC_SHORT:
                           case ARGUS_DST_SHORT:
                              ((unsigned short *)(dsr + 1))[0] = ntohs(((unsigned short *)(dsr + 1))[0]);
                              ((unsigned short *)(dsr + 1))[1] = ntohs(((unsigned short *)(dsr + 1))[1]);
                              break;
                           case ARGUS_SRC_INT:
                           case ARGUS_DST_INT:
                              ((unsigned int *)(dsr + 1))[0] = ntohl(((unsigned int *)(dsr + 1))[0]);
                              ((unsigned int *)(dsr + 1))[1] = ntohl(((unsigned int *)(dsr + 1))[1]);
                              break;
                           case ARGUS_SRC_LONGLONG:
                           case ARGUS_DST_LONGLONG:
                              ((long long *)(dsr + 1))[0] = ntohll(((long long *)(dsr + 1))[0]);
                              ((long long *)(dsr + 1))[1] = ntohll(((long long *)(dsr + 1))[1]);
                              break;
                        }

                     } else
                     if (dsr->subtype & ARGUS_METER_PKTS_BYTES_APP) {
                        switch (dsr->argus_dsrvl8.qual & 0x0F) {
                           case ARGUS_SRCDST_BYTE:
                           case ARGUS_SRC_BYTE:
                           case ARGUS_DST_BYTE:
                              break;
                           case ARGUS_SRCDST_SHORT:
                              ((unsigned short *)(dsr + 1))[0] = ntohs(((unsigned short *)(dsr + 1))[0]);
                              ((unsigned short *)(dsr + 1))[1] = ntohs(((unsigned short *)(dsr + 1))[1]);
                              ((unsigned short *)(dsr + 1))[2] = ntohs(((unsigned short *)(dsr + 1))[2]);
                              ((unsigned short *)(dsr + 1))[3] = ntohs(((unsigned short *)(dsr + 1))[3]);
                              ((unsigned short *)(dsr + 1))[4] = ntohs(((unsigned short *)(dsr + 1))[4]);
                              ((unsigned short *)(dsr + 1))[5] = ntohs(((unsigned short *)(dsr + 1))[5]);
                              break;
                           case ARGUS_SRCDST_INT:
                              ((unsigned int *)(dsr + 1))[0] = ntohl(((unsigned int *)(dsr + 1))[0]);
                              ((unsigned int *)(dsr + 1))[1] = ntohl(((unsigned int *)(dsr + 1))[1]);
                              ((unsigned int *)(dsr + 1))[2] = ntohl(((unsigned int *)(dsr + 1))[2]);
                              ((unsigned int *)(dsr + 1))[3] = ntohl(((unsigned int *)(dsr + 1))[3]);
                              ((unsigned int *)(dsr + 1))[4] = ntohl(((unsigned int *)(dsr + 1))[4]);
                              ((unsigned int *)(dsr + 1))[5] = ntohl(((unsigned int *)(dsr + 1))[5]);
                              break;
                           case ARGUS_SRCDST_LONGLONG:
                              ((long long *)(dsr + 1))[0] = ntohll(((long long *)(dsr + 1))[0]);
                              ((long long *)(dsr + 1))[1] = ntohll(((long long *)(dsr + 1))[1]);
                              ((long long *)(dsr + 1))[2] = ntohll(((long long *)(dsr + 1))[2]);
                              ((long long *)(dsr + 1))[3] = ntohll(((long long *)(dsr + 1))[3]);
                              ((long long *)(dsr + 1))[4] = ntohll(((long long *)(dsr + 1))[4]);
                              ((long long *)(dsr + 1))[5] = ntohll(((long long *)(dsr + 1))[5]);
                              break;

                           case ARGUS_SRC_SHORT:
                           case ARGUS_DST_SHORT:
                              ((unsigned short *)(dsr + 1))[0] = ntohs(((unsigned short *)(dsr + 1))[0]);
                              ((unsigned short *)(dsr + 1))[1] = ntohs(((unsigned short *)(dsr + 1))[1]);
                              ((unsigned short *)(dsr + 1))[2] = ntohs(((unsigned short *)(dsr + 1))[2]);
                              break;
                           case ARGUS_SRC_INT:
                           case ARGUS_DST_INT:
                              ((unsigned int *)(dsr + 1))[0] = ntohl(((unsigned int *)(dsr + 1))[0]);
                              ((unsigned int *)(dsr + 1))[1] = ntohl(((unsigned int *)(dsr + 1))[1]);
                              ((unsigned int *)(dsr + 1))[2] = ntohl(((unsigned int *)(dsr + 1))[2]);
                              break;
                           case ARGUS_SRC_LONGLONG:
                           case ARGUS_DST_LONGLONG:
                              ((long long *)(dsr + 1))[0] = ntohll(((long long *)(dsr + 1))[0]);
                              ((long long *)(dsr + 1))[1] = ntohll(((long long *)(dsr + 1))[1]);
                              ((long long *)(dsr + 1))[2] = ntohll(((long long *)(dsr + 1))[2]);
                              break;
                        }
                     }
                     break;
                  }

                  case ARGUS_PSIZE_DSR: {
                     switch (dsr->argus_dsrvl8.qual & 0x0F) {
                        case ARGUS_SRCDST_SHORT:
                           ((unsigned short *)(dsr + 1))[0] = ntohs(((unsigned short *)(dsr + 1))[0]);
                           ((unsigned short *)(dsr + 1))[1] = ntohs(((unsigned short *)(dsr + 1))[1]);
                           ((unsigned short *)(dsr + 1))[2] = ntohs(((unsigned short *)(dsr + 1))[2]);
                           ((unsigned short *)(dsr + 1))[3] = ntohs(((unsigned short *)(dsr + 1))[3]);
                           break;
                           
                        case ARGUS_SRC_SHORT:
                        case ARGUS_DST_SHORT:
                           ((unsigned short *)(dsr + 1))[0] = ntohs(((unsigned short *)(dsr + 1))[0]);
                           ((unsigned short *)(dsr + 1))[1] = ntohs(((unsigned short *)(dsr + 1))[1]);
                           break; 
                           
                        case ARGUS_SRCDST_INT:
                           ((unsigned int *)(dsr + 1))[0] = ntohl(((unsigned int *)(dsr + 1))[0]);
                           ((unsigned int *)(dsr + 1))[1] = ntohl(((unsigned int *)(dsr + 1))[1]);
                           ((unsigned int *)(dsr + 1))[2] = ntohl(((unsigned int *)(dsr + 1))[2]);
                           ((unsigned int *)(dsr + 1))[3] = ntohl(((unsigned int *)(dsr + 1))[3]);
                           break;
                           
                        case ARGUS_SRC_INT:
                        case ARGUS_DST_INT:
                           ((unsigned int *)(dsr + 1))[0] = ntohl(((unsigned int *)(dsr + 1))[0]);
                           ((unsigned int *)(dsr + 1))[1] = ntohl(((unsigned int *)(dsr + 1))[1]);
                           break;
                     }     
                     break;
                  }

                  case ARGUS_NETWORK_DSR: {
                     struct ArgusNetworkStruct *net = (struct ArgusNetworkStruct *)dsr;
                     switch (net->hdr.subtype) {
                        case ARGUS_TCP_INIT: {
                           struct ArgusTCPInitStatus *tcp = (void *)&net->net_union.tcpinit;
                           tcp->status       = ntohl(tcp->status);
                           tcp->seqbase      = ntohl(tcp->seqbase);
                           tcp->options      = ntohl(tcp->options);
                           tcp->win          = ntohs(tcp->win);
                           break;
                        }
                        case ARGUS_TCP_STATUS: {
                           struct ArgusTCPStatus *tcp = (struct ArgusTCPStatus *)&net->net_union.tcpstatus;
                           tcp->status       = ntohl(tcp->status);
                           break;
                        }
                        case ARGUS_TCP_PERF: {
                           struct ArgusTCPObject *tcp = (struct ArgusTCPObject *)&net->net_union.tcp;
                           tcp->status       = ntohl(tcp->status);
                           tcp->state        = ntohl(tcp->state);
                           tcp->options      = ntohl(tcp->options);
                           tcp->synAckuSecs  = ntohl(tcp->synAckuSecs);
                           tcp->ackDatauSecs = ntohl(tcp->ackDatauSecs);

                           tcp->src.lasttime.tv_sec  = ntohl(tcp->src.lasttime.tv_sec);
                           tcp->src.lasttime.tv_usec = ntohl(tcp->src.lasttime.tv_usec);
                           tcp->src.status = ntohl(tcp->src.status);
                           tcp->src.seqbase = ntohl(tcp->src.seqbase);
                           tcp->src.seq = ntohl(tcp->src.seq);
                           tcp->src.ack = ntohl(tcp->src.ack);
                           tcp->src.winnum = ntohl(tcp->src.winnum);
                           tcp->src.bytes = ntohl(tcp->src.bytes);
                           tcp->src.retrans = ntohl(tcp->src.retrans);
                           tcp->src.ackbytes = ntohl(tcp->src.ackbytes);
                           tcp->src.winbytes = ntohl(tcp->src.winbytes);
                           tcp->src.win = ntohs(tcp->src.win);

                           if (dsr->argus_dsrvl8.len > (((sizeof(struct ArgusTCPObject) - sizeof(struct ArgusTCPObjectMetrics))+3)/4 + 1)) {
                              tcp->dst.lasttime.tv_sec  = ntohl(tcp->dst.lasttime.tv_sec);
                              tcp->dst.lasttime.tv_usec = ntohl(tcp->dst.lasttime.tv_usec);
                              tcp->dst.status = ntohl(tcp->dst.status);
                              tcp->dst.seqbase = ntohl(tcp->dst.seqbase);
                              tcp->dst.seq = ntohl(tcp->dst.seq);
                              tcp->dst.ack = ntohl(tcp->dst.ack);
                              tcp->dst.winnum = ntohl(tcp->dst.winnum);
                              tcp->dst.bytes = ntohl(tcp->dst.bytes);
                              tcp->dst.retrans = ntohl(tcp->dst.retrans);
                              tcp->dst.ackbytes = ntohl(tcp->dst.ackbytes);
                              tcp->dst.winbytes = ntohl(tcp->dst.winbytes);
                              tcp->dst.win = ntohs(tcp->dst.win);
                           }
                           break;
                        }
                        case ARGUS_ICMP_DSR: {
                           struct ArgusICMPObject *icmpObj = (void *)&net->net_union.icmp;
                           icmpObj->iseq     = ntohl(icmpObj->iseq);
                           icmpObj->osrcaddr = ntohl(icmpObj->osrcaddr);
                           icmpObj->isrcaddr = ntohl(icmpObj->isrcaddr);
                           icmpObj->odstaddr = ntohl(icmpObj->odstaddr);
                           icmpObj->idstaddr = ntohl(icmpObj->idstaddr);
                           icmpObj->igwaddr  = ntohl(icmpObj->igwaddr);
                           break;
                        }
                        case ARGUS_ESP_DSR: {
                           struct ArgusESPObject *espObj = (struct ArgusESPObject *)&net->net_union.esp;
                           espObj->status  = ntohl(espObj->status);
                           espObj->spi     = ntohl(espObj->spi);
                           espObj->lastseq = ntohl(espObj->lastseq);
                           espObj->lostseq = ntohl(espObj->lostseq);
                           break;
                        }
                        case ARGUS_UDT_FLOW: {
                           struct ArgusUDTObject *udtObj = (struct ArgusUDTObject *)&net->net_union.udt;
                           udtObj->state                = ntohl(udtObj->state);
                           udtObj->status               = ntohl(udtObj->status);
                           udtObj->src.lasttime.tv_sec  = ntohl(udtObj->src.lasttime.tv_sec);
                           udtObj->src.lasttime.tv_usec = ntohl(udtObj->src.lasttime.tv_usec);
                           udtObj->src.seq              = ntohl(udtObj->src.seq);
                           udtObj->src.tstamp           = ntohl(udtObj->src.tstamp);
                           udtObj->src.ack              = ntohl(udtObj->src.ack);
                           udtObj->src.rtt              = ntohl(udtObj->src.rtt);
                           udtObj->src.var              = ntohl(udtObj->src.var);
                           udtObj->src.bsize            = ntohl(udtObj->src.bsize);
                           udtObj->src.rate             = ntohl(udtObj->src.rate);
                           udtObj->src.lcap             = ntohl(udtObj->src.lcap);
                           udtObj->src.solo             = ntohl(udtObj->src.solo);
                           udtObj->src.first            = ntohl(udtObj->src.first);
                           udtObj->src.middle           = ntohl(udtObj->src.middle);
                           udtObj->src.last             = ntohl(udtObj->src.last);
                           udtObj->src.drops            = ntohl(udtObj->src.drops);
                           udtObj->src.retrans          = ntohl(udtObj->src.retrans);
                           udtObj->src.nacked           = ntohl(udtObj->src.nacked);
                           break;
                        }
                        case ARGUS_RTP_FLOW: {
                           struct ArgusRTPObject *rtpObj = (struct ArgusRTPObject *)&net->net_union.rtp;
                           rtpObj->state       = ntohl(rtpObj->state);
                           rtpObj->src.rh_seq  = ntohs(rtpObj->src.rh_seq);
                           rtpObj->src.rh_time = ntohl(rtpObj->src.rh_time);
                           rtpObj->src.rh_ssrc = ntohl(rtpObj->src.rh_ssrc);

                           rtpObj->dst.rh_seq  = ntohs(rtpObj->dst.rh_seq);
                           rtpObj->dst.rh_time = ntohl(rtpObj->dst.rh_time);
                           rtpObj->dst.rh_ssrc = ntohl(rtpObj->dst.rh_ssrc);

                           rtpObj->sdrop       = ntohs(rtpObj->sdrop);
                           rtpObj->ddrop       = ntohs(rtpObj->ddrop);
                           rtpObj->ssdev       = ntohs(rtpObj->ssdev);
                           rtpObj->dsdev       = ntohs(rtpObj->dsdev);
                           break;
                        }
                        case ARGUS_RTCP_FLOW: {
                           struct ArgusRTCPObject *rtcpObj = (struct ArgusRTCPObject *)&net->net_union.rtcp;
                           rtcpObj->src.rh_len   = ntohs(rtcpObj->src.rh_len);
                           rtcpObj->src.rh_ssrc  = ntohl(rtcpObj->src.rh_ssrc);

                           rtcpObj->dst.rh_len   = ntohs(rtcpObj->dst.rh_len);
                           rtcpObj->dst.rh_ssrc  = ntohl(rtcpObj->dst.rh_ssrc);

                           rtcpObj->sdrop = ntohs(rtcpObj->sdrop);
                           rtcpObj->ddrop = ntohs(rtcpObj->ddrop);
                           break;
                        }
                     }
                     break;
                  }

                  case ARGUS_ICMP_DSR: {
                     struct ArgusIcmpStruct *icmp = (struct ArgusIcmpStruct *) dsr;
                     icmp->iseq     = ntohs(icmp->iseq);
                     icmp->osrcaddr = ntohl(icmp->osrcaddr);
                     icmp->isrcaddr = ntohl(icmp->isrcaddr);
                     icmp->odstaddr = ntohl(icmp->odstaddr);
                     icmp->idstaddr = ntohl(icmp->idstaddr);
                     icmp->igwaddr  = ntohl(icmp->igwaddr);
                     break;
                  }

                  case ARGUS_MAC_DSR: {
                     struct ArgusMacStruct *mac = (struct ArgusMacStruct *) dsr;
                     switch (mac->hdr.subtype & 0x3F) {
                     }
                     break;
                  }

                  case ARGUS_VLAN_DSR: {
                     struct ArgusVlanStruct *vlan = (struct ArgusVlanStruct *) dsr;
                     vlan->sid = ntohs(vlan->sid);
                     vlan->did = ntohs(vlan->did);
                     break;
                  }

                  case ARGUS_MPLS_DSR: {
                     struct ArgusMplsStruct *mpls = (struct ArgusMplsStruct *) dsr;
                     unsigned int *label = (unsigned int *)(dsr + 1);
                     int num, i;

                     if ((num = ((mpls->hdr.argus_dsrvl8.qual & 0xF0) >> 4)) > 0) {
                        for (i = 0; i < num; i++) {
                           *label = ntohl(*label);
                           label++;
                        }
                     }
                     if ((num = (mpls->hdr.argus_dsrvl8.qual & 0x0F)) > 0) {
                        for (i = 0; i < num; i++) {
                           *label = ntohl(*label);
                           label++;
                        }
                     }
                     break;
                  }
                   
                  case ARGUS_AGR_DSR: {
                     struct ArgusAgrStruct *agr = (struct ArgusAgrStruct *) dsr;
                     agr->count = ntohl(agr->count);
                     break;
                  }

                  case ARGUS_JITTER_DSR:
                  case ARGUS_COCODE_DSR:
                     break;

                  case ARGUS_DATA_DSR: {
                     struct ArgusDataStruct *data = (struct ArgusDataStruct *) dsr;
                     data->size  = ntohs(data->size);
                     data->count = ntohs(data->count);
                     break;
                  }

                  case ARGUS_BEHAVIOR_DSR: {
                     struct ArgusBehaviorStruct *actor = (struct ArgusBehaviorStruct *) dsr;
                     actor->keyStroke.src.n_strokes  = ntohl(actor->keyStroke.src.n_strokes);
                     actor->keyStroke.dst.n_strokes  = ntohl(actor->keyStroke.dst.n_strokes);
                     break;
                  }
               }

               if (dsr->subtype & ARGUS_LEN_16BITS)
                  dsr->argus_dsrvl16.len = ntohs(dsr->argus_dsrvl16.len);

               dsr = (struct ArgusDSRHeader *)((char *)dsr + cnt);
            }
         }
      }
   }
#endif
}


void
ArgusHtoN (struct ArgusRecord *argus)
{
#if defined(_LITTLE_ENDIAN)
   struct ArgusRecordHeader *hdr = &argus->hdr;
   struct ArgusDSRHeader *dsr = (struct ArgusDSRHeader *) (hdr + 1);

   switch (argus->hdr.type & 0xF0) {
      case ARGUS_MAR: {
         if (argus->hdr.len == sizeof (*argus)/4) {
            argus->argus_mar.status            = htonl(argus->argus_mar.status);
            argus->argus_mar.argusid           = htonl(argus->argus_mar.argusid);
            argus->argus_mar.localnet          = htonl(argus->argus_mar.localnet);
            argus->argus_mar.netmask           = htonl(argus->argus_mar.netmask);
            argus->argus_mar.nextMrSequenceNum = htonl(argus->argus_mar.nextMrSequenceNum);
            argus->argus_mar.startime.tv_sec   = htonl(argus->argus_mar.startime.tv_sec);
            argus->argus_mar.startime.tv_usec  = htonl(argus->argus_mar.startime.tv_usec);
            argus->argus_mar.now.tv_sec        = htonl(argus->argus_mar.now.tv_sec);
            argus->argus_mar.now.tv_usec       = htonl(argus->argus_mar.now.tv_usec);
            argus->argus_mar.reportInterval    = htons(argus->argus_mar.reportInterval);
            argus->argus_mar.argusMrInterval   = htons(argus->argus_mar.argusMrInterval);

            argus->argus_mar.pktsRcvd          = htonll(argus->argus_mar.pktsRcvd);
            argus->argus_mar.bytesRcvd         = htonll(argus->argus_mar.bytesRcvd);
            argus->argus_mar.drift             = htonll(argus->argus_mar.drift);

            argus->argus_mar.records           = htonl(argus->argus_mar.records);
            argus->argus_mar.flows             = htonl(argus->argus_mar.flows);
            argus->argus_mar.dropped           = htonl(argus->argus_mar.dropped);
            argus->argus_mar.queue             = htonl(argus->argus_mar.queue);
            argus->argus_mar.output            = htonl(argus->argus_mar.output);
            argus->argus_mar.clients           = htonl(argus->argus_mar.clients);
            argus->argus_mar.bufs              = htonl(argus->argus_mar.bufs);
            argus->argus_mar.bytes             = htonl(argus->argus_mar.bytes);

            argus->argus_mar.suserlen          = htons(argus->argus_mar.suserlen);
            argus->argus_mar.duserlen          = htons(argus->argus_mar.duserlen);

            argus->argus_mar.thisid            = htonl(argus->argus_mar.thisid);
            argus->argus_mar.record_len        = htonl(argus->argus_mar.record_len);
         }
         break;
      }


      case ARGUS_EVENT:
      case ARGUS_NETFLOW:
      case ARGUS_FAR: {
         if (argus->hdr.len > 1) {
            int cnt;
            while ((char *) dsr < ((char *) argus + (hdr->len * 4))) {
               switch (dsr->type & 0x7F) {
                  case ARGUS_FLOW_DSR: {
                     struct ArgusFlow *flow = (struct ArgusFlow *) dsr;

                     switch (flow->hdr.subtype & 0x3F) {
                        case ARGUS_FLOW_CLASSIC5TUPLE: {
                           switch ((flow->hdr.argus_dsrvl8.qual & 0x1F)) {
                              case ARGUS_TYPE_IPV4:
                                 flow->ip_flow.ip_src = htonl(flow->ip_flow.ip_src);
                                 flow->ip_flow.ip_dst = htonl(flow->ip_flow.ip_dst);
                                 switch (flow->ip_flow.ip_p) {
                                    case IPPROTO_TCP:
                                    case IPPROTO_UDP:
                                       flow->ip_flow.sport = htons(flow->ip_flow.sport);
                                       flow->ip_flow.dport = htons(flow->ip_flow.dport);
                                       break;
                                    case IPPROTO_ESP:
                                       flow->esp_flow.spi = htonl(flow->esp_flow.spi);
                                       break;
                                    case IPPROTO_IGMP:
                                       flow->igmp_flow.ip_id = htons(flow->igmp_flow.ip_id);
                                       break;
                                    case IPPROTO_ICMP:
                                       flow->icmp_flow.id    = ntohs(flow->icmp_flow.id);
                                       flow->icmp_flow.ip_id = ntohs(flow->icmp_flow.ip_id);
                                       break;
                                 }
                                 break; 

                              case ARGUS_TYPE_IPV6: {
                                 unsigned int *iptr = (unsigned int *)&flow->ipv6_flow;
                                 switch (flow->ipv6_flow.ip_p) {
                                    case IPPROTO_TCP:
                                    case IPPROTO_UDP:
                                       flow->ipv6_flow.sport = htons(flow->ipv6_flow.sport);
                                       flow->ipv6_flow.dport = htons(flow->ipv6_flow.dport);
                                       break;
                                 }
                                 iptr[8] = htonl(iptr[8]);
                                 break; 
                              }

                              case ARGUS_TYPE_ETHER: {
                                 struct ArgusMacFlow *mac = (struct ArgusMacFlow *) &flow->mac_flow;
                                 mac->mac_union.ether.ehdr.ether_type = htons(mac->mac_union.ether.ehdr.ether_type);
                                 break;
                              }

                              case ARGUS_TYPE_RARP: {
                                 struct ArgusRarpFlow *rarp = (struct ArgusRarpFlow *) &flow->rarp_flow;
                                 rarp->arp_tpa = htonl(rarp->arp_tpa);
                                 break;
                              }

                              case ARGUS_TYPE_ARP: {
                                 struct ArgusLegacyArpFlow *arp = (struct ArgusLegacyArpFlow *) &flow->flow_un;
                                 arp->arp_spa = htonl(arp->arp_spa);
                                 arp->arp_tpa = htonl(arp->arp_tpa);
                                 break;
                              }

                              case ARGUS_TYPE_ISIS: {
                                 struct ArgusIsisFlow *isis = (struct ArgusIsisFlow *) &flow->isis_flow;
                                 switch (isis->pdu_type) {
                                    case L1_LAN_IIH:
                                    case L2_LAN_IIH:
                                       break;

                                    case L1_CSNP:
                                    case L2_CSNP:
                                       break;

                                    case L1_PSNP:
                                    case L2_PSNP:
                                       break;

                                    case L1_LSP:
                                    case L2_LSP:
                                       isis->isis_un.lsp.seqnum = htonl(isis->isis_un.lsp.seqnum);
                                       break;
                                 }
                                 isis->pdu_type = htonl(isis->pdu_type);
                                 isis->chksum = htonl(isis->chksum);
                                 break;
                              }
                           }
                           break; 
                        }

                        case ARGUS_FLOW_ARP: {
                           switch (flow->hdr.argus_dsrvl8.qual & 0x1F) {
                              case ARGUS_TYPE_RARP: {
                                 struct ArgusRarpFlow *rarp = (struct ArgusRarpFlow *) &flow->rarp_flow;
                                 rarp->hrd = htons(rarp->hrd);
                                 rarp->pro = htons(rarp->pro);
                                 rarp->op  = htons(rarp->op);
                                 if (rarp->pln == 4) {
                                    rarp->arp_tpa = htonl(rarp->arp_tpa);
                                 }
                                 break;
                              }
                              case ARGUS_TYPE_ARP: {
                                 struct ArgusArpFlow *arp = (struct ArgusArpFlow *) &flow->arp_flow;
                                 arp->hrd = htons(arp->hrd);
                                 arp->pro = htons(arp->pro);
                                 arp->op  = htons(arp->op);
                                 if (arp->pln == 4) {
                                    arp->arp_spa = htonl(arp->arp_spa);
                                    arp->arp_tpa = htonl(arp->arp_tpa);
                                 }
                                 break;
                              }
                              default: {
                                 struct ArgusInterimArpFlow *arp = (void *) &flow->iarp_flow;
                                 arp->pro = htons(arp->pro);
                                 arp->arp_spa = htonl(arp->arp_spa);
                                 arp->arp_tpa = htonl(arp->arp_tpa);
                              }
                           }
                           break; 
                        }
                     }
                     break;
                  }

                  case ARGUS_ENCAPS_DSR: {
                     struct ArgusEncapsStruct *encaps = (struct ArgusEncapsStruct *) dsr;
                     encaps->src = htonl(encaps->src);
                     encaps->dst = htonl(encaps->dst);
                     break;
                  }

                  case ARGUS_ASN_DSR: {
                     struct ArgusAsnStruct *asn = (struct ArgusAsnStruct *) dsr;
                     asn->src_as = htonl(asn->src_as);
                     asn->dst_as = htonl(asn->dst_as);
                     if (asn->hdr.argus_dsrvl8.len > 3) 
                        asn->inode_as = htonl(asn->inode_as);
                     break;
                  }

                  case ARGUS_IPATTR_DSR: {
                     struct ArgusIPAttrStruct *attr = (struct ArgusIPAttrStruct *) dsr;
                     unsigned int *dsrptr = (unsigned int *)(dsr + 1);

                     if (attr->hdr.argus_dsrvl8.qual & ARGUS_IPATTR_SRC) {
                        struct ArgusIPAttrObject *aobj = (struct ArgusIPAttrObject *) dsrptr;
                        aobj->ip_id = htons(aobj->ip_id);
                        dsrptr++;
                     }
                     if (attr->hdr.argus_dsrvl8.qual & ARGUS_IPATTR_SRC_OPTIONS) {
                        *dsrptr = htonl(*dsrptr);
                        dsrptr++;
                     }
                     if (attr->hdr.argus_dsrvl8.qual & ARGUS_IPATTR_DST) {
                        struct ArgusIPAttrObject *aobj = (struct ArgusIPAttrObject *) dsrptr;
                        aobj->ip_id = htons(aobj->ip_id);
                        dsrptr++;
                     }
                     if (attr->hdr.argus_dsrvl8.qual & ARGUS_IPATTR_DST_OPTIONS) {
                        *dsrptr = htonl(*dsrptr);
                        dsrptr++;
                     }
                     break;
                  }

                  case ARGUS_TRANSPORT_DSR: {
                     struct ArgusTransportStruct *trans = (struct ArgusTransportStruct *) dsr;

                     if (trans->hdr.subtype & ARGUS_SEQ)
                        trans->seqnum = htonl(trans->seqnum);

                     if (trans->hdr.subtype & ARGUS_SRCID) {
                        switch (trans->hdr.argus_dsrvl8.qual) {
                           case ARGUS_TYPE_INT:
                              trans->srcid.a_un.value = htonl(trans->srcid.a_un.value);
                              break;
                           case ARGUS_TYPE_IPV4:
                              trans->srcid.a_un.value = htonl(trans->srcid.a_un.value);
                              break;

                           case ARGUS_TYPE_IPV6:
                           case ARGUS_TYPE_ETHER:
                           case ARGUS_TYPE_STRING:
                              break;
                        }
                     }
                     break;
                  }

                  case ARGUS_TIME_DSR: {
                     unsigned int i, *dtime = (unsigned int *) dsr;

                     for (i = 1; i < dsr->argus_dsrvl8.len; i++)
                        dtime[i] = htonl(dtime[i]);
                     break;
                  }

                  case ARGUS_METER_DSR: {
                     if (dsr->subtype & ARGUS_METER_PKTS_BYTES) {
                        switch (dsr->argus_dsrvl8.qual & 0x0F) {
                           case ARGUS_SRCDST_BYTE:
                           case ARGUS_SRC_BYTE:
                           case ARGUS_DST_BYTE:
                              break;
                           case ARGUS_SRCDST_SHORT:
                              ((unsigned short *)(dsr + 1))[0] = htons(((unsigned short *)(dsr + 1))[0]);
                              ((unsigned short *)(dsr + 1))[1] = htons(((unsigned short *)(dsr + 1))[1]);
                              ((unsigned short *)(dsr + 1))[2] = htons(((unsigned short *)(dsr + 1))[2]);
                              ((unsigned short *)(dsr + 1))[3] = htons(((unsigned short *)(dsr + 1))[3]);
                              break;
                           case ARGUS_SRCDST_INT:
                              ((unsigned int *)(dsr + 1))[0] = htonl(((unsigned int *)(dsr + 1))[0]);
                              ((unsigned int *)(dsr + 1))[1] = htonl(((unsigned int *)(dsr + 1))[1]);
                              ((unsigned int *)(dsr + 1))[2] = htonl(((unsigned int *)(dsr + 1))[2]);
                              ((unsigned int *)(dsr + 1))[3] = htonl(((unsigned int *)(dsr + 1))[3]);
                              break;
                           case ARGUS_SRCDST_LONGLONG:
                              ((long long *)(dsr + 1))[0] = htonll(((long long *)(dsr + 1))[0]);
                              ((long long *)(dsr + 1))[1] = htonll(((long long *)(dsr + 1))[1]);
                              ((long long *)(dsr + 1))[2] = htonll(((long long *)(dsr + 1))[2]);
                              ((long long *)(dsr + 1))[3] = htonll(((long long *)(dsr + 1))[3]);
                              break;
                           case ARGUS_SRC_SHORT:
                           case ARGUS_DST_SHORT:
                              ((unsigned short *)(dsr + 1))[0] = htons(((unsigned short *)(dsr + 1))[0]);
                              ((unsigned short *)(dsr + 1))[1] = htons(((unsigned short *)(dsr + 1))[1]);
                              break;
                           case ARGUS_SRC_INT:
                           case ARGUS_DST_INT:
                              ((unsigned int *)(dsr + 1))[0] = htonl(((unsigned int *)(dsr + 1))[0]);
                              ((unsigned int *)(dsr + 1))[1] = htonl(((unsigned int *)(dsr + 1))[1]);
                              break;
                           case ARGUS_SRC_LONGLONG:
                           case ARGUS_DST_LONGLONG:
                              ((long long *)(dsr + 1))[0] = htonll(((long long *)(dsr + 1))[0]);
                              ((long long *)(dsr + 1))[1] = htonll(((long long *)(dsr + 1))[1]);
                              break;
                        }
                     } else
                     if (dsr->subtype & ARGUS_METER_PKTS_BYTES_APP) {
                        switch (dsr->argus_dsrvl8.qual & 0x0F) {
                           case ARGUS_SRCDST_BYTE:
                           case ARGUS_SRC_BYTE:
                           case ARGUS_DST_BYTE:
                              break;
                           case ARGUS_SRCDST_SHORT:
                              ((unsigned short *)(dsr + 1))[0] = htons(((unsigned short *)(dsr + 1))[0]);
                              ((unsigned short *)(dsr + 1))[1] = htons(((unsigned short *)(dsr + 1))[1]);
                              ((unsigned short *)(dsr + 1))[2] = htons(((unsigned short *)(dsr + 1))[2]);
                              ((unsigned short *)(dsr + 1))[3] = htons(((unsigned short *)(dsr + 1))[3]);
                              ((unsigned short *)(dsr + 1))[4] = htons(((unsigned short *)(dsr + 1))[4]);
                              ((unsigned short *)(dsr + 1))[5] = htons(((unsigned short *)(dsr + 1))[5]);
                              break;
                           case ARGUS_SRCDST_INT:
                              ((unsigned int *)(dsr + 1))[0] = htonl(((unsigned int *)(dsr + 1))[0]);
                              ((unsigned int *)(dsr + 1))[1] = htonl(((unsigned int *)(dsr + 1))[1]);
                              ((unsigned int *)(dsr + 1))[2] = htonl(((unsigned int *)(dsr + 1))[2]);
                              ((unsigned int *)(dsr + 1))[3] = htonl(((unsigned int *)(dsr + 1))[3]);
                              ((unsigned int *)(dsr + 1))[4] = htonl(((unsigned int *)(dsr + 1))[4]);
                              ((unsigned int *)(dsr + 1))[5] = htonl(((unsigned int *)(dsr + 1))[5]);
                              break;
                           case ARGUS_SRCDST_LONGLONG:
                              ((long long *)(dsr + 1))[0] = htonll(((long long *)(dsr + 1))[0]);
                              ((long long *)(dsr + 1))[1] = htonll(((long long *)(dsr + 1))[1]);
                              ((long long *)(dsr + 1))[2] = htonll(((long long *)(dsr + 1))[2]);
                              ((long long *)(dsr + 1))[3] = htonll(((long long *)(dsr + 1))[3]);
                              ((long long *)(dsr + 1))[4] = htonll(((long long *)(dsr + 1))[4]);
                              ((long long *)(dsr + 1))[5] = htonll(((long long *)(dsr + 1))[5]);
                              break;
                           case ARGUS_SRC_SHORT:
                           case ARGUS_DST_SHORT:
                              ((unsigned short *)(dsr + 1))[0] = htons(((unsigned short *)(dsr + 1))[0]);
                              ((unsigned short *)(dsr + 1))[1] = htons(((unsigned short *)(dsr + 1))[1]);
                              ((unsigned short *)(dsr + 1))[2] = htons(((unsigned short *)(dsr + 1))[2]);
                              break;
                           case ARGUS_SRC_INT:
                           case ARGUS_DST_INT:
                              ((unsigned int *)(dsr + 1))[0] = htonl(((unsigned int *)(dsr + 1))[0]);
                              ((unsigned int *)(dsr + 1))[1] = htonl(((unsigned int *)(dsr + 1))[1]);
                              ((unsigned int *)(dsr + 1))[2] = htonl(((unsigned int *)(dsr + 1))[2]);
                              break;
                           case ARGUS_SRC_LONGLONG:
                           case ARGUS_DST_LONGLONG:
                              ((long long *)(dsr + 1))[0] = htonll(((long long *)(dsr + 1))[0]);
                              ((long long *)(dsr + 1))[1] = htonll(((long long *)(dsr + 1))[1]);
                              ((long long *)(dsr + 1))[2] = htonll(((long long *)(dsr + 1))[2]);
                              break;
                        }
                     }
                     break;
                  }

                  case ARGUS_PSIZE_DSR: {
                     switch (dsr->argus_dsrvl8.qual & 0x0F) {
                        case ARGUS_SRCDST_SHORT:
                           ((unsigned short *)(dsr + 1))[0] = htons(((unsigned short *)(dsr + 1))[0]);
                           ((unsigned short *)(dsr + 1))[1] = htons(((unsigned short *)(dsr + 1))[1]);
                           ((unsigned short *)(dsr + 1))[2] = htons(((unsigned short *)(dsr + 1))[2]);
                           ((unsigned short *)(dsr + 1))[3] = htons(((unsigned short *)(dsr + 1))[3]);
                           break;
                           
                        case ARGUS_SRC_SHORT:
                        case ARGUS_DST_SHORT:
                           ((unsigned short *)(dsr + 1))[0] = htons(((unsigned short *)(dsr + 1))[0]);
                           ((unsigned short *)(dsr + 1))[1] = htons(((unsigned short *)(dsr + 1))[1]);
                           break; 
                           
                        case ARGUS_SRCDST_INT:
                           ((unsigned int *)(dsr + 1))[0] = htonl(((unsigned int *)(dsr + 1))[0]);
                           ((unsigned int *)(dsr + 1))[1] = htonl(((unsigned int *)(dsr + 1))[1]);
                           ((unsigned int *)(dsr + 1))[2] = htonl(((unsigned int *)(dsr + 1))[2]);
                           ((unsigned int *)(dsr + 1))[3] = htonl(((unsigned int *)(dsr + 1))[3]);
                           break;
                           
                        case ARGUS_SRC_INT:
                        case ARGUS_DST_INT:
                           ((unsigned int *)(dsr + 1))[0] = htonl(((unsigned int *)(dsr + 1))[0]);
                           ((unsigned int *)(dsr + 1))[1] = htonl(((unsigned int *)(dsr + 1))[1]);
                           break;
                     }     
                     break;
                  }

                  case ARGUS_NETWORK_DSR: {
                     struct ArgusNetworkStruct *net = (struct ArgusNetworkStruct *)dsr;
                     switch (net->hdr.subtype) {
                        case ARGUS_TCP_INIT: {
                           struct ArgusTCPInitStatus *tcp = (void *)&net->net_union.tcpinit;
                           tcp->status       = htonl(tcp->status);
                           tcp->seqbase      = htonl(tcp->seqbase);
                           tcp->options      = htonl(tcp->options);
                           tcp->win          = htons(tcp->win);
                           break;
                        }
                        case ARGUS_TCP_STATUS: {
                           struct ArgusTCPStatus *tcp = (struct ArgusTCPStatus *)&net->net_union.tcpstatus;
                           tcp->status       = htonl(tcp->status);
                           break;
                        }
                        case ARGUS_TCP_PERF: {
                           struct ArgusTCPObject *tcp = (struct ArgusTCPObject *)&net->net_union.tcp;
                           tcp->status       = htonl(tcp->status);
                           tcp->state        = htonl(tcp->state);
                           tcp->options      = htonl(tcp->options);
                           tcp->synAckuSecs  = htonl(tcp->synAckuSecs);
                           tcp->ackDatauSecs = htonl(tcp->ackDatauSecs);

                           tcp->src.lasttime.tv_sec  = htonl(tcp->src.lasttime.tv_sec);
                           tcp->src.lasttime.tv_usec = htonl(tcp->src.lasttime.tv_usec);
                           tcp->src.status = htonl(tcp->src.status);
                           tcp->src.seqbase = htonl(tcp->src.seqbase);
                           tcp->src.seq = htonl(tcp->src.seq);
                           tcp->src.ack = htonl(tcp->src.ack);
                           tcp->src.winnum = htonl(tcp->src.winnum);
                           tcp->src.bytes = htonl(tcp->src.bytes);
                           tcp->src.retrans = htonl(tcp->src.retrans);
                           tcp->src.ackbytes = htonl(tcp->src.ackbytes);
                           tcp->src.winbytes = htonl(tcp->src.winbytes);
                           tcp->src.win = htons(tcp->src.win);

                           if (dsr->argus_dsrvl8.len > (((sizeof(struct ArgusTCPObject) - sizeof(struct ArgusTCPObjectMetrics))+3)/4 + 1)) {
                              tcp->dst.lasttime.tv_sec  = htonl(tcp->dst.lasttime.tv_sec);
                              tcp->dst.lasttime.tv_usec = htonl(tcp->dst.lasttime.tv_usec);
                              tcp->dst.status = htonl(tcp->dst.status);
                              tcp->dst.seqbase = htonl(tcp->dst.seqbase);
                              tcp->dst.seq = htonl(tcp->dst.seq);
                              tcp->dst.ack = htonl(tcp->dst.ack);
                              tcp->dst.winnum = htonl(tcp->dst.winnum);
                              tcp->dst.bytes = htonl(tcp->dst.bytes);
                              tcp->dst.retrans = htonl(tcp->dst.retrans);
                              tcp->dst.ackbytes = htonl(tcp->dst.ackbytes);
                              tcp->dst.winbytes = htonl(tcp->dst.winbytes);
                              tcp->dst.win = htons(tcp->dst.win);
                           }
                           break;
                        }
                        case ARGUS_ICMP_DSR: {
                           struct ArgusICMPObject *icmpObj = (void *)&net->net_union.icmp;
                           icmpObj->iseq     = htonl(icmpObj->iseq);
                           icmpObj->osrcaddr = htonl(icmpObj->osrcaddr);
                           icmpObj->isrcaddr = htonl(icmpObj->isrcaddr);
                           icmpObj->odstaddr = htonl(icmpObj->odstaddr);
                           icmpObj->idstaddr = htonl(icmpObj->idstaddr);
                           icmpObj->igwaddr  = htonl(icmpObj->igwaddr);
                           break;
                        }
                        case ARGUS_ESP_DSR: {
                           struct ArgusESPObject *espObj = (struct ArgusESPObject *)&net->net_union.esp;
                           espObj->status  = htonl(espObj->status);
                           espObj->spi     = htonl(espObj->spi);
                           espObj->lastseq = htonl(espObj->lastseq);
                           espObj->lostseq = htonl(espObj->lostseq);
                           break;
                        }
                        case ARGUS_UDT_FLOW: {
                           struct ArgusUDTObject *udtObj = (struct ArgusUDTObject *)&net->net_union.udt;
                           udtObj->state       = htonl(udtObj->state);
                           udtObj->status      = htonl(udtObj->status);
                           udtObj->src.lasttime.tv_sec  = htonl(udtObj->src.lasttime.tv_sec);
                           udtObj->src.lasttime.tv_usec = htonl(udtObj->src.lasttime.tv_usec);
                           udtObj->src.seq              = htonl(udtObj->src.seq);
                           udtObj->src.tstamp           = htonl(udtObj->src.tstamp);
                           udtObj->src.ack              = htonl(udtObj->src.ack);
                           udtObj->src.rtt              = htonl(udtObj->src.rtt);
                           udtObj->src.var              = htonl(udtObj->src.var);
                           udtObj->src.bsize            = htonl(udtObj->src.bsize);
                           udtObj->src.rate             = htonl(udtObj->src.rate);
                           udtObj->src.lcap             = htonl(udtObj->src.lcap);
                           udtObj->src.solo             = htonl(udtObj->src.solo);
                           udtObj->src.first            = htonl(udtObj->src.first);
                           udtObj->src.middle           = htonl(udtObj->src.middle);
                           udtObj->src.last             = htonl(udtObj->src.last);
                           udtObj->src.drops            = htonl(udtObj->src.drops);
                           udtObj->src.retrans          = htonl(udtObj->src.retrans);
                           udtObj->src.nacked           = htonl(udtObj->src.nacked);
                           break;
                        }
                        case ARGUS_RTP_FLOW: {
                           struct ArgusRTPObject *rtpObj = (struct ArgusRTPObject *)&net->net_union.rtp;
                           rtpObj->state       = htonl(rtpObj->state);
                           rtpObj->src.rh_seq  = htons(rtpObj->src.rh_seq);
                           rtpObj->src.rh_time = htonl(rtpObj->src.rh_time);
                           rtpObj->src.rh_ssrc = htonl(rtpObj->src.rh_ssrc);

                           rtpObj->dst.rh_seq  = htons(rtpObj->dst.rh_seq);
                           rtpObj->dst.rh_time = htonl(rtpObj->dst.rh_time);
                           rtpObj->dst.rh_ssrc = htonl(rtpObj->dst.rh_ssrc);

                           rtpObj->sdrop       = htons(rtpObj->sdrop);
                           rtpObj->ddrop       = htons(rtpObj->ddrop);
                           rtpObj->ssdev       = htons(rtpObj->ssdev);
                           rtpObj->dsdev       = htons(rtpObj->dsdev);
                           break;
                        }
                        case ARGUS_RTCP_FLOW: {
                           struct ArgusRTCPObject *rtcpObj = (struct ArgusRTCPObject *)&net->net_union.rtcp;
                           rtcpObj->src.rh_len   = htons(rtcpObj->src.rh_len);
                           rtcpObj->src.rh_ssrc  = htonl(rtcpObj->src.rh_ssrc);

                           rtcpObj->dst.rh_len   = htons(rtcpObj->dst.rh_len);
                           rtcpObj->dst.rh_ssrc  = htonl(rtcpObj->dst.rh_ssrc);

                           rtcpObj->sdrop = htons(rtcpObj->sdrop);
                           rtcpObj->ddrop = htons(rtcpObj->ddrop);
                           break;
                        }
                     }
                     break;
                  }

                  case ARGUS_ICMP_DSR: {
                     struct ArgusIcmpStruct *icmp = (struct ArgusIcmpStruct *) dsr;
                     icmp->iseq     = htons(icmp->iseq);
                     icmp->osrcaddr = htonl(icmp->osrcaddr);
                     icmp->isrcaddr = htonl(icmp->isrcaddr);
                     icmp->odstaddr = htonl(icmp->odstaddr);
                     icmp->idstaddr = htonl(icmp->idstaddr);
                     icmp->igwaddr  = htonl(icmp->igwaddr);
                     break;
                  }

                  case ARGUS_MAC_DSR: {
                     struct ArgusMacStruct *mac = (struct ArgusMacStruct *) dsr;
                     switch (mac->hdr.subtype & 0x3F) {
                     }
                     break;
                  }

                  case ARGUS_VLAN_DSR: {
                     struct ArgusVlanStruct *vlan = (struct ArgusVlanStruct *) dsr;
                     vlan->sid = htons(vlan->sid);
                     vlan->did = htons(vlan->did);
                     break;
                  }

                  case ARGUS_MPLS_DSR: {
                     struct ArgusMplsStruct *mpls = (struct ArgusMplsStruct *) dsr;
                     unsigned int *label = (unsigned int *)(dsr + 1);
                     int num, i;

                     if ((num = ((mpls->hdr.argus_dsrvl8.qual & 0xF0) >> 4)) > 0) {
                        for (i = 0; i < num; i++) {
                           *label = htonl(*label);
                           label++;
                        }
                     }
                     if ((num = (mpls->hdr.argus_dsrvl8.qual & 0x0F)) > 0) {
                        for (i = 0; i < num; i++) {
                           *label = htonl(*label);
                           label++;
                        }
                     }
                     break;
                  }

                  case ARGUS_AGR_DSR: {
                     struct ArgusAgrStruct *agr = (struct ArgusAgrStruct *) dsr;
                     agr->count = htonl(agr->count);
                     break;
                  }

                  case ARGUS_JITTER_DSR:
                  case ARGUS_COCODE_DSR:
                     break;

                  case ARGUS_DATA_DSR: {
                     struct ArgusDataStruct *data = (struct ArgusDataStruct *) dsr;
                     data->size  = htons(data->size);
                     data->count = htons(data->count);
                     break;
                  }

                  case ARGUS_BEHAVIOR_DSR: {
                     struct ArgusBehaviorStruct *actor = (struct ArgusBehaviorStruct *) dsr;
                     actor->keyStroke.src.n_strokes  = htonl(actor->keyStroke.src.n_strokes);
                     actor->keyStroke.dst.n_strokes  = htonl(actor->keyStroke.dst.n_strokes);
                     break;
                  }
               }

               if ((cnt = (((dsr->type & ARGUS_IMMEDIATE_DATA) ? 1 :
                           ((dsr->subtype & ARGUS_LEN_16BITS)  ? dsr->argus_dsrvl16.len :
                                                                 dsr->argus_dsrvl8.len))) * 4) > 0) {
                  if (dsr->subtype & ARGUS_LEN_16BITS)  
                     dsr->argus_dsrvl16.len = htons(dsr->argus_dsrvl16.len);

                  dsr = (struct ArgusDSRHeader *)((char *)dsr + cnt);

               } else
                  break;
            }
         }
         break;
      }
   }

   hdr->len = htons(hdr->len);
#endif
}

void
ArgusV2NtoH (struct ArgusV2Record *argus)
{
#if defined(_LITTLE_ENDIAN)
   int farlen = 0;

   argus->ahdr.length    = ntohs(argus->ahdr.length);
   argus->ahdr.argusid   = ntohl(argus->ahdr.argusid);
   argus->ahdr.seqNumber = ntohl(argus->ahdr.seqNumber);
   argus->ahdr.status    = ntohl(argus->ahdr.status);

   if (argus->ahdr.type & ARGUS_V2_MAR) {

      argus->argus_mar.startime.tv_sec  = ntohl(argus->argus_mar.startime.tv_sec);
      argus->argus_mar.startime.tv_usec = ntohl(argus->argus_mar.startime.tv_usec);
      argus->argus_mar.now.tv_sec  = ntohl(argus->argus_mar.now.tv_sec);
      argus->argus_mar.now.tv_usec = ntohl(argus->argus_mar.now.tv_usec);
      argus->argus_mar.reportInterval = ntohs(argus->argus_mar.reportInterval);
      argus->argus_mar.argusMrInterval = ntohs(argus->argus_mar.argusMrInterval);
      argus->argus_mar.argusid = ntohl(argus->argus_mar.argusid);
      argus->argus_mar.localnet = ntohl(argus->argus_mar.localnet);
      argus->argus_mar.netmask = ntohl(argus->argus_mar.netmask);
      argus->argus_mar.nextMrSequenceNum = ntohl(argus->argus_mar.nextMrSequenceNum);

      argus->argus_mar.pktsRcvd  = ntohll(argus->argus_mar.pktsRcvd);
      argus->argus_mar.bytesRcvd = ntohll(argus->argus_mar.bytesRcvd);

      argus->argus_mar.pktsDrop = ntohl(argus->argus_mar.pktsDrop);
      argus->argus_mar.flows = ntohl(argus->argus_mar.flows);
      argus->argus_mar.flowsClosed = ntohl(argus->argus_mar.flowsClosed);

      argus->argus_mar.actIPcons = ntohl( argus->argus_mar.actIPcons);
      argus->argus_mar.cloIPcons = ntohl( argus->argus_mar.cloIPcons);
      argus->argus_mar.actICMPcons = ntohl( argus->argus_mar.actICMPcons);
      argus->argus_mar.cloICMPcons = ntohl( argus->argus_mar.cloICMPcons);
      argus->argus_mar.actIGMPcons = ntohl( argus->argus_mar.actIGMPcons);
      argus->argus_mar.cloIGMPcons = ntohl( argus->argus_mar.cloIGMPcons);

      argus->argus_mar.inputs = ntohl( argus->argus_mar.inputs);
      argus->argus_mar.outputs = ntohl( argus->argus_mar.outputs);
      argus->argus_mar.qcount = ntohl( argus->argus_mar.qcount);
      argus->argus_mar.qtime = ntohl( argus->argus_mar.qtime);

      argus->argus_mar.record_len = ntohl(argus->argus_mar.record_len);

   } else {
      unsigned int status;
      int length = argus->ahdr.length - sizeof(argus->ahdr);
      struct ArgusV2FarHeaderStruct *farhdr = (struct ArgusV2FarHeaderStruct *) &argus->argus_far;

      farhdr->status = ntohs(farhdr->status);

      status = argus->ahdr.status;

      while (length > 0) {
         switch (farhdr->type) {
            case ARGUS_V2_FAR: {
               struct ArgusV2FarStruct *far = (struct ArgusV2FarStruct *) farhdr;
               
               far->ArgusV2TransRefNum = ntohl(far->ArgusV2TransRefNum);

               switch (status & (ETHERTYPE_IP|ETHERTYPE_IPV6|ETHERTYPE_ARP)) {
                  case ETHERTYPE_IP: {
                     struct ArgusV2IPFlow *ipflow = &far->flow.flow_union.ip;

                     far->attr_ip.soptions = ntohs(far->attr_ip.soptions);
                     far->attr_ip.doptions = ntohs(far->attr_ip.doptions);

                     switch (ipflow->ip_p) {
                        case IPPROTO_UDP:
                        case IPPROTO_TCP:
                           ipflow->ip_src = ntohl(ipflow->ip_src);
                           ipflow->ip_dst = ntohl(ipflow->ip_dst);
                           ipflow->sport  = ntohs(ipflow->sport);
                           ipflow->dport  = ntohs(ipflow->dport);
                           ipflow->ip_id  = ntohs(ipflow->ip_id);
                           break;

                        case IPPROTO_ICMP: {
                           struct ArgusV2ICMPFlow *icmpflow = &far->flow.flow_union.icmp;

                           icmpflow->ip_src = ntohl(icmpflow->ip_src);
                           icmpflow->ip_dst = ntohl(icmpflow->ip_dst);
                           icmpflow->id     = ntohs(icmpflow->id);
                           icmpflow->ip_id  = ntohs(icmpflow->ip_id);
                           break;
                        }

                        case IPPROTO_IGMP: {
                           struct ArgusV2IGMPFlow *igmpflow = &far->flow.flow_union.igmp;

                           igmpflow->ip_src = ntohl(igmpflow->ip_src);
                           igmpflow->ip_dst = ntohl(igmpflow->ip_dst);
                           igmpflow->ip_id  = ntohs(igmpflow->ip_id);
                           break;
                        }

                        default: {
                           ipflow->ip_src = ntohl(ipflow->ip_src);
                           ipflow->ip_dst = ntohl(ipflow->ip_dst);
                           break;
                        }
                     }
                     break;
                  }
         
                  case ETHERTYPE_IPV6:
                     break;

                  case ETHERTYPE_ARP: {
                     struct ArgusV2ArpFlow *arpflow = &far->flow.flow_union.arp;
         
                     arpflow->arp_tpa = ntohl(arpflow->arp_tpa);
                     arpflow->arp_spa = ntohl(arpflow->arp_spa);
                     break;
                  }

                  default:
                     break;
               }
         
               far->time.start.tv_sec  = ntohl(far->time.start.tv_sec);
               far->time.start.tv_usec = ntohl(far->time.start.tv_usec);
               far->time.last.tv_sec   = ntohl(far->time.last.tv_sec);
               far->time.last.tv_usec  = ntohl(far->time.last.tv_usec);
         
               far->src.count    = ntohl(far->src.count);
               far->src.bytes    = ntohl(far->src.bytes);
               far->src.appbytes = ntohl(far->src.appbytes);
         
               far->dst.count    = ntohl(far->dst.count);
               far->dst.bytes    = ntohl(far->dst.bytes);
               far->dst.appbytes = ntohl(far->dst.appbytes);
               break;
            }

            case ARGUS_V2_MAC_DSR: {
               struct ArgusV2MacStruct *mac = (struct ArgusV2MacStruct *) farhdr;
               if (farhdr->length == sizeof(*mac)) {
                  mac->status   = ntohs(mac->status);
               }
               break;
            }


            case ARGUS_V2_VLAN_DSR: {
               struct ArgusV2VlanStruct *vlan = (struct ArgusV2VlanStruct *) farhdr;

               if (vlan->length != sizeof (struct ArgusV2VlanStruct))  /* fix for pre 2.0.1 len problem */
                  vlan->length = sizeof (struct ArgusV2VlanStruct);

               vlan->status = ntohs(vlan->status);
               vlan->sid    = ntohs(vlan->sid);
               vlan->did    = ntohs(vlan->did);
               break;
            }

            case ARGUS_V2_MPLS_DSR: {
               struct ArgusV2MplsStruct *mpls = (struct ArgusV2MplsStruct *) farhdr;
               mpls->status = ntohs(mpls->status);
               mpls->slabel = ntohl(mpls->slabel);
               mpls->dlabel = ntohl(mpls->dlabel);
               mpls->length = sizeof(*mpls);  /* fix for V2 argus error */
               break;
            }

            case ARGUS_V2_TCP_DSR: {
               struct ArgusV2TCPObject *tcp = (struct ArgusV2TCPObject *) farhdr; 

               if (farhdr->length == sizeof(*tcp)) {
                  tcp->status = ntohs(tcp->status); 
                  tcp->state  = ntohl(tcp->state); 
                  tcp->synAckuSecs  = ntohl(tcp->synAckuSecs); 
                  tcp->ackDatauSecs = ntohl(tcp->ackDatauSecs); 
                  tcp->options = ntohl(tcp->options); 
                  tcp->src.seqbase  = ntohl(tcp->src.seqbase); 
                  tcp->src.ackbytes = ntohl(tcp->src.ackbytes); 
                  tcp->src.rpkts    = ntohl(tcp->src.rpkts); 
                  tcp->src.win     = ntohs(tcp->src.win); 
                  tcp->dst.seqbase  = ntohl(tcp->dst.seqbase); 
                  tcp->dst.ackbytes = ntohl(tcp->dst.ackbytes); 
                  tcp->dst.rpkts    = ntohl(tcp->dst.rpkts); 
                  tcp->dst.win     = ntohs(tcp->dst.win); 
               }
               break;
            }

            case ARGUS_V2_ICMP_DSR: {
               struct ArgusV2ICMPObject *icmp = (struct ArgusV2ICMPObject *) farhdr;
 
               if (farhdr->length == sizeof(*icmp)) {
                  icmp->status   = ntohs(icmp->status);
                  icmp->iseq     = ntohs(icmp->iseq);
                  icmp->osrcaddr = ntohl(icmp->osrcaddr);
                  icmp->odstaddr = ntohl(icmp->odstaddr);
                  icmp->isrcaddr = ntohl(icmp->isrcaddr);
                  icmp->idstaddr = ntohl(icmp->idstaddr);
                  icmp->igwaddr  = ntohl(icmp->igwaddr);
               }
               break;
            }

            case ARGUS_V2_IGMP_DSR: {
               struct ArgusV2IGMPObject *igmp = (struct ArgusV2IGMPObject *) farhdr;

               igmp->status         = ntohs(igmp->status);
               igmp->igmp_group     = ntohl(igmp->igmp_group);

               if (igmp->length == sizeof(struct ArgusV2IGMPObject)) {
                  igmp->jdelay.tv_sec  = ntohl(igmp->jdelay.tv_sec);
                  igmp->jdelay.tv_usec = ntohl(igmp->jdelay.tv_usec);
                  igmp->ldelay.tv_sec  = ntohl(igmp->ldelay.tv_sec);
                  igmp->ldelay.tv_usec = ntohl(igmp->ldelay.tv_usec);
               }
               break;
            }

            case ARGUS_V2_RTP_DSR: {
               struct ArgusV2RTPObject *rtp = (void *) farhdr;
               if (farhdr->length == sizeof(*rtp)) {
                  rtp->status = ntohs(rtp->status);
                  rtp->state  = ntohl(rtp->state);
                  rtp->sdrop  = ntohs(rtp->sdrop);
                  rtp->ddrop  = ntohs(rtp->ddrop);
                  rtp->ssdev  = ntohs(rtp->ssdev);
                  rtp->dsdev  = ntohs(rtp->dsdev);
               }
               break;
            }

            case ARGUS_V2_TIME_DSR: {
               struct ArgusV2TimeStruct *time = (void *) farhdr;

               if (farhdr->length == sizeof(*time)) {
                  time->status = ntohs(time->status);
                  time->src.act.n       = ntohl(time->src.act.n);
                  time->src.act.minval     = ntohl(time->src.act.minval);
                  time->src.act.meanval    = ntohl(time->src.act.meanval);
                  time->src.act.stdev   = ntohl(time->src.act.stdev);
                  time->src.act.maxval     = ntohl(time->src.act.maxval);
                  time->src.idle.n      = ntohl(time->src.idle.n);
                  time->src.idle.minval    = ntohl(time->src.idle.minval);
                  time->src.idle.meanval   = ntohl(time->src.idle.meanval);
                  time->src.idle.stdev  = ntohl(time->src.idle.stdev);
                  time->src.idle.maxval    = ntohl(time->src.idle.maxval);
                  time->dst.act.n       = ntohl(time->dst.act.n);
                  time->dst.act.minval     = ntohl(time->dst.act.minval);
                  time->dst.act.meanval    = ntohl(time->dst.act.meanval);
                  time->dst.act.stdev   = ntohl(time->dst.act.stdev);
                  time->dst.act.maxval     = ntohl(time->dst.act.maxval);
                  time->dst.idle.n      = ntohl(time->dst.idle.n);
                  time->dst.idle.minval    = ntohl(time->dst.idle.minval);
                  time->dst.idle.meanval   = ntohl(time->dst.idle.meanval);
                  time->dst.idle.stdev  = ntohl(time->dst.idle.stdev);
                  time->dst.idle.maxval    = ntohl(time->dst.idle.maxval);
               }
               break;
            }

            case ARGUS_V2_SRCUSRDATA_DSR: {
               struct ArgusV2UserStruct *user = (struct ArgusV2UserStruct *) farhdr;
               user->status   = ntohs(user->status);
               break;
            }

            case ARGUS_V2_DSTUSRDATA_DSR: {
               struct ArgusV2UserStruct *user = (struct ArgusV2UserStruct *) farhdr;
               user->status   = ntohs(user->status);
               break;
            }

            case ARGUS_V2_ESP_DSR: {
               struct ArgusV2ESPStruct *esp = (struct ArgusV2ESPStruct *) farhdr;
               if (farhdr->length == sizeof(*esp)) {
                  esp->status      = ntohs(esp->status);
                  esp->src.spi     = ntohl(esp->src.spi);
                  esp->src.lastseq = ntohl(esp->src.lastseq);
                  esp->src.lostseq = ntohl(esp->src.lostseq);
                  esp->dst.spi     = ntohl(esp->dst.spi);
                  esp->dst.lastseq = ntohl(esp->dst.lastseq);
                  esp->dst.lostseq = ntohl(esp->dst.lostseq);
               }
               break;
            }


            case ARGUS_V2_AGR_DSR: {
               struct ArgusV2AGRStruct *agr = (struct ArgusV2AGRStruct *) farhdr;
 
               if (farhdr->length == sizeof(*agr)) {
                  agr->status               = ntohs(agr->status);
                  agr->count                = ntohl(agr->count);
                  agr->laststartime.tv_sec  = ntohl(agr->laststartime.tv_sec);
                  agr->laststartime.tv_usec = ntohl(agr->laststartime.tv_usec);
                  agr->lasttime.tv_sec      = ntohl(agr->lasttime.tv_sec);
                  agr->lasttime.tv_usec     = ntohl(agr->lasttime.tv_usec);
                  agr->act.minval           = ntohl(agr->act.minval);
                  agr->act.meanval          = ntohl(agr->act.meanval);
                  agr->act.stdev            = ntohl(agr->act.stdev);
                  agr->act.maxval           = ntohl(agr->act.maxval);
                  agr->idle.minval          = ntohl(agr->idle.minval);
                  agr->idle.meanval         = ntohl(agr->idle.meanval);
                  agr->idle.stdev           = ntohl(agr->idle.stdev);
                  agr->idle.maxval          = ntohl(agr->idle.maxval);
               }
               break;
            }

            default:
               break;
         }
         if ((farlen = farhdr->length) == 0)
            break;

         if ((farhdr->type == ARGUS_V2_SRCUSRDATA_DSR) ||
             (farhdr->type == ARGUS_V2_DSTUSRDATA_DSR))
            farlen = farlen * 4;

         length -= farlen;
         farhdr = (struct ArgusV2FarHeaderStruct *)((char *)farhdr + farlen);
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (7, "ArgusV2NtoH (0x%x) returning.\n", argus);
#endif
#endif
}


void
ArgusV2HtoN (struct ArgusV2Record *argus)
{
#if defined(_LITTLE_ENDIAN)

#ifdef ARGUSDEBUG
   ArgusDebug (7, "ArgusV2HtoN (0x%x) returning.\n", argus);
#endif
#endif
}


void
ArgusPrintHex (const u_char *bp, u_int length)
{
   const u_short *sp;
   u_int i;
   int nshorts;

   sp = (u_short *)bp;
   nshorts = (u_int) length / sizeof(u_short);
   i = 0;
   while (--nshorts >= 0) {
      if ((i++ % 8) == 0) {
         (void)printf("\n\t");
      }
      (void)printf(" %04x", ntohs(*sp++));
   }

   if (length & 1) {
      if ((i % 8) == 0)
         (void)printf("\n\t");

      (void)printf(" %02x", *(u_char *)sp);
   }
   (void)printf("\n");
   fflush(stdout);
}


int ArgusAllocMax   = 0;
int ArgusAllocBytes = 0;
int ArgusAllocTotal = 0;
int ArgusFreeTotal  = 0;

struct ArgusMemoryList memory = {NULL, 0};

#define ARGUS_ALLOC	0x45672381
/*
#define ARGUS_ALIGN	128
*/

void *     
ArgusMalloc (int bytes) 
{          
   void *retn = NULL; 
   int offset;
 
   if (bytes) {
      if (ArgusAllocTotal++ == 0) {
#if defined(ARGUS_THREADS)
         pthread_mutex_init(&memory.lock, NULL);
#endif
      }
      ArgusAllocBytes += bytes;
      if (ArgusAllocMax < ArgusAllocBytes)
         ArgusAllocMax = ArgusAllocBytes;

#if defined(ARGUS_ALIGN)
      offset = ARGUS_ALIGN;
#else
      offset = 0;
#endif

#if !defined(ARGUSMEMDEBUG)
      retn = (void *) malloc (bytes + offset);
#else
      if ((retn = (u_int *) malloc (bytes + sizeof(struct ArgusMemoryHeader) + offset)) != NULL) {
         struct ArgusMemoryHeader *mem = (struct ArgusMemoryHeader *)retn;
         mem->tag = ARGUS_ALLOC;
         mem->len = bytes;
         mem->offset = offset;
#if defined(__GNUC__)
         mem->frame[0] = __builtin_return_address(0);
         mem->frame[1] = __builtin_return_address(1);
         mem->frame[2] = __builtin_return_address(2);
#endif
#if defined(ARGUS_THREADS)
         pthread_mutex_lock(&memory.lock);
#endif
         if (memory.start) {
            mem->nxt = memory.start;
            mem->prv = memory.end;
            mem->prv->nxt = mem;
            mem->nxt->prv = mem;
            memory.end = mem;
         } else {
            memory.start = mem;
            memory.end = mem;
            mem->nxt = mem;
            mem->prv = mem;
         }
         memory.count++;
         memory.total++;
#if defined(ARGUS_THREADS)
         pthread_mutex_unlock(&memory.lock);
#endif
         retn = (void *)(mem + 1);
      }
#endif

#if defined(ARGUS_ALIGN)
      if (retn != NULL) {
         unsigned short toff;
         toff = ((unsigned long)retn & (offset - 1));
         toff = offset - toff;
         retn = (void *)((char *)retn + toff);
         ((unsigned short *)retn)[-1] = toff;
      }
#endif
   }
#ifdef ARGUSDEBUG
   ArgusDebug (6, "ArgusMalloc (%d) returning 0x%x\n", bytes, retn); 
#endif
   return (retn); 
}

void *
ArgusCalloc (int nitems, int bytes)
{
   int offset, total = nitems * bytes;
   void *retn = NULL;

   if (total) {
      if (ArgusAllocTotal++ == 0) {
#if defined(ARGUS_THREADS)
         pthread_mutex_init(&memory.lock, NULL);
#endif
      }
      ArgusAllocBytes += total;
      if (ArgusAllocMax < ArgusAllocBytes)
         ArgusAllocMax = ArgusAllocBytes;

#if defined(ARGUS_ALIGN)
      offset = ARGUS_ALIGN;
#else
      offset = 0;
#endif

#if !defined(ARGUSMEMDEBUG)
      if ((retn = calloc (1, total + offset)) == NULL)
         ArgusLog (LOG_ERR, "ArgusCalloc: malloc error %s", strerror(errno));

#else
      if ((retn = calloc (1, total + sizeof(struct ArgusMemoryHeader) + offset)) != NULL) {
         struct ArgusMemoryHeader *mem = retn;
         mem->tag = ARGUS_ALLOC;
         mem->len = total;
         mem->offset = offset;
#if defined(__GNUC__)
         mem->frame[0] = __builtin_return_address(0);
         mem->frame[1] = __builtin_return_address(1);
         mem->frame[2] = __builtin_return_address(2);
#endif

#if defined(ARGUS_THREADS)
         pthread_mutex_lock(&memory.lock);
#endif
         if (memory.start) {
            mem->nxt = memory.start;
            mem->prv = memory.start->prv;
            mem->prv->nxt = mem;
            mem->nxt->prv = mem;
            memory.end = mem;
         } else {
            memory.start = mem;
            memory.end = mem;
            mem->nxt = mem;
            mem->prv = mem;
         }
         memory.total++;
         memory.count++;
#if defined(ARGUS_THREADS)
         pthread_mutex_unlock(&memory.lock);
#endif
         retn = (void *)(mem + 1);
      }
#endif

#if defined(ARGUS_ALIGN)
      if (retn != NULL) {
         unsigned short toff;
         toff = ((unsigned long)retn & (offset - 1));
         toff = offset - toff;
         retn = (void *)((char *)retn + toff);
         ((unsigned short *)retn)[-1] = toff;
      }
#endif
   }

#ifdef ARGUSDEBUG
   ArgusDebug (6, "ArgusCalloc (%d, %d) returning 0x%x\n", nitems, bytes, retn);
#endif
   return (retn);
}


void
ArgusFree (void *buf)
{
   void *ptr = buf;

   if (ptr) {
      ArgusFreeTotal++;
#if defined(ARGUSMEMDEBUG)
      {
         struct ArgusMemoryHeader *mem = ptr;
#if defined(ARGUS_ALIGN)
         unsigned short offset = ((unsigned short *)mem)[-1];
         mem = (void *)((char *)mem - offset);
#endif
         mem--;
         if (mem->tag != ARGUS_ALLOC)
            ArgusLog (LOG_ERR, "ArgusFree: buffer error 0x%x", ptr);

#if defined(ARGUS_THREADS)
         pthread_mutex_lock(&memory.lock);
#endif
         if (memory.count == 1) {
            memory.start = NULL;
            memory.end = NULL;
         } else {
            mem->prv->nxt = mem->nxt;
            mem->nxt->prv = mem->prv;
            if (mem == memory.start) {
               memory.start = mem->nxt;
            } else if (mem == memory.end) {
               memory.end = mem->prv;
            }
         }
         ArgusAllocBytes -= mem->len;
         memory.count--;
#if defined(ARGUS_THREADS)
         pthread_mutex_unlock(&memory.lock);
#endif
         ptr = mem;
      }
#else
#if defined(ARGUS_ALIGN)
      {
         unsigned short offset;
         if ((offset = ((unsigned short *)ptr)[-1]) > 0)
            ptr = (void *)((char *)ptr - offset);
      }
#endif
#endif
      free (ptr);
   }
#ifdef ARGUSDEBUG
   if (buf != ArgusParser)
      ArgusDebug (6, "ArgusFree (%p)\n", buf);
#endif
}

/* 
   the argus malloc list is the list of free MallocLists for the system.
   these are blocks that are used to convey flow data from the modeler
   to the output processor.  They are fixed length blocks, and so no need
   to malloc and free, so just keep them in a list when they aren't being
   used.  we keep 2000 in the list when demand goes below this, and we
   start with 20, when we initialize the modeler.  no more than 1M records.

   so, when something asks for one, we take it off the list if there is
   one, and if not we just create one and return the buffer.  The buffer
   has a memory header in front so that the records can be put in the 
   list when they are freed, without corrupting the headers that were
   in the last block.  Be sure and respect that so other routines
   don't stomp on our header.
*/


#define ARGUS_MEMORY_MAX	1000000
#define ARGUS_MEMORY_HI_THRESH	2000
#define ARGUS_MEMORY_LOW_THRESH	20

struct ArgusMemoryList *ArgusMallocList = NULL;

void ArgusInitMallocList (struct ArgusParserStruct *, int);
void ArgusDeleteMallocList (struct ArgusParserStruct *);

void
ArgusInitMallocList (struct ArgusParserStruct *parser, int length)
{
   struct ArgusMemoryList *retn = NULL;
   int memlen = length + sizeof(struct ArgusMemoryHeader);

   if (ArgusMallocList != NULL) {
      if (length == ArgusMallocList->size)
         return;
      else
         ArgusLog(LOG_ERR, "ArgusInitMallocList called with multiple sizes");
   }

#if defined(ARGUS_THREADS)
   if (parser)
      pthread_mutex_lock(&parser->lock);
#endif

   if ((retn = (struct ArgusMemoryList *) ArgusCalloc(1, sizeof(*ArgusMallocList))) == NULL)
         ArgusLog(LOG_ERR, "ArgusInitMallocList ArgusCalloc %s", strerror(errno));

   retn->size = length;

#if defined(ARGUS_THREADS)
   pthread_mutex_init(&retn->lock, NULL);
   pthread_mutex_lock(&retn->lock);
#endif

   ArgusMallocList = retn;

   while (ArgusMallocList->count < ARGUS_MEMORY_LOW_THRESH) {
      struct ArgusMemoryHeader *mem;
      if ((mem = (struct ArgusMemoryHeader *) ArgusCalloc (1, memlen)) != NULL) {
         if (ArgusMallocList->end) {
            ArgusMallocList->end->nxt = mem;
         } else {
            ArgusMallocList->start = mem;
            ArgusMallocList->count = 0;
         }
         ArgusMallocList->end = mem;
         ArgusMallocList->count++;
         ArgusMallocList->total++;
      }
   }

#if defined(ARGUS_THREADS)
   pthread_mutex_unlock(&ArgusMallocList->lock);
   if (parser)
      pthread_mutex_unlock(&parser->lock);
#endif

#ifdef ARGUSDEBUG 
   ArgusDebug (6, "ArgusInitMallocList (%p, %d) returning\n", parser, length);
#endif
   return;
}

void
ArgusDeleteMallocList (struct ArgusParserStruct *parser)
{
   struct ArgusMemoryList *retn = NULL;
   struct ArgusMemoryHeader *crt, *rel;
 
   if (ArgusMallocList != NULL) {
#if defined(ARGUS_THREADS)
      pthread_mutex_lock(&ArgusMallocList->lock);
#endif
      retn = ArgusMallocList;
      ArgusMallocList = NULL;
 
      if ((crt = retn->start) != NULL) {
         while (crt != NULL) {
            rel = crt;
            crt = crt->nxt;
            ArgusFree(rel);
         }
      }
 
#if defined(ARGUS_THREADS)
      pthread_mutex_destroy(&retn->lock);
#endif
      ArgusFree(retn);
   }
}

void *
ArgusMallocListRecord (struct ArgusParserStruct *parser, int length)
{
   struct ArgusMemoryHeader *mem = NULL;
   int memlen = length + sizeof(struct ArgusMemoryHeader);
   void *retn = NULL;

   if (ArgusMallocList == NULL)
      ArgusInitMallocList(parser, length);

   if (length == ArgusMallocList->size) {
#if defined(ARGUS_THREADS)
      pthread_mutex_lock(&ArgusMallocList->lock);
#endif
      if (ArgusMallocList->start == NULL) {
         if (ArgusMallocList->total < ARGUS_MEMORY_MAX) {
            if ((mem = (struct ArgusMemoryHeader *) ArgusCalloc (1, memlen)) == NULL)
               ArgusLog(LOG_ERR, "ArgusMallocListRecord ArgusCalloc %s", strerror(errno));

            mem->len = length;
            ArgusMallocList->total++;
            ArgusMallocList->out++;

         }

      } else {
         mem = ArgusMallocList->start;
         ArgusMallocList->start = mem->nxt;
         ArgusMallocList->out++;
         ArgusMallocList->count--;

         if (ArgusMallocList->start == NULL) {
            ArgusMallocList->end = NULL;
            ArgusMallocList->count = 0;
         }
      }
#if defined(ARGUS_THREADS)
      pthread_mutex_unlock(&ArgusMallocList->lock);
#endif
   }

   if (mem != NULL)
      retn = (void *)(mem + 1);

#ifdef ARGUSDEBUG
   ArgusDebug (8, "ArgusMallocListRecord (%d) returning %p total %d out %d\n", length, retn, ArgusMallocList->total, ArgusMallocList->out);
#endif
   return (retn);
}

void
ArgusFreeListRecord (struct ArgusParserStruct *parser, void *buf)
{
   struct ArgusMemoryHeader *mem = (struct ArgusMemoryHeader *)buf;
   struct ArgusRecordStruct *rec = buf;
   struct ArgusHashTableHdr *htblhdr;
   struct ArgusQueueStruct *nsq;

   if ((htblhdr = rec->htblhdr) != NULL) {
#ifdef ARGUSDEBUG 
      ArgusDebug (5, "ArgusFreeListRecord (%p) htbldr %p\n", buf, htblhdr);
#endif
   }

   if ((nsq = rec->nsq) != NULL) {
#ifdef ARGUSDEBUG 
      ArgusDebug (5, "ArgusFreeListRecord (%p) nsq %p\n", buf, nsq);
#endif
   }

   if (rec->dsrs[ARGUS_SRCUSERDATA_INDEX] != NULL) {
      ArgusFree(rec->dsrs[ARGUS_SRCUSERDATA_INDEX]);
      rec->dsrs[ARGUS_SRCUSERDATA_INDEX] = NULL;
   }

   if (rec->dsrs[ARGUS_DSTUSERDATA_INDEX] != NULL) {
      ArgusFree(rec->dsrs[ARGUS_DSTUSERDATA_INDEX]);
      rec->dsrs[ARGUS_DSTUSERDATA_INDEX] = NULL;
   }

   mem = mem - 1;

   if (ArgusMallocList == NULL) {
      ArgusFree(mem);

   } else {
#if defined(ARGUS_THREADS)
      if (pthread_mutex_lock(&ArgusMallocList->lock) == 0) {
#endif
         if (ArgusMallocList->count < ARGUS_MEMORY_HI_THRESH) {
            mem->nxt = NULL;
            if (ArgusMallocList->end != NULL)
               ArgusMallocList->end->nxt = mem;
   
            ArgusMallocList->end = mem;
   
            if (ArgusMallocList->start == NULL)
               ArgusMallocList->start = mem;
   
            ArgusMallocList->count++;
   
         } else {
            ArgusMallocList->total--;
            ArgusFree(mem);
         }

         ArgusMallocList->in++;

#if defined(ARGUS_THREADS)
         pthread_mutex_unlock(&ArgusMallocList->lock);
      }
#endif
   }

#ifdef ARGUSDEBUG 
   ArgusDebug (5, "ArgusFreeListRecord (%p, %p) returning\n", parser, buf);
#endif
   return;
}

#include <syslog.h>

struct ArgusLogPriorityStruct {
   int priority;
   char *label;
};

#define ARGUSPRIORITYSTR   8
struct ArgusLogPriorityStruct ArgusPriorityStr[ARGUSPRIORITYSTR] =
{
   { LOG_EMERG,   "ArgusEmergency" },
   { LOG_ALERT,   "ArgusAlert" },
   { LOG_CRIT,    "ArgusCritical" },
   { LOG_ERR,     "ArgusError" },
   { LOG_WARNING, "ArgusWarning" },
   { LOG_NOTICE,  "ArgusNotice" },
   { LOG_INFO,    "ArgusInfo" },
   { LOG_DEBUG,   "ArgusDebug" },
};

extern char *print_time(struct timeval *);

void
ArgusLog (int priority, char *fmt, ...)
{
   va_list ap;
   char buf[MAXSTRLEN], *ptr = buf;
   struct timeval now;
   char *label = NULL;
   int i;

   bzero(buf, sizeof(buf));
   gettimeofday (&now, 0L);

#if defined(ARGUS_THREADS)
   if (ArgusParser)
      pthread_mutex_lock(&ArgusParser->lock);
#endif

#ifdef ARGUS_SYSLOG
#ifndef LOG_PERROR
#define LOG_PERROR      LOG_CONS
#endif
   openlog (ArgusParser->ArgusProgramName, LOG_PID | LOG_PERROR, LOG_DAEMON);
   ArgusPrintTime(ArgusParser, buf, &now);
   ptr = &buf[strlen(buf)];
   *ptr++ = ' ';
#else

   if (priority == LOG_NOTICE)
      return;

   gettimeofday (&now, 0L);

#if defined(ARGUS_THREADS)
   {
      pthread_t ptid;
      char pbuf[128];
      int i;

      bzero(pbuf, sizeof(pbuf));
      ptid = pthread_self();
      for (i = 0; i < sizeof(ptid); i++) {
         snprintf (&pbuf[i*2], 3, "%02hhx", ((char *)&ptid)[i]);
      }
      (void) sprintf (buf, "%s[%d.%s]: ", ArgusParser->ArgusProgramName, (int)getpid(), pbuf);
   }
#else
   (void) sprintf (buf, "%s[%d]: ", ArgusParser->ArgusProgramName, (int)getpid());
#endif

   ArgusPrintTime(ArgusParser, &buf[strlen(buf)], &now);
   ptr = &buf[strlen(buf)];
   *ptr++ = ' ';
#endif

#if defined(__STDC__)
   va_start(ap, fmt);
#else
   va_start(ap);
#endif

   (void) vsnprintf (ptr, (MAXSTRLEN - strlen(buf)), fmt, ap);
   va_end (ap);

   while (buf[strlen(buf) - 1] == '\n')
      buf[strlen(buf) - 1] = '\0';

   ptr = &buf[strlen(buf)];

   for (i = 0; i < ARGUSPRIORITYSTR; i++) 
      if (ArgusPriorityStr[i].priority == priority) {
         label = ArgusPriorityStr[i].label;
         break;
      }
   
   if (ArgusParser->RaCursesMode) {
      if (priority == LOG_ERR) {
         ArgusWindowClose();
         fprintf (stderr, "%s: %s", label, buf);
      } else
#if defined(ARGUS_THREADS)
#endif
         snprintf (ArgusParser->RaDebugString, 1024, "%s: %s\n", label, buf);
#if defined(ARGUS_THREADS)
#endif
   } else {
#ifdef ARGUS_SYSLOG
      if (strchr(buf, '%')) {
         char tbuf[MAXSTRLEN], *tptr = tbuf;
         int i, len = strlen(buf);
         memset(tbuf, 0, MAXSTRLEN);
         for (i = 0; i < len; i++) {
            if (buf[i] == '%') 
               *tptr++ = '%';
            *tptr++ = buf[i];
         }

         memset(buf, 0, MAXSTRLEN);
         strncpy(buf, tbuf, MAXSTRLEN);
      }
 
      syslog (priority, "%s", buf);
      closelog ();

#if defined(HAVE_SOLARIS)
      fprintf (stderr, "%s: %s", label, buf);
#endif
#else
      fprintf (stderr, "%s: %s", label, buf);
#endif
   }

#if defined(ARGUS_THREADS)
   if (ArgusParser)
      pthread_mutex_unlock(&ArgusParser->lock);
#endif

   switch (priority) {
      case LOG_ERR:
         ArgusShutDown(-1);
         break;

      case LOG_ALERT:
         ArgusParser->ArgusExitStatus = 2;
         break;

      default:
         break;
   }
}


struct timeval *
RaMinTime (struct timeval *s1, struct timeval *s2)
{
   struct timeval *retn = s2;

   if ((s1->tv_sec < s2->tv_sec) || ((s1->tv_sec == s2->tv_sec) && (s1->tv_usec < s2->tv_usec)))
      retn = s1;

   return (retn);
}


struct timeval *
RaMaxTime (struct timeval *s1, struct timeval *s2)
{
   struct timeval *retn = s2;

   if ((s1->tv_sec > s2->tv_sec) || ((s1->tv_sec == s2->tv_sec) && (s1->tv_usec > s2->tv_usec)))
      retn = s1;
  
   return (retn);
}


int
RaDiffTime (struct timeval *s1, struct timeval *s2, struct timeval *diff)
{
   int retn = 0;

   if (s1 && s2 && diff) {
      bzero ((char *)diff, sizeof(*diff));

      double v1 = (s1->tv_sec * 1.0) + (s1->tv_usec / 1000000.0);
      double v2 = (s2->tv_sec * 1.0) + (s2->tv_usec / 1000000.0);
      double f, i;

      v1 -= v2;

      f = modf(v1, &i);

      diff->tv_sec  = i;
      diff->tv_usec = f * 1000000;

      retn = 1;
   }

   return (retn);
}


#include <ctype.h>


#define ARGUS_INTERSECTS_TIME		1
#define ARGUS_SPANS_TIME		2
#define ARGUS_INCLUDES_TIME		2
#define ARGUS_CONTAINS_TIME		3

int ArgusTimeRangeStrategy = ARGUS_INTERSECTS_TIME;
int ArgusTimeRangeNegation = 0;

int
ArgusParseTimeArg (char **argp, char *args[], int ind, struct tm *tm)
{
   char buf[64], *ptr = buf, *tmp, *end = NULL;
   char *arg = *argp;
   int retn = -1;

   bzero (buf, 64);

   if (!(isdigit((int)*arg))) {
      if (*arg == 'x') {
         ArgusTimeRangeNegation = 1;
         arg++;
      }
      switch (*arg) {
         case 'i': ArgusTimeRangeStrategy = ARGUS_INTERSECTS_TIME; ptr = &buf[1]; break;  // intersects
         case 'c': ArgusTimeRangeStrategy = ARGUS_CONTAINS_TIME;   ptr = &buf[1]; break;  // contains the record
         case 'n': ArgusTimeRangeStrategy = ARGUS_SPANS_TIME;      ptr = &buf[1]; break;  // includes the specified time range
         case 's': ArgusTimeRangeStrategy = ARGUS_SPANS_TIME;      ptr = &buf[1]; break;
      }
   }

   strncpy (buf, arg, 64);
   end += strlen (buf);
   if ((tmp = strchr(arg, '+')) && (*(tmp + 1) != '\0')) {
      retn = 0;
   } else 
   if ((tmp = strchr(arg, '-')) && (*(tmp + 1) != '\0')) {
      retn = 0;
   } else {
      if (args) {
         if (args[ind] && (*args[ind] == '-')) {
            if (strlen (args[ind]) == 1) {
               retn = 0;
               if (args[ind + 1] != NULL) {
                  if (!(ArgusCheckTimeFormat (tm, args[ind + 1]))) {
                     strncat (buf, "-", (64 - strlen(buf)));
                     strncat (buf, args[ind + 1], (64 - strlen(buf)));
                     if (ArgusParser->timearg != NULL)
                        free(ArgusParser->timearg);
                     ArgusParser->timearg = strdup(buf);
                     retn = 2;
                  }
               }

            } else {
               tmp = args[ind];
               if (isdigit((int)*(tmp + 1))) {
                  strncat (buf, args[ind], (64 - strlen(buf)));
                  if (ArgusParser->timearg != NULL)
                     free(ArgusParser->timearg);
                  ArgusParser->timearg = strdup(buf);
                  retn = 1;
               } else
                  retn = 0;
            }
         } else
            retn = 0;
      }
   }

   if (ArgusCheckTimeFormat (tm, ptr))
      ArgusLog (LOG_ERR, "time syntax error %s", buf);

#ifdef ARGUSDEBUG
   ArgusDebug (5, "ArgusParseTimeArg (%s, %d, %p)\n", buf, ind, tm);
#endif

   return (retn);
}


#define ARGUS_YEAR	1
#define ARGUS_MONTH	2
#define ARGUS_DAY	3
#define ARGUS_HOUR	4
#define ARGUS_MIN	5
#define ARGUS_SEC	6

int RaDaysInAMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

int
ArgusCheckTimeFormat (struct tm *tm, char *str)
{
   int retn = 0, startfrac = 0, lastfrac = 0;
   char *ptr, buf[128];

   /* time - [time] explicit timestamp range */  
   /* time + [time] explicit timestamp with range offset */

   bzero (buf, sizeof(buf));
   strncpy (buf, str, 120);

   if ((ptr = strpbrk (buf, "smhdMy")) != NULL) {
      if (tm->tm_year == 0) {
         time_t tsec = ArgusParser->ArgusGlobalTime.tv_sec;
         localtime_r(&tsec, tm);
         bcopy ((char *)tm, (char *)&ArgusParser->RaLastFilter, sizeof(struct tm));
      } else {
         bcopy ((char *)tm, (char *)&ArgusParser->RaLastFilter, sizeof(struct tm));
      }
   }

   if (*buf == '-')
      *buf = '_';

   if (((ptr = strchr(buf, '-')) != NULL) || ((ptr = strchr(buf, '+')) != NULL)) {
      char mode  = *ptr;
      if (*buf == '_') *buf = '-';

      *ptr++ = '\0';

      while (isspace((int) buf[strlen(buf) - 1]))
         buf[strlen(buf) - 1] = 0;
      while (isspace((int) *ptr))
         ptr++;
      
      if ((retn = ArgusParseTime (ArgusParser, &ArgusParser->RaStartFilter, tm, buf, ' ', &startfrac)) > 0)
         ArgusParseTime (ArgusParser, &ArgusParser->RaLastFilter, &ArgusParser->RaStartFilter, ptr, mode, &lastfrac);

      if (retn >= 0)
         retn = 0;

   } else {

      /* this is a time stamp should be preceeded with a '-' (translated to '_' */

      int len = strlen(buf);

      if (len > 0) {
         char mode = ' ';

         if (*buf == '_')
            *buf = '-';

         bcopy ((char *)tm, (char *)&ArgusParser->RaStartFilter, sizeof(struct tm));
         bcopy ((char *)tm, (char *)&ArgusParser->RaLastFilter, sizeof(struct tm));

         if ((retn = ArgusParseTime (ArgusParser, &ArgusParser->RaStartFilter, &ArgusParser->RaLastFilter, buf, mode, &startfrac)) > 0) {
            lastfrac = startfrac;
            if (*buf != '-') {
               bcopy ((char *)&ArgusParser->RaStartFilter, (char *)&ArgusParser->RaLastFilter, sizeof(struct tm));

               if (buf[len - 1] != '.') {
                  switch (retn) {
                     case ARGUS_YEAR:  ArgusParser->RaLastFilter.tm_year++; break;
                     case ARGUS_MONTH: ArgusParser->RaLastFilter.tm_mon++; break;
                     case ARGUS_DAY:   ArgusParser->RaLastFilter.tm_mday++; break;
                     case ARGUS_HOUR:  ArgusParser->RaLastFilter.tm_hour++; break;
                     case ARGUS_MIN:   ArgusParser->RaLastFilter.tm_min++; break;
                     case ARGUS_SEC:   ArgusParser->RaLastFilter.tm_sec++; break;
                     default: break;
                  }

                  while (tm->tm_sec  > 59) {tm->tm_min++;  tm->tm_sec -= 60;} 
                  while (tm->tm_min  > 59) {tm->tm_hour++; tm->tm_min  -= 60;}
                  while (tm->tm_hour > 23) {tm->tm_mday++; tm->tm_hour -= 24;}
                  while (tm->tm_mday > RaDaysInAMonth[tm->tm_mon]) {tm->tm_mday -= RaDaysInAMonth[tm->tm_mon]; tm->tm_mon++;} 
                  while (tm->tm_mon  > 11) {tm->tm_year++; tm->tm_mon  -= 12;}
               }

            } else 
               ArgusParseTime (ArgusParser, &ArgusParser->RaLastFilter, &ArgusParser->RaStartFilter, &buf[1], '+', &lastfrac);

            retn = 0;
         }
      }
   }

   if (retn == 0) {
      ArgusParser->startime_t.tv_sec  = mktime (&ArgusParser->RaStartFilter);
      ArgusParser->startime_t.tv_usec = startfrac;
      ArgusParser->lasttime_t.tv_sec  = mktime (&ArgusParser->RaLastFilter);
      ArgusParser->lasttime_t.tv_usec = lastfrac;

      if (!((ArgusParser->lasttime_t.tv_sec  > ArgusParser->startime_t.tv_sec) ||
           ((ArgusParser->lasttime_t.tv_sec == ArgusParser->startime_t.tv_sec) &&
           ((ArgusParser->lasttime_t.tv_usec > ArgusParser->startime_t.tv_usec))))) {
         ArgusLog (LOG_ERR, "error: invalid time range startime_t %d.%06d lasttime_t %d.%06d\n",
                 (int)ArgusParser->startime_t.tv_sec, (int)ArgusParser->startime_t.tv_usec,
                 (int)ArgusParser->lasttime_t.tv_sec, (int)ArgusParser->lasttime_t.tv_usec);
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (3, "ArgusCheckTimeFormat (%p, %s) %d.%06d-%d.%06d\n", tm, str, 
                         (int)ArgusParser->startime_t.tv_sec, (int)ArgusParser->startime_t.tv_usec,
                         (int)ArgusParser->lasttime_t.tv_sec, (int)ArgusParser->lasttime_t.tv_usec);
#endif
      
   return (retn);
}


int
ArgusParseTime (struct ArgusParserStruct *parser, struct tm *tm, struct tm *ctm, char *buf, char mode, int *frac)
{
   char *hptr = NULL, *dptr = NULL, *mptr = NULL, *yptr = NULL, *pptr = NULL;
   char *minptr = NULL, *secptr = NULL, *ptr;
   char strbuf[128], *str = strbuf;
   int retn = 0, year = 0, month = 0, day = 0, hour = 0, mins = 0, sec = 0, sign = 1;
   time_t thistime;
   double i;

   /*[[[yyyy/]mm/]dd].]hh[:mm[:ss]]*/
   /* yyyy/mm */
   /* yyyy */
   /* %d[yMdhms] */
   /* %d[yMdhms][[+]%d[yMdhms]] explict time range */
   /* -%d[yMdhms] explicit time range ending with now time in the range */

   bzero(str, sizeof(strbuf));
   strncpy(str, buf, sizeof(strbuf));

   if (!(isdigit((int)*str)) && !(*str == '-') && !(*str == '*')) {
      retn = -1;
   } else {
      if ((ptr = strpbrk (str, "yMdhms")) != NULL) {
         int status = 0;

         if (mode == ' ') {
            if (tm != &ArgusParser->RaLastFilter)
               bcopy ((u_char *) ctm, (u_char *) tm, sizeof (struct tm));
         } else
            bcopy ((u_char *) ctm, (u_char *) tm, sizeof (struct tm));

         thistime = mktime (tm);

         do {
            int wildcard = 0;
            char *endptr;

            if (*str == '*') {
               wildcard++;
               switch (*ptr) {
                  case 'y': i = 1970; status |= (1 << RAWILDCARDYEAR); break;
                  case 'M': i =    0; status |= (1 << RAWILDCARDMONTH); break;
                  case 'd': i =    1; status |= (1 << RAWILDCARDDAY); break;
                  case 'h': i =    0; status |= (1 << RAWILDCARDHOUR); break;
                  case 'm': i =    0; status |= (1 << RAWILDCARDMIN); break;
                  case 's': i =    0; status |= (1 << RAWILDCARDSEC); break;
               }
               parser->RaWildCardDate = status;
               
            } else  {
               i = strtod(str, &endptr);
               if (endptr == str)
                  ArgusLog (LOG_ERR, "time syntax error %s", parser->timearg);
            }

            if ((i >= 0) && (mode == ' ')) {
               switch (*ptr) {
                  case 'y': tm->tm_year = (i - 1900); retn = ARGUS_YEAR; break;
                  case 'M': tm->tm_mon = (i - 1); retn = ARGUS_MONTH; break;
                  case 'd': tm->tm_mday = i; retn = ARGUS_DAY; break;
                  case 'h': tm->tm_hour = i; retn = ARGUS_HOUR; break;
                  case 'm': tm->tm_min = i; retn = ARGUS_MIN; break;
                  case 's': tm->tm_sec = i; retn = ARGUS_SEC; break;
               }

            } else {
               if (tm != &ArgusParser->RaLastFilter)
                  i++;

               if (wildcard)
                  ArgusLog (LOG_ERR, "time syntax error %s", parser->timearg);

               switch (mode) {
                  case '-': sign = -1; break;
                  case '+': break;
               }

               switch (*ptr) {
                  case 'y': tm->tm_year += (i * sign); retn = ARGUS_YEAR; break;

                  case 'M': {
                     while (i > tm->tm_mon) {
                        tm->tm_year += 1 * sign;
                        i -= 12;
                     }
                     tm->tm_mon += i * sign;
                     thistime = mktime (tm);
                     retn = ARGUS_MONTH;
                     break;
                  }

                  case 'd':
                     thistime += (i * ((60 * 60) * 24)) * sign;
                     localtime_r (&thistime, tm);
                     retn = ARGUS_DAY;
                     break;

                  case 'h':
                     thistime += (i * (60 * 60)) * sign;
                     localtime_r (&thistime, tm);
                     retn = ARGUS_HOUR;
                     break;

                  case 'm':
                     thistime += (i * 60) * sign;
                     localtime_r (&thistime, tm);
                     retn = ARGUS_MIN;
                     break;

                  case 's':
                     thistime += i * sign;
                     localtime_r (&thistime, tm);
                     retn = ARGUS_SEC;
                     break;

                  default:
                     retn = -1;
                     break;
               }
            }

            if (retn >= 0) {
               str = ptr + 1;
               if ((!(isdigit((int)*str))) && !(*str == '*'))
                  break;
            } else
               break;

         } while ((ptr = strpbrk (str, "yMdhms")) != NULL);

         switch (retn) {
            case ARGUS_YEAR:   tm->tm_mon  = 0;
            case ARGUS_MONTH:  tm->tm_mday = 1;
            case ARGUS_DAY:    tm->tm_hour = 0;
            case ARGUS_HOUR:   tm->tm_min  = 0;
            case ARGUS_MIN:    tm->tm_sec  = 0;
            case ARGUS_SEC:    break;
         }

         if ((retn >= 0) && (sign < 0)) {
            struct tm tmbuf;
            bcopy ((u_char *) ctm, (u_char *)&tmbuf, sizeof (struct tm));
            bcopy ((u_char *) tm, (u_char *) ctm, sizeof (struct tm));
            bcopy ((u_char *)&tmbuf, (u_char *) tm, sizeof (struct tm));
         }
         
      } else {
         int status = parser->RaWildCardDate;

         bcopy ((u_char *) ctm, (u_char *) tm, sizeof (struct tm));
         year  = tm->tm_year;
         month = tm->tm_mon;
         day   = tm->tm_mday;
         hour  = tm->tm_hour;
         mins  = tm->tm_min;
         sec   = tm->tm_sec;

#if defined(HAVE_STRUCT_TM_TM_ZONE)
         tm->tm_zone = NULL;
         tm->tm_gmtoff = 0;
#endif
         thistime = mktime (tm);

         if ((hptr = strchr (str, '.')) != NULL) {
            if ((hptr - str) != (strlen(str) - 1)) {
               *hptr++ = '\0';
               if (!(isdigit((int)*hptr)) && !(*hptr == '*'))
                  return -1;
            } else {
               *hptr = '\0';
               pptr = hptr;
               hptr = NULL;
            }
         }
      
         if ((dptr = strrchr (str, '/')) != NULL) {  /* mm/dd  || yyyy/mm  || yyyy/mm/dd */
                                                     /*   ^   */
            *dptr++ = '\0';
            if ((mptr = strrchr (str, '/')) != NULL) {  /* yyyy/mm/dd */
               *mptr++ = '\0';
               yptr = str;

            } else {
               if (strlen(str) == 4) {
                  yptr = str;
                  mptr = dptr;
                  dptr =  NULL;
                  tm->tm_mday = 1;
               } else
                  mptr = str;
            }

         } else {
            if (hptr != NULL)
               dptr = str;
            else {
               int value = atoi(str);
               if ((value > 1900) && (value < (tm->tm_year + 1900))) {
                  yptr = str;
                  hour = 0;
               } else
                  hptr = str;
            }
         }
      
         if (yptr) {
            if (strlen(yptr) != 4)
               return -1;

            for (ptr = yptr, i = 0; i < strlen(yptr); i++) {
               if (*ptr == '*') {
                  status |= 1 << RAWILDCARDYEAR;
                  break;
               }
               if (!(isdigit((int)*ptr++)))
                  return -1;
            }

            if (!(status & (1 << RAWILDCARDYEAR))) {
               tm->tm_year = atoi(yptr) - 1900;
               retn = ARGUS_YEAR;
            } else
               tm->tm_year = 70;
            year = tm->tm_year;
         }

         if (mptr) {
            if (strlen(mptr) != 2)
               return -1;
            for (ptr = mptr, i = 0; i < strlen(mptr); i++) {
               if (*ptr == '*') {
                  status |= 1 << RAWILDCARDMONTH;
                  break;
               }
               if (!(isdigit((int)*ptr++)))
                  return -1;
            }
            if (!(status & (1 << RAWILDCARDMONTH))) {
               tm->tm_mon  = atoi(mptr) - 1;
               retn = ARGUS_MONTH;
            } else 
               tm->tm_mon  = 0;
            month = tm->tm_mon;
         }
      
         if (dptr) {
            if (strlen(dptr) != 2)
               return -1;
            for (ptr = dptr, i = 0; i < strlen(dptr); i++) {
               if (*ptr == '*') {
                  status |= 1 << RAWILDCARDDAY;
                  break;
               }
               if (!(isdigit((int)*ptr++)))
                  return -1;
            }
            if (!(status & (1 << RAWILDCARDDAY))) {
               tm->tm_mday = atoi(dptr);
               retn = ARGUS_DAY;
            } else
               tm->tm_mday = 1;
            day = tm->tm_mday;
         }
      
         if (hptr) {
            if ((pptr = strchr (hptr, '.')) != NULL) {
               char *tptr = pptr + 1;
               float scale = 1000000.0;
               *pptr = '\0';

                while(isdigit(*tptr++)) scale /= 10.0;
               *frac = atoi(&pptr[1]) * scale;
            }
            if ((minptr = strchr (hptr, ':')) != NULL) {
               *minptr++ = '\0';
               if ((secptr = strchr (minptr, ':')) != NULL) {
                  *secptr++ = '\0';
               }
            }

            for (ptr = hptr, i = 0; i < strlen(hptr); i++) {
               if (*ptr == '*') {
                  status |= 1 << RAWILDCARDHOUR;
                  break;
               }
               if (!(isdigit((int)*ptr++)))
                  return -1;
            }
      
            if (!(status & (1 << RAWILDCARDHOUR))) {
               hour = atoi(hptr);
               if (hour < 24) {
                  retn = ARGUS_HOUR;
                  hptr = hptr;
               }
            } else
               hour = 0;

            if (minptr != NULL) {
               for (ptr = minptr, i = 0; i < strlen(minptr); i++) {
                  if (*ptr == '*') {
                     status |= 1 << RAWILDCARDMIN;
                     break;
                  }
                  if (!(isdigit((int)*ptr++)))
                     return -1;
               }
      
               if (!(status & (1 << RAWILDCARDMIN))) {
                  mins = atoi(minptr);
                  retn = ARGUS_MIN;
               } else
                  mins = 0;
            }
      
            if (secptr != NULL) {
               for (ptr = secptr, i = 0; i < strlen(secptr); i++) {
                  if (*ptr == '*') {
                     status |= 1 << RAWILDCARDSEC;
                     break;
                  }
                  if (!(isdigit((int)*ptr++)))
                     return -1;
               }

               if (!(status & (1 << RAWILDCARDSEC))) {
                  sec = atoi(secptr);
                  retn = ARGUS_SEC;
               } else
                  sec = 0;
            }
         }

         switch (retn) {
            case ARGUS_YEAR:   tm->tm_mon  = month = 0;
            case ARGUS_MONTH:  tm->tm_mday = day = 1;
            case ARGUS_DAY:    tm->tm_hour = hour = 0;
            case ARGUS_HOUR:   tm->tm_min  = mins = 0;
            case ARGUS_MIN:    tm->tm_sec  = sec = 0;
            case ARGUS_SEC:    break;
         }

         if (hour > 24) {
            time_t value = hour;
            bzero(tm, sizeof(*tm));
            localtime_r (&value, tm);
            year  = tm->tm_year;
            month = tm->tm_mon;
            day   = tm->tm_mday;
            hour  = tm->tm_hour;
            mins  = tm->tm_min;
            sec   = tm->tm_sec;
            retn  = 1;
            status = 0;

         } else {
            tm->tm_hour = hour;
            tm->tm_min  = mins;
            tm->tm_sec  = sec;
      
            if (tm->tm_year < 0)
               retn = -1;
            if ((tm->tm_mon > 11) || (tm->tm_mon < 0))
               retn = -1;
            if ((tm->tm_mday > 31) || (tm->tm_mday < 1))
               retn = -1;
            if ((tm->tm_hour > 23) || (tm->tm_hour < 0))
               retn = -1;
            if ((tm->tm_min > 60) || (tm->tm_min < 0))
               retn = -1;
            if ((tm->tm_sec > 60) || (tm->tm_sec < 0))
               retn = -1;
         }

         parser->RaWildCardDate = status;

         if (retn >= 0) {
#if defined(HAVE_STRUCT_TM_TM_ZONE)
            tm->tm_isdst  = 0;
            tm->tm_gmtoff = 0;
            tm->tm_zone   = 0;
#endif
            thistime = mktime (tm);

#if defined(HAVE_STRUCT_TM_TM_ZONE)
            if (tm->tm_zone != NULL) {
               char *tmzone = strdup(tm->tm_zone);
               localtime_r (&thistime, tm);
               if (strncpy(tmzone, tm->tm_zone, strlen(tmzone))) {
                  tm->tm_year = year;
                  tm->tm_mon  = month;
                  tm->tm_mday = day;
                  tm->tm_hour = hour;
                  thistime    = mktime (tm);
               }
               free(tmzone);
            }
#endif
         }

         if (pptr != NULL)
            *pptr = '.';
      }

      if (!(parser->RaWildCardDate))
         ArgusParser->RaExplicitDate = 1;
   }

#ifdef ARGUSDEBUG
   {
      char *rstr;
      switch (retn) {
         case ARGUS_YEAR:  rstr = "year"; break;
         case ARGUS_MONTH: rstr = "mon"; break;
         case ARGUS_DAY:   rstr = "day"; break;
         case ARGUS_HOUR:  rstr = "hour"; break;
         case ARGUS_MIN:   rstr = "min"; break;
         case ARGUS_SEC:   rstr = "sec"; break;
      }

      ArgusDebug (3, "ArgusParseTime (%p, %p, %p, \"%s\", '%c', 0.%06d) retn %s(%d)\n", parser, tm, ctm, buf, mode, *frac, rstr, thistime);
   }
#endif
   return (retn);
}


int
ArgusCheckTime (struct ArgusParserStruct *parser, struct ArgusRecordStruct *ns)
{
   struct timeval start, last, pstart, plast;
   struct tm tmbuf, *tm;
   int retn = 0;

   if ((ns->hdr.type & 0xF0) == ARGUS_MAR) {
      struct ArgusRecord *rec = (void *)ns->dsrs[0];
      if (rec != NULL) {
         start.tv_sec  = rec->argus_mar.startime.tv_sec;
         start.tv_usec = rec->argus_mar.startime.tv_usec;

         last.tv_sec   = rec->argus_mar.now.tv_sec;
         last.tv_usec  = rec->argus_mar.now.tv_usec;
      } else {
         bzero(&start, sizeof(start));
         bzero(&last,  sizeof(last));
      }

   } else {
      struct ArgusTimeObject *dtime;
      if ((dtime = (struct ArgusTimeObject *) ns->dsrs[ARGUS_TIME_INDEX]) != NULL) {
         struct timeval sst, sdt, dst, ddt;

         start.tv_sec = 0x7FFFFFFF, start.tv_usec = 0;
         bzero(&last,  sizeof(last));

         sst.tv_sec  = dtime->src.start.tv_sec;
         sst.tv_usec = dtime->src.start.tv_usec;
         sdt.tv_sec  = dtime->src.end.tv_sec;
         sdt.tv_usec = dtime->src.end.tv_usec;
         dst.tv_sec  = dtime->dst.start.tv_sec;
         dst.tv_usec = dtime->dst.start.tv_usec;
         ddt.tv_sec  = dtime->dst.end.tv_sec;
         ddt.tv_usec = dtime->dst.end.tv_usec;

         if (sst.tv_sec && ((start.tv_sec  > sst.tv_sec) || 
                           ((start.tv_sec == sst.tv_sec) &&
                            (start.tv_usec > sst.tv_usec))))
            start = sst;
         if (sdt.tv_sec && ((start.tv_sec  > sdt.tv_sec) || 
                           ((start.tv_sec == sdt.tv_sec) &&
                            (start.tv_usec > sdt.tv_usec))))
            start = sdt;
         if (dst.tv_sec && ((start.tv_sec  > dst.tv_sec) || 
                           ((start.tv_sec == dst.tv_sec) &&
                            (start.tv_usec > dst.tv_usec))))
            start = dst;
         if (ddt.tv_sec && ((start.tv_sec  > ddt.tv_sec) || 
                           ((start.tv_sec == ddt.tv_sec) &&
                            (start.tv_usec > ddt.tv_usec))))
            start = ddt;

         if (sst.tv_sec && ((last.tv_sec  < sst.tv_sec) ||
                           ((last.tv_sec == sst.tv_sec) &&
                            (last.tv_usec < sst.tv_usec))))
            last = sst;
         if (sdt.tv_sec && ((last.tv_sec  < sdt.tv_sec) ||
                           ((last.tv_sec == sdt.tv_sec) &&
                            (last.tv_usec < sdt.tv_usec))))
            last = sdt;
         if (dst.tv_sec && ((last.tv_sec  < dst.tv_sec) ||
                           ((last.tv_sec == dst.tv_sec) &&
                            (last.tv_usec < dst.tv_usec))))
            last = dst;
         if (ddt.tv_sec && ((last.tv_sec  < ddt.tv_sec) ||
                           ((last.tv_sec == ddt.tv_sec) &&
                            (last.tv_usec < ddt.tv_usec))))
            last = ddt;
      }
   }

   if ((parser->RaStartTime.tv_sec  > start.tv_sec) || 
      ((parser->RaStartTime.tv_sec == start.tv_sec) &&
       (parser->RaStartTime.tv_usec > start.tv_usec))) {
 
      parser->RaStartTime.tv_sec  = start.tv_sec;
      parser->RaStartTime.tv_usec = start.tv_usec;
   }
 
   if ((parser->RaEndTime.tv_sec  < last.tv_sec) || 
      ((parser->RaEndTime.tv_sec == last.tv_sec) &&
       (parser->RaEndTime.tv_usec < last.tv_usec))) {
 
      parser->RaEndTime.tv_sec  = last.tv_sec;
      parser->RaEndTime.tv_usec = last.tv_usec;
   }

   if ((ns->hdr.type & 0xF0) == ARGUS_MAR)
      parser->ArgusGlobalTime = last;
   else
      parser->ArgusGlobalTime = start;

   gettimeofday (&parser->ArgusRealTime, 0L);
   ArgusAdjustGlobalTime (parser, &parser->ArgusRealTime);
 
   if (parser->tflag) {
#if defined(HAVE_STRUCT_TM_TM_ZONE)
      int tm_gmtoff = 0;
#endif
      time_t tsec;

      bzero((char *)&pstart, sizeof(pstart));
      bzero((char *)&plast, sizeof(plast));

      if (!parser->RaExplicitDate) {
         char *timearg = parser->timearg;

         tsec = start.tv_sec;
         tm = localtime_r(&tsec, &tmbuf);

#if defined(HAVE_STRUCT_TM_TM_ZONE)
         tm_gmtoff = tm->tm_gmtoff;
#endif
         if ((!isdigit((int)*timearg)) && (*timearg != '*')) timearg++;

         if (parser->RaWildCardDate) {
            struct tm stmbuf,  *stm;
            struct tm ltmbuf,  *ltm;
            int i;

            tsec = start.tv_sec;
            stm  = localtime_r (&tsec, &stmbuf);

            tsec = last.tv_sec;
            ltm  = localtime_r (&tsec, &ltmbuf);

            for (i = 0; i < RAMAXWILDCARDFIELDS; i++) {
               if (parser->RaWildCardDate & (1 << i)) {
                  switch (i) {
                     case RAWILDCARDYEAR: {
                        stm->tm_year = 70; ltm->tm_year = 70;
                        break;
                     }
                     case RAWILDCARDMONTH: {
                        stm->tm_mon = 0; ltm->tm_mon = 0;
                        break;
                     }
                     case RAWILDCARDDAY: {
                        stm->tm_mday = 1; ltm->tm_mday = 1;
                        break;
                     }
                     case RAWILDCARDHOUR: {
                        stm->tm_hour = 0; ltm->tm_hour = 0;
                        break;
                     }
                     case RAWILDCARDMIN: {
                        stm->tm_min = 0; ltm->tm_min = 0;
                        break;
                     }
                     case RAWILDCARDSEC: {
                        stm->tm_sec = 0; ltm->tm_sec = 0;
                        break;
                     }
                  }
               }
            }
#if defined(HAVE_STRUCT_TM_TM_ZONE)
            stm->tm_zone = NULL;
            ltm->tm_zone = NULL;
#endif
            start.tv_sec = mktime (stm);
            last.tv_sec  = mktime (ltm);

#if defined(HAVE_STRUCT_TM_TM_ZONE)
            if ((tm_gmtoff -= stm->tm_gmtoff) != 0) {
               start.tv_sec += tm_gmtoff;
               last.tv_sec  += tm_gmtoff;
            }
#endif
         }
      }

      pstart = parser->startime_t;
      plast  = parser->lasttime_t;

      if ((ns->hdr.type & 0xF0) == ARGUS_MAR) {
         if ((ns->hdr.cause & 0xF0) == ARGUS_START) {
            if ((start.tv_sec >= pstart.tv_sec) && (last.tv_sec <= plast.tv_sec))
               retn++;
         } else {
            if ((last.tv_sec >= pstart.tv_sec) && (last.tv_sec <= plast.tv_sec))
               retn++;
         }
      } else {

// here we want to provide flexibility in the time matching
// we should support all the set operations that make sense
//    1. the records are completely in the range
//    2. the records intersect in any way
//    3. the records overlap or span the range
//    4. negaton of the tests

// #define ARGUS_INTERSECTS_TIME           1
// #define ARGUS_SPANS_TIME                2
// #define ARGUS_INCLUDES_TIME             2
// #define ARGUS_CONTAINS_TIME             3

         switch (ArgusTimeRangeStrategy) {
            case ARGUS_INTERSECTS_TIME:  // the record intersects the range in any way
               if ((((pstart.tv_sec  < start.tv_sec) ||
                    ((pstart.tv_sec == start.tv_sec) && (pstart.tv_usec <= start.tv_usec))) &&
                    ((plast.tv_sec  > start.tv_sec) ||
                    ((plast.tv_sec == start.tv_sec) && (plast.tv_usec >= start.tv_usec)))) ||

                   (((pstart.tv_sec  < last.tv_sec) ||
                    ((pstart.tv_sec == last.tv_sec) && (pstart.tv_usec < last.tv_usec))) &&
                    ((plast.tv_sec  >  last.tv_sec) ||
                    ((plast.tv_sec ==  last.tv_sec) && (plast.tv_usec >=  last.tv_usec)))))

                  retn++;
               break;

            case ARGUS_CONTAINS_TIME:  // the record is completely within the filter time range
               if (((pstart.tv_sec  < start.tv_sec) ||
                   ((pstart.tv_sec == start.tv_sec) && (pstart.tv_usec <= start.tv_usec))) &&
                   ((plast.tv_sec  > last.tv_sec) ||
                   ((plast.tv_sec == last.tv_sec) && (plast.tv_usec >= last.tv_usec))))
                  retn++;
               break;

            case ARGUS_SPANS_TIME: {   // the filter time is completely within the argus record
               if (((start.tv_sec < pstart.tv_sec) || ((start.tv_sec == pstart.tv_sec) && (start.tv_usec <= pstart.tv_usec))) &&
                   ((last.tv_sec > plast.tv_sec) || ((last.tv_sec == plast.tv_sec) && (last.tv_usec >= plast.tv_usec))))
                  retn++;
            }
         }
      }

   } else
      retn++;

   return (ArgusTimeRangeNegation ? (retn ? 0 : 1) : retn);
}


int
ArgusGenerateCanonRecord (struct ArgusRecordStruct *argus)
{
   int retn = 1;
/*
   int i, ind = 0;
   struct ArgusDSRHeader **dsrs = NULL;
   struct ArgusCanonRecord *canon = &argus->canon;

   if (!(argus->hdr.type & ARGUS_MAR)) {
      bcopy ((char *)&argus->hdr, (char *)&canon->hdr, sizeof(canon->hdr));
   
      for (i = 0; i < ARGUSMAXDSRTYPE; i++) {
         ind = (1 << i);
         switch (ind) {
            case ARGUS_FLOW_INDEX:
               if (argus->dsrindex & (0x1 << ARGUS_FLOW_INDEX))
                  bcopy((char *) dsrs[ARGUS_FLOW_INDEX], (char *)&canon->flow, dsrs[ARGUS_FLOW_INDEX]->argus_dsrvl8.len);
               break;
            case ARGUS_TIME_INDEX:
               if (argus->dsrindex & (0x1 << ARGUS_TIME_INDEX))
                  bcopy((char *) dsrs[ARGUS_TIME_INDEX], (char *)&canon->time, dsrs[ARGUS_TIME_INDEX]->argus_dsrvl8.len);
               break;
            case ARGUS_TRANSPORT_INDEX:   
               if (argus->dsrindex & (0x1 << ARGUS_TRANSPORT_INDEX))
                  bcopy((char *) dsrs[ARGUS_TRANSPORT_INDEX], (char *)&canon->trans, dsrs[ARGUS_TRANSPORT_INDEX]->argus_dsrvl8.len);
               break;
            case ARGUS_METRIC_INDEX:   
               if (argus->dsrindex & (0x1 << ARGUS_METRIC_INDEX))
                  bcopy((char *) dsrs[ARGUS_METRIC_INDEX], (char *)&canon->metric, dsrs[ARGUS_METRIC_INDEX]->argus_dsrvl8.len);
               break;
            case ARGUS_NETWORK_INDEX:   
               if (argus->dsrindex & (0x1 << ARGUS_NETWORK_INDEX))
                  bcopy((char *) dsrs[ARGUS_NETWORK_INDEX], (char *)&canon->net, dsrs[ARGUS_NETWORK_INDEX]->argus_dsrvl8.len);
               break;
         }
      }
   }
*/

#ifdef ARGUSDEBUG
   ArgusDebug (7, "ArgusGenerateCanonRecord (%p) returning\n", argus);
#endif

   return (retn);
}


unsigned int ArgusIndexV2Record (struct ArgusV2Record *, struct ArgusV2FarHeaderStruct **);
unsigned char *ArgusConvertRecord (struct ArgusInput *, char *);


unsigned char *
ArgusConvertRecord (struct ArgusInput *input, char *ptr)
{
   if (input->ArgusConvBuffer == NULL) {
      if ((input->ArgusConvBuffer = (u_char *)ArgusCalloc (1, MAXARGUSRECORD)) == NULL)
         ArgusLog (LOG_ERR, "ArgusCalloc error %s", strerror(errno));
   }

   switch (input->type) {
      case ARGUS_V2_DATA_SOURCE: {
         struct ArgusV2Record *argus2 = (struct ArgusV2Record *) ptr;
         struct ArgusRecord *argus = (struct ArgusRecord *)input->ArgusConvBuffer;
#ifdef _LITTLE_ENDIAN
         ArgusV2NtoH(argus2);
#endif
         if (argus2->ahdr.type & ARGUS_V2_MAR) {
            argus->hdr.type   = (ARGUS_MAR | ARGUS_VERSION);
            switch (argus2->ahdr.cause) {
               case ARGUS_V2_START:    argus->hdr.cause = ARGUS_START; break;
               case ARGUS_V2_STATUS:   argus->hdr.cause = ARGUS_STATUS; break;
               case ARGUS_V2_STOP:     argus->hdr.cause = ARGUS_STOP; break;
               case ARGUS_V2_SHUTDOWN: argus->hdr.cause = ARGUS_SHUTDOWN; break;
               case ARGUS_V2_TIMEOUT:  argus->hdr.cause = ARGUS_TIMEOUT; break;
               case ARGUS_V2_ERROR:    argus->hdr.cause = ARGUS_ERROR; break;
            }
            argus->hdr.len    = (unsigned short) sizeof(struct ArgusRecord)/4;

            argus->argus_mar.status            = argus2->ahdr.status;
            if (argus->hdr.cause == ARGUS_START) {
               argus->argus_mar.thisid         = argus2->argus_mar.argusid;
               argus->argus_mar.argusid        = ARGUS_COOKIE;
            } else
               argus->argus_mar.argusid        = argus2->argus_mar.argusid;

            argus->argus_mar.startime          = argus2->argus_mar.startime;
            argus->argus_mar.now               = argus2->argus_mar.now;

            argus->argus_mar.major_version     = VERSION_MAJOR;
            argus->argus_mar.minor_version     = VERSION_MINOR;
            argus->argus_mar.reportInterval    = argus2->argus_mar.reportInterval;
            argus->argus_mar.argusMrInterval   = argus2->argus_mar.argusMrInterval;

            argus->argus_mar.localnet          = argus2->argus_mar.localnet;
            argus->argus_mar.netmask           = argus2->argus_mar.netmask;

            argus->argus_mar.nextMrSequenceNum = argus2->argus_mar.nextMrSequenceNum;

            argus->argus_mar.pktsRcvd          = argus2->argus_mar.pktsRcvd;
            argus->argus_mar.bytesRcvd         = argus2->argus_mar.bytesRcvd;
            argus->argus_mar.record_len        = argus2->argus_mar.record_len;

            ArgusHtoN(argus);

         } else {
            struct ArgusV2FarHeaderStruct *hdrs[32];
            unsigned int ArgusThisFarStatus = ArgusIndexV2Record (argus2, hdrs);
            struct ArgusDSRHeader *dsr = (struct ArgusDSRHeader *) argus;

            int i, ind, length;

            argus->hdr.type  = (ARGUS_FAR | ARGUS_VERSION);
            switch (argus2->ahdr.cause) {
               case ARGUS_V2_START:  argus->hdr.cause = ARGUS_START; break;
               case ARGUS_V2_STATUS: argus->hdr.cause = ARGUS_STATUS; break;
               case ARGUS_V2_STOP:   argus->hdr.cause = ARGUS_STOP; break;
            }

            argus->hdr.len = 1;
            dsr++;

            for (i = 0; i < 32; i++) {
               ind = (1 << i);
               if (ArgusThisFarStatus & ind) {
                  switch (ind) {
                     case ARGUS_V2_FAR_DSR_STATUS: {
                        struct ArgusV2FarStruct  *far = (struct ArgusV2FarStruct *)hdrs[ARGUS_V2_FAR_DSR_INDEX];
                        struct ArgusTransportStruct *trans = (struct ArgusTransportStruct *) dsr;
                        struct ArgusIPAttrStruct ipattrbuf, *ipattr = NULL;
                        struct ArgusFlow *flow = NULL;
                        struct ArgusTimeObject *dtime = NULL;
                        struct ArgusMetricStruct *metric = NULL;

                        long long *ptr;

                        trans->hdr.type               = ARGUS_TRANSPORT_DSR;
                        trans->hdr.subtype            = ARGUS_SEQ | ARGUS_SRCID;
                        trans->hdr.argus_dsrvl8.qual  = ARGUS_TYPE_IPV4;
                        trans->hdr.argus_dsrvl8.len   = 3;
                        trans->srcid.a_un.ipv4        = argus2->ahdr.argusid;
                        trans->seqnum                 = argus2->ahdr.seqNumber;

                        dsr += trans->hdr.argus_dsrvl8.len;
                        argus->hdr.len += trans->hdr.argus_dsrvl8.len;
                        flow = (struct ArgusFlow *) dsr;

                        flow->hdr.type               = ARGUS_FLOW_DSR;
                        flow->hdr.subtype            = ARGUS_FLOW_CLASSIC5TUPLE;

                        switch (argus2->ahdr.status & 0xFFFF) {
                           case ETHERTYPE_IP:
                              flow->hdr.argus_dsrvl8.qual = ARGUS_TYPE_IPV4;

                              ipattr = &ipattrbuf;
                              bzero (ipattr, sizeof(*ipattr));

                              ipattr->hdr.type               = ARGUS_IPATTR_DSR;
                              ipattr->hdr.argus_dsrvl8.len   = 1;

                              if (far->src.count) {
                                 ipattr->hdr.argus_dsrvl8.qual |= ARGUS_IPATTR_SRC;
                                 ipattr->hdr.argus_dsrvl8.len++;
                                 switch (far->flow.flow_union.ip.ip_p) {
                                    default:
                                    case IPPROTO_UDP:
                                    case IPPROTO_TCP:
                                       ipattr->src.ip_id = far->flow.flow_union.ip.ip_id;
                                       break;
                                    case IPPROTO_ICMP:
                                       ipattr->src.ip_id = far->flow.flow_union.icmp.ip_id;
                                       break;
                                    case IPPROTO_IGMP:
                                       ipattr->src.ip_id = far->flow.flow_union.igmp.ip_id;
                                       break;
                                 }
                                 ipattr->src.ttl = far->attr_ip.sttl;
                                 ipattr->src.tos = far->attr_ip.stos;

                                 if (far->attr_ip.soptions) {
                                    if (far->attr_ip.soptions & ARGUS_V2_FRAGMENTS) {
                                       ipattr->hdr.argus_dsrvl8.qual |= ARGUS_IPATTR_SRC_FRAGMENTS;
                                       flow->hdr.argus_dsrvl8.qual |= ARGUS_FRAGMENT;
                                       far->attr_ip.soptions &= ~ARGUS_V2_FRAGMENTS;
                                    }
                                    if (far->attr_ip.soptions & ARGUS_V2_TIMESTAMP) ipattr->src.options   |= ARGUS_TIMESTAMP;
                                    if (far->attr_ip.soptions & ARGUS_V2_SECURITY)  ipattr->src.options   |= ARGUS_SECURITY;
                                    if (far->attr_ip.soptions & ARGUS_V2_LSRCROUTE) ipattr->src.options   |= ARGUS_LSRCROUTE;
                                    if (far->attr_ip.soptions & ARGUS_V2_SSRCROUTE) ipattr->src.options   |= ARGUS_SSRCROUTE;
                                    if (far->attr_ip.soptions & ARGUS_V2_RECORDROUTE) ipattr->src.options |= ARGUS_RECORDROUTE;
                                    if (far->attr_ip.soptions & ARGUS_V2_SATNETID) ipattr->src.options    |= ARGUS_SATID;
                                    if (ipattr->src.options) {
                                       ipattr->hdr.argus_dsrvl8.qual |= ARGUS_IPATTR_SRC_OPTIONS;
                                       ipattr->hdr.argus_dsrvl8.len++;
                                    }
                                 }
                              }

                              if (far->dst.count) {
                                 ipattr->hdr.argus_dsrvl8.qual |= ARGUS_IPATTR_DST;
                                 ipattr->hdr.argus_dsrvl8.len++;
                                 switch (far->flow.flow_union.ip.ip_p) {
                                    default:
                                    case IPPROTO_UDP:
                                    case IPPROTO_TCP:
                                       ipattr->dst.ip_id = far->flow.flow_union.ip.ip_id;
                                       break;
                                    case IPPROTO_ICMP:
                                       ipattr->dst.ip_id = far->flow.flow_union.icmp.ip_id;
                                       break;
                                    case IPPROTO_IGMP:
                                       ipattr->dst.ip_id = far->flow.flow_union.igmp.ip_id;
                                       break;
                                 }
                                 ipattr->dst.ttl = far->attr_ip.dttl;
                                 ipattr->dst.tos = far->attr_ip.dtos;
   
                                 if (far->attr_ip.doptions) {
                                    if (far->attr_ip.doptions & ARGUS_V2_FRAGMENTS) {
                                       ipattr->hdr.argus_dsrvl8.qual |= ARGUS_IPATTR_DST_FRAGMENTS;
                                       flow->hdr.argus_dsrvl8.qual |= ARGUS_FRAGMENT;
                                       far->attr_ip.doptions &= ~ARGUS_V2_FRAGMENTS;
                                    }
                                    if (far->attr_ip.doptions & ARGUS_V2_TIMESTAMP) ipattr->dst.options   |= ARGUS_TIMESTAMP;
                                    if (far->attr_ip.doptions & ARGUS_V2_SECURITY)  ipattr->dst.options   |= ARGUS_SECURITY;
                                    if (far->attr_ip.doptions & ARGUS_V2_LSRCROUTE) ipattr->dst.options   |= ARGUS_LSRCROUTE;
                                    if (far->attr_ip.doptions & ARGUS_V2_SSRCROUTE) ipattr->dst.options   |= ARGUS_SSRCROUTE;
                                    if (far->attr_ip.doptions & ARGUS_V2_RECORDROUTE) ipattr->dst.options |= ARGUS_RECORDROUTE;
                                    if (far->attr_ip.doptions & ARGUS_V2_SATNETID) ipattr->dst.options    |= ARGUS_SATID;
                                    if (ipattr->dst.options) {
                                       ipattr->hdr.argus_dsrvl8.qual |= ARGUS_IPATTR_DST_OPTIONS;
                                       ipattr->hdr.argus_dsrvl8.len++;
                                    }
                                 }
                              }
                              break;

                           case ETHERTYPE_REVARP:
                              flow->hdr.argus_dsrvl8.qual = ARGUS_TYPE_RARP;
                              break;
                           case ETHERTYPE_ARP:
                              flow->hdr.argus_dsrvl8.qual = ARGUS_TYPE_ARP;
                              break;
                           default:
                              flow->hdr.argus_dsrvl8.qual = ARGUS_TYPE_ETHER;
                              far->flow.flow_union.mac.ehdr.ether_type = argus2->ahdr.status & 0xFFFF;
                              break;
                        }
                        flow->hdr.argus_dsrvl8.len    = 5;
                        bcopy ((char *)&far->flow.flow_union.ip, (char *)&flow->ip_flow, sizeof(flow->ip_flow));

                        dsr += flow->hdr.argus_dsrvl8.len;
                        argus->hdr.len += flow->hdr.argus_dsrvl8.len;
                        dtime = (struct ArgusTimeObject *) dsr;

                        dtime->hdr.type               = ARGUS_TIME_DSR;     
                        dtime->hdr.subtype            = ARGUS_TIME_ABSOLUTE_RANGE; 
                        dtime->hdr.argus_dsrvl8.qual  = ARGUS_TYPE_UTC_MICROSECONDS;
                        dtime->hdr.argus_dsrvl8.len   = 5;
                        bcopy ((char *)&far->time.start, (char *)&dtime->src.start, 16);

                        dsr += dtime->hdr.argus_dsrvl8.len;
                        argus->hdr.len += dtime->hdr.argus_dsrvl8.len;
                        metric = (struct ArgusMetricStruct *) dsr;

                        metric->hdr.type              = ARGUS_METER_DSR;
                        metric->hdr.subtype           = ARGUS_METER_PKTS_BYTES_APP;

                        if ((far->src.count > 0) && (far->dst.count > 0)) {
                           metric->hdr.argus_dsrvl8.qual = ARGUS_SRCDST_LONGLONG;
                           metric->hdr.argus_dsrvl8.len  = 13;
                           ptr    = &metric->src.pkts;
                           *ptr++ = far->src.count;
                           *ptr++ = far->src.bytes;
                           *ptr++ = far->src.appbytes;
                           *ptr++ = far->dst.count;
                           *ptr++ = far->dst.bytes;
                           *ptr++ = far->dst.appbytes;
                        } else
                        if (far->src.count > 0) {
                           metric->hdr.argus_dsrvl8.qual = ARGUS_SRC_LONGLONG;
                           metric->hdr.argus_dsrvl8.len  = 7;
                           ptr    = &metric->src.pkts;
                           *ptr++ = far->src.count;
                           *ptr++ = far->src.bytes;
                           *ptr++ = far->src.appbytes;
                        } else {
                           metric->hdr.argus_dsrvl8.qual = ARGUS_DST_LONGLONG;
                           metric->hdr.argus_dsrvl8.len  = 7;
                           ptr    = &metric->src.pkts;
                           *ptr++ = far->dst.count;
                           *ptr++ = far->dst.bytes;
                           *ptr++ = far->dst.appbytes;
                        }

                        dsr += metric->hdr.argus_dsrvl8.len;
                        argus->hdr.len += metric->hdr.argus_dsrvl8.len;

                        if (ipattr != NULL) {
                           unsigned int *dsrptr = (unsigned int *)(dsr + 1);

                           bcopy((char *) &ipattr->hdr, (char *) dsr, sizeof(*dsr));

                           if (ipattr->hdr.argus_dsrvl8.qual & ARGUS_IPATTR_SRC) {
                              *dsrptr = *(unsigned int *)&ipattr->src;
                              dsrptr++;
                           }
                           if (ipattr->hdr.argus_dsrvl8.qual & ARGUS_IPATTR_SRC_OPTIONS) {
                              *dsrptr = *(unsigned int *)&ipattr->src.options;
                              dsrptr++;
                           }
                           if (ipattr->hdr.argus_dsrvl8.qual & ARGUS_IPATTR_DST) {
                              *dsrptr = *(unsigned int *)&ipattr->dst;
                              dsrptr++;
                           }
                           if (ipattr->hdr.argus_dsrvl8.qual & ARGUS_IPATTR_DST_OPTIONS) {
                              *dsrptr = *(unsigned int *)&ipattr->dst.options;
                              dsrptr++;
                           }

                           dsr += ipattr->hdr.argus_dsrvl8.len;
                           argus->hdr.len += ipattr->hdr.argus_dsrvl8.len;
                        }

                        break;
                     }

                     case ARGUS_V2_TCP_DSR_STATUS: {
                        struct ArgusV2TCPObject *nv2tcp = (struct ArgusV2TCPObject *)hdrs[ARGUS_V2_TCP_DSR_INDEX];
                        struct ArgusNetworkStruct *net = (struct ArgusNetworkStruct *) dsr;
                        struct ArgusTCPObject *tcp = (struct ArgusTCPObject *)&net->net_union.tcp;

                        net->hdr.type             = ARGUS_NETWORK_DSR;     
                        net->hdr.subtype          = ARGUS_TCP_PERF;
                        net->hdr.argus_dsrvl8.len   = ((sizeof(*tcp) + 3)/4) + 1;

                        tcp->status               = nv2tcp->state;
                        tcp->state                = nv2tcp->status;
                        tcp->options              = nv2tcp->options;
                        tcp->synAckuSecs          = nv2tcp->synAckuSecs;
                        tcp->ackDatauSecs         = nv2tcp->ackDatauSecs;
                        tcp->src.seqbase          = nv2tcp->src.seqbase;
                        tcp->src.ackbytes         = nv2tcp->src.ackbytes;
                        tcp->src.bytes            = nv2tcp->src.bytes;
                        tcp->src.retrans          = nv2tcp->src.rpkts;
                        tcp->src.win              = nv2tcp->src.win;
                        tcp->src.winshift         = 0;
                        tcp->src.flags            = nv2tcp->src.flags;
                        tcp->src.status           = 0; tcp->src.seq    = 0;
                        tcp->src.ack              = 0; tcp->src.winnum = 0;
                        tcp->src.winbytes         = 0;
                        tcp->dst.seqbase          = nv2tcp->dst.seqbase;
                        tcp->dst.ackbytes         = nv2tcp->dst.ackbytes;
                        tcp->dst.bytes            = nv2tcp->dst.bytes;
                        tcp->dst.retrans          = nv2tcp->dst.rpkts;
                        tcp->dst.win              = nv2tcp->dst.win;
                        tcp->dst.winshift         = 0;
                        tcp->dst.flags            = nv2tcp->dst.flags;
                        tcp->dst.status           = 0; tcp->dst.seq    = 0;
                        tcp->dst.ack              = 0; tcp->dst.winnum = 0;
                        tcp->dst.winbytes         = 0;

                        dsr += net->hdr.argus_dsrvl8.len;
                        argus->hdr.len += net->hdr.argus_dsrvl8.len;
                        break;
                     }

                     case ARGUS_V2_ICMP_DSR_STATUS: {
                        struct ArgusV2ICMPObject *nv2icmp = (struct ArgusV2ICMPObject *)hdrs[ARGUS_V2_ICMP_DSR_INDEX];
                        struct ArgusV2FarStruct  *far = (struct ArgusV2FarStruct *)hdrs[ARGUS_V2_FAR_DSR_INDEX];
                        struct ArgusIcmpStruct *icmp = (struct ArgusIcmpStruct *) dsr;

                        icmp->hdr.type            = ARGUS_ICMP_DSR;     
                        icmp->hdr.subtype         = 0;
                        
                        if (far != NULL)
                           icmp->hdr.argus_dsrvl8.qual = far->status & ARGUS_V2_ICMP_MAPPED;
                        else
                           icmp->hdr.argus_dsrvl8.qual = 0;

                        icmp->hdr.argus_dsrvl8.len  = ((sizeof(*icmp) + 3)/4) + 1;

                        icmp->icmp_type = nv2icmp->icmp_type;
                        icmp->icmp_code = nv2icmp->icmp_code;
                        icmp->iseq      = nv2icmp->iseq;
                        icmp->osrcaddr  = nv2icmp->osrcaddr;
                        icmp->odstaddr  = nv2icmp->odstaddr;
                        icmp->isrcaddr  = nv2icmp->isrcaddr;
                        icmp->idstaddr  = nv2icmp->idstaddr;
                        icmp->igwaddr   = nv2icmp->igwaddr;

                        dsr += icmp->hdr.argus_dsrvl8.len;
                        argus->hdr.len += icmp->hdr.argus_dsrvl8.len;
                        break;
                     }

                     case ARGUS_V2_RTCP_DSR_STATUS: {
                        struct ArgusV2RTCPObject *nv2rtcp = (struct ArgusV2RTCPObject *)hdrs[ARGUS_V2_RTCP_DSR_INDEX];
                        struct ArgusNetworkStruct *net = (struct ArgusNetworkStruct *)dsr;
                        struct ArgusRTCPObject *rtcp = &net->net_union.rtcp;

                        net->hdr.type            = ARGUS_NETWORK_DSR;
                        net->hdr.subtype         = ARGUS_RTCP_FLOW;

                        net->hdr.argus_dsrvl8.len  = (((sizeof(*rtcp) + 3)/4) + 1) + 1;
                        bcopy((char *)&nv2rtcp->src, (char *)&rtcp->src, sizeof(rtcp->src));
                        bcopy((char *)&nv2rtcp->dst, (char *)&rtcp->dst, sizeof(rtcp->dst));
                        rtcp->sdrop = nv2rtcp->src_pkt_drop;
                        rtcp->ddrop = nv2rtcp->dst_pkt_drop;

                        dsr += net->hdr.argus_dsrvl8.len;
                        argus->hdr.len += net->hdr.argus_dsrvl8.len;
                        break;
                     }
                     case ARGUS_V2_RTP_DSR_STATUS: {
                        struct ArgusV2RTPObject *nv2rtp = (struct ArgusV2RTPObject *)hdrs[ARGUS_V2_RTP_DSR_INDEX];
                        struct ArgusNetworkStruct *net = (struct ArgusNetworkStruct *)dsr;
                        struct ArgusRTPObject *rtp = &net->net_union.rtp;

                        net->hdr.type            = ARGUS_NETWORK_DSR;
                        net->hdr.subtype         = ARGUS_RTP_FLOW;

                        net->hdr.argus_dsrvl8.len  = (((sizeof(*rtp) + 3)/4) + 1) + 1;
                        bcopy((char *)&nv2rtp->src, (char *)&rtp->src, sizeof(rtp->src));
                        bcopy((char *)&nv2rtp->dst, (char *)&rtp->dst, sizeof(rtp->dst));
                        rtp->sdrop = nv2rtp->sdrop;
                        rtp->ddrop = nv2rtp->ddrop;
                        rtp->ssdev = nv2rtp->ssdev;
                        rtp->dsdev = nv2rtp->dsdev;

                        dsr += net->hdr.argus_dsrvl8.len;
                        argus->hdr.len += net->hdr.argus_dsrvl8.len;
                        break;
                     }

                     case ARGUS_V2_IGMP_DSR_STATUS:  
                     case ARGUS_V2_ARP_DSR_STATUS:   
                        break;

                     case ARGUS_V2_SRCUSRDATA_DSR_STATUS: {
                        struct ArgusDataStruct *user = (struct ArgusDataStruct *)dsr;
                        struct ArgusV2UserStruct *nv2user = (struct ArgusV2UserStruct *)hdrs[ARGUS_V2_SRCUSRDATA_DSR_INDEX];
                        int len = (nv2user->length - 1) * 4;
                        len = (len < argus2->argus_far.src.appbytes) ? len : argus2->argus_far.src.appbytes;

                        user->hdr.type              = ARGUS_DATA_DSR;     
                        user->hdr.subtype           = ARGUS_LEN_16BITS | ARGUS_SRC_DATA;
                        user->hdr.argus_dsrvl16.len = nv2user->length + 1;
                        user->size                  = (nv2user->length - 1) * 4;
                        user->count                 = len;

                        bcopy (&nv2user->data, &user->array, (nv2user->length - 1) * 4);
                        dsr += user->hdr.argus_dsrvl16.len;
                        argus->hdr.len += user->hdr.argus_dsrvl16.len;
                        break;
                     }

                     case ARGUS_V2_DSTUSRDATA_DSR_STATUS: {
                        struct ArgusDataStruct *user = (struct ArgusDataStruct *)dsr;
                        struct ArgusV2UserStruct *nv2user = (struct ArgusV2UserStruct *)hdrs[ARGUS_V2_DSTUSRDATA_DSR_INDEX];
                        int len = (nv2user->length - 1) * 4;
                        len = (len < argus2->argus_far.dst.appbytes) ? len : argus2->argus_far.dst.appbytes;

                        user->hdr.type              = ARGUS_DATA_DSR;     
                        user->hdr.subtype           =  ARGUS_LEN_16BITS | ARGUS_DST_DATA;
                        user->hdr.argus_dsrvl16.len = nv2user->length + 1;
                        user->size                  = (nv2user->length - 1) * 4;
                        user->count                 = len;

                        bcopy (&nv2user->data, &user->array, (nv2user->length - 1) * 4);
                        dsr += user->hdr.argus_dsrvl16.len;
                        argus->hdr.len += user->hdr.argus_dsrvl16.len;
                        break;
                     }

                     case ARGUS_V2_ESP_DSR_STATUS:   
                        break;

                     case ARGUS_V2_AGR_DSR_STATUS:
                        break;

                     case ARGUS_V2_TIME_DSR_STATUS: {
                        struct ArgusJitterStruct *jitter = (struct ArgusJitterStruct *) dsr;
                        struct ArgusV2TimeStruct *time = (struct ArgusV2TimeStruct *)hdrs[ARGUS_V2_TIME_DSR_INDEX];
                        jitter->hdr.type             = ARGUS_JITTER_DSR;
                        jitter->hdr.subtype          = 0;
                        jitter->hdr.argus_dsrvl8.qual  = (ARGUS_SRC_ACTIVE_JITTER | ARGUS_DST_ACTIVE_JITTER |
                                                        ARGUS_SRC_IDLE_JITTER   | ARGUS_DST_IDLE_JITTER );
                        jitter->hdr.argus_dsrvl8.len   = sizeof(*jitter) >> 2;

                        jitter->src.act.n = time->src.act.n;
                        jitter->src.act.minval  = time->src.act.minval;
                        jitter->src.act.meanval = time->src.act.meanval;
                        jitter->src.act.stdev = time->src.act.stdev;
                        jitter->src.act.maxval  = time->src.act.maxval;
 
                        jitter->src.idle.n = time->src.idle.n;
                        jitter->src.idle.minval  = time->src.idle.minval;
                        jitter->src.idle.meanval = time->src.idle.meanval;
                        jitter->src.idle.stdev = time->src.idle.stdev;
                        jitter->src.idle.maxval  = time->src.idle.maxval;

                        jitter->dst.act.n = time->dst.act.n;
                        jitter->dst.act.minval  = time->dst.act.minval;
                        jitter->dst.act.meanval = time->dst.act.meanval;
                        jitter->dst.act.stdev = time->dst.act.stdev;
                        jitter->dst.act.maxval  = time->dst.act.maxval;
 
                        jitter->dst.idle.n = time->dst.idle.n;
                        jitter->dst.idle.minval  = time->dst.idle.minval;
                        jitter->dst.idle.meanval = time->dst.idle.meanval;
                        jitter->dst.idle.stdev = time->dst.idle.stdev;
                        jitter->dst.idle.maxval  = time->dst.idle.maxval;

                        dsr += jitter->hdr.argus_dsrvl8.len;
                        argus->hdr.len += jitter->hdr.argus_dsrvl8.len;
                        break;
                     }
/*
struct ArgusV2MacStruct {
   unsigned char type, length;
   unsigned short status;
   union {
      struct ArgusV2ETHERObject ether;
   } phys_union;
};
*/
                     case ARGUS_V2_MAC_DSR_STATUS: {
                        struct ArgusMacStruct *mac = (struct ArgusMacStruct *) dsr;
                        struct ArgusV2MacStruct *mac2 = (struct ArgusV2MacStruct *)hdrs[ARGUS_V2_MAC_DSR_INDEX];
                        mac->hdr.type              = ARGUS_MAC_DSR;
                        mac->hdr.subtype           = 0;
                        mac->hdr.argus_dsrvl8.len  = 5;

                        bcopy ((char *)&mac2->phys_union.ether.ethersrc,(char *)&mac->mac.mac_union.ether.ehdr.ether_shost, 6);
                        bcopy ((char *)&mac2->phys_union.ether.etherdst,(char *)&mac->mac.mac_union.ether.ehdr.ether_dhost, 6);
                        mac->mac.mac_union.ether.ehdr.ether_type = ntohs(mac2->status & 0xFFFF);

                        dsr += mac->hdr.argus_dsrvl8.len;
                        argus->hdr.len += mac->hdr.argus_dsrvl8.len;
                        break;
                     }

                     case ARGUS_V2_VLAN_DSR_STATUS: {
                        struct ArgusVlanStruct *vlan = (struct ArgusVlanStruct *) dsr;
                        struct ArgusV2VlanStruct *nv2vlan = (struct ArgusV2VlanStruct *)hdrs[ARGUS_V2_VLAN_DSR_INDEX];
                        vlan->hdr.type              = ARGUS_VLAN_DSR;
                        vlan->hdr.subtype           = 0;
                        vlan->hdr.argus_dsrvl8.len  = sizeof(*vlan)/4;
                        vlan->hdr.argus_dsrvl8.qual = 0;

                        if (nv2vlan->status & ARGUS_SRC_VLAN) {
                           vlan->hdr.argus_dsrvl8.qual |= ARGUS_SRC_VLAN;
                           vlan->sid = nv2vlan->sid;
                        } else
                           vlan->sid = 0;

                        if (nv2vlan->status & ARGUS_DST_VLAN) {
                           vlan->hdr.argus_dsrvl8.qual |= ARGUS_DST_VLAN;
                           vlan->did = nv2vlan->did;
                        } else
                           vlan->did = 0;
                        dsr += vlan->hdr.argus_dsrvl8.len;
                        argus->hdr.len += vlan->hdr.argus_dsrvl8.len;

                        break;
                     }
                     case ARGUS_V2_MPLS_DSR_STATUS: {
                        struct ArgusMplsStruct *mpls = (struct ArgusMplsStruct *) dsr;
                        struct ArgusV2MplsStruct *nv2mpls = (struct ArgusV2MplsStruct *)hdrs[ARGUS_V2_MPLS_DSR_INDEX];
                        mpls->hdr.type             = ARGUS_MPLS_DSR;     
                        mpls->hdr.subtype          = 0;
                        mpls->hdr.argus_dsrvl8.len   = ((sizeof(*mpls) + 3)/4) + 1;
                        mpls->slabel = nv2mpls->slabel;
                        mpls->dlabel = nv2mpls->dlabel;

                        dsr += mpls->hdr.argus_dsrvl8.len;
                        argus->hdr.len += mpls->hdr.argus_dsrvl8.len;

                        break;
                     }
                     case ARGUS_V2_FRG_DSR_STATUS:   
                        break;
                  }
               }
            }
            length = argus->hdr.len * 4;
            ((unsigned int *)argus)[argus->hdr.len] = 0;
            ArgusHtoN(argus);
         }
      }
   }

   return (input->ArgusConvBuffer);
}


int ArgusWriteConnection (struct ArgusParserStruct *parser, struct ArgusInput *, u_char *, int);

extern ArgusNetFlowHandler ArgusLookUpNetFlow(struct ArgusInput *, int); 

#define CISCO_VERSION_1         1
#define CISCO_VERSION_5         5
#define CISCO_VERSION_6         6
#define CISCO_VERSION_7         7
#define CISCO_VERSION_8         8

#if defined(HAVE_FLOWTOOLS)
extern struct ArgusRecord *ArgusParseFlowToolsRecord (struct ArgusParserStruct *parser, struct ArgusInput *, struct fts3rec_all *);
#endif

int
ArgusReadConnection (struct ArgusParserStruct *parser, struct ArgusInput *input, int type)
{
   struct ArgusRecord argus;
   u_char *ptr = (u_char *)&argus;
   u_char buf[MAXARGUSRECORD];
   int cnt, retn = -1, found = 0, len;

   switch  (type) {
      case ARGUS_FILE: {
         if (input->file != NULL) {
            switch (input->type) {
               case ARGUS_DATA_SOURCE:
               case ARGUS_V2_DATA_SOURCE: {
                  if ((cnt = fread (&argus, 1, 16, input->file)) == 16) {
#ifdef ARGUSDEBUG
                     ArgusDebug (2, "ArgusReadConnection() read %d bytes\n", cnt);
#endif
                     if (((ptr[0] == 0x1F) && ((ptr[1] == 0x8B) || (ptr[1] == 0x9D))) ||
                         ((ptr[0] == 'B') && (ptr[1] == 'Z') && (ptr[2] == 'h'))) {
                        char cmd[256];
                        bzero(cmd, 256);

                        fclose(input->file);
                        input->file = NULL;

                        if (ptr[0] == 'B')
                           strncpy(cmd, "bzip2 -dc ", 11);
                        else
                        if (ptr[1] == 0x8B)
                           strncpy(cmd, "gzip -dc ", 11);
                        else
                           strncpy(cmd, "zcat ", 6);
            
                        strncat(cmd, input->filename, (256 - strlen(cmd)));
                        strncat(cmd, " 2>/dev/null", (256 - strlen(cmd)));
             
                        if ((input->pipe = popen(cmd, "r")) == NULL)
                           ArgusLog (LOG_ERR, "ArgusReadConnection: popen(%s) failed. %s", cmd, strerror(errno));

                        if ((cnt = fread (&argus, 1, 16, input->pipe)) != 16) {
#ifdef ARGUSDEBUG
                           ArgusDebug (1, "ArgusReadConnection: read from '%s' failed. %s", cmd, strerror(errno));
#endif
                           pclose(input->pipe);
                           input->pipe = NULL;
                           return (retn);
                        } else {
#ifdef ARGUSDEBUG
                           ArgusDebug (1, "ArgusReadConnection() read %d bytes from pipe\n", cnt);
#endif
                           input->file = input->pipe;
                        }
                     }

                     input->offset = 16;

                     if ((argus.argus_mar.argusid == htonl(ARGUS_COOKIE)) ||
                         (argus.argus_mar.argusid ==       ARGUS_COOKIE)) {
                        int size = sizeof(argus) - 16;
                        int br = 0;
                        cnt = 0;
                        clearerr(input->file);
                        while (cnt != size) {
                           if ((br = fread (&((u_char *)&argus)[16 + cnt], 1, (size - cnt), input->file)) > 0) {
                              cnt += br;
                           } else {
#ifdef ARGUSDEBUG
                                 ArgusDebug (1, "ArgusReadConnection() read returned zero %s.\n", strerror(errno));
#endif
/*
                              if (feof(input->file) || ferror(input->file)) {
                                 if (input->pipe != NULL) {
                                    pclose(input->pipe);
                                    input->pipe = NULL;
                                 } else {
                                    fclose (input->file);
                                 }
                                 input->file = NULL;
                                 return (retn);
                              }
*/
                           }
                        }
#ifdef ARGUSDEBUG
                        ArgusDebug (2, "ArgusReadConnection() read %d bytes\n", cnt);
#endif
                        input->offset += cnt;
                        input->major_version = argus.argus_mar.major_version;
                        input->minor_version = argus.argus_mar.minor_version;
                        bcopy ((char *) &argus, (char *)&input->ArgusInitCon, sizeof (argus));
                        bcopy ((char *) &argus, (char *)&input->ArgusManStart, sizeof (argus));
#ifdef _LITTLE_ENDIAN
                        ArgusNtoH((struct ArgusRecord *)&input->ArgusManStart);
#endif

                        input->ArgusID = ntohl(argus.argus_mar.thisid);
                        input->ArgusReadSize = argus.argus_mar.record_len;

                        fstat(fileno(input->file), &input->statbuf);
                        ArgusParseInit (parser, input);
                        bzero(buf, MAXSTRLEN); 
                        found++;

                     } else {
                        struct ArgusV2Record *argus2 = (struct ArgusV2Record *) &argus;
                     
                        if (argus2->ahdr.type & ARGUS_V2_MAR) {
                           u_short length = ntohs(argus2->ahdr.length);
                           u_int argusid   = ntohl(argus2->ahdr.argusid);
                           u_int status   = ntohl(argus2->ahdr.status);
                           u_int sequence = ntohl(argus2->ahdr.seqNumber);

                           if ((sequence == 0) && (length == sizeof (*argus2))) {
                              if (argus2->ahdr.cause & ARGUS_V2_ERROR) {
#ifdef ARGUSDEBUG
                                 ArgusDebug (1, "ArgusReadConnection() ARGUS_V2_ERROR Mar.\n");
#endif
                                 if (status & ARGUS_MAXLISTENEXCD) {
                                    ArgusLog (LOG_ALERT, "remote exceed listen error.");
                                    if (input->pipe != NULL) {
                                       pclose(input->pipe);
                                       input->pipe = NULL;
                                    } else {
                                       fclose (input->file);
                                    }
                                    input->file = NULL;
                                    return (retn);
                                 }
                              }

                              if (argus2->ahdr.cause == ARGUS_V2_START) {
#ifdef ARGUSDEBUG
                                 ArgusDebug (5, "ArgusReadConnection() ARGUS_V2_START Mar.\n");
#endif
                                 input->type = ARGUS_V2_DATA_SOURCE;
                                 if ((argusid == ARGUS_V2_COOKIE) && (sequence == 0)) {
                                    int size = length - sizeof(argus2->ahdr);
                     
                                    if ((cnt = fread (&argus2->argus_mar, 1, size, input->file)) != size) {
#ifdef ARGUSDEBUG
                                       ArgusDebug (1, "ArgusReadConnection() read failed for ARGUS_START Mar %s.\n",
                                                       strerror(errno));
#endif
                                       if (input->pipe != NULL) {
                                          pclose(input->pipe);
                                          input->pipe = NULL;
                                       } else {
                                          fclose (input->file);
                                       }
                                       input->file = NULL;
                                       return (retn);
                                    }

                                    input->offset += cnt;
                                    ptr = ArgusConvertRecord(input, (char *)argus2);
                                    bcopy ((char *) ptr, (char *)&input->ArgusInitCon, sizeof (*argus2));

                                    fstat(fileno(input->file), &input->statbuf);
#ifdef _LITTLE_ENDIAN
                                    ArgusNtoH((struct ArgusRecord *)argus2);
#endif
                                    bcopy ((char *) argus2, (char *)&input->ArgusManStart, sizeof (*argus2));
                                    input->major_version = MAJOR_VERSION_2;
                                    input->minor_version = MINOR_VERSION_0;
                                    input->ArgusReadSize = ((struct ArgusRecord *)argus2)->argus_mar.record_len;

                                    input->ArgusID = argus2->argus_mar.argusid;

                                    ArgusParseInit (parser, input);
                                    found++;
                     
                                 } else {
                                    ArgusLog (LOG_ALERT, "ArgusReadConnection: not Argus-2.0 data stream.");
                                    if (input->pipe != NULL) {
                                       pclose(input->pipe);
                                       input->pipe = NULL;
                                    } else {
                                       fclose (input->file);
                                    }
                                    input->file = NULL;
                                 }
                              }
                           }
                        }
                     }
                  }
                  break;
               }

               case ARGUS_JFLOW_DATA_SOURCE: 
               case ARGUS_CISCO_DATA_SOURCE: {
                  char *ptr = (char *)&argus;
#ifdef ARGUSDEBUG
                  ArgusDebug (3, "ArgusReadConnection() testing for CISCO records\n");
#endif
                  if ((cnt = fread (&argus, 1, 16, input->file)) == 16) {
                     if (!(strncmp(&ptr[3], "SOURCE", 6))) {
                        BinaryHeaderF2 *ArgusNetFlow = (BinaryHeaderF2 *) buf;
                        int size;
   
                        bcopy ((char *)&argus, buf, 16);
                        size = sizeof(*ArgusNetFlow) - 16;
   
                        if ((cnt = fread (&buf[16], 1, size, input->file)) != size) {
                           ArgusLog (LOG_ALERT, "ArgusReadConnection: reading %d bytes, got %d bytes. %s", size, cnt, strerror(errno));
                           if (input->pipe != NULL) {
                              pclose(input->pipe);
                              input->pipe = NULL;
                           } else {
                              fclose (input->file);
                           }
                           input->file = NULL;
                           return (-1);
   
                        } else {
#ifdef _LITTLE_ENDIAN
                           ArgusNetFlow->starttime = ntohl(ArgusNetFlow->starttime);
                           ArgusNetFlow->endtime   = ntohl(ArgusNetFlow->endtime);
                           ArgusNetFlow->flows     = ntohl(ArgusNetFlow->flows);
                           ArgusNetFlow->missed    = ntohl(ArgusNetFlow->missed);
                           ArgusNetFlow->records   = ntohl(ArgusNetFlow->records);
#endif
                           bzero ((char *)&argus, sizeof(argus));
                           argus.hdr.type          = ARGUS_MAR | ARGUS_NETFLOW | ARGUS_VERSION;
                           argus.hdr.cause         = ARGUS_START;
                           argus.hdr.len           = sizeof (argus) / 4;
                           argus.argus_mar.argusid = ARGUS_COOKIE;
                           if (input->addr.s_addr != 0)
                              argus.argus_mar.thisid = input->addr.s_addr;

                           argus.argus_mar.startime.tv_sec = ArgusParser->ArgusGlobalTime.tv_sec;
                           argus.argus_mar.now.tv_sec      = ArgusParser->ArgusGlobalTime.tv_sec;
                           argus.argus_mar.major_version   = VERSION_MAJOR;
                           argus.argus_mar.minor_version   = VERSION_MINOR;
                           argus.argus_mar.record_len      = -1;

                           input->major_version = argus.argus_mar.major_version;
                           input->minor_version = argus.argus_mar.minor_version;

                           if ((input->ArgusCiscoNetFlowParse =
                                  ArgusLookUpNetFlow(input, ArgusNetFlow->aggregation)) != NULL) {
#ifdef _LITTLE_ENDIAN
                              ArgusHtoN(&argus);
#endif
                              bcopy ((char *) &argus, (char *)&input->ArgusInitCon, sizeof (argus));
#ifdef _LITTLE_ENDIAN
                              ArgusNtoH(&argus);
#endif
                              input->type = ARGUS_DATA_SOURCE;
                              ArgusParseInit (parser, input);
                              input->type = ARGUS_CISCO_DATA_SOURCE;
                              found++;

                           } else
                              ArgusLog (LOG_ERR, "%s: not supported Cisco data stream.\n");
                        }

                     } else {
                        unsigned short vers;
                        switch (vers = ntohs(*(unsigned short *)ptr)) {
                           case CISCO_VERSION_1:
                           case CISCO_VERSION_5:
                           case CISCO_VERSION_6:
                           case CISCO_VERSION_7:
                           case CISCO_VERSION_8:
                              found++;
                              input->type = ARGUS_CISCO_DATA_SOURCE;
                              fseek(input->file, 0, SEEK_SET);
                              ArgusParseInit (parser, input);
#ifdef ARGUSDEBUG
                              ArgusDebug (3, "ArgusReadConnection() found cflowd record\n");
#endif
                        }
                     }

                     break;
                  }
               }

               case ARGUS_SFLOW_DATA_SOURCE: 
#ifdef ARGUSDEBUG
                  ArgusDebug (3, "ArgusReadConnection() testing for Sflow based records\n");
#endif
                  break;

#if defined(HAVE_FLOWTOOLS)
               case ARGUS_FLOW_TOOLS_SOURCE: {
                  struct fts3rec_all cur;
                  struct fts3rec_offsets fo;
                  struct ftver ftv;
                  struct ftio ftio, *ftp = &ftio;
                  struct ArgusRecord *argus;
                  char *rec;
                  int done = 0;

#ifdef ARGUSDEBUG
                  ArgusDebug (3, "ArgusReadConnection() testing for Flow Tools based records\n");
#endif
                  if (ftio_init(ftp, fileno(input->file), FT_IO_FLAG_READ) < 0)
                     ArgusLog (LOG_ERR, "ArgusReadConnection() flowtools ftio_init failed");

                  ftio_get_ver(ftp, &ftv);
                  fts3rec_compute_offsets(&fo, &ftv);

                  while ((rec = ftio_read(ftp)) != NULL) {
                     long long xfield;
                     int i, fields = 0;

                     bzero(&cur, sizeof(cur));

                     for (i = 1, xfield = i; i < 32; i++) {
                        xfield = FT_XFIELD_UNIX_SECS << (i - 1);
                        if (!(ftio_check_xfield(ftp, xfield))) {
                           switch (xfield) {
                              case FT_XFIELD_UNIX_SECS:    cur.unix_secs  = ((u_int32_t *)(rec+fo.unix_secs)); break;
                              case FT_XFIELD_UNIX_NSECS:   cur.unix_nsecs = ((u_int32_t *)(rec+fo.unix_nsecs)); fields++; break;
                              case FT_XFIELD_SYSUPTIME:    cur.sysUpTime  = ((u_int32_t *)(rec+fo.sysUpTime)); fields++; break;
                              case FT_XFIELD_EXADDR:       cur.exaddr = ((u_int32_t *)(rec+fo.exaddr)); fields++; break;
                              case FT_XFIELD_DFLOWS:       cur.dFlows = ((u_int32_t *)(rec+fo.dFlows)); fields++; break;
                              case FT_XFIELD_DPKTS:        cur.dPkts = ((u_int32_t *)(rec+fo.dPkts)); fields++; break;
                              case FT_XFIELD_DOCTETS:      cur.dOctets = ((u_int32_t *)(rec+fo.dOctets)); fields++; break;
                              case FT_XFIELD_FIRST:        cur.First = ((u_int32_t *)(rec+fo.First)); fields++; break;
                              case FT_XFIELD_LAST:         cur.Last = ((u_int32_t *)(rec+fo.Last)); fields++; break;
                              case FT_XFIELD_ENGINE_TYPE: fields++; break;
                              case FT_XFIELD_ENGINE_ID:   fields++; break;
                              case FT_XFIELD_SRCADDR:      cur.srcaddr = ((u_int32_t *)(rec+fo.srcaddr)); fields++; break;
                              case FT_XFIELD_DSTADDR:      cur.dstaddr = ((u_int32_t *)(rec+fo.dstaddr)); fields++; break;
                              case FT_XFIELD_NEXTHOP:      fields++; break;
                              case FT_XFIELD_INPUT:        cur.input = ((u_int16_t *)(rec+fo.input)); fields++; break;
                              case FT_XFIELD_OUTPUT:       cur.output = ((u_int16_t *)(rec+fo.output)); fields++; break;
                              case FT_XFIELD_SRCPORT:      cur.srcport = ((u_int16_t *)(rec+fo.srcport)); fields++; break;
                              case FT_XFIELD_DSTPORT:      cur.dstport = ((u_int16_t *)(rec+fo.dstport)); fields++; break;
                              case FT_XFIELD_PROT:         cur.prot = ((u_int8_t *)(rec+fo.prot)); fields++; break;
                              case FT_XFIELD_TOS:          cur.tos = ((u_int8_t *)(rec+fo.tos)); fields++; break;
                              case FT_XFIELD_TCP_FLAGS:    cur.tcp_flags = ((u_int8_t *)(rec+fo.tcp_flags)); fields++; break;
                              case FT_XFIELD_SRC_MASK:     cur.src_mask = ((u_int8_t *)(rec+fo.src_mask)); fields++; break;
                              case FT_XFIELD_DST_MASK:     cur.dst_mask = ((u_int8_t *)(rec+fo.dst_mask)); fields++; break;
                              case FT_XFIELD_SRC_AS:       cur.src_as = ((u_int16_t *)(rec+fo.src_as)); fields++; break;
                              case FT_XFIELD_DST_AS:       cur.dst_as = ((u_int16_t *)(rec+fo.dst_as)); fields++; break;
                              case FT_XFIELD_IN_ENCAPS:    cur.in_encaps = ((u_int8_t *)(rec+fo.in_encaps)); fields++; break;
                              case FT_XFIELD_OUT_ENCAPS:   cur.out_encaps = ((u_int8_t *)(rec+fo.out_encaps)); fields++; break;
                              case FT_XFIELD_PEER_NEXTHOP: cur.peer_nexthop = ((u_int32_t *)(rec+fo.peer_nexthop)); fields++; break;
                              case FT_XFIELD_ROUTER_SC:    cur.router_sc = ((u_int32_t *)(rec+fo.router_sc)); fields++; break;
                              case FT_XFIELD_EXTRA_PKTS:   cur.extra_pkts = ((u_int32_t *)(rec+fo.extra_pkts)); fields++; break;
                              case FT_XFIELD_MARKED_TOS:   cur.marked_tos = ((u_int8_t *)(rec+fo.marked_tos)); fields++; break;
                              case FT_XFIELD_SRC_TAG:      cur.src_tag = ((u_int32_t *)(rec+fo.src_tag)); fields++; break; 
                              case FT_XFIELD_DST_TAG:      cur.dst_tag = ((u_int32_t *)(rec+fo.dst_tag)); fields++; break;
                           }
                        }
                     }

                     if ((argus = ArgusParseFlowToolsRecord (ArgusParser, input, &cur)) != NULL) {
                        if (argus && !done && !parser->RaParseDone) {
                           int len = 0;
                           if ((len = ArgusHandleDatum (ArgusParser, input, argus, &ArgusParser->ArgusFilterCode)) < 0) {
                              retn = 1;
                              done = 1;
                           } else {

                              input->offset += len;
                              input->ArgusReadPtr += len;
                              input->ArgusReadSocketCnt -= len;

                              if (input->ostop != -1) {
                                 if (input->offset > input->ostop) {
                                    retn = 1;
                                    done++;
                                 }
                              }
                           }
               
                        } else
                           done = 1;
                     }
                  }
                  break;
               }
#endif

               default:
                  ArgusLog (LOG_ERR, "ArgusReadConnection(0x%x) unknown source type", input);
                  break;
            }

            if (!found) {
               if (input->pipe != NULL) {
                  pclose(input->pipe);
                  input->pipe = NULL;
               } else {
                  if (input->file != NULL)
                     fclose (input->file);
               }
               input->file = NULL;
            } else
                  retn = 1;

         } else {
            if (input->pipe != NULL) {
               pclose(input->pipe);
               input->pipe = NULL;
            } else {
               if (input->file != NULL)
                  fclose (input->file);
            }
            input->file = NULL;
         }
         break;
      }
      
      case ARGUS_SOCKET: {
         switch (input->type) {
            case ARGUS_DATA_SOURCE:
            case ARGUS_V2_DATA_SOURCE: {
               if (input->fd >= 0) {
                  int bytes = 0;
                  while (bytes < 16) {
                     if ((cnt = read (input->fd, &((char *)&argus)[bytes], 16 - bytes)) > 0)
                        bytes += cnt;
                     else
                        break;
                  }

                  if (bytes == 16) {
#ifdef ARGUSDEBUG
                     ArgusDebug (2, "ArgusReadConnection() read %d bytes\n", cnt);
#endif
                     input->offset = 16;

                     if (((argus.hdr.type & 0x0F) >= MAJOR_VERSION_3) || (argus.argus_mar.argusid == htonl(ARGUS_COOKIE))) {
                        switch (argus.hdr.cause & 0xF0) {
                           case ARGUS_ERROR:
                              switch (argus.hdr.cause & 0x0F) {
                                 case ARGUS_ACCESSDENIED:
                                    ArgusLog (LOG_ALERT, "remote access denied.");
                                    close (input->fd);
                                    input->fd = -1;
                                    return (retn);
                                 case ARGUS_MAXLISTENEXCD:
                                    ArgusLog (LOG_ALERT, "remote exceed listen error.");
                                    close (input->fd);
                                    input->fd = -1;
                                    return (retn);
                              }
                              break;

                           case ARGUS_START:
                              if (argus.argus_mar.argusid == htonl(ARGUS_COOKIE)) {
/*
                                 int size = sizeof(argus) - 16;

                                 if ((cnt = read (input->fd, &((u_char *)&argus)[16], size)) != size) {
#ifdef ARGUSDEBUG
                                    ArgusDebug (1, "ArgusReadConnection() read failed for ARGUS_START Mar %s.\n", strerror(errno));
#endif
                                    close (input->fd);
                                    input->fd = -1;
                                    return (retn);
                                 } else {
#ifdef ARGUSDEBUG
                                    ArgusDebug (5, "ArgusReadConnection() read %d bytes\n", cnt);
#endif
                                 }
*/
                                 int size = sizeof(argus) - 16;
                                 int br = 0;
                                 cnt = 0;
                                 while (cnt != size) {
                                    if ((br = read (input->fd, &((u_char *)&argus)[16 + cnt], (size - cnt))) > 0) {
                                       cnt += br;
                                    } else {
#ifdef ARGUSDEBUG
                                       ArgusDebug (1, "ArgusReadConnection() read returned zero %s.\n", strerror(errno));
#endif
                                       if (br < 0) {
                                          if (input->pipe != NULL) {
                                             pclose(input->pipe);
                                             input->pipe = NULL;
                                          } else {
                                             close (input->fd);
                                          }
                                          input->fd = -1;
                                          return (retn);
                                       }
                                    }
                                 }
                                 input->offset += cnt;
                                 input->major_version = argus.argus_mar.major_version;
                                 input->minor_version = argus.argus_mar.minor_version;
                                 bcopy ((char *) &argus, (char *)&input->ArgusInitCon, sizeof (argus));
                                 bcopy ((char *) &argus, (char *)&input->ArgusManStart, sizeof (argus));
#ifdef _LITTLE_ENDIAN
                                 ArgusNtoH((struct ArgusRecord *)&input->ArgusManStart);
#endif
                                 input->ArgusID = ntohl(argus.argus_mar.thisid);
                                 input->ArgusReadSize = argus.argus_mar.record_len;

                                 fstat(input->fd, &input->statbuf);
                                 ArgusParseInit (parser, input);
                                 found++;
                              }
                              break;
                        }

                     } else {
                        struct ArgusV2Record *argus2 = (struct ArgusV2Record *) &argus;
                     
                        if (argus2->ahdr.type & ARGUS_V2_MAR) {
                           u_short length = ntohs(argus2->ahdr.length);
                           u_int argusid  = ntohl(argus2->ahdr.argusid);
                           u_int status   = ntohl(argus2->ahdr.status);
                           u_int sequence = ntohl(argus2->ahdr.seqNumber);

                           if (argus2->ahdr.cause & ARGUS_V2_ERROR) {
#ifdef ARGUSDEBUG
                              ArgusDebug (1, "ArgusReadConnection() ARGUS_V2_ERROR Mar.\n");
#endif
                              if (status & ARGUS_V2_MAXLISTENEXCD) {
                                 ArgusLog (LOG_ALERT, "remote exceed listen error.");
                                 close (input->fd);
                                 input->fd = -1;
                                 return (retn);
                              }
                           }

                           if (argus2->ahdr.cause == ARGUS_V2_START) {
#ifdef ARGUSDEBUG
                              ArgusDebug (5, "ArgusReadConnection() ARGUS_V2_START Mar.\n");
#endif
                              input->type = ARGUS_V2_DATA_SOURCE;
                              if ((argusid == ARGUS_V2_COOKIE) && (sequence == 0)) {
                                 int size = length - sizeof(argus2->ahdr);
                  
                                 if ((cnt = read (input->fd, &argus2->argus_mar, size)) != size) {
#ifdef ARGUSDEBUG
                                    ArgusDebug (1, "ArgusReadConnection() read failed for ARGUS_START Mar %s.\n", strerror(errno));
#endif
                                    close (input->fd);
                                    input->fd = -1;
                                    return (retn);
                                 }

                                 input->offset += cnt;
                                 ptr = ArgusConvertRecord(input, (char *)argus2);
                                 bcopy ((char *) ptr, (char *)&input->ArgusInitCon, sizeof (*argus2));

                                 fstat(input->fd, &input->statbuf);
#ifdef _LITTLE_ENDIAN
                                 ArgusNtoH((struct ArgusRecord *)argus2);
#endif
                                 bcopy ((char *) argus2, (char *)&input->ArgusManStart, sizeof (*argus2));
                                 input->major_version = MAJOR_VERSION_2;
                                 input->minor_version = MINOR_VERSION_0;
                                 input->ArgusReadSize = ((struct ArgusRecord *)argus2)->argus_mar.record_len;

                                 ArgusParseInit (parser, input);
                                 found++;
                  
                              } else {
                                 ArgusLog (LOG_ALERT, "ArgusReadConnection: not Argus-2.0 data stream.");
                                 close (input->fd);
                                 input->fd = -1;
                              }
                           }
                        }
                     }

                  } else {
                     if (cnt < 0)
                        ArgusLog (LOG_ALERT, "ArgusReadConnection: %s %s.", input->hostname, strerror(errno));
                     else
                        ArgusLog (LOG_ALERT, "ArgusReadConnection: %s %s.", input->hostname, "connection closed");
                     close (input->fd);
                     input->fd = -1;
                  }

                  if (found) {
                     if (input->major_version >= MAJOR_VERSION_2) {
                        if (argus.argus_mar.status & htonl(ARGUS_SASL_AUTHENTICATE)) {
                           if (!(ArgusAuthenticate(input))) {
                              close(input->fd);
                              input->fd = -1;
                              return (retn);
                           }
                        }

                        if (input->fd >= 0) {
                           if (parser->ArgusRemoteFilter != NULL) {
                              snprintf ((char *) buf, MAXSTRLEN-1, "FILTER: man or (%s)",
                                       (char *) parser->ArgusRemoteFilter);

                              len = strlen((char *) buf);
                              if (ArgusWriteConnection (parser, input, (u_char *) buf, len) < 0) {
                                 ArgusLog (LOG_ALERT, "%s: write remote filter error %s.", strerror(errno));
                                 close(input->fd);
                                 input->fd = -1;
                                 return (retn);
                              } 

                              if (input->major_version >= MAJOR_VERSION_3) {
                                 if ((cnt = read (input->fd, buf, 2)) != 2) {
                                    ArgusLog (LOG_ALERT, "remote Filter error");
                                    close(input->fd);
                                    input->fd = -1;
                                    return (retn);
                                 }

                                 if (strncmp((char *)buf, "OK", 2)) {
                                    ArgusLog (LOG_ALERT, "remote filter syntax error.");
                                    close(input->fd);
                                    input->fd = -1;
                                    return (retn);
                                 }
                              }
                           }

                           if (input->filename) {
                              int len = 0;
                              snprintf ((char *) buf, MAXSTRLEN-1, "FILE: %s", input->filename);
                              len = strlen((char *) buf);
                              if (ArgusWriteConnection (parser, input, (u_char *) buf, len) < 0) {
                                 ArgusLog (LOG_ALERT, "%s: write FILE indication error %s.", strerror(errno));
                                 close(input->fd);
                                 input->fd = -1;
                                 return (retn);
                              }
#ifdef ARGUSDEBUG
                              ArgusDebug (3, "ArgusReadConnection() sent %s to remote\n", buf);
#endif
                              if ((cnt = recv (input->fd, buf, MAXSTRLEN - 1, 0)) < 0) {
                                 ArgusLog (LOG_ALERT, "remote error recv %d bytes, %s", cnt, strerror(errno));
                                 close(input->fd);
                                 input->fd = -1;
                                 return (retn);

                              } else {
                                 unsigned int filesize;
                                 filesize = *(int *)buf;
#ifdef _LITTLE_ENDIAN
                                 filesize = ntohl(filesize);
#endif

                                 if (cnt != sizeof(filesize)) {
                                    if (cnt == 2) 
                                       ArgusLog (LOG_ALERT, "remote file error");
                                    else
                                       ArgusLog (LOG_ALERT, "remote file error recv %d bytes, %s", cnt, strerror(errno));

                                    close(input->fd);
                                    input->fd = -1;
                                    return (retn);
                                 }
                              }
                           }

                           if (input->major_version >= MAJOR_VERSION_3) {
                              if (!parser->RaPollMode) {
                                 sprintf ((char *) buf, "START: ");
                                 len = strlen((char *) buf);
                                 if (ArgusWriteConnection (parser, input, (u_char *) buf, len) < 0) {
                                    ArgusLog (LOG_ALERT, "%s: write remote START msg error %s.", strerror(errno));
                                    close(input->fd);
                                    input->fd = -1;
                                    return (retn);
                                 }
                              }
                           }

                           if (input->filename) {
                              if ((cnt = read (input->fd, &argus, 16)) == 16) {
#ifdef ARGUSDEBUG
                                 ArgusDebug (2, "ArgusReadConnection() read %d bytes\n", cnt);
#endif
                                 input->offset = 16;

                                 if (argus.argus_mar.argusid == htonl(ARGUS_COOKIE)) {
                                    int size = sizeof(argus) - 16;

                                    if ((cnt = read (input->fd, &((u_char *)&argus)[16], size)) != size) {
#ifdef ARGUSDEBUG
                                       ArgusDebug (1, "ArgusReadConnection() read failed for ARGUS_START Mar %s.\n", strerror(errno));
#endif
                                       close (input->fd);
                                       input->fd = -1;
                                       return (retn);
                                    } else {
#ifdef ARGUSDEBUG
                                       ArgusDebug (5, "ArgusReadConnection() read %d bytes\n", cnt);
#endif
                                    }
                                    input->offset += cnt;
                                    input->major_version = argus.argus_mar.major_version;
                                    input->minor_version = argus.argus_mar.minor_version;
                                    bcopy ((char *) &argus, (char *)&input->ArgusInitCon, sizeof (argus));
                                    bcopy ((char *) &argus, (char *)&input->ArgusManStart, sizeof (argus));
#ifdef _LITTLE_ENDIAN
                                    ArgusNtoH((struct ArgusRecord *)&input->ArgusManStart);
#endif
                                    input->ArgusID = ntohl(argus.argus_mar.thisid);
                                    input->ArgusReadSize = argus.argus_mar.record_len;

                                    fstat(input->fd, &input->statbuf);
                                    ArgusParseInit (parser, input);
                                    found++;

                                 } else {
                                    struct ArgusV2Record *argus2 = (struct ArgusV2Record *) &argus;
                                 
                                    if (argus2->ahdr.type & ARGUS_V2_MAR) {
                                       u_short length = ntohs(argus2->ahdr.length);
                                       u_int argusid   = ntohl(argus2->ahdr.argusid);
                                       u_int status   = ntohl(argus2->ahdr.status);
                                       u_int sequence = ntohl(argus2->ahdr.seqNumber);

                                       if (argus2->ahdr.cause & ARGUS_V2_ERROR) {
#ifdef ARGUSDEBUG
                                          ArgusDebug (1, "ArgusReadConnection() ARGUS_V2_ERROR Mar.\n");
#endif
                                          if (status & ARGUS_V2_MAXLISTENEXCD) {
                                             ArgusLog (LOG_ALERT, "remote exceed listen error.");
                                             close (input->fd);
                                             input->fd = -1;
                                             return (retn);
                                          }
                                       }

                                       if (argus2->ahdr.cause == ARGUS_V2_START) {
#ifdef ARGUSDEBUG
                                          ArgusDebug (5, "ArgusReadConnection() ARGUS_V2_START Mar.\n");
#endif
                                          input->type = ARGUS_V2_DATA_SOURCE;
                                          if ((argusid == ARGUS_V2_COOKIE) && (sequence == 0)) {
                                             int size = length - sizeof(argus2->ahdr);
                              
                                             if ((cnt = read (input->fd, &argus2->argus_mar, size)) != size) {
#ifdef ARGUSDEBUG
                                                ArgusDebug (1, "ArgusReadConnection() read failed for ARGUS_START Mar %s.\n", strerror(errno));
#endif
                                                close (input->fd);
                                                input->fd = -1;
                                                return (retn);
                                             }

                                             input->offset += cnt;
                                             ptr = ArgusConvertRecord(input, (char *)argus2);
                                             bcopy ((char *) ptr, (char *)&input->ArgusInitCon, sizeof (*argus2));

                                             fstat(input->fd, &input->statbuf);
#ifdef _LITTLE_ENDIAN
                                             ArgusNtoH((struct ArgusRecord *)argus2);
#endif
                                             bcopy ((char *) argus2, (char *)&input->ArgusManStart, sizeof (*argus2));
                                             input->major_version = MAJOR_VERSION_2;
                                             input->minor_version = MINOR_VERSION_0;
                                             input->ArgusReadSize = ((struct ArgusRecord *)argus2)->argus_mar.record_len;

                                             ArgusParseInit (parser, input);
                                             found++;
                              
                                          } else {
                                             ArgusLog (LOG_ALERT, "ArgusReadConnection: not Argus-2.0 data stream.");
                                             close (input->fd);
                                             input->fd = -1;
                                             return (retn);
                                          }
                                       }
                                    }
                                 }

                              } else {
                                 ArgusLog (LOG_ALERT, "ArgusReadConnection: %s", strerror(errno));
                                 close (input->fd);
                                 input->fd = -1;
                                 return (retn);
                              }
                           }
                        }
                     }

                     retn = 1;

                  } else {
                     if (input->fd >= 0) {
                        close (input->fd);
                        input->fd = -1;
                        return (retn);
                     }
                  }
               }
               break;
            }

            case ARGUS_JFLOW_DATA_SOURCE:
               ArgusParseInit (parser, input);
               retn = 0;
#ifdef ARGUSDEBUG
               ArgusDebug (3, "ArgusReadConnection(0x%x, %d) reading juniper wire format", input, type);
#endif
               break;

            case ARGUS_SFLOW_DATA_SOURCE:
               ArgusParseInit (parser, input);
               retn = 0;
#ifdef ARGUSDEBUG
               ArgusDebug (3, "ArgusReadConnection(0x%x, %d) reading inmon wire format", input, type);
#endif
               break;

            case ARGUS_CISCO_DATA_SOURCE:
               ArgusParseInit (parser, input);
               retn = 0;
#ifdef ARGUSDEBUG
               ArgusDebug (3, "ArgusReadConnection(0x%x, %d) reading cisco wire format", input, type);
#endif
               break;

            default:
               ArgusLog (LOG_ERR, "ArgusReadConnection(0x%x) unknown source type", input);
               break;
         }
         break;
      }

      default:
         ArgusLog (LOG_ERR, "ArgusReadConnection(0x%x) unknown stream type", input);
         break;
   }

#ifdef ARGUSDEBUG
   ArgusDebug (3, "ArgusReadConnection(0x%x, %d) returning %d\n", input, type, retn);
#endif

   return (retn);
}



void ArgusRecordDump (struct ArgusRecord *);
void ArgusDump (const u_char *, int, char *);
int setArgusRemoteFilter(struct ArgusParserStruct *, char *);

int
setArgusRemoteFilter(struct ArgusParserStruct *parser, char *str)
{
   struct ArgusInput *input = NULL;
   char buf[MAXSTRLEN];
   int retn = 0, len;

   if (str != NULL) {
      if (strcmp(parser->ArgusRemoteFilter, str)) {
         if (parser->ArgusRemoteFilter != NULL)
            free(parser->ArgusRemoteFilter);

         parser->ArgusRemoteFilter = str;

         if ((input = (struct ArgusInput *)parser->ArgusRemoteHosts->start) != NULL) {
            do {
               if ((input->type & ARGUS_DATA_SOURCE) && (input->filename == NULL)) {
                  snprintf ((char *) buf, MAXSTRLEN-1, "FILTER: man or (%s)",
                                       (char *) parser->ArgusRemoteFilter);
                  len = strlen((char *) buf);
                  if (ArgusWriteConnection (parser, input, (u_char *) buf, len) < 0) {
                     ArgusLog (LOG_ALERT, "%s: write remote filter error %s.", strerror(errno));
                  }
               }
               input = (struct ArgusInput *)input->qhdr.nxt;
            } while (input != (struct ArgusInput *)parser->ArgusRemoteHosts->start);
         }
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (1, "setArgusRemoteFilter(0x%x, %s) returning %d\n", parser, str, retn);
#endif

   return (retn);
}

int
ArgusWriteConnection (struct ArgusParserStruct *parser, struct ArgusInput *input, u_char *buf, int cnt)
{
   int retn = 0, fd = 0;
   unsigned int len = cnt;
   u_char *output = NULL;

   if (input != NULL) {
      fd = input->fd;
      if ((fd > 2) || (parser->dflag && (fd >= 0))) {
#ifdef ARGUS_SASL
         u_char outputbuf[MAXARGUSRECORD];
         output = outputbuf;
         len = cnt;

         if (input->sasl_conn) {
            const int *ssfp;
            int result;

            if ((result = sasl_getprop(input->sasl_conn, SASL_SSF, (const void **) &ssfp)) != SASL_OK)
               ArgusLog (LOG_ERR, "sasl_getprop: error %s\n", sasl_errdetail(input->sasl_conn));

            if (ssfp && (*ssfp > 0)) {

#ifdef ARGUSDEBUG
               ArgusDebug (5, "ArgusWriteConnection: sasl_encode(0x%x, 0x%x, %d, 0x%x, 0x%x)\n",
                                            input->sasl_conn, buf, cnt, &output, &len);
#endif
               if ((retn = sasl_encode(input->sasl_conn, (char *)buf, (u_int) cnt, (const char **) &output, &len)) != SASL_OK)
                  ArgusLog (LOG_ERR, "sasl_encode: failed returned %d", retn);

            } else 
               output = buf;
         } else
            output = buf;
#else
         output = buf;

#endif /* ARGUS_SASL */

#ifdef ARGUSDEBUG
         ArgusDebug (4, "ArgusWriteConnection: write(%d, 0x%x, %d)\n", fd, output, len);
#endif
         while ((retn = write(fd, output, len)) != len) {
            if (retn >= 0) {
               output += retn;
               len -= retn;
            } else {
               if ((errno != EAGAIN) && (errno != EINTR))
                  break;
            }
         }
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (3, "ArgusWriteConnection(0x%x, 0x%x, %d) returning %d\n", input, buf, cnt, len);
#endif

   return (retn);
}


void
ArgusCloseInput(struct ArgusParserStruct *parser, struct ArgusInput *input)
{
   if ((input != NULL) && (input->status & ARGUS_CLOSED)) {
      input->fd = -1;
      return;
   }

#if defined(ARGUS_THREADS)
   pthread_mutex_lock(&input->lock); 
#endif

#ifdef ARGUSDEBUG
   ArgusDebug (3, "ArgusCloseInput(0x%x) closing", input);
#endif

   input->status |= ARGUS_CLOSED;

   if (input->pipe) {
      pclose(input->pipe);
      input->pipe = NULL;
      input->file = NULL;
   }

   if (parser->Sflag)
      ArgusWriteConnection (parser, input, (u_char *)"DONE: ", strlen("DONE: "));

   input->ArgusReadSocketCnt = 0;
   input->ArgusReadSocketSize = 0;

   if (input->qhdr.queue != NULL)
      ArgusRemoveFromQueue(input->qhdr.queue, &input->qhdr, ARGUS_LOCK);

   if (parser->RaCloseInputFd && (input->fd >= 0)) {
      if (close (input->fd))
         ArgusLog (LOG_ERR, "ArgusCloseInput: close error %s", strerror(errno));

      input->fd = -1;
 
      if (!(ArgusParser->RaShutDown) && (parser->ArgusReliableConnection)) {
         ArgusAddToQueue(parser->ArgusRemoteHosts, &input->qhdr, ARGUS_LOCK);

      } else {
         parser->ArgusRemotes--;
      }
   }

   if (input->ArgusReadBuffer != NULL) {
      ArgusFree(input->ArgusReadBuffer);
      input->ArgusReadBuffer = NULL;
   }

   if (input->ArgusConvBuffer != NULL) {
     ArgusFree(input->ArgusConvBuffer);
     input->ArgusConvBuffer = NULL;
   }

#ifdef ARGUS_SASL
   if (input->ArgusSaslBuffer != NULL) {
      ArgusFree(input->ArgusSaslBuffer);
      input->ArgusSaslBuffer = NULL;
   }
#endif /* ARGUS_SASL */

   if (parser->RaCloseInputFd && (input->file != NULL)) {
      if (fclose (input->file))
         ArgusLog (LOG_ERR, "ArgusCloseInput: close error %s", strerror(errno));
      input->file = NULL;
   }

#if defined(ARGUS_THREADS)
   pthread_mutex_unlock(&input->lock);
#endif

#ifdef ARGUSDEBUG
   ArgusDebug (3, "ArgusCloseInput(0x%x) done\n", input);
#endif
}

#define HEXDUMP_BYTES_PER_LINE 16
#define HEXDUMP_SHORTS_PER_LINE (HEXDUMP_BYTES_PER_LINE / 2)
#define HEXDUMP_HEXSTUFF_PER_SHORT 5 /* 4 hex digits and a space */
#define HEXDUMP_HEXSTUFF_PER_LINE \
                (HEXDUMP_HEXSTUFF_PER_SHORT * HEXDUMP_SHORTS_PER_LINE)


void
ArgusRecordDump (struct ArgusRecord *argus)
{
   int length = argus->hdr.len;
   const u_char *cp = (const u_char *) argus;

   ArgusDump (cp, length, NULL);
}

void
ArgusDump (const u_char *cp, int length, char *prefix)
{
   u_int oset = 0;
   register u_int i;
   register int s1, s2;
   register int nshorts;
   char hexstuff[HEXDUMP_SHORTS_PER_LINE*HEXDUMP_HEXSTUFF_PER_SHORT+1], *hsp;
   char asciistuff[HEXDUMP_BYTES_PER_LINE+1], *asp;

#ifdef ARGUSDEBUG
   ArgusDebug (2, "ArgusDump(0x%x, %d)", cp, length);
#endif

   nshorts = length / sizeof(u_short);
   i = 0;
   hsp = hexstuff; asp = asciistuff;
   while (--nshorts >= 0) {
           s1 = *cp++;
           s2 = *cp++;
           (void)snprintf(hsp, sizeof(hexstuff) - (hsp - hexstuff),
               " %02x%02x", s1, s2);
           hsp += HEXDUMP_HEXSTUFF_PER_SHORT;
           *(asp++) = (isgraph(s1) ? s1 : '.');
           *(asp++) = (isgraph(s2) ? s2 : '.');
           if (++i >= HEXDUMP_SHORTS_PER_LINE) {
               *hsp = *asp = '\0';
               if (prefix != NULL)
                  (void)printf("\n%s0x%04x\t%-*s\t%s", prefix,
                            oset, HEXDUMP_HEXSTUFF_PER_LINE,
                            hexstuff, asciistuff);
               else
                  (void)printf("\n0x%04x\t%-*s\t%s",
                            oset, HEXDUMP_HEXSTUFF_PER_LINE,
                            hexstuff, asciistuff);
               i = 0; hsp = hexstuff; asp = asciistuff;
               oset += HEXDUMP_BYTES_PER_LINE;
           }
   }
   if (length & 1) {
      s1 = *cp++;
      (void)snprintf(hsp, sizeof(hexstuff) - (hsp - hexstuff), " %02x", s1);
      hsp += 3;
      *(asp++) = (isgraph(s1) ? s1 : '.');
      ++i;
   }
   if (i > 0) {
      *hsp = *asp = '\0';
      if (prefix != NULL)
         (void)printf("\n%s0x%04x\t%-*s\t%s", prefix,
                        oset, HEXDUMP_HEXSTUFF_PER_LINE,
                        hexstuff, asciistuff);
      else
         (void)printf("\n0x%04x\t%-*s\t%s",
                        oset, HEXDUMP_HEXSTUFF_PER_LINE,
                        hexstuff, asciistuff);
   }
   (void)printf("\n");
}



int
ArgusAddMaskList (struct ArgusParserStruct *parser, char *ptr)
{
   int retn = 0;
   struct ArgusModeStruct *mode, *list;

   if (ptr) {
      if ((mode = (struct ArgusModeStruct *) ArgusCalloc (1, sizeof(struct ArgusModeStruct))) != NULL) {
         if ((list = parser->ArgusMaskList) != NULL) {
            while (list->nxt)
               list = list->nxt;
            list->nxt = mode;
         } else
            parser->ArgusMaskList = mode;

         mode->mode = strdup(ptr);
         retn = 1;
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (8, "ArgusAddMaskList (%s) returning %d\n", ptr, retn);
#endif

   return (retn);
}

void
ArgusDeleteMaskList (struct ArgusParserStruct *parser)
{

   if (parser && parser->ArgusMaskList) {
      struct ArgusModeStruct *mode = parser->ArgusMaskList;
 
      while (mode) {
        if (mode->mode)
           free(mode->mode);
 
        mode = mode->nxt;
        ArgusFree(parser->ArgusMaskList);
        parser->ArgusMaskList = mode;
      }
   }
 
#ifdef ARGUSDEBUG
   ArgusDebug (2, "ArgusDeleteMaskList () returning\n");
#endif
}


int
ArgusAddModeList (struct ArgusParserStruct *parser, char *ptr)
{
   int retn = 0;
   struct ArgusModeStruct *mode, *list;

   if (ptr) {
      if ((mode = (struct ArgusModeStruct *) ArgusCalloc (1, sizeof(struct ArgusModeStruct))) != NULL) {
         if ((list = parser->ArgusModeList) != NULL) {
            while (list->nxt)
               list = list->nxt;
            list->nxt = mode;
         } else
            parser->ArgusModeList = mode;

         mode->mode = strdup(ptr);
         retn = 1;
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (8, "ArgusAddModeList (%s) returning %d\n", ptr, retn);
#endif

   return (retn);
}

void
ArgusDeleteModeList (struct ArgusParserStruct *parser)
{

   if (parser && parser->ArgusModeList) {
      struct ArgusModeStruct *mode = parser->ArgusModeList;

      while (mode) {
        if (mode->mode)
           free(mode->mode);

        mode = mode->nxt;
        ArgusFree(parser->ArgusModeList);
        parser->ArgusModeList = mode;
      }
   }

#ifdef ARGUSDEBUG 
   ArgusDebug (2, "ArgusDeleteModeList () returning\n");
#endif
}

int
ArgusAddFileList (struct ArgusParserStruct *parser, char *ptr, int type, long long ostart, long long ostop)
{
   int retn = 0;
   struct ArgusInput *file, *list;

   if (ptr) {
      if ((file = (struct ArgusInput *) ArgusCalloc (1, sizeof(struct ArgusInput))) != NULL) {
         if ((list = parser->ArgusInputFileList) != NULL) {
            while (list->qhdr.nxt)
               list = (struct ArgusInput *)list->qhdr.nxt;
            list->qhdr.nxt = &file->qhdr;
         } else
            parser->ArgusInputFileList = file;

         file->ArgusOriginal = (struct ArgusRecord *)&file->ArgusOriginalBuffer;
         file->type = type;
         file->ostart = ostart;
         file->ostop = ostop;
         file->filename = strdup(ptr);
         file->fd = -1;
         retn = 1;
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (2, "ArgusAddFileList (0x%x, %s, %d, %d, %d) returning %d\n", parser, ptr, type, ostart, ostop, retn);
#endif

   return (retn);
}

void
ArgusDeleteFileList (struct ArgusParserStruct *parser)
{
   if (parser && parser->ArgusInputFileList) {
      struct ArgusInput *addr = parser->ArgusInputFileList;

      while (addr) {
        if (addr->filename)
           free(addr->filename);

        addr = (struct ArgusInput *)addr->qhdr.nxt;
        ArgusFree(parser->ArgusInputFileList);
        parser->ArgusInputFileList = addr;
      }
   }

#ifdef ARGUSDEBUG
   ArgusDebug (2, "ArgusDeleteFileList () returning\n");
#endif
}


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*
   Format:
      [proto://]host[:port[/file]]
*/

int
ArgusAddHostList (struct ArgusParserStruct *parser, char *host, int type, int proto)
{
   static char *str, strbuf[MAXSTRLEN], msgbuf[MAXSTRLEN];
   static char *protoStr = "tcp", portbuf[16];
   char *fptr = NULL, *tptr = NULL, *sptr = NULL;
   char *ptr = NULL, *endptr = NULL;
   char *hptr = NULL, *file = NULL;
   struct ArgusInput *addr = NULL;
   char *servname = NULL;
   long int portnum = 0;
   int retn = 0;

#if defined(HAVE_GETADDRINFO)
   struct addrinfo hints, *hp = NULL;
#else
   struct hostent *hp = NULL;
#endif

   strncpy (strbuf, host, MAXSTRLEN);
   str = strbuf;

   while ((sptr = strtok(str, " ")) != NULL) {
      char *pptr = sptr;
      if ((ptr = strstr(pptr, "://")) != NULL) {
         ptr = &ptr[3];
         if (!(strncmp("argus-tcp:", pptr, 9)) || (!(strncmp("argus:", pptr, 9)))) {
         } else 
         if (!(strncmp("argus-udp:", pptr, 9))) {
            protoStr = "udp";
            proto = IPPROTO_UDP;
         } else
         if (!(strncmp("sflow:", pptr, 6))) {
            protoStr = "udp";
            type = ARGUS_SFLOW_DATA_SOURCE;
            proto = IPPROTO_UDP;
         } else
         if (!(strncmp("jflow:", pptr, 6))) {
            protoStr = "udp";
            type = ARGUS_JFLOW_DATA_SOURCE;
            proto = IPPROTO_UDP;
         } else
         if (!(strncmp("cisco:", pptr, 6))) {
            protoStr = "udp";
            type = ARGUS_CISCO_DATA_SOURCE;
            proto = IPPROTO_UDP;
         } else
         if (!(strncmp("ipfix-sctp:", pptr, 6))) {
            protoStr = "sctp";
            type = ARGUS_IPFIX_DATA_SOURCE;
            proto = IPPROTO_SCTP;
         } else
         if (!(strncmp("ipfix-tcp:", pptr, 6))) {
            protoStr = "tcp";
            type = ARGUS_IPFIX_DATA_SOURCE;
            proto = IPPROTO_TCP;
         } else
         if (!(strncmp("ipfix-udp:", pptr, 6))) {
            protoStr = "udp";
            type = ARGUS_IPFIX_DATA_SOURCE;
            proto = IPPROTO_UDP;
         } else
         if (!(strncmp("ipfix:", pptr, 6))) {
            protoStr = "tcp";
            type = ARGUS_IPFIX_DATA_SOURCE;
            proto = IPPROTO_TCP;
         }

      } else {
         ptr = sptr;
         pptr = NULL;
      }

      if ((fptr = strchr (ptr, (int)'/')) != NULL) {
         file = strdup(fptr);
         *fptr = '\0';
      }

      if ((tptr = strchr (ptr, (int)'[')) != NULL) {
         hptr = tptr + 1;
         if ((tptr = strchr (ptr, (int)']')) != NULL) {
            *tptr++ = '\0';
            if ((tptr = strchr (tptr, (int)':')) != NULL) {
               *tptr++ = '\0';
               portnum = strtol(tptr, &endptr, 10);
               if (endptr != &tptr[strlen(tptr)]) {
                  ArgusLog (LOG_ALERT, "ArgusAddHostList(%s) format error %s is not a port number", tptr);
               } else
                  servname = tptr;
            }
         } else {
            ArgusLog (LOG_ALERT, "ArgusAddHostList(%s) literal ipv6 format error %s: no terminating ']'", host, ptr);
         }

      } else {
         char *cptr = ptr;
         int cnt = 0;
         tptr = NULL;

         while (*cptr != '\0') if (*cptr++ == ':') { cnt++; tptr = cptr;}

         if ((cnt == 1) && (tptr != NULL)) {
            hptr = ptr;
            if ((cptr = strrchr(ptr, ':')) != NULL)
               *cptr = '\0';
            
            portnum = strtol(tptr, &endptr, 10);
            if (endptr == tptr) {
               ArgusLog (LOG_ALERT, "ArgusAddHostList(%s) format error %s is not a port number", host, tptr);
            } else
               servname = tptr;

         } else {
            if ((cnt > 1) && (tptr != NULL)) {  // IPv6 address, should we look for a port?  No.
               portnum = 0;
               hptr = ptr;
            } else 
            if (cnt == 0) {
               if (strchr (ptr, (int)'.')) {
                  portnum = 0;
                  hptr = ptr;
               } else 
               if (isdigit((int)*ptr)) {
                  portnum = strtol(ptr, &endptr, 10);
                  if (endptr == ptr) {
                     portnum = 0;
                     hptr = ptr;
                  } else {
                     hptr = NULL;
                  }
               } else
                  hptr = ptr;
            }
         }
      }

      if (portnum == 0) {
         struct servent *sp;
         if (!parser->ArgusPortNum) {
            if ((sp = getservbyname ("monitor", protoStr)) != NULL)
               portnum = ntohs(sp->s_port);
            else
               portnum = ARGUS_DEFAULTPORT;
         } else
            portnum = parser->ArgusPortNum;

         if (servname == NULL) {
            servname = portbuf;
            snprintf(servname, 16, "%ld", portnum);
         }
      }

      if ((hptr != NULL) && (strlen(hptr) > 0)) {
#if defined(HAVE_GETADDRINFO)
         memset(&hints, 0, sizeof(hints));
//       hints.ai_family   = AF_INET;
         if ((type == ARGUS_CISCO_DATA_SOURCE) || (proto == IPPROTO_UDP)) {
            hints.ai_socktype = SOCK_DGRAM;
            hints.ai_protocol = IPPROTO_UDP;
            hints.ai_family   = AF_INET;
         } else
            hints.ai_socktype = SOCK_STREAM;
 
         if (!(strncasecmp("any", hptr, 3))) {
            hptr = NULL;
            hints.ai_flags = AI_PASSIVE;
         }

         if ((retn = getaddrinfo(hptr, servname, &hints, &hp)) != 0) {
            switch (retn) {
               case EAI_AGAIN: 
                  sprintf (msgbuf, "dns server not available");
                  break;
               case EAI_NONAME:
                  sprintf (msgbuf, "host %s unknown", ptr);
                  break;
#if defined(EAI_ADDRFAMILY)
               case EAI_ADDRFAMILY:
                  sprintf (msgbuf, "host %s has no IP address", ptr);
                  break;
#endif
               case EAI_SYSTEM:
               default:
                  sprintf (msgbuf, "host '%s' %s", ptr, gai_strerror(retn));
                  break;
            }
         }
#else
         if ((hp = gethostbyname(hptr)) != NULL) {
            u_int **p;
            for (p = (u_int **)hp->h_addr_list; *p; ++p)
               **p = ntohl(**p);
         } else {
            switch (h_errno) {
               case TRY_AGAIN:
                  sprintf (msgbuf, "dns server not available");
                  break;
               case HOST_NOT_FOUND:
                  sprintf (msgbuf, "host %s unknown", ptr);
                  break;
               case NO_ADDRESS:
                  sprintf (msgbuf, "host %s has no IP address", ptr);
                  break;
               case NO_RECOVERY:
                  sprintf (msgbuf, "host %s name server error", ptr);
                  break;
            }
         }
#endif
      }

      str = NULL;

      if ((type != ARGUS_CISCO_DATA_SOURCE) && (hp == NULL)) {
         if (hptr != NULL) {
            ArgusLog (LOG_ALERT, "%s", msgbuf);
         } else
            ArgusLog (LOG_ERR, "error: port only requires -C option");
      }

         if ((addr = (struct ArgusInput *) ArgusCalloc (1, sizeof (struct ArgusInput))) != NULL) {
            addr->fd      = -1;
#if defined(HAVE_GETADDRINFO)
            addr->host    = hp;
#else
            if (hp != NULL) {
               addr->addr.s_addr = **(u_int **)hp->h_addr_list;
               addr->hostname = strdup(hp->h_name);
            }
#endif
            if ((addr->hostname == NULL) && (hptr != NULL)) 
               addr->hostname = strdup(hptr);

            addr->filename = file;
            if (servname != NULL) addr->servname = strdup(servname);
            addr->portnum = portnum;
            addr->ArgusOriginal = (struct ArgusRecord *)&addr->ArgusOriginalBuffer;
            addr->type = type;
            addr->mode = proto;

            if (!(addr->portnum = portnum)) {
               struct servent *sp;
               if (!parser->ArgusPortNum) {
                  if ((sp = getservbyname ("monitor", "tcp")) != NULL)
                     addr->portnum = ntohs(sp->s_port);
                  else
                     addr->portnum = ARGUS_DEFAULTPORT;
               } else
                  addr->portnum = parser->ArgusPortNum;
            }

            addr->status |= type;
            addr->index = -1;
            addr->ostart = -1;
            addr->ostop = -1;

#if defined(ARGUS_THREADS)
            pthread_mutex_init(&addr->lock, NULL);
#endif
            ArgusAddToQueue(parser->ArgusRemoteHosts, &addr->qhdr, ARGUS_LOCK);
            retn = 1;

         } else
            ArgusLog (LOG_ERR, "ArgusAddHostList(%s) ArgusCalloc %s", str, strerror(errno));
   }

#ifdef ARGUSDEBUG
   ArgusDebug (2, "ArgusAddHostList (0x%x, %s, %d, %d) returning %d\n", parser, host, type, proto, retn);
#endif

   return (retn);
}

void
ArgusDeleteHostList (struct ArgusParserStruct *parser)
{
   struct ArgusInput *input = parser->ArgusRemoteHostList, *prv;

   while ((prv = input) != NULL) {
      ArgusCloseInput(parser, input);
      input = (struct ArgusInput *)input->qhdr.nxt; 
      ArgusFree(prv);
   }

   parser->ArgusRemoteHostList = NULL;

#ifdef ARGUSDEBUG
   ArgusDebug (2, "ArgusDeleteHostList () returning\n");
#endif
}


int
ArgusWriteNewLogfile (struct ArgusParserStruct *parser, struct ArgusInput *input, struct ArgusWfileStruct *wfile, struct ArgusRecord *argus)
{
   char *file = NULL;
   int retn = 0;

   if ((wfile == NULL) || (argus == NULL))
      ArgusLog (LOG_ERR, "ArgusWriteNewLogfile() parameter/system init error");
   else
      file = wfile->filename;

   if (*file != '-') {
      if (strncmp(file, "/dev/null", 9)) {
         if (parser->ArgusRealTime.tv_sec > wfile->laststat.tv_sec) {
            if ((stat (file, &wfile->statbuf) < 0)) {
               if (wfile->fd != NULL) {
                  if (fflush (wfile->fd) != 0)
                     ArgusLog (LOG_ERR, "ArgusWriteNewLogfile(%s, 0x%x) fflush error %s", file, argus, strerror(errno));
                  fclose (wfile->fd);
                  wfile->fd = NULL;
               }

            } else {
               if (wfile->statbuf.st_size == 0)
                  wfile->firstWrite++;
            }
            wfile->laststat = parser->ArgusRealTime;
         }
      }

      if (wfile->fd == NULL) {
         char realpathname[MAXSTRLEN], *tptr, *pptr;
         struct stat statbuf;

         sprintf (realpathname, "%s", file);

         if ((tptr = strrchr(realpathname, (int) '/')) != NULL) {
            *tptr = '\0';
            pptr = tptr;

            while ((pptr != NULL) && ((stat(realpathname, &statbuf)) < 0)) {
               switch (errno) {
                  case ENOENT:
                     if ((pptr = strrchr(realpathname, (int) '/')) != NULL) {
                        if (pptr != realpathname) {
                           *pptr = '\0';
                        } else {
                           pptr = NULL;
                        }
                     }
                     break;

                  default:
                     ArgusLog (LOG_ERR, "stat: %s %s\n", realpathname, strerror(errno));
               }
            }

            while (&realpathname[strlen(realpathname)] <= tptr) {
               if ((mkdir(realpathname, 0777)) < 0) {
                  if (errno != EEXIST)
                     ArgusLog (LOG_ERR, "mkdir: %s %s\n", realpathname, strerror(errno));
               }
               realpathname[strlen(realpathname)] = '/';
            }
            *tptr = '/';
         }

         if ((wfile->fd = fopen (file, "a+")) == NULL)
            ArgusLog (LOG_ERR, "ArgusWriteNewLogfile(%s, 0x%x) fopen %s", file, argus, strerror(errno));
         else {
            fstat (fileno(wfile->fd), &wfile->statbuf);
            if (wfile->statbuf.st_size == 0)
               wfile->firstWrite++;
         }
      }

   } else {
      if (wfile->fd == NULL) {
         wfile->fd = stdout;
         wfile->firstWrite++;
      }
   }

   if (wfile->firstWrite) {
      int len = ntohs(parser->ArgusInitCon.hdr.len) * 4;
      if (len == 0) {
         struct ArgusRecord *argus = (struct ArgusRecord *)&parser->ArgusInitCon;
         argus->hdr.type   = (ARGUS_MAR | ARGUS_VERSION);
         argus->hdr.cause  = ARGUS_START;
         argus->hdr.len    = (unsigned short) sizeof(struct ArgusRecord)/4;

         len = argus->hdr.len * 4;

         argus->argus_mar.thisid     = 0;
         argus->argus_mar.argusid    = ARGUS_COOKIE;
         argus->argus_mar.startime.tv_sec  = parser->ArgusGlobalTime.tv_sec;
         argus->argus_mar.startime.tv_usec = parser->ArgusGlobalTime.tv_usec;
         argus->argus_mar.now        = argus->argus_mar.startime;
         argus->argus_mar.record_len = -1;

         ArgusHtoN(argus);
      }
      if (fwrite ((char *)&parser->ArgusInitCon, 1, len, wfile->fd) < 1) {
         if (ferror(wfile->fd) || feof(wfile->fd))
            RaParseComplete(SIGQUIT);
      }
      fflush(wfile->fd);
   }

   if ((argus != NULL) && (!(wfile->firstWrite && (argus == &input->ArgusInitCon)))) {
      int cnt, len = ntohs(argus->hdr.len) * 4;

      if (len > 0) {
         if ((cnt = fwrite (argus, 1, len, wfile->fd)) < 1) {
            if ((ferror(wfile->fd)) || feof(wfile->fd))
               RaParseComplete(SIGQUIT);
         } else {
#ifdef ARGUSDEBUG
            ArgusDebug (7, "ArgusWriteNewLogfile (%s, 0x%x) fwrite %d bytes", file, argus, len);
#endif
         }
      } else {
#ifdef ARGUSDEBUG
         ArgusDebug (7, "ArgusWriteNewLogfile (%s, 0x%x) record len is %d", file, argus, len);
#endif
      }
   }
   
   if (wfile->firstWrite)
      wfile->firstWrite = 0;

#ifdef ARGUSDEBUG
   ArgusDebug (6, "ArgusWriteNewLogfile (%s, 0x%x) returning %d\n", file, argus, retn);
#endif

   return (retn);
}


#include <netdb.h>
#include <sys/socket.h>       
#include <netinet/in.h>       
#include <arpa/inet.h>        
#include <ctype.h>
   
   
static char ArgusPidFileName[MAXPATHNAMELEN];      
char *ArgusCreatePIDFile (struct ArgusParserStruct *, char *);
int ArgusDeletePIDFile (struct ArgusParserStruct *);

char *
ArgusCreatePIDFile (struct ArgusParserStruct *parser, char *appname)
{
   FILE *fd;
   char pidstrbuf[128], *pidstr = pidstrbuf;
   char *retn = NULL, *homepath = NULL;
   struct stat statbuf;
   int pid;

   if (appname == NULL)
      appname = parser->ArgusProgramName;

   if ((homepath = parser->ArgusPidPath) == NULL)
      if (stat ("/var/run", &statbuf) == 0)
         homepath = "/var/run";

   if ((appname != NULL) && (homepath != NULL)) {
      snprintf (ArgusPidFileName, MAXPATHNAMELEN - 1, "%s/%s.pid", homepath, appname);
      retn = ArgusPidFileName;

      if ((stat (retn, &statbuf)) == 0) {
         if ((fd = fopen (ArgusPidFileName, "r")) != NULL) {
            if ((pidstr = fgets (pidstrbuf, 128, fd)) != NULL) {
               if ((pid = strtol(pidstr, (char **)NULL, 10)) > 0) {
                  if (pid < 100000000) {
                     if ((kill (pid, 0)) == 0) {
                        ArgusLog (LOG_ERR, "%s[%d] is already running!\n", appname, pid);
                     } else {
                        switch (errno) {
                           case ESRCH: break;
                           default: ArgusLog (LOG_ERR, "kill returned error: %s\n", strerror(errno));
                        }
                     }
                  }
               }
            }

            fclose (fd);
         } else {
            ArgusLog (LOG_ERR, "ArgusCreatePIDFile(%s, %s) fopen error %s:%s\n", homepath, appname, ArgusPidFileName, strerror(errno));
         }
      }

      if (retn && ((fd = fopen (retn, "w+")) != NULL)) {
         pid = getpid();
         fprintf (fd, "%d\n", pid);
         fclose (fd);
      } else {
         ArgusLog (LOG_ERR, "ArgusCreatePIDFile(%s, %s) fopen error %s:%s\n", homepath, appname, ArgusPidFileName, strerror(errno));
         retn = NULL;
      }

      parser->ArgusPidFile = retn;

   } else
      ArgusLog (LOG_ERR, "cannot create pidfile %s\n", ArgusPidFileName);

#ifdef ARGUSDEBUG
   ArgusDebug (1, "ArgusCreatePIDFile(0x%x) returning %s\n", parser, retn);
#endif

   return (retn);
}
   
int
ArgusDeletePIDFile (struct ArgusParserStruct *parser)
{
   char pidstrbuf[128], *pidstr = pidstrbuf;
   struct stat statbuf;
   pid_t mypid = getpid();
   int retn = 0;
   FILE *fd;
   int pid;

   if (parser->ArgusPidFile != NULL) {
      if (stat (parser->ArgusPidFile, &statbuf) == 0) {
         if ((fd = fopen (parser->ArgusPidFile, "r")) != NULL) {
            if ((pidstr = fgets (pidstrbuf, 128, fd)) != NULL) {
               if ((pid = strtol(pidstr, (char **)NULL, 10)) > 0) {
                  if (pid == (int) mypid) {
                     unlink(parser->ArgusPidFile);
                  } else {
#ifdef ARGUSDEBUG
                     ArgusDebug (1, "ArgusDeletePIDFile(%s) not owner pid %d\n", parser->ArgusPidFile, pid);
#endif
                  }
               }
            }
            fclose (fd);
         }
      }

#ifdef ARGUSDEBUG
      ArgusDebug (1, "ArgusDeletePIDFile(%s) returning %d\n", parser->ArgusPidFile, retn);
#endif
   } else {
#ifdef ARGUSDEBUG
      ArgusDebug (1, "ArgusDeletePIDFile(0x%x) returning %d, no pid file\n", parser, retn);
#endif
   }

   return (retn);
}

void
setArguspidflag (struct ArgusParserStruct *parser, int value)
{
   parser->pidflag = value;
}  

      
int
getArguspidflag (struct ArgusParserStruct *parser)
{  
   return (parser->pidflag);
}

void
setArgusArchive(struct ArgusParserStruct *parser, char *dir)
{
   parser->RadiumArchive = strdup(dir);

   if (getuid() == 0) {
      if (chroot(parser->RadiumArchive) < 0)
         ArgusLog (LOG_ERR, "setArgusArchive: chroot(%s) %s", dir, strerror(errno));

      if (chdir("/") < 0)
         ArgusLog (LOG_ERR, "setArgusArchive: chdir(/) %s", strerror(errno));
   }

#ifdef ARGUSDEBUG
   ArgusDebug (1, "setArgusArchive (0x%x, %s) returning\n", parser, dir);
#endif 
}

void
clearArgusWfile(struct ArgusParserStruct *parser)
{
   ArgusDeleteList (parser->ArgusWfileList, ARGUS_WFILE_LIST);
   parser->ArgusWfileList = NULL;
}


#include <libgen.h>

#if !defined(PATH_MAX)
#define PATH_MAX	4098
#endif

void
setArgusWfile(struct ArgusParserStruct *parser, char *file, char *filter)
{
   struct ArgusWfileStruct *wfile = NULL;
   char realpathname[PATH_MAX], *ptr = NULL;
   char *tptr, *pptr;
   FILE *fd = NULL;

   if (parser->ArgusWfileList == NULL)
      parser->ArgusWfileList = ArgusNewList();

   if (file) {
      if (strcmp (file, "-")) {
         if (!(strncmp(parser->ArgusProgramName, "radium", 6))  &&
             ((!(strncmp (file, "argus-udp://", 12))) || 
              (!(strncmp (file, "cisco-udp://", 12))) || 
              (!(strncmp (file,       "udp://", 6))))) {
            ptr = strdup(file);
         } else
         if ((strncmp(parser->ArgusProgramName,  "rasplit", 7)) &&
             (strncmp(parser->ArgusProgramName, "rastream", 8))) {
            struct stat statbuf;
            sprintf (realpathname, "%s", file);

            if ((stat(realpathname, &statbuf)) < 0) {
               switch (errno) {
                  case ENOENT: {
                     if ((fd = fopen (file, "a+")) == NULL) {
                        if ((errno == ENOENT) || (errno == ENOTDIR)) {
                           if (strncmp(parser->ArgusProgramName, "radium", 6)) {
                              if ((tptr = strrchr(realpathname, (int) '/')) != NULL) {   /* if there is a path */
                                 *tptr = '\0';
                                 pptr = tptr;
    
                                 while ((pptr != NULL) && ((stat(realpathname, &statbuf)) < 0)) {
                                    switch (errno) {
                                       case ENOENT: 
                                          if ((pptr = strrchr(realpathname, (int) '/')) != NULL) {
                                             if (pptr != realpathname) {
                                                *pptr = '\0';
                                             } else {
                                                pptr = NULL; 
                                             }
                                          }
                                          break;

                                       default:
                                          ArgusLog (LOG_ERR, "stat: %s %s\n", realpathname, strerror(errno));
                                    }
                                 }

                                 while (&realpathname[strlen(realpathname)] <= tptr) {
                                    if ((mkdir(realpathname, 0777)) < 0) {
                                       if (errno != EEXIST)
                                          ArgusLog (LOG_ERR, "mkdir: %s %s\n", realpathname, strerror(errno));
                                    }
                                    realpathname[strlen(realpathname)] = '/';
                                 }
                                 *tptr = '/';
                              }
                           }

                           if ((fd = fopen (file, "a+")) == NULL)
                              ArgusLog (LOG_ERR, "setArgusWfile open %s %s", file, strerror(errno));
                        }
                     }

                     if (fd != NULL) {
                        fclose (fd);
                        bzero (realpathname, PATH_MAX);
                        if ((ptr = realpath (file, realpathname)) == NULL)
                           ArgusLog (LOG_ERR, "setArgusWfile, realpath %s %s", file, strerror(errno));
                        else
                           ptr = strdup(ptr);
                        unlink(file);
                     }
                     break;
                  }

                  default:
                     ArgusLog (LOG_ERR, "%s %s", file, strerror(errno));
                     break;
               }

            } else {
               bzero (realpathname, PATH_MAX);
               if ((ptr = realpath (file, realpathname)) == NULL)
                  ArgusLog (LOG_ERR, "setArgusWfile, realpath %s %s", file, strerror(errno));
               else
                  ptr = strdup(ptr);
            }
/*
            So do we remove the file and start anew, or are we appending to the data?
            This is a tough one, but the tradition is to append so lets do that.

            unlink (ptr);
*/

         } else 
            ptr = strdup(file);

      } else
         ptr = strdup(file);

      if ((wfile = (struct ArgusWfileStruct *) ArgusCalloc (1, sizeof (*wfile))) != NULL) {
         ArgusPushFrontList(parser->ArgusWfileList, (struct ArgusListRecord *)wfile, ARGUS_LOCK);

         wfile->filename  = ptr;

         if (filter) {
            wfile->filterstr = strdup(filter);

            if (ArgusFilterCompile (&wfile->filter, wfile->filterstr, ArgusParser->Oflag) < 0)
               ArgusLog (LOG_ERR, "setArgusWfile: ArgusFilterCompile returned error");
         }

         if (parser->exceptfile != NULL)
            if (!(strcmp(file, parser->exceptfile)))
               parser->exceptfile = ptr;

      } else
         ArgusLog (LOG_ERR, "setArgusWfile, ArgusCalloc %s", strerror(errno));

   } else
      ArgusLog (LOG_ERR, "setArgusWfile, file is null");
}

void
ArgusProcessLabelOptions(struct ArgusParserStruct *parser, char *label)
{
   if (label != NULL) {
      char buf[1024];
      int retn;
      
      bzero(buf, sizeof(buf));
      while (isspace((int) *label)) label++;
      if (*label == '\"') {
         char *ptr = &label[strlen(label)];
         while (*ptr != '\"') ptr--;
         if (ptr != label)
            *ptr = '\0';
         label++;
      }

      if ((retn = regcomp(&parser->lpreg, label, REG_EXTENDED | REG_NOSUB)) != 0) {
         char errbuf[MAXSTRLEN];
         if (regerror(retn, &parser->lpreg, errbuf, MAXSTRLEN))
            ArgusLog (LOG_ERR, "ArgusProcessLabelOption: label regex error %s", errbuf);
      }
   }
}


#define RA_ADD_OPTION           2
#define RA_SUB_OPTION           3

/*
#define ARGUS_TRANSPORT_INDEX		0
#define ARGUS_FLOW_INDEX		1
#define ARGUS_TIME_INDEX		2
#define ARGUS_METRIC_INDEX		3
#define ARGUS_AGR_INDEX			4
#define ARGUS_FRAG_INDEX		5
#define ARGUS_NETWORK_INDEX		5 
#define ARGUS_VLAN_INDEX                6 
#define ARGUS_MPLS_INDEX                7
#define ARGUS_JITTER_INDEX              8
#define ARGUS_IPATTR_INDEX              9
#define ARGUS_PSIZE_INDEX               10
#define ARGUS_SRCUSERDATA_INDEX		11
#define ARGUS_DSTUSERDATA_INDEX		12
#define ARGUS_MAC_INDEX                 13 
#define ARGUS_ICMP_INDEX		14
#define ARGUS_ENCAPS_INDEX              15
#define ARGUS_TIME_ADJ_INDEX		16
#define ARGUS_BEHAVIOR_INDEX            17
#define ARGUS_HISTO_INDEX               18
#define ARGUS_COR_INDEX			18
#define ARGUS_COCODE_INDEX		19
#define ARGUS_LABEL_INDEX               20
#define ARGUS_ASN_INDEX                 21
*/

char *ArgusDSRKeyWords[ARGUSMAXDSRTYPE] = {
   "trans",
   "flow",
   "time",
   "metric",
   "agr",
   "net",
   "vlan",
   "mpls",
   "jitter",
   "ipattr",
   "psize",
   "suser",
   "duser",
   "mac",
   "icmp",
   "encaps",
   "tadj",
   "behavior",
   "cor",
   "cocode",
   "label",
   "asn",
};


void ArgusProcessStripOptions(struct ArgusParserStruct *, char *);

void
ArgusProcessStripOptions(struct ArgusParserStruct *parser, char *options)
{
   if (options != NULL) {
      int x, RaOptionOperation, setValue = 0; 
      int ArgusFirstMOptionField = 1;
      char *ptr = options, *tok;    
                     
      while ((tok = strtok(ptr, " ,")) != NULL) {
         if (*tok == '-') {
            if (ArgusFirstMOptionField) {
               for (x = 0; x < ARGUSMAXDSRTYPE; x++)
                  parser->ArgusDSRFields[x] = 1;
               ArgusFirstMOptionField = 0;
            }
            ptr = tok + 1;
            RaOptionOperation = RA_SUB_OPTION;
         } else
         if (*tok == '+') {
            if (ArgusFirstMOptionField) {
               for (x = 0; x < ARGUSMAXDSRTYPE; x++)
                  parser->ArgusDSRFields[x] = 1;
               ArgusFirstMOptionField = 0;
            }
            ptr = tok + 1;
            RaOptionOperation = RA_ADD_OPTION;
         } else {
            if (ArgusFirstMOptionField) {
               bzero ((char *) parser->ArgusDSRFields, sizeof(parser->ArgusDSRFields));
               ArgusFirstMOptionField = 0;
            }
            ptr = tok;
            RaOptionOperation = RA_ADD_OPTION;
         }

         setValue = (RaOptionOperation == RA_ADD_OPTION) ? RA_ADD_OPTION : 0;

         for (x = 0; x < ARGUSMAXDSRTYPE; x++) {
            if (strlen(ArgusDSRKeyWords[x])) {
               if (!strncmp (ArgusDSRKeyWords[x], ptr, strlen(ArgusDSRKeyWords[x]))) {
                  parser->ArgusDSRFields[x] = setValue;
                  break;
               }
            }
         }
         ptr = NULL;
      }
   }

#ifdef ARGUS_V2DEBUG
   ArgusDebug (2, "ArgusProcessStripOptions (0x%x, %s)", parser, options);
#endif
}



void ArgusProcessSOptions(struct ArgusParserStruct *);

void
ArgusProcessSOptions(struct ArgusParserStruct *parser)
{
   int i, x, RaOptionOperation, RaOptionRank;
   char *soption = NULL, *ptr = NULL;
   char **endptr = NULL;
   int value = 0;

   if ((soption = parser->RaPrintOptionStrings[0]) != NULL) {
      if (!((*soption == '+') || (*soption == '-'))) {
         x = 0;
         while (parser->RaPrintAlgorithmList[x] != NULL) {
           ArgusFree(parser->RaPrintAlgorithmList[x]);
           parser->RaPrintAlgorithmList[x] = NULL;
           x++;
         }
      } else {
         x = 0;
         if (parser->RaPrintAlgorithmList[0] == NULL) {
            while (RaPrintAlgorithms[x]) {
               for (i = 0; i < MAX_PRINT_ALG_TYPES; i++) {
                  if (RaPrintAlgorithmTable[i].print == RaPrintAlgorithms[x]) {
                     if ((parser->RaPrintAlgorithmList[x] = ArgusCalloc(1, sizeof(*parser->RaPrintAlgorithm))) == NULL) 
                        ArgusLog (LOG_ERR, "ArgusCalloc error %s", strerror(errno));
            
                     bcopy(&RaPrintAlgorithmTable[i], parser->RaPrintAlgorithmList[x], sizeof(*parser->RaPrintAlgorithm));
                  }
               }
               x++;
            }
         }
      }
   }

   for (i = 0; i < ARGUS_MAX_S_OPTIONS; i++) {
      int RaNewLength = 0, RaNewIndex = 0;
      char *RaNewFormat = NULL;

      if ((soption = parser->RaPrintOptionStrings[i]) != NULL) {
         int found = 0;
         RaOptionOperation = RA_ADD_OPTION;
         RaOptionRank = -1;
         if ((*soption == '+') || (*soption == '-')) {
            if (*soption == '-')
               RaOptionOperation = RA_SUB_OPTION;

            soption++;
            if (isdigit((int)*soption)) {
               sscanf(soption, "%d", &RaOptionRank);
               while(isdigit((int)*soption)) soption++;
            }
         }

/* format is field[index,index-index]:len:format */

         if ((ptr = strchr(soption, '[')) != NULL) {
            char *cptr = NULL;
            int sind = -1, dind = -1;
            *ptr++ = '\0';
            while (*ptr != ']') {
               if (isdigit((int)*ptr)) {
                  dind = strtol(ptr, (char **)&cptr, 10);
                  if (cptr == ptr)
                     usage ();

                  if (sind < 0) 
                     sind = dind;

                  for (x = sind; x <= dind; x++)
                     RaNewIndex |= 0x01 << x;
                  
                  ptr = cptr;
                  if (*ptr != ']')
                     ptr++;
                  if (*cptr != '-')
                     sind = -1;
               } else
                  usage ();
            }

         } else
            ptr = soption;

         if ((ptr = strchr(soption, ':')) != NULL) {
            char *sptr = NULL;
            *ptr++ = '\0';
            if (isdigit((int)*ptr))
               if ((RaNewLength = strtol(ptr, endptr, 10)) == 0)
                  if (*endptr == ptr)
                     usage();
            if ((sptr = strchr(ptr, ':')) != NULL) {
               *sptr++ = '\0';
               RaNewFormat = sptr;
            }
         }

         if ((ptr = strchr(soption, '/')) != NULL) {
            *ptr++ = '\0';
            if ((value = strtol(ptr, endptr, 10)) == 0)
               if (*endptr == ptr)
                  usage();
         }

         if (!(strncmp("rec", soption, 3))) {
            switch (RaOptionOperation) {
               case RA_ADD_OPTION: ArgusSOptionRecord = 1; break;
               case RA_SUB_OPTION: ArgusSOptionRecord = 0; break;
            }
         } else {
            for (x = 0; x < MAX_PRINT_ALG_TYPES; x++) {
               if (strlen(RaPrintAlgorithmTable[x].field)) {
                  if (!strcmp (RaPrintAlgorithmTable[x].field, soption)) {
                     if (RaNewLength) {
                        RaPrintAlgorithmTable[x].length = RaNewLength;
                     }
                     if (RaNewIndex) {
                        RaPrintAlgorithmTable[x].index = RaNewIndex;
                     }
                     if (RaNewFormat) {
                        RaPrintAlgorithmTable[x].format = strdup(RaNewFormat);
                     }
                     switch (RaOptionOperation) {
                        case RA_ADD_OPTION:
                           if (RaOptionRank == -1) {
                              int z = 0;
                              while (parser->RaPrintAlgorithmList[z] != NULL) z++;
                              if (z < ARGUS_MAX_PRINT_ALG) {
                                 if ((parser->RaPrintAlgorithmList[z] = ArgusCalloc(1, sizeof(RaPrintAlgorithmTable[x]))) == NULL)
                                    ArgusLog (LOG_ERR, "ArgusCalloc error %s", strerror(errno));
                                 bcopy(&RaPrintAlgorithmTable[x], parser->RaPrintAlgorithmList[z], sizeof (RaPrintAlgorithmTable[x]));
                              }
                           } else {
                              int z = RaOptionRank;
                              while (parser->RaPrintAlgorithmList[z] != NULL) z++;
                              while (z != RaOptionRank) {
                                 parser->RaPrintAlgorithmList[z] = parser->RaPrintAlgorithmList[z - 1];
                                 parser->RaPrintAlgorithmList[z - 1] = NULL;
                                 z--;
                              }
                              if ((parser->RaPrintAlgorithmList[z] = ArgusCalloc(1, sizeof(RaPrintAlgorithmTable[x]))) == NULL)
                                 ArgusLog (LOG_ERR, "ArgusCalloc error %s", strerror(errno));
                              bcopy(&RaPrintAlgorithmTable[x], parser->RaPrintAlgorithmList[z], sizeof (RaPrintAlgorithmTable[x]));
                           }
                           break;

                        case RA_SUB_OPTION: {
                           for (RaOptionRank = 0; RaOptionRank < ARGUS_MAX_PRINT_ALG; RaOptionRank++)
                              if ((parser->RaPrintAlgorithmList[RaOptionRank] != NULL) && 
                                  (parser->RaPrintAlgorithmList[RaOptionRank]->print == RaPrintAlgorithmTable[x].print))
                                 break;

                           if (RaOptionRank < ARGUS_MAX_PRINT_ALG) {
                              parser->RaPrintAlgorithmList[RaOptionRank] = NULL;

                              while (RaOptionRank < (ARGUS_MAX_PRINT_ALG - 1)) {
                                 parser->RaPrintAlgorithmList[RaOptionRank] = 
                                      parser->RaPrintAlgorithmList[RaOptionRank + 1];
                                 RaOptionRank++;
                              }
                           }
                           break;
                        }
                     }
                     found++;
                     break;
                  }
               }
            }
         }

      } else
         break;
   }
}


/*
 * Copy arg vector into a new argus_strbuffer, concatenating arguments with spaces.
 */

char *
ArgusCopyArgv(char **argv)
{
   char *retn = NULL, **p;
   char *src, *dst;
   int len = 0;

   p = argv;
   if (*p == 0)
      return 0;

   while (*p)
      len += strlen(*p++) + 1;

   retn = (char *) malloc (len);

   p = argv;
   dst = retn;
   while ((src = *p++) != NULL) {
      while ((*dst++ = *src++) != '\0')
         ;
      dst[-1] = ' ';
   }
   dst[-1] = '\0';

   return retn;
}



unsigned int
ArgusIndexV2Record (struct ArgusV2Record *argus, struct ArgusV2FarHeaderStruct **hdrs)
{
   unsigned int retn = 0;
   struct ArgusV2FarHeaderStruct *far = (struct ArgusV2FarHeaderStruct *) &argus->argus_far;
   unsigned int length = argus->ahdr.length - sizeof(argus->ahdr);
   unsigned int farlen;
 
   bzero ((char *) hdrs, 32 * sizeof(struct ArgusFarHeaderStruct *));

   if (argus->ahdr.type & ARGUS_V2_FAR) {
      while ((length > 0) && (far->length > 0) && (length >= far->length)) {
         switch (far->type) {
            case ARGUS_V2_FAR:
               if (retn & ARGUS_V2_FAR_DSR_STATUS)
                  return (retn);
               retn |= ARGUS_V2_FAR_DSR_STATUS;
               hdrs[ARGUS_V2_FAR_DSR_INDEX] = far;
               break;
            case ARGUS_V2_MAC_DSR:    
               if (retn & ARGUS_V2_MAC_DSR_STATUS)
                  return (retn);
               retn |= ARGUS_V2_MAC_DSR_STATUS;
               hdrs[ARGUS_V2_MAC_DSR_INDEX] = far;
               break;
            case ARGUS_V2_VLAN_DSR:    
               if (retn & ARGUS_V2_VLAN_DSR_STATUS)
                  return (retn);
               retn |= ARGUS_V2_VLAN_DSR_STATUS;
               hdrs[ARGUS_V2_VLAN_DSR_INDEX] = far;
               break;
            case ARGUS_V2_MPLS_DSR:    
               if (retn & ARGUS_V2_MPLS_DSR_STATUS)
                  return (retn);
               retn |= ARGUS_V2_MPLS_DSR_STATUS;
               hdrs[ARGUS_V2_MPLS_DSR_INDEX] = far;
               break;
            case ARGUS_V2_AGR_DSR:    
               if (retn & ARGUS_V2_AGR_DSR_STATUS)
                  return (retn);
               retn |= ARGUS_V2_AGR_DSR_STATUS;
               hdrs[ARGUS_V2_AGR_DSR_INDEX] = far;
               break;
            case ARGUS_V2_TIME_DSR: 
               if (retn & ARGUS_V2_TIME_DSR_STATUS)
                  return (retn);
               retn |= ARGUS_V2_TIME_DSR_STATUS;
               hdrs[ARGUS_V2_TIME_DSR_INDEX] = far;
               break;
            case ARGUS_V2_SRCUSRDATA_DSR:
               if (retn & ARGUS_V2_SRCUSRDATA_DSR_STATUS)
                  return (retn);
               retn |= ARGUS_V2_SRCUSRDATA_DSR_STATUS;
               hdrs[ARGUS_V2_SRCUSRDATA_DSR_INDEX] = far;
               break;
            case ARGUS_V2_DSTUSRDATA_DSR:
               if (retn & ARGUS_V2_DSTUSRDATA_DSR_STATUS)
                  return (retn);
               retn |= ARGUS_V2_DSTUSRDATA_DSR_STATUS;
               hdrs[ARGUS_V2_DSTUSRDATA_DSR_INDEX] = far;
               break;
            case ARGUS_V2_TCP_DSR:    
               if (retn & ARGUS_V2_TCP_DSR_STATUS)
                  return (retn);
               retn |= ARGUS_V2_TCP_DSR_STATUS;
               hdrs[ARGUS_V2_TCP_DSR_INDEX] = far;
               break;
            case ARGUS_V2_ICMP_DSR:   
               if (retn & ARGUS_V2_ICMP_DSR_STATUS)
                  return (retn);
               retn |= ARGUS_V2_ICMP_DSR_STATUS;
               hdrs[ARGUS_V2_ICMP_DSR_INDEX] = far;
               break;
            case ARGUS_V2_RTP_DSR:    
               if (retn & ARGUS_V2_RTP_DSR_STATUS)
                  return (retn);
               retn |= ARGUS_V2_RTP_DSR_STATUS;
               hdrs[ARGUS_V2_RTP_DSR_INDEX] = far;
               break;
            case ARGUS_V2_IGMP_DSR:   
               if (retn & ARGUS_V2_IGMP_DSR_STATUS)
                  return (retn);
               retn |= ARGUS_V2_IGMP_DSR_STATUS;
               hdrs[ARGUS_V2_IGMP_DSR_INDEX] = far;
               break;
            case ARGUS_V2_ARP_DSR:    
               if (retn & ARGUS_V2_ARP_DSR_STATUS)
                  return (retn);
               retn |= ARGUS_V2_ARP_DSR_STATUS;
               hdrs[ARGUS_V2_ARP_DSR_INDEX] = far;
               break;
            case ARGUS_V2_FRG_DSR:    
               if (retn & ARGUS_V2_FRG_DSR_STATUS)
                  return (retn);
               retn |= ARGUS_V2_FRG_DSR_STATUS;
               hdrs[ARGUS_V2_FRG_DSR_INDEX] = far;
               break;
            case ARGUS_V2_ESP_DSR:    
               if (retn & ARGUS_V2_ESP_DSR_STATUS)
                  return (retn);
               retn |= ARGUS_V2_ESP_DSR_STATUS;
               hdrs[ARGUS_V2_ESP_DSR_INDEX] = far;
               break;
         }
   
         if ((farlen = far->length) == 0)
            break;

         if ((far->type == ARGUS_V2_SRCUSRDATA_DSR) ||
             (far->type == ARGUS_V2_DSTUSRDATA_DSR))
            farlen = farlen * 4;

         length -= farlen;
         far = (struct ArgusV2FarHeaderStruct *)((char *)far + farlen);
      }
   }

#ifdef ARGUS_V2DEBUG
   ArgusDebug (10, "ArgusIndexRecord (0x%x, 0x%x) returns 0x%x", argus, hdrs, retn);
#endif

   return (retn);
}



void
ArgusParseInit (struct ArgusParserStruct *parser, struct ArgusInput *input)
{
   int fd = 0;

   if (input != NULL) {
      fd = input->fd;

      input->ArgusLocalNet = htonl(input->ArgusInitCon.argus_mar.localnet);
      input->ArgusNetMask = htonl(input->ArgusInitCon.argus_mar.netmask);

      bcopy((char *)&input->ArgusInitCon, (char *)&parser->ArgusInitCon, sizeof(input->ArgusInitCon));

      input->ArgusLastTime = parser->ArgusRealTime;
      input->ArgusMarInterval = ntohs(input->ArgusInitCon.argus_mar.argusMrInterval);

      if (input->ArgusReadBuffer != NULL) {
         ArgusFree(input->ArgusReadBuffer);
         input->ArgusReadBuffer = NULL;
      }

      if (input->ArgusConvBuffer != NULL) {
         ArgusFree(input->ArgusConvBuffer);
         input->ArgusConvBuffer = NULL;
      }

      switch (input->type) {
         case ARGUS_SFLOW_DATA_SOURCE:
         case ARGUS_JFLOW_DATA_SOURCE:
         case ARGUS_CISCO_DATA_SOURCE: {
            if ((input->ArgusReadBuffer = (u_char *)ArgusCalloc (1, ARGUS_MAX_STREAM)) == NULL)
               ArgusLog (LOG_ERR, "ArgusCalloc error %s", strerror(errno));

            input->ArgusBufferLen = ARGUS_MAX_STREAM;

            if (parser->ArgusActiveHosts) {
               input->ArgusReadSocketState = ARGUS_READINGDATAGRAM;
               input->ArgusReadSize = k_maxFlowPacketSize;
               bzero (input->ArgusReadBuffer, k_maxFlowPacketSize);
               input->ArgusReadSocketSize  = k_maxFlowPacketSize;
            }

            if (parser->ArgusInputFileList) {
               input->ArgusReadSocketState = ARGUS_READINGPREHDR;
               input->ArgusReadSize = 4;
            }

            if ((input->ArgusReadBuffer = (unsigned char *)ArgusCalloc (1, k_maxFlowPacketSize)) == NULL)
               ArgusLog (LOG_ERR, "ArgusCalloc error %s", strerror(errno));

            if ((input->ArgusConvBuffer = (u_char *)ArgusCalloc (1, k_maxFlowPacketSize)) == NULL)
               ArgusLog (LOG_ERR, "ArgusCalloc error %s", strerror(errno));

            break;
         }

         case ARGUS_DATA_SOURCE: 
         case ARGUS_V2_DATA_SOURCE: {
            if ((input->ArgusReadBuffer = (u_char *)ArgusCalloc (1, ARGUS_MAX_STREAM)) == NULL)
               ArgusLog (LOG_ERR, "ArgusCalloc error %s", strerror(errno));
            input->ArgusBufferLen = ARGUS_MAX_STREAM;

            if ((input->ArgusConvBuffer = (u_char *)ArgusCalloc (1, MAXARGUSRECORD)) == NULL)
               ArgusLog (LOG_ERR, "ArgusCalloc error %s", strerror(errno));

#ifdef ARGUS_SASL
            if (input->ArgusSaslBuffer != NULL)
               ArgusFree(input->ArgusSaslBuffer);

            if ((input->ArgusSaslBuffer = (u_char *)ArgusCalloc (1, ARGUS_MAX_STREAM)) == NULL)
               ArgusLog (LOG_ERR, "ArgusCalloc error %s", strerror(errno));
#endif /* ARGUS_SASL */

            if (input->major_version > 2) {
               input->ArgusReadSocketState = ARGUS_READINGHDR;
               input->ArgusReadSocketSize = (input->ArgusReadSize < 0) ?
                                sizeof(struct ArgusRecordHeader) : input->ArgusReadSize;

            } else
            if (input->major_version > 1) {
               input->ArgusReadSocketState = ARGUS_READINGHDR;
               input->ArgusReadSocketSize = (input->ArgusReadSize < 0) ?
                                sizeof(struct ArgusV2RecordHeader) : input->ArgusReadSize;

            } else {
               input->ArgusReadSocketState = ARGUS_READINGBLOCK;
               input->ArgusReadSize = 60;
            }
         }
      }

      input->ArgusReadPtr = input->ArgusReadBuffer;
      input->ArgusConvPtr = input->ArgusConvBuffer;
   }

   if (!(ArgusParseInited++)) {
      if (input)
         ArgusInitAddrtoname (parser, input->ArgusLocalNet, input->ArgusNetMask);
      else
         ArgusInitAddrtoname (parser, 0L, 0L);
   }

#ifdef ARGUSDEBUG
   if (input) {
      ArgusDebug (2, "ArgusParseInit(0x%x 0x%x\n", parser, input);
   } else
      ArgusDebug (2, "ArgusParseInit(0x%x, NULL)", parser);
#endif
}

/*
 *  this is a generic routine for printing unknown data;
 *  we pass on the linefeed plus indentation string to
 *  get a proper output - returns 0 on error
 */

int
print_unknown_data(const u_char *cp, const char *ident, int len)
{
   hex_print((const u_char *)ident,cp,len);
   return(1); /* everything is ok */
}

/*
 *  print 128-bit integers into buffer.
 */

void
sprint128(char *buf, char *format, uint128 *t)
{
   uint128 nbuf, *n = &nbuf;
   int d[39] = {0}, i, j, x, sign = 1;

   n->high = ntohll(t->high);
   n->low  = ntohll(t->low);

   if (format[strlen(format) - 1] == 'x') {
      sprintf(buf, "0x%016llx%016llx", n->high, n->low);

   } else {
      if (format[strlen(format) - 1] == 'd') {
         if (n->high >> 63) {
            sign = -1;
            n->high = n->high & 0x7FFFFFFFFFFFFFFF;
         }
      }

      for (i = 63; i > -1; i--) {
         if ((n->high >> i) & 1) d[0]++;
         for (j = 0; j < 39; j++) d[j] *= 2;
         for (j = 0; j < 38; j++) d[j+1] += d[j]/10, d[j] %= 10;
      }
      for (i = 63; i > -1; i--) {
         if ((n->low >> i) & 1) d[0]++;
         if (i > 0) for (j = 0; j < 39; j++) d[j] *= 2;
         for (j = 0; j < 38; j++) d[j+1] += d[j]/10, d[j] %= 10;
      }
      for (i = 38; i > 0; i--) if (d[i] > 0) break;

      x = 0;
      if (sign < 0) buf[x++] = '-';
      for (; i > -1; i--) {
         sprintf(&buf[x++], "%c", '0'+d[i]);
      }
      buf[x] = '\0';
   }
}


/*
 * Convert a token value to a string; use "fmt" if not found.
 */
const char *
tok2str(const struct tok *lp, const char *fmt, int v)
{
   static char buf[128];

   while (lp->s != NULL) {
      if (lp->v == v)
         return (lp->s);
      ++lp;
   }
   if (fmt == NULL)
      fmt = "#%d";
   (void)snprintf(buf, sizeof(buf), fmt, v);
   return (buf);
}

/*
 * Convert a bit token value to a string; use "fmt" if not found.
 * this is useful for parsing bitfields, the output strings are comma seperated
 */

char *
bittok2str(const struct tok *lp, const char *fmt, int v)
{
   static char buf[256]; /* our stringbuffer */
   int buflen=0;
   int rotbit; /* this is the bit we rotate through all bitpositions */
   int tokval;

   while (lp->s != NULL) {
      tokval=lp->v;   /* load our first value */
      rotbit=1;
      while (rotbit != 0) {
         /*
          * lets AND the rotating bit with our token value
          * and see if we have got a match
          */
         if (tokval == (v&rotbit)) {
            /* ok we have found something */
            buflen+=snprintf(buf+buflen, sizeof(buf)-buflen, "%s, ",lp->s);
            break;
         }
         rotbit=rotbit<<1; /* no match - lets shift and try again */
      }
      lp++;
   }

   if (buflen != 0) { /* did we find anything */
      /* yep, set the the trailing zero 2 bytes before to eliminate the last comma & whitespace */
      buf[buflen-2] = '\0';
      return (buf);
   } else {
      /* bummer - lets print the "unknown" message as advised in the fmt string if we got one */
      if (fmt == NULL)
         fmt = "#%d";
      (void)snprintf(buf, sizeof(buf), fmt, v);
      return (buf);
   }
}


#define ASCII_LINELENGTH		300
#define HEXDUMP_BYTES_PER_LINE		16
#define HEXDUMP_SHORTS_PER_LINE		(HEXDUMP_BYTES_PER_LINE / 2)
#define HEXDUMP_HEXSTUFF_PER_SHORT	5
#define HEXDUMP_HEXSTUFF_PER_LINE	(HEXDUMP_HEXSTUFF_PER_SHORT * HEXDUMP_SHORTS_PER_LINE)

void
hex_print_with_offset(const u_char *ident, const u_char *cp, u_int length, u_int oset)
{
   u_int i, s;
   int nshorts;

   nshorts = (u_int) length / sizeof(u_short);
   i = 0;
   while (--nshorts >= 0) {
      if ((i++ % 8) == 0) {
         (void)printf("%s0x%04x: ", ident, oset);
         oset += HEXDUMP_BYTES_PER_LINE;
      }
      s = *cp++;
      (void)printf(" %02x%02x", s, *cp++);
   }
   if (length & 1) {
      if ((i % 8) == 0)
         (void)printf("%s0x%04x: ", ident, oset);
      (void)printf(" %02x", *cp);
   }
}

/*
 * just for completeness
 */
void
hex_print(const u_char *ident, const u_char *cp, u_int length)
{
   hex_print_with_offset(ident, cp, length, 0);
}

/*
 * Print a relative number of seconds (e.g. hold time, prune timer)
 * in the form 5m1s.  This does no truncation, so 32230861 seconds
 * is represented as 1y1w1d1h1m1s.
 */
void
relts_print(char *buf, int secs)
{
   static const char *lengths[] = {"y", "w", "d", "h", "m", "s"};
   static const int seconds[] = {31536000, 604800, 86400, 3600, 60, 1};
   const char **l = lengths;
   const int *s = seconds;

   if (secs == 0) {
      (void)sprintf(&buf[strlen(buf)], "0s");
      return;
   }
   if (secs < 0) {
      (void)sprintf(&buf[strlen(buf)], "-");
      secs = -secs;
   }
   while (secs > 0) {
      if (secs >= *s) {
         (void)sprintf(&buf[strlen(buf)], "%d%s", secs / *s, *l);
         secs -= (secs / *s) * *s;
      }
      s++;
      l++;
   }
}

char *
ArgusAbbreviateMetric(struct ArgusParserStruct *parser, char *buf, int len, double value)
{
   char *retn = buf, *ptr;
   int ind = 0;

   while (value >= 1000.0) {
      value /= 1000.0;
      ind++;
   }

   snprintf (buf, len, "%.03f", value);
   if (parser->pflag < 3) {
      ptr = strchr(buf, '.');
      ptr++;
      ptr[parser->pflag] = '\0';
      ptr[parser->pflag + 1] = '\0';
   }

   switch (ind) {
      case 0: sprintf (&buf[strlen(buf)], "%c", ' '); break;
      case 1: sprintf (&buf[strlen(buf)], "%c", 'K'); break;
      case 2: sprintf (&buf[strlen(buf)], "%c", 'M'); break;
      case 3: sprintf (&buf[strlen(buf)], "%c", 'G'); break;
      case 4: sprintf (&buf[strlen(buf)], "%c", 'T'); break;
      case 5: sprintf (&buf[strlen(buf)], "%c", 'P'); break;
      case 6: sprintf (&buf[strlen(buf)], "%c", 'E'); break;
      case 7: sprintf (&buf[strlen(buf)], "%c", 'Z'); break;
      case 8: sprintf (&buf[strlen(buf)], "%c", 'Y'); break;
   }

   return (retn);
}

char *
ArgusTrimString (char *str)
{
   char *retn = NULL, *ptr = str;

   if ((ptr != NULL) && (strlen(ptr) > 0)) {
      int len;
      while (isspace((int)*ptr)) ptr++;
      retn = ptr;

      len = strlen(ptr);
      ptr = &ptr[len - 1];

      while ((*ptr != '\0') && (isspace((int)*ptr)) && (ptr != retn))
         *ptr-- = '\0';
   }
   return (retn);
}

