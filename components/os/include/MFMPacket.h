/*                                             -*- mode:C++; fill-column:100 -*-
  MFMPacket.h - Packet structure and access methods
  Copyright (C) 2009-2013 The Regents of the University of New Mexico.  All rights reserved.

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
  \file MFMPacket.h Packet structure and access methods
  \author David H. Ackley.  
  \date (C) 2009 All rights reserved.
  \lgpl
 */
#ifndef MFMPACKET_H_
#define MFMPACKET_H_

#include "MFMTypes.h"
#include "MFMAssert.h"

/**
   The information associated with a packet, when it is stored in memory only -- there is no
   preestablished packet header on the wire.  Normally these fields are never accessed directly,
   instead, see #packetSource(), #packetFlags(), #packetCursor(), and #packetLength().
 */
struct PacketHeader {
  u8 f[4];
  enum Fields {
    SOURCE,                   /* NORTH, SOUTH, EAST, WEST, or some more exotic source */ 
    FLAGS,                    /* PK_OVERRUN|PK_PARITY|PK_FRAMING|PK_BREAK|PK_BUFFER|PK_BAD_ESCAPE */
    CURSOR,                   /* Current read index within the packet */ 
    LENGTH,                   /* Overall packet data length (excluding header and trailing null) */ 
    FIELD_COUNT
  };
};

/** \cond */
struct PacketBuffer {
  union {
    u32 wh;
    PacketHeader ph;
  } h;
  u8 bbuf[SINGLE_PACKET_BUFFER_SIZE_BYTES];
};
/** \endcond */

/* MFM packet framing special character codes */
#define PFSC_END  ((u8) '\012')  /* End of packet. AKA 'newline', ^J, 0x0a, NOT carriage return */
#define PFSC_ESC  ((u8) '\333')  /* Indicates escaped byte code follows, 0xdb */
#define PFSC_EEND ((u8) '\334')  /* ESC EEND means END data byte, 0xdc */
#define PFSC_EESC ((u8) '\335')  /* ESC EESC means ESC data byte, 0xdd */

#define CHECK_BYTE_INIT_VALUE (0xF0)
#define CHECK_BYTE_UPDATE(v,d) ((v) =  ((v<<1)|((v>>7)&1))^d)

/** Low-level packet handling methods 
    @{
*/

inline const PacketHeader & packetHeaderInternalUnsafeConst(const u8 * packet) {
  return *(const PacketHeader *) (packet-4);
}

inline PacketHeader & packetHeaderInternalUnsafe(u8 * packet) {
  return *(PacketHeader *) (packet-4);
}

/**
   Check if two packets have the same content.  Considers \e only the lengths and content of the
   packets; any possible differences in their #packetSource(), #packetFlags(), and/or
   #packetCursor() are ignored.

   \return \c true if the \a packet1 and \a packet2 are the same length and all #packetLength()
   bytes match; \c false otherwise

   \blinks #E_API_INVALID_PACKET if \a packet1 or \a packet2 is null or points at an invalid packet.

   \since 0.9.9
 */
extern bool packetEqual(const u8 * packet1, const u8 * packet2);

extern u8 packetLength(const u8 * packet);

extern u8 packetReadLength(const u8 * packet);

extern u8 packetFlags(const u8 * packet);

extern u8 packetSource(const u8 * packet);

extern u8 packetCursor(const u8 * packet);

extern bool packetCheckByteValid(const u8 * packet);

#define API_ASSERT_VALID_PACKET(u8ptr) API_ASSERT(validPacket(u8ptr),E_API_INVALID_PACKET)

extern bool validPacket(const u8 * packet) ;

extern void packetReread(u8 * packet, u32 newIndex = 0) ;

extern bool packetRead(u8 * packet, int &result, int code = DEC, u32 maxLen = MAX_PACKET_LENGTH) ;

extern bool packetRead(u8 * packet, u32 &result, int code = DEC, u32 maxLen = MAX_PACKET_LENGTH) ;

extern bool packetRead(u8 * packet, u64 &result) ;

extern bool packetRead(u8 * packet, u8 * result, u32 length) ;

extern bool packetReadCheckByte(u8 * packet) ;

extern bool packetReadEOF(u8 * packet) ;

extern bool packetReadPacket(u8 * packet, u8 *& subpacket) ;

extern bool zpacketPrefix(u8 * packet, const char * to);

extern u8 * makePacket(u8 * buffer, u32 bufferLength, u8 face, const char * packetData);

extern u8 * makePacket(u8 * buffer, u32 bufferLength, u8 face, const u8 * packetData, u32 packetDataLength);

/** 
    @}
*/

#endif /* MFMPACKET_H_ */
