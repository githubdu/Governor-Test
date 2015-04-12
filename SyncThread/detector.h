#include "resource.h"
#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <atlstr.h>
#include <winuser.h>
#include <CommCtrl.h>
#include <commdlg.h>
#include "com.h"

#define velocity2rate 63.0
#define count 5120



//全局变量
extern TCHAR	g_sType[10];
extern TCHAR	g_sId[10];
extern TCHAR	g_sType0[10];
extern TCHAR	g_sId0[10];
extern double	g_dwDia;
extern double	g_dwRatedSpeed;
extern double	g_dwRatedSpeedLimitLow;
extern double	g_dwRatedSpeedLimitUp;
extern double	g_dwElecSpeed;
extern double	g_dwMechaSpeed;
extern int		g_iOrder;
extern double	g_dwVelocity[count];
extern int		g_iIndex;
extern double	g_rx;
extern double	g_ry;
extern double	g_tx;
extern double	g_ty;

extern TCHAR	TypeFileName[100];
extern	TCHAR	ID[5];
extern TCHAR	Type[20];
extern TCHAR	CODE[5];
extern TCHAR	INFO[50];
extern		TCHAR Speed[20];
extern TCHAR Limit[20];
extern int		ModifyMark;
extern TCHAR	TestsFileName[100];



struct DecodeUnit{
	UINT message;
	LONG (*fun)(HWND,UINT,WPARAM,LPARAM);
};



typedef struct _HandleData{
	HWND HList;
	HWND HStatic;
} HANDLEDATA;


INT_PTR CALLBACK DlgAdminPorc(HWND,UINT,WPARAM,LPARAM);

INT_PTR CALLBACK DlgOperatorPorc(HWND,UINT,WPARAM,LPARAM);

INT_PTR CALLBACK DlgAdminAddPorc(HWND,UINT,WPARAM,LPARAM);

//系统消息处理
LONG DoOperatorInit(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG DoOperatorCommand(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG OnOperatorOk(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
//文本框处理
LONG OnIDC_EDIT_Type(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG OnIDC_EDIT_ID(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG OnIDC_EDIT_Dia(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG OnIDC_EDIT_RSpeed(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
//按钮处理
LONG OnBtnNext(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG OnBtnPrev(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG OnBtnPageUp(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG OnBtnPageDown(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG OnBtnStart(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG OnBtnOpen(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG OnBtnSave(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG OnBtnPrint(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG OnClose(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);




//其他数据处理
LONG MapInit(HWND hDlg);
LONG drawMap(HWND hDlg,int index);
DWORD WINAPI MainThread(LPVOID lpVoid);
DWORD WINAPI MapThread(LPVOID lpVoid);
void SetButtonEnble(HWND hDlg,BOOL B);
LONG OnInit(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);