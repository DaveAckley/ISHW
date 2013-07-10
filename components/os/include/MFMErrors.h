/*                                              -*- mode:C++; fill-column:100 -*-
  MFMErrors.h - Names for MFM errors, in terms of 'blink maps'.
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
  \file MFMErrors.h Definitions of the failed assertion 'blink codes'.  This file defines the known
  "die blinking codes" that may occur.

  When any of a large class of programming errors is detected, the MFM will stop normal operation
  and repeatedly display a 'blink code' that may help in determining what went wrong.

  The blink code is preceded by blinking of the face LED's in a NS-EW-NS-EW-NS sequence, followed by
  0 to 6 red flashes, then 0 to 6 green flashes, and finally 0 to 6 blue flashes; then the entire
  sequence repeats.

  If an error flashes, say, 1 red, then 3 green, then 5 blue, that corresponds to \code
  BLINK_CODE(1,3,5) \endcode Searching for that blink code in MFMErrors.h then reveals that an
  E_API_REFLEX_FULL error has occurred.

  Notes:

  - All codes of the form BLINK_MAP(0,X,Y) -- i.e., with no RED flashes -- are reserved for
    sketch use!

  - The range of blinks per color is 0..7, but only 0..6 are used so far; 7 in any position is
    reserved for expansion

  - Normally once a blink code occurs, it will blink indefinitely, until the MFM is reset.  But if
    the #RF_OKAY_NEW_GUY reflex flag is set, then after a small number of blink code displays the MFM
    will attempt to reboot automatically.

  \author David H. Ackley.
  \date (C) 2008 All rights reserved.
  \lgpl
 */
#ifndef MFMERRORS_H_
#define MFMERRORS_H_

#include "MFMConstants.h"

#define E_API_DEBUG_POISON0      BLINK_MAP(1,0,0)  /**< a debug 'POISON(0)' call was executed */
#define E_API_DEBUG_POISON1      BLINK_MAP(1,0,1)  /**< a debug 'POISON(1)' call was executed */
#define E_API_DEBUG_POISON2      BLINK_MAP(1,0,2)  /**< a debug 'POISON(2)' call was executed */
#define E_API_DEBUG_POISON3      BLINK_MAP(1,0,3)  /**< a debug 'POISON(3)' call was executed */
#define E_API_DEBUG_POISON4      BLINK_MAP(1,0,4)  /**< a debug 'POISON(4)' call was executed */
#define E_API_DEBUG_POISON5      BLINK_MAP(1,0,5)  /**< a debug 'POISON(5)' call was executed */
#define E_API_DEBUG_POISON6      BLINK_MAP(1,0,6)  /**< a debug 'POISON(6)' call was executed */

#define E_API_ARGUMENT           BLINK_MAP(1,1,0)  /**< Otherwise unspecified bad API argument */
#define E_API_ILLEGAL_STATE      BLINK_MAP(1,1,1)  /**< Otherwise unspecified illegal API state */
#define E_API_BAD_PIN            BLINK_MAP(1,1,2)  /**< Invalid pin passed as argument */
#define E_API_BAD_FACE           BLINK_MAP(1,1,3)  /**< Invalid face passed as argument */
#define E_API_MAX_RANGE          BLINK_MAP(1,1,4)  /**< Value is out of range 0 to max-1 */
#define E_API_TRUE               BLINK_MAP(1,1,5)  /**< Value that must be nonzero (true) wasn't */
#define E_API_FALSE              BLINK_MAP(1,1,6)  /**< Value that must be zero (false) wasn't */

#define E_API_PACKET_ERROR       BLINK_MAP(1,2,0)  /**< Broken packet detected and RF_DIE_PACKET_ERROR was set */
#define E_API_EQUAL              BLINK_MAP(1,2,1)  /**< Values that must be equal weren't */
#define E_API_ZERO               BLINK_MAP(1,2,2)  /**< Value must be 0 but wasn't */
#define E_API_NONZERO            BLINK_MAP(1,2,3)  /**< Value must be nonzero but wasn't */
#define E_API_NOT_EQUAL          BLINK_MAP(1,2,4)  /**< Value must be != to another but wasn't */
#define E_API_LESS               BLINK_MAP(1,2,5)  /**< Value must be < than another but wasn't */
#define E_API_LESS_EQUAL         BLINK_MAP(1,2,6)  /**< Value must be <= than another but wasn't */

