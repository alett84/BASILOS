#include <rtc.h>
#include <stdint.h>
#include <stdbool.h>
#include <mpx/io.h>  // or wherever outb/inb are defined

// Some RTC registers we typically care about
#define CMOS_ADDRESS  0x70
#define CMOS_DATA     0x71

#define REG_SECONDS   0x00
#define REG_MINUTES   0x02
#define REG_HOURS     0x04
#define REG_DAY       0x07
#define REG_MONTH     0x08
#define REG_YEAR      0x09

// For example, if your hardware stores century in register 0x32, you can define that:
#define REG_CENTURY   0x32  // This can vary by motherboard/BIOS

static uint8_t read_rtc_register(uint8_t reg);
static void write_rtc_register(uint8_t reg, uint8_t value);
static uint8_t bcd_to_bin(uint8_t val);
static uint8_t bin_to_bcd(uint8_t val);

typedef enum {
  JAN = 1u,
  FEB = 2u,
  MAR = 3u,
  APR = 4u,
  MAY = 5u,
  JUN = 6u,
  JUL = 7u,
  AUG = 8u,
  SEP = 9u,
  OCT = 10u,
  NOV = 11u,
  DEC = 12u
} month;

///////////////////////////////////////////////////////////
// Public Functions
///////////////////////////////////////////////////////////

void get_rtc(rtc_time_t *t) {
  // Read raw values from the RTC
  uint8_t raw_seconds = read_rtc_register(REG_SECONDS);
  uint8_t raw_minutes = read_rtc_register(REG_MINUTES);
  uint8_t raw_hours   = read_rtc_register(REG_HOURS);
  uint8_t raw_day     = read_rtc_register(REG_DAY);
  uint8_t raw_month   = read_rtc_register(REG_MONTH);
  uint8_t raw_year    = read_rtc_register(REG_YEAR);
  
  // If the RTC is in BCD mode, convert from BCD to binary
  t->second = bcd_to_bin(raw_seconds);
  t->minute = bcd_to_bin(raw_minutes);
  t->hour   = bcd_to_bin(raw_hours);
  t->day    = bcd_to_bin(raw_day);
  t->month  = bcd_to_bin(raw_month);
  t->year   = bcd_to_bin(raw_year);

  // If you want full four-digit years, you might also read the century register
  // and combine it. E.g.:
  /*
  uint8_t raw_century = read_rtc_register(REG_CENTURY);
  uint8_t century = bcd_to_bin(raw_century);
  
  // Combine. For instance, if century = 20 and year=23 => full year = 2023
  t->full_year = (century * 100) + t->year;
  */
}

void set_rtc(uint8_t day, uint8_t month, uint8_t year,
             uint8_t hour, uint8_t minute, uint8_t second)
{
  // Convert from binary to BCD if your RTC is in BCD mode
  uint8_t bcd_sec  = bin_to_bcd(second);
  uint8_t bcd_min  = bin_to_bcd(minute);
  uint8_t bcd_hour = bin_to_bcd(hour);
  uint8_t bcd_day  = bin_to_bcd(day);
  uint8_t bcd_mon  = bin_to_bcd(month);
  uint8_t bcd_year = bin_to_bcd(year);

  // If handling the century register:
  //   uint8_t bcd_century = bin_to_bcd(20); // for example, if you assume "20xx"

  // Write them to the hardware registers
  write_rtc_register(REG_SECONDS, bcd_sec);
  write_rtc_register(REG_MINUTES, bcd_min);
  write_rtc_register(REG_HOURS,   bcd_hour);
  write_rtc_register(REG_DAY,     bcd_day);
  write_rtc_register(REG_MONTH,   bcd_mon);
  write_rtc_register(REG_YEAR,    bcd_year);
  
  // If handling century:
  //   write_rtc_register(REG_CENTURY, bcd_century);
}

///////////////////////////////////////////////////////////
// Static (Private) Helper Functions
///////////////////////////////////////////////////////////

static uint8_t read_rtc_register(uint8_t reg)
{
  // Some boards require disabling NMI by setting high bit of reg => reg | 0x80
  // if that is needed in your system. Otherwise just pass `reg`.
  outb(CMOS_ADDRESS, reg);
  return inb(CMOS_DATA);
}

static void write_rtc_register(uint8_t reg, uint8_t value)
{
  outb(CMOS_ADDRESS, reg);
  outb(CMOS_DATA, value);
}

static uint8_t bcd_to_bin(uint8_t val)
{
  return (uint8_t)(((val >> 4) & 0x0F) * 10 + (val & 0x0F));
}

static uint8_t bin_to_bcd(uint8_t val)
{
  return (uint8_t)(((val / 10) << 4) | (val % 10));
}

int is_leap_year(uint8_t year)
{
  return (year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0));
}

int month_length(uint8_t month, uint8_t year)
{
  if (month == FEB)
  {
    if (!is_leap_year(year))
    {
      return 28;
    }
    else
    {
      return 29;
    }
  }
  else if (month == APR || month == JUN || month == SEP || month == NOV)
  {
    return 30;
  }
  else 
  {
    return 31;
  }
}
