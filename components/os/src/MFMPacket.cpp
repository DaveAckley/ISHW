/*                                              -*- mode:C++; fill-column:100 -*-
  MFMPacket.cpp -  Packet structure and access methods
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

/* TO COMPILE FOR TESTING:
  g++ -O0 -g3 -Wall -pedantic -Werror -Wundef    -I../../../src/include  -DHOST_MODE -DTEST_PACKET_BUFFER -o"./testsfbpacketio" MFMPacket.cpp MFMReactor.cpp random.c;./testsfbpacketio
*/


#include <string.h>        /* For memcpy */ 
#include "MFMPacket.h"
#include "MFMAssert.h"

/**
  Access the length of \a packet.
 
  \param packet The packet whose length is needed
 
  \return from 0 to #MAX_PACKET_LENGTH
 
  \blinks #E_API_INVALID_PACKET if \a packet is null or pointing at something that is detectably
  corrupt or not a packet.

  \usage
  \code
    void myHandler(u8 * packet) {
      print("I received a packet of length ");   // Start a packet with a fixed message
      println(packetLength(packet), DEC);        // End it with a number in decimal
    }
  \endcode
 */
u8 packetLength(const u8 * packet) {
  API_ASSERT_VALID_PACKET(packet);
  return packetHeaderInternalUnsafeConst(packet).f[PacketHeader::LENGTH];
}

/**
  Access the 'read length' of \a packet.  The \e read \e length is the difference between the
  #packetLength() and the #packetCursor() of a given packet -- in other words, it is the number of
  further characters that may be read from \a packet.
 
  \param packet The packet whose read length is needed
 
  \return from 0 to #MAX_PACKET_LENGTH
 
  \blinks #E_API_INVALID_PACKET if \a packet is null or pointing at something that is detectably
  corrupt or not a packet.

  \usage
  \code
    void pointlessDemo(u8 * packet) {
      if (packetReadLength(packet) > 0) {    // True if at least one byte can be read
        u8 ch;
        bool result = packetRead(packet,&ch,BYTE);   // So this must succeed
        API_ASSERT_TRUE(result);             // So the universe is insane if it doesn't
      }
    }
  \endcode
 */
u8 packetReadLength(const u8 * packet) {
  API_ASSERT_VALID_PACKET(packet);
  const PacketHeader & ph = packetHeaderInternalUnsafeConst(packet);
  return ph.f[PacketHeader::LENGTH]-ph.f[PacketHeader::CURSOR];
}

/**
  Access the flags of \a packet.  Non-zero packet flags generally, though not necessarily,
  correspond to various error conditions, and by default packets with errors will _not_ cause reflex
  triggering, so in the simplest packet handling circumstances, the return value from this function
  will typically be 0.
 
  \param packet The packet whose flags are to be read.
 
  \return a bitwise-OR of these possible flag values: #PK_DELETED, #PK_OVERRUN, #PK_PARITY,
  #PK_BREAK, #PK_BUFFER, #PK_BAD_ESCAPE.
 
  \blinks #E_API_INVALID_PACKET if \a packet is null or pointing at something that is detectably
  corrupt or not a packet.

  \usage
  \code
    void myAHandler(u8 * packet) {                     // 'Broken' packets not delivered to normal handlers..
      API_ASSERT_ZERO(packetFlags(packet)&PK_BROKEN);  // ..so none of those packet flags can be set.
      ...
    }
    void myBHandler(u8 * packet) {                     // But here we've specially requested broken packets.
      if (packetFlags(packet)&PK_BROKEN)               // ..so this 'if' condition might be true
        println("WARNING!  Packet with errors received");
      ...
    }
    ...
    void setup() {
      reflex('a', myAHandler);      // Define a normal reflex
      reflex('b', myBHandler, MFMReactor::TRIGGER_SPINE, true);  // Final 'true' arg says include broken packets
  \endcode
 */
u8 packetFlags(const u8 * packet) {
  API_ASSERT_VALID_PACKET(packet);
  return packetHeaderInternalUnsafeConst(packet).f[PacketHeader::FLAGS];
}

/**
   Access the source of \a packet.  The packet source is commonly, but not necessarily, the MFM face
   (NORTH, SOUTH, EAST, or WEST) that the packet arrived from.

   \cond
   MFM packets are '8 bit clean', meaning they can contain as data any byte value whatsoever, and so
   in general classic C functions such as 'strlen' -- which assume that a null byte is the string
   terminator -- cannot be used reliably on packets.  

   Nonetheless, in any specific case the programmer may know that some particular packet cannot
   contain any embedded zero bytes, so it could be handy to be able to treat a packet as a
   'null-terminated string', with a zero byte at the end.  It is for that purpose, in such simple
   cases, that the MFM library is designed to ensure such a null byte always exists, following the
   last data byte of every packet.

   Since MFM packets are designed to be packed together solidly in an #MFMPacketIO buffer, one way
   to ensure that a packet is followed by a null byte is to specify that the first byte of the
   following packet header must be such a null byte.  The 'packet zero' is that first, zero-valued
   byte of the packet header.

   Given that the packet zero exists, it is also used as an additional mechanism for checking the
   validity of packet pointer passed into the MFM library routines.  (See, e.g.,
   #API_ASSERT_VALID_PACKET).
   
   Finally, most perplexingly, there are circumstances in which the value of a packet zero may be 1,
   as a flag value, but this is currently undocumented.
   \endcond
   \param packet The packet whose source is to be read.
 
   \return A value from 0 to #MAX_FACE_INDEX-1.
 
   \blinks #E_API_INVALID_PACKET if \a packet is null or pointing at something that is detectably
   corrupt or not a packet.
 */
u8 packetSource(const u8 * packet) {
  API_ASSERT_VALID_PACKET(packet);
  return packetHeaderInternalUnsafeConst(packet).f[PacketHeader::SOURCE];
}

