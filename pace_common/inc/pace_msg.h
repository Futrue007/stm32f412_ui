#ifndef _PACE_MSG_H_
#define _PACE_MSG_H_

#include "FreeRTOS.h"
#include "semphr.h"
#include "pace_task.h"

//libin for deamon post task test 
#define PACE_DEAMON_POST_TEST 0


typedef enum
{
    PACE_FALSE,
    PACE_TRUE
}pace_bool;

typedef enum
{
  PACE_SUCCESS,
  PACE_FAIL,
  PACE_INVALID
}pace_status;

typedef enum
{
  PACE_CH_APP,
  PACE_CH_KERNEL,
//  PACE_INVALID
}pace_ch;



typedef enum {
    MSG_ID_INVALID_TYPE = 0,

    /*Protocol Task MSG ID*/
    MSG_ID_PROTOCOL_TASK_BEGIN = MSG_ID_INVALID_TYPE + 10,
    MSG_ID_PROTOCOL_SEND_DATA,
    MSG_ID_PROTOCOL_RECV_DATA,
    MSG_ID_PROTOCOL_TASK_END = MSG_ID_PROTOCOL_TASK_BEGIN + 100,

    /*Gui Task MSG ID*/
    MSG_ID_GUI_TASK_START,
    MSG_ID_GUI_UPDATE_TIME,
    MSG_ID_GUI_SET_TIME,
    //for deamon post task test 
    MSG_ID_GUI_DEAMON_CMD_TEST,
  
    /*TBD*/
    /*TBD*/
    MSG_ID_GUI_TASK_END = MSG_ID_GUI_TASK_START + 10,

    /*Time Task MSG ID*/
    MSG_ID_TIM_TASK_START,
    MSG_ID_TIM_INIT_RTC,/*RTC Init*/
    MSG_ID_TIM_SET_DATETIME,/*SET Date&TIME*/
    //MSG_ID_TIM_SET_TIME,/*Set Time*/
    MSG_ID_TIM_S_INTR,/*Second Interrupt*/
    MSG_ID_TIM_TASK_END = MSG_ID_TIM_TASK_START + 10,
		
		
		/*History Task MSG ID*/
    MSG_ID_HIS_TASK_START,
    MSG_ID_HIS_INIT_RTC,/*RTC Init*/
    MSG_ID_HIS_TASK_END = MSG_ID_TIM_TASK_START + 10,		
	
    /*AT Task MSG ID*/
    MSG_ID_AT_TASK_START,
    MSG_ID_AT_CMD,
    /*TBD*/
    MSG_ID_AT_TASK_END = MSG_ID_AT_TASK_START + 10,
    /*Demo Task MSG ID*/
    MSG_ID_DEMO_TASK_START,
    /*TBD*/
    /*TBD*/
    MSG_ID_DEMO_TASK_END = MSG_ID_DEMO_TASK_START + 50,
	/*Deamon Post MSG ID*/
    MSG_ID_DEAMON_POST_TASK_START,
    /*TBD*/
    /*TBD*/
    MSG_ID_DEAMON_RECV_DATA_FROM_SPI,
    //MSG_ID_DEAMON_POST_DATA_TO_BUSS,
    MSG_ID_DEAMON_POST_DATA_TO_UI,
    MSG_ID_DEAMON_POST_DATA_TO_PM,
    MSG_ID_DEAMON_POST_DATA_TO_SENSOR,
    MSG_ID_DEAMON_POST_DATA_TO_ALGO,
    MSG_ID_DEAMON_POST_DATA_TO_GPS,
    MSG_ID_DEAMON_POST_DATA_TO_TIME,
    MSG_ID_DEAMON_POST_DATA_TO_HIS,
    MSG_ID_DEAMON_POST_DATA_TO_DEAMON,
    MSG_ID_DEAMON_POST_TASK_END = MSG_ID_DEAMON_POST_TASK_START + 50,

    /*Deamon Recv MSG ID*/
    MSG_ID_DEAMON_RECV_TASK_START,
    /*TBD*/
    /*TBD*/
    //MSG_ID_DEAMON_RECV_DATA_FROM_BUSS,
    MSG_ID_DEAMON_RECV_DATA_FROM_UI,
    MSG_ID_DEAMON_RECV_DATA_FROM_PM,
    MSG_ID_DEAMON_RECV_DATA_FROM_SENSOR,
    MSG_ID_DEAMON_RECV_DATA_FROM_ALGO,
    MSG_ID_DEAMON_RECV_DATA_FROM_GPS,
    MSG_ID_DEAMON_RECV_DATA_FROM_TIME,
    MSG_ID_DEAMON_RECV_DATA_FROM_HIS,
    MSG_ID_DEAMON_RECV_DATA_FROM_DEAMON,
    
    MSG_ID_DEAMON_POST_DATA_TO_SPI,
    MSG_ID_DEAMON_RECV_TASK_END = MSG_ID_DEAMON_RECV_TASK_START + 50,

    
    MSG_ID_MAX
} pace_msg_id;


#define NULL_MSG   {TASK_NONE,TASK_NONE,MSG_ID_INVALID_TYPE,0,NULL,NULL}


/*message struct that sends common msg and data between tasks*/
typedef struct {
	pace_task_id src_task;
	pace_task_id dst_task;
	pace_msg_id msg_id;
    uint16_t length;
	void *buffer;
	xSemaphoreHandle hEvent;
} pace_msg;


/*only for spi daemon task*/
typedef struct {
	uint8_t ch_id;
	uint8_t buss_id;
	uint16_t length;
	void *buffer;/*msgpack or raw struct*/
} pace_dgram;


extern pace_status pace_post_msg_to_queue(pace_msg *msg);

extern pace_status pace_receive_msg_from_queue(pace_task_id taskid, pace_msg *msg);
#endif /* __PACE_RTC_H */
