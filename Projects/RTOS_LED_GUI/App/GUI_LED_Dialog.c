/**
  ******************************************************************************
  * @file    GUI_LED_Dialog.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   emXGUI LED控制对话框
  *          通过触摸屏点击按钮控制红、绿、蓝三个LED的亮灭
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
#include <string.h>
#include <stdio.h>
#include "bsp_led.h"
#include "bsp_lcd.h"

/*===================================================================================*/
/* 全局变量和函数定义（供emXGUI库使用） */
/*===================================================================================*/

/* 字体变量 */
int ASCII_24_4BPP = 0;
int ASCII_16_4BPP = 0;
int ASCII_20_4BPP = 0;
int GB2312_16_2BPP = 0;
int GB2312_24_2BPP = 0;
int GB2312_24_4BPP = 0;
int LOGO_50_4BPP = 0;
int ICON_100_4BPP = 0;
int CONTROL_60_8BPP = 0;

/* 加载状态 */
BOOL Load_state = TRUE;

/* 触摸设备指针 */
void *pTouchDev = NULL;

/* GUI初始化函数 */
BOOL GUI_Init(void) { return TRUE; }
SURFACE* GUI_LCD_Init(void) { LCD_Init(); LCD_BackLed_Control(1); return NULL; }
void GUI_Touch_Init(void) {}
void GUI_Font_Init(void) { Load_state = TRUE; }
HFONT XFT_CreateFont(const void *font_data) { (void)font_data; return NULL; }

/*===================================================================================*/

/*
*************************************************************************
*                             按钮ID定义
*************************************************************************
*/
enum
{
    eID_EXIT = 0x1001,  /* 退出按钮ID */
    eID_LED_RED,        /* 红灯按钮ID */
    eID_LED_GREEN,      /* 绿灯按钮ID */
    eID_LED_BLUE,       /* 蓝灯按钮ID */
};

/*
*************************************************************************
*                             LED状态标志
*************************************************************************
*/
static uint8_t LED_Red_State = 0;    /* 红灯状态: 0=灭, 1=亮 */
static uint8_t LED_Green_State = 0;  /* 绿灯状态: 0=灭, 1=亮 */
static uint8_t LED_Blue_State = 0;   /* 蓝灯状态: 0=灭, 1=亮 */

/*
*************************************************************************
*                             绘制LED符号
*************************************************************************
*/
/**
  * @brief  绘制LED符号（圆形）
  * @param  hdc: 设备上下文句柄
  * @param  cx: 圆心X坐标
  * @param  cy: 圆心Y坐标
  * @param  radius: 半径
  * @param  color: LED颜色
  * @param  state: LED状态 (0=灭, 1=亮)
  * @retval 无
  */
static void DrawLED_Symbol(HDC hdc, int cx, int cy, int radius, uint16_t color, uint8_t state)
{
    /* 绘制外圈（灰色边框） */
    SetBrushColor(hdc, MapRGB(hdc, 180, 180, 180));
    FillCircle(hdc, cx, cy, radius + 3);

    /* 绘制LED本体 */
    if (state)
    {
        /* LED亮：显示彩色 */
        SetBrushColor(hdc, color);
    }
    else
    {
        /* LED灭：显示深灰色 */
        SetBrushColor(hdc, MapRGB(hdc, 80, 80, 80));
    }
    FillCircle(hdc, cx, cy, radius);

    /* 绘制高光效果（小白色圆） */
    if (state)
    {
        SetBrushColor(hdc, MapRGB(hdc, 255, 255, 255));
        FillCircle(hdc, cx - radius/3, cy - radius/3, radius/4);
    }
}

/*
*************************************************************************
*                             自定义按钮绘制
*************************************************************************
*/
/**
  * @brief  自定义按钮绘制函数
  * @param  ds: 绘制项结构体
  * @retval 无
  */
static void btn_owner_draw(DRAWITEM_HDR *ds)
{
    HDC hdc;
    RECT rc;
    WCHAR wbuf[128];

    hdc = ds->hDC;
    rc = ds->rc;

    /* 启用抗锯齿 */
    EnableAntiAlias(hdc, TRUE);

    /* 绘制按钮外框 */
    SetBrushColor(hdc, MapRGB(hdc, 66, 254, 255));
    FillRoundRect(hdc, &rc, MIN(rc.h, rc.w));

    if (ds->State & BST_PUSHED)
    {
        /* 按钮按下状态 */
        OffsetRect(&rc, 1, 1);
        SetTextColor(hdc, MapRGB(hdc, 200, 200, 200));
    }
    else
    {
        /* 按钮弹起状态 */
        SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
    }

    InflateRect(&rc, -5, -5);
    SetBrushColor(hdc, MapRGB(hdc, 13, 148, 214));
    FillRoundRect(hdc, &rc, MIN(rc.h, rc.w));

    /* 关闭抗锯齿 */
    EnableAntiAlias(hdc, FALSE);

    /* 获取按钮文本 */
    GetWindowText(ds->hwnd, wbuf, 128);

    /* 显示文本 */
    DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);
}

