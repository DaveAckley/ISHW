/*                                              -*- mode:C++; fill-column:100 -*-
  MFMMacros.h - Standalone early macros requiring no constants.
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
  \file MFMMacros.h Various utility macro definitions.
  \author David H. Ackley.
  \date (C) 2008 All rights reserved.
  \lgpl
 */
#ifndef MFMMACROS_H_
#define MFMMACROS_H_

#include "MFMTypes.h"


/** \name Operations on MFM face codes.

    These macros are generally only valid for the "physical faces" like #NORTH - #WEST */
/*@{*/
#define CLOCKWISE_FACE(dir)        GET_MAP4BY2(MAP4BY2(EAST,WEST,SOUTH,NORTH),dir) /**< Maps N->E,
                                                                                      S->W, E->S,
                                                                                      W->N */
#define COUNTERCLOCKWISE_FACE(dir) GET_MAP4BY2(MAP4BY2(WEST,EAST,NORTH,SOUTH),dir) /**< Maps N->W,
                                                                                      S->E, E->N,
                                                                                      W->S */
#define ANTICLOCKWISE_FACE(dir) COUNTERCLOCKWISE_FACE(dir)
#define OPPOSITE_FACE(dir)         GET_MAP4BY2(MAP4BY2(SOUTH,NORTH,WEST,EAST),dir) /**< Maps N->S,
                                                                                      S->N, E->W,
                                                                                      W->E */

extern const char * const faceNames[];
#define FACE_NAME(face) (faceNames[face]) /**< Maps N,S,E,W -> "North","South","East","West" */
#define FACE_CODE(face) GET_MAP4BY8(MAP4BY8('N','S','E','W'),face) /**< Maps N,S,E,W ->
                                                                      'N','S','E','W' */
#define FACE_NUMBER_FROM_CODE(code) (((((code)&~0x20)^(((code)&~0x20)>>1))-1)&0x3)  /**< Maps
                                                                                       'N','n'->#NORTH,
                                                                                       'S','s'->#SOUTH,
                                                                                       'E','e'->#EAST,
                                                                                       'W','w'->#WEST */
/* This looks fancy shmancy but compiles into ten instructions:

  #define IS_FACE_CODE(code) ((((code)>>6)==1) && ((0x00884020>>((code)&0x1f))&1))

  (mostly because developing the constant sucks), whereas this looks gross and stupid but compiles
  into eight instructions: */
#define IS_FACE_CODE(code)                     \
  (((code)&~0x20)=='N'||   \
   ((code)&~0x20)=='S'||   \
   ((code)&~0x20)=='E'||   \
   ((code)&~0x20)=='W')       /**< 'N','n','S','s','E','e','W','w'->1,everything else-> 0 */


/*@}*/

/** \name Bit Operations

    These macros generally expect u32 operands, though some may work on smaller types */
/*@{*/

/** Set bit \a bitNum in \a value
  \usage
  \code
    ...
     u32 num = 0;
     SET_BIT(num,3);
     println(num);     // Prints 8 (since that's 1000 in binary)
   ...
  \endcode
*/
#define SET_BIT(value,bitNum) ((value) |= (1<<(bitNum)))

/** Clear bit \a bitNum in \a value
  \usage
  \code
    ...
     u32 num = 3;      // I.e., 11 in binary
     CLEAR_BIT(num,0); // Num is now 10 in binary
     println(num);     // Prints 2
   ...
  \endcode
*/
#define CLEAR_BIT(value,bitNum) ((value) &= ~(1<<(bitNum)))

/** Access bit \a bitNum in \a value
  \returns 1 if the specified bit is set, else 0
  \usage
  \code
    ...
     u32 num = 2;      // I.e., 11 in binary
     println(GET_BIT(num,1));     // Prints 1 since bit 1 of binary 10 is set
     println(GET_BIT(num,3));     // Prints 0 since bit 3 of binary 10 is clear
   ...
  \endcode
*/
#define GET_BIT(value,bitNum) (((value)>>(bitNum))&1)

/** Check if bit \a bitNum in \a value is set.  Typically produces slightly faster code, compared to
    #GET_BIT, when \a bitNum is a constant.

  \returns 0 if the specified bit is clear, else a non-zero value (not necessarily 1)
  \usage
  \code
    ...
     u32 num = 2;      // I.e., 10 in binary
     if (TEST_BIT(num,1)) println("Yes"); else println("No"); // prints Yes
   ...
  \endcode
*/
#define TEST_BIT(value,bitNum) ((value)&(1<<(bitNum)))

