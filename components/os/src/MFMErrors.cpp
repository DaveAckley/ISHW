/*                                             ; fill-column:100 -*- mode:C++ -*-
  MFMErrors.cpp - Basic library utility routines and macros
  Copyright (C) 2008 The Regents of the University of New Mexico.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
  USA
*/

#ifdef HOST_MODE

#ifdef __cplusplus
#define __STDC_FORMAT_MACROS 1   /* To get PRId32 when loading inttypes.h, sigh */
#endif

#include <inttypes.h>           /* for PRId32 */
#include <stdio.h>              /* for printf */
#include <stdlib.h>             /* for exit() */
#endif

#include "MFMErrors.h"
#include "MFMMacros.h"

#define RED    0                /* Red color index */
#define GREEN  1                /* Green color index */
#define BLUE   2                /* Blue color index */

#include "MFMTileSpecific.h"    /* For _dieOnBoard_ */
#if 0
#ifdef HOST_MODE

/*
void _dieNow_(const char * m1, const char * m2, const char * m3, const char * m4, u32 blinkCode)
{
  fprintf(stderr,"Fatal error: %s%s%s%s\nRGB code: BLINK_MAP(%" PRId32 ",%" PRId32 ",%" PRId32 ")\n",m1,m2,m3,m4,
          (u32) GET_BLINK_MAP_COLOR(blinkCode,RED), (u32) GET_BLINK_MAP_COLOR(blinkCode,GREEN), (u32) GET_BLINK_MAP_COLOR(blinkCode,BLUE));
  exit(blinkCode);
}
*/
#else

#include "MFMHWPins.h"  /* For _dieOnBoard_ */

 /*
void _dieNow_(const char * , const char * , const char * , const char * , u32 blinkCode)
{
  _dieOnBoard_(blinkCode);
}
 */
#endif
#endif
