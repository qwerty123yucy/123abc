#include <utils/time.h>
#include <stdbool.h>
#include <stdint.h>
#include <utils/io.h>
#include <rtc.h>
static uint8_t day_of_mon[12] = {31, 0, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// given the day number in a year, it return its month and the day_idx in this month
// if is_leap, then day should be between 0-365, otherwise it should be between 0-364
// the mon_idx returned is between 0-11 indicating month 1-12
// bit 15..8: mon_idx  7..0: day_idx
uint16_t mon_of_day(int day, bool is_leap){
	int index = -1;
	if(day >= DAY_IN_COMMON_YEAR){
		if(!(is_leap && day == DAY_IN_COMMON_YEAR)){
			return 0;
		}
	}
	
	do{
		index++;
		// 'feb' needs additional care
		if(index == 1){
			if(is_leap){
				day -= 29;
			}
			else{
				day -= 28;
			}
		}
		else{
			day -= day_of_mon[index];
		}
	}while(day >= 0);
	
	if(index == 1){
		if(is_leap){
			day += 29;
		}
		else{
			day += 28;
		}
	}
	else{
		day += day_of_mon[index];
	}
	
	return (uint16_t)index << 8 | (uint16_t)day;
}

// this function return a date_t with its content filled according to the given unix timestamp
struct date_t get_date(uint32_t sec){
	struct date_t d;
	
	uint32_t period = PERIOD_OF_SEC(sec);
	uint32_t period_remain = PERIOD_OF_SEC_REMAIN(sec);
	uint32_t year_idx_in_period = YEAR_OF_SEC(period_remain);
	
	d.year = (uint16_t)(period * 4 + year_idx_in_period + 1970);
	
	uint32_t sec_idx_in_year = YEAR_OF_SEC_REMAIN(period_remain);
	uint32_t day_idx_in_year = DAY_OF_SEC(sec_idx_in_year);
	
	uint16_t mon_day = mon_of_day((int)day_idx_in_year, IS_LEAP_OF_SEC(period_remain));

	d.mon = (uint8_t)(mon_day >> 8);
	d.day = (uint8_t)(mon_day & 0xff);

	return d;


}

// this function return a time_t with its content filled according to the given unix timestamp
struct time_t get_time(uint32_t sec){
	struct time_t t;

	uint32_t sec_idx_in_day = sec % SEC_IN_DAY;
	uint32_t hour = sec_idx_in_day / SEC_IN_HOUR;
	uint32_t sec_idx_in_hour = sec_idx_in_day % SEC_IN_HOUR;
	uint32_t min = sec_idx_in_hour / SEC_IN_MIN;
	uint32_t sec_r = sec_idx_in_hour % SEC_IN_MIN;
	
	t.hour = (uint16_t)hour;
	t.min = (uint8_t)min;
	t.sec = (uint8_t)sec_r;
	
	return t;
}

void put_current_date(){
	uint32_t rtc_cnt = rtc_read_cnt();
	rtc_cnt += GMT_VAL * SEC_IN_HOUR;
	struct date_t d = get_date(rtc_cnt);
        uint32_t year = (uint32_t)d.year;
        uint32_t mon = (uint32_t)d.mon;
        uint32_t day = (uint32_t)d.day;
	print_f("%u-%u-%u", year, mon + 1, day + 1);
	
	return;
}

void put_current_time(){
	uint32_t rtc_cnt = rtc_read_cnt();
	rtc_cnt += GMT_VAL * SEC_IN_HOUR;
	struct time_t t = get_time(rtc_cnt);
        uint32_t hour = (uint32_t)t.hour;
        uint32_t min = (uint32_t)t.min;
        uint32_t sec = (uint32_t)t.sec;
	print_f("%u:%u:%u", hour, min, sec);
	
	return;


}







