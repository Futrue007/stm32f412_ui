/*****************************************************************************/

#include "pace_key.h"
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "pace_log.h"

TIM_HandleTypeDef    Tim2Handle;

#define LONG_PRESS_DELAY        700    /* Long press delay in milliseconds. */
#define DOWN_PRESS_MONITOR      52     /* The time(milliseconds) of filtering down key press */
#define DOUBLE_KEY_INTERAL      500

//#define DOWN_PRESS_TIM      (DOWN_PRESS_MONITOR * 2)
//#define LONG_PRESS_TIM      (LONG_PRESS_DELAY * 2)
#define DOWN_PRESS_TIM (100)   /* The time(milliseconds) of filtering down key press */
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
        if (gpio_ret == GPIO_PIN_SET) {// key down
	        __HAL_TIM_SET_COUNTER(&Tim2Handle, DOWN_PRESS_TIM * 10);
	        __HAL_TIM_CLEAR_IT(&Tim2Handle, TIM_FLAG_UPDATE);
	        __HAL_TIM_ENABLE(&Tim2Handle);
              printf("Start TIM2 tick: %ld\n", HAL_GetTick());
        } else {  //key up
              printf("Disable TIM2 tick: %ld\n", HAL_GetTick());
	        __HAL_TIM_DISABLE(&Tim2Handle);
	        __HAL_TIM_CLEAR_IT(&Tim2Handle, TIM_FLAG_UPDATE);

        }


    }
}





/**
  * @brief  TIM initialize.
  * @param  None
  * @retval None
  */
static void tim_init(void)
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
	//HAL_TIM_Base_Start_IT(&Tim2Handle);
	__HAL_TIM_ENABLE_IT(&Tim2Handle, TIM_IT_UPDATE);
	__HAL_TIM_CLEAR_IT(&Tim2Handle, TIM_FLAG_UPDATE);

  

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
        
    }

}


/**
  * @brief  Period elapsed callback in non blocking mode
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (TIM2 == htim->Instance) {
        printf("Time elapsed tick: %ld\n", HAL_GetTick());

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
    tim_init();
}




