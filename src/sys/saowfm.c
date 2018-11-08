/******************************************************************************
**  saowfm.c - functions for reading/writing *.sac files
**              part of SAO system library -> see "lib/saosys.h"
**
**    Core functions:
**  readSacH(..)    - read header from SAC file into SacH structure
**  getSacBegin(..) - get begining Moment from SacH structure
**  writeSacInfo(..)  - get info from SacH as a string of specified format
**
*******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../../lib/saocore.h"
#include "../../lib/saosys.h"



/*******************************************************************************
**    Read header from SAC file into SacH structure
**      OUT: New SacH structure
**      IN:  String with path to SAC file
*/
SacH
readSacH(FILE *fsac)
{
  SacH hdr = UNDEFINED_SACH;
  if (fsac != NULL)
    fread(&hdr, sizeof(SacH), 1, fsac);
  return hdr;
}
/******************************************************************************/



/*******************************************************************************
**    Get begining Moment from SacH structure
**      OUT: New Moment structure
**      IN:  Pointer to SacH structure
**  SAC files have
*/
Moment
getSacBegin(SacH hdr)
{
  char* buf;  int len = 19;
  buf = (char*) malloc((len + 1) * sizeof(char));
  sprintf(buf, "%04d-%03d_%02d%02d%02d.%03d",
          hdr.nzyear, hdr.nzjday, hdr.nzhour,
          hdr.nzmin, hdr.nzsec, hdr.nzmsec);
  Moment t = readMoment(buf);
  t = addSecs(t, hdr.b);
  return t;
}
/******************************************************************************/



/*******************************************************************************
**    Write info about SAC file in specific mode
**      IN1: Path to file
**      IN2: Mode of writing info
**            0         - default, few lines of general info
**            115 ('s') - short, one line with main info
**            102 ('f') - full, long table with all header info
*/
char*
writeSacInfo (SacH hdr, int mode)
{
  char *buf;
  int len = 255;

  buf = (char*) malloc((len + 1) * sizeof(char));
  if (mode == 's'){
    sprintf(buf, "%s,%d,%f,%s,%s,%s", writeMoment(getSacBegin(hdr), "ISO"),
            hdr.npts, hdr.delta, hdr.knetwk, hdr.kstnm, hdr.kcmpnm);
  }
  else if (mode == 'f'){

  }
  else {
    Moment b = getSacBegin(hdr);
    Moment e = addSecs(b, (hdr.npts * hdr.delta));
    sprintf(buf, "Station |%s| of |%s| network\nLocated at (%f,%f,%f)\nChannel |%s| sampling frequency: %f\nData for period: %s - %s\n", hdr.kstnm, hdr.knetwk, hdr.stla, hdr.stlo, hdr.stel, hdr.kcmpnm, 1/hdr.delta, writeMoment(b, "SAO"), writeMoment(e, "SAO"));
  }
  return buf;
}
/******************************************************************************/
