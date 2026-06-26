#include "emXGUI.h"
#include "./clock/GUI_CLOCK_DIALOG.h"
#include "x_libc.h"
#include <stdlib.h>
#include "string.h"
#include "ff.h"
#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"
#include "emxgui_png.h"
#include  ".\clock\rtc\bsp_rtc.h"

#define ICON_BTN_NUM     3     // 按钮数量
#define ICON_TEXT_NUM   (1 + ICON_BTN_NUM)    // 文本数量

#define CLOCK_BACK_COLOR     230, 230, 230

RTC_DateTypeDef RTC_Date;

struct
{
	uint8_t page;    // 当前设置页面
	uint8_t dial;    // 选中的表盘
}Set_Start;

uint8_t clock_dial = 0;    // 表盘
static HWND clock_hwnd;

const WCHAR Week_List[][4] = {{L"星期日"}, {L"星期一"}, {L"星期二"}, {L"星期三"}, {L"星期四"}, {L"星期五"}, {L"星期六"}};
const uint8_t month_list[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};    // 记录每月的最多天数

//图标管理数组
const clock_icon_t clock_icon[] = {

  /* 按钮 */
  {L"O",              {740,  25,  36,  36},  ID_CLOCK_EXIT},            // 0. 退出
  {L"2019年",         {456, 164, 230, 230},  ID_CLOCK_DATE},            // 1. 日期（年月日周）
  {L" ",              {113, 164, 230, 230},  ID_CLOCK_TIME},            // 2. 表盘显示区域
  
  /* 文本 */
  {L"时钟&日期",      {100,   0, 600,  80},  ID_CLOCK_TITLE},           // 3. 标题
  
  /************ 设置窗口控件 **************/
  /* 按钮 */
  {L"F",              {  0,  16,  80,  48},  ID_CLOCK_WAIVE},           // 4. 放弃设置
  {L"完成",           {317, 390, 166,  70},  ID_CLOCK_OK},              // 5. 完成设置

  /* 文本 */
  {L"设置时间",       {352,  23,  96,  33},  ID_CLOCK_SETTITLE},        // 6. 设置窗口标题
  {L"00:00",          {314, 105, 173,  30},  ID_CLOCK_SETTIME},         // 7. 设置窗口标题
  {L"2000年01月01日", {314, 105, 173,  30},  ID_CLOCK_SETDATE},         // 8. 设置窗口标题

  /* 时钟&日历选择列表 */
  {L" ",              {230, 138, 170, 220},  ID_CLOCK_SetHour},         // 9.  设置小时的列表
  {L" ",              {400, 138, 169, 220},  ID_CLOCK_SetMinute},       // 10. 设置分钟的列表

  {L" ",              {230, 138, 113, 220},  ID_CLOCK_SetYear},         // 11. 设置年的列表
  {L" ",              {343, 138, 113, 220},  ID_CLOCK_SetMonth},        // 12. 设置月的列表
  {L" ",              {456, 138, 113, 220},  ID_CLOCK_SetDate},         // 13. 设置日的列表
 
};

static void exit_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
  HDC hdc;
  RECT rc;

	hdc = ds->hDC;   
	rc = ds->rc; 

  SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
  FillRect(hdc, &rc);

  if (ds->State & BST_PUSHED)
	{ //按钮是按下状态
		SetPenColor(hdc, MapRGB(hdc, 120, 120, 120));      //设置文字色
	}
	else
	{ //按钮是弹起状态

		SetPenColor(hdc, MapRGB(hdc, 250, 250, 250));
	}
  
  SetPenSize(hdc, 2);

  InflateRect(&rc, 0, -1);
  
  for(int i=0; i<4; i++)
  {
    HLine(hdc, rc.x, rc.y, rc.w);
    rc.y += 9;
  }
}

static void waive_btn_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HDC hdc;
	RECT rc;
  WCHAR wbuf[128];

	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.

  SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
  FillRect(hdc, &rc);

  if (ds->State & BST_PUSHED)
  { //按钮是按下状态
    SetTextColor(hdc, MapRGB(hdc, 120, 120, 120));
  }
  else
  { //按钮是弹起状态
    SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
  }
  
  HFONT controlFont_48;
  controlFont_48 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_48);
  
  GetWindowText(ds->hwnd, wbuf, 128); //获得按钮控件的文字
  SetFont(hdc, controlFont_48);
  /* 显示文本 */
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)
  
  DeleteFont(controlFont_48);
}

static void btn_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HDC hdc;
	RECT rc, rc_tmp;
  WCHAR wbuf[128];
  HWND hwnd;
  
  hwnd = ds->hwnd;
	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.

  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换

  EnableAntiAlias(hdc, TRUE);
  
  SetBrushColor(hdc, MapRGB(hdc, 66, 254, 255));
  FillRoundRect(hdc, &rc, MIN(rc.h, rc.w));

  if (ds->State & BST_PUSHED)
  { //按钮是按下状态
    OffsetRect(&rc, 1, 1);
    SetTextColor(hdc, MapRGB(hdc, 200, 200, 200));
  }
  else
  { //按钮是弹起状态
    SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
  }
  
  InflateRect(&rc, -5, -5);
  SetBrushColor(hdc, MapRGB(hdc, 13, 148, 214));
  FillRoundRect(hdc, &rc, MIN(rc.h, rc.w));
  
  EnableAntiAlias(hdc, FALSE);
  
  GetWindowText(ds->hwnd, wbuf, 128); //获得按钮控件的文字
  
  /* 显示文本 */
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)
}