/** Set a consecutive pair of bits in \a bits, located at \a position, to value \a newValue.  \a
    newValue must be in the range of 0 to 3 for correct operation.

  \returns the modified value of \a bits.
  \usage
  \code
    ...
     u32 num = 0;                    // num: ...000000 binary
     BIT_PAIR_AT_POSITION(num,3,1);  // num: ...000110 binary
     println(num);                   // prints 6
     BIT_PAIR_AT_POSITION(num,2,2);  // num: ...001010 binary
     println(num);                   // prints 10
   ...
  \endcode
*/
#define BIT_PAIR_AT_POSITION(bits,newValue,position) \
  ((bits) = ((bits)&(~(3<<(position))))|((newValue)<<(position)))
/*@}*/

/****************** Structured H/W register access *****************/

/**
 * Access a 32 bit H/W register.  Access given a base address and a BYTE COUNT offset, (which must
 * be a multiple of four or you'll be making an unaligned access).
 */
#define REGISTER_OFFSET(baseAddress,byteOffset) \
  (*((uv32 *) (((u32)(baseAddress))+(byteOffset))))

/** \name Small Constant Array Maps
 */

/*@{*/
/** Encode 4 numbers each 2 bits long, see #MAP3BY3 for background.
    \param a first argument becomes index 0 in the map
    \param b second argument becomes index 1 in the map
    \param c third argument becomes index 2 in the map
    \param d fourth argument becomes index 3 in the map
  \usage
  \code
    ...
     u32 num = MAP4BY2(1,2,3,3);     // Create a 4 by 2 SCAM
     println(GET_MAP4BY2(num,0));    // prints 1
     println(GET_MAP4BY2(num,1));    // prints 2
     println(GET_MAP4BY2(num,2));    // prints 3
     println(GET_MAP4BY2(num,3));    // prints 3
   ...
  \endcode
*/
#define MAP4BY2(a, b, c, d) ((((((((u8) d)<<2)|(c))<<2)|(b))<<2)|(a))

/** Access the values encoded by #MAP4BY2
    \param map The value created by #MAP4BY2
    \param idx The array index to access, from 0 to 3
  \usage
  \code
    ...
     u32 num = MAP4BY2(1,2,3,3);     // Create a 4 by 2 SCAM
     println(GET_MAP4BY2(num,0));    // prints 1
     println(GET_MAP4BY2(num,1));    // prints 2
     println(GET_MAP4BY2(num,2));    // prints 3
     println(GET_MAP4BY2(num,3));    // prints 3
   ...
  \endcode
*/
#define GET_MAP4BY2(map, idx) (((map)>>((idx)<<1))&0x3)

/** Encode 4 numbers each 4 bits long, see #MAP3BY3 for background.
    \param a first argument becomes index 0 in the map
    \param b second argument becomes index 1 in the map
    \param c third argument becomes index 2 in the map
    \param d fourth argument becomes index 3 in the map
  \usage
  \code
    ...
     u32 num = MAP4BY4(9,7,2,0xf);   // Create a 4 by 4 SCAM
     println(GET_MAP4BY4(num,0));    // prints 9
     println(GET_MAP4BY2(num,1));    // prints 7
     println(GET_MAP4BY2(num,2));    // prints 2
     println(GET_MAP4BY2(num,3));    // prints 15
   ...
  \endcode
*/
#define MAP4BY4(a, b, c, d) ((((((((u16) d)<<4)|(c))<<4)|(b))<<4)|(a))

/** Access the values encoded by #MAP4BY4
    \param map The value created by #MAP4BY4
    \param idx The array index to access, from 0 to 3
  \usage
  \code
    ...
     u32 num = MAP4BY4(9,7,2,0xf);   // Create a 4 by 4 SCAM
     println(GET_MAP4BY4(num,0));    // prints 9
     println(GET_MAP4BY2(num,1));    // prints 7
     println(GET_MAP4BY2(num,2));    // prints 2
     println(GET_MAP4BY2(num,3));    // prints 15
   ...
  \endcode
*/
#define GET_MAP4BY4(map, idx) (((map)>>((idx)<<2))&0xf)

