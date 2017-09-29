/*****************************************************************************/

#include "pace_key.h"
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "pace_log.h"
#include "pace_timer.h"
#include "pace_msg.h"
#include "pace_task.h"

TIM_HandleTypeDef    Tim2Handle;
TIM_HandleTypeDef    Tim3Handle;

#define LONG_PRESS_DELAY        700    /* Long press delay in milliseconds. */
#define DOWN_PRESS_MONITOR      52     /* The time(milliseconds) of filtering down key press */
#define DOUBLE_KEY_INTERAL      500

#define DOWN_PRESS_TIM      (DOWN_PRESS_MONITOR * 2)
#define LONG_PRESS_TIM      (LONG_PRESS_DELAY * 2)

#define POWER_KEY_TIMER     TIM2

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif


typedef enum {
    KEY_DOWN = 0,
    KEY_UP
} KeyState;



struct key_msg {
    pace_msg_id msg_down;
    pace_msg_id msg_up;
    pace_msg_id msg_long;
    pace_msg_id msg_double;
};

struct key_strut {
    int id;
    KeyState state;
    uint8_t isValidDownKey; /* Deal with key jitter. */
    uint8_t tras; /* Key trasaction */

    TIM_TypeDef *timx;
    struct key_msg kmsg; /* Key message to UI. */
    int click_times; /* Click times for double press use. */
};


static struct key_strut key_inst[KEY_MAX] = 
{
    {SEL_KEY, KEY_UP, FALSE, FALSE, POWER_KEY_TIMER, {POWER_KEY_DOWN, POWER_KEY_UP, POWER_KEY_LONG, POWER_KEY_DOUBLE}, 0},
};

void key_input_event(pace_msg_id type)
{
	pace_msg msg = NULL_MSG;
    
	msg.msg_id = type;
	msg.dst_task = TASK_GUI;
    msg.src_task = getCurrentTaskId();
    pace_post_msg_to_queue(&msg);

}



void key_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    /*PA0 JOY SEL*/
	/* Enable GPIOA clock */
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStructure.Pin =  GPIO_PIN_0;
	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Enable and set EXTI lines 0 Interrupt to the lowest priority */
	HAL_NVIC_SetPriority(EXTI0_IRQn, PP_KEY, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}


/* Call back from the STM32 Cube lib */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == GPIO_PIN_0)
	{
	    uint8_t gpio_ret = RESET;
	    
	    gpio_ret = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
        printf("gpio_ret = %d\n", gpio_ret);
        if ((gpio_ret == GPIO_PIN_SET) && (key_inst[SEL_KEY].state == KEY_UP)) {// key down
	        __HAL_TIM_SET_COUNTER(&Tim2Handle, DOWN_PRESS_TIM * 10);// start 100ms tim
	        __HAL_TIM_CLEAR_IT(&Tim2Handle, TIM_FLAG_UPDATE);
	        __HAL_TIM_ENABLE(&Tim2Handle);
            key_inst[SEL_KEY].state = KEY_DOWN;

            printf("Start TIM2 tick: %ld\n", HAL_GetTick());
        } else {  //key up
            printf("Disable TIM2 tick: %ld\n", HAL_GetTick());
            __HAL_TIM_DISABLE(&Tim2Handle);
            __HAL_TIM_CLEAR_IT(&Tim2Handle, TIM_FLAG_UPDATE);
//            key_input_event(key_inst[SEL_KEY].kmsg.msg_up);

	        if (key_inst[SEL_KEY].state == KEY_DOWN) {
	            if (key_inst[SEL_KEY].isValidDownKey == TRUE) {
	               if (key_inst[SEL_KEY].tras) {
	                    key_inst[SEL_KEY].tras = FALSE;
                        printf("send msg_up tick: %ld\n", HAL_GetTick());
	                    key_input_event(key_inst[SEL_KEY].kmsg.msg_up);
	                }

	                key_inst[SEL_KEY].isValidDownKey = FALSE;
	            }

	            key_inst[SEL_KEY].state = KEY_UP;
	        }

        }


    }
}





/**
  * @brief  TIM2 initialize.
  * @param  None
  * @retval None
  */
