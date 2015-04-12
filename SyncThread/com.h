#include "resource.h"


#define dim(x) (sizeof(x)/sizeof(x[0]))
extern WCHAR	SendText[amount];
extern HANDLE	hPrinterComPort;
extern HANDLE	hServoComPort;

extern HANDLE	hComReadThread;
extern HANDLE	hComSendThread;
extern DWORD	dwRStatID,dwSStatID;

HANDLE InitCommunication ( LPTSTR pszDevName,HANDLE hComPort,int parity);


BOOL PrinterCom(HWND hDlg,HANDLE hComPort);


BOOL ServoCom(HANDLE hComPort);

DWORD WINAPI ServoComSendThread (PVOID pArg) ;
DWORD WINAPI ServoComReadThread (PVOID pArg);

DWORD SendPackage(HANDLE hComPort);
DWORD ReadPackage(HANDLE hComPort);

DWORD StartMovePosition(HANDLE hComPort,double position,double velocity);
DWORD ReadVelocity(HANDLE hComPort,double position,double velocity);

