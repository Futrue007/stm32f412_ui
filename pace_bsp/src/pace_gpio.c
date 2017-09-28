/*****************************************************************************/
/*                                                            Date : 10/2017 */
/*                            PRESENTATION                                   */
/*              Copyright (c) 2006 PACEWEAR, Inc.                            */
/*****************************************************************************/
/*                                                                           */
/*    This material is company confidential, cannot be reproduced in any     */
/*    form without the written permission of JRD Communications, Inc.        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*   Author :  weifu ma                                                       */
/*   Role :                                                                  */
/*   Reference documents: GPIOS' configuration and initialization for board. */
/*---------------------------------------------------------------------------*/
/* Comments :                                                                */
/*     file    : F412ZGT6-dis\pace_bsp\src\pace_gpio.c                       */
/*     Labels  :                                                             */
/*===========================================================================*/
/* Modifications   (month/day/year)                                          */
/*---------------------------------------------------------------------------*/
/* date    | author    | modification                                        */
/*---------+-----------+-----------------------------------------------------*/
/*         |           |                                                     */
/*===========================================================================*/
/* Problems Report                                                           */
/*---------------------------------------------------------------------------*/
/* date    | author    | PR #     |                                          */
/*---------|-----------|----------|------------------------------------------*/
/*         |           |          |                                          */
/*---------|-----------|----------|------------------------------------------*/
/*         |           |          |                                          */
/*===========================================================================*/

#include "main.h"
#include "pace_gpio.h"


#define CONFIG_UNUSED_GPIO     1 /* Config unused GPIOs */

#define CONFIG_UNUSED_GPIO_OUTPUT_LOW     0

#if CONFIG_UNUSED_GPIO
/**
  * @brief  Configure & init unused of the gpio.
  * @param  None
  * @retval None
  */
static void pace_gpio_unused_cfg(void)
{
#if 0
	GPIO_InitTypeDef GPIO_InitStructure;

	__HAL_RCC_GPIOA_CLK_ENABLE();	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();	
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOI_CLK_ENABLE();

	 /* Configure and initial the GPIO pin */
#if CONFIG_UNUSED_GPIO_OUTPUT_LOW
	GPIO_InitStructure.Mode    = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull    = GPIO_NOPULL;
	GPIO_InitStructure.Speed   = GPIO_SPEED_FREQ_LOW;
#else
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed   = GPIO_SPEED_FREQ_LOW;
#endif

	/* GPIOA unused */
	GPIO_InitStructure.Pin = GPIO_PIN_0
		| GPIO_PIN_8;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* GPIOB unused */
	GPIO_InitStructure.Pin = GPIO_PIN_4
		| GPIO_PIN_10
		| GPIO_PIN_12
		| GPIO_PIN_13                          
		| GPIO_PIN_14;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* GPIOC unused */
	GPIO_InitStructure.Pin = GPIO_PIN_0
		| GPIO_PIN_11
		| GPIO_PIN_12
		| GPIO_PIN_15;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* GPIOD unused */
	GPIO_InitStructure.Pin    = GPIO_PIN_2;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* GPIOE unused */

 	/* GPIOF unused */
 	GPIO_InitStructure.Pin = GPIO_PIN_10
		| GPIO_PIN_11
 		| GPIO_PIN_13
		| GPIO_PIN_14;
	 HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);

 	 /* GPIOG unused */
	GPIO_InitStructure.Pin     = GPIO_PIN_1
		| GPIO_PIN_4
		| GPIO_PIN_5
		| GPIO_PIN_6
		| GPIO_PIN_7
		| GPIO_PIN_8 ;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	 /* GPIOH unused */
	GPIO_InitStructure.Pin = GPIO_PIN_2
		| GPIO_PIN_8
		| GPIO_PIN_11
		| GPIO_PIN_13
		| GPIO_PIN_15;
	HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);

	/* GPIOI unused */
	GPIO_InitStructure.Pin = GPIO_PIN_0
		| GPIO_PIN_1
		| GPIO_PIN_2
		| GPIO_PIN_3
		| GPIO_PIN_4;
	HAL_GPIO_Init(GPIOI, &GPIO_InitStructure);

