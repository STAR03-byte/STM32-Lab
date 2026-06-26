/**
  **********************************************************************
  * @file    GUI_LED_KEY_Dialog.c
  * @version V2.0
  * @date    2026-06-26
  * @brief   LED 灯控制对话框
  *          在 LCD 图形界面上显示红、绿、蓝 LED 灯符号，
  *          通过点击三个按键控制红、绿、蓝三个 LED 的亮灭。
  *          基于 FreeRTOS + emXGUI 实现。
  **********************************************************************
  */

#include <emXGUI.h>
#include <string.h>
#include "GUI_AppDef.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h"

/*============================================================================
 *  控件 ID
 *============================================================================*/
enum
{
  eID_EXIT      = 0x1001,   /* 退出按钮 */
  eID_LED_RED   = 0x1002,   /* 红色 LED 按钮 */
  eID_LED_GREEN = 0x1003,   /* 绿色 LED 按钮 */
  eID_LED_BLUE  = 0x1004,   /* 蓝色 LED 按钮 */
};

/*============================================================================
 *  LED 状态（0=灭, 1=亮）
 *============================================================================*/
static uint8_t led_red_state   = 0;
static uint8_t led_green_state = 0;
static uint8_t led_blue_state  = 0;

/*============================================================================
 *  DrawLED_Symbol -- 绘制圆形 LED 指示灯
 *============================================================================*/
static void DrawLED_Symbol(HDC hdc, int cx, int cy, int radius,
                           COLORREF color_on, uint8_t state)
{
  /* 1. 灰色外圈边框 */
  SetBrushColor(hdc, MapRGB(hdc, 180, 180, 180));
  SetPenColor(hdc, MapRGB(hdc, 180, 180, 180));
  FillCircle(hdc, cx, cy, radius + 3);

  /* 2. LED 主体 */
  if (state)
  {
    SetBrushColor(hdc, color_on);
    SetPenColor(hdc, color_on);
  }
  else
  {
    SetBrushColor(hdc, MapRGB(hdc, 80, 80, 80));
    SetPenColor(hdc, MapRGB(hdc, 80, 80, 80));
  }
  FillCircle(hdc, cx, cy, radius);

  /* 3. 亮灯时绘制高光点 */
  if (state)
  {
    int hl_r = radius / 3;
    SetBrushColor(hdc, MapRGB(hdc, 255, 255, 255));
    SetPenColor(hdc, MapRGB(hdc, 255, 255, 255));
    FillCircle(hdc, cx - radius / 3, cy - radius / 3, hl_r);
  }
}

/*============================================================================
 *  btn_owner_draw -- 自绘按钮外观
 *============================================================================*/
static void btn_owner_draw(DRAWITEM_HDR *ds, uint8_t r, uint8_t g, uint8_t b)
{
  HDC hdc;
  RECT rc;
  WCHAR wbuf[64];

  hdc = ds->hDC;
  rc  = ds->rc;

  EnableAntiAlias(hdc, TRUE);

  /* 外层圆角矩形（亮色边框） */
  SetBrushColor(hdc, MapRGB(hdc, 66, 254, 255));
  FillRoundRect(hdc, &rc, MIN(rc.h, rc.w));

  /* 按下时文字偏移 + 颜色变暗 */
  if (ds->State & BST_PUSHED)
  {
    OffsetRect(&rc, 1, 1);
    SetTextColor(hdc, MapRGB(hdc, 200, 200, 200));
  }
  else
  {
    SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
  }

  /* 内层圆角矩形（使用对应 LED 颜色） */
  InflateRect(&rc, -5, -5);
  SetBrushColor(hdc, MapRGB(hdc, r, g, b));
  FillRoundRect(hdc, &rc, MIN(rc.h, rc.w));

  EnableAntiAlias(hdc, FALSE);

  /* 按钮文字 */
  GetWindowText(ds->hwnd, wbuf, 64);
  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);
}

/*============================================================================
 *  exit_btn_owner_draw -- 退出按钮自绘（三条横线）
 *============================================================================*/
static void exit_btn_owner_draw(DRAWITEM_HDR *ds)
{
  HDC hdc;
  RECT rc;

  hdc = ds->hDC;
  rc  = ds->rc;

  if (ds->State & BST_PUSHED)
    SetPenColor(hdc, MapRGB(hdc, 1, 191, 255));
  else
    SetPenColor(hdc, MapRGB(hdc, 250, 250, 250));

  SetPenSize(hdc, 2);
  InflateRect(&rc, 0, -1);

  for (int i = 0; i < 4; i++)
  {
    HLine(hdc, rc.x, rc.y, rc.w);
    rc.y += 9;
  }
}

/*============================================================================
 *  win_proc -- 窗口消息处理回调
 *============================================================================*/
