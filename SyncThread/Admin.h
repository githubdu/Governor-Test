#include "resource.h"
#include "detector.h"



TCHAR name[20];
TCHAR buf[64];


#define NUM_COLUMNS			4
#define MAX_PROID			12
#define MAX_NAME			64
#define MAX_PRIOTRY			16
#define MAX_THREADS			12


#define DETECTOR_TYPE		20
#define DETECTOR_CODE		5
#define	DETECTOR_INFO		50

typedef struct tag_detector
{
	int detectorID;
	TCHAR Type[20];
	TCHAR Code[5];
	TCHAR Info[50];
} TAG_DETECTOR;




//ϵͳ��Ϣ����
LONG DoAdminInit(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG DoAdminCommand(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
//LONG OnAdminOk(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);

//��ť����

LONG OnAdminCancel(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG OnAdminListInit(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG OnAddClick(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
//LONG OnStyleClick(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG OnDelClick(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG OnAdminModifyClick(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);


