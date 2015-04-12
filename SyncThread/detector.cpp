
#include "detector.h"
#include "gonvernor.h"


#define demo 0

/*全局变量*/
TCHAR	g_sType[10];
TCHAR	g_sId[10];
TCHAR	g_sType0[10] = {NULL};
TCHAR	g_sId0[10]	= {NULL};
double	g_dwDia;
double	g_dwRatedSpeed;
double	g_dwRatedSpeedLimitLow;
double	g_dwRatedSpeedLimitUp;
double	g_dwElecSpeed;
double	g_dwMechaSpeed;
int		g_iOrder = 1;
double	g_dwVelocity[count];
int		g_iIndex = 0;
double	g_rx;
double	g_ry;
double	g_tx;
double	g_ty;

TCHAR	ID[5];
TCHAR	Type[20];
TCHAR	CODE[5];
TCHAR	INFO[50];
TCHAR Speed[20];
TCHAR Limit[20];

int ModifyMark = 0;


TCHAR	TestsFileName[100];














HANDLE	hmainThread;
DWORD	dwMainThreadID;
HANDLE	hMapThread;
DWORD	dwMapThreadID;
HANDLE	m_Event;
BOOL	MapFlag = TRUE;
BOOL	MainFlag = TRUE;
BOOL	MainThreadStart = FALSE;

TCHAR	TypeFileName[100];




/*处理操作员窗口消息*/
struct DecodeUnit DlgOperatorMessage[] ={
	WM_INITDIALOG,	DoOperatorInit,
	WM_COMMAND,		DoOperatorCommand,
};

/*处理操作员控件消息*/
struct DecodeUnit DlgOperatorCommandMessage[] = {
	IDOK,			OnOperatorOk,
	IDC_EDIT_Type,	OnIDC_EDIT_Type,
	IDC_EDIT_ID,	OnIDC_EDIT_ID,
	IDC_EDIT_Dia,	OnIDC_EDIT_Dia,
	IDC_EDIT_RSpeed,OnIDC_EDIT_RSpeed,
	IDC_BtnNext,	OnBtnNext,
	IDC_BtnPrev,	OnBtnPrev,
	IDC_BtnPageUp,	OnBtnPageUp,
	IDC_BtnPageDown,OnBtnPageDown,
	IDC_BtnStart,	OnBtnStart,
	IDC_BtnOpen,	OnBtnOpen,
	IDC_BtnSave,	OnBtnSave,
	IDC_BtnPrint,	OnBtnPrint,
	IDC_BtnClose,	OnClose,
};



/*操作员对话框消息处理函数*/
INT_PTR CALLBACK DlgOperatorPorc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	int i;
	for (i = 0;i < dim(DlgOperatorMessage);i++)
	{
		if(DlgOperatorMessage[i].message == message)
			(*DlgOperatorMessage[i].fun)(hDlg,message,wParam,lParam);
	}
	return FALSE;
}


/*窗口初始化*/
LONG DoOperatorInit(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	SetWindowPos(hDlg,NULL,0,0,800,480,SWP_SHOWWINDOW);

	//打印机串口通信初始化
	TCHAR STRCOM[20];
	_stprintf_s(STRCOM,10,_T("COM1:"));
	//hPrinterComPort = InitCommunication(STRCOM,hPrinterComPort,NOPARITY);

	//plc控制器串口通信初始化
	_stprintf_s(STRCOM,10,_T("COM4:"));
	hServoComPort = InitCommunication(STRCOM,hServoComPort,NOPARITY);

	hPrinterComPort = hServoComPort;
	//下拉菜单初始化
	 OnInit( hDlg, message, wParam, lParam);

	//开启速度曲线刷新线程
	MapFlag = TRUE;
	if (!GetExitCodeThread (hMapThread, &dwMapThreadID) ||(dwMapThreadID != STILL_ACTIVE)) 
	{
		hMapThread = CreateThread(NULL,0,MapThread,(LPVOID)hDlg,0,&dwMapThreadID);
	}
	//开启伺服控制主线程
	MainFlag = TRUE;
	if (!GetExitCodeThread (hmainThread, &dwMainThreadID) ||(dwMainThreadID != STILL_ACTIVE)) 
	{
		hmainThread = CreateThread(NULL,0,MainThread,(LPVOID)hDlg,0,&dwMainThreadID);
	}

	//线程同步：伺服控制主线程和绘图线程
	m_Event = CreateEvent(NULL,FALSE,TRUE,NULL);

	Button_Enable(GetDlgItem(hDlg,IDC_BtnNext),FALSE);


	return TRUE;
}




