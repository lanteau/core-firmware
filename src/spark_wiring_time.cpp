/**
 ******************************************************************************
 * @file    spark_wiring_time.cpp
 * @author  Satish Nair
 * @version V1.0.0
 * @date    3-March-2014
 * @brief   Time utility functions to set and get Date/Time using RTC
 ******************************************************************************
  Copyright (c) 2013-14 Spark Labs, Inc.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */

#include "spark_wiring_time.h"

static struct skew
{
  int8_t error;
  uint8_t ticks;
} skew;

/* The calendar "tm" structure from the standard libray "time.h" has the following definition: */
//struct tm
//{
//	int tm_sec;         /* seconds,  range 0 to 59          */
//	int tm_min;         /* minutes, range 0 to 59           */
//	int tm_hour;        /* hours, range 0 to 23             */
//	int tm_mday;        /* day of the month, range 1 to 31  */
//	int tm_mon;         /* month, range 0 to 11             */
//	int tm_year;        /* The number of years since 1900   */
//	int tm_wday;        /* day of the week, range 0 to 6    */
//	int tm_yday;        /* day in the year, range 0 to 365  */
//	int tm_isdst;       /* daylight saving time             */
//};

struct tm calendar_time_cache;	// a cache of calendar time structure elements
time_t unix_time_cache;  		// a cache of unix_time that was updated
time_t time_zone_cache;			// a cache of the time zone that was set

/* Time utility functions */
static struct tm Convert_UnixTime_To_CalendarTime(time_t unix_time);
static time_t Convert_CalendarTime_To_UnixTime(struct tm calendar_time);
static time_t Get_UnixTime(void);
static cal_time Get_CalendarTime(void);
static void Set_UnixTime(time_t unix_time);
static void Set_CalendarTime(struct tm t);
static void Refresh_UnixTime_Cache(time_t unix_time);

/*******************************************************************************
 * Function Name  : Wiring_RTC_Interrupt_Handler (Declared as weak in stm32_it.cpp)
 * Description    : This function handles RTC global interrupt request.
 * Input          : None.
 * Output         : None.
 * Return         : None.
 *******************************************************************************/
void Wiring_RTC_Interrupt_Handler(void)
{
  // // Only intervene if we have an error to correct
  // if (0 != skew.error && 0 < skew.ticks)
  // {
  //   time_t now = Get_UnixTime();

  //   // By default, we set the clock 1 second forward
  //   time_t skew_step = 1;

  //   if (skew.error > 0)
  //   {
  //     // Error is positive, so we need to slow down
  //     if (skew.ticks / skew.error < 2)
  //     {
  //       // Don't let time go backwards!
  //       // Hold the clock still for a second
  //       skew_step--;
  //       skew.error--;
  //     }
  //   }
  //   else
  //   {
  //     // Error is negative, so we need to speed up
  //     if (skew.ticks / skew.error > -2)
  //     {
  //       // Skip a second forward
  //       skew_step++;
  //       skew.error++;
  //     }
  //   }

  //   skew.ticks--;
  //   Set_UnixTime(now + skew_step);
  // }
}

/* Convert Unix/RTC time to Calendar time */
// static struct tm Convert_UnixTime_To_CalendarTime(time_t unix_time)
// {
// 	struct tm *calendar_time;
// 	unix_time += time_zone_cache;
// 	calendar_time = localtime(&unix_time);
// 	calendar_time->tm_year += 1900;
// 	return *calendar_time;
// }

/* Convert Calendar time to Unix/RTC time */
// static time_t Convert_CalendarTime_To_UnixTime(struct tm calendar_time)
// {
// 	calendar_time.tm_year -= 1900;
// 	time_t unix_time = mktime(&calendar_time);
// 	return unix_time;
// }

const int DAYS_PER_YEAR        = 365;           // Non leap year
const int SECONDS_PER_YEAR     = 31536000;      // Non leap year
const int SECONDS_PER_DAY      = 86400;
const int SECONDS_PER_HOUR     = 3600;
const int SECONDS_PER_MINUTE   = 60;

 
// This leap year test works for all years from 1901 to 2099 (yes, including 2000)
// Which is more than enough for UnixTime computations, which only operate over the range [1970, 2038].  (Omei Qunhua)
static int LeapYear(uint8_t year)
{
    return !(year & 3);
}
 
static int DaysPerMonth(uint8_t year, uint8_t month)
{
    // Compact Days-Per-Month algorithm. Omei Qunhua.
    if (month == 2)
    	return 28 + LeapYear(year);
    return 30 + ( (month + (month > 7) ) & 1); // Odd months up to July, and even months after July, have 31 days
}
 
