#ifndef _PACE_BSP_H
#define _PACE_BSP_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "stm32f4xx.h"
#include "pace_bsp_def.h"

#include "freeRTOSConfig.h"

  /* IRQ priority to mask before to erase sectors in bank1 of internal flash */
#define __PP_WWDG       0x02
#define IRQ_PRIO_MASK_MOST_IRQ \
                ( (__PP_WWDG + 1)  << (8 - __NVIC_PRIO_BITS) )
#define IRQ_PRIO_MASK_TO_ERASE_INT_FLASH_BANK1  IRQ_PRIO_MASK_MOST_IRQ



//end 

typedef enum {
    PP_WWDG = __PP_WWDG, /* Watch dog */
    PP_VSYNC = 0x04,
    PP_CSR_WAKE_UP = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 0x01,
    PP_I2C,
    PP_CSR_UART,
    PP_SPORT_TRACKER,
    PP_A_G,
    PP_TP,
    PP_KEY,
    PP_RTC,
    PP_CHARGING,
    PP_AT_PC,
    PP_MAX = 0x0F,
} PreemptionPriority;

typedef enum {
    BOOT_UP_NULL = 0,       /* Power on reset */
    BOOT_UP_BY_KEY,         /* Boot up by key */
    BOOT_UP_BY_SW,          /* Software reset */
    BOOT_UP_BY_IWDG,        /* Independent watchdog */
    BOOT_UP_BY_WWDG,        /* Window watchdog */
    BOOT_UP_BY_CHARGING,    /* Boot up by charging */
    BOOT_UP_MAX_MODE,
} BootUpTypeEnum;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
