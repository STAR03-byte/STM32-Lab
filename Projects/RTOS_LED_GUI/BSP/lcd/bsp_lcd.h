/**
  ******************************************************************************
  * @file    bsp_lcd.h
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   LCD驱动头文件 (NT35510, 4.3寸 480x800)
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#ifndef __LCD_H
#define __LCD_H

#include "stm32f4xx.h"

/* LCD尺寸定义 (NT35510) - 在 gui_drv_cfg.h 中定义 */

/* LCD颜色定义 */
#define LCD_COLOR_WHITE     0xFFFF
#define LCD_COLOR_BLACK     0x0000
#define LCD_COLOR_RED       0xF800
#define LCD_COLOR_GREEN     0x07E0
#define LCD_COLOR_BLUE      0x001F
#define LCD_COLOR_YELLOW    0xFFE0
#define LCD_COLOR_CYAN      0x07FF
#define LCD_COLOR_MAGENTA   0xF81F

/* 函数声明 */
void LCD_Init(void);
void LCD_Clear(uint16_t Color);
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);
void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue);
uint16_t LCD_ReadReg(uint16_t LCD_Reg);
void LCD_WriteRAM(uint16_t RGB_Code);
void LCD_WriteRAM_Prepare(void);
uint16_t LCD_ReadRAM(void);
void LCD_SetWindow(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend);
void LCD_DrawPixel(uint16_t Xpos, uint16_t Ypos, uint16_t Color);
void LCD_FillRect(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend, uint16_t Color);

/* NT35510特定函数 */
void LCD_BackLed_Control(int on);

#endif /* __LCD_H */
