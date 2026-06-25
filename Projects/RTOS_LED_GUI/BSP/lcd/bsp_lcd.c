/**
  ******************************************************************************
  * @file    bsp_lcd.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   LCD驱动 (NT35510, 4.3寸 480x800)
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "bsp_lcd.h"
#include "bsp_nt35510_lcd.h"

/* NT35510 LCD尺寸 */
#define NT35510_WIDTH   480
#define NT35510_HEIGHT  800

/* 当前LCD尺寸 */
static uint16_t lcd_width = NT35510_WIDTH;
static uint16_t lcd_height = NT35510_HEIGHT;

/**
  * @brief  初始化LCD
  * @param  无
  * @retval 无
  */
void LCD_Init(void)
{
    /* 调用NT35510初始化 */
    LCD_Init_Init();

    /* 设置LCD尺寸 */
    lcd_width = NT35510_WIDTH;
    lcd_height = NT35510_HEIGHT;
}

/**
  * @brief  LCD背光控制
  * @param  on: 0=关闭, 非0=打开
  * @retval 无
  */
void LCD_BackLed_Control(int on)
{
    /* 调用NT35510背光控制 */
    LCD_Init_BackLed_Control(on);
}

/**
  * @brief  清屏
  * @param  Color: 要清屏的填充色
  * @retval 无
  */
void LCD_Clear(uint16_t Color)
{
    NT35510_Clear(0, 0, lcd_width, lcd_height);
}

/**
  * @brief  设置光标位置
  * @param  Xpos: X坐标
  * @param  Ypos: Y坐标
  * @retval 无
  */
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
    /* NT35510使用窗口方式，不需要单独设置光标 */
    (void)Xpos;
    (void)Ypos;
}

/**
  * @brief  写LCD寄存器
  * @param  LCD_Reg: 寄存器地址
  * @param  LCD_RegValue: 要写入的数据
  * @retval 无
  */
void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue)
{
    /* NT35510使用命令方式 */
    (void)LCD_Reg;
    (void)LCD_RegValue;
}

/**
  * @brief  读LCD寄存器
  * @param  LCD_Reg: 寄存器地址
  * @retval 读到的数据
  */
uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{
    /* NT35510使用命令方式 */
    (void)LCD_Reg;
    return 0;
}

/**
  * @brief  写LCD RAM
  * @param  RGB_Code: 颜色值
  * @retval 无
  */
void LCD_WriteRAM(uint16_t RGB_Code)
{
    /* NT35510使用命令方式 */
    (void)RGB_Code;
}

/**
  * @brief  准备写LCD RAM
  * @param  无
  * @retval 无
  */
void LCD_WriteRAM_Prepare(void)
{
    /* NT35510使用命令方式 */
}

/**
  * @brief  读LCD RAM
  * @retval 读到的数据
  */
uint16_t LCD_ReadRAM(void)
{
    return 0;
}

/**
  * @brief  设置LCD窗口
  * @param  Xstart: 窗口起始X坐标
  * @param  Ystart: 窗口起始Y坐标
  * @param  Xend: 窗口结束X坐标
  * @param  Yend: 窗口结束Y坐标
  * @retval 无
  */
void LCD_SetWindow(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend)
{
    NT35510_OpenWindow(Xstart, Ystart, Xend - Xstart, Yend - Ystart);
}

/**
  * @brief  画点
  * @param  Xpos: X坐标
  * @param  Ypos: Y坐标
  * @param  Color: 颜色值
  * @retval 无
  */
void LCD_DrawPixel(uint16_t Xpos, uint16_t Ypos, uint16_t Color)
{
    NT35510_OpenWindow(Xpos, Ypos, 1, 1);
    /* 写入颜色数据 */
    /* NT35510需要通过命令方式写入 */
}

/**
  * @brief  填充矩形区域
  * @param  Xstart: 起始X坐标
  * @param  Ystart: 起始Y坐标
  * @param  Xend: 结束X坐标
  * @param  Yend: 结束Y坐标
  * @param  Color: 填充颜色
  * @retval 无
  */
void LCD_FillRect(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend, uint16_t Color)
{
    NT35510_Clear(Xstart, Ystart, Xend - Xstart, Yend - Ystart);
}
