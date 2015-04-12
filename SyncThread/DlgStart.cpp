
#include "DlgStart.h"


HINSTANCE g_hInstance;



/*����ʼ������Ϣ*/
struct DecodeUnit DlgStartMessage[] ={ 
	WM_INITDIALOG,	DoStartInit,
	WM_COMMAND,		DoStartCommand,
};

/*����ʼ���ڿؼ���Ϣ*/
struct DecodeUnit DlgStartCommandMessage[] = {
	IDSTARTCANCEL,		OnStartCancel,
	IDC_BtnAdmin,		OnBtnAdmin,
	IDC_BtnOperator,	OnBtnOperator,
	
};
/*������ڣ�����MFC��׼�Ի���*/
int WINAPI WinMain(HINSTANCE hInstance,	HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow)		
{
	g_hInstance = hInstance;
	DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG_START),NULL,DlgStartPorc);
	CommandBar_Show(NULL, FALSE);
	return (0);
}

/*��ʼ�Ի�����Ϣ������*/
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




/*��ʼ���ڳ�ʼ��*/
LONG DoStartInit(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	SetWindowPos(hDlg,NULL,0,0,800,480,SWP_SHOWWINDOW);
	_stprintf_s(TypeFileName,50,_T("\\NandFlash\\detector.txt"));//�����������ļ�
	return TRUE;
}




/*��ʼ���ڿؼ���������ʼ��*/
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