/*******************************************************************************
**  saotime.c - functions for time calculations based on Moment structure type
**      Part of SAO core library -> see "lib/saocore.h"
**
**    Core functions:
**  fromEpoch(..)       - convert epoch time into a Moment structure
**  toEpoch(..)         - calculate epoch time from a Moment structure
**  addSecs(..)         - add or substract seconds to/from a Moment
**  difDays(..)         - calculate difference in days between two Moments
**  difSecs(..)         - calculate difference in seconds between two Moments
**  readMoment(..)      - read Moment from a string of supported format
**  writeMoment(..)     - write Moment to a string of specified format
**  printMoment(..)     - print Moment to standard output
**
*******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../../lib/saocore.h"



/*********************************************************************************    Convert epoch time into a Moment structure
**      OUT: New Moment structure
**      IN:  Epoch time (seconds since EPOCH_0)
**  The main idea here is to first split a big number on date and time parts
**  A: Calc number of full days between EPOCH_0 and the moment
**  B: Sign if the moment before (-1) or after (+1) EPOCH_0
**  C: Calc residual seconds since beginnig of moment day and moment itself
**  D: Calc date part of the moment by consecutive subtraction from 'days'
**  E: Checking the number of days in a year to substract
**  F: Usign a sign to add or substract a year and remove 'ndays' from 'days'
**  G: In case 'days' ends to be negative (epoch<0), repeat F-G once more
**  H: Getting the date part of the moment by using support function
**  I: Simple subtractions to get time part of the moment step by step
**  J: Little trick with multiplication for proper milliseconds calculation
*/
Moment
fromEpoch (double epoch)
{
  Moment t = EPOCH_0;   long ndays = 365;

  long days = (long)(epoch / 86400);                                      // A
  short sg = 1;  if (epoch < 0.0f) sg = -1;                               // B
  double rs = epoch - (double)(days * 86400);                             // C
  if (rs < 0.0f) { rs += 86400.0;  days += sg; }                          // C
  while (abs(days) >= ndays) {                                            // D
    if ((sg==1 && t.year % 4 == 0) || (sg==-1 && (t.year - 1) % 4 == 0))  // E
      ndays = 366; else ndays = 365;                                      // E
    days -= sg * ndays;  t.year += sg;                                    // F
  }
  if (days < 0) {                                                         // G
    if ((t.year - 1) % 4 == 0) ndays = 366; else ndays = 365;             // G
    days -= sg * ndays; t.year += sg;                                     // G
  }
  t.yday = days + 1;    getMonthDay(&t.month, &t.day, t.year, t.yday);    // H
  t.hour = (int)(rs / 3600.0);                                            // I
  t.min  = (int)((rs - t.hour * 3600.0) / 60.0);                          // I
  t.sec  = (int)(rs - t.hour * 3600.0 - t.min * 60.0);                    // I
  t.msec = (int)(1000.1 * (rs - 3600.0 * t.hour - 60.0 * t.min - t.sec)); // J
  return t;
}
/******************************************************************************/



/*********************************************************************************    Calculate epoch time from a Moment structure
**      OUT: Epoch time in seconds since EPOCH_0
**      IN:  Moment structure
**  In inverse function we again using substraction along with a sign tricks
**  A: Defining number of years between the moment and EPOCH_0
**  B: Best way to count number of leap years
**  C: Check to avoid division by zero
**  D: Calc number of days in 'years'
**  E: Calc total number of days to the moment day
**  F: Multiply by 86400 to define epoch time
**  G: Finally add time part to return epoch time
*/
double
toEpoch (Moment t)
{
  short year;   long days;    int nleaps = 0;

  int years = t.year - EPOCH_0.year;                                      // A
  if (years < 0) { for (year = EPOCH_0.year; year >= t.year; year--)      // B
    if (year % 4 == 0) nleaps++;  }                                       // B
  else { for (year = EPOCH_0.year; year <= t.year; year++)                // B
    if (year % 4 == 0) nleaps++; }                                        // B

  if (years == 0) days = 0;                                               // C
  else days = years * 365 + (years/abs(years)) * nleaps;                  // D
  days +=  (t.yday - 1) - (EPOCH_0.yday - 1);                             // E
  double epoch = (double)(days * 86400.0);                                // F
  epoch += (double)(t.hour * 3600 + t.min * 60 + t.sec);                  // G
  return epoch + (double)t.msec / 1000.0;
}
/******************************************************************************/



