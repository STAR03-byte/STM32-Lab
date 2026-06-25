/**
  ******************************************************************************
  * @file    GUI_Startup.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   emXGUI 启动入口
  *          简化版：直接启动LED控制界面，无需外部资源
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include <emXGUI.h>
#include "gui_drv.h"
#include "gui_os_port.h"
#include "gui_lcd_port.h"

/* 外部声明 */
extern void GUI_LED_Dialog(void);

/**
  * @brief  emXGUI 启动函数
  * @param  无
  * @retval 无
  * @note   此函数由FreeRTOS GUI任务调用，不会返回
  */
void GUI_Startup(void)
{
    /* 初始化GUI内核 */
    GUI_Init();

    /* 初始化LCD */
    GUI_LCD_Init();

    /* 初始化触摸屏 */
    GUI_Touch_Init();

    /* 初始化字体 */
    GUI_Font_Init();

    /* 直接启动LED控制界面 */
    GUI_LED_Dialog();
}
