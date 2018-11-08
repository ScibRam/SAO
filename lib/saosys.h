/*******************************************************************************
**  saosys.h - system library for OS/platform dependent functions
**      Seismicity Analysis Organizer low level abstractions
**
**  The library allows SAO interact with files and OS environment
**  This header contains definitions of structures, constants and C-functions
**  described in "src/sys/" files as follow:
**    "saowfm.c" - IO functions for waveforms in SAC file format
*******************************************************************************/
#ifndef SAOSYS_H
#define SAOSYS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "saocore.h"



/*******************************************************************************
**  SAC header structure - 154 bytes
*/
typedef struct {
  float     delta,      depmin,     depmax,     scale,      odelta;
  float     b,          e,          o,          a,          internal1;
  float     t0,         t1,         t2,         t3,         t4;
  float     t5,         t6,         t7,         t8,         t9;
  float     f,          resp0,      resp1,      resp2,      resp3;
  float     resp4,      resp5,      resp6,      resp7,      resp8;
  float     resp9,      stla,       stlo,       stel,       stdp;
  float     evla,       evlo,       evel,       evdp,       unused1;
  float     user0,      user1,      user2,      user3,      user4;
  float     user5,      user6,      user7,      user8,      user9;
  float     dist,       az,         baz,        gcarc,      internal2;
  float     internal3,  depmen,     cmpaz,      cmpinc,     unused2;
  float     unused3,    unused4,    unused5,    unused6,    unused7;
  float     unused8,    unused9,    unused10,   unused11,   unused12;
  int32_t   nzyear,     nzjday,     nzhour,     nzmin,      nzsec;
  int32_t   nzmsec,     internal4,  internal5,  internal6,  npts;
  int32_t   internal7,  internal8,  unused13,   unused14,   unused15;
  int32_t   iftype,     idep,       iztype,     unused16,   iinst;
  int32_t   istreg,     ievreg,     ievtyp,     iqual,      isynth;
  int32_t   unused17,   unused18,   unused19,   unused20,   unused21;
  int32_t   unused22,   unused23,   unused24,   unused25,   unused26;
  int32_t   leven,      lpspol,     lovrok,     lcalda,     unused27;
  char      kstnm[8],               kevnm[16];
  char                  khole[8],               ko[8];
  char      ka[8],                  kt0[8],                 kt1[8];
  char                  kt2[8],                 kt3[8];
  char      kt4[8],                 kt5[8],                 kt6[8];
  char                  kt7[8],                 kt8[8];
  char      kt9[8],                 kf[8],                  kuser0[8];
  char                  kuser1[8],              kuser2[8];
  char      kcmpnm[8],              knetwk[8],              kdatrd[8];
  char                  kinst[8];
}  SacH;      // Total size: 512 bytes


/*******************************************************************************
**  SAC header for empty or invalid file
*/
static const SacH
UNDEFINED_SACH = {-12345.,  -12345.,  -12345.,  -12345.,  -12345.,
                  -12345.,  -12345.,  -12345.,  -12345.,  -12345.,
                  -12345.,  -12345.,  -12345.,  -12345.,  -12345.,
                  -12345.,  -12345.,  -12345.,  -12345.,  -12345.,
                  -12345.,  -12345.,  -12345.,  -12345.,  -12345.,
                  -12345.,  -12345.,  -12345.,  -12345.,  -12345.,
                  -12345.,  -12345.,  -12345.,  -12345.,  -12345.,
                  -12345.,  -12345.,  -12345.,  -12345.,  -12345.,
                  -12345.,  -12345.,  -12345.,  -12345.,  -12345.,
                  -12345.,  -12345.,  -12345.,  -12345.,  -12345.,
                  -12345.,  -12345.,  -12345.,  -12345.,  -12345.,
                  -12345.,  -12345.,  -12345.,  -12345.,  -12345.,
                  -12345.,  -12345.,  -12345.,  -12345.,  -12345.,
                  -12345.,  -12345.,  -12345.,  -12345.,  -12345.,
                  -12345,   -12345,   -12345,   -12345,   -12345,
                  -12345,   -12345,   -12345,   -12345,   -12345,
                  -12345,   -12345,   -12345,   -12345,   -12345,
                  -12345,   -12345,   -12345,   -12345,   -12345,
                  -12345,   -12345,   -12345,   -12345,   -12345,
                  -12345,   -12345,   -12345,   -12345,   -12345,
                  -12345,   -12345,   -12345,   -12345,   -12345,
                  -12345,   -12345,   -12345,   -12345,   -12345,
  { '-','1','2','3','4','5',' ',' ' },
  { '-','1','2','3','4','5',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' },
  { '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
  { '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
  { '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
  { '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
  { '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
  { '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
  { '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
  { '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
  { '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
  { '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
  { '-','1','2','3','4','5',' ',' ' }};


/*******************************************************************************
**    Support functions:
**  readSacH(..)      - read header from file-stream into SacH structure
**  getSacBegin(..)   - get begining Moment from SacH structure
**  writeSacInfo(..)  - get info from SacH as a string of specified format
*/
SacH
readSacH(FILE *fsac);

Moment
getSacBegin(SacH hdr);

char*
writeSacInfo (SacH hdr, int mode);
/******************************************************************************/
#endif /* SAOSYS_H */