/*
 * @brief  重绘列表
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void _draw_listbox(HDC hdc,HWND hwnd,COLOR_RGB32 text_c,COLOR_RGB32 back_c,COLOR_RGB32 sel_c, int fontsize)
{

	RECT rc,rc_cli;
	int i,count;
	WCHAR wbuf[128];


	GetClientRect(hwnd,&rc_cli);

	SetBrushColor(hdc,MapRGB888(hdc,back_c));
	FillRect(hdc,&rc_cli);

	SetTextColor(hdc,MapRGB888(hdc,text_c));

	i=SendMessage(hwnd,LB_GETTOPINDEX,0,0);
	count=SendMessage(hwnd,LB_GETCOUNT,0,0);
  
  HFONT controlFont_32;
  controlFont_32 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_32);

  if (fontsize == 32)
  {
    SetFont(hdc,controlFont_32);
  }
  else
  {
    SetFont(hdc,defaultFont);
  }

	while(i<count)
	{
		SendMessage(hwnd,LB_GETITEMRECT,i,(LPARAM)&rc);
		if(rc.y > rc_cli.h)
		{
			break;
		}

		SendMessage(hwnd,LB_GETTEXT,i,(LPARAM)wbuf);
		DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_CENTER|DT_VCENTER);

		i++;
	}
  
  DeleteFont(controlFont_32);
}

static void listbox_owner_draw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc,hdc0,hdc1;
	RECT rc_m;
	int x,y,w,h;

	hwnd =ds->hwnd;
	hdc =ds->hDC;

	//定义一个中间的矩形．
	rc_m.w =ds->rc.w;
	rc_m.h =44;
	rc_m.x =0;
	rc_m.y =(ds->rc.h-rc_m.h)>>1;

	//创建两个一样大小的DC,把listbox分别绘制进去，但颜色参数不同的.
	hdc0 =CreateMemoryDC(SURF_SCREEN,ds->rc.w,ds->rc.h);
	hdc1 =CreateMemoryDC(SURF_SCREEN,ds->rc.w,ds->rc.h);

	//一个listbox绘到hdc0中．
	_draw_listbox(hdc0,hwnd,RGB888(168,168,168),RGB888(250,250,250),RGB888(10,100,100), 24);

	//一个listbox绘到hdc1中．
	_draw_listbox(hdc1,hwnd,RGB888(236,126,65),RGB888(232,232,232),RGB888(150,200,220), 32);

	//中间框绘制到hdc1中．
	SetPenColor(hdc1,MapRGB(hdc1,212,212,212));
	// DrawRect(hdc1,&rc_m);

  HLine(hdc1, rc_m.x, rc_m.y, rc_m.x + rc_m.w);
  HLine(hdc1, rc_m.x, rc_m.y+rc_m.h-1, rc_m.x + rc_m.w);

	//上面的矩形部分丛hdc0里复制出来.
	x =0;
	y =0;
	w =rc_m.w;
	h =rc_m.y-ds->rc.y;
	BitBlt(hdc,x,y,w,h,hdc0,x,y,SRCCOPY);

	//中间矩形部分丛hdc1里复制出来.
	BitBlt(hdc,rc_m.x,rc_m.y,rc_m.w,rc_m.h,hdc1,rc_m.x,rc_m.y,SRCCOPY);

	//下面的矩形部分丛hdc0里复制出来.
	x =0;
	y =rc_m.y+rc_m.h;
	w =rc_m.w;
	h =ds->rc.h-(rc_m.y+rc_m.h);
	BitBlt(hdc,x,y,w,h,hdc0,x,y,SRCCOPY);


	DeleteDC(hdc0);
	DeleteDC(hdc1);

}



static void Dial_OwnerDraw(DRAWITEM_HDR *ds)  // 绘制表盘
{
  HDC hdc;
	RECT rc;
  WCHAR wbuf[128];
  uint8_t Sec = 0;
  uint8_t Min = 0;
  uint8_t Hour = 0;

	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.

	
  SetBrushColor(hdc, MapRGB(hdc, CLOCK_BACK_COLOR));
  FillRect(hdc, &rc);

  RTC_TIME rtc_time;
  RTC_GetTime(RTC_Format_BIN, &rtc_time.RTC_Time);
  Hour = rtc_time.RTC_Time.RTC_Hours;
  Min  = rtc_time.RTC_Time.RTC_Minutes;
  Sec  = rtc_time.RTC_Time.RTC_Seconds;

	
	
  EnableAntiAlias(hdc, TRUE);
  
  SetBrushColor(hdc, MapRGB(hdc, 255, 255, 255));
  FillCircle(hdc, rc.w/2, rc.h/2, MIN(rc.w, rc.h)/2);

  SetPenSize(hdc, 2);
  SetPenColor(hdc, MapRGB(hdc, 10, 10, 10));
  DrawCircle(hdc, rc.w/2, rc.h/2, MIN(rc.w, rc.h)/2);
  
  SetBrushColor(hdc, MapRGB(hdc, 128, 128, 255));
  FillArc(hdc, rc.w/2, rc.h/2, 0, MIN(rc.w, rc.h)/2 - 1, 90, Sec / 60.0 * 360 + 90);
  
  x_wsprintf(wbuf, L"%02d %02d", Hour, Min);
  
  HFONT controlFont_64;
  controlFont_64 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_64);
  
  SetFont(hdc, controlFont_64);
  SetTextColor(hdc, MapRGB(hdc, 10, 10, 10));
  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);
  
  /* 画两个圆点 */
  SetBrushColor(hdc, MapRGB(hdc, 10, 10, 10));
  FillCircle(hdc, rc.w/2, rc.h/2 - 10, 5);
  FillCircle(hdc, rc.w/2, rc.h/2 + 10, 5);

  DeleteFont(controlFont_64);
  EnableAntiAlias(hdc, FALSE);
	
	
}