static int DaysPerYear(uint8_t year)
{
	return 365 + LeapYear(year);
}

/* Get converted Calendar time */
static cal_time Get_CalendarTime(void)
{
	RTC_DateTypeDef RTC_DateStructure;
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);

	cal_time calendar_time;
	calendar_time.year = RTC_DateStructure.RTC_Year;
	calendar_time.month = RTC_DateStructure.RTC_Month;
	calendar_time.day = RTC_DateStructure.RTC_Date;
	calendar_time.weekday = RTC_DateStructure.RTC_WeekDay;
	calendar_time.hour = RTC_TimeStructure.RTC_Hours;
	calendar_time.minute = RTC_TimeStructure.RTC_Minutes;
	calendar_time.second = RTC_TimeStructure.RTC_Seconds;

	return calendar_time;
}

/* Get Unix/RTC time */
static time_t Get_UnixTime(void)
{
	cal_time time = Get_CalendarTime();
	time_t unix_time = 0;
	int yr = 1970;
	int mt = 1;
	int days;
 
	while(yr < time.year)
	{
		days = DaysPerYear(yr++);
		unix_time += days * SECONDS_PER_DAY;
	}
 
	while (mt < time.month)
	{
		days = DaysPerMonth(time.year, mt++);
		unix_time += days * SECONDS_PER_DAY;
	}
 
	days = time.day - 1;
	unix_time += days * SECONDS_PER_DAY;
	unix_time += time.hour * SECONDS_PER_HOUR;
	unix_time += time.minute * SECONDS_PER_MINUTE;
	unix_time += time.second;

	return unix_time;
}

// /* Set Unix/RTC time */
// static void Set_UnixTime(time_t unix_time)
// {
// 	RTC_WaitForLastTask();
// 	RTC_SetCounter((uint32_t)unix_time);
// 	RTC_WaitForLastTask();
// }

/* Set Calendar time as Unix/RTC time */
static void Set_CalendarTime(cal_time calendar_time)
{
	RTC_DateTypeDef RTC_DateStructure;
	RTC_TimeTypeDef RTC_TimeStructure;

	/* Set the date: Friday January 11th 2013 */
	RTC_DateStructure.RTC_Year = calendar_time.year;
	RTC_DateStructure.RTC_Month = calendar_time.month;
	RTC_DateStructure.RTC_Date = calendar_time.day;
	RTC_DateStructure.RTC_WeekDay = calendar_time.weekday;
	RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);

	/* Set the time to 05h 20mn 00s AM */
	RTC_TimeStructure.RTC_Hours   = calendar_time.hour;
	RTC_TimeStructure.RTC_Minutes = calendar_time.minute;
	RTC_TimeStructure.RTC_Seconds = calendar_time.second; 
	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure); 
}

// /* Refresh Unix/RTC time cache */
// static void Refresh_UnixTime_Cache(time_t unix_time)
// {
// 	if(unix_time != unix_time_cache)
// 	{
// 		calendar_time_cache = Convert_UnixTime_To_CalendarTime(unix_time);
// 		unix_time_cache = unix_time;
// 	}
// }

/* current hour */
int TimeClass::hour()
{
	cal_time time = Get_CalendarTime();
	return time.hour;
}

// /* the hour for the given time */
// int TimeClass::hour(time_t t)
// {
// 	Refresh_UnixTime_Cache(t);
// 	return calendar_time_cache.tm_hour;
// }

/* current hour in 12 hour format */
int TimeClass::hourFormat12()
{
	cal_time time = Get_CalendarTime();
	if(time.hour > 12)
		time.hour -= 12;
	return time.hour;
}

// /* the hour for the given time in 12 hour format */
// int TimeClass::hourFormat12(time_t t)
// {
// 	Refresh_UnixTime_Cache(t);
// 	if(calendar_time_cache.tm_hour == 0)
// 		return 12;	//midnight
// 	else if( calendar_time_cache.tm_hour > 12)
// 		return calendar_time_cache.tm_hour - 12 ;
// 	else
// 		return calendar_time_cache.tm_hour ;
// }

/* returns true if time now is AM */
uint8_t TimeClass::isAM()
{
	cal_time time = Get_CalendarTime();
	return time.hour < 12;
}

/* returns true the given time is AM */
// uint8_t TimeClass::isAM(time_t t)
// {
// 	return !isPM(t);
// }

/* returns true if time now is PM */
uint8_t TimeClass::isPM()
{
	return !isAM();
}

