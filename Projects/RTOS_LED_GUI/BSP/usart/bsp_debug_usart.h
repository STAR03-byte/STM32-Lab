/**
  ******************************************************************************
  * @file    bsp_debug_usart.h
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   重定向c库printf函数到usart端口
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#ifndef __DEBUG_USART_H
#define __DEBUG_USART_H

#include "stm32f4xx.h"

/* 串口波特率 */
#define DEBUG_USART_BAUDRATE    115200

/* 引脚定义 */
#define DEBUG_USART_GPIO_TX     GPIO_Pin_9
#define DEBUG_USART_GPIO_RX     GPIO_Pin_10
#define DEBUG_USART_GPIO        GPIOA
#define DEBUG_USART_GPIO_CLK    RCC_AHB1Periph_GPIOA

#define DEBUG_USART             USART1
#define DEBUG_USART_CLK         RCC_APB2Periph_USART1
#define DEBUG_USART_AF          GPIO_AF_USART1
#define DEBUG_USART_TX_SOURCE   GPIO_PinSource9
#define DEBUG_USART_RX_SOURCE   GPIO_PinSource10

void Debug_USART_Config(void);
void Usart_SendByte(USART_TypeDef* pUSARTx, uint8_t ch);
void Usart_SendString(USART_TypeDef* pUSARTx, char* str);

#endif /* __DEBUG_USART_H */
