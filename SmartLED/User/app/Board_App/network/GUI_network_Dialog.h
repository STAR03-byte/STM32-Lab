#include <emXGUI.h>

#ifndef __GUI_NETEORK_DIALOG
#define	__GUI_NETEORK_DIALOG

/* 控件 ID 定义 */
enum
{   
   /* 窗口 ID */
  ID_TEXTBOX_Title      = 0x00,     // 标题栏
  ID_TEXTBOX_Send        ,//0x01     // 发送显示
  ID_TEXTBOX_Receive     ,//0x02     // 接收显示
  ID_TEXTBOX_RemoteIP1   ,//0x07     // 远端IP
  ID_TEXTBOX_RemoteIP2   ,//0x08     // 远端IP
  ID_TEXTBOX_RemoteIP3   ,//0x09     // 远端IP
  ID_TEXTBOX_RemoteIP4  ,//0x0A     // 远端IP
  ID_TEXTBOX_RemotePort  ,//0x0B     // 远端端口
  
  /* 按钮 ID */
  eID_Network_EXIT    ,//0x03
  eID_LINK_STATE      ,//0x04
  eID_Network_Send    ,//0x05
  eID_Receive_Clear   ,//0x06
  
  ID_Hint_Win,
  
};
extern HWND Network_Main_Handle;

#endif




