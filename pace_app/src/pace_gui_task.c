/***************************************************************************/
/*                                                           Date:08/2017  */
/*                        Pace GUI Task                                    */
/*                                                   (c)copyright Pacewear */
/*-------------------------------------------------------------------------*/
/*   Author:  weifu.ma                                                      */
/*   Reference documents:                                                  */
/*-------------------------------------------------------------------------*/
/*  Comments:                                                              */
/*   File : pace_task.c                                                    */
/*   Labels :                                                              */
/*   Brief   Pace GUI Task                                                 */
/*=========================================================================*/
/*  Modifications                                                          */
/*-------------------------------------------------------------------------*/
/*  date    | author  |modification                                        */
/*----------|---------|----------------------------------------------------*/
/*17/08/2017| weifu.ma    Creat                                             */
/*----------|---------|----------------------------------------------------*/

/*=========================================================================*/
/*  Problems Report                                                        */
/*-------------------------------------------------------------------------*/
/*  date    | author  |modification                                        */
/*----------|---------|----------------------------------------------------*/
/***************************************************************************/


#include <stdio.h>
#include <string.h>
#include "pace_msg.h"
#include "pace_task.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

//#include "pace_uart.h"
#include "pace_hal_rtc.h"


/**************************************************************
  * @brief  gui_task_func: gui task function
  * @param  None
  * @retval None
***************************************************************/
static void gui_task_init( void )
{

}

void gui_task_func(void *paramter)
{
    printf("%s\n", __func__);
	pace_msg msg = NULL_MSG;

    DateTimeType date_time = {0};
    gui_task_init();
    while(1)
    {
       if (pace_receive_msg_from_queue(TASK_GUI, &msg) == PACE_SUCCESS)
        {
           switch (msg.msg_id)
            {
				case MSG_ID_GUI_UPDATE_TIME:
                    printf("GUI Task msg : MSG_ID_GUI_UPDATE_TIME\r\n");
                    date_time = pace_hal_get_date_time();
                    printf("GUI_time,%02d:%02d:%02d\n", date_time.RTC_Hours, date_time.RTC_Minutes, date_time.RTC_Seconds);
                    printf("GUI_date,%02d-%02d-%02d Week:%x\n", date_time.RTC_Year, date_time.RTC_Month, date_time.RTC_Date, date_time.RTC_WeekDay);
                    break;
				case MSG_ID_GUI_SET_TIME:
                    printf("GUI Task msg : MSG_ID_GUI_SET_TIME\r\n");
                    break;
                                
				default:
					break;
			}
        }

    }
}
