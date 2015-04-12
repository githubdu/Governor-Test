#include "resource.h"
#include "detector.h"


typedef struct tag_detector
{
	int detector_tests_ID;
	TCHAR Code[5];
	TCHAR ESpeed[20];
	TCHAR MSpeed[20];
} TAG_DETECTOR_TESTS;




//ϵͳ��Ϣ����
LONG DoAdminAddInit(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG DoAdminAddCommand(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);


//��ť����
LONG OnAdminAddOK(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG OnAdminAddCancel(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
//�ı�����
LONG OnIDC_EDIT_AdminAdd_Type(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG OnIDC_EDIT_AdminAdd_Code(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG OnIDC_EDIT_AdminAdd_Info(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG OnIDC_EDIT_Admin_Speed(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LONG OnIDC_EDIT_Admin_SpeedLimit(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);


LONG AdminAddInit(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);