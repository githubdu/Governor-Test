#include "resource.h"
#include "detector.h"






INT_PTR CALLBACK DlgStartPorc(HWND,UINT,WPARAM,LPARAM);



//系统消息处理
LONG DoStartInit(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG DoStartCommand(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG OnStartCancel(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG OnBtnAdmin(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG OnBtnOperator(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);