/*
 * @brief  重绘日期显示区域
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void date_OwnerDraw(DRAWITEM_HDR *ds)
{
  HDC hdc;
	RECT rc;
  WCHAR wbuf[20];

	
  RECT rc1 = {50, 27, 130, 72};
  RECT rc3 = {79, 161, 72, 36};


	
	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.

  SetBrushColor(hdc, MapRGB(hdc, CLOCK_BACK_COLOR));
  FillRect(hdc, &rc);
  
  EnableAntiAlias(hdc, TRUE);
  
  SetBrushColor(hdc, MapRGB(hdc, 255, 255, 255));
  FillCircle(hdc, rc.w/2, rc.h/2, MIN(rc.w, rc.h)/2);

  SetPenSize(hdc, 2);
  SetPenColor(hdc, MapRGB(hdc, 10, 10, 10));
  DrawCircle(hdc, rc.w/2, rc.h/2, MIN(rc.w, rc.h)/2);
  
  EnableAntiAlias(hdc, FALSE);
  
  /* 创建字体 */
  HFONT controlFont_48;
  controlFont_48 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_48);
	
  /* 显示年 */
  x_wsprintf(wbuf, L"%d", 2000+RTC_Date.RTC_Year);
  
  SetTextColor(hdc, MapRGB(hdc, 10, 10, 10));
  
  SetFont(hdc, defaultFont);
  rc1.y += 6;
  DrawText(hdc, L"           年", -1, &rc1, DT_VCENTER|DT_CENTER);
  rc1.y -= 6;
  
  SetFont(hdc, controlFont_48);
  DrawText(hdc, wbuf, -1, &rc1, DT_VCENTER|DT_CENTER);
  
  /* 显示月日 */
  RECT rc_mont    = {29,  89 + 5,  58, 55};
  RECT rc_mont_zh = {90,  115, 27, 29};
  RECT rc_mday    = {117,  89 + 5, 56, 55};
  RECT rc_mday_zh = {173, 115, 27, 30};
  
  x_wsprintf(wbuf, L"%d", RTC_Date.RTC_Month);
  
  SetFont(hdc, controlFont_48);
  DrawText(hdc, wbuf, -1, &rc_mont, DT_RIGHT | DT_BOTTOM);    // 显示月份
  
  SetFont(hdc, defaultFont);
  DrawText(hdc, L"月", -1, &rc_mont_zh, DT_RIGHT | DT_BOTTOM);    // 显示“月”
  
  x_wsprintf(wbuf, L"%d", RTC_Date.RTC_Date);
  
  SetFont(hdc, controlFont_48);
  DrawText(hdc, wbuf, -1, &rc_mday, DT_RIGHT | DT_BOTTOM);     // 显示日期
  
  SetFont(hdc, defaultFont);
  DrawText(hdc, L"日", -1, &rc_mday_zh, DT_RIGHT | DT_BOTTOM);     // 显示“日”
  
  /* 显示周 */
  SetFont(hdc, defaultFont);
  DrawText(hdc, Week_List[RTC_Date.RTC_WeekDay - 1], -1, &rc3, DT_VCENTER|DT_CENTER);

  DeleteFont(controlFont_48);
}

/*
 * @brief  重绘标题透明文本
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void Title_Textbox_OwnerDraw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
  RECT rc;
	WCHAR wbuf[128];

  hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
  rc = ds->rc;

  /* 背景 */
  SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
  FillRect(hdc, &rc);

	SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	GetWindowText(hwnd, wbuf, 128);                        // 获得按钮控件的文字
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);    // 绘制文字(居中对齐方式)
}

/*
 * @brief  重绘白色背景文本
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void WhiteBK_Textbox_OwnerDraw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
  RECT rc;
	WCHAR wbuf[128];

  hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
  rc = ds->rc;

  /* 背景 */
  SetBrushColor(hdc, MapRGB(hdc, 250, 250, 250));
  FillRect(hdc, &rc);

	SetTextColor(hdc, MapRGB(hdc, 10, 10, 10));
	GetWindowText(hwnd, wbuf, 128);                        // 获得按钮控件的文字
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);    // 绘制文字(居中对齐方式)
}

/*
 * @brief  获得列表选中项的数值（字符串转整型）
 * @param  hwnd:	窗口句柄
 * @param  ID：   列表 ID
 * @retval NONE
*/
static uint16_t GetListCurselVal(HWND hwnd, uint32_t ID)
{
  HWND wnd;
  uint8_t csl;    // 选中项
  WCHAR wbuf[10];
  char cbuf[10];

  wnd = GetDlgItem(hwnd, ID);
  csl = SendMessage(wnd, LB_GETCURSEL, 0, 0);    // 获得选中项
  SendMessage(wnd, LB_GETTEXT, csl, (LPARAM)wbuf);       // 获得选中项的文本
  x_wcstombs(cbuf, wbuf, 9);

  return x_atoi(cbuf);                           // 返回选中项的值
}