/** Encode 4 numbers each 8 bits long, see #MAP3BY3 for background.
    \param a first argument becomes index 0 in the map
    \param b second argument becomes index 1 in the map
    \param c third argument becomes index 2 in the map
    \param d fourth argument becomes index 3 in the map
  \usage
  \code
    ...
     u32 num = MAP4BY8(0x12,' ',99,127);   // Create a 4 by 8 SCAM
     println(GET_MAP4BY8(num,0));    // prints 18
     println(GET_MAP4BY8(num,1));    // prints 32 (ASCII value of the space ' ' char)
     println(GET_MAP4BY8(num,2));    // prints 99
     println(GET_MAP4BY8(num,3));    // prints 127
   ...
  \endcode
*/
#define MAP4BY8(a, b, c, d) ((((((((u32) d)<<8)|(c))<<8)|(b))<<8)|(a))
/** Access the values encoded by #MAP4BY8
    \param map The value created by #MAP4BY8
    \param idx The array index to access, from 0 to 3
  \usage
  \code
    ...
     u32 num = MAP4BY8(0x12,' ',99,127);   // Create a 4 by 8 SCAM
     println(GET_MAP4BY8(num,0));    // prints 18
     println(GET_MAP4BY8(num,1));    // prints 32 (ASCII value of the space ' ' char)
     println(GET_MAP4BY8(num,2));    // prints 99
     println(GET_MAP4BY8(num,3));    // prints 127
   ...
  \endcode
*/
#define GET_MAP4BY8(map, idx) (((map)>>((idx)<<3))&0xff)

/** Encode 3 numbers each 3 bits long.  The primary use of such 3 by 3 maps is for efficiently
    storing and accessing the 'blink codes' displayed when errors occur; see #BLINK_MAP and
    \ref MFMErrors.h .

    These macros (#MAP3BY3, #MAP4BY2, #MAP4BY4, #MAP4BY8) encode a small array of small numbers into
    a single constant value -- a Small Constant Array Map (SCAM) -- which can be accessed and stored
    with relatively little code and memory.  They come in matched pairs -- a \c MAPxBYy to make a
    SCAM, and a \c GET_MAPxBYy to access it.  In such pairings, the \e x is the number of
    associations in the map, and the \e y is number of bits in each mapped value.

    In all cases it is the programmer's responsibility to ensure that all arguments passed to these
    macros are in range for the sizes involved; the macros do no error checking.  Note also that if
    any of the arguments to a \c MAPxBYy macro are not \e compile-time constants, there are likely
    no efficiency gains, and it is probably not worth using these macros, in that context, at all.

    \param a first argument becomes index 0 in the map
    \param b second argument becomes index 1 in the map
    \param c third argument becomes index 2 in the map
  \usage
  \code
    ...
     u32 num = MAP3BY3(2,0,4);   // Create a 3 by 3 SCAM
     println(GET_MAP3BY3(num,0));    // prints 2
     println(GET_MAP3BY3(num,1));    // prints 0
     println(GET_MAP3BY3(num,2));    // prints 4
   ...
  \endcode
*/
#define MAP3BY3(a, b, c) ((((((u16) c)<<3)|(b))<<3)|(a))

/** Access the values encoded by #MAP3BY3
    \param map The value created by #MAP3BY3
    \param idx The array index to access, from 0 to 2
  \usage
  \code
    ...
     u32 num = MAP3BY3(2,0,4);   // Create a 3 by 3 SCAM
     println(GET_MAP3BY3(num,0));    // prints 2
     println(GET_MAP3BY3(num,1));    // prints 0
     println(GET_MAP3BY3(num,2));    // prints 4
   ...
  \endcode
*/
#define GET_MAP3BY3(map, idx) (((map)>>((idx)*3))&0x7)

/** Use a MAP3BY3 to encode a 'blink code' of \a red, \a green, and \a blue flash counts.  BLINK_MAP
    puts \a red at the left of the map, so that any BLINK_MAP with \a red less than 4 will yield a
    constant that fits in 8 bits -- which the compiler will be able to generate in one instruction
    via an immediate operand, avoiding a memory reference.

    \param red number of red flashes, 0 to 7
    \param green number of green flashes, 0 to 7
    \param blue number of blue flashes, 0 to 7

  \usage
  \code
    ...
     API_ASSERT(foo<bar,BLINK_MAP(0,2,1));  // Die blinking 0,2,1 (a sketch custom code), unless foo < bar
   ...
  \endcode
*/
#define BLINK_MAP(red,green,blue) MAP3BY3(blue,green,red)


