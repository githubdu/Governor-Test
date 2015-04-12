#include "Admin.h"


/*处理管理员窗口消息*/
struct DecodeUnit DlgAdminMessage[] ={
	WM_INITDIALOG,	DoAdminInit,
	WM_COMMAND,		DoAdminCommand,
};

/*处理管理员控件消息*/
struct DecodeUnit DlgAdminCommandMessage[] = {
	IDC_BtnAminCancle,	OnAdminCancel,
	IDB_ADD,			OnAddClick,
	IDB_DEL,			OnDelClick,
	IDB_MODIFY,			OnAdminModifyClick,
};


//管理员表格（listview）表头
TCHAR * ColumName[] =
{
	TEXT("序号"),
	TEXT("型号"),
	TEXT("编码"),
	TEXT("备注"),
};

TAG_DETECTOR detectors[20];

int ColumWidth[4] = {100,150,75,300};

//HINSTANCE hInst;
HWND hWndList;


/*管理员对话框消息处理函数*/
INT_PTR CALLBACK DlgAdminPorc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	int i;
	for (i = 0;i < dim(DlgAdminMessage);i++)
	{
		if(DlgAdminMessage[i].message == message)
			(*DlgAdminMessage[i].fun)(hDlg,message,wParam,lParam);
	}
	return FALSE;
}

/*控件处理函数初始化*/
LONG DoAdminCommand(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	int i ;
	for(i =0 ;i< dim(DlgAdminCommandMessage);++i)
	{
		if(DlgAdminCommandMessage[i].message == LOWORD(wParam))
			(*DlgAdminCommandMessage[i].fun)(hDlg,message,wParam,lParam);
	}
	return TRUE;
}


/*窗口初始化*/
LONG DoAdminInit(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	SetWindowPos(hDlg,NULL,0,0,800,480,SWP_SHOWWINDOW);

	hWndList = GetDlgItem(hDlg,IDC_LIST_Admin);//一个指向管理员窗口表格的全局指针

	

	OnAdminListInit(hDlg,message,wParam,lParam);//管理员窗口表格初始化
	return TRUE;  
}




LONG OnAdminCancel(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	EndDialog(hDlg,0);
	return TRUE;
}




