/**
  ******************************************************************************
  * @file    bsp_debug_usart.c
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

#include "bsp_debug_usart.h"
#include <stdio.h>

/**
  * @brief  配置调试用的USART
  * @param  无
  * @retval 无
  */
void Debug_USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    /* 使能USART GPIO时钟 */
    RCC_AHB1PeriphClockCmd(DEBUG_USART_GPIO_CLK, ENABLE);

    /* 使能USART时钟 */
    RCC_APB2PeriphClockCmd(DEBUG_USART_CLK, ENABLE);

    /* GPIO复用功能配置 */
    GPIO_PinAFConfig(DEBUG_USART_GPIO, DEBUG_USART_TX_SOURCE, DEBUG_USART_AF);
    GPIO_PinAFConfig(DEBUG_USART_GPIO, DEBUG_USART_RX_SOURCE, DEBUG_USART_AF);

    /* 配置Tx引脚为复用功能 */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_GPIO_TX;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DEBUG_USART_GPIO, &GPIO_InitStructure);

    /* 配置Rx引脚为复用功能 */
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_GPIO_RX;
    GPIO_Init(DEBUG_USART_GPIO, &GPIO_InitStructure);

    /* 配置串口参数 */
    USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(DEBUG_USART, &USART_InitStructure);

    /* 使能串口 */
    USART_Cmd(DEBUG_USART, ENABLE);
}

/**
  * @brief  发送一个字节
  * @param  pUSARTx: 串口基地址
  * @param  ch: 要发送的字节
  * @retval 无
  */
void Usart_SendByte(USART_TypeDef* pUSARTx, uint8_t ch)
{
    /* 发送一个字节数据到USART */
    USART_SendData(pUSARTx, ch);

    /* 等待发送数据寄存器为空 */
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}

/**
  * @brief  发送字符串
  * @param  pUSARTx: 串口基地址
  * @param  str: 要发送的字符串
  * @retval 无
  */
void Usart_SendString(USART_TypeDef* pUSARTx, char* str)
{
    unsigned int k = 0;
    do
    {
        Usart_SendByte(pUSARTx, *(str + k));
        k++;
    } while (*(str + k) != '\0');

    /* 等待发送完成 */
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET);
}

/// 重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE* f)
{
    /* 发送一个字节数据到串口 */
    USART_SendData(DEBUG_USART, (uint8_t)ch);

    /* 等待发送完毕 */
    while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);

    return (ch);
}

/// 重定向c库函数scanf到串口，重定向后可使用scanf、getchar等函数
int fgetc(FILE* f)
{
    /* 等待串口输入数据 */
    while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_RXNE) == RESET);

    return (int)USART_ReceiveData(DEBUG_USART);
}
