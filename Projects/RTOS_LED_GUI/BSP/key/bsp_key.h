/**
  ******************************************************************************
  * @file    bsp_key.h
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   按键驱动头文件
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#ifndef __KEY_H
#define __KEY_H

#include "stm32f4xx.h"

/* 引脚定义 */
#define KEY1_PIN                  GPIO_Pin_0
#define KEY1_GPIO_PORT            GPIOA
#define KEY1_GPIO_CLK             RCC_AHB1Periph_GPIOA

#define KEY2_PIN                  GPIO_Pin_13
#define KEY2_GPIO_PORT            GPIOC
#define KEY2_GPIO_CLK             RCC_AHB1Periph_GPIOC

/* 按键按下标志 */
#define KEY_ON  1
#define KEY_OFF 0

void Key_GPIO_Config(void);
uint8_t Key_Scan(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

#endif /* __KEY_H */
