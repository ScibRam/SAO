/*******************************************************************************
**  sacinfo.c - SAC file(s) Information Tool
**      Part of Seismicity Analysis Organizer
**
**  This program is heavily based on 'saowfm.c' (part of SAO system library)
**  Program info and usage described in programInfo(..) functions
**  The <unistd.h> is required for getopt(..) function
**  With MinGW runtime it is possible to compile it under Windows OS
*******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../../lib/saocore.h"
#include "../../lib/saosys.h"



/*******************************************************************************
**    Print program info and usage
**      IN: 0 - program usage and info, 1 - full list of options
*/
void
programInfo (int full)
{
  char *fullinfo =
  "SAC file(s) Information Tool.\n"
  "Options:\n"
  "  no options     show info about FILE(S)\n"
  "  -s             print main info to one line for (each) FILE\n"
//  "  -c             check data to header conformity\n"
//  "  -f             show full info with description\n\n"
  "Seismicity Analysis Organizer <https://github.com/ScibRam/SAO>.\n";

  printf("Usage: sacinfo [OPTION] FILE...\n");
  if (full == 1) printf("%s\n", fullinfo);
  else printf("Try 'sacinfo -h' for full list of options and examples\n");
  return ;
}
/******************************************************************************/



/*********************************************************************************    Main function - detecting options and defining program and output modes
**  Program modes (int mode):
**      0       no options    - print info about file(s)
**      115     (-s) option   - print main info in line with commas
*/
int main (int argc, char *argv[])
{
  char *options = "hs";    int   opt;
  int   optdone = 0;        int   mode = 0;
  FILE *fsac;               SacH  hdr;
  char* buf;

  while (optdone != 1) {
    if ((opt = getopt(argc, argv, options)) != -1) {
      switch (opt) {
        case 'h':
          programInfo(1);
          exit(0);
        case 's':
          mode = 's';
          break;
        default:
          optdone = 1;
          break;
      }
    }
    else optdone = 1;
  }
  //printf("Arguments count: %d, optind = %d\n", argc, optind);
  if (optind < argc) {
    while (optind < argc) {
      if ((fsac = fopen(argv[optind], "rb")))
      {
        hdr = readSacH(fsac);
        if (hdr.internal4 != 6)
          printf("%s - incorrect SAC file\n", argv[optind]);
        else {
          buf = writeSacInfo(hdr, mode);
          if (mode == 's')
            printf("%s,%s\n", argv[optind], buf);
          else
            printf("File '%s' is a valid SAC file\n%s\n", argv[optind], buf);
        }
        optind++;
        fclose(fsac);
      }
    }
  }
  else programInfo(0);
  return 0;
}
/******************************************************************************/
