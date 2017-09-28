/***************************************************************************/
/*                                                           Date:08/2017  */
/*                        Pace LOG API                                    */
/*                                                   (c)copyright Pacewear */
/*-------------------------------------------------------------------------*/
/*   Author:  weifuma                                                      */
/*   Reference documents:                                                  */
/*-------------------------------------------------------------------------*/
/*  Comments:                                                              */
/*   File : pace_log.c                                                    */
/*   Labels :                                                              */
/*   Brief :  Log API                                          */
/*=========================================================================*/
/*  Modifications                                                          */
/*-------------------------------------------------------------------------*/
/*  date    | author  |modification                                        */
/*----------|---------|----------------------------------------------------*/
/*29/08/2017| weifu.ma   Creat                                              */
/*----------|---------|----------------------------------------------------*/

/*=========================================================================*/
/*  Problems Report                                                        */
/*-------------------------------------------------------------------------*/
/*  date    | author  |modification                                        */
/*----------|---------|----------------------------------------------------*/
/***************************************************************************/

#include "pace_log.h"
#ifdef PACE_DEBUG
#include "FreeRTOS.h"
#include "semphr.h"
static char *p_log__buf = NULL;
static uint8_t pace_log_init_flag = 0;

typedef struct
{
    pace_debug_mod module;
    const char *mod_str;
}pace_debug_str;


static const pace_debug_str pace_debug_str_array[] =
{
    { MOD_BSP, "BSP" },
    { MOD_COM, "COMMON" },
    { MOD_HAL, "HAL" },
    { MOD_APP, "APP" },
    { MOD_ALG, "ALG" },
    { MOD_PRO, "PROTOCOL" },
    { MOD_GPS, "GPS" },
    { MOD_HRM, "HRM" }
};


static uint8_t pace_add_log_module_str( char *p_msg, pace_debug_mod mod_id );
static uint8_t pace_get_debug_str_num( void );
static xSemaphoreHandle pMuxUartLog    = NULL;



void pace_log_init(void)
{
    pace_uart2_init();
    p_log__buf = (char *)malloc( PACE_LOG_BUF_SIZE );
    assert_param( NULL != p_log__buf );
    memset( p_log__buf, 0, PACE_LOG_BUF_SIZE );
    pace_log_init_flag = 1;
    pMuxUartLog = xSemaphoreCreateMutex();


}


static uint8_t pace_get_debug_str_num( void )
{
    return sizeof(pace_debug_str_array)/sizeof(pace_debug_str);
}

const pace_debug_str * pace_get_debug_str( uint8_t idx )
{
    const pace_debug_str *p_debug = NULL;

    if (idx < pace_get_debug_str_num()) {
        p_debug = &pace_debug_str_array[idx];
    }

    return p_debug;
}


static uint8_t pace_add_log_module_str( char *p_msg, pace_debug_mod mod_id )
{
    const pace_debug_str * p_debug = NULL;
    uint8_t str_len = 0;
    uint8_t i = 0;
    uint8_t num = pace_get_debug_str_num();

    assert_param( NULL != p_msg );

    p_msg[0] = '[';
    for (i = 0; i < num; i++) 
    {
        p_debug = pace_get_debug_str( i );
        if ((mod_id&p_debug->module) != 0) {
            strcat( p_msg, p_debug->mod_str );
            strcat( p_msg, "]");
        }
    }
    str_len = strlen( p_msg );
    p_msg[str_len] = ':';

    return str_len+1;
}


void pace_uart_log( pace_debug_mod mod_id, const char *msg, ... )
{
    char *p_msg = p_log__buf;
    uint8_t str_len = 0;
    va_list trace_p;

    if ( !pace_log_init_flag ||(mod_id&pace_debug_cfg) == MOD_NONE ) {
        return;
    }
    assert_param( NULL != p_msg );
    if (__get_IPSR() != 0) {
        return;
    }
	if (xSemaphoreTake(pMuxUartLog, (portTickType) portUartlog_DELAY) == pdTRUE) {   
        memset( p_msg, 0, PACE_LOG_BUF_SIZE );
        str_len = pace_add_log_module_str(p_msg, mod_id);
        va_start(trace_p, msg);
        vsnprintf(p_msg+str_len, PACE_LOG_BUF_SIZE-str_len, msg, trace_p);
        va_end(trace_p);
        printf( (const char *)p_msg ); 
        xSemaphoreGive(pMuxUartLog);
   }
}
#endif

