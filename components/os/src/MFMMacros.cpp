/*                                              -*- mode:C++; fill-column:100 -*-
  MFMMacros.cpp - Standalone early macros requiring no constants.
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

#include "MFMMacros.h"

const char * const faceNames[] = {
  "North",
  "South",
  "East",
  "West",
  "Spine",
  "WMem",
  "Brain"
};

#ifdef TEST_MFM_MACROS

// TO TEST: g++ -Wall -pedantic -Werror -I../../include -DTEST_MFM_MACROS MFMMacros.cpp;./a.out

#include "MFMConstants.h"

#include <stdio.h>
#include <stdlib.h> // for exit, random
#include <string.h> // for strcmp
void _apiError_(u32 code,const char * file, int lineno) { 
  fprintf(stderr,"APIERROR %d %s:%d\n",code,file,lineno);
  exit(code); 
}
void die(const char * file, int line, const char * msg) {
  fprintf(stderr,"%s:%d: %s\n",file, line, msg);
  exit(100);
}
#define TEST(condition) do { if (!(condition)) die(__FILE__,__LINE__,#condition); } while (0)

void testFACES() {
  TEST(CLOCKWISE_FACE(NORTH)==EAST);
  TEST(CLOCKWISE_FACE(EAST)==SOUTH);
  TEST(CLOCKWISE_FACE(SOUTH)==WEST);
  TEST(CLOCKWISE_FACE(WEST)==NORTH);

  TEST(COUNTERCLOCKWISE_FACE(NORTH)==WEST);
  TEST(COUNTERCLOCKWISE_FACE(WEST)==SOUTH);
  TEST(COUNTERCLOCKWISE_FACE(SOUTH)==EAST);
  TEST(COUNTERCLOCKWISE_FACE(EAST)==NORTH);

  TEST(ANTICLOCKWISE_FACE(NORTH)==WEST);
  TEST(ANTICLOCKWISE_FACE(WEST)==SOUTH);
  TEST(ANTICLOCKWISE_FACE(SOUTH)==EAST);
  TEST(ANTICLOCKWISE_FACE(EAST)==NORTH);

  TEST(OPPOSITE_FACE(NORTH)==SOUTH);
  TEST(OPPOSITE_FACE(EAST)==WEST);
  TEST(OPPOSITE_FACE(SOUTH)==NORTH);
  TEST(OPPOSITE_FACE(WEST)==EAST);

  TEST(0==strcmp(FACE_NAME(NORTH),"North"));
  TEST(0==strcmp(FACE_NAME(SOUTH),"South"));
  TEST(0==strcmp(FACE_NAME(EAST),"East"));
  TEST(0==strcmp(FACE_NAME(WEST),"West"));

  TEST(0==strcmp(FACE_NAME(SPINE),"Spine"));
  TEST(0==strcmp(FACE_NAME(WMEM),"WMem"));
  TEST(0==strcmp(FACE_NAME(BRAIN),"Brain"));
  
  TEST(FACE_CODE(NORTH)=='N');
  TEST(FACE_CODE(EAST)=='E');
  TEST(FACE_CODE(SOUTH)=='S');
  TEST(FACE_CODE(WEST)=='W');

  TEST(FACE_NUMBER_FROM_CODE('N')==NORTH);
  TEST(FACE_NUMBER_FROM_CODE('n')==NORTH);
  TEST(FACE_NUMBER_FROM_CODE('S')==SOUTH);
  TEST(FACE_NUMBER_FROM_CODE('s')==SOUTH);
  TEST(FACE_NUMBER_FROM_CODE('E')==EAST);
  TEST(FACE_NUMBER_FROM_CODE('e')==EAST);
  TEST(FACE_NUMBER_FROM_CODE('W')==WEST);
  TEST(FACE_NUMBER_FROM_CODE('w')==WEST);

  for (u32 i = 0; i < 256; ++i) {
    switch (i) {
    case 'N': case 'n': case 'S': case 's':
    case 'E': case 'e': case 'W': case 'w':
      TEST(IS_FACE_CODE(i));
      break;
    default:
      TEST(!IS_FACE_CODE(i));
    }
  }
}

void testBITS() {
  for (u32 i = 0; i < 32; ++i) {
    u32 b = 0;
    SET_BIT(b,i);
    TEST(b==(1u<<i));
  }

  for (u32 i = 0; i < 32; ++i) {
    u32 b = -1;
    CLEAR_BIT(b,i);
    TEST(b==~(1u<<i));
  }

  for (u32 j = 0; j < 10; ++j) {
    u32 v = random();
    for (u32 i = 0; i < 32; ++i) {
      TEST(GET_BIT(v,i)==((v>>i)&1));
      TEST(!!TEST_BIT(v,i)==!!GET_BIT(v,i));
    }
  }

  for (u32 i = 0; i < 32-1; ++i) {
    for (u32 j = 0; j < 4; ++j) {
      u32 b0 = 0, b1 = -1;
      BIT_PAIR_AT_POSITION(b0,j,i);
      BIT_PAIR_AT_POSITION(b1,j,i);
      TEST(b0==(j<<i));
      TEST(b1==(((-1)^(3<<i))|(j<<i)));
    }
  }
}

void testREGISTER_OFFSET() {
  const u32 SIZE = 15;
  u32 buf[SIZE];
  uptr base = (uptr) buf;
  for (u32 i = 0; i < SIZE; ++i) {
    buf[i] = 0x10101010*i;
  }
  for (u32 i = 0; i < SIZE; ++i) {
    for (u32 j = 0; j < SIZE; ++j) {
      if (i==j)
        TEST(buf[i]==REGISTER_OFFSET(base,j*4));
      else
        TEST(buf[i]!=REGISTER_OFFSET(base,j*4));
    }
  }
}

void testSCAMs() {
  for (u32 a = 0; a < 4; ++a)
    for (u32 b = 0; b < 4; ++b)
      for (u32 c = 0; c < 4; ++c)
        for (u32 d = 0; d < 4; ++d) {
          u32 m = MAP4BY2(a,b,c,d);
          TEST(GET_MAP4BY2(m,0)==a);
          TEST(GET_MAP4BY2(m,1)==b);
          TEST(GET_MAP4BY2(m,2)==c);
          TEST(GET_MAP4BY2(m,3)==d);
        }

  for (u32 a = 0; a < 4; ++a)
    for (u32 b = 0; b < 4; ++b)
      for (u32 c = 0; c < 4; ++c)
        for (u32 d = 0; d < 4; ++d) {
          u32 m = MAP4BY4(a*b,b*c,c*d,d*a);
          TEST(GET_MAP4BY4(m,0)==a*b);
          TEST(GET_MAP4BY4(m,1)==b*c);
          TEST(GET_MAP4BY4(m,2)==c*d);
          TEST(GET_MAP4BY4(m,3)==d*a);
        }

  for (u32 a = 0; a < 4; ++a)
    for (u32 b = 0; b < 4; ++b)
      for (u32 c = 0; c < 4; ++c)
        for (u32 d = 0; d < 4; ++d) {
          u32 m = MAP4BY8(a*b<<c,b*c<<d,c*d<<a,d*a<<b);
          TEST(GET_MAP4BY8(m,0)==a*b<<c);
          TEST(GET_MAP4BY8(m,1)==b*c<<d);
          TEST(GET_MAP4BY8(m,2)==c*d<<a);
          TEST(GET_MAP4BY8(m,3)==d*a<<b);
        }

  for (u32 a = 0; a < 7; ++a)
    for (u32 b = 0; b < 7; ++b)
      for (u32 c = 0; c < 7; ++c) {
        u32 m = MAP3BY3(a,b,c);
        TEST(GET_MAP3BY3(m,0)==a);
        TEST(GET_MAP3BY3(m,1)==b);
        TEST(GET_MAP3BY3(m,2)==c);

        u32 bm = BLINK_MAP(b,c,a);
        TEST(GET_BLINK_MAP_COLOR(bm,0)==b);
        TEST(GET_BLINK_MAP_COLOR(bm,1)==c);
        TEST(GET_BLINK_MAP_COLOR(bm,2)==a);
      }
}
void testIS_EARLIER() {
  TEST(IS_EARLIER(0,0)==false);
  TEST(IS_EARLIER(0,1)==true);
  TEST(IS_EARLIER(1,0)==false);
  TEST(IS_EARLIER(1,1)==false);

  TEST(IS_EARLIER_OR_EQUAL(0,0)==true);
  TEST(IS_EARLIER_OR_EQUAL(0,1)==true);
  TEST(IS_EARLIER_OR_EQUAL(1,0)==false);
  TEST(IS_EARLIER_OR_EQUAL(1,1)==true);

  TEST(IS_LATER(0,0)==false);
  TEST(IS_LATER(0,1)==false);
  TEST(IS_LATER(1,0)==true);
  TEST(IS_LATER(1,1)==false);

  TEST(IS_LATER_OR_EQUAL(0,0)==true);
  TEST(IS_LATER_OR_EQUAL(0,1)==false);
  TEST(IS_LATER_OR_EQUAL(1,0)==true);
  TEST(IS_LATER_OR_EQUAL(1,1)==true);

  const u32 TESTS = 10000;
  for (u32 i = 0; i < TESTS; ++i) {
    u32 v1 = random();
    u32 v2 = (i%(TESTS/10)==0)?v1:random();
    bool v1equalv2 = v1==v2;
    bool v1lessv2 = (v2-v1) < (v1-v2);
    bool v1gtrv2 = (v1-v2) < (v2-v1);

    TEST(IS_EARLIER(v1,v2)==v1lessv2);
    TEST(IS_LATER(v1,v2)==v1gtrv2);
    TEST(IS_EARLIER_OR_EQUAL(v1,v2)==v1lessv2 || v1equalv2);
    TEST(IS_LATER_OR_EQUAL(v1,v2)==v1gtrv2 || v1equalv2);
  }
}

void testIS_EARLIER16() {
  TEST(IS_EARLIER16(0,0)==false);
  TEST(IS_EARLIER16(0,1)==true);
  TEST(IS_EARLIER16(1,0)==false);
  TEST(IS_EARLIER16(1,1)==false);

  TEST(IS_EARLIER_OR_EQUAL16(0,0)==true);
  TEST(IS_EARLIER_OR_EQUAL16(0,1)==true);
  TEST(IS_EARLIER_OR_EQUAL16(1,0)==false);
  TEST(IS_EARLIER_OR_EQUAL16(1,1)==true);

  TEST(IS_LATER16(0,0)==false);
  TEST(IS_LATER16(0,1)==false);
  TEST(IS_LATER16(1,0)==true);
  TEST(IS_LATER16(1,1)==false);

  TEST(IS_LATER_OR_EQUAL16(0,0)==true);
  TEST(IS_LATER_OR_EQUAL16(0,1)==false);
  TEST(IS_LATER_OR_EQUAL16(1,0)==true);
  TEST(IS_LATER_OR_EQUAL16(1,1)==true);

  const u32 TESTS = 10000;
  for (u32 i = 0; i < TESTS; ++i) {
    u16 v1 = (u16) (random()&0xffff);
    u16 v2 = (i%(TESTS/10)==0)?v1:((u16) (random()&0xffff));
    bool v1equalv2 = v1==v2;
    bool v1lessv2 = ((u16)(v2-v1)) < ((u16)(v1-v2));
    bool v1gtrv2 = ((u16)(v1-v2)) < ((u16)(v2-v1));

    TEST(IS_EARLIER16(v1,v2)==v1lessv2);
    TEST(IS_LATER16(v1,v2)==v1gtrv2);
    TEST(IS_EARLIER_OR_EQUAL16(v1,v2)==v1lessv2 || v1equalv2);
    TEST(IS_LATER_OR_EQUAL16(v1,v2)==v1gtrv2 || v1equalv2);
  }
}

void testIS_EARLIER8() {
  TEST(IS_EARLIER8(0,0)==false);
  TEST(IS_EARLIER8(0,1)==true);
  TEST(IS_EARLIER8(1,0)==false);
  TEST(IS_EARLIER8(1,1)==false);

  TEST(IS_EARLIER_OR_EQUAL8(0,0)==true);
  TEST(IS_EARLIER_OR_EQUAL8(0,1)==true);
  TEST(IS_EARLIER_OR_EQUAL8(1,0)==false);
  TEST(IS_EARLIER_OR_EQUAL8(1,1)==true);

  TEST(IS_LATER8(0,0)==false);
  TEST(IS_LATER8(0,1)==false);
  TEST(IS_LATER8(1,0)==true);
  TEST(IS_LATER8(1,1)==false);

  TEST(IS_LATER_OR_EQUAL8(0,0)==true);
  TEST(IS_LATER_OR_EQUAL8(0,1)==false);
  TEST(IS_LATER_OR_EQUAL8(1,0)==true);
  TEST(IS_LATER_OR_EQUAL8(1,1)==true);

  const u32 TESTS = 10000;
  for (u32 i = 0; i < TESTS; ++i) {
    u8 v1 = (u8) (random()&0xff);
    u8 v2 = (i%(TESTS/10)==0)?v1:((u8) (random()&0xff));
    bool v1equalv2 = v1==v2;
    bool v1lessv2 = ((u8)(v2-v1)) < ((u8)(v1-v2));
    bool v1gtrv2 = ((u8)(v1-v2)) < ((u8)(v2-v1));

    TEST(IS_EARLIER8(v1,v2)==v1lessv2);
    TEST(IS_LATER8(v1,v2)==v1gtrv2);
    TEST(IS_EARLIER_OR_EQUAL8(v1,v2)==v1lessv2 || v1equalv2);
    TEST(IS_LATER_OR_EQUAL8(v1,v2)==v1gtrv2 || v1equalv2);
  }
}
struct testMISCStruct {
  u32 f1;
  u16 f2;
  u8 f3, f4;
};
void testMISC() {
  TEST(STRUCT_MEMBER_OFFSET(testMISCStruct,f1)==0);
  TEST(STRUCT_MEMBER_OFFSET(testMISCStruct,f2)==4);
  TEST(STRUCT_MEMBER_OFFSET(testMISCStruct,f3)==6);
  TEST(STRUCT_MEMBER_OFFSET(testMISCStruct,f4)==7);

  const u32 TESTS = 100;
  for (u32 i = 0; i < TESTS; ++i) {
    u32 v1 = random();
    u32 v2 = random();
    u32 min = MIN(v1,v2);
    u32 max = MAX(v1,v2);

    if (v1<=v2) 
      TEST(min==v1 && max==v2);
    else 
      TEST(min==v2 && max==v1);
  }

  for (u32 i = 0; i < TESTS; ++i) {
    u16 v1 = ((u16) random()&0xffff);
    u16 v2 = ((u16) random()&0xffff);
    u16 min = MIN(v1,v2);
    u16 max = MAX(v1,v2);

    if (v1<=v2) 
      TEST(min==v1 && max==v2);
    else 
      TEST(min==v2 && max==v1);
  }

  for (u32 i = 0; i < TESTS; ++i) {
    u8 v1 = ((u8) random()&0xff);
    u8 v2 = ((u8) random()&0xff);
    u8 min = MIN(v1,v2);
    u8 max = MAX(v1,v2);

    if (v1<=v2) 
      TEST(min==v1 && max==v2);
    else 
      TEST(min==v2 && max==v1);
  }

  for (u32 i = 0; i < TESTS; ++i) {
    s32 v1 = random();
    s32 v2 = random();
    s32 min = MIN(v1,v2);
    s32 max = MAX(v1,v2);

    if (v1<=v2) 
      TEST(min==v1 && max==v2);
    else 
      TEST(min==v2 && max==v1);
  }

  for (u32 i = 0; i < TESTS; ++i) {
    s16 v1 = ((s16) random()&0xffff);
    s16 v2 = ((s16) random()&0xffff);
    s16 min = MIN(v1,v2);
    s16 max = MAX(v1,v2);

    if (v1<=v2) 
      TEST(min==v1 && max==v2);
    else 
      TEST(min==v2 && max==v1);
  }

  for (u32 i = 0; i < TESTS; ++i) {
    s8 v1 = ((s8) random()&0xff);
    s8 v2 = ((s8) random()&0xff);
    s8 min = MIN(v1,v2);
    s8 max = MAX(v1,v2);

    if (v1<=v2) 
      TEST(min==v1 && max==v2);
    else 
      TEST(min==v2 && max==v1);
  }

  for (u32 i = 0; i < TESTS; ++i) {
    float v1 = random()/10000.0;
    float v2 = random()/10000.0;
    float min = MIN(v1,v2);
    float max = MAX(v1,v2);

    if (v1<=v2) 
      TEST(min==v1 && max==v2);
    else 
      TEST(min==v2 && max==v1);
  }

  for (u32 i = 0; i < TESTS; ++i) {
    double v1 = random()/10000.0;
    double v2 = random()/10000.0;
    double min = MIN(v1,v2);
    double max = MAX(v1,v2);

    if (v1<=v2) 
      TEST(min==v1 && max==v2);
    else 
      TEST(min==v2 && max==v1);
  }
}

u32 b36dig(u8 dig) {
  if (dig >= '0' && dig <= '9') return dig-'0';
  if (dig >= 'a' && dig <= 'z') return dig-'a'+10;
  if (dig >= 'A' && dig <= 'Z') return dig-'A'+10;
  TEST(dig!=dig);
  return 0; /* not reached */
}
u32 b36str(const char * str) {
  u32 val = 0;
  u8 ch;
  while ((ch = *str++)) {
    val = 36*val+b36dig(ch);
  }
  return val;
}
void testB36() {
  TEST(B36_1(0)==b36str("0"));
  TEST(B36_1(9)==b36str("9"));
  TEST(B36_1(a)==b36str("a"));
  TEST(B36_1(m)==b36str("m"));
  TEST(B36_1(z)==b36str("z"));
  TEST(B36_1(A)==b36str("a"));
  TEST(B36_1(M)==b36str("m"));
  TEST(B36_1(Z)==b36str("z"));

  TEST(B36_2(H,i)==b36str("hi"));

  TEST(B36_3(z,o,t)==b36str("zot"));

  TEST(B36_4(p,o,0,p)==b36str("po0p"));

  TEST(B36_5(F,U,Z,8,Y)==b36str("fuz8y"));

  TEST(B36_6(Z,i,k,0,z,j)==b36str("zik0zj"));
}
int main() {
  testFACES();
  testBITS();
  testREGISTER_OFFSET();
  testSCAMs();
  testIS_EARLIER();
  testIS_EARLIER16();
  testIS_EARLIER8();
  testMISC();
  testB36();
  return 0;
}

#endif
