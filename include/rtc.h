#ifndef RTC_H
#define RTC_H

/**
 @file rtc.h
 @brief Functions for interfacing with the RTC
*/

#include <stdint.h> // for uint8_t, etc.

typedef struct {
  uint8_t second;
  uint8_t minute;
  uint8_t hour;
  uint8_t day;
  uint8_t month;
  uint8_t year; // last two digits
} rtc_time_t;

/**
 Gets the current time.
 @param t rtc_time struct to store time data into
 */
void get_rtc(rtc_time_t *t);

/**
 Sets the current time.
 @param day current day of the month
 @param month current month of the year
 @param year current year 
 @param hour current hour of the day, military time
 @param minute current minute of the hour
 @param second current second of the minute
 */
void set_rtc(uint8_t day, uint8_t month, uint8_t year, 
             uint8_t hour, uint8_t minute, uint8_t second);

/**
 Gets whether or not a calendar year is a leap year.
 @param year year to evaluate
 @return 1 if is leap year, 0 otherwise
 */
int is_leap_year(uint8_t year);

/**
 Gets the length in days of a month, taking into account leap years.
 @param month month to evaluate
 @param year year of said month, for purposes of leap years. Doesn't matter if month isn't February.
 @return length of month in days
 */
int month_length(uint8_t month, uint8_t year);

#endif 