#define E_API_GREATER            BLINK_MAP(1,3,0)  /**< Value must be > than another but wasn't */
#define E_API_GREATER_EQUAL      BLINK_MAP(1,3,1)  /**< Value must be >= than another but wasn't */
#define E_API_PURE_VIRTUAL       BLINK_MAP(1,3,2)  /**< Pure virtual function called */
#define E_API_MALLOC_CALL        BLINK_MAP(1,3,3)  /**< malloc() called but doesn't exist */
#define E_API_FREE_CALL          BLINK_MAP(1,3,4)  /**< free() called but doesn't exist */
#define E_API_REFLEX_FULL        BLINK_MAP(1,3,5)  /**< Too many reflexes defined */
#define E_API_BAD_REFLEX         BLINK_MAP(1,3,6)  /**< Attempt to define reserved reflex */

#define E_API_BAD_BAUD           BLINK_MAP(1,4,0)  /**< Illegal baud rate */
#define E_API_BAD_ALARM          BLINK_MAP(1,4,1)  /**< Illegal alarm number specified (0, too large, or never created) */
#define E_API_NULL_HANDLER       BLINK_MAP(1,4,2)  /**< Null callback supplied where illegal */
#define E_API_BAD_TRIGGER        BLINK_MAP(1,4,3)  /**< triggerUp() called outside of active PacketHandler */
#define E_API_BAD_FORGET         BLINK_MAP(1,4,4)  /**< forget() called outside of active memory */
#define E_API_MEMCPY             BLINK_MAP(1,4,5)  /**< memcpy called */
#define E_API_FORMAT_ARG         BLINK_MAP(1,4,6)  /**< a bad print or read code was passed */

#define E_API_HALFWORD_ALIGNED   BLINK_MAP(1,5,0)  /**< a value/address needed to be a multiple of two but wasn't */
#define E_API_WORD_ALIGNED       BLINK_MAP(1,5,1)  /**< a value/address needed to be a multiple of four but wasn't */
#define E_API_DOUBLEWORD_ALIGNED BLINK_MAP(1,5,2)  /**< a value/address needed to be a multiple of eight but wasn't */
#define E_API_BAD_SECTOR         BLINK_MAP(1,5,3)  /**< A bad sector number or invalid address for sector */
#define E_API_REFLEX_REDEF       BLINK_MAP(1,5,4)  /**< Attempt to redefine library reflex */
#define E_API_CXA_GUARD          BLINK_MAP(1,5,5)  /**< cxa_guard_acquire, _release, or _abort called */
#define E_API_NO_MORE_ALARMS     BLINK_MAP(1,5,6)  /**< Failed to create an alarm when needed */

#define E_API_POSITIVE           BLINK_MAP(1,6,0)  /**< Value must be positive but wasn't */
#define E_API_NONNEGATIVE        BLINK_MAP(1,6,1)  /**< Value must be nonnegative but wasn't */
#define E_API_DEAD_FRAME         BLINK_MAP(1,6,2)  /**< Attempted access to an uninitialized frame */
#define E_API_NO_TRANSPORT       BLINK_MAP(1,6,3)  /**< A transport was required but not present */
#define E_API_TRANSPORT_CONFIG   BLINK_MAP(1,6,4)  /**< A transport failed to configure when required */
#define E_API_USE_BEFORE_INIT    BLINK_MAP(1,6,5)  /**< Some service was used before it was initialized */

#define E_API_NULL_PACKET        BLINK_MAP(2,0,1)  /**< A null packet pointer where illegal */
#define E_API_NULL_POINTER       BLINK_MAP(2,0,2)  /**< A null pointer of some non-packet type, where illegal */
#define E_API_POINTER_ALIGNMENT  BLINK_MAP(2,0,3)  /**< A pointer needed to be half or word-aligned but wasn't */
#define E_API_INVALID_PACKET     BLINK_MAP(2,0,4)  /**< A null, corrupt, or non-packet pointer passed where packet expected */
#define E_API_NOT_HEAP_PACKET    BLINK_MAP(2,0,5)  /**< A valid but non-heap-allocated packet passed where heap packet needed */
#define E_API_BAD_FREE_ARG       BLINK_MAP(2,0,6)  /**< corrupt, non-malloc()ed, or already-freed pointer passed to free() */

#define E_API_BAD_FORMAT_CODE    BLINK_MAP(2,1,0)  /**< Char after '%' not recognized in facePrintf or packetScanf format */
#define E_API_BAD_INTERRUPT_PIN  BLINK_MAP(2,1,1)  /**< attach/detachInterrupt pin can't be interrupted on */
#define E_API_NO_MORE_INTERRUPTS BLINK_MAP(2,1,2)  /**< Ran out of room for attach/detachInterrupt handlers */
#define E_API_EEPROM_ADDRESS     BLINK_MAP(2,1,3)  /**< EEPROM address out of range */
#define E_API_EEPROM_RESPONSE    BLINK_MAP(2,1,4)  /**< EEPROM failed to respond */
#define E_API_EEPROM_DATA        BLINK_MAP(2,1,5)  /**< EEPROM data corrupt */
#define E_API_BUFFER_LOCKED      BLINK_MAP(2,1,6)  /**< Attempt to modify locked packet buffer */