/*控件处理函数初始化*/
LONG DoOperatorCommand(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	int i ;
	for(i =0 ;i< dim(DlgOperatorCommandMessage);++i)
	{
		if(DlgOperatorCommandMessage[i].message == LOWORD(wParam))
			(*DlgOperatorCommandMessage[i].fun)(hDlg,message,wParam,lParam);
	}
	return TRUE;
}


LONG OnOperatorOk(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	SipShowIM(SIPF_OFF);
	HWND hand = GetDlgItem(hDlg,IDC_BtnStart);
	SetFocus(hand);
	return TRUE;
}


/*坐标图初始化*/
LONG MapInit(HWND hDlg)
{

	//取得画布句柄
	HWND hStaticMap = GetDlgItem(hDlg,IDC_STATIC_MAP1);
	HDC hdc = GetDC(hStaticMap);
	RECT rect;
	GetClientRect(hStaticMap,&rect);
	HBRUSH hbrush,hOldbrush;

	HDC MemDC = CreateCompatibleDC(NULL);//首先定义一个显示设备对象 
	HBITMAP MemBitmap = CreateCompatibleBitmap(hdc,rect.right-rect.left,rect.bottom-rect.top);//定义一个与屏幕显示兼容位图对象 
	SelectObject(MemDC,MemBitmap);//将位图选入到内存显示设备中 //只有选入了位图的内存显示设备才有地方绘图，画到指定的位图上 
	

	//绘制画布背景
	hbrush = CreateSolidBrush(RGB(0,150,150));
	hOldbrush = (HBRUSH) SelectObject(MemDC,hbrush);
	SetBkMode(MemDC,TRANSPARENT);
	Rectangle(MemDC,rect.left,rect.top,rect.right,rect.bottom);

	//绘制坐标系
	COLORREF cr = RGB(0,0,255); 
	HPEN hPen = ::CreatePen(PS_SOLID, 3, cr);
	LOGBRUSH lb;lb.lbStyle = BS_SOLID;lb.lbColor = cr;
	hbrush = ::CreateBrushIndirect(&lb);	
	HPEN hOldPen = (HPEN)::SelectObject(MemDC, hPen);
	POINT lineStart,lineEnd,pointleft,pointbottom,pointright;
	// 画Y轴
	lineStart.x = rect.left + 20;lineStart.y = rect.bottom - 19;
	lineEnd.x = rect.left + 20; lineEnd.y = rect.top + 10;
	pointleft.x = lineEnd.x - 6; pointleft.y = lineEnd.y + 15;
	pointbottom.x = lineEnd.x; pointbottom.y = lineEnd.y + 5;
	pointright.x = lineEnd.x + 6;pointright.y = lineEnd.y + 15;
	::MoveToEx(MemDC, lineStart.x, lineStart.y, NULL);
	::LineTo(MemDC, lineEnd.x, lineEnd.y);
	POINT ptPolygon[4] = {lineEnd.x,lineEnd.y,pointleft.x,pointleft.y,pointbottom.x,pointbottom.y,pointright.x,pointright.y};
	::Polygon(MemDC, ptPolygon, 4);
	lineStart.x = rect.left + 19;lineStart.y = rect.bottom - 20;
	lineEnd.x = rect.right - 10; lineEnd.y = rect.bottom - 20;
	pointleft.x = lineEnd.x - 15; pointleft.y = lineEnd.y - 6;
	pointbottom.x = lineEnd.x - 5; pointbottom.y = lineEnd.y;
	pointright.x = lineEnd.x - 15;pointright.y = lineEnd.y + 6;
	// 画X轴
	::MoveToEx(MemDC, lineStart.x, lineStart.y, NULL);
	::LineTo(MemDC, lineEnd.x, lineEnd.y);
	POINT ptPolygon2[4] = {lineEnd.x,lineEnd.y,pointleft.x,pointleft.y,pointbottom.x,pointbottom.y,pointright.x,pointright.y};
	::Polygon(MemDC, ptPolygon2, 4);
	//画网格
	cr = RGB(255,255,0);
	HPEN hGridPen = ::CreatePen(PS_DASH,1,cr);
	::SelectObject(MemDC,hGridPen);
	int i = 0; int gap = 30;
	for (i = 1; i*gap<rect.right;i++)
	{
		::MoveToEx(MemDC,rect.left + 20 + i*gap,rect.bottom - 20,NULL);
		::LineTo(MemDC,rect.left + 20 + i*gap, rect.top + 10);


	}
	for(i = 1; i*gap <rect.bottom-50;i++)
	{
		::MoveToEx(MemDC,rect.left + 20,rect.bottom - 20 - i*gap,NULL);
		::LineTo(MemDC,rect.right - 10, rect.bottom - 20 - i*gap);
	}


	//将内存中的图拷贝到屏幕上进行显示 
	BitBlt(hdc,0,0,rect.right-rect.left,rect.bottom-rect.top,MemDC,0,0,SRCCOPY);

	//绘图完成后的清理 
	::SelectObject(MemDC, hOldPen);
	::SelectObject(MemDC,hOldbrush);
	::DeleteObject(hPen);
	::DeleteObject(hOldPen);
	::DeleteObject(hGridPen);
	::DeleteObject(hbrush);
	::DeleteObject(hOldbrush);
	::DeleteObject(MemBitmap);
	::DeleteObject(hdc);
	::DeleteObject(MemDC);

	return TRUE;
}

