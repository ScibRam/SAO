/*******************************************************************************
**  saocore.h - core library concepts and definitions
**      Seismicity Analysis Organizer high level abstractions
**
**  This library operates in RAM with several basic concepts
**  Each concept is presented by a C-structure type in this header file
**  Sets of C-functions described in "src/core/" files operates these types:
**    "saotime.c" - time calculation functions for Moment concept
**    "saodata.c" - data management functions for Trace concept
*******************************************************************************/
#ifndef SAOCORE_H
#define SAOCORE_H



/*******************************************************************************
**    <Moment> concept - describes specific moment in time.
**  Moment structure contain fields for human-readable calendar date:
**  year, month, day, hour, minute, second. Yet in machine time moment usually
**  represents as a number of seconds since some moment called "epoch".
**  Epoch time (so-called Unix time) is much simpler for computations as it's
**  only one number. However, understanding of this huge value is tricky for
**  the user.
*/
typedef struct Moment {
  short     year, month;        // Year and month
  short     day,  yday;         // Day in the month and day in the year
  short     hour, min;          // Hour and minute
  short     sec,  msec;         // Second and millisecond
}  Moment;                      // Total size: 16 bytes

static const Moment
EPOCH_0 = {1970, 1, 1, 1, 0, 0, 0, 0};

static const Moment
NOT_MOMENT = {0, 0, 0, 0, 0, 0, 0, 0};


/*******************************************************************************
**    For reading and writing here is a table of supported string formats:
**  Key    |          Format          |       Description
**  "ORD"  | YYYY-DDD                 | ordinal date
**  "STD"  | YYYY-MM-DD               | standart date format
**  "SAC"  | YYYY-DDD_hhmmss          | usual SAC convertion format
**  "SAO"  | YYYY-MM-DD_hhmmss        | default SAO date and time string
**  "ISO"  | YYYY-MM-DDThh:mm:ss.sss  | ISO-style string
**    Char   000000000011111111112222
**    pos -> 012345678901234567890123
**
**  Date means it have time equal 00:00:00 (begining of the day)
**  Unsupported string will return NULL instead of Moment structure.
**    Examples of supported strings:
**      2013-239
**      2013-08-28
**      2013-239_214918
**      2013-08-28_214918
**      2013-08-28T21:49:18.990
*******************************************************************************/


/*******************************************************************************
**    Core functions for working with the Moment concept - "saotime.c"
**  fromEpoch(..)       - convert epoch time into a Moment structure
**  toEpoch(..)         - calculate epoch time from a Moment structure
**  addSecs(..)         - add or substract seconds to/from a Moment
**  difDays(..)         - calculate difference in days between two Moments
**  difSecs(..)         - calculate difference in seconds between two Moments
**  readMoment(..)      - read Moment from a string of supported format
**  writeMoment(..)     - write Moment to a string of specified format
**  printMoment(..)     - print Moment to standard output
*/
Moment
fromEpoch (double epoch);

double
toEpoch (Moment t);

Moment
addSecs (Moment t, double ds);

double
difSecs (Moment t2, Moment t1);

long
difDays (Moment t2, Moment t1);

Moment
readMoment (const char *buf);

char*
writeMoment (Moment t, const char* format);

void
printMoment (Moment t);
/******************************************************************************/


/*******************************************************************************
**  Support functions:
**  isDate(..)      - check that three numbers represent a correct date
**  isTime(..)      - check that three numbers represent a correct time moment
**  getMonthDay(..) - determine month and day from the day in a year
**  getYday(..)     - determine the day in a year from month and day
**  isMoment(..)    - check if the Moment structure is correct
*/
extern int    isDate (short y, short m, short d);
extern int    isTime (short h, short m, short s);
extern short  getYday (short year, short month, short day);
extern void   getMonthDay (short* month, short* day, short year, short yday);
extern int    isMoment (Moment t);
/******************************************************************************/

/******************************************************************************/
#endif /* SAOCORE_H */
