/***************************************************************************/
/*                                                           Date:08/2017  */
/*                        Pace Uart Driver                                 */
/*             (c)copyright Pacewear                                       */
/*-------------------------------------------------------------------------*/
/*   Author:     future                                                    */
/*   Reference documents:                                                  */
/*-------------------------------------------------------------------------*/
/*  Comments:                                                              */
/*   File :    pace_uart.c                                                 */
/*   Labels :                                                              */
/*   Brief    Uart Driver                                                  */
/*=========================================================================*/
/*  Modifications                                                          */
/*-------------------------------------------------------------------------*/
/*  date    | author  |modification                                        */
/*----------|---------|----------------------------------------------------*/
/*11/08/2017|   |                                                          */
/*----------|---------|----------------------------------------------------*/

/*=========================================================================*/
/*  Problems Report                                                        */
/*-------------------------------------------------------------------------*/
/*  date    | author  |modification                                        */
/*----------|---------|----------------------------------------------------*/
/***************************************************************************/


/**********include files************/


#include "pace_log.h"
#include "pace_msg.h"
#include "pace_task.h"


/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
char aRxBuffer[1];
uint8_t rx_count=0;
char at_cmd[200]={0};
char Rxbuff[200]={0};


#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART2 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}


 void pace_uart2_init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if(HAL_UART_Receive_IT(&huart2, (uint8_t *)aRxBuffer, 1) != HAL_OK)
  {
    Error_Handler();
  }
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
    pace_msg msg_intr= NULL_MSG;

    Rxbuff[rx_count] = aRxBuffer[0];
    
    if((Rxbuff[rx_count] =='Z')||(Rxbuff[rx_count] =='z')) {
        memset(at_cmd, 0, sizeof(at_cmd));
        memcpy(at_cmd, Rxbuff, rx_count);
     //   printf("send %s length =%d\n",at_cmd,strlen(at_cmd));
        msg_intr.msg_id = MSG_ID_AT_CMD;
        msg_intr.src_task = getCurrentTaskId();
        msg_intr.dst_task = TASK_AT_CMD;
        msg_intr.length = strlen(at_cmd);
        msg_intr.buffer = (void *)at_cmd;
        msg_intr.hEvent = NULL;

        pace_post_msg_to_queue(&msg_intr);

        memset(Rxbuff,0,sizeof(Rxbuff));
        rx_count = 0;
    }else {
        rx_count++;
    }
    HAL_UART_Receive_IT(UartHandle,(uint8_t *)aRxBuffer,1);
 }


void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{

    GPIO_InitTypeDef GPIO_InitStruct;
    if(huart->Instance==USART2)
    {
        /*##-1- Enable peripherals and GPIO Clocks #################################*/
        /* Enable GPIO TX/RX clock */
        USARTx_TX_GPIO_CLK_ENABLE();
        USARTx_RX_GPIO_CLK_ENABLE();


        /* Enable USARTx clock */
        USARTx_CLK_ENABLE(); 

        /*##-2- Configure peripheral GPIO ##########################################*/  
        /* UART TX GPIO pin configuration  */
        GPIO_InitStruct.Pin       = USARTx_TX_PIN;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
        GPIO_InitStruct.Alternate = USARTx_TX_AF;

        HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

        /* UART RX GPIO pin configuration  */
        GPIO_InitStruct.Pin = USARTx_RX_PIN;
        GPIO_InitStruct.Alternate = USARTx_RX_AF;

        HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);

        /*##-3- Configure the NVIC for UART ########################################*/
        /* NVIC for USART */
        HAL_NVIC_SetPriority(USARTx_IRQn, 10, 0);
        HAL_NVIC_EnableIRQ(USARTx_IRQn);
    }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{

    if(huart->Instance==USART2)
    {
        /*##-1- Reset peripherals ##################################################*/
        USARTx_FORCE_RESET();
        USARTx_RELEASE_RESET();

        /*##-2- Disable peripherals and GPIO Clocks #################################*/
        /* Configure UART Tx as alternate function  */
        HAL_GPIO_DeInit(USARTx_TX_GPIO_PORT, USARTx_TX_PIN);
        /* Configure UART Rx as alternate function  */
        HAL_GPIO_DeInit(USARTx_RX_GPIO_PORT, USARTx_RX_PIN);

        /*##-3- Disable the NVIC for UART ##########################################*/
        HAL_NVIC_DisableIRQ(USARTx_IRQn);

    }

}