/**
  Access the read cursor of \a packet.  Each packet has an internal \e cursor value indicating how
  much of the packet has been read (e.g., by #packetRead() or #packetScanf()); this method accesses
  that value.  A cursor value of 0 means the cursor is before the first byte, and so the first byte
  will next be read, and a cursor value of #packetLength() means the cursor is after the last byte,
  so no more bytes can be read and #packetReadEOF() will be true.

  Note that #packetReread() can be used to modify the cursor value of a packet.
 
  \param packet The packet whose cursor position is needed
 
  \return from 0 to #packetLength(u8 * packet).
 
  \blinks #E_API_INVALID_PACKET if \a packet is null or pointing at something that is detectably
  corrupt or not a packet.
 */
u8 packetCursor(const u8 * packet) {
  API_ASSERT_VALID_PACKET(packet);
  return packetHeaderInternalUnsafeConst(packet).f[PacketHeader::CURSOR];
}

/**
  Determine if \a packet begins with the sequence of non-null bytes given by \a to.  Note that
  unlike 'packet reading' functions such as #packetRead() or #packetScanf, this function ignores the
  #packetCursor() and always examines \a packet from the beginning.
 
  \param packet The packet whose initial bytes are to be checked for a match against \a to.
 
  \param to The zero-terminated string specifying a prefix to be sought in \a packet.
 
  \return true if #packetLength(const u8 * packet) is >= strlen(to), and the first strlen(to) bytes of \a
  packet are identical to the corresponding bytes of \a to; and false otherwise.
 
  \blinks #E_API_INVALID_PACKET if \a packet is null or pointing at something that is detectably
                                corrupt or not a packet.\par  
  \blinks #E_API_NULL_POINTER if \a to is null.

  \usage
  \code
    void myHandler(u8 * packet) {
      if (zpacketPrefix(packet,"foo")) {
        println("could be 'foo', 'foo bar', or 'food', but not 'fo', 'Foo', or ' foo'");
      }
    }
  \endcode
 */
bool zpacketPrefix(u8 * packet, const char * to) {
  API_ASSERT_VALID_PACKET(packet);
  API_ASSERT_NONNULL(to);
  u32 plen = packetLength(packet);
  while (plen-- && *to) {
    if (*packet++ != (u8) *to++)
      return false;
  }
  return *to == 0;
}

/**
  Determine if \a packet ends with a valid 'check byte'.  A packet \e check \e byte is generated by
  calling #facePrintlnCheckByte() or by using the \p "%\n" format code in #facePrintf(); it is a
  single byte 'summary' or 'digest' of the all the rest of the packet content that precedes it.

  A packet may or may not end with a check byte, depending on how it was generated.  Although it is
  a mistake to call this method on a packet that does not contain a check byte, it is up to the
  programmer to avoid it and this mistake is not detected by the MFM library; the return value of
  such a call will usually be false, but might, simply by bad luck, be true.
 
  Note that this method ignores the #packetCursor(); it always checks the entire \a packet contents.

  \param packet The packet to examine for possessing a valid check byte.
 
  \return true if \a packet has a valid check byte; false if the #packetLength() is zero or the
  final byte is not a valid check byte for the rest of \a packet.
 
  \blinks #E_API_INVALID_PACKET if \a packet is null or pointing at something that is detectably
  corrupt or not a packet.

  \usage
  \code
    void myHandler(u8 * packet) {
      if (!packetCheckByteValid(packet)) // Not doing API_ASSERT_TRUE(packetCheckByteValid(packet)) --
        return;                          // Who knows what the world might send us?  Just ignore bogons..
      println("valid packet");
      ...
    }
    void setup() {
      reflex('h',myHandler);
    }
    void loop() {
      pprintf("hello world%\n");     // The %\n adds the check byte!
      delay(1000);
    }
  \endcode
 */
bool packetCheckByteValid(const u8 * packet) {
  API_ASSERT_VALID_PACKET(packet);
  u32 len = packetLength(packet);
  if (len-- == 0) return false;
  u8 checkByte = CHECK_BYTE_INIT_VALUE;
  for (u32 i = 0; i < len; ++i) {
    CHECK_BYTE_UPDATE(checkByte,packet[i]);
  }
  return checkByte==packet[len];
}


/**
  Starting from the #packetCursor() position, attempt to read a number into \a result, in the format
  specified by \a code, from \a packet.   

  See #packetRead(u8 * packet, int &result, int code, u32 maxLen) for other details including the
  possible values of \a code and the meaning of \a maxLen.
 
  \param packet The packet to read

  \param result A reference to where the read number will be stored, if the return value is true.
 
  \param code A code representing the format to read; see #packetRead(u8 * packet, int &result, int code, u32 maxLen)
 
  \param maxLen How many bytes to read at most; see #packetRead(u8 * packet, int &result, int code, u32 maxLen)
 
  \return true if suitable number was read and stored in \a result; false if no such number can be
     read from the current position, either because an input unsuitable for \a code is seen first,
     or because the end of the packet is reached first.

  \blinks See #packetRead(u8 * packet, int &result, int code, u32 maxLen) for possible errors.

  \sideeffect  The #packetCursor() is advanced by however many bytes were successfully read, which
               will be a value between zero and the smaller of #packetLength() and \a maxlen,
               inclusive.  ::packetCursor() is left set equal to #packetLength(), if the entire
               remainder of \a packet was read, or pointing at the first character of \a packet
               could not be interpreted as part of a number of type \a code, or at the next byte
               to be read after \a maxlen bytes have been read \e by \e this \e call.

  \usage
  \code
    void myHandler(u8 * packet) {
      u32 num;
      if (!packetRead(packet,num,BIN))   // Can't really fail, the way it's set up..
        return false;                    // because there'll always be at least one legal byte to read..
      pprintf("received a packet beginning with %d in binary\n",num); 
    }
    void setup() {
      reflex('0',myHandler);  // Put same handler on both type 0
      reflex('1',myHandler);  // and type 1 packets.
    }
    ...
  \endcode
 */
