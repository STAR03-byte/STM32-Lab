/**
  ******************************************************************************
  * @file    stm32f4xx_conf.h
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   外设头文件配置
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#ifndef __STM32F4xx_CONF_H
#define __STM32F4xx_CONF_H

/* 头文件包含 */
#include "stm32f4xx_adc.h"
#include "stm32f4xx_can.h"
#include "stm32f4xx_crc.h"
#include "stm32f4xx_cryp.h"
#include "stm32f4xx_dac.h"
#include "stm32f4xx_dbgmcu.h"
#include "stm32f4xx_dcmi.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_flash.h"
#include "stm32f4xx_fsmc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_hash.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_iwdg.h"
#include "stm32f4xx_pwr.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_rng.h"
#include "stm32f4xx_rtc.h"
#include "stm32f4xx_sdio.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_wwdg.h"
#include "misc.h"

/* 在这里检查HSE的值 */
#if !defined (HSE_VALUE)
  #define HSE_VALUE    ((uint32_t)25000000)  /* 外部高速晶振的值为25MHz (野火开发板) */
#endif

#if !defined (HSE_STARTUP_TIMEOUT)
  #define HSE_STARTUP_TIMEOUT    ((uint32_t)0x0500)  /* HSE启动超时时间 */
#endif

/* 在这里检查HSI的值 */
#if !defined (HSI_VALUE)
  #define HSI_VALUE    ((uint32_t)16000000)  /* 内部高速晶振的值为16MHz */
#endif

/* 在这里检查LSE的值 */
#if !defined (LSE_VALUE)
  #define LSE_VALUE    ((uint32_t)32768)  /* 外部低速晶振的值为32.768KHz */
#endif

#if !defined (LSE_STARTUP_TIMEOUT)
  #define LSE_STARTUP_TIMEOUT    ((uint32_t)0x0500)  /* LSE启动超时时间 */
#endif

/* 在这里检查LSI的值 */
#if !defined (LSI_VALUE)
  #define LSI_VALUE    ((uint32_t)32000)  /* 内部低速晶振的值为32KHz */
#endif

/* 在这里检查外部IO的值 */
#if !defined (EXTERNAL_CLOCK_VALUE)
  #define EXTERNAL_CLOCK_VALUE    ((uint32_t)12288000)  /* 外部IO晶振的值为12.288MHz */
#endif

/* 如果需要使用标准外设库的断言功能，取消下面的注释 */
/* #define USE_FULL_ASSERT    1 */

#ifdef USE_FULL_ASSERT
  /**
    * @brief  assert_param宏用于检查函数参数
    * @param  expr: 如果expr为假，则调用assert_failed函数
    * @retval None
    */
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0)
#endif

#endif /* __STM32F4xx_CONF_H */