/** Access the color flash counts encoded by #BLINK_MAP
    \param code The value created by #BLINK_MAP
    \param color The color to access, either 0==RED, 1==GREEN, or 2==BLUE
  \usage
  \code
    ...
     u32 map = BLINK_MAP(2,0,4);   // Create a blink map
     println(GET_BLINK_MAP_COLOR(map,0));    // prints 2
     println(GET_BLINK_MAP_COLOR(map,1));    // prints 0
     println(GET_BLINK_MAP_COLOR(map,2));    // prints 4
   ...
  \endcode
*/
#define GET_BLINK_MAP_COLOR(code,color) GET_MAP3BY3(code,2-(color))
/*@}*/

/** \name Fast u32 Comparisons Handling Rollover */
/*@{*/
/**
   Functions like #millis() and #micros() can be tricky to use robustly.  It's not too hard to write
   code like:
   \code
      u32 end = millis()+1000; // Compute the value of millis() 1 second from now
      ..prepare..              // Take some time preparing to work..
      while (millis() < end) { // ..then while there's still time in this second [[WARNING!  CONTAINS ERROR!]]
        ..work..               // ..do some low-priority work
      }
   \endcode

   which seems very safe, and will indeed work correctly \e most of the time.  But suppose the \e
   prepare step might sometimes take more than one second?  You might answer "So what?  Then the \c
   while loop will just quit immediately, because #millis() is already later than \c end so there's
   no time left to work."

   Indeed.  But, just suppose your sketch has already been running for a long long time.  After
   about 49 days, the total number of elapsed milliseconds exceeds the maximum possible value of a
   \c u32!  When that happens, ::millis() "rolls over" back to zero!  So a terrible possibility
   arises.  Suppose:

   -# As the code is entered, #millis() happens to be very large, but \c millis()+1000 doesn't quite
      roll over, so \c end gets a very large value, and
   -# the \e prepare step runs overtime by enough that #millis() then \e does roll over, and so
      begins returning very small values.  That means
   -# the \c while loop will then perform its low-priority work, not for less than a second, but for
      the next 49 days!

   (If you can't even \e dream of your sketch running for six weeks solid, well, you be the judge.
   Still, one might consider dreaming \e bigger -- or consider #micros(), which rolls over about
   every 70 minutes.  And either way, the fix is sweet; read on.)

   The fundamental problem, given that counters \e will eventually roll over, is that '<' doesn't
   quite capture the most useful notion of 'earlier' or 'before'.  In the above example, if \c end
   is very large and #millis() is very small, the most sensible interpretation is that #millis() has
   wrapped and is therefore 'later' than or 'after' \c end, even though in that case #millis() is
   numerically \e less than \c end.

   And we \e could try to fix our code along those lines, saying that "< means earlier" \e unless
   some added special-case code decides that a wraparound has occurred.  But, it turns out, there's
   a better, faster, and cleaner way to do it.

   With two values \e a and \e b referenced to the same rollover counter, what we'd really like to
   know is: <b>Would the counter count from \e a to \e b quicker than \e b to \e a, considering
   rollovers?</b> If counting from \e a to \e b is shorter, then we're better off saying \e a is \b
   earlier than \e b.

   And that's exactly the comparison the #IS_EARLIER macro makes.

   And, it does it wicked \e fast: The comparison itself is typically just three ARM assembly
   language instructions.  If \c IS_EARLIER(a,b) is \c true, then it takes less
   counting-ups-with-possible-wrapping to go from \e a to \e b than \e b to \e a, so we assume \e a
   is earlier than \e b.  Now, in general that assumption could be wrong, but in any particular case
   if we know our #millis() operands are less than about 24 days apart (or #micros() operands are
   less than about a half hour apart), then we are \e golden.

   Our resulting code, hardly changed but now satisfyingly robust, looks like this:
   \code
      u32 end = millis()+1000; // Compute the value of millis() 1 second from now
      ..prepare..              // Take some time preparing to work..
      while (IS_EARLIER(millis(),end)) { // ..then while there's still time in this second
        ..work..               // ..do some low-priority work
      }
   \endcode
   and it will work as desired so long as the \e prepare step definitely finishes in less than
   about 24 days.
 */

#define IS_EARLIER(a,b)          ((((u32)(a))-((u32)(b))) > 0x80000000UL)

#define IS_LATER_OR_EQUAL(a,b)   ((((u32)(a))-((u32)(b))) < 0x80000000UL) /**< \a a after or same as
                                                                             \a b, with wraparound;
                                                                             see #IS_EARLIER. */
