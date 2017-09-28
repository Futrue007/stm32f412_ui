

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pace_msg.h"
#include "pace_task.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "pace_atcmd_task.h"
#include "pace_hal_rtc.h"





static at_cmd_func_status AT_SET_TIMEDATE(char *cmd_ext)
{
    pace_msg msg_intr= NULL_MSG;
    DateTimeType *time_date_tmp;
    char p[4]={0};
    time_date_tmp = (DateTimeType*)malloc(sizeof(time_date_tmp));
    memset(time_date_tmp,0,sizeof(DateTimeType));
    cmd_ext++;
    if(strcmp(cmd_ext,""))
    {
        /*Year : 2017*/
        memcpy(p,cmd_ext,4);
        time_date_tmp->RTC_Year = atoi(p);
        memset(p,0,sizeof(p));
        
        /*Month : 08*/
        cmd_ext+=4;
        memcpy(p,cmd_ext,2);
        time_date_tmp->RTC_Month = atoi(p);
        memset(p,0,sizeof(p));
        
        /*Date : 25*/
        cmd_ext+=2;
        memcpy(p,cmd_ext,2);
        time_date_tmp->RTC_Date = atoi(p);
        memset(p,0,sizeof(p));
        
        /*Week : 5*/
        cmd_ext+=2;
        memcpy(p,cmd_ext,1);
        time_date_tmp->RTC_WeekDay = atoi(p); 

        /*Hour: 15*/
        cmd_ext++;
        memcpy(p,cmd_ext,2);
        time_date_tmp->RTC_Hours = atoi(p);

        /*Minute: 24*/
        cmd_ext+=2;
        memcpy(p,cmd_ext,2);
        time_date_tmp->RTC_Minutes = atoi(p);

        /*second: 00*/
        cmd_ext+=2;
        memcpy(p,cmd_ext,2);
        time_date_tmp->RTC_Seconds = atoi(p);    

        msg_intr.msg_id = MSG_ID_TIM_SET_DATETIME;
        msg_intr.src_task = getCurrentTaskId();
        msg_intr.dst_task = TASK_TIME_SYS;
        msg_intr.buffer = (void *)time_date_tmp;

        pace_post_msg_to_queue(&msg_intr);



    }
    return AT_OK;
}


static at_cmd_func_status AT_GET_DATETIME(char *cmd_ext)
{
    DateTimeType date_time = {0};

    date_time=pace_hal_get_date_time();
    printf("AT_time,%02d:%02d:%02d\n", date_time.RTC_Hours, date_time.RTC_Minutes, date_time.RTC_Seconds);
    printf("AT_date,%02d-%02d-%02d Week:%x\n", date_time.RTC_Year, date_time.RTC_Month, date_time.RTC_Date, date_time.RTC_WeekDay);

    return AT_OK;
}



static at_cmd_func_status AT_GET_TIME_STAMP(char *cmd_ext)
{
    uint32_t tmp = 0;
    tmp = pace_convert_current_time_to_timestamp();
    printf("AT second %d\n", tmp);

    return AT_OK;
}


static at_cmd_func_status AT_SET_DATE_TIME_BY_STAMP(char *cmd_ext)
{
    uint32_t tmp = 0;
    calendar_time_t calendar_time = {0};

    cmd_ext++;
    if(strcmp(cmd_ext,""))
    {
        tmp = atoi(cmd_ext);
        printf("AT receive timestamp %d\n", tmp);
        calendar_time =  pace_convert_timestamp_to_calendar_time(tmp);

        printf("AT GET timestamp Time %d-%d-%d %d:%d:%d week =%d\n", calendar_time.year,calendar_time.month,calendar_time.day,calendar_time.hour,calendar_time.minutes,calendar_time.seconds,calendar_time.week);


        }
    return AT_OK;
}

static const at_cmd_func at_cmd_func_table[ ] =
{
    /** TIME AT CMD begin**/
    {"AT+SETTIME", AT_SET_TIMEDATE},
    {"AT+GETTIME", AT_GET_DATETIME},
    {"AT+GETSECOND", AT_GET_TIME_STAMP},
    {"AT+SETSECOND", AT_SET_DATE_TIME_BY_STAMP},
    /** TIME AT CMD end**/
    {NULL, NULL}
};


char* pace_strchange(char *str)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

	int i = 0;
	int length = strlen(str);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	for(i=0; i< length; i++)
	{
		if('='==str[i])
		{
			break;
		}
		if((str[i]<='z')&&(str[i]>='a'))
		{
			str[i] = str[i] - 32;
		}
	}

    return str;

}

int at_cmd_parse(char *atcmdstr)
{
        /*----------------------------------------------------------------*/
        /* Local Variables                                                */
        /*----------------------------------------------------------------*/
		char *cmdString = NULL;
		char *func_para = NULL;
		int i;
		int length;
        /*----------------------------------------------------------------*/
        /* Code Body                                                      */
        /*----------------------------------------------------------------*/
		pace_strchange(atcmdstr);
		if(strcmp(atcmdstr, "AT") == 0)
		{
			printf("AT CMD OK");
			printf("\r\n");
			return 0;
		}

		for (i = 0 ; at_cmd_func_table[i].cmd_string != NULL; i++ )
		{
			cmdString = at_cmd_func_table[i].cmd_string;
			length = strlen(cmdString);
			if (strncmp(atcmdstr, cmdString, length) == 0)
			{
				func_para = atcmdstr + length;
				return at_cmd_func_table[i].cmd_func(func_para);
			}
		}

        return 0;
}



/**************************************************************
  * @brief  atcmd_task_func: atcmd task function
  * @param  None
  * @retval None
***************************************************************/
void atcmd_task_func(void *paramter)
{
    pace_msg msg = NULL_MSG;
    char r_buffer[200] = {0};
    uint16_t len = 0;
    printf("%s\n", __func__);
	while (1)
	{
       if (pace_receive_msg_from_queue(TASK_AT_CMD, &msg) == PACE_SUCCESS)
        {
           if (msg.msg_id == MSG_ID_AT_CMD) 
           {
            memset(r_buffer,0,sizeof(r_buffer));
            memcpy(r_buffer, (char *)msg.buffer, msg.length);
            len = msg.length;
            if (len > 0) 
             {
                printf("cmd: ");
                printf("%s",r_buffer);
                printf("\r\n");
                at_cmd_parse(r_buffer);
             
            }

                   

            }
        }
    }
}