#if CONFIG_UNUSED_GPIO_OUTPUT_LOW

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0
		|GPIO_PIN_8, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4
		| GPIO_PIN_10
		| GPIO_PIN_12
		| GPIO_PIN_13                          
		| GPIO_PIN_14, GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0
		| GPIO_PIN_11
		| GPIO_PIN_12
		| GPIO_PIN_15, GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10
		| GPIO_PIN_11
 		| GPIO_PIN_13
		| GPIO_PIN_14, GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1
		| GPIO_PIN_4
		| GPIO_PIN_5
		| GPIO_PIN_6
		| GPIO_PIN_7
		| GPIO_PIN_8, GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(GPIOH, GPIO_PIN_2
		| GPIO_PIN_8
		| GPIO_PIN_11
		| GPIO_PIN_13
		| GPIO_PIN_15, GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_0
		| GPIO_PIN_1
		| GPIO_PIN_2
		| GPIO_PIN_3
		| GPIO_PIN_4, GPIO_PIN_RESET);

#endif
#endif
}

#endif

void pace_gpio_init(void)
{
	pace_gpio_unused_cfg();

    /* GPIO Ports Clock Enable */
//    __HAL_RCC_GPIOC_CLK_ENABLE();
//    __HAL_RCC_GPIOH_CLK_ENABLE();
//    __HAL_RCC_GPIOA_CLK_ENABLE();
//    __HAL_RCC_GPIOB_CLK_ENABLE();



#if 0
	GPIO_InitTypeDef   GPIO_InitStructure;
	
#if CONFIG_UNUSED_GPIO
	pace_gpio_unused_cfg();
#else
	  /* Enable GPIOC clock */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOI_CLK_ENABLE();
#endif

	/* GPIOA Config */
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Pin = GPIO_PIN_2
		| GPIO_PIN_3 
		| GPIO_PIN_6 
		| GPIO_PIN_7;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* GPIOB Config */
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Pin = GPIO_PIN_1;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
	
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Pin = GPIO_PIN_11;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* GPIOC Config */
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Pin = GPIO_PIN_13;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* GPIOF Config */
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Pin = GPIO_PIN_2 
		|GPIO_PIN_4 
		|GPIO_PIN_12;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_2 
		|GPIO_PIN_4, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_12, GPIO_PIN_RESET);

	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Pin = GPIO_PIN_3;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);

	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Pin = GPIO_PIN_5 | GPIO_PIN_15;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);

	/* GPIOG Config */
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Pin = GPIO_PIN_0 
		|GPIO_PIN_1 
		|GPIO_PIN_2
		|GPIO_PIN_11;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_0
		|GPIO_PIN_1 
		|GPIO_PIN_2
		|GPIO_PIN_11, GPIO_PIN_RESET);

	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Pin = GPIO_PIN_3;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);

	/* GPIOH Config */
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Pin = GPIO_PIN_3 
		|GPIO_PIN_4 
//		|GPIO_PIN_5   //BT_DEV_WAKE
		|GPIO_PIN_9
		|GPIO_PIN_10;
	HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);
	HAL_GPIO_WritePin(GPIOH, GPIO_PIN_3
		|GPIO_PIN_4 
//		|GPIO_PIN_5
		|GPIO_PIN_9
		|GPIO_PIN_10, GPIO_PIN_RESET);
		
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Pin = GPIO_PIN_14;
	HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);

	/* GPIOI Config */		
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Pin = GPIO_PIN_9
		|GPIO_PIN_10;
	HAL_GPIO_Init(GPIOI, &GPIO_InitStructure);
	#endif
}