static void tim2_init(void)
{
    /* Compute the prescaler value to have TIMx counter clock equal to 10K Hz */
    /*In this example TIM2 input clock (TIM2CLK)  is set to APB1 clock (PCLK1) x1,
    since APB1 prescaler is equal to 4.
    TIM2CLK = PCLK1*1
    PCLK1 = HCLK/1
    => TIM2CLK = HCLK/1 = SystemCoreClock/1
    To get TIM2 counter clock at 10 KHz, the Prescaler is computed as follows:
    Prescaler = (TIM2CLK / TIM2 counter clock) - 1
    Prescaler = ((SystemCoreClock/1) /10 KHz) - 1
    TIM2 counter clock = TIM2CLK/(Prescaler +1)
    SystemCoreClock is set to 180 MHz for STM32F4xx Devices.

    The TIM2 ARR register value is equal to 10000 - 1, 
    Update rate = TIM2 counter clock / (Period + 1) = 1 Hz,

    so Update rate = TIM2CLK/((Prescaler +1) *(Period + 1))
    So the TIM3 generates an interrupt each 1 s
    */
	uint32_t uwPrescalerValue = (uint32_t)((SystemCoreClock / 1) / 10000) - 1;
    pace_bsp_printf("%s SystemCoreClock =%d\n", __func__, SystemCoreClock);
	/* Set TIMx instance */
	Tim2Handle.Instance = TIM2;

	/* Initialize TIM2 peripheral as follows:
	 + Period = 10000 - 1
	 + Prescaler = ((SystemCoreClock / 2)/10000) - 1
	 + ClockDivision = 0
	 + Counter direction = Up
	*/
	Tim2Handle.Init.Period			 = 0xFFFFFFFF - 1;
	Tim2Handle.Init.Prescaler		 = uwPrescalerValue;//  TIM CLK 10KHZ  >>  100us
	Tim2Handle.Init.ClockDivision	 = 0;
	Tim2Handle.Init.CounterMode		 = TIM_COUNTERMODE_DOWN;
	Tim2Handle.Init.RepetitionCounter = 0;

	if (HAL_TIM_Base_Init(&Tim2Handle) != HAL_OK) {
		/* Initialization Error */
        Error_Handler();
	}
  //  if (HAL_TIM_Base_Start_IT(&Tim2Handle) != HAL_OK) {
        /* Starting Error */
  //      Error_Handler();
 //   }

	__HAL_TIM_CLEAR_IT(&Tim2Handle, TIM_FLAG_UPDATE);
	__HAL_TIM_ENABLE_IT(&Tim2Handle, TIM_IT_UPDATE);
	__HAL_TIM_CLEAR_IT(&Tim2Handle, TIM_FLAG_UPDATE);
}

/**
  * @brief  TIM3 initialize.
  * @param  None
  * @retval None
  */
static void tim3_init(void)
{
	uint32_t uwPrescalerValue = (uint32_t)((SystemCoreClock / 1) / 10000) - 1;
    pace_bsp_printf("%s SystemCoreClock =%d\n", __func__, SystemCoreClock);
	/* Set TIMx instance */
	Tim3Handle.Instance = TIM3;

	Tim3Handle.Init.Period			 = 0xFFFFFFFF - 1;
	Tim3Handle.Init.Prescaler		 = uwPrescalerValue;//  TIM CLK 10KHZ  >>  100us
	Tim3Handle.Init.ClockDivision	 = 0;
	Tim3Handle.Init.CounterMode		 = TIM_COUNTERMODE_DOWN;
	Tim3Handle.Init.RepetitionCounter = 0;

	if (HAL_TIM_Base_Init(&Tim3Handle) != HAL_OK) {
		/* Initialization Error */
        Error_Handler();
	}

	__HAL_TIM_CLEAR_IT(&Tim3Handle, TIM_FLAG_UPDATE);
	__HAL_TIM_ENABLE_IT(&Tim3Handle, TIM_IT_UPDATE);
	__HAL_TIM_CLEAR_IT(&Tim3Handle, TIM_FLAG_UPDATE);
}

