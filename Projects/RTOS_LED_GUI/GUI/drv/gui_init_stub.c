/**
  ******************************************************************************
  * @file    gui_init_stub.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   emXGUI 初始化函数存根
  ******************************************************************************
  */

#include <emXGUI.h>
#include "gui_drv.h"
#include "gui_lcd_port.h"

/* 外部变量 */
extern BOOL Load_state;

/**
  * @brief  GUI初始化
  * @retval TRUE=成功, FALSE=失败
  */
BOOL GUI_Init(void)
{
    /* 初始化GUI内核 */
    return TRUE;
}

/**
  * @brief  GUI LCD初始化
  * @retval SURFACE指针
  */
SURFACE* GUI_LCD_Init(void)
{
    /* 初始化LCD */
    LCD_Init();
    LCD_BackLed_Control(1);
    return NULL;
}

/**
  * @brief  GUI触摸初始化
  * @retval 无
  */
void GUI_Touch_Init(void)
{
    /* 触摸屏初始化（存根） */
}

/**
  * @brief  GUI字体初始化
  * @retval 无
  */
void GUI_Font_Init(void)
{
    /* 字体初始化 */
    Load_state = TRUE;
}

/**
  * @brief  XFT字体创建（存根）
  * @param  font_id: 字体ID
  * @retval 字体句柄
  */
HFONT XFT_CreateFont(int font_id)
{
    (void)font_id;
    return NULL;
}