#define IS_LATER(a,b)            ((((u32)(b))-((u32)(a))) > 0x80000000UL) /**< \a a after \a b, with
                                                                             wraparound; see
                                                                             #IS_EARLIER. */
#define IS_EARLIER_OR_EQUAL(a,b) ((((u32)(b))-((u32)(a))) < 0x80000000UL) /**< \a a before or same
                                                                             as \a b, with
                                                                             wraparound; see
                                                                             #IS_EARLIER. */
#define IS_EARLIER16(a,b)        ((u16)(((u16)(a))-((u16)(b))) > 0x8000UL)  /**< \a a before \a b, with
                                                                         wraparound, for u16
                                                                         values; see #IS_EARLIER */
#define IS_LATER_OR_EQUAL16(a,b) ((u16)(((u16)(a))-((u16)(b))) < 0x8000UL)  /**< \a a after or same as \a
                                                                         b, with wraparound, for u16
                                                                         values; see #IS_EARLIER. */
#define IS_LATER16(a,b)          ((u16)(((u16)(b))-((u16)(a))) > 0x8000UL)  /**< \a a after \a b, with
                                                                         wraparound, for u16 values;
                                                                         see #IS_EARLIER. */
#define IS_EARLIER_OR_EQUAL16(a,b) ((u16)(((u16)(b))-((u16)(a))) < 0x8000UL)/**< \a a before or same as \a
                                                                         b, with wraparound, for u16
                                                                         values; see #IS_EARLIER. */
#define IS_EARLIER8(a,b)        ((u8)(((u8)(a))-((u8)(b))) > 0x80UL)  /**< \a a before \a b, with
                                                                         wraparound, for u8
                                                                         values; see #IS_EARLIER */
#define IS_LATER_OR_EQUAL8(a,b) ((u8)(((u8)(a))-((u8)(b))) < 0x80UL)  /**< \a a after or same as \a
                                                                         b, with wraparound, for u8
                                                                         values; see #IS_EARLIER. */
#define IS_LATER8(a,b)          ((u8)(((u8)(b))-((u8)(a))) > 0x80UL)  /**< \a a after \a b, with
                                                                         wraparound, for u8 values;
                                                                         see #IS_EARLIER. */
#define IS_EARLIER_OR_EQUAL8(a,b) ((u8)(((u8)(b))-((u8)(a))) < 0x80UL)/**< \a a before or same as \a
                                                                         b, with wraparound, for u8
                                                                         values; see #IS_EARLIER. */
/*@}*/


/** \name Miscellaneous macro hackery */
/*@{*/

/**
   Find the offset of a particular \a member within some \a structure.  Sometimes it is necessary or
   helpful to know how for 'into' a given \c struct or \c class some particular member is located.
   STRUCT_MEMBER_OFFSET produces a non-negative \c u32 giving the offset to that \a member from the
   beginning of the \a structure, in bytes.

   \usage
   \code
   struct Foo {                                // Some random struct..
     u8 title[17];
     u8 type;
     u32 value;
   };
   ...
     println(STRUCT_MEMBER_OFFSET(Foo,type));  // Prints 17.  'title' takes 17 bytes, which land at 
                                               // offsets 0..16, and then 'type' immediately follows.
   \endcode
 */
#define STRUCT_MEMBER_OFFSET(structure,member) ((uptr) &(((structure *) 0)->member))

#define XSTR(arg) STR(arg)  /**< The (now) traditional, but still disgusting, two-level
                               stringification song and dance to stringify an expansion result,
                               using #XSTR and #STR  */
#define STR(arg) #arg       /**< The (now) traditional, but still disgusting, two-level
                               stringification song and dance to stringify an expansion result,
                               using #XSTR and #STR  */

#define MIN(a,b) (((a)<(b))?(a):(b))  /**< The traditional minimum value macro.  This is unsafe
                                           because it evaluates one of its arguments twice, so it
                                           must not be used with arguments that have side-effects.
                                           \return the smaller of its two arguments
                                      */

#define MAX(a,b) (((a)>(b))?(a):(b)) /**< The traditional maximum value macro.  This is unsafe
                                           because it evaluates one of its arguments twice, so it
                                           must not be used with arguments that have side-effects.
                                           \return the larger of its two arguments
                                      */