bool packetRead(u8 * packet, u32 &result, int code, u32 maxLen) {
  return packetRead(packet,*(int*)&result,code,maxLen);
}

static bool packetReadByte(u8 * packet, u8 & result, u32 maxPosition) {
  u32 position = packetCursor(packet);
  if (position >= maxPosition || packetReadEOF(packet))
    return false;
  result = packet[position];
  packetReread(packet,position+1);
  return true;
}

/**
  Starting from the #packetCursor() position of \a packet, read a big-endian network order u64 into
  \a result.  Reads values output by #facePrintBinary(u8 face,u64 value).
 
  \param packet The packet to read from.

  \param result A reference to where the read number will be stored, if the return value is true.
 
  \return true if a u64 was read and stored in \a result; false if no such number could be read from
     the current position, because less than eight bytes remained to be read from \a packet.

  \blinks #E_API_INVALID_PACKET if \a packet is null or pointing at something that is detectably
          corrupt or not a packet.

  \sideeffect \a result is modified (if return true).  The #packetCursor() is advanced by either 8
  (if return true) or 0 (if return false) bytes.

  \since 0.9.20
 */
bool packetRead(u8 * packet, u64 &result) {
  API_ASSERT_VALID_PACKET(packet);
  if (packetReadLength(packet) < 8) return false;
  u64 val = 0;
  for (u32 i = 0; i < 8; ++i) {
    u8 ch;
    packetReadByte(packet,ch,MAX_PACKET_LENGTH);
    val = (val<<8)|ch;
  }
  result = val;
  return true;
}

/**
  Have we read to the end of \a packet?

  \param packet The packet whose #packetCursor() position to check
 
  \return true if and only if the #packetReadLength() of \a packet is 0, meaning no more bytes can
     be read from \a packet.

  \blinks #E_API_INVALID_PACKET if \a packet is null or pointing at something that is detectably
  corrupt or not a packet.

  \usage
  \code
    void myHandler(u8 * packet) {
      u32 ch;
      while (!packetReadEOF(packet)) {   // While there's still something in there..
        packetRead(packet,ch,BYTE);      // ..read another byte
        pprintf("got %c\n", ch);          // This can generate a lot of packets!
     }
    }
    void setup() {
      reflex('s',myHandler);  // 's' for 's'pam-generator..
    }
    ...
  \endcode
 */
bool packetReadEOF(u8 * packet) {
  return (packetCursor(packet) >= packetLength(packet));
}

bool validPacket(const u8 * packet) {
  if (packet==0)
    return false;

  const PacketHeader & ph = packetHeaderInternalUnsafeConst(packet);
  return 
    (ph.f[PacketHeader::LENGTH] <= MAX_PACKET_LENGTH)
    && (ph.f[PacketHeader::CURSOR] <= ph.f[PacketHeader::LENGTH])
    && (ph.f[PacketHeader::SOURCE] < MAX_FACE_INDEX);
}

/**
   Attempt to read a valid chack byte from the current position of \a packet.  

  \param packet The packet to read the check byte from.
 
  \return \p true if and only if #packetReadLength() had been 1 (meaning we had been looking at the
          last byte of \a packet) and that byte was a valid check byte for the rest of \a packet,
          and returns \p false otherwise.

  \blinks #E_API_INVALID_PACKET if \a packet is null or pointing at something that is detectably
          corrupt or not a packet.

  \sideeffect None, if \p false is returned, otherwise the #packetReadLength() of \a packet is
              decreased from 1 to 0, if \p true is returned.

  \usage
  \code
    void myHandler(u8 * packet) {
      u32 val;
      if (!packetRead(packet,val,BYTE) || val != 'x') return; // Bail unless first byte is 'x'
      if (!packetRead(packet,val,DEC) || val >= 100) return;  // Bail unless then a decimal in 0..99
      if (!packetReadCheckByte(packet)) return;               // Bail unless then valid check byte
      pprintf("L received valid 'x%d' packet\n",val); // Announce success
    }
    void setup() {
      reflex('x',myHandler);         // Create an 'x' reflex
    }
    void loop() {
      pprint("x%d%\n",random(100));  // Broadcast x packets.  "%\n" appends the check byte!
      delay(1000);
    }
  \endcode
 */
bool packetReadCheckByte(u8 * packet) {
  API_ASSERT_VALID_PACKET(packet);
  if (packetReadLength(packet) != 1 || !packetCheckByteValid(packet))
    return false;
  u8 res;
  packetReadByte(packet,res,packetLength(packet));
  return true;
}


/**
   Set the value of the 'read cursor' within \a packet to be \a newIndex, or to
   #packetLength(const u8 *), whichever is smaller.

  \param packet The packet whose read cursor to alter

  \param newIndex The new position of \a packet's read cursor (default value: 0)
 
  \blinks #E_API_INVALID_PACKET if \a packet is null or pointing at something that is detectably
          corrupt or not a packet.

  \sideeffect \a packet's read cursor may be altered.

  \usage
  \code
    void myHandler(u8 * packet) {
      u32 serialNumber;
      if (packetScanf(packet,"x%d:",&serialNumber) != 3) return; // Bail unless good start..

      u32 index = packetCursor(packet);              // Now remember where we are
      u32 val;
      if (packetScanf(packet,"0x%x\n", &val) != 4) { // Try for "0x" plus a hexadecimal..
        packetReread(packet,index);                  // But if that fails, back up..
        if (packetScanf(packet,"%d\n",&val) != 2)    // and try for a decimal instead
          return;                                    // And if that fails, punt
      }
      ..
    }
  \endcode
 */
