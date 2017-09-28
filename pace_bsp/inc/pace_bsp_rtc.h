/**
  ******************************************************************************
  * @file    rtc.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    11-November-2013
  * @brief   Header for rtc module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PACE_RTC_H
#define __PACE_RTC_H
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rtc.h"

#define RTC_Format_BIN    RTC_FORMAT_BIN
#define RTC_Format_BCD    RTC_FORMAT_BCD

#define RTC_H12_AM	RTC_HOURFORMAT12_AM
#define RTC_H12_PM	RTC_HOURFORMAT12_PM

#define RTC_Weekday_Monday             ((uint8_t)0x01)
#define RTC_Weekday_Tuesday            ((uint8_t)0x02)
#define RTC_Weekday_Wednesday          ((uint8_t)0x03)
#define RTC_Weekday_Thursday           ((uint8_t)0x04)
#define RTC_Weekday_Friday             ((uint8_t)0x05)
#define RTC_Weekday_Saturday           ((uint8_t)0x06)
#define RTC_Weekday_Sunday             ((uint8_t)0x07)

/* Coded in BCD format */
#define RTC_Month_January              ((uint8_t)0x01)
#define RTC_Month_February             ((uint8_t)0x02)
#define RTC_Month_March                ((uint8_t)0x03)
#define RTC_Month_April                ((uint8_t)0x04)
#define RTC_Month_May                  ((uint8_t)0x05)
#define RTC_Month_June                 ((uint8_t)0x06)
#define RTC_Month_July                 ((uint8_t)0x07)
#define RTC_Month_August               ((uint8_t)0x08)
#define RTC_Month_September            ((uint8_t)0x09)
#define RTC_Month_October              ((uint8_t)0x10)
#define RTC_Month_November             ((uint8_t)0x11)
#define RTC_Month_December             ((uint8_t)0x12)


typedef struct
{
  
  uint16_t RTC_Year;     /*!< Specifies the RTC Date Year.
                        This parameter must be set to a value in the 0-99 range. */
  uint8_t RTC_Month;   /*!< Specifies the RTC Date Month (in BCD format).
                        This parameter can be a value of @ref RTC_Month_Date_Definitions */
  uint8_t RTC_Date;     /*!< Specifies the RTC Date.
                        This parameter must be set to a value in the 1-31 range. */
  uint8_t RTC_WeekDay; /*!< Specifies the RTC Date WeekDay.
                        This parameter can be a value of @ref RTC_WeekDay_Definitions */
  uint8_t RTC_Hours;    /*!< Specifies the RTC Time Hour.
                        This parameter must be set to a value in the 0-12 range
                        if the RTC_HourFormat_12 is selected or 0-23 range if
                        the RTC_HourFormat_24 is selected. */
  uint8_t RTC_Minutes;  /*!< Specifies the RTC Time Minutes.
                        This parameter must be set to a value in the 0-59 range. */
  uint8_t RTC_Seconds;  /*!< Specifies the RTC Time Seconds.
                        This parameter must be set to a value in the 0-59 range. */
}DateTimeType;

void pace_rtc_init(void);
void pace_data_time_config(void);
void pace_alarm_config(void);
DateTimeType convert_bcd_to_dec(DateTimeType* bcd_datatime);
DateTimeType convert_dec_to_bcd(DateTimeType* dec_datatime);

void RTC_GetDateTime(DateTimeType* RTC_DateTime);
void RTC_SetDateTime(DateTimeType* RTC_DateTime);
#if 0

void RTC_SetDate(RTC_DateTypeDef* RTC_DateStruct);
void RTC_GetDate(RTC_DateTypeDef* RTC_DateStruct);
void RTC_SetTime(RTC_TimeTypeDef* RTC_TimeStruct);
void RTC_GetTime(RTC_TimeTypeDef* RTC_TimeStruct);
void RTC_SetAlarm(uint32_t RTC_Format, RTC_AlarmTypeDef* RTC_AlarmStruct);
void RTC_GetAlarm(uint32_t RTC_Format, uint32_t Alarm, RTC_AlarmTypeDef* RTC_AlarmStruct);
#endif
#endif /* __PACE_RTC_H */

