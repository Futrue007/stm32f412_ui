/*                                                           Date:08/2017  */
/*                        Pace RTC Driver                                  */
/*                                                   (c)copyright Pacewear */
/*-------------------------------------------------------------------------*/
/*   Author:  weifu.ma                                                      */
/*   Reference documents:                                                  */
/*-------------------------------------------------------------------------*/
/*  Comments:                                                              */
/*   File : pace_rtc.c                                                     */
/*   Labels :                                                              */
/*   Brief    RTC Driver                                                   */
/*=========================================================================*/
/*  Modifications                                                          */
/*-------------------------------------------------------------------------*/
/*  date    | author  |modification                                        */
/*----------|---------|----------------------------------------------------*/
/*15/08/2017| weifu.ma   Creat                                              */
/*----------|---------|----------------------------------------------------*/

/*=========================================================================*/
/*  Problems Report                                                        */
/*-------------------------------------------------------------------------*/
/*  date    | author  |modification                                        */
/*----------|---------|----------------------------------------------------*/
/***************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rtc.h"
#include "pace_msg.h"
#include "pace_task.h"
//#include "pace_uart.h"
#include "pace_bsp_rtc.h"

/* RTC handler declaration */
RTC_HandleTypeDef RtcHandle;

/* Defines related to Clock configuration */
/* Uncomment to enable the adaquate Clock Source */
#define RTC_CLOCK_SOURCE_LSE
//#define RTC_CLOCK_SOURCE_LSI

#ifdef RTC_CLOCK_SOURCE_LSI
#define RTC_ASYNCH_PREDIV    0x7F
#define RTC_SYNCH_PREDIV     0x0130
#endif

#ifdef RTC_CLOCK_SOURCE_LSE
#define RTC_ASYNCH_PREDIV  0x7F
#define RTC_SYNCH_PREDIV   0x00FF
#endif

/* Private function prototypes -----------------------------------------------*/
#if 0

void RTC_SetDate(RTC_DateTypeDef* RTC_DateStruct)
{
    HAL_RTC_SetDate(&RtcHandle, RTC_DateStruct, RTC_FORMAT_BCD);
    HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x32F2);
}

void RTC_GetDate( RTC_DateTypeDef* RTC_DateStruct)
{
    HAL_RTC_GetDate(&RtcHandle, RTC_DateStruct, RTC_FORMAT_BIN);
}

void RTC_SetTime(RTC_TimeTypeDef* RTC_TimeStruct)
{
    HAL_RTC_SetTime(&RtcHandle, RTC_TimeStruct, RTC_FORMAT_BCD);
    HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x32F2);
}

void RTC_GetTime(RTC_TimeTypeDef* RTC_TimeStruct)
{
    HAL_RTC_GetTime(&RtcHandle, RTC_TimeStruct, RTC_FORMAT_BIN);
}
void RTC_SetAlarm(RTC_AlarmTypeDef* RTC_AlarmStruct)
{
    HAL_RTC_SetAlarm(&RtcHandle, RTC_AlarmStruct, RTC_FORMAT_BCD);
}

void RTC_GetAlarm(uint32_t Alarm, RTC_AlarmTypeDef* RTC_AlarmStruct)
{
    HAL_RTC_GetAlarm(&RtcHandle, RTC_AlarmStruct, Alarm, RTC_FORMAT_BCD);
}

#endif

void RTC_SetDateTime(DateTimeType* RTC_DateTime)
{
    RTC_DateTypeDef sdatestructureget;
    RTC_TimeTypeDef stimestructureget;

    sdatestructureget.Year = RTC_DateTime->RTC_Year;
    sdatestructureget.Month = RTC_DateTime->RTC_Month;
    sdatestructureget.Date = RTC_DateTime->RTC_Date;
    sdatestructureget.WeekDay = RTC_DateTime->RTC_WeekDay;

    stimestructureget.Hours = RTC_DateTime->RTC_Hours; 
    stimestructureget.Minutes = RTC_DateTime->RTC_Minutes;
    stimestructureget.Seconds = RTC_DateTime->RTC_Seconds;
    stimestructureget.TimeFormat = RTC_HOURFORMAT12_AM;//RTC_DateTime->RTC_H12;
    
    if(HAL_RTC_SetDate(&RtcHandle,&sdatestructureget,RTC_FORMAT_BCD) != HAL_OK)
     {
        /* Error */
        Error_Handler(); 
     }
    
    if(HAL_RTC_SetTime(&RtcHandle,&stimestructureget,RTC_FORMAT_BCD) != HAL_OK)
     {
        /* Error */
        Error_Handler(); 
     }
    
    HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x32F2);
    
}