/*绘制速度曲线*/
LONG drawMap(HWND hDlg)
{

	//取得画布句柄
	HWND hStaticMap = GetDlgItem(hDlg,IDC_STATIC_MAP1);
	HDC hdc = GetDC(hStaticMap);
	RECT rect;
	GetClientRect(hStaticMap,&rect);
	HBRUSH hbrush,hOldbrush;

	HDC MemDC = CreateCompatibleDC(NULL);//首先定义一个显示设备对象 
	HBITMAP MemBitmap = CreateCompatibleBitmap(hdc,rect.right-rect.left,rect.bottom-rect.top);//定义一个与屏幕显示兼容位图对象 
	HGDIOBJ oldbmp = SelectObject(MemDC,MemBitmap);//将位图选入到内存显示设备中 //只有选入了位图的内存显示设备才有地方绘图，画到指定的位图上 


	//绘制画布背景
	hbrush = CreateSolidBrush(RGB(0,150,150));
	hOldbrush = (HBRUSH) SelectObject(MemDC,hbrush);
	SetBkMode(MemDC,TRANSPARENT);
	Rectangle(MemDC,rect.left,rect.top,rect.right,rect.bottom);

	//绘制坐标系
	COLORREF cr = RGB(0,0,255); 
	HPEN hPen = ::CreatePen(PS_SOLID, 3, cr);
	LOGBRUSH lb;lb.lbStyle = BS_SOLID;lb.lbColor = cr;
	hbrush = ::CreateBrushIndirect(&lb);	
	HPEN hOldPen = (HPEN)::SelectObject(MemDC, hPen);
	POINT lineStart,lineEnd,pointleft,pointbottom,pointright;

	// 画Y轴
	lineStart.x = rect.left + 20;lineStart.y = rect.bottom - 19;
	lineEnd.x = rect.left + 20; lineEnd.y = rect.top + 10;
	pointleft.x = lineEnd.x - 6; pointleft.y = lineEnd.y + 15;
	pointbottom.x = lineEnd.x; pointbottom.y = lineEnd.y + 5;
	pointright.x = lineEnd.x + 6;pointright.y = lineEnd.y + 15;
	::MoveToEx(MemDC, lineStart.x, lineStart.y, NULL);
	::LineTo(MemDC, lineEnd.x, lineEnd.y);
	POINT ptPolygon[4] = {lineEnd.x,lineEnd.y,pointleft.x,pointleft.y,pointbottom.x,pointbottom.y,pointright.x,pointright.y};
	::Polygon(MemDC, ptPolygon, 4);
	lineStart.x = rect.left + 19;lineStart.y = rect.bottom - 20;
	lineEnd.x = rect.right - 10; lineEnd.y = rect.bottom - 20;
	pointleft.x = lineEnd.x - 15; pointleft.y = lineEnd.y - 6;
	pointbottom.x = lineEnd.x - 5; pointbottom.y = lineEnd.y;
	pointright.x = lineEnd.x - 15;pointright.y = lineEnd.y + 6;
	// 画X轴
	::MoveToEx(MemDC, lineStart.x, lineStart.y, NULL);
	::LineTo(MemDC, lineEnd.x, lineEnd.y);
	POINT ptPolygon2[4] = {lineEnd.x,lineEnd.y,pointleft.x,pointleft.y,pointbottom.x,pointbottom.y,pointright.x,pointright.y};
	::Polygon(MemDC, ptPolygon2, 4);
	//画网格
	cr = RGB(255,255,0);
	HPEN hGridPen = ::CreatePen(PS_DASH,1,cr);
	::SelectObject(MemDC,hGridPen);
	int i = 0; int gap = 30;
	for (i = 1; i*gap<rect.right;i++)
	{
		::MoveToEx(MemDC,rect.left + 20 + i*gap,rect.bottom - 20,NULL);
		::LineTo(MemDC,rect.left + 20 + i*gap, rect.top + 10);


	}
	for(i = 1; i*gap <rect.bottom-50;i++)
	{
		::MoveToEx(MemDC,rect.left + 20,rect.bottom - 20 - i*gap,NULL);
		::LineTo(MemDC,rect.right - 10, rect.bottom - 20 - i*gap);
	}

	cr = RGB(255,0,150); 
	hPen = ::CreatePen(PS_SOLID, 2, cr);
	lb.lbStyle = BS_SOLID;lb.lbColor = cr;	
	hOldPen = (HPEN)::SelectObject(MemDC, hPen);
	POINT pStart,pEnd;

	i = 0; 
	int Index;
	Index = g_iIndex;
	for (i = 0; i< Index; i++)
	{
		if (i == 0)
		{
			pStart.x = rect.left + 20;pStart.y = rect.bottom - 20;
		}else
		{
			pStart.x = (i-1) * g_rx + g_tx;  pStart.y=-g_dwVelocity[i-1]*g_ry+g_ty;
		}

		pEnd.x=i * g_rx + g_tx;  pEnd.y=-g_dwVelocity[i]*g_ry+g_ty;
		::MoveToEx(MemDC, pStart.x, pStart.y, NULL);
		::LineTo(MemDC, pEnd.x, pEnd.y);
	}

	//将内存中的图拷贝到屏幕上进行显示 
	BitBlt(hdc,0,0,rect.right-rect.left,rect.bottom-rect.top,MemDC,0,0,SRCCOPY);

	//绘图完成后的清理 
	::SelectObject(MemDC, hOldPen);
	::SelectObject(MemDC,hOldbrush);
	::SelectObject(MemDC,oldbmp);
	::DeleteObject(hPen);
	::DeleteObject(hOldPen);
	::DeleteObject(hGridPen);
	::DeleteObject(hbrush);
	::DeleteObject(hOldbrush);
	::DeleteObject(MemBitmap);
	::DeleteObject(hdc);
	::DeleteDC(MemDC);
	return TRUE;
}
LONG OnIDC_EDIT_Type(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	HWND hand = GetDlgItem(hDlg,LOWORD(wParam));
	if (EN_SETFOCUS == HIWORD(wParam))
	{

		SetFocus(hand);
		SipShowIM(SIPF_ON);
		SipShowIM(SIPF_DOCKED);
	}
	if (EN_KILLFOCUS == HIWORD(wParam))
	{
		SipShowIM(SIPF_OFF);
	}

	return TRUE;
}
LONG OnIDC_EDIT_ID(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	HWND hand = GetDlgItem(hDlg,LOWORD(wParam));
	if (EN_SETFOCUS == HIWORD(wParam))
	{

		SetFocus(hand);
		SipShowIM(SIPF_ON);
		SipShowIM(SIPF_DOCKED);
	}
	if (EN_KILLFOCUS == HIWORD(wParam))
	{
		SipShowIM(SIPF_OFF);
	}

	return TRUE;
}
LONG OnIDC_EDIT_Dia(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	HWND hand = GetDlgItem(hDlg,LOWORD(wParam));
	if (EN_SETFOCUS == HIWORD(wParam))
	{

		SetFocus(hand);
		SipShowIM(SIPF_ON);
		SipShowIM(SIPF_DOCKED);
	}
	if (EN_KILLFOCUS == HIWORD(wParam))
	{
		SipShowIM(SIPF_OFF);
	}
	return TRUE;
}
LONG OnIDC_EDIT_RSpeed(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	/*
	HWND hand = GetDlgItem(hDlg,LOWORD(wParam));
	if (EN_SETFOCUS == HIWORD(wParam))
	{

		SetFocus(hand);
		SipShowIM(SIPF_ON);
		SipShowIM(SIPF_DOCKED);
	}
	if (EN_KILLFOCUS == HIWORD(wParam))
	{
		SipShowIM(SIPF_OFF);
	}*/
	return TRUE;
}


