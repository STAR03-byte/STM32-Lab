/**
  ******************************************************************************
  * @file    bsp_lcd_ext.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   LCD外部接口函数（供emXGUI调用）
  ******************************************************************************
  */

#include "bsp_lcd.h"
#include "emXGUI_Arch.h"

/* LCD背光控制引脚定义 */
#define LCD_BL_PIN      GPIO_Pin_15
#define LCD_BL_PORT     GPIOB
#define LCD_BL_CLK      RCC_AHB1Periph_GPIOB

/* 内部变量 */
static int _lcd_x = 0, _lcd_y = 0;

/**
  * @brief  外部LCD写开始
  * @param  sx: 起始X坐标
  * @param  sy: 起始Y坐标
  * @param  ex: 结束X坐标
  * @param  ey: 结束Y坐标
  * @retval 无
  */
void EXT_LCD_WriteStart(U16 sx, U16 sy, U16 ex, U16 ey)
{
    (void)ex;
    (void)ey;
    LCD_SetCursor(sx, sy);
    LCD_WriteRAM_Prepare();
    _lcd_x = sx;
    _lcd_y = sy;
}

/**
  * @brief  外部LCD读开始
  * @param  sx: 起始X坐标
  * @param  sy: 起始Y坐标
  * @param  ex: 结束X坐标
  * @param  ey: 结束Y坐标
  * @retval 无
  */
void EXT_LCD_ReadStart(U16 sx, U16 sy, U16 ex, U16 ey)
{
    (void)ex;
    (void)ey;
    LCD_SetCursor(sx, sy);
    _lcd_x = sx;
    _lcd_y = sy;
}

/**
  * @brief  外部LCD写像素
  * @param  c: 颜色值
  * @retval 无
  */
void EXT_LCD_WritePixel(U16 c)
{
    LCD_WriteRAM(c);
}

/**
  * @brief  外部LCD写RGB
  * @param  r: 红色分量
  * @param  g: 绿色分量
  * @param  b: 蓝色分量
  * @retval 无
  */
void EXT_LCD_WriteRGB(U8 r, U8 g, U8 b)
{
    uint16_t color = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
    LCD_WriteRAM(color);
}

/**
  * @brief  外部LCD读像素
  * @retval 颜色值
  */
U16 EXT_LCD_ReadPixel(void)
{
    return LCD_ReadRAM();
}

/**
  * @brief  外部LCD读RGB
  * @param  r: 红色分量指针
  * @param  g: 绿色分量指针
  * @param  b: 蓝色分量指针
  * @retval 无
  */
void EXT_LCD_ReadRGB(U8 *r, U8 *g, U8 *b)
{
    uint16_t color = LCD_ReadRAM();
    *r = (color >> 11) << 3;
    *g = ((color >> 5) & 0x3F) << 2;
    *b = (color & 0x1F) << 3;
}

/**
  * @brief  外部LCD写位
  * @param  x: X坐标
  * @param  y: Y坐标
  * @param  w: 宽度
  * @param  h: 高度
  * @param  bits: 数据指针
  * @param  width_bytes: 每行字节数
  * @retval 无
  */
void EXT_LCD_WriteBits(U16 x, U16 y, U16 w, U16 h, const U8 *bits, int width_bytes)
{
    int i, j;
    const U16 *p = (const U16 *)bits;

    for (j = 0; j < h; j++) {
        LCD_SetCursor(x, y + j);
        LCD_WriteRAM_Prepare();
        for (i = 0; i < w; i++) {
            LCD_WriteRAM(p[j * width_bytes / 2 + i]);
        }
    }
}

/**
  * @brief  外部LCD读位
  * @param  x: X坐标
  * @param  y: Y坐标
  * @param  w: 宽度
  * @param  h: 高度
  * @param  out: 输出缓冲区
  * @param  width_bytes: 每行字节数
  * @retval 无
  */
void EXT_LCD_ReadBits(U16 x, U16 y, U16 w, U16 h, U8 *out, int width_bytes)
{
    int i, j;
    U16 *p = (U16 *)out;

    for (j = 0; j < h; j++) {
        LCD_SetCursor(x, y + j);
        for (i = 0; i < w; i++) {
            p[j * width_bytes / 2 + i] = LCD_ReadRAM();
        }
    }
}

/**
  * @brief  外部LCD填充矩形
  * @param  x: X坐标
  * @param  y: Y坐标
  * @param  w: 宽度
  * @param  h: 高度
  * @param  c: 颜色值
  * @retval 无
  */
void EXT_LCD_FillRect(U16 x, U16 y, U16 w, U16 h, U16 c)
{
    LCD_FillRect(x, y, x + w - 1, y + h - 1, c);
}

/* pTouchDev 在 GUI_LED_Dialog.c 中定义 */
