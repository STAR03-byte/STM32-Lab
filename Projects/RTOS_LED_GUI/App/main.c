/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   NT35510 LCD测试程序
  *********************************************************************
  */

#include "stm32f4xx.h"
#include "bsp_led.h"
#include "bsp_debug_usart.h"
#include "bsp_nt35510_lcd.h"

/**
  * @brief  延时函数
  * @param  ms: 延时毫秒数
  * @retval 无
  */
void delay_ms(uint32_t ms)
{
    uint32_t i;
    while (ms--) {
        for (i = 0; i < 10000; i++);
    }
}

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
    /* 配置NVIC为优先级组4 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    /* 初始化串口 */
    Debug_USART_Config();

    /* 初始化LED */
    LED_GPIO_Config();

    printf("NT35510 LCD 测试程序\n");

    /* 初始化LCD */
    printf("正在初始化LCD...\n");
    LCD_Init();
    printf("LCD初始化完成\n");

    /* 清屏为红色 */
    printf("清屏为红色...\n");
    NT35510_Clear(0, 0, 480, 800);
    printf("清屏完成\n");

    /* 主循环 */
    while (1) {
        LED1_ON;
        delay_ms(500);
        LED1_OFF;
        delay_ms(500);
    }
}
