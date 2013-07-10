/*                                             -*- mode:C++; fill-column:100 -*-
  MFMConstants.h - Names for MFM constants
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
  \file MFMConstants.h Various constant definitions.
  \author David H. Ackley.  
  \date (C) 2008 All rights reserved.
  \lgpl
 */
#ifndef MFMCONSTANTS_H_
#define MFMCONSTANTS_H_

#include "MFMTypes.h"
#include "MFMMacros.h"

/** \name Good old fashioned pin states */
/*@{*/
#define HIGH 1            /**< High, set, logical 1 value */
#define LOW 0             /**< Low, clear, logical 0 value */
/*@}*/

/** \name Wiring/Arduino/MFM 'print' method format codes */
/*@{*/
#define DEC 10            /**< Print in decimal, signed if negative (in most contexts) */
#define HEX 16            /**< Print in hexadecimal (base 16, using 0-9A-Z), unsigned */
#define OCT 8             /**< Print in octal (base 8, using 0-7), unsigned */
#define BIN 2             /**< Print in binary (base 2, using 0-1), unsigned */
#define BYTE 0            /**< Print a single byte directly, as an unaltered 8 bit value */

#define BESHORT -1        /**< (MFM only) Print a short (2 bytes) in big endian (network order) */
#define BELONG -2         /**< (MFM only) Print a long (4 bytes) in big endian (network order) */
#define B36 36            /**< Print in base 36 ("hexatrigintimal"?, using 0-9A-Z), unsigned */
/*@}*/

/** \name Wiring/Arduino/MFM pinMode direction modes */
/*@{*/
#define INPUT  0          /**< Pin is input, under sketch control */
#define OUTPUT 1          /**< Pin is output, under sketch control */
#define REFLEX_MODE 2     /**< (MFM only) Pin is under reflex control */
#define HW_MODE 3         /**< (MFM only) Pin is under hardware control and shouldn't be messed with */
/*@}*/

/** \name MFM physical face indices.  
    Note these are ALSO the HW UART#'s of corresponding faces! */
/*@{*/
#define NORTH  0u         /**< Port number of the North Face */
#define SOUTH  1u         /**< Port number of the South Face */
#define EAST   2u         /**< Port number of the East Face */
#define WEST   3u         /**< Port number of the West Face */

/** The number of physical faces. 

  Although it is perfectly legal and acceptable to iterate over the physical faces using #FACE_COUNT
  in the familiar way:

  \code
     for (u32 face = 0; face < FACE_COUNT; ++face) ..use face..;
  \endcode
  
  because there are several different ranges of "more or less real" faces (see, e.g.,
  #MAX_FACE_INDEX), the MFM library codebase itself more often writes:

  \code
     for (u32 face = NORTH; face <= WEST; ++face) ..use face..;
  \endcode

  to stress the specific range of face values being visited.  Because the numeric values of #NORTH,
  #SOUTH, #EAST, and #WEST map directly to the underlying UART hardware registers, it is fair to
  consider the iteration order of the physical faces as "frozen in stone".

 */
#define FACE_COUNT 4      
/*@}*/

/** \name MFM 'extended' faces indices.
    These codes are valid only in certain contexts (reflex triggering,
    printing, etc) depending on the specific code. */
/*@{*/
#define SPINE 4u          /**< All the spatial faces (NSEW) flow to here. */
#define WMEM  5u          /**< Working memory: internal memorized packets. */
#define BRAIN 6u          /**< SPINE+WMEM flows here: The last stop for reflex responses. */