LONG OnBtnNext(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	MapInit(hDlg);

	g_iOrder++;
	g_iIndex = 0;
	MainThreadStart = TRUE;
	SetButtonEnble(hDlg,FALSE);
	return TRUE;
}
LONG OnBtnPrev(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	MapInit(hDlg);


	return TRUE;
}
LONG OnBtnPageUp(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	MapInit(hDlg);

	return TRUE;
}
LONG OnBtnPageDown(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	MapInit(hDlg);

	return TRUE;
}
/*响应开始测试按钮*/
LONG OnBtnStart(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	//MapInit(hDlg);
	SetButtonEnble(hDlg,FALSE);
	//读取类型
	g_sType[0] = NULL;
	HWND combox = GetDlgItem(hDlg,IDC_COMBOX_TYPE);
	ComboBox_GetText(combox,(LPWSTR)g_sType,10);


	if (*g_sType == NULL) 
	{
		SetButtonEnble(hDlg,TRUE);
		return FALSE;
	}

	//打开类型文档读取速度
	_stprintf_s(TestsFileName,50,_T("\\NandFlash\\detector_"));
	memcpy(TestsFileName+20,g_sType,5);
	int i = 0;
	for (i = 0; i <5; i++)
	{
		if (g_sType[i] == NULL)
		{
			break;
		}
	}
	_stprintf_s(TestsFileName+20+i,60,_T(".txt"));

	HANDLE hFile = CreateFile(TestsFileName,
		GENERIC_READ ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);
	if( hFile == INVALID_HANDLE_VALUE)
		return FALSE;
	char detector[120] = {NULL};//读取文件内容到该缓存
	TCHAR buf[20] = {NULL};
	DWORD dwRead = -1;
	ReadFile(hFile,(LPVOID)detector,120,&dwRead,NULL);
	memcpy(buf,detector+75,20);
	Edit_SetText(GetDlgItem(hDlg,IDC_EDIT_RSpeed),(LPCWSTR)buf);A	g_dwRatedSpeed = GetDlgItemInt(hDlg,IDC_EDIT_RSpeed,NULL,FALSE);
	memcpy(buf,detector+105,10);
	Edit_SetText(GetDlgItem(hDlg,IDC_EDIT_RSpeedLimitLow),(LPCWSTR)buf);
	g_dwRatedSpeedLimitLow = GetDlgItemInt(hDlg,IDC_EDIT_RSpeedLimitLow,NULL,FALSE);
	memcpy(buf,detector+95,10);
	Edit_SetText(GetDlgItem(hDlg,IDC_EDIT_RSpeedLimitUp),(LPCWSTR)buf);
	g_dwRatedSpeedLimitUp = GetDlgItemInt(hDlg,IDC_EDIT_RSpeedLimitUp,NULL,FALSE);




	CloseHandle(hFile);

	//读取编号

	g_sId[0] = NULL;
	GetDlgItemText(hDlg,IDC_EDIT_ID,g_sId,10);
	if (*g_sId0 == NULL)
	{
		for (i = 0; i<10; i++)
		{
			g_sId0[i] = g_sId[i];
			g_sType0[i] = g_sType[i];
		}

	}
	else
	{
		if ((*g_sType == *g_sType0)&&(*g_sId == *g_sId0))
		{
			SetButtonEnble(hDlg,TRUE);
			wsprintf(buf,TEXT("请改型号或点下一次！"));
			Edit_SetText(GetDlgItem(hDlg,IDC_BtnStart),buf);
			return FALSE;
		}

	}

	//g_sId[0] =(TCHAR ) GetDlgItemText(hDlg,IDC_EDIT_ID,NULL,FALSE);
	g_iOrder = 1;

	if (!demo)
	{


		if (hServoComPort == INVALID_HANDLE_VALUE) 
		{
			HWND hand = GetDlgItem(hDlg,IDC_BtnStart);
			TCHAR buf[64];
			wsprintf(buf,TEXT("与放大器串口通信失败！"));
			Button_SetText(hand,buf);
			SetButtonEnble(hDlg,TRUE);
			return FALSE;
		}
		if ( *g_sId == NULL)
		{
			HWND hand = GetDlgItem(hDlg,IDC_EDIT_ID);
			SetFocus(hand);
			TCHAR buf[64];
			wsprintf(buf,TEXT("请输编号！"));
			Edit_SetText(hand,buf);
			SetButtonEnble(hDlg,TRUE);
			return FALSE;
		}


	}
	else
	{
		Gonvernor gonvernor;
		gonvernor.SetGonvernor();
		gonvernor.GetVelocity(g_dwVelocity,count);

		TCHAR buf[64];
		HWND hand = GetDlgItem(hDlg,IDC_EDIT_Type);
		gonvernor.GetType(buf);
		Edit_SetText(hand,buf);

		hand = GetDlgItem(hDlg,IDC_EDIT_ID);
		gonvernor.GetID(buf);
		Edit_SetText(hand,buf);
		

		hand = GetDlgItem(hDlg,IDC_EDIT_RSpeed);
		//gonvernor.GetRSpeed(&dwbuf);
		//wsprintf(buf,TEXT("%.0f"),dwbuf);
		Edit_SetText(hand,buf);

		hand = GetDlgItem(hDlg,IDC_EDIT_ESpeed);
		//gonvernor.GetESpeed(&dwbuf);
		//wsprintf(buf,TEXT("%.0f"),dwbuf);
		//Edit_SetText(hand,buf);

		hand = GetDlgItem(hDlg,IDC_EDIT_MSpeed);
		//gonvernor.GetMSpeed(&dwbuf);
		//wsprintf(buf,TEXT("%.0f"),dwbuf);
		//Edit_SetText(hand,buf);

		int nbuf;
		hand = GetDlgItem(hDlg,IDC_EDIT_Order);
		gonvernor.GetOrder(&nbuf);
		gonvernor.GetOrder(&g_iIndex);
		wsprintf(buf,TEXT("%d"),nbuf);
		Edit_SetText(hand,buf);
	}

	MainThreadStart = TRUE;

	return TRUE;
}

