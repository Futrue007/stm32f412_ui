#ifndef _PACE_TASK_H_
#define _PACE_TASK_H_

#include "queue.h"
#include "task.h"
#include "FreeRTOS.h"

typedef enum
{   TASK_BEGIN = 0x00,
    TASK_GUI,
	TASK_TIME_SYS,
    TASK_AT_CMD,
    TASK_ID_MAX,
    TASK_NONE = 0xFF
} pace_task_id;


typedef struct
{
    TaskFunction_t task_func;
    char * task_name;
    uint16_t task_stack_depth;
    void *parameter;
    UBaseType_t task_priority;
    /*TaskHandle_t task_xHandle;*/
    pace_task_id  task_id;
    uint16_t task_msg_q_size;
}TASK_INFO_STRUCT;


typedef struct
{
   xTaskHandle        task_handle;
   xQueueHandle       task_queue_handle;
}TASK_Q_HANDLE_STRUCT;

void pace_create_tasks(void);


extern pace_task_id getCurrentTaskId(void);
extern xQueueHandle *get_pace_task_queue( pace_task_id task_id );

#endif
