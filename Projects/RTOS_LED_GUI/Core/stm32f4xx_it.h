/**
  ******************************************************************************
  * @file    stm32f4xx_it.h
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   中断处理函数头文件
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#ifndef __STM32F4xx_IT_H
#define __STM32F4xx_IT_H

#include "stm32f4xx.h"

/* 函数声明 */
/* Note: SVC_Handler, PendSV_Handler, SysTick_Handler are provided by FreeRTOS */
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void DebugMon_Handler(void);

#endif /* __STM32F4xx_IT_H */
