/**
  *********************************************************************
  * @file    gui_drv_cfg.h
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   emXGUI驱动配置文件
  *          配置显示设备、输入设备、存储设备、字体、资源设备参数
  *********************************************************************
  * @attention
  * 官网:www.emXGUI.com
  * 此配置已禁用外部资源（SD卡/外部Flash），使用内置字体
  **********************************************************************
  */
#ifndef __GUI_DRV_CFG_H__
#define __GUI_DRV_CFG_H__

#include "board.h"

/*===========显示设备配置===gui_lcd_port.c===============================================*/
/* 野火4.3寸TFT, 480x272, ILI9806G */

/* 显存起始地址 - 使用内部SRAM */
#define  LCD_FRAME_BUFFER   0x20010000

/* 使用的显示格式 */
#define LCD_FORMAT    COLOR_FORMAT_RGB565

/* 当前使用液晶屏的宽度 (NT35510) */
#define LCD_XSIZE           480
#define LCD_YSIZE           800

/* 使用的液晶屏的最大宽高，用于计算显存占用 */
#define LCD_MAX_XSIZE       480
#define LCD_MAX_YSIZE       800

/* 使用的LCD类型编号，支持不同的分辨率 */
#define  GUI_LCD_TYPE_NUM   0

/* 屏幕旋转设置 */
//#define LCD_ROTATE        ROTATE_0

/* 是否使用硬件图形加速器（DMA2D之类），需要按需重定义 */
#define DMA2D_EN                   0

/* 是否使用窗口和控件，使用的话会分配一块跟液晶屏等大的虚拟缓冲区，显示效果更好不闪烁 */
#define  FRAME_BUFFER_EN           1


/*==========输入设备配置===gui_input_port.c==================================================*/
/* 是否使用输入设备 */
#define  GUI_INPUT_DEV_EN               1

/* 是否使用键盘、鼠标、触摸屏 */
#define GUI_TOUCHSCREEN_EN              1
#define GUI_KEYBOARD_EN                 0
#define GUI_MOUSE_EN                    0

/* 是否需要触摸校准-电阻屏需要 */
#define GUI_TOUCHSCREEN_CALIBRATE       0

/* 是否显示光标 */
#define  GUI_SHOW_CURSOR_EN             0


/*===========存储设备配置===gui_mem_port.c===============================================*/
/* 通过GUI内核使用的MEM内存堆，VMEM内存供用户使用 */

/* GUI内核使用的存储器的起始地址 - 使用FreeRTOS堆管理，不需要指定 */
#define GUI_CORE_MEM_BASE   0

/* GUI内核使用的存储器的大小（推荐最小值为8KB） */
#define  GUI_CORE_MEM_SIZE  (64*1024)  /* FreeRTOS在heap_4.c实现 */
/* 最小分配精度，单位为字节 */
#define GUI_CORE_MEM_ALLOC_UNIT   (64)


/* 关于vmem的起始地址、大小以及分配精度 */
/* 不使用VMEM内存池（无外部SDRAM时禁用） */
#define  GUI_VMEM_EN             0

/* 液晶屏显存使用的空间 */
#if (LCD_FORMAT == COLOR_FORMAT_RGB565)
  #define LCD_FRAME_SIZE  (LCD_MAX_XSIZE*LCD_MAX_YSIZE*2)
#elif (LCD_FORMAT == COLOR_FORMAT_XRGB8888)
  #define LCD_FRAME_SIZE (LCD_MAX_XSIZE*LCD_MAX_YSIZE*4)
#endif

/* 不使用外部SDRAM时，VMEM设为0 */
#define VMEM_BASE         (0)
#define VMEM_SIZE         (0)
#define VMEM_ALLOC_UNIT   (64)

//设置变量分配到EXRAM区域的宏（不使用外部RAM时为空）
#define __EXRAM


/*===========字体配置===gui_font_port.c===============================================*/
/* 不使用外部XFT字体库（无SD卡/外部Flash） */
#define GUI_FONT_XFT_EN       0
/* 不支持TTF字体 */
#define GUI_FONT_TTF_EN       0

/* 使用内置字体 */
#define GUI_DEFAULT_FONT_EN          ASCII_24_4BPP

/* 不使用内置的中文字体（需要XFT支持） */
#define GUI_INER_CN_FONT_EN      0
/* 默认的内置中文字体（不使用） */
#define GUI_DEFAULT_FONT_CN          0

/* 不使用外部FLASH中的字库 */
#define GUI_EXTERN_FONT_EN       0

/* 不加载外部字体到RAM */
#define GUI_FONT_LOAD_TO_RAM_EN    0

/* 外部字体文件名（不使用） */
#define GUI_DEFAULT_EXTERN_FONT    ""

/* 不使用ICON LOGO字体（需要外部资源） */
#define  GUI_ICON_LOGO_EN         0

/* LOGO字体（不使用） */
#define  GUI_LOGO_FONT              ""
/* ICON字体（不使用） */
#define   GUI_ICON_FONT_100        ""
/* 控件ICON字体（不使用） */
#define   GUI_CONTROL_FONT_64      ""


/*===========日志设备配置===gui_log_port.c===============================================*/

/* 是否使能日志输出（调试函数） */
#define GUI_DEBUG_EN          1
#define GUI_DEBUG_ARRAY_EN    1
#define GUI_DEBUG_FUNC_EN     1



/*===========资源设备配置===gui_resource_port.c===============================================*/
/* 不使用资源设备（无SD卡/外部Flash） */
#define GUI_RES_DEV_EN         0

/* 不使用文件系统接口 */
#define GUI_FS_EN              0

/* 不使用资源文件系统 */
#define GUI_RES_FS_EN          0

/* 资源存储的基地址（不使用） */
#define GUI_RES_BASE           0

/* 存储在FLASH中的资源目录大小（不使用） */
#define GUI_CATALOG_SIZE      (0)


/*===========图片接口配置===gui_picture_port.c===============================================*/
/* 不使用文件系统图片接口 */
#define GUI_PIC_FS_EN          0

/* 不支持解码JPEG图片 */
#define GUI_PIC_JPEG_EN        0

/* 不支持解码PNG图片 */
#define GUI_PIC_PNG_EN        0

/* 不截图 */
#define GUI_PIC_CAPTURE_SCREEN_EN  0


/*===========是否加载各个APP===============================================*/

/* 不显示启动界面（需要外部资源） */
#define GUI_APP_BOOT_INTERFACE_EN     0

/* 不使用资源烧录 */
#define GUI_APP_RES_WRITER_EN         0
/*============================================================================*/

#endif /*__GUI_DRV_CFG_H__*/
