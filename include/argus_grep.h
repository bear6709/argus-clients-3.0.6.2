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
 * $Id: //depot/argus-3.0.6/clients/include/argus_grep.h#1 $
 * $DateTime: 2012/04/17 12:22:02 $
 * $Change: 2368 $
 */


#ifndef ArgusGrep_h
#define ArgusGrep_h

#ifdef __cplusplus
extern "C" {
#endif

extern void ArgusInitializeGrep (struct ArgusParserStruct *parser);
extern int ArgusGrepUserData (struct ArgusParserStruct *, struct ArgusRecordStruct *);

#ifdef __cplusplus
}
#endif

#endif  /* ArgusGrep_h */

