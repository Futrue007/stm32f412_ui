

#ifndef PACE_HAL_RTC_H__
#define PACE_HAL_RTC_H__
#include "pace_bsp_rtc.h"

#ifdef __cplusplus
extern "C" {
#endif


/**< macro defination*/
#define ISLEAPYEAR(yr) (!((yr) % 400) || (((yr) % 100) && !((yr) % 4)))
#define YEARLENGTH(yr) (ISLEAPYEAR(yr) ? 366 : 365)
#define BEGYEAR         1970      /**< UTC started at 00:00:00 January 1, 1970*/
#define DAY             86400UL   /**< 24 hours * 60 minutes * 60 seconds*/
#define SYSTEM_ORIGIN_DAY_OF_WEEK (THUR)/**< 1970-01-01 is THUR*/
#define LENGTH_OF_WEEK      (7)



/**< week time enum*/
typedef enum
{
    MON  = 0,
    TUES = 1,
    WED  = 2,
    THUR = 3,
    FRI  = 4,
    SAT  = 5,
    SUN  = 6
} DAY_OF_WEEK;

/**< calend time struct*/
typedef struct
{
    uint16_t year;    // 1970+
    uint8_t month;    // 1-12
    uint8_t day;      // 1-30
    uint8_t hour;     // 0-23
	uint8_t minutes;  // 0-59
	uint8_t seconds;  // 0-59
	DAY_OF_WEEK week; // 1 -7 
} calendar_time_t;

void pace_hal_rtc_init(void); 
void pace_hal_set_date_time(DateTimeType* BCD_DateTime);
DateTimeType pace_hal_get_date_time(void);

uint32_t pace_convert_current_time_to_timestamp(void);

uint32_t pace_convert_calendar_time_to_timestamp(calendar_time_t *ptime);
calendar_time_t pace_convert_timestamp_to_calendar_time(uint32_t timestamp);




#ifdef __cplusplus
}
#endif

#endif // PACE_HAL_RTC_H__