/**
  * @brief  退出按钮绘制函数
  * @param  ds: 绘制项结构体
  * @retval 无
  */
static void exit_btn_owner_draw(DRAWITEM_HDR *ds)
{
    HDC hdc;
    RECT rc;

    hdc = ds->hDC;
    rc = ds->rc;

    if (ds->State & BST_PUSHED)
    {
        SetPenColor(hdc, MapRGB(hdc, 1, 191, 255));
    }
    else
    {
        SetPenColor(hdc, MapRGB(hdc, 250, 250, 250));
    }

    SetPenSize(hdc, 2);
    InflateRect(&rc, 0, -1);

    /* 绘制三条横线（菜单图标） */
    for (int i = 0; i < 3; i++)
    {
        HLine(hdc, rc.x, rc.y, rc.w);
        rc.y += 9;
    }
}

/*
*************************************************************************
*                             窗口过程函数
*************************************************************************
*/
/**
  * @brief  窗口过程函数
  * @param  hwnd: 窗口句柄
  * @param  msg: 消息ID
  * @param  wParam: 消息参数1
  * @param  lParam: 消息参数2
  * @retval 消息处理结果
  */
static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            /* 初始化LED GPIO */
            LED_GPIO_Config();

            RECT rc;
            GetClientRect(hwnd, &rc);

            /* 创建退出按钮 */
            CreateWindow(BUTTON, L"X",
                        WS_TRANSPARENT | BS_FLAT | BS_NOTIFY | WS_OWNERDRAW | WS_VISIBLE,
                        430, 10, 40, 40, hwnd, eID_EXIT, NULL, NULL);

            /* 创建红灯控制按钮 */
            CreateWindow(BUTTON, L"RED",
                        WS_TRANSPARENT | BS_FLAT | BS_NOTIFY | WS_OWNERDRAW | WS_VISIBLE,
                        30, 200, 120, 50, hwnd, eID_LED_RED, NULL, NULL);

            /* 创建绿灯控制按钮 */
            CreateWindow(BUTTON, L"GREEN",
                        WS_TRANSPARENT | BS_FLAT | BS_NOTIFY | WS_OWNERDRAW | WS_VISIBLE,
                        180, 200, 120, 50, hwnd, eID_LED_GREEN, NULL, NULL);

            /* 创建蓝灯控制按钮 */
            CreateWindow(BUTTON, L"BLUE",
                        WS_TRANSPARENT | BS_FLAT | BS_NOTIFY | WS_OWNERDRAW | WS_VISIBLE,
                        330, 200, 120, 50, hwnd, eID_LED_BLUE, NULL, NULL);

            break;
        }

        case WM_PAINT:
        {
            HDC hdc;
            PAINTSTRUCT ps;
            RECT rc_title;
            RECT rc_client;

            hdc = BeginPaint(hwnd, &ps);
            GetClientRect(hwnd, &rc_client);

            /* 绘制标题栏背景 */
            rc_title = rc_client;
            rc_title.h = 60;
            SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
            FillRect(hdc, &rc_title);

            /* 绘制标题文字 */
            SetFont(hdc, defaultFont);
            SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
            DrawText(hdc, L"LED Control Panel", -1, &rc_title, DT_VCENTER | DT_CENTER);

            /* 绘制分隔线 */
            RECT rc_line = {0, 60, LCD_XSIZE, 4};
            GradientFillRect(hdc, &rc_line,
                           MapRGB(hdc, 150, 150, 150),
                           MapRGB(hdc, 220, 220, 220), TRUE);

            /* 绘制内容区域背景 */
            RECT rc_content = {0, 64, LCD_XSIZE, LCD_YSIZE - 64};
            SetBrushColor(hdc, MapRGB(hdc, 240, 240, 240));
            FillRect(hdc, &rc_content);

            /* 绘制LED标签 */
            SetTextColor(hdc, MapRGB(hdc, 50, 50, 50));
            RECT rc_label_red = {30, 80, 120, 30};
            DrawText(hdc, L"Red LED", -1, &rc_label_red, DT_VCENTER | DT_CENTER);

            RECT rc_label_green = {180, 80, 120, 30};
            DrawText(hdc, L"Green LED", -1, &rc_label_green, DT_VCENTER | DT_CENTER);

            RECT rc_label_blue = {330, 80, 120, 30};
            DrawText(hdc, L"Blue LED", -1, &rc_label_blue, DT_VCENTER | DT_CENTER);

            /* 绘制LED符号 */
            DrawLED_Symbol(hdc, 90, 140, 25,
                          MapRGB(hdc, 255, 0, 0), LED_Red_State);

            DrawLED_Symbol(hdc, 240, 140, 25,
                          MapRGB(hdc, 0, 255, 0), LED_Green_State);

            DrawLED_Symbol(hdc, 390, 140, 25,
                          MapRGB(hdc, 0, 0, 255), LED_Blue_State);

            /* 绘制说明文字 */
            RECT rc_info = {0, 260, LCD_XSIZE, 20};
            SetTextColor(hdc, MapRGB(hdc, 100, 100, 100));
            DrawText(hdc, L"Click button to toggle LED", -1, &rc_info, DT_VCENTER | DT_CENTER);

            EndPaint(hwnd, &ps);
            break;
        }

        case WM_DRAWITEM:
        {
            DRAWITEM_HDR *ds;
            ds = (DRAWITEM_HDR*)lParam;

            switch (ds->ID)
            {
                case eID_EXIT:
                {
                    exit_btn_owner_draw(ds);
                    return TRUE;
                }

                case eID_LED_RED:
                case eID_LED_GREEN:
                case eID_LED_BLUE:
                {
                    btn_owner_draw(ds);
                    return TRUE;
                }
            }
            break;
        }

        case WM_NOTIFY:
        {
            uint16_t code, id;
            id = LOWORD(wParam);
            code = HIWORD(wParam);

            /* 按钮点击事件 */
            if (code == BN_CLICKED)
            {
                switch (id)
                {
                    case eID_EXIT:
                    {
                        /* 关闭所有LED */
                        LED_RGBOFF;
                        LED_Red_State = 0;
                        LED_Green_State = 0;
                        LED_Blue_State = 0;
                        PostCloseMessage(hwnd);
                        break;
                    }

                    case eID_LED_RED:
                    {
                        /* 切换红灯状态 */
                        LED_Red_State = !LED_Red_State;
                        if (LED_Red_State) {
                            LED1_ON;
                        } else {
                            LED1_OFF;
                        }
                        /* 刷新显示 */
                        InvalidateRect(hwnd, NULL, FALSE);
                        break;
                    }

                    case eID_LED_GREEN:
                    {
                        /* 切换绿灯状态 */
                        LED_Green_State = !LED_Green_State;
                        if (LED_Green_State) {
                            LED2_ON;
                        } else {
                            LED2_OFF;
                        }
                        /* 刷新显示 */
                        InvalidateRect(hwnd, NULL, FALSE);
                        break;
                    }

                    case eID_LED_BLUE:
                    {
                        /* 切换蓝灯状态 */
                        LED_Blue_State = !LED_Blue_State;
                        if (LED_Blue_State) {
                            LED3_ON;
                        } else {
                            LED3_OFF;
                        }
                        /* 刷新显示 */
                        InvalidateRect(hwnd, NULL, FALSE);
                        break;
                    }
                }
            }
            break;
        }

        case WM_DESTROY:
        {
            /* 关闭所有LED */
            LED_RGBOFF;
            LED_Red_State = 0;
            LED_Green_State = 0;
            LED_Blue_State = 0;
            return PostQuitMessage(hwnd);
        }

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return WM_NULL;
}

/*
*************************************************************************
*                             LED控制对话框入口
*************************************************************************
*/
/**
  * @brief  LED控制对话框入口函数
  * @param  无
  * @retval 无
  */
void GUI_LED_Dialog(void)
{
    WNDCLASS wcex;
    MSG msg;
    HWND hwnd;

    wcex.Tag = WNDCLASS_TAG;

    wcex.Style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = win_proc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = NULL;
    wcex.hIcon = NULL;
    wcex.hCursor = NULL;

    /* 创建窗口 */
    hwnd = CreateWindowEx(WS_EX_NOFOCUS | WS_EX_FRAMEBUFFER,
                          &wcex,
                          L"GUI_LED_Dialog",
                          WS_VISIBLE | WS_CLIPCHILDREN,
                          0, 0, LCD_XSIZE, LCD_YSIZE,
                          NULL, NULL, NULL, NULL);

    /* 显示窗口 */
    ShowWindow(hwnd, SW_SHOW);

    /* 进入消息循环 */
    while (GetMessage(&msg, hwnd))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

/********************************END OF FILE****************************/
