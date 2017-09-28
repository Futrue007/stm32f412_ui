/***************************************************************************/
/*                                                           Date:08/2017  */
/*                        Pace Task API                                    */
/*                                                   (c)copyright Pacewear */
/*-------------------------------------------------------------------------*/
/*   Author:  weifuma                                                      */
/*   Reference documents:                                                  */
/*-------------------------------------------------------------------------*/
/*  Comments:                                                              */
/*   File : pace_task.c                                                    */
/*   Labels :                                                              */
/*   Brief :  Create a task&queue                                          */
/*=========================================================================*/
/*  Modifications                                                          */
/*-------------------------------------------------------------------------*/
/*  date    | author  |modification                                        */
/*----------|---------|----------------------------------------------------*/
/*17/08/2017| weifu.ma   Creat                                              */
/*----------|---------|----------------------------------------------------*/

/*=========================================================================*/
/*  Problems Report                                                        */
/*-------------------------------------------------------------------------*/
/*  date    | author  |modification                                        */
/*----------|---------|----------------------------------------------------*/
/***************************************************************************/




#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "pace_task.h"
#include "queue.h"
#include "pace_msg.h"

extern void gui_task_func(void *parameter);
extern void atcmd_task_func(void *parameter);
extern void timer_sys_taks_func(void *parameter);


TASK_Q_HANDLE_STRUCT pace_taskid_to_handle[TASK_ID_MAX];



/************  Pace Task List Begin   *************/

static const TASK_INFO_STRUCT pace_tasks_list[] = {
{ gui_task_func,      "GUI",     configMINIMAL_STACK_SIZE + 0, NULL, tskIDLE_PRIORITY + 5, TASK_GUI, 10},
{ timer_sys_taks_func, "TIMESYS", configMINIMAL_STACK_SIZE+ 0, NULL, tskIDLE_PRIORITY + 6,  TASK_TIME_SYS, 10 },
{ atcmd_task_func,     "ATCMD",   configMINIMAL_STACK_SIZE +0, NULL, tskIDLE_PRIORITY + 2,  TASK_AT_CMD, 10 },
};

/************  Pace Task List End   *************/

pace_task_id getCurrentTaskId(void)
{
  return __get_IPSR() > 0 ? TASK_NONE : ((pace_task_id)(uint32_t)xTaskGetCurrentTaskId());
}

xQueueHandle *get_pace_task_queue( pace_task_id task_id )
{
    TASK_Q_HANDLE_STRUCT *p_task_handle = NULL;
    assert_param( task_id < TASK_ID_MAX );

    p_task_handle = &pace_taskid_to_handle[task_id];

    return p_task_handle->task_queue_handle;
}

static void pace_create_task_queue( const TASK_INFO_STRUCT *p_tasks_array, int task_num )
{
    printf("create tasks begin...\r\n");
    int i = 0;
    BaseType_t result;

    const TASK_INFO_STRUCT *p_task = NULL;
    TASK_Q_HANDLE_STRUCT *p_task_handle = NULL;

    assert_param( NULL != p_tasks_array );
    assert_param( task_num < TASK_ID_MAX );
    
    memset( (void *)pace_taskid_to_handle, 0, sizeof(pace_taskid_to_handle) );
    printf("task_num=%d\n", task_num);

    for ( i = 0; i < task_num; i++ )
    {
        p_task = &(p_tasks_array[i]);
        assert_param( NULL != p_task );
        printf("p_task->task_id=%d\r\n", p_task->task_id);
        p_task_handle = &pace_taskid_to_handle[p_task->task_id];
        assert_param( NULL != p_task_handle );

        result = xTaskCreate(   p_task->task_func,
                                p_task->task_name,
                                p_task->task_stack_depth,
                                p_task->parameter,
                                p_task->task_priority,
                                &(p_task_handle->task_handle) );
		
        vTaskSetTaskId(p_task_handle->task_handle, p_task->task_id);
            
        if ( pdPASS != result ) {
			printf("\r\nfailed to create %s\r\n", p_task->task_name);
			printf("free heap size : %d\r\n", xPortGetFreeHeapSize());
			return;
		}


        assert_param( NULL != p_task_handle );

        if ( p_task->task_msg_q_size != 0 )
        {
            printf("p_task->task_msg_q_size =%d\n", p_task->task_msg_q_size);
            p_task_handle->task_queue_handle = xQueueCreate( p_task->task_msg_q_size, sizeof(pace_msg));
        }
    }
    printf("create tasks finish\r\n");

}

void pace_create_tasks(void)
{
	pace_create_task_queue( pace_tasks_list, sizeof(pace_tasks_list)/sizeof(TASK_INFO_STRUCT) );
}