#define E_API_RESOURCE_EXHAUSTED BLINK_MAP(2,2,0)  /**< Some required resource has run out \since 0.9.18 */
#define E_API_EXIT_CALLED        BLINK_MAP(2,2,1)  /**< exit() was called \since 0.9.20 */
#define E_API_NONNULL_POINTER    BLINK_MAP(2,2,2)  /**< A non-null pointer of some non-packet type, where null was required \since 0.9.20 */

#define E_HW_UNDEF               BLINK_MAP(4,0,0)  /**< An 'undefined instruction' hardware exception occurred */
#define E_HW_DABT                BLINK_MAP(4,0,1)  /**< A 'data abort' hardware exception occurred */
#define E_HW_PABT                BLINK_MAP(4,0,2)  /**< A 'prefetch abort' hardware exception occurred */
#define E_HW_SWI                 BLINK_MAP(4,0,3)  /**< A 'software interrupt' hardware exception occurred */
#define E_HW_FIQ                 BLINK_MAP(4,0,4)  /**< A FIQ trap occurred */

#define E_B3_IAP_FAILURE         BLINK_MAP(4,1,0)  /**< A flash programming command failed */
#define E_B3_VERIFY_FAILURE      BLINK_MAP(4,1,1)  /**< An apparently successful flash write failed verification */

#define E_BUG_NO_SETUP           BLINK_MAP(6,0,1)  /**< Bug: No setup() routine was linked (at least, none with C linkage!) */
#define E_BUG_NO_LOOP            BLINK_MAP(6,0,2)  /**< Bug: No loop() routine was linked (at least, none with C linkage!) */

#define E_BUG_STACK_CORRUPT      BLINK_MAP(6,0,3)  /**< Bug: Flag value at stack end was altered.  This can be caused by
                                                             either a stack overflow, or a global variable buffer
                                                             overflow. Check #getMinimumStackSpace(). \since 0.9.10 */

#define E_BUG_UNIMPLEMENTED      BLINK_MAP(6,1,1)  /**< Bug: Some unimplemented function was called */
#define E_BUG_REFLEX_RECURSION   BLINK_MAP(6,1,2)  /**< Bug: Reflex dispatching was entered recursively */
#define E_BUG_INCONSISTENT_STATE BLINK_MAP(6,1,3)  /**< Bug: Inconsistent internal state detected */
#define E_BUG_RECURSIVE_DEATH    BLINK_MAP(6,1,4)  /**< Bug: An error occurred while dying */

#define E_BUG_UNSPECIFIED_BUG    BLINK_MAP(6,6,6)  /**< Bug: An unspecified library bug was detected */


/****************** Bug macros, and death function declarations ******************/

extern
#ifdef __cplusplus
 "C"
#endif

/** \cond */
void _apiError_(u32 blinkCode, const char * file, int lineno) __attribute__ ((noreturn));
/** \endcond */

/**
   Kill the system, leaving it blinking \a code.  Additional \a message may be visible if the reflex
   flag #RF_DIE_ALOUD is enabled via #SET_REFLEX_FLAGS.

   \param message a last message to possibly make available for debugging

   \param code a 'blinkcode' to display (see #BLINK_MAP)

   \return Does not return
   
 */
#define BUG_CODE(message,code) _apiError_(code, #message " - " __FILE__, __LINE__)

/**
   Kill the system, leaving it blinking #E_BUG_UNSPECIFIED_BUG.  Additional \a message may be
   visible if the reflex flag #RF_DIE_ALOUD is enabled via #SET_REFLEX_FLAGS.

   \param message a last message to possibly make available for debugging

   \return Does not return
   
 */
#define BUG(message) BUG_CODE(message, E_BUG_UNSPECIFIED_BUG)

/**
   Conditionally kill the system, leaving it blinking \a code, if \a condition is non-zero.

   \param code a 'blinkcode' to display (see #BLINK_MAP)

   \param condition an expression to evaluate that produces a bool value

 */
#define BUG_IF_CODE(code, condition) do { if (condition) BUG_CODE("", code); } while (0)

/**
   Kill the system, leaving it blinking #E_BUG_UNIMPLEMENTED.  

   \return Does not return
   
 */
#define BUG_UNIMPLEMENTED() BUG_CODE("Unimplemented",E_BUG_UNIMPLEMENTED)



#endif /*MFMERRORS_H_*/
