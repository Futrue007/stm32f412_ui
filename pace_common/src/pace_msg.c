/***************************************************************************/
/*                                                           Date:08/2017  */
/*                        Pace message API                                 */
/*                                                   (c)copyright Pacewear */
/*-------------------------------------------------------------------------*/
/*   Author:  weifu.ma                                                      */
/*   Reference documents:                                                  */
/*-------------------------------------------------------------------------*/
/*  Comments:                                                              */
/*   File : pace_msg.c                                                     */
/*   Labels :                                                              */
/*   Brief :  Provides  API for send and receive messages                  */
/*=========================================================================*/
/*  Modifications                                                          */
/*-------------------------------------------------------------------------*/
/*  date    | author  |modification                                        */
/*----------|---------|----------------------------------------------------*/
/*17/08/2017| weifu.ma   Creat                                             */
/*----------|---------|----------------------------------------------------*/

/*=========================================================================*/
/*  Problems Report                                                        */
/*-------------------------------------------------------------------------*/
/*  date    | author  |modification                                        */
/*----------|---------|----------------------------------------------------*/
/***************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "pace_task.h"
#include "pace_msg.h"

#define MAX_TICK_TO_WAIT portMAX_DELAY

pace_status pace_post_msg_to_queue(pace_msg *msg)
{
  signed portBASE_TYPE xHigherPriorityTaskWoken;
  xQueueHandle p_hdl;
  uint32_t ipsr;

  msg->hEvent = (xSemaphoreHandle)xTaskGetTickCount();

  ipsr = __get_IPSR();

  if (msg->dst_task >= TASK_ID_MAX)
  {
    return PACE_INVALID;
  }
  p_hdl = get_pace_task_queue(msg->dst_task);

  if ( ipsr > 0 )
  {
    if (pdPASS == xQueueSendFromISR(p_hdl, msg, &xHigherPriorityTaskWoken))
    {
      if (xHigherPriorityTaskWoken) {
        taskYIELD();
      }
      return PACE_SUCCESS;
    }
  }
  else if (pdPASS == xQueueSend(p_hdl, msg, 0))
  {
    return PACE_SUCCESS;
  }
  return PACE_FAIL;
}



pace_status pace_receive_msg_from_queue(pace_task_id taskid, pace_msg *msg)
{
  if (xQueueReceive(get_pace_task_queue(taskid), msg, MAX_TICK_TO_WAIT) == pdPASS)
  {
    return PACE_SUCCESS;
  }
  return PACE_FAIL;
}
