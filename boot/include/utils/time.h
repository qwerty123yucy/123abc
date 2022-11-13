#ifndef	__TIME_H
#define __TIME_H

#include <stdint.h>
#include <stdbool.h>


#define GMT_VAL				8U


#define DAY_IN_COMMON_YEAR		365U
#define DAY_IN_LEAP_YEAR		366U


#define HOUR_IN_DAY			24U
#define MIN_IN_HOUR			60U
#define SEC_IN_MIN			60U

#define SEC_IN_DAY				(HOUR_IN_DAY * MIN_IN_HOUR * SEC_IN_MIN)
#define SEC_IN_HOUR				(SEC_IN_MIN * MIN_IN_HOUR)


#define SEC_IN_COMMON_YEAR			(DAY_IN_COMMON_YEAR * SEC_IN_DAY)
#define SEC_IN_LEAP_YEAR			(DAY_IN_LEAP_YEAR * SEC_IN_DAY)

// 'period' is a four year period with 'common' 'common' 'leap' 'common' sequence
// for convenience since unix timestamp starting with 1970 
#define SEC_IN_PERIOD				(SEC_IN_LEAP_YEAR + (SEC_IN_COMMON_YEAR * 3))

// given the unix timestamp, return the period_idx
#define PERIOD_OF_SEC(sec)			(sec / SEC_IN_PERIOD)
#define PERIOD_OF_SEC_REMAIN(sec)		(sec % SEC_IN_PERIOD)


// given the sec_idx in a period, return the year_idx in a period
#define YEAR_OF_SEC(sec)			((sec >= (3 * SEC_IN_COMMON_YEAR))?(sec - SEC_IN_DAY)/SEC_IN_COMMON_YEAR:sec/SEC_IN_COMMON_YEAR)
#define IS_LEAP_OF_SEC(sec)			(YEAR_OF_SEC(sec) == 2)
#define YEAR_OF_SEC_REMAIN(sec)			(sec - (SEC_IN_COMMON_YEAR * YEAR_OF_SEC(sec)) - (YEAR_OF_SEC(sec)>2?SEC_IN_DAY:0) )

// given the sec_idx in a year, return the day_idx in this year
#define DAY_OF_SEC(sec)				(sec / SEC_IN_DAY)


struct date_t {
	uint16_t year;
	uint8_t mon;
	uint8_t day;
};

struct time_t {
	uint16_t hour;
	uint8_t	min;
	uint8_t sec;
};

// this function return a date_t with its content filled according to the given unix timestamp
struct date_t get_date(uint32_t sec);

// this function return a time_t with its content filled according to the given unix timestamp
struct time_t get_time(uint32_t sec);


void put_current_date();
void put_current_time();



#endif