/**
  * @brief TIM MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  * @param htim: TIM handle pointer
  * @retval None
  */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (TIM2 == htim->Instance) {
        /*##-1- Enable peripheral clock #################################*/
        /* TIMx Peripheral clock enable */
        __HAL_RCC_TIM2_CLK_ENABLE();

        /*##-2- Configure the NVIC for TIMx ########################################*/
        /* Set the TIMx priority */
        HAL_NVIC_SetPriority(TIM2_IRQn, PP_KEY, 0);

        /* Enable the TIMx global Interrupt */
        HAL_NVIC_EnableIRQ(TIM2_IRQn);
        
    } else if (TIM3 == htim->Instance) {
        /*##-1- Enable peripheral clock #################################*/
        /* TIMx Peripheral clock enable */
        __HAL_RCC_TIM3_CLK_ENABLE();

        /*##-2- Configure the NVIC for TIMx ########################################*/
        /* Set the TIMx priority */
        HAL_NVIC_SetPriority(TIM3_IRQn, PP_KEY, 0);

        /* Enable the TIMx global Interrupt */
        HAL_NVIC_EnableIRQ(TIM3_IRQn);
    }

}

/**
  * @brief  callback for double timer.
  * @param  None
  * @retval None
  */
static void double_filter(void)
{
    key_inst[SEL_KEY].click_times = 0;
}


/**
  * @brief  Period elapsed callback in non blocking mode
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (TIM2 == htim->Instance) {
		__HAL_TIM_DISABLE(htim);
        printf("isValidDownKey=%d tick: %ld\n",key_inst[SEL_KEY].isValidDownKey, HAL_GetTick());
		if (key_inst[SEL_KEY].isValidDownKey == FALSE) {
            key_inst[SEL_KEY].tras = TRUE;
           // key_input_event(key_inst[SEL_KEY].kmsg.msg_down);
            /* If it is the first press, start a timer to deal with the next step double or long press. */
           
            printf("click_times=%d tick: %ld\n",key_inst[SEL_KEY].click_times, HAL_GetTick());

           
            if (++key_inst[SEL_KEY].click_times == 1) {
                //Stop_OS_Timer(TIMER_ID_KEY);
                //Start_OS_Timer(TIMER_ID_KEY, DOUBLE_KEY_INTERAL, double_filter);
                __HAL_TIM_SET_COUNTER(&Tim3Handle, DOUBLE_KEY_INTERAL * 10);// start 500ms tim2
                __HAL_TIM_CLEAR_IT(&Tim3Handle, TIM_FLAG_UPDATE);
                __HAL_TIM_ENABLE(&Tim3Handle);
                printf("Start TIM3(500) tick: %ld\n", HAL_GetTick());
            }

            /* To analyse if the key is the double click or not. */
            if (key_inst[SEL_KEY].click_times == 2) {
                 printf("send msg_double  tick: %ld\n", HAL_GetTick());
                 key_input_event(key_inst[SEL_KEY].kmsg.msg_double);
            } else {
                printf("send msg_down tick: %ld\n", HAL_GetTick());
                key_input_event(key_inst[SEL_KEY].kmsg.msg_down);
                __HAL_TIM_SET_COUNTER(htim, (LONG_PRESS_TIM - DOWN_PRESS_TIM) * 10);
                __HAL_TIM_ENABLE(htim);
                printf("enable Tim * tick: %ld\n", HAL_GetTick());

            }

			key_inst[SEL_KEY].isValidDownKey = TRUE;

		} else {
			/* Long press event */
			if (key_inst[SEL_KEY].tras) {
                printf("send msg_long  tick: %ld\n", HAL_GetTick());
				key_input_event(key_inst[SEL_KEY].kmsg.msg_long);
			}
		}
	} else if (TIM3 == htim->Instance) {
        __HAL_TIM_DISABLE(htim);
        printf("TIM3 Callback tick: %ld\n", HAL_GetTick());
        double_filter();
		
	}
}

/**
  * @brief  Key initialize.
  * @param  None
  * @retval None
  */
void key_init(void)
{
    key_gpio_init();
    tim2_init();
    tim3_init();
}