#define ALL_FACES 7u      /**< Implies NORTH+SOUTH+EAST+WEST.  Special code, valid in certain
                             contexts (e.g., #facePrint()) */
#define NO_FACES 8u       /**< Implies no faces.  Special code, valid in certain contexts (e.g.,
                             #facePrint()) */

#define MIN_VIRTUAL_FACE 9u  /**< Lowest number corresponding to a user-defined 'virtual' face,
                                valid in certain contexts (e.g., facePrintf) */

#define VIRTUAL_FACE_COUNT 32 /**< The number of virtual faces that can be defined.  Legal virtual
                                 face codes start at #MIN_VIRTUAL_FACE and end just below
                                 #MAX_FACE_INDEX. 
                                 Increased from 16 to 32 as of 0.9.18
                              */

#define MAX_FACE_INDEX (MIN_VIRTUAL_FACE+VIRTUAL_FACE_COUNT) /**< The overall count of possible face
                                                                codes.  This includes the #FACE_COUNT
                                                                physical faces, the "extended face
                                                                codes" such as #BRAIN, and the up to
                                                                #VIRTUAL_FACE_COUNT "virtual
                                                                faces". */
#define NOT_A_FACE (MAX_FACE_INDEX) /**< A number that isn't a legal face code in any context. */

/*@}*/

/** \name Wiring/Arduino 'attachInterrupt' modes.  
    Note that mode 'LOW', meaning interrupt on low level, is not currently supported in the MFM. */
/*@{*/
#define RISING  0x01            /**< Interrupt on #LOW to #HIGH transition */ 
#define FALLING 0x02            /**< Interrupt on #HIGH to #LOW transition */ 
#define CHANGING ((RISING)|(FALLING)) /**< Interrupt on either #RISING or #FALLING */
/*@}*/

/** \name Wiring/Arduino 'shiftOut' bit orders.  
    Used by #shiftOut(). */
/*@{*/
#define LSBFIRST 0              /**< Ship the least-significant-bit (LSB) first */ 
#define MSBFIRST 1              /**< Ship the most-significant-bit (MSB) first */ 
/*@}*/

/** \name MFM tertiary bootloader boot modes */
/*@{*/
#define UNKNOWN_BOOT_MODE 0     /**< Boot mode not (yet) known. */ 
#define RED_BOOT_MODE 1         /**< Red boot.  */ 
#define GREEN_BOOT_MODE 2       /**< Green boot.  */ 
#define BLUE_BOOT_MODE 3        /**< Blue boot.  */ 
#define MAX_BOOT_MODES 4       /**< Number of boot modes.  */ 
/*@}*/

/** \name Packet flags.  
    The value returned by #packetFlags() is the bitwise-OR of these values.  Warning: The
    PK_OVERRUN, PK_PARITY, PK_FRAMING, and PK_BREAK flag definitions match the corresponding
    conditions in LPC hardware registers (UxLSR) and so must not be changed! */
/*@{*/
#define PK_DELETED    0x01   /**< Packet deleted in buffer, do not dispatch (NYI) */
#define PK_OVERRUN    0x02   /**< Packet byte(s) lost due to an overrun condition */
#define PK_PARITY     0x04   /**< Packet byte(s) had a parity error */
#define PK_FRAMING    0x08   /**< Packet byte(s) had a framing error */
#define PK_BREAK      0x10   /**< Break interrupt occurred during packet byte(s) */
#define PK_BUFFER     0x20   /**< Packet exceeded max length, or packet buffer space exhausted */
#define PK_BAD_ESCAPE 0x40   /**< Packet had invalid escaping on some byte(s) */
#define PK_RESERVED8  0x80   /**< Reserved; should be zero; should not be read */

/** \name Packet flag combinations */
/*@{*/
#define PK_BYTE_ERROR (PK_OVERRUN|PK_PARITY|PK_FRAMING|PK_BREAK) /**< If nonzero, some hardware UART
                                                                      per-byte error(s) occurred */
#define PK_PACKET_ERROR (PK_BUFFER|PK_BAD_ESCAPE)   /**< If nonzero, some protocol or packet level
                                                         error(s) occurred */
#define PK_BROKEN (PK_PACKET_ERROR|PK_BYTE_ERROR)   /**< If nonzero, some error(s) of any kind
                                                         occurred in the packet */
/*@}*/

/*@}*/

/** \name Sizes and limits */
/*@{*/
/**
 * The maximum size of a packet (just the raw packet data itself,
 * not including packet framing overhead).
 *
 * There are a lot of arguments going into the MAX_PACKET_LENGTH
 * (MPL) value, favoring big (+), small (-), and other (o):
 *
 * - o Artificial limits are bad; the only limit should be total RAM.
 *
 * - + Failing that, MPL should be as big as possible.
 *
 * - o Total RAM _is_ an artificial limit like any other.  And for a
 *   packet communications protocol, there should always be _some_ size
 *   that's guaranteed to fit; MPL is really a 'minimum maximum size'
 *   guarantee.
 *
 * - - RAM is a precious resource in the MFM with many demands on
 *   it, and the MFM library minimum RAM requirements just for packet
 *   buffers grows as 9MPL. (4 UART * 2MPL per UART buffer + 1 MPL
 *   shared dispatch buffer).
 *
 * - + MPL should be at _least_ 270 or so, because 256 is the minimum
 *   size of an IAP 'Copy RAM to flash' operation, so a packet should
 *   have enough room for 256 data bytes plus addressing, checksum,
 *   and packet overhead.
 *
 * - - Well, that would be nice, but unfortunately the current scheme
 *   breaks if MPL is 252 or higher, because then a packet length
 *   doesn't fit in a u8 and can't be (easily) stored in the data
 *   stream as it currently is.
 *
 * - + Well, gah, you could halfword-align the packet header in the
 *   packet buffer, at a cost of at most one more byte per packet of
 *   overhead, and then manipulate the packet length in place via
 *   *(u16*) stuff.  Probably wouldn't cost a cycle, in the fast path,
 *   on a nice 32 bit machine like this.  So that's a phony argument
 *   for MPL size restriction.
 *
 *   - + And, furthermore, for no more than two bytes more per packet,
 *     more you could guarantee that every packet would start out
 *     word-aligned, which could then be counted on during packet
 *     design.  So that, for example, would allow dispatch code to do a
 *     'Copy RAM to Flash' _directly_ from the packet buffer
 *
 *     - o (Except that, gah, it turns out the frickin IAP routines
 *       require it be _user_ RAM for 'Copy RAM to Flash' operations!
 *       So we _cannot_ burn directly from the packet buffer, 
 *       regardless of packet size or alignment!)
 *
 * UPDATE Mon Apr 13 07:24:13 2009 Note: 288 is the maximum size that allows
 * 7 max-length packets to fit in a 2K buffer (including the 4 byte
 * per packet overhead).
 *
 * UPDATE Thu Jul  2 03:23:46 2009 Note: Considering that we cannot, it turns
 * out, burn directly from packet memory anyway (not without trying to
 * bypass IAP anyway), we _have_ to have an (at least) 256 byte buffer 
 * in fastram, which means there's no essential programming advantage to
 * a packet size big enough to hold 256 data bytes plus overhead all
 * at once.  We could reduce the programming data unit to 128 bytes
 * per packet with little added inefficiency.
 *
 * Given that, and given the fact that 288 is so minimally larger than
 * 252, we have to ask whether the advantages of going a handful a
 * bytes over 252 are worth it.  In particular, if we cut back to max
 * 252, we can go back to a u8 for length, which frees up an entire
 * byte in the packet header -- which is room enough for a whole
 * nother length field.
 *
 * And with a second length field, we could add a 'read index' or
 * 'cursor' to a packet, allowing a packet to be treated as an input
 * stream, with all the extreme handiness and clean code that offers.  
 * In particular, we could have a reasonably non-gross packetScanf..
 *
 * Hmm.  Hmm...
 *
 * But we'd have to rewrite the programming packet code down to 128
 * data bytes before we could even try it..
 *
 * UPDATE Fri Jul  3 09:31:22 2009 Well we did that and it wasn't that 
 * bad; and now we've really gone for it: We've abandoned the
 * four byte alignment requirement as well.  After all, that was only
 * really there to facilitate embedding structs in packets, but
 * because of endianness that's frankly something we don't want to
 * encourage.  Things like #facePrintf() (and the to-come 
 * #packetScanf()) is the way.
 *
 * So now the MPL doesn't even need to be a multiple of four.  But
 * leaving it at 252 since that's the size that produces most complete
 * worst-case use of power-of-2 buffer sizes.
 */
#define MAX_PACKET_LENGTH 252  

/** Size of a standalone (non-FIFO) buffer for a single packet.  Note that since a single packet
 * buffer doesn't wrap (unlike the main rolling buffers), for a standalone single packet buffer we
 * need three extra words beyond the packet data size: One for the header at the front, one to
 * guarantee room for null-termination at the back, and one more so that we can accomodate arbitrary
 * packet offsets and still use word-aligned copying rather than going byte by byte.
 */
#define SINGLE_PACKET_BUFFER_SIZE_WORDS (((MAX_PACKET_LENGTH)>>2)+1)
#define SINGLE_PACKET_BUFFER_SIZE_BYTES (SINGLE_PACKET_BUFFER_SIZE_WORDS<<2)

#define BYTE_BUFFER_BITS 10   /* 1024 bytes.  Big enough for two worst-case escaped packet (though
                                that's not strictly required, since we deframe incrementally). */
#define BYTE_BUFFER_BYTES (1<<BYTE_BUFFER_BITS)

#define BYTE_BUFFER_MASK (BYTE_BUFFER_BYTES-1)


/*@}*/

#endif /*MFMCONSTANTS_H_*/
