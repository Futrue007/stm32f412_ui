

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pace_msg.h"
#include "pace_task.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
//#include "pace_uart.h"
#include "pace_hal_rtc.h"

static void tim_sys_task_init( void )
{

}


/**************************************************************
  * @brief  timer_sys_taks_func: time task function
  * @param  None
  * @retval None
***************************************************************/
void timer_sys_taks_func(void *paramter)
{
    printf("%s\n", __func__);
    pace_msg msg = NULL_MSG;
    tim_sys_task_init();
 
    DateTimeType *set_datetime;
    while(1)
    {
       if (pace_receive_msg_from_queue(TASK_TIME_SYS, &msg) == PACE_SUCCESS)
        {

           switch (msg.msg_id)
            {
				case MSG_ID_TIM_INIT_RTC:
					 printf("Time task : MSG_ID_TIM_INIT_RTC\n");
                     pace_hal_rtc_init();
                    break;
                case MSG_ID_TIM_SET_DATETIME:
                    printf("Time task : MSG_ID_TIM_SET_DATE\n");
                    set_datetime = (DateTimeType *)msg.buffer;
                    pace_hal_set_date_time(set_datetime);
                    if(msg.buffer)
                        free(msg.buffer);
                    
                    break;


                case MSG_ID_TIM_S_INTR:
                    break;
				default:
					break;
			}
        
        }
    }
}