void packetReread(u8 * packet, u32 newIndex) {
  API_ASSERT_VALID_PACKET(packet);
  PacketHeader & ph = packetHeaderInternalUnsafe(packet);
  if (newIndex > ph.f[PacketHeader::LENGTH])
    newIndex = ph.f[PacketHeader::LENGTH];
  ph.f[PacketHeader::CURSOR] = newIndex;
}

/**
  Starting from #packetCursor() of \a packet, read exactly \a length bytes into \a buffer, advancing
  #packetCursor() by \a length, and return \c true.  If #packetReadLength() of \a packet is less
  than \a length return \c false and leave \a packet and \a buffer unchanged.
 
  \param packet The packet to read.

  \param buffer A non-null pointer to at least \a length bytes of writable space.
 
  \param length How many bytes to read.
 
  \return \c true if \a length bytes were read and stored in \a buffer; \c false if no such number
     can be read from the current position, either because an input unsuitable for \a code is seen
     first, or because the end of the packet is reached first.

  \blinks #E_API_INVALID_PACKET if \a packet is null or pointing at something that is detectably
  corrupt or not a packet.

  \blinks #E_API_NULL_POINTER if \a buffer is null.

  \sideeffect If \c true is returned, the #packetCursor() of \a packet is advanced by \a length
               bytes, and \a length bytes are written to \a buffer.

  \since 0.9.10
 */
bool packetRead(u8 * packet, u8 * buffer, u32 length) {
  API_ASSERT_VALID_PACKET(packet);
  API_ASSERT_NONNULL(buffer);
  u32 prl = packetReadLength(packet);
  if (prl < length)
    return false;
  u32 pc = packetCursor(packet);
  memcpy((void*) buffer, (void*) &packet[pc], length);
  packetReread(packet,pc+length);
  return true;
}

bool packetReadPacket(u8 * packet, u8 *& subp) {
  API_ASSERT_VALID_PACKET(packet);
  u32 prl = packetReadLength(packet);
  if (prl < 4+1) return false;  // 4 for header before, 1 for null after
  u8 * possible = packet+packetCursor(packet)+4;
  if (!validPacket(possible)) return false;
  u32 len = packetLength(possible);
  if (prl-4-1 < len) return false;
  subp = possible;
  packetReread(packet,packetCursor(packet)+len+4+1);
  return true;
}

/**
  Starting from #packetCursor() of \a packet, consume no more than \a maxLen bytes to read an
  integer, in one of a variety of formats specified by \a code, and store the read value into \a
  result.
 
  \param packet The packet to read.

  \param result Where to store the read integer value
 
  \param code What format number to read; one of: #BYTE, #BESHORT, or #BELONG, or #BIN, #OCT, #DEC,
              or #HEX, or another number in the range 2..36 to read a number in that base.
 
  \param maxLen Maximum number of bytes to read.
 
  \return \c true if number in the specified format was successfully read and stored in \a result; \c false
     otherwise.

  \blinks #E_API_INVALID_PACKET if \a packet is null or pointing at something that is detectably
  corrupt or not a packet.

  \blinks #E_API_FORMAT_ARG if \a code is not one of the valid values given above.

  \sideeffect If \c true is returned, the #packetCursor() of \a packet is advanced by the number of
              bytes read, and \a result is overwritten with the read value.  Note that if \c false
              is returned, the #packetCursor() of \a packet may or may not have been advanced,
              depending on the reason for the failure.

 */
bool packetRead(u8 * packet, int & result, int code, u32 maxLen) {
  API_ASSERT_VALID_PACKET(packet);

  u32 maxPosition = packetCursor(packet) + maxLen;
  if (code <= BYTE && code >= BELONG) {  // Handle BYTE, BESHORT, and BELONG
    int num = 0;
    for (int i = 1<<-code; i>0; --i) {
      u8 ch;
      if (!packetReadByte(packet,ch,maxPosition)) return false;
      num = (num<<8)|ch;
    }
    result = num;
    return true;
  }

  // Here to read in base 2..36
  API_ASSERT(code >= 2 && code <= 36, E_API_FORMAT_ARG);

  // Skip leading spaces (only, not other 'whitespace')
  u8 ch;
  do {
    if (!packetReadByte(packet,ch,maxPosition))
      return false;             // Can't get started
  } while (ch==' ');

  // ch is now a non-whitespace
  bool negative;
  if ((negative = ch=='-') || ch=='+') {
    if (!packetReadByte(packet,ch,maxPosition))
      return false;             // Can't get started
  } 

  int num = 0;
  bool readSome = false;
  do {
    bool valid = true;
    u8 val = 0;  // Needlessly initted to quiet compiler warning
    if (ch >= '0' && ch <= '9') val = ch-'0';
    else {
      ch |= 0x20;  // standardize on lowercase
      if (ch >= 'a' && ch <= 'z') val = ch-('a'-10);
      else 
        valid = false;
    }
    if (valid && val < code) {
      num = num*code+val;
      readSome = true;
    } else if (readSome) {
      packetReread(packet,packetCursor(packet)-1); // Read one too far, back up
      break;
    } else
      return false;
  } while (packetReadByte(packet,ch,maxPosition));
  result = negative?-num:num;
  return true;
}

bool packetEqual(const u8 * p1, const u8 * p2) {
  API_ASSERT_VALID_PACKET(p1);
  API_ASSERT_VALID_PACKET(p2);
  if (packetLength(p1) != packetLength(p2)) return false;
  for (u32 i = packetLength(p1); i > 0; --i) 
    if (*p1++ != *p2++) return false;
  return true;
}

u8 * makePacket(u8 * buffer, u32 bufferLength, u8 face, const char * packetData) {
  API_ASSERT_NONNULL(packetData);
  u32 len;
  for (len = 0; packetData[len]; ++len) ;
  return makePacket(buffer, bufferLength, face, (const u8 *) packetData, len);
}

