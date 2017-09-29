#include "pace_timer.h"


#define PACE_TIMER_NO_DELAY  (( portTickType )0U)


static pace_timer pace_timers[MAX_TIMERS];

static void pace_os_timer_callback( xTimerHandle xTimer )
{
    pace_timer *p_timer = NULL;
    TimerFuncPtr fun_ptr = NULL;
    pace_timer_id timerid = (pace_timer_id)((uint32_t)pvTimerGetTimerID(xTimer));

    if ( timerid < MAX_TIMERS )
    {
        p_timer = &pace_timers[timerid];
        assert_param( NULL != p_timer );

        fun_ptr = p_timer->timerfuncPtr;
        
      if (fun_ptr != NULL)
        {  
            fun_ptr();
        }
    }
}

void Start_OS_TimerEx(pace_timer_id timerid, uint16_t delay, unsigned portBASE_TYPE uxAutoReload, TimerFuncPtr timerfuncPtr)
{
    pace_timer *p_timer = NULL;
    signed portBASE_TYPE xHigherPriorityTaskWoken;

    if (timerid < MAX_TIMERS)
    {
        p_timer = &pace_timers[timerid];

        if (p_timer->timer != NULL)
        {
            xTimerDelete(p_timer->timer, PACE_TIMER_NO_DELAY);
        }

        p_timer->timerfuncPtr = timerfuncPtr;
        p_timer->timer = xTimerCreate("OS_Timer", delay, uxAutoReload, (void *)timerid, pace_os_timer_callback);

        if (p_timer->timer != NULL)
        {
            if (__get_IPSR() > 0) {
                if (pdPASS ==  xTimerStartFromISR(p_timer->timer, &xHigherPriorityTaskWoken)) {
                    if (xHigherPriorityTaskWoken) {
                        taskYIELD();
                    }
                        
                }
            } else {
                xTimerStart(p_timer->timer, PACE_TIMER_NO_DELAY);
            }
        }
    }
}

void Start_OS_Timer(pace_timer_id timerid, uint16_t delay, TimerFuncPtr timerfuncPtr)
{
    Start_OS_TimerEx(timerid, delay, pdFALSE, timerfuncPtr);
}

void Stop_OS_Timer( pace_timer_id timerid )
{
    pace_timer *p_timer = NULL;

    if (timerid < MAX_TIMERS)
    {
        p_timer = &pace_timers[timerid];

        if (p_timer->timer != NULL)
        {
            xTimerDelete(p_timer->timer, PACE_TIMER_NO_DELAY);
            
            p_timer->timer = NULL;
            p_timer->timerfuncPtr = NULL;
        }
    }
}



