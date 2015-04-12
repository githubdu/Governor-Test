#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <winuser.h>
#include "com.h"
#include "resource.h"
#include "detector.h"


WCHAR	SendText[amount];
HANDLE	hPrinterComPort = INVALID_HANDLE_VALUE;
HANDLE  hServoComPort = INVALID_HANDLE_VALUE;

HANDLE	hComReadThread	= INVALID_HANDLE_VALUE;
HANDLE	hComSendThread	= INVALID_HANDLE_VALUE;
DWORD	dwRStatID,dwSStatID;


/*串口通信初始化*/
HANDLE InitCommunication ( LPTSTR pszDevName,HANDLE hComPort,int parity) 
{
	
	int nSpeed = CBR_9600;
	DCB dcb;
	TCHAR szDbg[128];
	COMMTIMEOUTS cto;
	HANDLE hLocal;

	hLocal = hComPort;
	hComPort = INVALID_HANDLE_VALUE;

	if (hLocal != INVALID_HANDLE_VALUE)
		CloseHandle (hLocal);  

	hLocal = CreateFile (pszDevName, GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, 0, NULL);

	if (hLocal != INVALID_HANDLE_VALUE) 
	{
		// 配置串口
		dcb.DCBlength = sizeof (dcb);
		GetCommState (hLocal, &dcb);
		dcb.BaudRate = nSpeed;

		if (parity == NOPARITY)
		{
			dcb.fParity = FALSE;
		}
		else
		{
			dcb.fParity = TRUE;
		}
		dcb.Parity = parity;
		
		dcb.fNull = FALSE;
		dcb.StopBits = ONESTOPBIT;
		dcb.ByteSize = 8;
		SetCommState (hLocal, &dcb);

		// 设定超时
		cto.ReadIntervalTimeout = 0;
		cto.ReadTotalTimeoutMultiplier = 0;
		cto.ReadTotalTimeoutConstant = 0;
		cto.WriteTotalTimeoutMultiplier = 0;
		cto.WriteTotalTimeoutConstant = 0;
		SetCommTimeouts (hLocal, &cto);
		hComPort = hLocal;
	} 
	else 
	{
		wsprintf (szDbg, TEXT ("Couldn\'t open port %s. rc=%d\r\n"),
			pszDevName, GetLastError());
		MessageBox(NULL,szDbg,TEXT("message"),MB_OK | MB_ICONERROR);
	}

	return hComPort;
}