u8 * makePacket(u8 * buffer, u32 bufferLength,u8 face, const u8 * packetData, u32 packetDataLength) {
  API_ASSERT_NONNULL(buffer);
  API_ASSERT_NONNULL(packetData);
  API_ASSERT_MAX(packetDataLength,MAX_PACKET_LENGTH-1);
  API_ASSERT_VALID_EXTENDED_FACE(face);
  if (packetDataLength+1+4 > bufferLength) // 1 for null, 4 for header
    return 0;
  u8 * ret = buffer+4;
  PacketHeader & ph = packetHeaderInternalUnsafe(ret);
  ph.f[PacketHeader::SOURCE] = face;
  ph.f[PacketHeader::FLAGS] = 0;
  ph.f[PacketHeader::CURSOR] = 0;
  ph.f[PacketHeader::LENGTH] = packetDataLength;

  u32 i;
  for (i = 0; i < packetDataLength; ++i) 
    ret[i] = packetData[i];

  ret[i] = 0;
  return ret;
}


#ifdef TEST_PACKET_BUFFER

#include <stdio.h>
#include <stdlib.h> // for exit, random

u32 reflexLibraryFlags = 0;

#if 0
void MFMPacketIO::bufferCheck() {

  if (bflags&BFLAG_FULL)
    API_ASSERT_TRUE(newPHIndex <= oldPHIndex && newPHIndex+sizeof(PacketHeader) > oldPHIndex);

  u16 idx = oldPHIndex;
  int packets = 0;

  while (idx != newPHIndex && packets < 10000) {
    PacketHeader & ph = getPacketHeader(idx);
    API_ASSERT_LESS(ph.f[SOURCE],MAX_FACE_INDEX);
    API_ASSERT_LESS_EQUAL(ph.f[LENGTH],MAX_PACKET_LENGTH);
    API_ASSERT_LESS_EQUAL(ph.f[CURSOR],ph.f[LENGTH]);
    API_ASSERT_TRUE(ph.f[FLAGS]==0 || ph.f[FLAGS]==PK_BUFFER);

    idx = skipOverPacket(idx);
    packets++;
  }
  API_ASSERT_LESS(packets,10000);
}

void MFMPacketIO::printBuffer() {

  u16 idx = oldPHIndex;
  bool full = bflags&BFLAG_FULL;
  u32 totalLen = 0;
  u32 totalPackets = 0;
  while (full || idx != newPHIndex) {
    ++totalPackets;
    full = false;
    PacketHeader & ph = getPacketHeader(idx);
    printf("%4d@ %3dF %3dL [", 
           idx, ph.flags, ph.length);
    totalLen += (ph.length+3)&~3;

    int which = forwardBytes(idx,4);
    for (int i = 0; i < ph.length; ++i) {
      printf("%c",buf[which]);
      which = nextByte(which);
    }
    printf("]\n");
    idx = skipOverPacket(idx);
  }
  printf("%4d %4d %4d\n",totalPackets*4+totalLen, totalPackets, totalLen);
  printf("%4d@ %3dF/%3dBF %3dL %4d@@[", newPHIndex, flags, bflags, newPacketLength, forwardBytes(newPHIndex,4+newPacketLength));
  int which = newPHIndex+4;
  for (int i = 0; i < newPacketLength; ++i) {
    printf("%c",buf[which]);
    which = nextByte(which);
  }
  printf("]\n");
}
#endif

void _apiError_(u32 code,const char * file, int lineno) { 
  fprintf(stderr,"APIERROR %d %s:%d\n",code,file,lineno);
  exit(code); 
}

/*
void _dieNow_(const char * m1, const char * m2, const char * m3, const char * m4, u32 blinkCode) {
  exit(blinkCode);
}
*/
void die(const char * file, int line, const char * msg) {
  fprintf(stderr,"%s:%d: %s\n",file, line, msg);
  exit(100);
}

#define TEST(condition) do { if (!(condition)) die(__FILE__,__LINE__,#condition); } while (0)

u8 testIn[MFMPACKETIO_BUFFER_SIZE_BYTES];
static void print(MFMPacketIO & b, const u32 byte) {
  b.putByte((char) byte);
}
#if 0
static void printHex(MFMPacketIO & b, u32 hex) {
  while (hex) {
    u8 byte = (hex>>24)&0xff;
    hex <<= 8;
    if (byte < 10)
      byte += '0';
    else
      byte += 'A'-10;
    b.putByte((char) byte);
  }
}
#endif
static void print(MFMPacketIO & b, const char * str) {
  u8 ch;
  while ((ch= *str++))
    b.putByte(ch);
}
static void println(MFMPacketIO & b, const char * str = "") {
  print(b,str);
  b.terpri();
}

PacketBuffer packetBuffer;

u8 * copyPacketAndDiscardTest(MFMPacketIO & b) {
  return b.copyPacketAndDiscard(packetBuffer);
}

void test1() {
  MFMPacketIO test(testIn);
  test.reset();
}

void test2() {
  MFMPacketIO test(testIn);
  test.reset();
  test.forceSync();

  TEST(test.canAddByte());
  TEST(test.isEmptyOfPackets());  

  for (char ch = 'x'; ch <= 'y'; ++ch) {
    for (int i = 0; i < MAX_PACKET_LENGTH; ++i) {
      TEST(test.canAddByte());
      test.storeByte(ch);
    }
    test.storeByte('\012');
  }

  for (char ch = 'x'; ch <= 'y'; ++ch) {
    TEST(test.packetsRemovable() > 0);

    u8 * ret = copyPacketAndDiscardTest(test);
    TEST(ret);

    TEST(packetLength(ret) == MAX_PACKET_LENGTH);
    TEST(packetFlags(ret) == 0);

    for (u16 i = 0; i < MAX_PACKET_LENGTH; ++i) {
      TEST(ret[i] == ch);
    }
  }
  TEST(test.packetsRemovable()==0);
}

