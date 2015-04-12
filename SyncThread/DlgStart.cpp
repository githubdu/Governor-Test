
#include "DlgStart.h"


HINSTANCE g_hInstance;



/*处理开始窗口消息*/
struct DecodeUnit DlgStartMessage[] ={ 
	WM_INITDIALOG,	DoStartInit,
	WM_COMMAND,		DoStartCommand,
};

/*处理开始窗口控件消息*/
struct DecodeUnit DlgStartCommandMessage[] = {
	IDSTARTCANCEL,		OnStartCancel,
	IDC_BtnAdmin,		OnBtnAdmin,
	IDC_BtnOperator,	OnBtnOperator,
	
};
/*程序入口，调用MFC标准对话框*/
int WINAPI WinMain(HINSTANCE hInstance,	HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow)		
{
	g_hInstance = hInstance;
	DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG_START),NULL,DlgStartPorc);
	CommandBar_Show(NULL, FALSE);
	return (0);
}

/*开始对话框消息处理函数*/
INT_PTR CALLBACK DlgStartPorc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	int i;
	for (i = 0;i < dim(DlgStartMessage);i++)
	{
		if(DlgStartMessage[i].message == message)
			(*DlgStartMessage[i].fun)(hDlg,message,wParam,lParam);
	}
	return FALSE;
}




/*开始窗口初始化*/
LONG DoStartInit(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	SetWindowPos(hDlg,NULL,0,0,800,480,SWP_SHOWWINDOW);
	_stprintf_s(TypeFileName,50,_T("\\NandFlash\\detector.txt"));//限速器类型文件
	return TRUE;
}




/*开始窗口控件处理函数初始化*/
LONG DoStartCommand(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	int i ;
	for(i =0 ;i< dim(DlgStartCommandMessage);++i)
	{
		if(DlgStartCommandMessage[i].message == LOWORD(wParam))
			(*DlgStartCommandMessage[i].fun)(hDlg,message,wParam,lParam);
	}
	return TRUE;
}
LONG OnBtnAdmin(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{

	DialogBox(g_hInstance,MAKEINTRESOURCE(IDD_DIALOG_Admin),NULL,DlgAdminPorc);
	CommandBar_Show(NULL, FALSE);

	return TRUE;
}


LONG OnBtnOperator(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	DialogBox(g_hInstance,MAKEINTRESOURCE(IDD_DIALOG_OPERATOR),NULL,DlgOperatorPorc);
	CommandBar_Show(NULL, FALSE);

	return TRUE;
}




LONG OnStartCancel(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	EndDialog(hDlg,0);
	return TRUE;
}