/** FILEPOS yields the current position in the source code as a "MFMMacros.h:63"-style string */
#define FILEPOS __FILE__ ":" XSTR(__LINE__)
/*@}*/


/****************** B36 constants macros ******************/

/** \cond */
#define B36_CONST_0 0u
#define B36_CONST_1 1u
#define B36_CONST_2 2u
#define B36_CONST_3 3u
#define B36_CONST_4 4u
#define B36_CONST_5 5u
#define B36_CONST_6 6u
#define B36_CONST_7 7u
#define B36_CONST_8 8u
#define B36_CONST_9 9u
#define B36_CONST_A 10u
#define B36_CONST_a 10u
#define B36_CONST_B 11u
#define B36_CONST_b 11u
#define B36_CONST_C 12u
#define B36_CONST_c 12u
#define B36_CONST_D 13u
#define B36_CONST_d 13u
#define B36_CONST_E 14u
#define B36_CONST_e 14u
#define B36_CONST_F 15u
#define B36_CONST_f 15u
#define B36_CONST_G 16u
#define B36_CONST_g 16u
#define B36_CONST_H 17u
#define B36_CONST_h 17u
#define B36_CONST_I 18u
#define B36_CONST_i 18u
#define B36_CONST_J 19u
#define B36_CONST_j 19u
#define B36_CONST_K 20u
#define B36_CONST_k 20u
#define B36_CONST_L 21u
#define B36_CONST_l 21u
#define B36_CONST_M 22u
#define B36_CONST_m 22u
#define B36_CONST_N 23u
#define B36_CONST_n 23u
#define B36_CONST_O 24u
#define B36_CONST_o 24u
#define B36_CONST_P 25u
#define B36_CONST_p 25u
#define B36_CONST_Q 26u
#define B36_CONST_q 26u
#define B36_CONST_R 27u
#define B36_CONST_r 27u
#define B36_CONST_S 28u
#define B36_CONST_s 28u
#define B36_CONST_T 29u
#define B36_CONST_t 29u
#define B36_CONST_U 30u
#define B36_CONST_u 30u
#define B36_CONST_V 31u
#define B36_CONST_v 31u
#define B36_CONST_W 32u
#define B36_CONST_w 32u
#define B36_CONST_X 33u
#define B36_CONST_x 33u
#define B36_CONST_Y 34u
#define B36_CONST_y 34u
#define B36_CONST_Z 35u
#define B36_CONST_z 35u
/** \endcond */

#define B36_1(a) B36_CONST_##a                                  /**< A length 1 base 36 constant */
#define B36_2(a,b) (B36_1(a)*36u+B36_1(b))                      /**< A length 2 base 36 constant */
#define B36_3(a,b,c) (B36_2(a,b)*36u+B36_1(c))                  /**< A length 3 base 36 constant */
#define B36_4(a,b,c,d) (B36_3(a,b,c)*36u+B36_1(d))              /**< A length 4 base 36 constant */
#define B36_5(a,b,c,d,e) (B36_4(a,b,c,d)*36u+B36_1(e))          /**< A length 5 base 36 constant */
#define B36_6(a,b,c,d,e,f) (B36_5(a,b,c,d,e)*36u+B36_1(f))      /**< A length 6 base 36 constant. (Note: Some argument combinations are invalid)  */
#define B36_7(a,b,c,d,e,f,g) (B36_6(a,b,c,d,e,f)*36u+B36_1(g))  /**< A length 7 base 36 constant. (Note: Argument 'a' must be 0 to have even a chance of this producing a valid constant.  Why does this macro exist?) */

/****************** Memory placement macros ******************/

#ifndef HOST_MODE

#define IN_BODYRAM  __attribute__((section (".bodyram")))
#define IN_TALKRAM  __attribute__((section (".talkram")))
#define IN_FASTRAM  __attribute__((section (".fastram")))

#define IN_BODYRAM_BSS  __attribute__((section (".bss.bodyram")))
#define IN_TALKRAM_BSS  __attribute__((section (".bss.talkram")))
#define IN_FASTRAM_BSS  __attribute__((section (".bss.fastram")))

#else

#define IN_BODYRAM  /* in host mode */
#define IN_TALKRAM  /* no special ram regions */
#define IN_FASTRAM  /* are defined */

#define IN_BODYRAM_BSS  /* for */
#define IN_TALKRAM_BSS  /* any */
#define IN_FASTRAM_BSS  /* thing */

#endif

#endif /*MFMMACROS_H_*/
