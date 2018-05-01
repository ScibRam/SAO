/*******************************************************************************
**  utc.c - Coordinated Universal Time (UTC) Convertion and Calculation Tool
**      Part of Seismicity Analysis Organizer
**
**  Program info and usage described in programInfo(..) functions
**  The unistd.h is required for getopt(..) function
**  With MinGW runtime it is possible to compile it under Windows OS
*******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../../lib/saocore.h"



/*******************************************************************************
**    Print program info and usage
**      IN: 0 - program usage and info, 1 - full list of options
*/
void
programInfo (int full)
{
  char *fullinfo =
  "Coordinated Universal Time (UTC) Convertion and Calculation Tool.\n"
  "Convert MOMENT string to epoch time and vice versa.\n"
  "Supported formats of string are listed in a table below.\n"
  "Epoch time is a commonly used machine representation of date&time.\n"
  "It is simply a number of seconds since 00:00:00 Jan 1, 1970\n\n"
  "Options:\n"
  "  no options     convert MOMENT string to epoch time in seconds\n"
  "  -d             output in days instead of seconds\n"
  "  -e             convert MOMENT epoch time into string\n"
  "  -a=DELTA       add/subtract DELTA seconds to/from MOMENT\n"
  "  -b=REF         calc difference between REF and MOMENT in seconds\n"
  "  -o=FORMAT      format of output string, default: SAO\n"
  "  -h             display this help and exit\n\n"
  "Formats:\n"
  "  ORD  | YYYY-DDD                 | ordinal date\n"
  "  STD  | YYYY-MM-DD               | standart date format\n"
  "  SAC  | YYYY-DDD_hhmmss          | usual SAC convertion format\n"
  "  SAO  | YYYY-MM-DD_hhmmss        | default SAO format\n"
  "  ISO  | YYYY-MM-DDThh:mm:ss.sss  | ISO-style string\n\n"
  "Examples:\n\n"
  "1) Calculate epoch time for some time moment\n"
  "  $ utc 1991-08-28_234059\n"
  "  > 683422859.000\n\n"
  "2) Get the moment of 1.5 billion seconds since start of UNIX epoch\n"
  "  $ utc -e 1500000000\n"
  "  > 2017-07-14_024000\n\n"
  "3) Add/subtract one hour to/from moment and return fancy string\n"
  "  $ utc -a 3600 -o ISO 2018-04-12_093000\n"
  "  > 2018-04-12T10:30:00.000\n"
  "  $ utc -a -3600 -o ISO 2018-04-12_093000\n"
  "  > 2018-04-12T08:30:00.000\n\n"
  "4) Calculate difference between two dates in seconds/dates\n"
  "  $ utc -b 1960-03-29 1991-08-28\n"
  "  > 991353600.000\n"
  "  $ utc -db 1991-08-28 2019-01-13\n"
  "  > 10000\n\n"
  "5) Convert standart date to ordinary one and vice versa\n"
  "  $ utc -o ORD 1991-08-28\n"
  "  > 1991-240\n"
  "  $ utc -o SAO 2013-239_072815\n"
  "  > 2013-08-27_072815\n\n"
  "6) Complicated case\n"
  "  $ utc -e -a 86400 -b 2013-240_000000 1377588495.999\n"
  "  > 26895.999\n\n"
  "Seismicity Analysis Organizer <https://github.com/ScibRam/SAO>.\n";

  printf("Usage: utc [OPTION]... MOMENT\n");
  if (full == 1) printf("%s", fullinfo);
  else printf("Try './utc -h' for full list of options and examples\n");
  return ;
}
/******************************************************************************/



/*******************************************************************************
**    Main function - detecting options and defining program and output modes
**  Program modes (int mode):
**      0       default, no options, input - string of supported format
**      1       (-e) option, input - epoch time
**     1?       (-a) option, adding mode (optarg - delta to add/subtract)
**     t1       (-b) option, difference mode (optarg - reference t1)
**  Output regimes (char format):
**      ""      default, seconds for 0 mode
**      "SAO"   (-e) option, default format if (-o) not specified
**      "***"   (-o) option, one of supported formats
**      "DBD"   (-d) option, days instead of seconds (for difference mode)
**                           days from EPOCH_0 (for convertion/adding modes)
**  First it process all options and its arguments by getopt(..) function
**  After program 'mode' and output 'format' decided it reads input MOMENT 't'
**  If adding mode is on, then first add 'delta' to 't'
**  Then if difference mode is on calculate how much time passed since 't1'
**  Finally return output in prefered 'format' (first detected from options)
*/
int main (int argc, char *argv[])
{
  Moment t1 = NOT_MOMENT;                   Moment t;
  char *options = "hedb:a:o:1234567890.";   int opt;
  int optdone = 0;                          int mode = 0;
  double delta = 0.0;                       char format[32] = "";

  while (optdone != 1) {
    if ((opt = getopt(argc, argv, options)) != -1) {
      switch (opt) {
        case 'h':
          programInfo(1);
          exit(0);
        case 'e':
          mode = 1;
          if (strcmp(format,"") == 0) strcpy(format, "SAO");
          break;
        case 'd':
          if (strcmp(format,"") == 0) strcpy(format, "DBD");
          break;
        case 'b':
          t1 = readMoment(optarg);
          break;
        case 'a':
          mode += 10;
          delta = atof(optarg);
          break;
        case 'o':
          strcpy(format, optarg);
          break;
        default:
          optdone = 1;
          break;
      }
    }
    else optdone = 1;
  }

  if (optind < argc) {
    if (mode % 2 == 0) t = readMoment(argv[optind]);
    else  t = fromEpoch(atof(argv[optind]));

    if (mode / 10 == 1)  t = addSecs(t, delta);

    if (isMoment(t1) == 1) {
      delta = difSecs(t, t1);
      if (strcmp(format,"DBD") == 0)
        fprintf(stdout, "%.d\n", (int)(delta / 86400));
      else fprintf(stdout, "%.3f\n", delta);
    }
    else {
      if (strcmp(format,"") == 0) fprintf(stdout, "%.3f\n", toEpoch(t));
      else if (strcmp(format,"DBD") == 0)
        fprintf(stdout, "%.d\n", (int)(toEpoch(t) / 86400));
      else fprintf(stdout, "%s\n", writeMoment(t, format));
    }
  }
  else programInfo(0);
  return 0;
}
/******************************************************************************/
