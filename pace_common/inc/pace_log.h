#ifndef _PACE_LOG_H_
#define _PACE_LOG_H_
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "pace_bsp_uart.h"


#define pace_debug_mod unsigned long

#define  MOD_NONE  0x0000         /* none */
#define  MOD_BSP   0x0001         /* driver module */
#define  MOD_COM   0x0002         /* Common module */
#define  MOD_HAL   0x0004         /* HAL   module */
#define  MOD_APP   0x0008         /* APP   module */
#define  MOD_ALG   0x0010         /* algorithm module */
#define  MOD_PRO   0x0020         /* protocol module */
#define  MOD_GPS   0x0040         /* GPS module */
#define  MOD_HRM   0x0080         /* heart rate monitor module*/
/* You can add your module below */
#define  MOD_MCU (MOD_BSP|MOD_COM|MOD_HAL|MOD_APP|MOD_ALG|MOD_PRO)


#define PACE_LOG_BUF_SIZE 128

#define PACE_DEBUG

static pace_debug_mod pace_debug_cfg = MOD_MCU;

#define portUartlog_DELAY  500



#ifdef PACE_DEBUG

#define pace_debug_printf( mod_id, msg,...)   do { pace_uart_log( mod_id, msg, ##__VA_ARGS__ ); }while( 0 )

#define pace_bsp_printf(msg,...)   do { pace_uart_log( MOD_BSP, msg, ##__VA_ARGS__ ); }while( 0 )
#define pace_com_printf(msg,...)   do { pace_uart_log( MOD_COM, msg, ##__VA_ARGS__ ); }while( 0 )
#define pace_hal_printf(msg,...)   do { pace_uart_log( MOD_HAL, msg, ##__VA_ARGS__ ); }while( 0 )
#define pace_app_printf(msg,...)   do { pace_uart_log( MOD_APP, msg, ##__VA_ARGS__ ); }while( 0 )
#define pace_alg_printf(msg,...)   do { pace_uart_log( MOD_ALG, msg, ##__VA_ARGS__ ); }while( 0 )
#define pace_pro_printf(msg,...)   do { pace_uart_log( MOD_PRO, msg, ##__VA_ARGS__ ); }while( 0 )
#define pace_gps_printf(msg,...)   do { pace_uart_log( MOD_GPS, msg, ##__VA_ARGS__ ); }while( 0 )
#define pace_hrm_printf(msg,...)   do { pace_uart_log( MOD_HRM, msg, ##__VA_ARGS__ ); }while( 0 )

#else

#define pace_debug_printf( mod_id, msg,...)
#define pace_bsp_printf(msg,...)
#define pace_com_printf(msg,...)
#define pace_hal_printf(msg,...) 
#define pace_app_printf(msg,...)
#define pace_alg_printf(msg,...)
#define pace_pro_printf(msg,...)
#define pace_gps_printf(msg,...)
#define pace_hrm_printf(msg,...)

#endif

void pace_log_init(void);
void pace_uart_log( pace_debug_mod mod_id, const char *msg, ... );

#endif /*_PACE_LOG_H_*/