void RTC_GetDateTime(DateTimeType* RTC_DateTime)
{
    RTC_DateTypeDef sdatestructureget;
    RTC_TimeTypeDef stimestructureget;
	
    HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BCD);
    HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BCD);

    RTC_DateTime->RTC_Year = sdatestructureget.Year;
    RTC_DateTime->RTC_Month = sdatestructureget.Month;
    RTC_DateTime->RTC_Date = sdatestructureget.Date;
    RTC_DateTime->RTC_WeekDay = sdatestructureget.WeekDay;

    RTC_DateTime->RTC_Hours = stimestructureget.Hours;
    RTC_DateTime->RTC_Minutes = stimestructureget.Minutes;
    RTC_DateTime->RTC_Seconds = stimestructureget.Seconds;
   // RTC_DateTime->RTC_H12 = stimestructureget.TimeFormat;
}


void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
    /* Peripheral clock enable */
    __HAL_RCC_RTC_ENABLE();
    /* RTC interrupt Init */
    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc)
{
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();

    /* RTC interrupt DeInit */
    HAL_NVIC_DisableIRQ(RTC_Alarm_IRQn);
}


void pace_rtc_init(void) 
{

  /*##-1- Configure the RTC peripheral #######################################*/
  /* Configure RTC prescaler and RTC data registers */
  /* RTC configured as follow:
      - Hour Format    = Format 24
      - Asynch Prediv  = Value according to source clock
      - Synch Prediv   = Value according to source clock
      - OutPut         = Output Disable
      - OutPutPolarity = High Polarity
      - OutPutType     = Open Drain */
  RtcHandle.Instance = RTC; 
  RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  
  if(HAL_RTC_Init(&RtcHandle) != HAL_OK)
  {
    /* Error */
    Error_Handler(); 
  }
	printf("pace_rtc_init\n");
}

void pace_data_time_config(void)
{
   RTC_DateTypeDef sdatestructure;
   RTC_TimeTypeDef stimestructure;
  /*## Check if Data stored in BackUp register0: No Need to reconfigure RTC#*/
  /* Read the BackUp Register 0 Data */
  if(HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR0) != 0x32F2)
  {  

  /*##-1- Configure the Date #################################################*/
  /* Set Date: SUNDAY JANUARY 1th 2017 */
  sdatestructure.Year = 0x17;
  sdatestructure.Month = RTC_MONTH_JANUARY;
  sdatestructure.Date = 0x01;
  sdatestructure.WeekDay = RTC_WEEKDAY_SUNDAY;
  
  if(HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Error */
    Error_Handler(); 
  } 
  
  /*##-2- Configure the Time #################################################*/
  /* Set Time: 00:00:00 */
  stimestructure.Hours = 0x23;
  stimestructure.Minutes = 0x59;
  stimestructure.Seconds = 0x50;
  stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;
  
  if(HAL_RTC_SetTime(&RtcHandle,&stimestructure,RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Error */
    Error_Handler(); 
  }
  
  /*##-3- Writes a data in a RTC Backup data Register0 #######################*/
  HAL_RTCEx_BKUPWrite(&RtcHandle,RTC_BKP_DR0,0x32F2);  
  
  }
}


