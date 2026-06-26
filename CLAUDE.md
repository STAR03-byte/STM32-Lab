# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

2023级计科嵌入式系统综合实训项目 — 基于 STM32F407 + FreeRTOS + emXGUI 的 LED 灯控制 GUI 应用（SmartLED）。
硬件平台：野火 STM32F407 "霸天虎" V2.0 开发板。

## Build

Keil MDK5 项目，**不使用命令行构建**。在 Keil 中打开 `SmartLED/Project/SmartLED/SmartLED.uvprojx`，点 Build (F7)。

- 编译器：ARM Compiler V5.06 (AC5)，**不是 AC6**
- 优化等级：Level 2
- MicroLib 已启用，C99 模式
- 输出：`SmartLED/Output/SmartLED.hex`

### Compiler Defines

```
USE_STDPERIPH_DRIVER,STM32F40_41xxx,X_GUI_USE_FREERTOS,EXT_LCD_ARCH
```

| Define | 作用 |
|--------|------|
| `USE_STDPERIPH_DRIVER` | 使用标准外设库（非 HAL） |
| `STM32F40_41xxx` | 选择 F407/F417 设备 |
| `X_GUI_USE_FREERTOS` | emXGUI 使用 FreeRTOS 作为 OS 后端 |
| `EXT_LCD_ARCH` | 启用 MPU 绘图引擎（直接写 LCD，无帧缓冲） |

## Architecture

### 目录结构

```
Projects/RTOS_LED_GUI/
  App/              ← 用户代码：main.c、GUI_Startup.c、GUI_LED_Dialog.c
  BSP/              ← 板级驱动：led/、key/、lcd/、touch/、usart/、flash/
  Core/             ← board.h（硬件总头文件）、中断、stm32f4xx_conf.h
  Drivers/          ← STM32 标准外设库 + CMSIS
  GUI/              ← emXGUI 图形库
    arch/           ← 架构层：EXT_LCD_Arch.c（MPU 绘图引擎）、OS 适配
    drv/            ← 移植层：LCD/触摸/字体/内存/日志等端口
    inc/            ← emXGUI 公共头文件（Win32 风格 API：HWND, HDC, WM_*）
    libs/           ← 预编译库：emXGUI100_MDK_M4.lib、x_libc_MDK_M4.lib
    OEM_Face/       ← 控件实现：Button、CheckBox 等
    gui_drv_cfg.h   ← GUI 主配置（显示/输入/字体/资源/内存）
  RTOS/             ← FreeRTOS v9.0.0 源码（ARM_CM4F port, heap_4）
  Project/          ← Keil 工程文件
```

### 关键架构决策

1. **无帧缓冲模式**：F407 无外部 SDRAM，`FRAME_BUFFER_EN=0`。`EXT_LCD_ARCH` 启用 MPU 绘图引擎，所有像素通过 FSMC 直写 LCD（地址 `0x68000000` 命令 / `0x68000002` 数据）。
2. **初始化链**：`main()` → FreeRTOS 任务 → `GUI_Startup()`（gui_startup.c）→ `GUI_DesktopStartup()`（GUI_Startup.c）→ `GUI_LED_Dialog()`（GUI_LED_Dialog.c）
3. **内存管理**：GUI 和 FreeRTOS 共用 heap_4，64KB 堆（`configTOTAL_HEAP_SIZE`）。无 VMEM，无外部 RAM。
4. **触摸轮询**：`GUI_LED_Dialog.c` 的 `WM_TIMER`（35ms）调用 `GUI_InputHandler()` 轮询 GT911 触摸控制器，生成鼠标事件派发给按钮控件。
5. **字体**：仅内置 ASCII_24_4BPP stub（gui_font_data.c），`defaultFont` 可能为 NULL，使用前需检查。

### emXGUI 控件模式

GUI_LED_Dialog.c 使用 emXGUI 窗口/控件 API：
- `CreateWindowEx(WS_EX_NOFOCUS, ...)` 创建主窗口
- `CreateWindow(BUTTON, ..., BS_NOTIFY | WS_OWNERDRAW, ...)` 创建自绘按钮
- `WM_DRAWITEM` 自绘按钮外观（圆角矩形 + 文字）
- `WM_NOTIFY` + `BN_CLICKED` 处理按钮点击
- `WM_TIMER` + `GUI_InputHandler()` 轮询触摸输入
- `GetMessage/TranslateMessage/DispatchMessage` 消息循环

### 硬件引脚

| 外设 | 引脚 | 说明 |
|------|------|------|
| RGB LED | PF6(红) PF7(绿) PF8(蓝) | 低电平点亮 |
| LCD FSMC | Bank1_NORSRAM3 | 16-bit 并口, NT35510 800x480 (ROTATE_90) |
| GT911 触摸 I2C | PB6(SCL) PB7(SDA) | I2C1, 400kHz, 地址 0xBA |
| HSE 晶振 | 25MHz | 非常见的 8MHz |

## Important Constraints

1. **标准外设库，非 HAL**：所有 STM32 代码使用 `STM32F4xx_StdPeriph_Driver`，不要引入 HAL。
2. **AC5 编译器**：不支持 AC6 语法，避免 C11 特性。
3. **内存极有限**：192KB SRAM（128KB SRAM1 + 64KB CCM），64KB 已给 FreeRTOS 堆。
4. **预编译 .lib**：`emXGUI100_MDK_M4.lib` 和 `x_libc_MDK_M4.lib` 是 AC5 编译的，不能用 AC6 链接。
5. **EXT_LCD_Arch.c 通过 #include 编译**：在 `bsp_nt35510_lcd.c` 中 `#include`，不要单独加入 Keil 工程（会重复定义）。
6. **LED/ 目录是参考项目**：来自野火官方例程，包含大量额外功能。我们的项目在 `Projects/RTOS_LED_GUI/` 中。
7. **`defaultFont` 是全局变量**：在 `gui_font_port.c` 中定义，由 `GUI_Default_FontInit()` 初始化。使用前必须检查 NULL。

## Common Pitfalls

- **宏展开陷阱**：`LED1_ON` 展开为 `{ GPIO_ResetBits(...); }`，在 if-else 中直接使用会断链。改用 `GPIO_WriteBit()`。
- **函数名匹配**：`EXT_LCD_CreateSurfaceRGB565`（无下划线），不要写成 `EXT_LCD_CreateSurface_RGB565`。
- **gui_font_data.c stub**：`ASCII_24_4BPP` 等至少 1 字节，否则链接报未定义。
- **触摸轮询**：对话框必须创建 `WM_TIMER`（35ms）调用 `GUI_InputHandler()`，否则触摸无响应。参考 `GUI_LED_Dialog.c` 的实现。
- **GUI_TouchPanel.c 不要加入工程**：触摸由 `BSP/touch/bsp_touch.c`（GT911 驱动）+ `GUI/drv/gui_touch_port.c`（端口适配）实现。
- **FreeRTOS 中断映射**：`SVC_Handler` → `vPortSVCHandler`，`PendSV_Handler` → `xPortPendSVHandler`（FreeRTOSConfig.h 中 #define）。