static LRESULT setting_win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg){
      case WM_CREATE:
      {
        
        for (uint8_t xC=4; xC<6; xC++)     //  按钮
        {
          /* 循环创建按钮 */
          CreateWindow(BUTTON, clock_icon[xC].icon_name, WS_OWNERDRAW | WS_VISIBLE,
                        clock_icon[xC].rc.x, clock_icon[xC].rc.y,
                        clock_icon[xC].rc.w, clock_icon[xC].rc.h,
                        hwnd, clock_icon[xC].id, NULL, NULL); 
        }
        
        /* 创建设置标题文本框 */
        CreateWindow(TEXTBOX, clock_icon[6].icon_name, WS_OWNERDRAW | WS_VISIBLE | WS_OVERLAPPED,
                      clock_icon[6].rc.x, clock_icon[6].rc.y,
                      clock_icon[6].rc.w,clock_icon[6].rc.h,
                      hwnd, clock_icon[6].id, NULL, NULL);
        
        /* 根据设置时间或设置日期来创建不同的控件 */
        if (Set_Start.page == 0)
        {
          SetWindowText(GetDlgItem(hwnd, ID_CLOCK_SETTITLE), L"设置时间");
          
          /* 循环时间显示7文本框 */
          CreateWindow(TEXTBOX, clock_icon[7].icon_name, WS_OWNERDRAW | WS_VISIBLE | WS_OVERLAPPED,
                        clock_icon[7].rc.x, clock_icon[7].rc.y,
                        clock_icon[7].rc.w,clock_icon[7].rc.h,
                          hwnd, clock_icon[7].id, NULL, NULL);

          for (uint8_t xC=9; xC<11; xC++)
          {
            /* 创建设置列表（创建为不立即可见） */
            CreateWindow(LISTBOX, clock_icon[xC].icon_name, WS_OWNERDRAW | LBS_NOTIFY | WS_OVERLAPPED | WS_VISIBLE,
                              clock_icon[xC].rc.x, clock_icon[xC].rc.y, clock_icon[xC].rc.w,
                              clock_icon[xC].rc.h, hwnd, clock_icon[xC].id, NULL, NULL);
          }

          for (uint8_t xC=0; xC<24; xC++)
          {
            WCHAR wbuf[10];
            HWND ListHwnd = GetDlgItem(hwnd, ID_CLOCK_SetHour);

            /* 在列表中增加小时 */
            x_wsprintf(wbuf, L"%02d", xC);
            SendMessage(ListHwnd, LB_ADDSTRING, xC, (LPARAM)wbuf);
            SendMessage(ListHwnd, LB_SETITEMHEIGHT, xC, (LPARAM)44);
          }
          
          for (uint8_t xC=0; xC<60; xC++)
          {
            WCHAR wbuf[10];
            HWND ListHwnd = GetDlgItem(hwnd, ID_CLOCK_SetMinute);

            /* 在列表中增加分钟 */
            x_wsprintf(wbuf, L"%02d", xC);
            SendMessage(ListHwnd, LB_ADDSTRING, xC, (LPARAM)wbuf);
            SendMessage(ListHwnd, LB_SETITEMHEIGHT, xC, (LPARAM)44);
          }
        }
        else
        {
          SetWindowText(GetDlgItem(hwnd, ID_CLOCK_SETTITLE), L"设置日期");
          
          /* 循环时间显示7文本框 */
          CreateWindow(TEXTBOX, clock_icon[8].icon_name, WS_OWNERDRAW | WS_VISIBLE | WS_OVERLAPPED,
                        clock_icon[8].rc.x, clock_icon[8].rc.y,
                        clock_icon[8].rc.w,clock_icon[8].rc.h,
                          hwnd, clock_icon[8].id, NULL, NULL);

          for (uint8_t xC=11; xC<14; xC++)
          {
            /* 创建设置列表 */
            CreateWindow(LISTBOX, clock_icon[xC].icon_name, WS_OWNERDRAW | LBS_NOTIFY | WS_OVERLAPPED | WS_VISIBLE,
                              clock_icon[xC].rc.x, clock_icon[xC].rc.y, clock_icon[xC].rc.w,
                              clock_icon[xC].rc.h, hwnd, clock_icon[xC].id, NULL, NULL);
          }
          
          uint8_t i=0;
          for (uint16_t xC=2000; xC<2050; xC++)
          {
            WCHAR wbuf[10];
            
            HWND ListHwnd = GetDlgItem(hwnd, ID_CLOCK_SetYear);

            /* 在列表中增加年 */
            x_wsprintf(wbuf, L"%d", xC);
            SendMessage(ListHwnd, LB_ADDSTRING, i, (LPARAM)wbuf);
            SendMessage(ListHwnd, LB_SETITEMHEIGHT, i++, (LPARAM)44);
          }

          i=0;
          for (uint8_t xC=1; xC<13; xC++)
          {
            WCHAR wbuf[10];
            HWND ListHwnd = GetDlgItem(hwnd, ID_CLOCK_SetMonth);

            /* 在列表中增加月 */
            x_wsprintf(wbuf, L"%d", xC);
            SendMessage(ListHwnd, LB_ADDSTRING, i, (LPARAM)wbuf);
            SendMessage(ListHwnd, LB_SETITEMHEIGHT, i++, (LPARAM)44);
          }

          i=0;
          for (uint8_t xC=1; xC<32; xC++)
          {
            WCHAR wbuf[10];
            HWND ListHwnd = GetDlgItem(hwnd, ID_CLOCK_SetDate);

            /* 在列表中增加日期 */
            x_wsprintf(wbuf, L"%d", xC);
            SendMessage(ListHwnd, LB_ADDSTRING, i, (LPARAM)wbuf);
            SendMessage(ListHwnd, LB_SETITEMHEIGHT, i++, (LPARAM)44);
          }
        }
        SetTimer(hwnd, 1, 50, TMR_START, NULL);
        
        break;
      }

      case WM_TIMER:
      {
        int cur_sel, i, y;
        HWND wnd;
        RECT rc0, rc;

        if(GetKeyState(VK_LBUTTON)==0) //鼠标左键／触摸屏　没有按下时.
        {
          for (uint32_t xC=ID_CLOCK_SetYear; xC<ID_CLOCK_SetMinute+1; xC++)
          {
            wnd =GetDlgItem(hwnd, xC); //获得LISTBOX 设置时的HWND.

            cur_sel=SendMessage(wnd,LB_GETCURSEL,0,0); //获得LISTBOX顶项.

            GetClientRect(wnd,&rc0);
            SendMessage(wnd,LB_GETITEMRECT,cur_sel,(LPARAM)&rc); //获得顶项的矩形位置.

            y = rc.y+(rc.h/2);

            if(y < (rc0.h/2)) //如果选中的项在Listbox　中线的上面,则往下移动.
            {
              if(abs(y-(rc0.h/2)) > (rc.h>>2))
              {//与中线隔得远，就每次多移一点距离(看上去移动速度快)．
                //i =rc.h>>2;
                i = 4;
              }
              else
              {//与中线隔得很近了，就只移1个offset(缓慢移动)．
                i = 1;
              }

              SendMessage(wnd,LB_OFFSETPOS,TRUE,i);
            }

            if(y > (rc0.h/2)) //如果选中的项在Listbox　中线的下面,则往上移动.
            {
              if(abs(y-(rc0.h/2)) > (rc.h>>2))
              {//与中线隔得远，就每次多移一点距离(看上去移动速度快)．
                //i =rc.h>>2;
                i = 4;
              }
              else
              {//与中线隔得很近了，就只移1个offset(缓慢移动)．
                i = 1;
              }

              SendMessage(wnd,LB_OFFSETPOS,TRUE,-i);
            }
          }
        }
        
      }  
			break;     
      
      case WM_NOTIFY:
      {
         u16 code, id;
         id  =LOWORD(wParam);//获取消息的ID码
         code=HIWORD(wParam);//获取消息的类型
         
         NMHDR *nr;
         nr =(NMHDR*)lParam;      

         //发送单击
        if(code == BN_CLICKED)
        {
          switch (id)
          {
            /* 退出按钮按下 */
            case ID_CLOCK_WAIVE:
            {
              PostCloseMessage(hwnd);    // 发送关闭窗口的消息
            }
            break;

            /* 完成设置按钮按下 */
            case ID_CLOCK_OK:
            {
              if (Set_Start.page == 0)    // 时间
              {
                RTC_TimeTypeDef RTC_Time;

                RTC_Time.RTC_Hours = GetListCurselVal(hwnd, ID_CLOCK_SetHour);        // 读取列表的时
                RTC_Time.RTC_Minutes = GetListCurselVal(hwnd, ID_CLOCK_SetMinute);    // 读取列表的分
                RTC_Time.RTC_Seconds = 0;

                RTC_SetTime(RTC_Format_BIN, &RTC_Time);    // 设置时间
              }
              else if (Set_Start.page == 1)    // 修改日历
              {

                RTC_Date.RTC_Year = GetListCurselVal(hwnd, ID_CLOCK_SetYear) - 2000;    // 读取列表的年
                RTC_Date.RTC_Month = GetListCurselVal(hwnd, ID_CLOCK_SetMonth);         // 读取列表的月
                RTC_Date.RTC_Date = GetListCurselVal(hwnd, ID_CLOCK_SetDate);           // 读取列表的日
                /* 基姆拉尔森周计算公式 */
                RTC_Date.RTC_WeekDay = (RTC_Date.RTC_Date + 2 * RTC_Date.RTC_Month + 3     \
                                        * (RTC_Date.RTC_Month + 1) / 5 + RTC_Date.RTC_Year \
                                        + RTC_Date.RTC_Year / 4 - RTC_Date.RTC_Year / 100  \
                                        + RTC_Date.RTC_Year / 400) % 7 + 1;
                RTC_SetDate(RTC_Format_BIN, &RTC_Date);                                 // 设置日期
                /* 设置当前显示日期 */
                WCHAR wbuf[5];
                x_wsprintf(wbuf, L"%d", RTC_Date.RTC_Year);
                SetWindowText(GetDlgItem(hwnd, ID_CLOCK_YEAR), wbuf);   
								
                x_wsprintf(wbuf, L"%d", RTC_Date.RTC_Date);
                SetWindowText(GetDlgItem(hwnd, ID_CLOCK_DATE), wbuf);    // 设置日期
          
                x_wsprintf(wbuf, L"%d", RTC_Date.RTC_Month);
                SetWindowText(GetDlgItem(hwnd, ID_CLOCK_MONTH), wbuf);    // 设置月
          
                SetWindowText(GetDlgItem(hwnd, ID_CLOCK_WEEK), Week_List[RTC_Date.RTC_WeekDay - 1]);    // 设置星期

              }

              PostCloseMessage(hwnd);    // 发送关闭窗口的消息
            }
            break;



            /* 下一步 */
            case ID_CLOCK_NEXT:
            {
                Set_Start.page = 1;    // 标记为时钟设置界面
                SetWindowText(GetDlgItem(hwnd, ID_CLOCK_SETTITLE), L"设置日期");


                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetHour),      SW_HIDE);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetMinute),    SW_HIDE);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_NEXT),         SW_HIDE);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_BACK),         SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SETTIME),      SW_HIDE);

                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetDate),  SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetYear),  SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetMonth), SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SETDATE),  SW_SHOW);
              
            }
            break;

            /* 上一步 */
            case ID_CLOCK_BACK:
            {
                Set_Start.page = 0;    // 标记为设置时间界面
                SetWindowText(GetDlgItem(hwnd, ID_CLOCK_SETTITLE), L"设置时间");


               ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetDate),  SW_HIDE);
               ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetYear),  SW_HIDE);
               ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetMonth), SW_HIDE);
               ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SETDATE),  SW_HIDE);

                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetHour),      SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetMinute),    SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SETTIME),      SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_BACK),         SW_HIDE);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_NEXT),         SW_SHOW);
            }
            break;
          }
        }

        /* 列表处理 */
        if (id >= ID_CLOCK_SetYear && id <= ID_CLOCK_SetMinute)
        {
          if(nr->code == LBN_SELCHANGE)    // 列表选中项被改变
          {
            
          }
          
          if(nr->code == LBN_POSCHANGE)    // 有列表位置被改变
          {
            RECT rc0,rc;
            HWND wnd;
            int i;

            wnd = GetDlgItem(hwnd, id);     // 获得被改变列表的 HWND

            GetClientRect(wnd,&rc0);
            /* 使列表循环 */
            i = 0;
            SendMessage(wnd,LB_GETITEMRECT,i,(LPARAM)&rc);
            if(rc.y > 2)
            {
              i =SendMessage(wnd,LB_GETCOUNT,0,0);
              i-=1;
              SendMessage(wnd,LB_MOVEINDEX,i,0);
            }
            else
            {
              i =SendMessage(wnd,LB_GETCOUNT,0,0);
              i-=1;
              SendMessage(wnd,LB_GETITEMRECT,i,(LPARAM)&rc);
              if((rc.y+rc.h) < (rc0.h-8))
              {
                SendMessage(wnd,LB_MOVEINDEX,0,0xFFFF);
              }
            }

            /* 设置靠近中间项为选中项 */
            int min = 0;    // 记录最近的（最后的选中项）
            int len = 0xFFFFFFF;
            int count = SendMessage(wnd,LB_GETCOUNT,0,0);
            i = SendMessage(wnd, LB_GETTOPINDEX, 0, 0);            // 首个显示的子项目
            
            while (1)
            {
              SendMessage(wnd, LB_GETITEMRECT, i, (LPARAM)&rc);    // 获得子项目的矩形参数

              if (rc.y >= rc0.h || i >= count - 1)          // 判断是不是在显示区域内
              {
                SendMessage(wnd, LB_SETCURSEL, min, 0);     // 设置当前选中的子项目
                break;                                      // 不是在显示区域，结束循环
              }

              rc.y += rc.h/2;                // 计算这项的中间位置
              rc.y = abs(rc.y - rc0.h/2);    // 计算当前项到中间的距离

              if (len > rc.y)
              {
                len = rc.y;    // 记录较小距离
                min = i;       // 记录较小项
              }
              i++;         // 查找下一项
              if (i>=count) 
              {
                i = 0;
              }
            }

            WCHAR wbuf[5];
            if (id >= ID_CLOCK_SetYear && id <= ID_CLOCK_SetDate)    // 这在设置日历
            {
              x_wsprintf(wbuf, L"%d年%02d月%02d日", GetListCurselVal(hwnd, ID_CLOCK_SetYear),
                                                    GetListCurselVal(hwnd, ID_CLOCK_SetMonth),
                                                    GetListCurselVal(hwnd, ID_CLOCK_SetDate));

              SetWindowText(GetDlgItem(hwnd, ID_CLOCK_SETDATE), wbuf);    // 设置日期
            }
            else    // 正在设置时间
            {
              x_wsprintf(wbuf, L"%02d:%02d", GetListCurselVal(hwnd, ID_CLOCK_SetHour),
                                             GetListCurselVal(hwnd, ID_CLOCK_SetMinute));

              SetWindowText(GetDlgItem(hwnd, ID_CLOCK_SETTIME), wbuf);    // 设置日期
            }
      
            uint8_t month = 0;
            month = GetListCurselVal(hwnd, ID_CLOCK_SetMonth);    // 获得选中的月
            if (id == ID_CLOCK_SetMonth || (id == ID_CLOCK_SetYear && month == 2))    // 设置月份，不同月份需调整日期
            {
              uint8_t date_max = 0;

              date_max = month_list[month - 1];

              if (month == 2)    // 调整平年 闰年
              {
                uint16_t year = GetListCurselVal(hwnd, ID_CLOCK_SetYear);    // 获得选中的年

                if( year % 400 == 0 || (year % 4 ==  0 && year % 100 != 0))
                {
                  date_max = 29;    // 闰年
                }
              }

              HWND ListHwnd = GetDlgItem(hwnd, ID_CLOCK_SetDate);

              SendMessage(ListHwnd, LB_RESETCONTENT, 0, 0);

              for (uint8_t xC=1; xC<date_max+1; xC++)
              {
                WCHAR wbuf[10];

                /* 在列表中增加日期 */
                x_wsprintf(wbuf, L"%d", xC);
                SendMessage(ListHwnd, LB_ADDSTRING, xC-1, (LPARAM)wbuf);
                SendMessage(ListHwnd, LB_SETITEMHEIGHT, xC-1, (LPARAM)44);
              }
              SendMessage(ListHwnd, LB_SETCURSEL, 2, 0);

            }
          }
        }

      break;
    }

      // 重绘制函数消息
      case WM_DRAWITEM:
      {
        DRAWITEM_HDR *ds;
        ds = (DRAWITEM_HDR*)lParam;        
        if (ds->ID >= ID_CLOCK_BACK && ds->ID <= ID_CLOCK_OK)
        {
          btn_owner_draw(ds);             // 重绘按钮
          return TRUE;
        }
        else if (ds->ID == ID_CLOCK_SETTITLE)
        {
          Title_Textbox_OwnerDraw(ds);
          return TRUE;
        }
        else if (ds->ID == ID_CLOCK_SETTIME || ds->ID == ID_CLOCK_SETDATE)
        {
          WhiteBK_Textbox_OwnerDraw(ds);
          return TRUE;
        }
        else if (ds->ID == ID_CLOCK_WAIVE)
        {
          waive_btn_owner_draw(ds);
          return TRUE;
        }
//        else if (ds->ID >= ID_CLOCK_Background00 && ds->ID <= ID_CLOCK_Background02)
//        {
//          radiobox_owner_draw(ds, ds->ID);
//          return TRUE;
//        }
        else if (ds->ID >= ID_CLOCK_SetYear && ds->ID <= ID_CLOCK_SetMinute)
        {
          listbox_owner_draw(ds);
          return TRUE;
        }

        return FALSE;
      }     
      
      //绘制窗口界面消息
      case WM_PAINT:
      {
        PAINTSTRUCT ps;
//        HDC hdc;//屏幕hdc

        //开始绘制
        BeginPaint(hwnd, &ps); 
        
        EndPaint(hwnd, &ps);
      }
      break;

      case WM_ERASEBKGND:
      {
        HDC hdc =(HDC)wParam;
        
        RECT rc_title = {0, 0, GUI_XSIZE, 80};
        RECT rc_title_grad = {0, 80, GUI_XSIZE, 5};
        RECT rc_lyric = {0, 80, GUI_XSIZE, 400};
        RECT rc1 = {230, 102, 339, 272};

        SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
        FillRect(hdc, &rc_title);

        SetBrushColor(hdc, MapRGB(hdc, CLOCK_BACK_COLOR));
        FillRect(hdc, &rc_lyric);
        
        GradientFillRect(hdc, &rc_title_grad, MapRGB(hdc, 150, 150, 150), MapRGB(hdc, CLOCK_BACK_COLOR), TRUE);
        
        SetBrushColor(hdc, MapRGB(hdc, 250, 250, 250));
        EnableAntiAlias(hdc, TRUE);
        FillRoundRect(hdc, &rc1, 10);    // 画列表背景
        EnableAntiAlias(hdc, FALSE);
    

        return FALSE;
      }

      //关闭窗口消息处理case
      case WM_CLOSE:
      {   
        DestroyWindow(hwnd);
        return TRUE;	
      }
    
      //关闭窗口消息处理case
      case WM_DESTROY:
      {
        SetTimer(clock_hwnd, 1, 400, TMR_START, NULL);
        Set_Start.page = 0;

        return PostQuitMessage(hwnd);		
      }
      
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }
     
   return WM_NULL;
}