void test3() {
  MFMPacketIO test(testIn);
  test.reset();
  test.forceSync();

  while (test.canAddByte()) {  /* Add length-1 packets until can't no more.. */
    test.storeByte('!');
    test.storeByte('\012');
  }

  TEST(!test.canAddByte());
  TEST(!test.isEmptyOfPackets());  

  /* At 5 bytes per packet.. */ 
  TEST(test.packetsRemovable()==(MFMPACKETIO_BUFFER_SIZE_BYTES/5));

  while (!test.isEmptyOfPackets()) {

    u8 * ret = copyPacketAndDiscardTest(test);
    TEST(ret);

    TEST(packetLength(ret) == 1);
    TEST(packetFlags(ret) == 0);
    TEST(ret[0] == '!');
  }
}

void dispatch4(u8 * packet, u8 source) {
  TEST(packetFlags(packet) == 0);
  TEST(packetLength(packet) == 1);
  TEST(packet[0] == '!');
}

void test4() {
  MFMPacketIO test(testIn);
  test.reset();
  test.forceSync();

  while (test.canAddByte()) {  /* Add length-1 packets until can't no more.. */
    test.storeByte('!');
    test.storeByte('\012');
  }

  TEST(!test.canAddByte());
  TEST(!test.isEmptyOfPackets());  

  /* At 5 bytes per packet */ 
  TEST(test.packetsRemovable()==(MFMPACKETIO_BUFFER_SIZE_BYTES/5));

  while (!test.isEmptyOfPackets()) {

    bool ret = test.dispatchPacket(0,dispatch4);
    TEST(ret);

  }
}

static int packetsBlown = 0;

void dispatch5(u8 * packet, u8 source) {
  if (packetFlags(packet)&PK_BUFFER) {
    ++packetsBlown;
    return;
  }
  TEST(packetFlags(packet) == 0);
  TEST(packet[0] == packetLength(packet)); 
  for (int i = 1; i < packetLength(packet); ++i) {
    TEST(packet[i] == (u8) (i%10+'a'));
  }
}

void test5() {
  MFMPacketIO test(testIn);
  test.forceSync();

  test.bufferCheck();

  for (int packetLen = 1; packetLen <= 255; packetLen++) {

    /* We'll stuff the buffer until we blow a packet, at each size */

    u32 packetsAdded = test.packetsRemovable();
    while (test.canAddByte()) {

      test.storeData((u8) packetLen); /* Store the length we expect */

      test.bufferCheck();
      for (int i = 1; i < packetLen; ++i) {
        test.storeData((u8) (i%10+'a'));
        test.bufferCheck();
      }
      test.terpri();
      test.bufferCheck();

      if (packetsAdded == test.packetsRemovable()) 
        break;
      packetsAdded = test.packetsRemovable();
    }
    test.bufferCheck();

    /* Now dispatch all but one of them */
    while (test.packetsRemovable() > 1) {
      test.bufferCheck();
      bool ret = test.dispatchPacket(0,dispatch5);
      TEST(ret);
      test.bufferCheck();
    }
  }

  TEST(!test.isEmptyOfPackets());
}

static int blown6 = 0;
static int empty6 = 0;
static int flushSome = 0;

void dispatch6(u8 * packet,u8 source) {
  if (packetFlags(packet)==PK_BUFFER) {
    ++blown6;
    return;
  }
  TEST(packetFlags(packet) == 0);
  if (packetLength(packet)==0) {
    ++empty6;
    return;
  }
  if (packet[0]=='S') {
    TEST(packetLength(packet)==3);
    TEST(packet[1]=='I');
    TEST(packet[2]=='M');
  } else if (packet[0]=='p') {
    u8 ch;
    while ((ch = *++packet)) {
      if (packet[1]==0)
        TEST(ch=='z');
      else
        TEST(ch==' '||ch=='1'||ch=='0');
    }
  } else TEST(0);
}

void test6() {
  MFMPacketIO test(testIn);
  test.forceSync();

  test.bufferCheck();

  srandom(3);
  const int EVENTS = 1000000;
  for (int i = 0; i < EVENTS; ++i) {
    int event = random()%5;
    if (flushSome > 0) {
      --flushSome;
      event = 3;
    }
    switch (event) {
    case 0:
      println(test,"SIM");
      break;
    case 1:
      print(test,"p");
      for (int i = 0; i < 8; ++i) {
        print(test," ");
        for (int j = 0; j < 12; ++j) 
          print(test,'0'+random()%2); 
      }
      print(test,'z');
      println(test);
      if (test.isFull())
        flushSome = 5;

      break;
    case 2:
      println(test);  /* Generate an empty packet */
      break;

    case 3:
    case 4:
      if (test.packetsRemovable() > 0) {
        test.bufferCheck();
        bool ret = test.dispatchPacket(0,dispatch6);
        TEST(ret);
        test.bufferCheck();
      }
    }
  }

  while (test.packetsRemovable() > 0) {
    test.bufferCheck();
    bool ret = test.dispatchPacket(0,dispatch6);
    TEST(ret);
    test.bufferCheck();
  }

  TEST(blown6 <  5*EVENTS/100);  // No more than 5% blown
  TEST(empty6 < 20*EVENTS/100);  // No more than 20% empties
}

void dispatch7(u8 * packet,u8 source) {
  TEST(packetFlags(packet)==0);
  TEST(packetLength(packet)==16);

  for (int i = 0; i < 16; ++i) {
    TEST(packet[i] == ((i<<4)|i));
  }
}

