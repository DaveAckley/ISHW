/*                                              -*- mode:C++; fill-column:100 -*-
  MFMAssert.h - Names for MFM assertions
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

/**
  \file MFMAssert.h Support for assertions.
  \author David H. Ackley.  
  \date Copyright (C) 2008 All rights reserved.
  \lgpl
 */
#ifndef MFMASSERT_H_
#define MFMASSERT_H_

#include "MFMErrors.h"  /* For E_ error codes */

#ifndef API_UNPROTECTED

/**
   Causes an MFM to die immediately, blinking the given code.  See MFMErrors.h for more information.

   \usage
   \code
   ...
   if (foo < bar) API_BUG(E_BUG_INCONSISTENT_STATE);
   ...
   switch (ch) {
    default: API_BUG(BLINK_MAP(0,0,1);  // Die with custom code (1 blue only)
   ...
   \endcode
 */
#define API_BUG(code) do { _apiError_(code,__FILE__,__LINE__); } while (0)


/**
   Causes an MFM to die if condition is false, blinking the given code.  See MFMErrors.h for more
   information.

   \usage
   \code
   ...
   API_ASSERT(foo < bar,E_BUG_INCONSISTENT_STATE);
   ...
   void function(int arg) {
    API_ASSERT(arg >= -1 && arg < 256,E_API_ARGUMENT);
   ...
   \endcode
 */
#define API_ASSERT(condition,code) do { if (!(condition)) API_BUG(code); } while (0)

#else

#define API_BUG(code) 0 /* ignored */ 
#define API_ASSERT(condition,code) /* ignored */

#endif

/**
   Causes the sketch to die immediately with a 'POISON'-related blinkcode.  Used for basic
   debugging, to determine when execution has reached a particular point in the code.  The argument,
   a number from 0 to 6, determines which code is 'blinked'; this can be used to distinguish
   multiple possible poisoning locations.

   \code
     void myHandler(u8 *) {
        POISON(0);  // XXX IS MY HANDLER EVEN GETTING CALLED?  
      ..stuff..
     }
   \endcode
   \blinks #E_API_DEBUG_POISON0 if \c POISON(0) is executed
   \blinks #E_API_DEBUG_POISON1 if \c POISON(1) is executed
   \blinks #E_API_DEBUG_POISON2 if \c POISON(2) is executed
   \blinks #E_API_DEBUG_POISON3 if \c POISON(3) is executed
   \blinks #E_API_DEBUG_POISON4 if \c POISON(4) is executed
   \blinks #E_API_DEBUG_POISON5 if \c POISON(5) is executed
   \blinks #E_API_DEBUG_POISON6 if \c POISON(6) is executed
 */
#define POISON(code0_6) API_BUG(BLINK_MAP(1,0,code0_6))

/** Assert that \a value is in the range 0..maxval-1, else die with code. \blinks the supplied code,
    as produced by the #BLINK_MAP macro */
#define API_ASSERT_MAX_CODE(value,maxval,code) API_ASSERT(((u32)(value))<(maxval),code)

/** Assert that \a value is in the range 0..maxval-1, else die. \blinks #E_API_MAX_RANGE*/
#define API_ASSERT_MAX(value,maxval) API_ASSERT_MAX_CODE(value,maxval,E_API_MAX_RANGE)

/** Assert that \a value is non-zero, else die. \blinks #E_API_TRUE */
#define API_ASSERT_TRUE(value) API_ASSERT((value)!=0,E_API_TRUE)

/** Assert that \a value is zero, else die. \blinks #E_API_FALSE */
#define API_ASSERT_FALSE(value) API_ASSERT((value)==0,E_API_FALSE)

/** Assert that \a value is non-zero, else die. \blinks #E_API_NONZERO */
#define API_ASSERT_NONZERO(value) API_ASSERT((value)!=0,E_API_NONZERO)

/** Assert that \a value is non-zero, else die. \blinks #E_API_NULL_POINTER */
#define API_ASSERT_NONNULL(value) API_ASSERT((value)!=0,E_API_NULL_POINTER)

