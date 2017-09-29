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

#include "pace_log.h"
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
                case MSG_ID_INPUT_KEY_PRESSDOWN:
                 //   SRQC_increment_counters(SRQCE_COUNTER_SHORT_KEY_PRESS);
                    pace_app_printf("[Gui_task] Hard Key Down \r\n");
                  //  gui_ecec_key_handler(msg.msg_id,0);
                    break;

                case MSG_ID_INPUT_KEY_LONGPRESS:
                //  SRQC_increment_counters(SRQCE_COUNTER_LONG_KEY_PRESS);
                  pace_app_printf("[Gui_task] Hard Key Long \r\n");
                //  if (MMI_POWERON_CHARGE==mmi_get_poweron_type()) {
				 // 	LCD_Close();//youzhenfu 20170103 for bug3882089
                 //   shex_power_off(SHEXE_POWER_OFF_POWER_ON_FROM_CHARGE_MODE);
                 // }
                 // gui_ecec_key_handler(msg.msg_id,0);
                  break;
                case MSG_ID_INPUT_KEY_DOUBULEPRESS:
                //  SRQC_increment_counters(SRQCE_COUNTER_LONG_KEY_PRESS);
                  pace_app_printf("[Gui_task] Hard Key Doubule \r\n");
                //  if (MMI_POWERON_CHARGE==mmi_get_poweron_type()) {
				 // 	LCD_Close();//youzhenfu 20170103 for bug3882089
                 //   shex_power_off(SHEXE_POWER_OFF_POWER_ON_FROM_CHARGE_MODE);
                 // }
                 // gui_ecec_key_handler(msg.msg_id,0);
                  break;
                case MSG_ID_INPUT_KEY_PRESSUP:
                  pace_app_printf("[Gui_task] Hard Key up \r\n");
                //  gui_ecec_key_handler(msg.msg_id,0);
                 // Gui_Input_Unlock();
                  break;
				default:
					break;
			}
        }

    }
}