/* returns true the given time is PM */
// uint8_t TimeClass::isPM(time_t t)
// {
// 	return (hour(t) >= 12);
// }

/* current minute */
int TimeClass::minute()
{
	cal_time time = Get_CalendarTime();
	return time.minute;
}

/* the minute for the given time */
// int TimeClass::minute(time_t t)
// {
// 	Refresh_UnixTime_Cache(t);
// 	return calendar_time_cache.tm_min;
// }

/* current seconds */
int TimeClass::second()
{
	cal_time time = Get_CalendarTime();
	return time.second;
}

/* the second for the given time */
// int TimeClass::second(time_t t)
// {
// 	Refresh_UnixTime_Cache(t);
// 	return calendar_time_cache.tm_sec;
// }

/* current day */
int TimeClass::day()
{
	cal_time time = Get_CalendarTime();
	return time.day;
}

/* the day for the given time */
// int TimeClass::day(time_t t)
// {
// 	Refresh_UnixTime_Cache(t);
// 	return calendar_time_cache.tm_mday;
// }

/* the current weekday */
int TimeClass::weekday()
{
	cal_time time = Get_CalendarTime();
	return time.weekday;
}

/* the weekday for the given time */
// int TimeClass::weekday(time_t t)
// {
// 	Refresh_UnixTime_Cache(t);
// 	return (calendar_time_cache.tm_wday + 1);//Arduino's weekday representation
// }

/* current month */
int TimeClass::month()
{
	cal_time time = Get_CalendarTime();
	return time.month;
}

/* the month for the given time */
// int TimeClass::month(time_t t)
// {
// 	Refresh_UnixTime_Cache(t);
// 	return (calendar_time_cache.tm_mon + 1);//Arduino's month representation
// }

/* current four digit year */
int TimeClass::year()
{
	cal_time time = Get_CalendarTime();
	return time.year;
}

/* the year for the given time */
// int TimeClass::year(time_t t)
// {
// 	Refresh_UnixTime_Cache(t);
// 	return calendar_time_cache.tm_year;
// }

/* return the current time as seconds since Jan 1 1970 */
time_t TimeClass::now()
{
	return Get_UnixTime();
}

/* set the time zone (+/-) offset from GMT */
// void TimeClass::zone(float GMT_Offset)
// {
// 	if(GMT_Offset < -12 || GMT_Offset > 13)
// 	{
// 		return;
// 	}
// 	time_zone_cache = GMT_Offset * 3600;
// }

/* set the given time as unix/rtc time */
void TimeClass::setTime(time_t t)
{
  // int32_t delta_error = Get_UnixTime() - t;
  // if (delta_error > 127 || delta_error < -127)
  // {
  //   // big delta, jump abruptly to the new time
  //   Set_UnixTime(t);
  // }
  // else
  // {
  //   // small delta, gradually skew toward the new time
  //   skew.error = delta_error;
  //   skew.ticks = 2 * abs(delta_error);
  // }
	int days_since_1_1_1970     = t / SECONDS_PER_DAY;
    int day = days_since_1_1_1970 + 1;
    int year  = 1970;
    int days_per_year = DaysPerYear(year);
 
    while (day > days_per_year)
    {
        day -= days_per_year;
        ++year;
        days_per_year = DaysPerYear(year);
    }
 
    int month = 1;
    int days_per_month = DaysPerMonth(year, month);
 
    while (day > days_per_month)
    {
        day -= days_per_month;
 
        if (++month > 12)
        {    
            ++year;
            month = 1;
        }
 
        days_per_month = DaysPerMonth(year, month);
    }
 
    int seconds_since_midnight  = t % SECONDS_PER_DAY;
    int hour        = seconds_since_midnight / SECONDS_PER_HOUR;
    int second      = seconds_since_midnight % SECONDS_PER_HOUR;
    int minute      = second / SECONDS_PER_MINUTE;
    second          = second % SECONDS_PER_MINUTE;

    cal_time time;
    time.year = year;
    time.month = month;
    time.day = day;
    time.hour = hour;
    time.minute = minute;
    time.second = second;

    Set_CalendarTime(time);
}


/* return string representation of the current time */
// String TimeClass::timeStr()
// {
// 	return timeStr(now());
// }

/* return string representation for the given time */
// String TimeClass::timeStr(time_t t)
// {
// 	cal_time time = Get_CalendarTime();

// 	struct tm *calendar_time;
// 	t += time_zone_cache;
// 	calendar_time = localtime(&t);
// 	String calendar_time_string;;
// 	return calendar_time_string;
// }

TimeClass Time;