/** Assert that \a value is zero, else die. \blinks #E_API_NONNULL_POINTER */
#define API_ASSERT_NULL(value) API_ASSERT((value)==0,E_API_NONNULL_POINTER)

/** Assert that \a value is zero, else die. \blinks #E_API_ZERO */
#define API_ASSERT_ZERO(value) API_ASSERT((value)==0,E_API_ZERO)

/** Assert that \a target is equal to \a value, else die. \blinks #E_API_EQUAL */
#define API_ASSERT_EQUAL(target,value) API_ASSERT((value)==(target),E_API_EQUAL)

/** Assert that \a target is not equal to \a value, else die. \blinks #E_API_NOT_EQUAL */
#define API_ASSERT_NOT_EQUAL(target,value) API_ASSERT((value)!=(target),E_API_NOT_EQUAL)

/** Assert that \a target is less than to \a value, else die. \blinks #E_API_LESS */
#define API_ASSERT_LESS(target,value) API_ASSERT((target)<(value),E_API_LESS)

/** Assert that \a target is less than or equal to \a value, else die. \blinks #E_API_LESS_EQUAL */
#define API_ASSERT_LESS_EQUAL(target,value) API_ASSERT((target)<=(value),E_API_LESS_EQUAL)

/** Assert that \a target is greater than \a value, else die. \blinks #E_API_GREATER */
#define API_ASSERT_GREATER(target,value) API_ASSERT((target)>(value),E_API_GREATER)

/** Assert that \a target is greater than or equal to \a value, else die. \blinks #E_API_GREATER_EQUAL */
#define API_ASSERT_GREATER_EQUAL(target,value) API_ASSERT((target)>=(value),E_API_GREATER_EQUAL)

/** Assert that \a u32Pin is a valid pin number, else die. \blinks #E_API_BAD_PIN */
#define API_ASSERT_VALID_PIN(u32Pin) API_ASSERT_MAX_CODE(u32Pin,VIRTUAL_PIN_COUNT,E_API_BAD_PIN)

/** Assert that \a u32Face is a valid 'physical face' number, else die. Note there are several
    different notions of 'face' used in different contexts; this assertion, allowing only the
    physical faces #NORTH, #SOUTH, #EAST, and #WEST, is the most restrictive.  Compare to
    #API_ASSERT_VALID_EXTENDED_FACE.  \blinks #E_API_BAD_FACE */
#define API_ASSERT_VALID_FACE(u32Face) API_ASSERT_MAX_CODE(u32Face,FACE_COUNT,E_API_BAD_FACE)

/** Assert that \a u32Face is a valid face number in the loosest possible sense, else die. Note
    there are several different notions of 'face' used in different contexts; this assertion is the
    most liberal, allowing the physical faces #NORTH, #SOUTH, #EAST, and #WEST, as well as
    'internal' faces such as #SPINE and #BRAIN, and potential 'virtual' faces defined by sketch
    code.  Compare to #API_ASSERT_VALID_FACE.  \blinks #E_API_BAD_FACE */
#define API_ASSERT_VALID_EXTENDED_FACE(u32Face) API_ASSERT_MAX_CODE(u32Face,MAX_FACE_INDEX,E_API_BAD_FACE)

/** Assert that \a u32address is an even number, else die. \blinks #E_API_HALFWORD_ALIGNED */
#define API_ASSERT_HALFWORD_ALIGNED(uptraddress) API_ASSERT((((uptr)(uptraddress))&1)==0,E_API_HALFWORD_ALIGNED)

/** Assert that \a u32address is a multiple of four, else die. \blinks #E_API_WORD_ALIGNED */
#define API_ASSERT_WORD_ALIGNED(uptraddress) API_ASSERT((((uptr)(uptraddress))&3)==0,E_API_WORD_ALIGNED)

/** Assert that \a u32address is a multiple of eight, else die. \blinks #E_API_DOUBLEWORD_ALIGNED */
#define API_ASSERT_DOUBLEWORD_ALIGNED(uptraddress) API_ASSERT((((uptr)(uptraddress))&7)==0,E_API_DOUBLEWORD_ALIGNED)

#endif /*MFMASSERT_H_*/