void pace_alarm_config(void)
{
    RTC_AlarmTypeDef salarmstructure;
    
    /*Alarm A */
    salarmstructure.Alarm = RTC_ALARM_A;
    salarmstructure.AlarmDateWeekDay = RTC_WEEKDAY_MONDAY;
    salarmstructure.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    salarmstructure.AlarmMask = RTC_ALARMMASK_ALL^RTC_ALARMMASK_SECONDS;
    salarmstructure.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
    salarmstructure.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;
    salarmstructure.AlarmTime.Hours = 0x00;
    salarmstructure.AlarmTime.Minutes = 0x00;
    salarmstructure.AlarmTime.Seconds = 0x00;
    salarmstructure.AlarmTime.SubSeconds = 0x00;
    if(HAL_RTC_SetAlarm_IT(&RtcHandle,&salarmstructure, RTC_FORMAT_BCD) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler(); 
    }
    
    /*Alarm B */
    salarmstructure.Alarm = RTC_ALARM_B;
    salarmstructure.AlarmDateWeekDay = RTC_WEEKDAY_MONDAY;
    salarmstructure.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    salarmstructure.AlarmMask = RTC_ALARMMASK_ALL;
    salarmstructure.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
    salarmstructure.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;
    salarmstructure.AlarmTime.Hours = 0x00;
    salarmstructure.AlarmTime.Minutes = 0x00;
    salarmstructure.AlarmTime.Seconds = 0x00;
    salarmstructure.AlarmTime.SubSeconds = 0x00;
    if(HAL_RTC_SetAlarm_IT(&RtcHandle, &salarmstructure, RTC_FORMAT_BCD) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler(); 
    } 

}


 DateTimeType convert_bcd_to_dec(DateTimeType* bcd_datatime)
{
    DateTimeType temp = {0};
    if(bcd_datatime != NULL)
    {
        temp.RTC_Year = (bcd_datatime->RTC_Year/16)*10 + bcd_datatime->RTC_Year%16 + 2000;
        temp.RTC_Month = (bcd_datatime->RTC_Month/16)*10 + bcd_datatime->RTC_Month%16;
        temp.RTC_Date = (bcd_datatime->RTC_Date/16)*10 + bcd_datatime->RTC_Date%16;
        temp.RTC_WeekDay = bcd_datatime->RTC_WeekDay;
        temp.RTC_Hours = (bcd_datatime->RTC_Hours/16)*10 + bcd_datatime->RTC_Hours%16;
        temp.RTC_Minutes = (bcd_datatime->RTC_Minutes/16)*10 + bcd_datatime->RTC_Minutes%16;
        temp.RTC_Seconds = (bcd_datatime->RTC_Seconds/16)*10 + bcd_datatime->RTC_Seconds%16;
    }
    
    return temp;

}


 DateTimeType convert_dec_to_bcd(DateTimeType* dec_datatime)
{
    DateTimeType temp = {0};
    if(dec_datatime != NULL)
    {
        temp.RTC_Year = ((dec_datatime->RTC_Year-2000)/10)*16 + (dec_datatime->RTC_Year-2000)%10;
        temp.RTC_Month = (dec_datatime->RTC_Month/10)*16 + dec_datatime->RTC_Month%10;
        temp.RTC_Date = (dec_datatime->RTC_Date/10)*16 + dec_datatime->RTC_Date%10;
        temp.RTC_WeekDay = dec_datatime->RTC_WeekDay;
        temp.RTC_Hours = (dec_datatime->RTC_Hours/10)*16 + dec_datatime->RTC_Hours%10;
        temp.RTC_Minutes = (dec_datatime->RTC_Minutes/10)*16 + dec_datatime->RTC_Minutes%10;
        temp.RTC_Seconds = (dec_datatime->RTC_Seconds/10)*16 + dec_datatime->RTC_Seconds%10;

    }
    
    return temp;

}


void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    /* Send MSG to GUI every minute */
  pace_msg msg_intr= NULL_MSG;
  msg_intr.msg_id = MSG_ID_GUI_UPDATE_TIME;
  msg_intr.src_task = getCurrentTaskId();
  msg_intr.dst_task = TASK_GUI;
  msg_intr.buffer = NULL;
  msg_intr.hEvent = NULL;

  pace_post_msg_to_queue(&msg_intr);


}
void HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *hrtc)
{
  /* Send MSG to GUI every second */

	

}
