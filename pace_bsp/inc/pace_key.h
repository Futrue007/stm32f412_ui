#ifndef _KEY_H
#define _KEY_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include "pace_bsp.h"

typedef enum {
    SEL_KEY = 0,
    KEY_MAX,
} key_id;


#define POWER_KEY_DOWN      MSG_ID_INPUT_KEY_PRESSDOWN
#define POWER_KEY_UP        MSG_ID_INPUT_KEY_PRESSUP
#define POWER_KEY_LONG      MSG_ID_INPUT_KEY_LONGPRESS
#define POWER_KEY_DOUBLE    MSG_ID_INPUT_KEY_DOUBULEPRESS


void key_init(void);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _KEY_H */