static uint8_t sec_old = ~0;
static uint8_t sec_date = ~0;

static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch(msg){
      case WM_CREATE:
      {
        for (uint8_t xC=0; xC<ICON_BTN_NUM; xC++)     //  按钮
        {
          /* 循环创建按钮 */
          CreateWindow(BUTTON, clock_icon[xC].icon_name, WS_OWNERDRAW | WS_VISIBLE,
                        clock_icon[xC].rc.x, clock_icon[xC].rc.y,
                        clock_icon[xC].rc.w, clock_icon[xC].rc.h,
                        hwnd, clock_icon[xC].id, NULL, NULL); 
        }

        for (uint8_t xC=ICON_BTN_NUM; xC<ICON_TEXT_NUM; xC++)    // 文本框
        {
          /* 循环创建文本框 */
          CreateWindow(TEXTBOX, clock_icon[xC].icon_name, WS_OWNERDRAW | WS_VISIBLE,
                        clock_icon[xC].rc.x, clock_icon[xC].rc.y,
                        clock_icon[xC].rc.w,clock_icon[xC].rc.h,
                        hwnd, clock_icon[xC].id, NULL, NULL); 
        }

        /* 初始化日期 */
        /* 初始化日期 */
        WCHAR wbuf[5];
				x_wsprintf(wbuf, L"%d", 2000+RTC_Date.RTC_Year);
				SetWindowText(GetDlgItem(hwnd, ID_CLOCK_YEAR), wbuf);   
								
        RTC_GetDate(RTC_Format_BIN, &RTC_Date);
        
        x_wsprintf(wbuf, L"%d",RTC_Date.RTC_Date);
        SetWindowText(GetDlgItem(hwnd, ID_CLOCK_DATE), wbuf);    // 设置日期
        
        x_wsprintf(wbuf, L"%d", RTC_Date.RTC_Month);
        SetWindowText(GetDlgItem(hwnd, ID_CLOCK_MONTH), wbuf);    // 设置月
        
        SetWindowText(GetDlgItem(hwnd, ID_CLOCK_WEEK), Week_List[RTC_Date.RTC_WeekDay - 1]);    // 设置星期

        SetTimer(hwnd, 1, 400, TMR_START, NULL);

        break;
      }

      case WM_TIMER:
      {
        int tmr_id;

				tmr_id = wParam;    // 定时器 ID

				if (tmr_id == 1)
        {
					WCHAR wbuf[5];

//					RTC_GetDate(RTC_Format_BIN, &RTC_Date);								
//					x_wsprintf(wbuf, L"%d", RTC_Date.RTC_Year);
//					SetWindowText(GetDlgItem(hwnd, ID_CLOCK_YEAR), wbuf);   

//          
//          x_wsprintf(wbuf, L"%d", RTC_Date.RTC_Date);
//          SetWindowText(GetDlgItem(hwnd, ID_CLOCK_DATE), wbuf);    // 设置日期
//          
//          x_wsprintf(wbuf, L"%d", RTC_Date.RTC_Month);
//          SetWindowText(GetDlgItem(hwnd, ID_CLOCK_MONTH), wbuf);    // 设置月
//          
//          SetWindowText(GetDlgItem(hwnd, ID_CLOCK_WEEK), Week_List[RTC_Date.RTC_WeekDay - 1]);    // 设置星期
//          /* Unfreeze the RTC DR Register */
//          (void)RTC->DR;
          
          InvalidateRect(GetDlgItem(hwnd, ID_CLOCK_TIME), NULL, TRUE);
        }
        
      }  
			break;     
      
      case WM_NOTIFY:
      {
         u16 code,  id;
         id  =LOWORD(wParam);//获取消息的ID码
         code=HIWORD(wParam);//获取消息的类型   

         //发送单击
        if(code == BN_CLICKED)
        {
          switch (id)
          {
            /* 退出按钮按下 */
            case ID_CLOCK_EXIT:
            {
              PostCloseMessage(hwnd);    // 发送关闭窗口的消息
            }
            break;

            /* 显示日期区域按下 */
            case ID_CLOCK_DATE:
            {
              KillTimer(hwnd, 1);
              /* 创建设置窗口 */
              WNDCLASS wcex;
              RECT rc;
              
              Set_Start.page = 1;

              wcex.Tag	 		= WNDCLASS_TAG;

              wcex.Style			= CS_HREDRAW | CS_VREDRAW;
              wcex.lpfnWndProc	= (WNDPROC)setting_win_proc;
              wcex.cbClsExtra		= 0;
              wcex.cbWndExtra		= 0;
              wcex.hInstance		= NULL;
              wcex.hIcon			= NULL;
              wcex.hCursor		= NULL;
              
              rc.x = 0;
              rc.y = 0;
              rc.w = GUI_XSIZE;
              rc.h = GUI_YSIZE;
              
              // 创建"设置"窗口.
              CreateWindow(&wcex, L"---", WS_CLIPCHILDREN | WS_VISIBLE | WS_OVERLAPPED,
                           rc.x, rc.y, rc.w, rc.h, hwnd, ID_CLOCK_SetWin, NULL, NULL);
            }
            break;
            
            /* 显示时间区域按下 */
            case ID_CLOCK_TIME:
            {
              KillTimer(hwnd, 1);
              /* 创建设置窗口 */
              WNDCLASS wcex;
              RECT rc;
              
              Set_Start.page = 0;

              wcex.Tag	 		= WNDCLASS_TAG;

              wcex.Style			= CS_HREDRAW | CS_VREDRAW;
              wcex.lpfnWndProc	= (WNDPROC)setting_win_proc;
              wcex.cbClsExtra		= 0;
              wcex.cbWndExtra		= 0;
              wcex.hInstance		= NULL;
              wcex.hIcon			= NULL;
              wcex.hCursor		= NULL;
              
              rc.x = 0;
              rc.y = 0;
              rc.w = GUI_XSIZE;
              rc.h = GUI_YSIZE;
              
              // 创建"设置"窗口.
              CreateWindow(&wcex, L"---", WS_CLIPCHILDREN | WS_VISIBLE | WS_OVERLAPPED,
                           rc.x, rc.y, rc.w, rc.h, hwnd, ID_CLOCK_SetWin, NULL, NULL);
            }
          }
        }
        break;
      }

      //重绘制函数消息
      case WM_DRAWITEM:
      {
         DRAWITEM_HDR *ds;
         ds = (DRAWITEM_HDR*)lParam;        
         if(ds->ID == ID_CLOCK_EXIT)
         {
            exit_owner_draw(ds);
            return TRUE;
         }
         else if (ds->ID == ID_CLOCK_DATE)
         {
            date_OwnerDraw(ds);
            return TRUE;
         }
         else if (ds->ID == ID_CLOCK_TITLE)
         {
            Title_Textbox_OwnerDraw(ds);
            return TRUE;
         }
         else if (ds->ID == ID_CLOCK_TIME)
         {
            Dial_OwnerDraw(ds);
            return TRUE;
         }

         return FALSE;
      }     
      
      //绘制窗口界面消息
      case WM_PAINT:
      {
        PAINTSTRUCT ps;

        //开始绘制
        BeginPaint(hwnd, &ps); 
        
        EndPaint(hwnd, &ps);
        break;
      }
      
      case WM_ERASEBKGND:
      {
        HDC hdc =(HDC)wParam;
        
        RECT rc_title = {0, 0, GUI_XSIZE, 80};
        RECT rc_title_grad = {0, 80, GUI_XSIZE, 5};
        RECT rc_lyric = {0, 80, GUI_XSIZE, 400};

        SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
        FillRect(hdc, &rc_title);

        SetBrushColor(hdc, MapRGB(hdc, CLOCK_BACK_COLOR));
        FillRect(hdc, &rc_lyric);
        
        GradientFillRect(hdc, &rc_title_grad, MapRGB(hdc, 150, 150, 150), MapRGB(hdc, CLOCK_BACK_COLOR), TRUE);

        return FALSE;
      }

      //关闭窗口消息处理case
      case WM_CLOSE:
      {   
        DestroyWindow(hwnd);
        return TRUE;	
      }
    
      //关闭窗口消息处理case
      case WM_DESTROY:
      {        
        sec_old = ~0;
        sec_date = ~0;
        return PostQuitMessage(hwnd);		
      }
      
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }
     
   return WM_NULL;
}


void GUI_CLOCK_DIALOG(void)
{ 	
	WNDCLASS	wcex;
	MSG msg;

	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = win_proc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

	//创建主窗口
	clock_hwnd = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,//
                                    &wcex,
                                    L"GUI CLOCK DIALOG",
                                    WS_VISIBLE|WS_OVERLAPPED,
                                    0, 0, GUI_XSIZE, GUI_YSIZE,
                                    NULL, NULL, NULL, NULL);

	//显示主窗口
	ShowWindow(clock_hwnd, SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, clock_hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


