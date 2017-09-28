/**
 *****************************************************************************************
 *
 * @file pace_hal_rtc.c
 *
 * @brief  RTC hal
 *
 *
 *
 *****************************************************************************************
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "pace_hal_rtc.h" 

  

 #if 0 
calendar_time_t             calendar_format_time;        
static volatile uint32_t    second_counter;              
static volatile uint32_t    second_counter_GMT;

#define BEIJIN_TIMEZONE_OFFSET  (8*60*60)
int32_t current_timezont_offset = 0;
#endif


  /**
  * @func   pace_hal_rtc_init
  * @brief  rtc init 
  *            set date&time :2017-01-10 00:00:00
  *          set set second&minute interrupt
  ***/
void pace_hal_rtc_init(void) 
{
    pace_rtc_init();
    pace_data_time_config();
    pace_alarm_config();
}

void pace_hal_set_date_time(DateTimeType* DEC_DateTime)
{
    DateTimeType BCD_DateTime;
    DateTimeType dec_date_time;
    dec_date_time = pace_hal_get_date_time();

    if(dec_date_time.RTC_Year != DEC_DateTime->RTC_Year)
        dec_date_time.RTC_Year = DEC_DateTime->RTC_Year;
    if(dec_date_time.RTC_Month != DEC_DateTime->RTC_Month)
        dec_date_time.RTC_Month = DEC_DateTime->RTC_Month;
    if(dec_date_time.RTC_Date != DEC_DateTime->RTC_Date)
        dec_date_time.RTC_Date = DEC_DateTime->RTC_Date;
    if(dec_date_time.RTC_WeekDay != DEC_DateTime->RTC_WeekDay)
        dec_date_time.RTC_WeekDay = DEC_DateTime->RTC_WeekDay;
    if(dec_date_time.RTC_Hours != DEC_DateTime->RTC_Hours)
        dec_date_time.RTC_Hours= DEC_DateTime->RTC_Hours;
    if(dec_date_time.RTC_Minutes!= DEC_DateTime->RTC_Minutes)
        dec_date_time.RTC_Minutes = DEC_DateTime->RTC_Minutes;
    if(dec_date_time.RTC_Seconds != DEC_DateTime->RTC_Seconds)
        dec_date_time.RTC_Seconds = DEC_DateTime->RTC_Seconds;
    
    BCD_DateTime = convert_dec_to_bcd(&dec_date_time);
    RTC_SetDateTime(&BCD_DateTime);
}

DateTimeType pace_hal_get_date_time(void)
{

    DateTimeType BCD_DateTime = {0};
    RTC_GetDateTime(&BCD_DateTime);
    
    return convert_bcd_to_dec(&BCD_DateTime);
}


uint32_t pace_convert_current_time_to_timestamp(void)
{
    DateTimeType current_gmt_time = {0};
    calendar_time_t ptime = {0};
    uint32_t current_timestamp = 0;
    current_gmt_time = pace_hal_get_date_time();
    
    ptime.year = current_gmt_time.RTC_Year;
    ptime.month= current_gmt_time.RTC_Month;
    ptime.day= current_gmt_time.RTC_Date;
    ptime.hour= current_gmt_time.RTC_Hours;
    ptime.minutes= current_gmt_time.RTC_Minutes;
    ptime.seconds= current_gmt_time.RTC_Seconds;
    current_timestamp = pace_convert_calendar_time_to_timestamp(&ptime);
    return current_timestamp;
    
}

static uint8_t monthLength( uint8_t lpyr, uint8_t mon )
{
    uint8_t days = 31;
    if ( mon == 1 ) // feb
    {
        days = ( 28 + lpyr );
    }
    else 
    {
        if ( mon > 6 ) // aug-dec
        {
            mon--;
        }

        if ( mon & 1 )
        {
            days = 30;
        }
    }
    return ( days );
}



/**< conver calender time to second time */
 uint32_t pace_convert_calendar_time_to_timestamp(calendar_time_t *ptime)
{
    uint32_t i = 0;
    uint32_t offset = 0;

    //day time
    offset += ptime->seconds;
    offset += ptime->minutes * 60;
    offset += ptime->hour * 60 * 60;

    uint8_t leapYear = ISLEAPYEAR(ptime->year);

    offset += DAY * (ptime->day - 1);

    for(i = 0; i < ptime->month -1; ++i)   //month start from 1
    {
        offset += monthLength(leapYear, i) * DAY;
    }

    for(i = BEGYEAR; i < ptime->year ; i++)
    {
        if(ISLEAPYEAR(i))
        {
            offset += DAY * 366;
        }
        else
        {
            offset += DAY * 365;
        }
    }
    return offset;
}




/* calculate day of week */
static DAY_OF_WEEK pace_get_day_of_week(uint32_t timestamp)
{
    uint32_t day = timestamp / DAY;
    DAY_OF_WEEK today = (DAY_OF_WEEK)(((day % LENGTH_OF_WEEK) + SYSTEM_ORIGIN_DAY_OF_WEEK) % LENGTH_OF_WEEK);

    return today;
}


calendar_time_t pace_convert_timestamp_to_calendar_time(uint32_t timestamp )
{
    calendar_time_t calendar_tm;
	
    // calculate the time less than a day - hours, minutes, seconds
    uint32_t day = timestamp % DAY;
    calendar_tm.seconds = day % 60UL;
    calendar_tm.minutes = (day % 3600UL) / 60UL;
    calendar_tm.hour = day / 3600UL;

    // Fill in the calendar - day, month, year
    uint16_t numDays = timestamp / DAY;
    calendar_tm.year = BEGYEAR;
    while ( numDays >= YEARLENGTH( calendar_tm.year ) )
    {
        numDays -= YEARLENGTH( calendar_tm.year );
        calendar_tm.year++;
    }

    calendar_tm.month = 0;
    while ( numDays >= monthLength( ISLEAPYEAR( calendar_tm.year ), calendar_tm.month ))
    {
        numDays -= monthLength( ISLEAPYEAR( calendar_tm.year ), calendar_tm.month );
        calendar_tm.month++;
    }

    calendar_tm.day = numDays;
    calendar_tm.week = pace_get_day_of_week(timestamp);
    
    calendar_tm.month += 1; //calibration
    calendar_tm.day += 1; //calibration
    calendar_tm.week += 1; //calibration

    return calendar_tm;
}

#if 0
uint32_t get_rtc_time_in_GMT(void)
{
    return second_counter_GMT;
}
uint32_t get_rtc_time_in_beijin_timezone(void)
{
    return (second_counter_GMT + BEIJIN_TIMEZONE_OFFSET);
}
uint32_t get_rtc_time_in_current_timezone(void)
{
    return (second_counter_GMT + current_timezont_offset);
}
uint32_t get_rtc_time_in_current_time(void)
{
 //   return get_rtc_time();
}
#endif