static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
    /*------------------------------------------------------------
     *  WM_CREATE: 创建控件、启动定时器
     *------------------------------------------------------------*/
    case WM_CREATE:
    {
      /* 初始化按键 GPIO */
      Key_GPIO_Config();

      /* 退出按钮（右上角） */
      CreateWindow(BUTTON, L"X",
                   WS_TRANSPARENT | BS_FLAT | BS_NOTIFY | WS_OWNERDRAW | WS_VISIBLE,
                   740, 15, 40, 40, hwnd, eID_EXIT, NULL, NULL);

      /* 三个 LED 控制按钮 */
      CreateWindow(BUTTON, L"Red",
                   WS_TRANSPARENT | BS_FLAT | BS_NOTIFY | WS_OWNERDRAW | WS_VISIBLE,
                   60,  360, 160, 60, hwnd, eID_LED_RED, NULL, NULL);
      CreateWindow(BUTTON, L"Green",
                   WS_TRANSPARENT | BS_FLAT | BS_NOTIFY | WS_OWNERDRAW | WS_VISIBLE,
                   320, 360, 160, 60, hwnd, eID_LED_GREEN, NULL, NULL);
      CreateWindow(BUTTON, L"Blue",
                   WS_TRANSPARENT | BS_FLAT | BS_NOTIFY | WS_OWNERDRAW | WS_VISIBLE,
                   580, 360, 160, 60, hwnd, eID_LED_BLUE, NULL, NULL);

      /* 定时器 0: 每 20ms 扫描物理按键 */
      SetTimer(hwnd, 0, 20, TMR_START, NULL);

      break;
    }

    /*------------------------------------------------------------
     *  WM_TIMER: 物理按键扫描
     *------------------------------------------------------------*/
    case WM_TIMER:
    {
      if (wParam == 0)
      {
        /* KEY1 (PA0) 切换绿色 LED */
        if (Key_Scan(KEY1_GPIO_PORT, KEY1_PIN))
        {
          led_green_state ^= 1;
          GPIO_WriteBit(LED2_GPIO_PORT, LED2_PIN, led_green_state ? Bit_RESET : Bit_SET);
          InvalidateRect(hwnd, NULL, FALSE);
        }
        /* KEY2 (PC13) 切换蓝色 LED */
        if (Key_Scan(KEY2_GPIO_PORT, KEY2_PIN))
        {
          led_blue_state ^= 1;
          GPIO_WriteBit(LED3_GPIO_PORT, LED3_PIN, led_blue_state ? Bit_RESET : Bit_SET);
          InvalidateRect(hwnd, NULL, FALSE);
        }
      }
      break;
    }

    /*------------------------------------------------------------
     *  WM_PAINT: 界面绘制
     *------------------------------------------------------------*/
    case WM_PAINT:
    {
      HDC hdc;
      PAINTSTRUCT ps;
      RECT rc_title = {0, 0, GUI_XSIZE, 70};
      RECT rc_sep   = {0, 70, GUI_XSIZE, 4};
      RECT rc_body  = {0, 74, GUI_XSIZE, GUI_YSIZE - 74};
      RECT rc_label;

      hdc = BeginPaint(hwnd, &ps);

      /* --- 标题栏 --- */
      SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
      FillRect(hdc, &rc_title);
      SetFont(hdc, defaultFont);
      SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
      DrawText(hdc, L"LED Control Panel", -1, &rc_title, DT_VCENTER | DT_CENTER);

      /* --- 渐变分隔线 --- */
      GradientFillRect(hdc, &rc_sep,
                       MapRGB(hdc, 150, 150, 150),
                       MapRGB(hdc, 220, 220, 220), TRUE);

      /* --- 内容区域背景 --- */
      SetBrushColor(hdc, MapRGB(hdc, 240, 240, 240));
      FillRect(hdc, &rc_body);

      /* --- LED 标签文字 --- */
      SetTextColor(hdc, MapRGB(hdc, 60, 60, 60));

      rc_label = (RECT){100, 100, 120, 30};
      DrawText(hdc, L"Red LED", -1, &rc_label, DT_CENTER);

      rc_label = (RECT){340, 100, 120, 30};
      DrawText(hdc, L"Green LED", -1, &rc_label, DT_CENTER);

      rc_label = (RECT){580, 100, 120, 30};
      DrawText(hdc, L"Blue LED", -1, &rc_label, DT_CENTER);

      /* --- 绘制三个圆形 LED 指示灯 --- */
      DrawLED_Symbol(hdc, 160, 220, 35, MapRGB(hdc, 255, 0, 0),   led_red_state);
      DrawLED_Symbol(hdc, 400, 220, 35, MapRGB(hdc, 0, 255, 0),   led_green_state);
      DrawLED_Symbol(hdc, 640, 220, 35, MapRGB(hdc, 0, 100, 255), led_blue_state);

      /* --- 状态文字 --- */
      rc_label = (RECT){100, 290, 120, 25};
      SetTextColor(hdc, led_red_state ? MapRGB(hdc, 255, 0, 0) : MapRGB(hdc, 150, 150, 150));
      DrawText(hdc, led_red_state ? L"ON" : L"OFF", -1, &rc_label, DT_CENTER);

      rc_label = (RECT){340, 290, 120, 25};
      SetTextColor(hdc, led_green_state ? MapRGB(hdc, 0, 200, 0) : MapRGB(hdc, 150, 150, 150));
      DrawText(hdc, led_green_state ? L"ON" : L"OFF", -1, &rc_label, DT_CENTER);

      rc_label = (RECT){580, 290, 120, 25};
      SetTextColor(hdc, led_blue_state ? MapRGB(hdc, 0, 100, 255) : MapRGB(hdc, 150, 150, 150));
      DrawText(hdc, led_blue_state ? L"ON" : L"OFF", -1, &rc_label, DT_CENTER);

      /* --- 提示信息 --- */
      rc_label = (RECT){0, 430, GUI_XSIZE, 30};
      SetTextColor(hdc, MapRGB(hdc, 120, 120, 120));
      DrawText(hdc, L"Touch button or press KEY1/KEY2 to toggle LED", -1, &rc_label, DT_CENTER);

      EndPaint(hwnd, &ps);
      break;
    }

    /*------------------------------------------------------------
     *  WM_DRAWITEM: 自绘按钮
     *------------------------------------------------------------*/
    case WM_DRAWITEM:
    {
      DRAWITEM_HDR *ds = (DRAWITEM_HDR *)lParam;

      switch (ds->ID)
      {
        case eID_EXIT:
          exit_btn_owner_draw(ds);
          return TRUE;

        case eID_LED_RED:
          btn_owner_draw(ds, 200, 30, 30);
          return TRUE;

        case eID_LED_GREEN:
          btn_owner_draw(ds, 30, 160, 30);
          return TRUE;

        case eID_LED_BLUE:
          btn_owner_draw(ds, 30, 80, 200);
          return TRUE;
      }
      break;
    }

    /*------------------------------------------------------------
     *  WM_NOTIFY: 按钮点击事件
     *------------------------------------------------------------*/
    case WM_NOTIFY:
    {
      u16 id   = LOWORD(wParam);
      u16 code = HIWORD(wParam);

      if (code != BN_CLICKED)
        break;

      switch (id)
      {
        case eID_EXIT:
          /* 关闭对话框前熄灭所有 LED */
          LED_RGBOFF;
          led_red_state   = 0;
          led_green_state = 0;
          led_blue_state  = 0;
          PostCloseMessage(hwnd);
          break;

        case eID_LED_RED:
          led_red_state ^= 1;
          GPIO_WriteBit(LED1_GPIO_PORT, LED1_PIN, led_red_state ? Bit_RESET : Bit_SET);
          InvalidateRect(hwnd, NULL, FALSE);
          break;

        case eID_LED_GREEN:
          led_green_state ^= 1;
          GPIO_WriteBit(LED2_GPIO_PORT, LED2_PIN, led_green_state ? Bit_RESET : Bit_SET);
          InvalidateRect(hwnd, NULL, FALSE);
          break;

        case eID_LED_BLUE:
          led_blue_state ^= 1;
          GPIO_WriteBit(LED3_GPIO_PORT, LED3_PIN, led_blue_state ? Bit_RESET : Bit_SET);
          InvalidateRect(hwnd, NULL, FALSE);
          break;
      }
      break;
    }

    /*------------------------------------------------------------
     *  WM_DESTROY: 清理
     *------------------------------------------------------------*/
    case WM_DESTROY:
      LED_RGBOFF;
      led_red_state   = 0;
      led_green_state = 0;
      led_blue_state  = 0;
      return PostQuitMessage(hwnd);

    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }

  return WM_NULL;
}

/*============================================================================
 *  GUI_LED_KEY_Dialog -- 对话框入口函数
 *============================================================================*/
void GUI_LED_KEY_Dialog(void)
{
  WNDCLASS wcex;
  MSG msg;
  HWND hwnd;

  wcex.Tag          = WNDCLASS_TAG;
  wcex.Style        = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc  = win_proc;
  wcex.cbClsExtra   = 0;
  wcex.cbWndExtra   = 0;
  wcex.hInstance    = NULL;
  wcex.hIcon        = NULL;
  wcex.hCursor      = NULL;

  hwnd = CreateWindowEx(WS_EX_NOFOCUS | WS_EX_FRAMEBUFFER,
                        &wcex,
                        L"GUI_LED_KEY_Dialog",
                        WS_VISIBLE | WS_CLIPCHILDREN,
                        0, 0, GUI_XSIZE, GUI_YSIZE,
                        NULL, NULL, NULL, NULL);

  ShowWindow(hwnd, SW_SHOW);

  while (GetMessage(&msg, hwnd))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}