LONG OnBtnOpen(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{

	OPENFILENAME of;
	TCHAR szFileName [MAX_PATH] = {0};
	const LPTSTR pszOpenFilter = TEXT("All File(*.*)\0*.*\0\0");
	szFileName[0] = '\0';
	memset(&of,0,sizeof(of));

	of.lStructSize= sizeof(of);
	of.hwndOwner = hDlg;
	of.lpstrFilter = pszOpenFilter;
	of.nMaxFile = dim(szFileName);
	of.lpstrFile = szFileName;
	of.Flags = 0;

	if ( GetOpenFileName(&of))
	{
		//Edit_SetText(GetDlgItem(hDlg,IDC_EDIT1),szFileName);
	}


	return TRUE;
}
LONG OnBtnSave(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	//MapInit(hDlg,message,wParam,lParam);

	return TRUE;
}




/*响应打印按钮*/
LONG OnBtnPrint(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	if (hPrinterComPort != INVALID_HANDLE_VALUE) 
	{
		PrinterCom(hDlg,hPrinterComPort);
	}
	
	return TRUE;
}

/*关闭窗口*/
LONG OnClose(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	g_iIndex = 0;
	SipShowIM(SIPF_OFF);
	MapFlag = FALSE;
	MainFlag = FALSE;
	CloseHandle(hMapThread);
	CloseHandle(hmainThread);
	CloseHandle(m_Event);
	EndDialog(hDlg,0);
	return TRUE;
}



/*MapThread,速度曲线线程*/
DWORD WINAPI MapThread(LPVOID lpVoid)
{
	while(MapFlag)
	{
		HWND hDlg = (HWND)lpVoid;
		/*以下为调试代码，需删除*/
		drawMap(hDlg);
		Sleep(1);
		/*以上为调试代码需删除*/
		
	}

	return TRUE;
}
/*MainThread,伺服控制线程*/
DWORD WINAPI MainThread(LPVOID lpVoid)
{
	HWND hDlg = (HWND)lpVoid;

	while(MainFlag)
	{
		if (MainThreadStart)
		{
			MainThreadStart = FALSE;
			TCHAR buf[20];
			wsprintf(buf,TEXT("%d"),g_iOrder);
			Edit_SetText(GetDlgItem(hDlg,IDC_EDIT_Order),(LPCWSTR)buf);

			HWND hand = GetDlgItem(hDlg,IDC_BtnStart);
			int Rspeed = GetDlgItemInt(hDlg,IDC_EDIT_RSpeed,NULL,FALSE);
			//取得画布句柄
			HWND hStaticMap = GetDlgItem(hDlg,IDC_STATIC_MAP1);
			HDC hdc = GetDC(hStaticMap);
			RECT rect;
			GetClientRect(hStaticMap,&rect);
			//设置坐标变换系数
			g_tx = rect.left + 20;
			g_ty = rect.bottom - 20;
			g_ry = (rect.bottom - rect.top-40)/(Rspeed*1.5);


			 hand = GetDlgItem(hDlg,IDC_BtnStart);
			//发送指令
			if (StartMovePosition(hServoComPort,NULL,g_dwRatedSpeed))
			{
				
				SetFocus(hand);
				
				wsprintf(buf,TEXT("请按下启动按钮！"));
				Edit_SetText(hand,buf);
			}
			
			//读取速度
			DWORD cBytes, i = 0;
			DWORD MaxSpeed = 0;
			do 
			{
				i++;
				INT16 szAnsi[2] = {0};//每次接受16*2位即4字节的数据
				ReadFile (hServoComPort, szAnsi, 4, &cBytes, 0);//每次接受16位的数据
				g_dwVelocity[i] = szAnsi[0]/velocity2rate;
				hand = GetDlgItem(hDlg,IDC_EDIT_Velocity);
				wsprintf(buf,TEXT("速度是%0.2f"),g_dwVelocity[i]);
				Edit_SetText(hand,buf);


				g_rx = (rect.right - rect.left-40)/(400+0.0);
				if (i > 400)
				{
					g_rx = (rect.right - rect.left-40)/(i+0.0);
				}
				
				if (g_dwVelocity[i]>MaxSpeed)
				{
					MaxSpeed = g_dwVelocity[i];
				}

				if (MaxSpeed > 1.5*Rspeed)
				{
					g_ry = (rect.bottom - rect.top-40)/(MaxSpeed*1.1);
				}

				g_iIndex = (g_iIndex+1);
				//StartMovePosition(hServoComPort,NULL,g_dwVelocity[i]);
				if (szAnsi[1] == 0xFF)
				{
					g_dwElecSpeed = g_dwVelocity[i];
					g_dwMechaSpeed = MaxSpeed;

					MainThreadStart = FALSE;
					break;
				}
			} while (i < count);

			SetButtonEnble(hDlg,TRUE);
			wsprintf(buf,TEXT("开始测量！"));
			Edit_SetText(GetDlgItem(hDlg,IDC_BtnStart),buf);
			wsprintf(buf,TEXT("%0.2f"),g_dwElecSpeed);
			Edit_SetText(GetDlgItem(hDlg,IDC_EDIT_ESpeed),buf);
			wsprintf(buf,TEXT("%0.2f"),g_dwMechaSpeed);
			Edit_SetText(GetDlgItem(hDlg,IDC_EDIT_MSpeed),buf);
				
			if ((g_dwRatedSpeedLimitLow<g_dwElecSpeed)&&(g_dwMechaSpeed<g_dwRatedSpeedLimitUp))
			{
				wsprintf(buf,TEXT("合格"));
				Edit_SetText(GetDlgItem(hDlg,IDC_EDIT_Qualified),buf);
			}
			else
			{
				wsprintf(buf,TEXT("不合格"));
				Edit_SetText(GetDlgItem(hDlg,IDC_EDIT_Qualified),buf);

			}


		//保存数据

			//写入限速器类型文件
			HANDLE hFiletest = CreateFile(TestsFileName,
			GENERIC_WRITE ,
			FILE_SHARE_READ,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			0);
			if( hFiletest == INVALID_HANDLE_VALUE)
			return FALSE;

			SetFilePointer(hFiletest,0,NULL,FILE_END);

			DWORD dwWriete = -1;size_t siz;

			char test[50] = {NULL};
			TCHAR temp[15] = {NULL};
			do 
			{	
				memcpy(test,g_sId,10);
				GetDlgItemText(hDlg,IDC_EDIT_ESpeed,(LPWSTR)temp,15);
				memcpy(test+10,temp,15);
				GetDlgItemText(hDlg,IDC_EDIT_MSpeed,(LPWSTR)temp,15);
				memcpy(test+25,temp,15);
				LPCTSTR buf = _T("END");
				memcpy(test+47,buf,5);
	
				
			} while (!WriteFile(hFiletest,test,50,&dwWriete,NULL));

			CloseHandle(hFiletest);
		}
		else
		{
			Sleep(100);
		}

	}	
	return TRUE;
}

void SetButtonEnble(HWND hDlg,BOOL B)
{

		//HWND hand = GetDlgItem(hDlg,IDC_EDIT_Type);

		HWND hand = GetDlgItem(hDlg,IDC_BtnNext);
		Button_Enable(hand,B);

		 hand = GetDlgItem(hDlg,IDC_BtnPrev);
		Button_Enable(hand,B);

		 hand = GetDlgItem(hDlg,IDC_BtnPageUp);
		Button_Enable(hand,B);

		 hand = GetDlgItem(hDlg,IDC_BtnPageDown);
		Button_Enable(hand,B);

		 hand = GetDlgItem(hDlg,IDC_BtnSave);
		Button_Enable(hand,B);

		 hand = GetDlgItem(hDlg,IDC_BtnPrint);
		Button_Enable(hand,B);

		 hand = GetDlgItem(hDlg,IDC_BtnStart);
		Button_Enable(hand,B);

		hand = GetDlgItem(hDlg,IDC_BtnOpen);
		Button_Enable(hand,B);

		hand = GetDlgItem(hDlg,IDC_BtnClose);
		Button_Enable(hand,B);
}

LONG OnInit(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
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

		char buf[120] = {NULL};//读取文件内容到该缓存
		TCHAR numBuf[50];	//单元格内容缓存
		DWORD dwRead = -1;
		int index = 0;

		int i = 0;
		do 
		{
			for (i = 0; i < 120; i++)
			{
				buf[i] = NULL;
			}
			ReadFile(hFile,(LPVOID)buf,120,&dwRead,NULL);

			if (*buf == NULL)
			{
				break;
			}
			memcpy(numBuf,buf+20,5);
			HWND comboxType = GetDlgItem(hDlg,IDC_COMBOX_TYPE);
			ComboBox_AddString(comboxType,numBuf);

		} while (1);

		index++;
	CloseHandle(hFile);

	return TRUE;
}