/*******************************************************************************
**    Add or substract seconds to/from a Moment
**      OUT: New Moment structure
**      IN1: Moment structure
**      IN2: Number of seconds to add (if positive) or substrack (if negative)
*/
Moment
addSecs (Moment t, double ds)
{
  double epoch = toEpoch(t);
  return fromEpoch(epoch + ds);
}
/******************************************************************************/



/*******************************************************************************
**    Calculate difference in seconds between two Moments
**      OUT: Amount of seconds between two Moments
**      INs: Moments to compare
**  Returns positive value if t2 moment later than t1 and negative vice versa
*/
double
difSecs (Moment t2, Moment t1)
{
  return toEpoch(t2) - toEpoch(t1);
}
/******************************************************************************/



/*******************************************************************************
**    Calculate difference in days between two Moments
**      OUT: Number of days between two moments
**      INs: Moments to compare
**  Returns positive value if t2 contains in calendar day later than t1
**  Negative value vice versa and zero if t1 and t2 are in the same day
*/
long
difDays (Moment t2, Moment t1)
{
  return (long)(toEpoch(t2) / 86400) - (long)(toEpoch(t1) / 86400);
}
/******************************************************************************/



/*******************************************************************************
**    Read Moment from a string of supported format
**      OUT: New Moment struct
**      IN:  String of supported format (see in saocore.h)
*/
Moment
readMoment (const char *buf)
{
  Moment t = EPOCH_0;
  char tmp2[3], tmp3[4], tmp4[5];
  int len = strlen(buf);
  int shift = 0;
  tmp2[2] = '\0'; tmp3[3] = '\0'; tmp4[4] = '\0';

  if (len > 4 &&
  (buf[0] >= 0x30 && buf[0] <= 0x39) &&                 // 0-9  year
  (buf[1] >= 0x30 && buf[1] <= 0x39) &&                 // 0-9  year
  (buf[2] >= 0x30 && buf[2] <= 0x39) &&                 // 0-9  year
  (buf[3] >= 0x30 && buf[3] <= 0x39)) {                 // 0-9  year
    t.year  = atoi(memcpy(tmp4, buf + 0, 4));
  }
  else goto wrongfmt;
                                                        // buf[4] - separator
  if (len > 7 &&
  (buf[5] >= 0x30 && buf[5] <= 0x33) &&                 // 0-3  day of year
  (buf[6] >= 0x30 && buf[6] <= 0x39) &&                 // 0-9  day of year
  (buf[7] >= 0x30 && buf[5] <= 0x39)) {                 // 0-9  day of year
    t.yday = atoi(memcpy(tmp3, buf + 5, 3));
    getMonthDay(&t.month, &t.day, t.year, t.yday);
  }
  else if (len > 9 &&
  (buf[5] >= 0x30 && buf[5] <= 0x31) &&                 // 0-1  month
  (buf[6] >= 0x30 && buf[6] <= 0x39) &&                 // 0-9  month
                                                        // buf[7] - separator
  (buf[8] >= 0x30 && buf[5] <= 0x33) &&                 // 0-3  day
  (buf[9] >= 0x30 && buf[6] <= 0x39)) {                 // 0-9  day
    t.month = atoi(memcpy(tmp2, buf + 5, 2));
    t.day   = atoi(memcpy(tmp2, buf + 8, 2));
    t.yday  = getYday(t.year, t.month, t.day);
    shift += 2;
  }
  else goto wrongfmt;

  if (len == 8+shift) goto strdone;
            // else means buf[8+shift] - separator ('_' or 'T', usually)

  if (len > 14+shift &&
  (buf[ 9+shift] >= 0x30 && buf[ 9+shift] <= 0x32) &&   // 0-2  hour
  (buf[10+shift] >= 0x30 && buf[10+shift] <= 0x39) &&   // 0-9  hour
  (buf[11+shift] >= 0x30 && buf[11+shift] <= 0x35) &&   // 0-5  min
  (buf[12+shift] >= 0x30 && buf[12+shift] <= 0x39) &&   // 0-9  min
  (buf[13+shift] >= 0x30 && buf[13+shift] <= 0x35) &&   // 0-5  sec
  (buf[14+shift] >= 0x30 && buf[14+shift] <= 0x39)) {   // 0-9  sec
    t.hour = atoi(memcpy(tmp2, buf +  9 + shift, 2));
    t.min  = atoi(memcpy(tmp2, buf + 11 + shift, 2));
    t.sec  = atoi(memcpy(tmp2, buf + 13 + shift, 2));
  }
  else if (len > 16+shift &&
  (buf[ 9+shift] >= 0x30 && buf[ 9+shift] <= 0x32) &&   // 0-2  hour
  (buf[10+shift] >= 0x30 && buf[10+shift] <= 0x39) &&   // 0-9  hour
                                                        // buf[11+shift] - sep
  (buf[12+shift] >= 0x30 && buf[12+shift] <= 0x35) &&   // 0-5  min
  (buf[13+shift] >= 0x30 && buf[13+shift] <= 0x39) &&   // 0-9  min
                                                        // buf[14+shift] - sep
  (buf[15+shift] >= 0x30 && buf[15+shift] <= 0x35) &&   // 0-5  sec
  (buf[16+shift] >= 0x30 && buf[16+shift] <= 0x39)) {   // 0-9  sec
    t.hour = atoi(memcpy(tmp2, buf +  9 + shift, 2));
    t.min  = atoi(memcpy(tmp2, buf + 12 + shift, 2));
    t.sec  = atoi(memcpy(tmp2, buf + 15 + shift, 2));
    shift += 2;
  }
  else goto wrongfmt;
                                                        // buf[15+shift] - sep
  if (len == 19+shift &&
  (buf[16+shift] >= 0x30 && buf[16+shift] <= 0x39) &&   // 0-9  msec
  (buf[17+shift] >= 0x30 && buf[17+shift] <= 0x39) &&   // 0-9  msec
  (buf[18+shift] >= 0x30 && buf[18+shift] <= 0x39)) {   // 0-9  msec
    t.msec = atoi(memcpy(tmp3, buf + 16 + shift, 3));
  }
strdone:
  if (isMoment(t) == 1)
    return t;

wrongfmt:
  return NOT_MOMENT;
}
/******************************************************************************/