/*打印机串口通信处理程序*/
BOOL PrinterCom(HWND hDlg,HANDLE hComPort)
{

	HANDLE hFile = CreateFile(TestsFileName,
		GENERIC_READ ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);
	if( hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	FILETIME CreteTime,LastTime,LastWrite;
	GetFileTime(hFile,&CreteTime,&LastTime,&LastWrite);

	SYSTEMTIME st;

	FILETIME locTime;
	FileTimeToLocalFileTime(&LastTime,&locTime);

	FileTimeToSystemTime(&locTime,&st);

	TCHAR buf[64];TCHAR buf2[20];
	wsprintf(buf,TEXT("%d - %d - %d : %d:%d"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute);


	CloseHandle(hFile);


	int rc;
	DWORD cBytes;
	char szAnsi[amount];
	size_t siz;

	//发送打印机初始化命令
	char head[] = {0x1B,0x40,0x1C,0x26};
	//rc = WriteFile(hComPort,&head,strlen(head),&cBytes,0);
	
	// 获得发送内容并转换成asci码
	wsprintf (SendText, TEXT ("*****************************\n\n         限速器检测报告\n\n"));
	wcstombs_s (&siz, szAnsi, sizeof (szAnsi), SendText, _TRUNCATE);
	rc = WriteFile(hComPort,szAnsi,strlen(szAnsi),&cBytes,0);
	wsprintf (SendText, TEXT ("型号："));
	wcstombs_s (&siz, szAnsi, sizeof (szAnsi), SendText, _TRUNCATE);
	wcstombs_s (&siz, szAnsi+6, sizeof (szAnsi), (wchar_t *)g_sType, _TRUNCATE);
	rc = WriteFile(hComPort,szAnsi,strlen(szAnsi),&cBytes,0);

	wsprintf (SendText, TEXT ("\n编号："));
	wcstombs_s (&siz, szAnsi, sizeof (szAnsi), SendText, _TRUNCATE);
	rc = WriteFile(hComPort,szAnsi,strlen(szAnsi),&cBytes,0);
	GetDlgItemText(hDlg,IDC_EDIT_ID,buf2,10);
	wcstombs_s (&siz, szAnsi, sizeof (szAnsi),buf2, _TRUNCATE);
	rc = WriteFile(hComPort,szAnsi,strlen(szAnsi),&cBytes,0);

	wsprintf (SendText, TEXT ("\n额定速度："));
	wcstombs_s (&siz, szAnsi, sizeof (szAnsi), SendText, _TRUNCATE);
	rc = WriteFile(hComPort,szAnsi,strlen(szAnsi),&cBytes,0);
	GetDlgItemText(hDlg,IDC_EDIT_RSpeed,buf2,10);
	wcstombs_s (&siz, szAnsi, sizeof (szAnsi),buf2, _TRUNCATE);
	rc = WriteFile(hComPort,szAnsi,strlen(szAnsi),&cBytes,0);

	wsprintf (SendText, TEXT ("\n电气动作速度：%0.2f"),g_dwElecSpeed);
	wcstombs_s (&siz, szAnsi, sizeof (szAnsi), SendText, _TRUNCATE);
	rc = WriteFile(hComPort,szAnsi,strlen(szAnsi),&cBytes,0);

	wsprintf (SendText, TEXT ("\n机械动作速度：%0.2f"),g_dwMechaSpeed);
	wcstombs_s (&siz, szAnsi, sizeof (szAnsi), SendText, _TRUNCATE);
	rc = WriteFile(hComPort,szAnsi,strlen(szAnsi),&cBytes,0);

	wsprintf (SendText, TEXT ("\n第几次测量：%d"),g_iOrder);
	wcstombs_s (&siz, szAnsi, sizeof (szAnsi), SendText, _TRUNCATE);
	rc = WriteFile(hComPort,szAnsi,strlen(szAnsi),&cBytes,0);

	wsprintf (SendText, TEXT ("\n是否合格："));
	wcstombs_s (&siz, szAnsi, sizeof (szAnsi), SendText, _TRUNCATE);
	rc = WriteFile(hComPort,szAnsi,strlen(szAnsi),&cBytes,0);
	GetDlgItemText(hDlg,IDC_EDIT_Qualified,buf2,10);
	wcstombs_s (&siz, szAnsi, sizeof (szAnsi),buf2, _TRUNCATE);
	rc = WriteFile(hComPort,szAnsi,strlen(szAnsi),&cBytes,0);

	wsprintf (SendText, TEXT ("\n\n\n检测日期："));
	wcstombs_s (&siz, szAnsi, sizeof (szAnsi), SendText, _TRUNCATE);
	rc = WriteFile(hComPort,szAnsi,strlen(szAnsi),&cBytes,0);
	wcstombs_s (&siz, szAnsi, sizeof (szAnsi), buf, _TRUNCATE);
	rc = WriteFile(hComPort,szAnsi,strlen(szAnsi),&cBytes,0);
	wsprintf (SendText, TEXT ("\n\n********************************\n\n\n"));
	wcstombs_s (&siz, szAnsi, sizeof (szAnsi), SendText, _TRUNCATE);
	rc = WriteFile(hComPort,szAnsi,strlen(szAnsi),&cBytes,0);





	/*
	if (!rc) 
	{
		// 如果写入失败，提示错误信息
		wsprintf (SendText, TEXT ("Send failed rc=%d\r\n"), 
			GetLastError());
		DWORD dwErr = 0;
		COMSTAT Stat;
		if (ClearCommError (hComPort, &dwErr, &Stat)) 
		{
			MessageBox(NULL,SendText,TEXT("message"),0);
		}
	}
	*/
	return TRUE;
}



/*放大器串口通信处理程序*/
BOOL ServoCom(HANDLE hComPort)
{

	if (!GetExitCodeThread (hComReadThread, &dwRStatID) ||(dwRStatID != STILL_ACTIVE)) 
	{
		hComReadThread = CreateThread (NULL, 0, ServoComReadThread,hComPort,0, &dwRStatID);
	}
	if (!GetExitCodeThread (hComSendThread, &dwSStatID) ||(dwSStatID != STILL_ACTIVE)) 
	{
		hComSendThread = CreateThread (NULL, 0, ServoComSendThread,hComPort,0, &dwSStatID);
	}

	return TRUE;
}

DWORD WINAPI ServoComSendThread (PVOID pArg) 
{

	HANDLE hComPort = (HANDLE) pArg;
	SendPackage(hComPort);
	return TRUE;
}


DWORD WINAPI ServoComReadThread (PVOID pArg) 
{
	HANDLE hComPort = (HANDLE) pArg;

	return TRUE;
}


DWORD SendPackage(HANDLE hComPort)
{
	//WCHAR	SendText[amount];
	
	int rc;
	DWORD cBytes;
	char szAnsi[amount];
	size_t siz;


	// 获得发送内容并转换成asci码

	StringCchCat (SendText, dim(SendText), TEXT ("你好\r\n"));
	wcstombs_s (&siz, szAnsi, sizeof (szAnsi), SendText, _TRUNCATE);

	// 写入到串口
	rc = WriteFile(hComPort,szAnsi,strlen(szAnsi),&cBytes,0);

	if (!rc) {
		// 如果写入失败，提示错误信息
		wsprintf (SendText, TEXT ("Send failed rc=%d\r\n"), 
			GetLastError());
		DWORD dwErr = 0;
		COMSTAT Stat;

		if (ClearCommError (hComPort, &dwErr, &Stat)) {
			MessageBox(NULL,SendText,TEXT("message"),0);
		}
	}
	return TRUE;
}
DWORD ReadPackage(HANDLE hComPort)
{
	DWORD cBytes, i;
	WCHAR szText[32];
	char szAnsi[32];
	size_t siz;
	while(1)
	{

		for (i = 0; i < sizeof (szAnsi)-1; i++) {

			while (!ReadFile (hComPort, &szAnsi[i], 1, &cBytes, 0))
				if (hComPort == INVALID_HANDLE_VALUE)
					return 0;

			if (i >1 &&szAnsi[i] == '\n' && szAnsi[i-1] == '\0') {
				break;
			}
		}
		mbstowcs_s (&siz, szText, dim (szText), szAnsi, _TRUNCATE);
		MessageBox(NULL,szText,TEXT("message"),MB_OK | MB_ICONERROR);

	}

	return TRUE;
}

/**/
DWORD StartMovePosition(HANDLE hComPort,double position,double velocity)
{


	int rc;
	DWORD cBytes;
	int speed = (int) velocity * velocity2rate;
	//rc = WriteFile(hComPort,&speed,sizeof(speed),&cBytes,0);

	//发送初始化命令
	INT16 head[4] = {0};
	memcpy(head,&speed,2);head[1]=0x21;
	speed = speed *5*100;
	memcpy(head+2,&(speed),4);

	rc = WriteFile(hComPort,&head,8,&cBytes,0);



	return TRUE;
}

DWORD ReadVelocity(HANDLE hComPort,double position,double velocity)
{




	return TRUE;
}
	/*
	int rc;
	DWORD cBytes;
	char head[] = {0x01,0x30,0x38,0x42,0x02,0x30,0x30,0x30,0x30,0x30,0x32,0x03,0x44,0x31};
	rc = WriteFile(hComPort,&head,strlen(head),&cBytes,0);

	char head1[] = {0x01,0x30,0x41,0x30,0x02,0x31,0x30,0x30,0x33,0x45,0x38,0x03,0x45,0x37};
	rc = WriteFile(hComPort,&head1,strlen(head1),&cBytes,0);

	char head2[] = {0x01,0x30,0x41,0x30,0x02,0x31,0x31,0x30,0x30,0x36,0x34,0x03,0x44,0x32};
	rc = WriteFile(hComPort,&head2,strlen(head2),&cBytes,0);

	char head3[] = {0x01,0x30,0x41,0x30,0x02,0x32,0x30,0x33,0x42,0x39,0x41,0x43,0x39,0x46,0x46,0x03,0x46,0x46};
	rc = WriteFile(hComPort,&head3,strlen(head3),&cBytes,0);

	char head4[] = {0x01,0x30,0x41,0x30,0x02,0x32,0x31,0x30,0x30,0x30,0x30,0x03,0x43,0x39};
	rc = WriteFile(hComPort,&head4,strlen(head4),&cBytes,0);

	char head5[] = {0x01,0x30,0x39,0x32,0x02,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x37,0x03,0x38,0x37};
	rc = WriteFile(hComPort,&head5,strlen(head5),&cBytes,0);

	char head6[] = {0x01,0x30,0x41,0x30,0x02,0x34,0x30,0x31,0x45,0x41,0x35,0x03,0x46,0x36};
	rc = WriteFile(hComPort,&head6,strlen(head6),&cBytes,0);

	Sleep(10000);

	char head7[] = {0x01,0x30,0x38,0x42,0x02,0x30,0x30,0x30,0x30,0x30,0x30,0x03,0x43,0x46};
	rc = WriteFile(hComPort,&head7,strlen(head7),&cBytes,0);
	*/