#if 0  // This functionaly is not MFMPacketIO's anymore..
void test7() {
  MFMPacketIO test(testIn);
  test.reset();
  test.forceSync();

  test.print(0x00,BYTE);
  test.print(0x1122,BESHORT);
  test.print(0x33445566,BELONG);
  test.print(0x77,BYTE);
  test.print(0x8899,BESHORT);
  test.print(0xaabbccdd,BELONG);
  test.println(0xeeff,BESHORT);

  TEST(test.packetsRemovable()==1);

  while (!test.isEmptyOfPackets()) {

    bool ret = test.dispatchPacket(0,dispatch7);
    TEST(ret);

  }
}
#endif

#include <string.h>

static u8 * makePacket(const char * ch, int len=-1) {
  static union {
    long align;
    u8 buf[100];
  } f;
  if (len < 0)
    len = strlen(ch);
  f.buf[0] = 0;
  f.buf[1] = 0;
  f.buf[2] = 0;
  f.buf[3] = 0;
  for (int i = 0; i < len; ++i) {
    f.buf[i+4] = *ch++;
    ++f.buf[3];
  }
  return &f.buf[4];
}

void test8() {
  int val;

  TEST(packetRead(makePacket("ab",2),val,BYTE));
  TEST(val=='a');

  TEST(packetRead(makePacket("ab",2),val,BESHORT));
  TEST(((val>> 8)&0xff)=='a');
  TEST(((val>> 0)&0xff)=='b');

  TEST(!packetRead(makePacket("abc",3),val,BELONG));

  TEST(packetRead(makePacket("xyzw",4),val,BELONG));
  TEST(((val>>24)&0xff)=='x');
  TEST(((val>>16)&0xff)=='y');
  TEST(((val>> 8)&0xff)=='z');
  TEST(((val>> 0)&0xff)=='w');

  u8 * packet = makePacket("1 2 34 5 6789abc");
  TEST(packetRead(packet,val,DEC));
  TEST(val==1);
  TEST(packetRead(packet,val,3));
  TEST(val==2);
  TEST(packetRead(packet,val,5));
  TEST(val==19);
  TEST(packetRead(packet,val,DEC));
  TEST(val==5);
  TEST(packetRead(packet,val,HEX));
  TEST(val==0x6789abc);

  packet = makePacket("-10001 +000789 +z00           98 F00BAR-R");
  TEST(packetRead(packet,val,BIN));
  TEST(val==-17);
  TEST(packetRead(packet,val,DEC));
  TEST(val==789);
  TEST(packetRead(packet,val,36));
  TEST(val==35*36*36);
  TEST(packetRead(packet,val,DEC));
  TEST(val==98);
  TEST(packetRead(packet,val,HEX));
  TEST(val==0xf00ba);
  TEST(packetRead(packet,val,BYTE));
  TEST(val=='R');
  TEST(packetRead(packet,val,36));
  TEST(val==-27);

  packet = makePacket("P 3902831 7D00 7E00 4a0c8a7676961058 0");
  TEST(packetRead(packet,val,BYTE));
  TEST(val=='P');
  TEST(packetRead(packet,val,DEC));
  TEST(val==3902831);
  TEST(packetRead(packet,val,HEX));
  TEST(val==0x7d00);
  TEST(packetRead(packet,val,HEX));
  TEST(val==0x7e00);
  TEST(packetRead(packet,val,BYTE));
  TEST(val==' ');
  TEST(packetRead(packet,val,HEX,8));
  TEST(val==0x4a0c8a76);
  TEST(packetRead(packet,val,HEX,8));
  TEST(val==0x76961058);
  TEST(packetRead(packet,val,HEX,8));
  TEST(val==0);
}

#if 0
//// check byte computation has moved up to the 
//// MFMPrint level, so can no longer be tested here
void dispatch9(u8 * packet,u8 source) {
  switch (source) {
  case 0:
    TEST(packetLength(packet)==1);
    TEST(packetCheckByteValid(packet));
    TEST(packet[0]==0xF0);
    break;
  case 1:
    TEST(packetLength(packet)==2);
    TEST(packetCheckByteValid(packet));
    TEST(packet[0]==0);
    TEST(packet[1]==0xE1);
    break;
  case 2:
    TEST(packetLength(packet)==42);
    TEST(packetCheckByteValid(packet));
    TEST(packet[41]==0x25);
    break;
  case 3:
    TEST(packetLength(packet)==0);
    TEST(!packetCheckByteValid(packet));
    break;
  case 4:
    TEST(packetLength(packet)==1);
    TEST(!packetCheckByteValid(packet));
    break;
  default:
    TEST(0);
  }
}

void test9() {

  MFMPacketIO test(testIn);
  test.reset();
  test.forceSync();

  //test.printlnCheckByte();

  test.print(0,BYTE);
  //test.printlnCheckByte();

  test.print("A man, a plan, a canal, Palindrome Alaska");
  //test.printlnCheckByte();

  test.println();

  test.println("X");

  TEST(test.packetsRemovable()==5);

  int count = 0;
  while (!test.isEmptyOfPackets()) {

    bool ret = test.dispatchPacket(count++,dispatch9);
    TEST(ret);

  }
}
#endif

static u32 dispatch10blown = 0;
void dispatch10(u8 * packet,u8 source) {
  u32 flags = packetFlags(packet);
  u32 len = packetLength(packet);
  if (flags&PK_BUFFER) {
    ++dispatch10blown;
  }
  TEST(len==packet[0] || (flags&PK_BUFFER));
  for (u32 i = 1; i < len; ++i) {
    TEST(packet[i] == i);
  }
}