/*******************************************************************************
**    Write Moment to a string of specified format
**      OUT: Pointer to the new string
**      IN1: Moment to write
**      IN2: Format of string
*/
char*
writeMoment (Moment t, const char *format)
{
  char *buf;  int len = 0;
  if (isMoment(t) == 0) return NULL;
  if (strcmp(format, "ORD") == 0) len = 8;
  if (strcmp(format, "STD") == 0) len = 10;
  if (strcmp(format, "SAC") == 0) len = 15;
  if (strcmp(format, "SAO") == 0) len = 17;
  if (strcmp(format, "ISO") == 0) len = 23;

  buf = (char*) malloc((len + 1) * sizeof(char));
  switch (len) {
    case 8:
      sprintf(buf, "%04d-%03d", t.year, t.yday);
      break;
    case 10:
      sprintf(buf, "%04d-%02d-%02d", t.year, t.month, t.day);
      break;
    case 15:
      sprintf(buf, "%04d-%03d_%02d%02d%02d",
                    t.year, t.yday, t.hour, t.min, t.sec);
      break;
    case 17:
      sprintf(buf, "%04d-%02d-%02d_%02d%02d%02d",
                    t.year, t.month, t.day, t.hour, t.min, t.sec);
      break;
    case 23:
      sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d.%03d",
                    t.year, t.month, t.day, t.hour, t.min, t.sec, t.msec);
      break;
    default:
      fprintf(stderr, "Not supported format for writing the moment.\n");
      break;
  }
  buf[len] = '\0';
  return buf;
}
/******************************************************************************/