LONG OnAdminListInit(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	
	//管理员表格（listview）表头初始化
	int index = 0;
	LV_COLUMN lvC;	//表头变量			
	TCHAR szText[MAX_PATH];		
	lvC.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;//列标示
	lvC.fmt = LVCFMT_LEFT;  //表头对齐方式      
	lvC.pszText = szText;	//表头内容
	for (index = 0; index < NUM_COLUMNS; index++)
	{
		lvC.iSubItem = index;
		lvC.cx = ColumWidth[index]; //列宽度
		wcscpy_s(szText,sizeof(ColumName[index]),ColumName[index]);
		if (ListView_InsertColumn(hWndList, index, &lvC) == -1)
			return NULL;
	}//表头初始化完毕

	//读取限速器类型文件
	HANDLE hFile = CreateFile(TypeFileName,
		GENERIC_READ ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);
	if( hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	index = 0;
	char buf[120] = {NULL};//读取文件内容到该缓存
	TCHAR numBuf[50];	//单元格内容缓存
	DWORD dwRead = -1;

	LVITEM lvI;	//listview表格变量
	lvI.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
	lvI.state = 0;      
	lvI.stateMask = 0; 

	ReadFile(hFile,(LPVOID)buf,120,&dwRead,NULL);
	while (*buf != NULL)
	{

		lvI.iItem = index;
		lvI.iSubItem = 0;
		lvI.iImage = index;

		wsprintf(numBuf,TEXT("%7d"),index+1);
		detectors[index].detectorID = index+1;
		lvI.pszText = numBuf;
		int p = ListView_InsertItem(hWndList, &lvI);

		int iSubItem = 1;
		memcpy(detectors[index].Type,&buf,20);
		ListView_SetItemText( hWndList,p,iSubItem++,detectors[index].Type);
		memcpy(detectors[index].Code,&buf[20],5);
		ListView_SetItemText( hWndList,p,iSubItem++,detectors[index].Code);
		memcpy(detectors[index].Info,&buf[25],50);
		ListView_SetItemText( hWndList,p,iSubItem++,detectors[index].Info);



		int i = 0;
		for (i = 0; i < 120; i++)
		{
			buf[i] = NULL;
		}
		ReadFile(hFile,(LPVOID)buf,120,&dwRead,NULL);
		index++;
	}
	CloseHandle(hFile);

	return 0;
}

LONG OnAddClick(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{

	DialogBox(NULL,MAKEINTRESOURCE(IDD_DIALOG_Admin_Add),NULL,DlgAdminAddPorc);
	//DialogBox(NULL,MAKEINTRESOURCE(IDD_DIALOG_OPERATOR),NULL,DlgOperatorPorc);
	CommandBar_Show(NULL, FALSE);
	EndDialog(hDlg,0);
	return TRUE;
}

/*LONG OnStyleClick(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{

	static DWORD style = LVS_ICON;

	SetWindowLong(hWndList, GWL_STYLE,
		(style ++ % 4));
	return 0;
}*/

LONG OnDelClick(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	TCHAR szbuf[120] =TEXT("\0");
	
	DeleteFile(_T("\\Temp\\detector.txt"));
	//写入限速器类型文件
	HANDLE hFiletemp = CreateFile(_T("\\Temp\\detector.txt"),
		GENERIC_WRITE ,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		0);
	if( hFiletemp == INVALID_HANDLE_VALUE)
		return FALSE;
	
	//读取限速器类型文件
	HANDLE hFile = CreateFile(TypeFileName,
		GENERIC_READ ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);
	if( hFile == INVALID_HANDLE_VALUE)
		return FALSE;


	int index = 0;
	int mark = ListView_GetSelectionMark(hWndList);

	DWORD dwWriete = -1;
	DWORD dwRead = -1;
	char detector[120] = {NULL};
	
	do 
	{	
		int i = 0;
		for (i = 0; i < 120; i++)
		{
			detector[i] = NULL;
		}
		ReadFile(hFile,(LPVOID)detector,120,&dwRead,NULL);

		if (*detector == NULL)
		{
			break;
		}

		

		if (index != mark)
		{
			index++;
			continue;
		}


		_stprintf_s(TestsFileName,50,_T("\\NandFlash\\detector_"));
		memcpy(TestsFileName+20,detector+20,5);
		for (i = 0; i <5; i++)
		{
			if (detector[20+i*2] == NULL)
			{
				break;;
			}
		}
		_stprintf_s(TestsFileName+20+i,60,_T(".txt"));
		DeleteFile(TestsFileName);




		for (i = 0; i < 120; i++)
		{
			detector[i] = NULL;
		}
		ReadFile(hFile,(LPVOID)detector,120,&dwRead,NULL);
		if (*detector == NULL)
		{
			break;
		}
		index++;
	} while (WriteFile(hFiletemp,detector,120,&dwWriete,NULL));

	CloseHandle(hFiletemp);
	CloseHandle(hFile);

	DeleteFile(TypeFileName);
	//写入限速器类型文件
	 hFiletemp = CreateFile(TypeFileName,
		GENERIC_WRITE ,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		0);
	if( hFiletemp == INVALID_HANDLE_VALUE)
		return FALSE;

	//读取限速器类型文件
	 hFile = CreateFile(_T("\\Temp\\detector.txt"),
		GENERIC_READ ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);
	if( hFile == INVALID_HANDLE_VALUE)
		return FALSE;


	 index = 0;
	 dwWriete = -1;
	 dwRead = -1;


	do 
	{	
		int i = 0;
		for (i = 0; i < 120; i++)
		{
			detector[i] = NULL;
		}
		ReadFile(hFile,(LPVOID)detector,120,&dwRead,NULL);
			if (*detector == NULL)
			{
				break;
			}

	} while (WriteFile(hFiletemp,detector,120,&dwWriete,NULL));

	CloseHandle(hFiletemp);
	CloseHandle(hFile);

	ListView_DeleteItem(hWndList,mark);
	EndDialog(hDlg,0);
	DialogBox(NULL,MAKEINTRESOURCE(IDD_DIALOG_Admin),NULL,DlgAdminPorc);
	return 0;
}


LONG OnAdminModifyClick(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	ModifyMark = ListView_GetSelectionMark(hWndList)+1;

	if (ModifyMark !=0)
	{

		//读取限速器类型文件
		HANDLE hFile = CreateFile(TypeFileName,
			GENERIC_READ ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			0);
		if( hFile == INVALID_HANDLE_VALUE)
			return FALSE;

		int		index = 0;
		char detector[120] = {NULL};//读取文件内容到该缓存
		char buf[20] = {NULL};
		DWORD dwRead = -1;
		SetFilePointer(hFile,(ModifyMark-1)*120,NULL,FILE_CURRENT);
		ReadFile(hFile,(LPVOID)detector,120,&dwRead,NULL);
		_stprintf_s(TestsFileName,50,_T("\\NandFlash\\detector_"));
		memcpy(TestsFileName+20,detector+20,5);
		int i = 0;
		for (i = 0; i <5; i++)
		{
			if (detector[20+i*2] == NULL)
			{
				break;;
			}
		}
		_stprintf_s(TestsFileName+20+i,60,_T(".txt"));
		CloseHandle(hFile);
		DialogBox(NULL,MAKEINTRESOURCE(IDD_DIALOG_Admin_Add),NULL,DlgAdminAddPorc);
		//DialogBox(NULL,MAKEINTRESOURCE(IDD_DIALOG_OPERATOR),NULL,DlgOperatorPorc);
		CommandBar_Show(NULL, FALSE);
		EndDialog(hDlg,0);
	}

	return TRUE;
}