void test10() {

  MFMPacketIO test(testIn);
  test.reset();
  test.forceSync();
  
  u32 packetsIn = 0;
  u32 packetsOut = 0;

  srandom(10);
  for (u32 i = 0; i < 100000; ++i) {
    switch (random()%5) {
    case 0:
    case 1:
      {
        u32 len = random()%252+1;
        print(test,len);
        for (u32 rest = 1; rest < len; ++rest)
          print(test,rest);
        println(test);
        ++packetsIn;
      }
      break;
    case 2:
    case 3:
    case 4:
      if (!test.isEmptyOfPackets()) {
        bool ret = test.dispatchPacket(NORTH,dispatch10);
        TEST(ret);

        ++packetsOut;
        break;
      }
      break;
    }
  }
  //  printf("10b=%d in=%d out=%d\n",dispatch10blown,packetsIn,packetsOut);
}

static u32 dispatch11blown = 0;
static u32 dispatch11sent = 0;
static u32 dispatch11rcvd = 0;
#define DISPATCH11_BUF_SIZE 10000
static u8 dispatch11buf[DISPATCH11_BUF_SIZE];

void send11(MFMPacketIO & test,u8 byte) {
  if (dispatch11sent < DISPATCH11_BUF_SIZE) {
    dispatch11buf[dispatch11sent++] = byte;
    print(test,byte);
  }
}
void dispatch11(u8 * packet,u8 source) {
  u32 flags = packetFlags(packet);
  u32 len = packetLength(packet);
  if (flags&PK_BUFFER) {
    ++dispatch11blown;
  }
  TEST(len==1 || (flags&PK_BUFFER));
  TEST(packet[0] == dispatch11buf[dispatch11rcvd]);
  ++dispatch11rcvd;
}

void test11() {

  MFMPacketIO test(testIn);

  test.reset();

  test.setInputFraming(MFMPacketIO::BYTES);
  test.setOutputFraming(MFMPacketIO::BYTES);

  test.forceSync();
  
  u32 packetsIn = 0;
  u32 packetsOut = 0;

  srandom(11);
  while (dispatch11rcvd < DISPATCH11_BUF_SIZE) {
    switch (random()%5) {
    case 0:
    case 1:
      {
        send11(test,random()%256);
        ++packetsIn;
      }
      break;
    case 2:
    case 3:
    case 4:
      if (!test.isEmptyOfPackets()) {
        bool ret = test.dispatchPacket(NORTH,dispatch11);
        TEST(ret);

        ++packetsOut;
        break;
      }
      break;
    }
  }
  printf("11b=%d in=%d out=%d\n",dispatch11blown,packetsIn,packetsOut);
}

static u32 dispatch12sent = 0;
static u32 dispatch12blown = 0;
static u32 dispatch12rcvd = 0;

static const u32 maxnumdigs12 = 4; 

static u32 numdigs12 = maxnumdigs12; 

// Let's make a little state machine to deliver bytes, so we can (deterministically) simulate
// interrupt-driven I/O behavior.  
#define S12INIT 0
#define S12MAKEPACKET 1
#define S12SENDPACKET 2
#define S12IDLE 3

static char buf12[15];
static u32 sidx12;
static u32 slen12;
static int idle12;
static u32 state12 = S12INIT;
static MFMPacketIO * p12;

static u32 buran() {
  return (random()%6)*(random()%6)*(random()%6)+1;
}
static void step12() {
  switch (state12) {
  case S12INIT: {
    state12 = S12MAKEPACKET;
    break;
  }
  case S12MAKEPACKET: {
    buf12[0] = 'r';
    slen12 = numdigs12+1+(numdigs12<maxnumdigs12?random()%2:0);
    TEST(slen12 <= maxnumdigs12+1);
    for (u32 i = 1; i < slen12; ++i) {
      buf12[i] = '1'+i;
    }
    sidx12 = 0;
    ++dispatch12sent;
    state12 = S12SENDPACKET;
    break;
  } 
  case S12SENDPACKET: {
    if (sidx12 >= slen12) {
      println(*p12);
      idle12 = buran();
      state12 = S12IDLE;
      break;
    }
    print(*p12,buf12[sidx12]);
    ++sidx12;
    break;
  }
  case S12IDLE: {
    if (--idle12 <= 0) {
      state12 = S12MAKEPACKET;
      if (numdigs12 == 0) {
        if (buran() > 10) ++numdigs12;
      } else if (numdigs12 == maxnumdigs12) {
        if (buran() > 10) --numdigs12;
      } else 
        numdigs12 += (random()%3) - 1;
    }
    break;
  }
  default:
    TEST(0);
  } 
}
static void burst12() {
  u32 num = buran();
  while (num--) {
    step12();
  }
}

void dispatch12(u8 * packet,u8 source) {

  burst12();
  u32 flags = packetFlags(packet);
  u32 len = packetLength(packet);
  if (flags&PK_BUFFER) {
    ++dispatch12blown;
    return;
  }
  TEST(len<=3+maxnumdigs12 || (flags&PK_BUFFER));
  TEST(packet[0] == 'r');
  for (u32 i = 1; i < len; ++i) {
    burst12();
    TEST(packet[i] == '1'+i);
  }
  ++dispatch12rcvd;
}

void test12() {

  MFMPacketIO test(testIn);

  p12 = &test;
  test.reset();
  test.forceSync();
  
  srandom(12);
  for (int tests = 0; tests < 1000000; ++tests) {
    u32 num = random()%4;
    if (test.isFull() && random()%3==0)
      num = 3;
    switch (num) {
    case 0:
    case 1:
      {
        burst12();
      }
      break;
    case 2:
    case 3:
      {
        bool wasEmpty = test.isEmptyOfPackets();
        burst12();
        bool ret = test.dispatchPacket(NORTH,dispatch12);
        burst12();
        TEST(ret || wasEmpty);

        break;
      }
      break;
    }
  }
  printf("12b=%d in=%d out=%d\n",dispatch12blown,dispatch12sent,dispatch12rcvd);
}

int main() {
  test12();
  test11();
  test10();
  //  test9();
  test8();
  //  test7();
  test6();
  test1();
  test2();
  test3();
  test4();
  test5();
  return 0;
}
#endif