/*******************************************************************************
**    Print Moment to standard output
**      IN:  Moment structure to print
*/
void
printMoment (Moment t)
{
  fprintf(stdout,"\t%d year, %d month, %d day\n", t.year, t.month, t.day);
  fprintf(stdout,"\t%d hour, %d minute, %d second\n", t.hour, t.min, t.sec);
  fprintf(stdout,"\t%d milliseconds", t.msec);
  fprintf(stdout,"\t%d day in the year\n", t.yday);
  fprintf(stderr, "\tThe structure at %p adress ", &t);
  if (isMoment(t) == 1)
    fprintf(stderr, "is a correct Moment.\n");
  else
    fprintf(stderr, "is INCORRECT.\n");
}
/******************************************************************************/



/*******************************************************************************
**  Support functions:
**  isDate(..)      - check that three numbers represent a correct date
**  isTime(..)      - check that three numbers represent a correct time moment
**  getMonthDay(..) - determine month and day from the day in a year
**  getYday(..)     - determine the day in a year from month and day
**  isMoment(..)    - check if the Moment structure is correct
*/

/*  Support constants - days before month's begin in a normal/leap year  */
const short
DAYS_365[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
const short
DAYS_366[] = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 };


/*******************************************************************************
**    Check that three numbers represent a correct dat
**      OUT: 1 - true, 0 - false
**      INs: Year, Month, Day
**  January, March, May, July, August, October and Decemeber have 31 days
**  April, June, September, November - 30 days, Febrary depends of a leap year
*/
inline int
isDate (short y, short m, short d)
{
  short const* days;
  if (y > 0 && y % 4 == 0) days = DAYS_366; else days = DAYS_365;
  if (m >= 1 && m <= 12)
    if (d >= 1 && d <= days[m] - days[m-1])
      return 1;
    else return 0;
  else return 0;
}
/******************************************************************************/



/*******************************************************************************
**    Check that three numbers represent a correct time moment
**      OUT: 1 - true, 0 - false
**      INs: Hour, Minute, Second
**  Only 24 hour per day (for regular person)
**  60 minuts per hour and 60 seconds per minute
*/
inline int
isTime (short h, short m, short s)
{
  if ((h >= 0 && h <= 23) && (m >= 0 && m <= 59) && (s >= 0 && s <= 59))
    return 1;
  else return 0;
}
/******************************************************************************/



/*******************************************************************************
**    Determine month and day from the day in a year
**      OUT: Two pointers (1) Month number and (2) Day number
**      IN1: Year number (bcs of the leap years)
**      IN2: Year day
*/
inline void
getMonthDay (short* month, short* day, short year, short yday)
{
  short const *days;   short m;
  if ( yday < 1 || yday > 366)
    fprintf(stderr, "Incorect date. yday == [%d]\n", yday);
  if (year % 4 == 0) days = DAYS_366; else days = DAYS_365;
  for (m = 0; m < 12; m++)
    if (yday >= days[m] + 1 && yday <=days[m+1])
      { *day = yday - days[m]; break; }
  *month = m + 1;
  return ;
}
/******************************************************************************/



/*******************************************************************************
**    Determine the day in a year from month and day
**      OUT: Number of the day in a year
**      INs: Year, Month and Day
*/
inline short
getYday (short year, short month, short day)
{
  short const *days;   short yday = 0;
  if (isDate(year, month, day) == 0)
    fprintf(stderr, "Incorect date. |%d-%d-%d|\n", year, month, day);
  if (year % 4 == 0) days = DAYS_366; else days = DAYS_365;
  yday = day + days[month-1];
  return yday;
}
/******************************************************************************/



/*********************************************************************************    Check if the Moment structure is correct
**      OUT: 1 - true, 0 - false
**      IN:  Moment structure to check
*/
inline int
isMoment  (Moment t)
{
  if ((isDate(t.year, t.month, t.day) == 1)       &&
      (getYday(t.year, t.month, t.day) == t.yday) &&
      (isTime(t.hour, t.min, t.sec) == 1)         &&
      (t.msec >= 0 && t.msec <= 999))
    return 1;
  else return 0;
}
/******************************************************************************/
