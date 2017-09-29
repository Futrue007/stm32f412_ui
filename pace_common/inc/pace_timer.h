#ifndef _PACE_TIMER_H_
#define _PACE_TIMER_H_
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

typedef enum
{
    TIMER_ID_LCD_SLEEP,
    TIMER_ID_KEY,
    /*ADD Your Timer ID Before This Line */
    MAX_TIMERS
}pace_timer_id;


#ifndef TimerFuncPtr
typedef void (*TimerFuncPtr) (void);
#endif



typedef struct
{
    xTimerHandle timer;
    TimerFuncPtr timerfuncPtr;
} pace_timer;


extern void Start_OS_TimerEx(pace_timer_id timerid, uint16_t delay, unsigned portBASE_TYPE uxAutoReload, TimerFuncPtr timerfuncPtr);
extern void Start_OS_Timer(pace_timer_id timerid, uint16_t delay, TimerFuncPtr timerfuncPtr);
extern void Stop_OS_Timer(pace_timer_id timerid);


#endif /*_PACE_TIMER_H_*/
