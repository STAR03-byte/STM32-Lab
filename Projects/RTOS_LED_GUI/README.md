# RTOS_LED_GUI - FreeRTOS + emXGUI LED控制实验

## 项目简介

基于 FreeRTOS + emXGUI 的 LED 灯控制项目，运行在 STM32F407 野火霸天虎开发板上。

**功能：**
1. LCD 图形界面上显示红、绿、蓝 LED 灯符号（圆形图标）
2. LCD 图形界面显示三个触摸按钮
3. 通过点击按钮控制红、绿、蓝三个 LED 的亮灭

## 硬件平台

- **MCU**: STM32F407ZGT6
- **开发板**: 野火霸天虎 V3.0
- **LCD**: 4.3寸 480×272 (ILI9806G)
- **LED**: RGB 三色 LED (PF6-红, PF7-绿, PF8-蓝)

## 使用方法

### 1. 打开工程

双击 `Project/RTOS_LED_GUI.uvprojx` 打开 Keil 工程。

### 2. 烧录 emXGUI 资源

**首次使用前必须先烧录字体资源到外部 Flash！**

1. 打开 `综合训练资料/emXGUI_demo_stm32f407_4.3_4.5_20230927.zip`
2. 找到 `0-刷外部FLASH程序（烧录emXGUI资源文件）` 目录
3. 用 Keil 打开该目录下的工程
4. 编译并下载到开发板
5. 等待资源烧录完成（约1-2分钟）

### 3. 编译下载

1. 在 Keil 中点击 **Build** (F7) 编译工程
2. 点击 **Download** (F8) 下载到开发板
3. 复位开发板，观察 LCD 显示

## 目录结构

```
RTOS_LED_GUI/
├── App/                          # 应用层
│   ├── main.c                    # 主程序入口
│   ├── FreeRTOSConfig.h          # FreeRTOS配置
│   └── GUI_LED_Dialog.c          # LED控制GUI对话框
├── BSP/                          # 板级支持包
│   ├── led/bsp_led.c/h           # RGB LED驱动
│   ├── key/bsp_key.c/h           # 按键驱动
│   ├── lcd/bsp_lcd.c/h           # LCD驱动
│   └── usart/bsp_debug_usart.c/h # 调试串口
├── Core/                         # 系统核心
│   ├── board.h                   # 板级总头文件
│   ├── stm32f4xx_conf.h          # 外设配置
│   └── stm32f4xx_it.c/h          # 中断处理
├── Drivers/                      # STM32标准外设驱动
│   ├── CMSIS/                    # ARM CMSIS核心
│   └── STM32F4xx_StdPeriph_Driver/
├── GUI/                          # emXGUI库
│   ├── inc/                      # 头文件
│   ├── libs/                     # 预编译库
│   ├── arch/                     # 架构适配
│   ├── drv/                      # 驱动适配
│   ├── OEM_Face/                 # 外观定制
│   └── gui_drv_cfg.h             # GUI配置
├── RTOS/                         # FreeRTOS内核
│   ├── include/                  # 头文件
│   ├── port/                     # 移植层
│   └── src/                      # 源码
└── Project/                      # Keil工程文件
    └── RTOS_LED_GUI.uvprojx
```

## 注意事项

1. **LCD引脚配置**: 根据实际开发板修改 `BSP/lcd/bsp_lcd.c` 中的 FSMC 配置
2. **触摸屏**: 如果触摸不工作，检查 `GUI/drv/gui_touch_port.c` 中的触摸引脚配置
3. **串口调试**: USART1 (PA9/PA10), 波特率 115200
4. **LED引脚**: PF6(红), PF7(绿), PF8(蓝)，低电平点亮

## 常见问题

### Q: 编译报错找不到头文件
A: 检查 Keil 工程中的 Include Paths 配置是否正确。

### Q: LCD 白屏
A: 检查 LCD 驱动是否匹配，确认 FSMC 配置正确。

### Q: 触摸无反应
A: 确认触摸屏引脚配置正确，检查是否需要触摸校准。

### Q: LED 不亮
A: 检查 LED 引脚定义是否与开发板一致，确认 LED 是低电平还是高电平点亮。
