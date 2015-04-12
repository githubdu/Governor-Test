#include "AdminAdd.h"


#define NUM_COLUMNS		4



//�Ѳ����ݱ���ͷ
TCHAR * ColumNameAdd[] =
{
	TEXT("���"),
	TEXT("����"),
	TEXT("����"),
	TEXT("��е"),
};

int ColumWidthAdd[NUM_COLUMNS] = {75,150,150,150};


TAG_DETECTOR_TESTS detector_tests[20];//???


TCHAR	FileName2[50];//

HWND hWndList_tests;




/*��������Ϣ*/
struct DecodeUnit DlgAdminAddMessage[] ={
	WM_INITDIALOG,	DoAdminAddInit,
	WM_COMMAND,		DoAdminAddCommand,
};

/*����ؼ���Ϣ*/
struct DecodeUnit DlgAdminAddCommandMessage[] = {
	IDD_Admin_Add_OK,		OnAdminAddOK,
	IDD_Admin_Add_CANCEL,	OnAdminAddCancel,
	IDC_EDIT_Admin_Type,	OnIDC_EDIT_AdminAdd_Type,
	IDC_EDIT_Admin_Code,	OnIDC_EDIT_AdminAdd_Code,
	IDC_EDIT_Admin_Info,	OnIDC_EDIT_AdminAdd_Info,
	IDC_EDIT_Admin_Speed,	OnIDC_EDIT_Admin_Speed,
	IDC_EDIT_Admin_SpeedLimit,OnIDC_EDIT_Admin_SpeedLimit,
	IDC_EDIT_Admin_SpeedLimit2,OnIDC_EDIT_Admin_SpeedLimit,
	IDC_EDIT_Admin_Dia,		OnIDC_EDIT_AdminAdd_Type,

};





/*�����ͺŶԻ�����Ϣ������*/
INT_PTR CALLBACK DlgAdminAddPorc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	int i;
	for (i = 0;i < dim(DlgAdminAddMessage);i++)
	{
		if(DlgAdminAddMessage[i].message == message)
			(*DlgAdminAddMessage[i].fun)(hDlg,message,wParam,lParam);
	}
	return FALSE;
}

/*�ؼ���������ʼ��*/
LONG DoAdminAddCommand(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	int i ;
	for(i =0 ;i< dim(DlgAdminAddCommandMessage);++i)
	{
		if(DlgAdminAddCommandMessage[i].message == LOWORD(wParam))
			(*DlgAdminAddCommandMessage[i].fun)(hDlg,message,wParam,lParam);
	}
	return TRUE;
}


/*���ڳ�ʼ��*/
LONG DoAdminAddInit(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	SetWindowPos(hDlg,NULL,0,0,800,480,SWP_SHOWWINDOW);
	hWndList_tests = GetDlgItem(hDlg,IDC_LIST_Admin_tests);//һ��ָ���Ѳ����ݱ���ȫ��ָ��

	//_stprintf_s(FileName2,50,_T("\\NandFlash\\detector_test.txt"));//�����������ļ�


	AdminAddInit(hDlg,message,wParam,lParam);


	return TRUE;  
}

LONG AdminAddInit(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{

	if (ModifyMark != 0)
	{
		//��ȡ�����������ļ�
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
		char detector[120] = {NULL};//��ȡ�ļ�ͷ���ݵ��û���
		char buf1[20] = {NULL};
		char buf2[5]  = {NULL};
		char buf3[50] = {NULL};
		char buf4[20] = {NULL};
		char buf5[10] = {NULL};
		char buf6[10] = {NULL};
		DWORD dwRead = -1;
		SetFilePointer(hFile,(ModifyMark-1)*120,NULL,FILE_CURRENT);
		ReadFile(hFile,(LPVOID)detector,120,&dwRead,NULL);
		CloseHandle(hFile);

		memcpy(buf1,detector,20);
		Edit_SetText(GetDlgItem(hDlg,IDC_EDIT_Admin_Type),(LPCWSTR)buf1);
		memcpy(buf2,detector+20,5);
		Edit_SetText(GetDlgItem(hDlg,IDC_EDIT_Admin_Code),(LPCWSTR)buf2);
		memcpy(buf3,detector+25,50);
		Edit_SetText(GetDlgItem(hDlg,IDC_EDIT_Admin_Info),(LPCWSTR)buf3);
		memcpy(buf4,detector+75,20);
		Edit_SetText(GetDlgItem(hDlg,IDC_EDIT_Admin_Speed),(LPCWSTR)buf4);
		memcpy(buf5,detector+95,10);
		Edit_SetText(GetDlgItem(hDlg,IDC_EDIT_Admin_SpeedLimit),(LPCWSTR)buf5);
		memcpy(buf6,detector+105,10);
		Edit_SetText(GetDlgItem(hDlg,IDC_EDIT_Admin_SpeedLimit2),(LPCWSTR)buf6);
	}





	//�Ѳ����ݱ���ͷ��ʼ��
	int index = 0;
	LV_COLUMN lvC;	//��ͷ����			
	TCHAR szText[MAX_PATH];		
	lvC.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;//�б�ʾ
	lvC.fmt = LVCFMT_LEFT;  //��ͷ���뷽ʽ      
	lvC.pszText = szText;	//��ͷ����
	for (index = 0; index < NUM_COLUMNS; index++)
	{
		lvC.iSubItem = index;
		lvC.cx = ColumWidthAdd[index]; //�п��
		wcscpy_s(szText,sizeof(ColumNameAdd[index]),ColumNameAdd[index]);
		if (ListView_InsertColumn(hWndList_tests, index, &lvC) == -1)
			return NULL;
	}//��ͷ��ʼ�����





	/*

	//д�������������ļ�
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

	DWORD dwWriete = -1;
	TCHAR tests[50] = {NULL};

	do 
	{	
		_stprintf_s(tests,50,_T("jh   12             13"));

	} while (!WriteFile(hFiletest,tests,50,&dwWriete,NULL));
	CloseHandle(hFiletest);

*/




	//��ȡ�������Ѳ������ļ�
	HANDLE hFile = CreateFile(TestsFileName,
		GENERIC_READ ,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		0);
	if( hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	index = 0;
	char buf[50] = {NULL};//��ȡ�ļ����ݵ��û���
	TCHAR numBuf[20];	//��Ԫ�����ݻ���
	DWORD dwRead = -1;

	LVITEM lvI;	//listview������
	lvI.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
	lvI.state = 0;      
	lvI.stateMask = 0; 
	SetFilePointer(hFile,120,NULL,FILE_CURRENT);

	ReadFile(hFile,(LPVOID)buf,50,&dwRead,NULL);
	while (*buf != NULL)
	{

		lvI.iItem = index;
		lvI.iSubItem = 0;
		lvI.iImage = index;

		wsprintf(numBuf,TEXT("%4d"),index+1);
		detector_tests[index].detector_tests_ID = index+1;
		lvI.pszText = numBuf;
		int p = ListView_InsertItem(hWndList_tests, &lvI);

		int iSubItem = 1;
		memcpy(detector_tests[index].Code,&buf,10);
		ListView_SetItemText( hWndList_tests,p,iSubItem++,detector_tests[index].Code);
		memcpy(detector_tests[index].ESpeed,&buf[10],15);
		ListView_SetItemText( hWndList_tests,p,iSubItem++,detector_tests[index].ESpeed);
		memcpy(detector_tests[index].MSpeed,&buf[25],15);
		ListView_SetItemText( hWndList_tests,p,iSubItem++,detector_tests[index].MSpeed);

		int i = 0;
		for (i = 0; i < 50; i++)
		{
			buf[i] = NULL;
		}
		ReadFile(hFile,(LPVOID)buf,50,&dwRead,NULL);
		index++;
	}
	CloseHandle(hFile);




	return TRUE;
}


LONG OnAdminAddCancel(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	ModifyMark = 0;
	EndDialog(hDlg,0);
	DialogBox(NULL,MAKEINTRESOURCE(IDD_DIALOG_Admin),NULL,DlgAdminPorc);
	return TRUE;
}



LONG OnAdminAddOK(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{


	GetDlgItemText(hDlg,IDC_EDIT_Admin_Type,Type,20);
	GetDlgItemText(hDlg,IDC_EDIT_Admin_Code,CODE,5);
	GetDlgItemText(hDlg,IDC_EDIT_Admin_Info,INFO,50);
	GetDlgItemText(hDlg,IDC_EDIT_Admin_Speed,(LPWSTR)Speed,20);
	GetDlgItemText(hDlg,IDC_EDIT_Admin_SpeedLimit,(LPWSTR)Limit,10);
	GetDlgItemText(hDlg,IDC_EDIT_Admin_SpeedLimit2,(LPWSTR)Limit+10,10);



	//д�������������ļ�
	HANDLE hFile = CreateFile(TypeFileName,
		GENERIC_WRITE ,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		0);
	if( hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	SetFilePointer(hFile,0,NULL,FILE_END);

	_stprintf_s(TestsFileName,50,_T("\\NandFlash\\detector_"));
	memcpy(TestsFileName+20,CODE,5);
	int i = 0;
	for (i = 0; i <5; i++)
	{
		if (CODE[i] == NULL)
		{
			break;;
		}
	}
	_stprintf_s(TestsFileName+20+i,60,_T(".txt"));



	if (ModifyMark !=0)
	{
		
		TCHAR	TestsFileNameTemp[100];
		SetFilePointer(hFile,0,NULL,FILE_BEGIN);
		SetFilePointer(hFile,(ModifyMark-1)*120,NULL,FILE_CURRENT);

		_stprintf_s(TestsFileNameTemp,50,_T("\\NandFlash\\detector_"));
		memcpy(TestsFileNameTemp+20,CODE,5);
		int i = 0;
		for (i = 0; i <5; i++)
		{
			if (CODE[i] == NULL)
			{
				break;;
			}
		}
		_stprintf_s(TestsFileNameTemp+20+i,60,_T(".txt"));
		MoveFile(TestsFileName,TestsFileNameTemp);
		DeleteFile(TestsFileName);
		memcpy(TestsFileName,TestsFileNameTemp,100);
	}


	DWORD dwWriete = -1;
	char detector[120] = {NULL};
	do 
	{	
		LPCTSTR buf = _T("END");
		memcpy(&detector,Type,20);memcpy(&detector[20],CODE,5);memcpy(&detector[25],INFO,50);
		memcpy(&detector[75],Speed,20);memcpy(&detector[95],Limit,20);memcpy(&detector[105],Limit+10,20);
		memcpy(&detector[115],buf,5);

	} while (!WriteFile(hFile,detector,120,&dwWriete,NULL));
	CloseHandle(hFile);


	//д�������������ļ�
	HANDLE hFiletest = CreateFile(TestsFileName,
		GENERIC_WRITE ,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		0);
	if( hFiletest == INVALID_HANDLE_VALUE)
		return FALSE;

	SetFilePointer(hFiletest,0,NULL,FILE_BEGIN);

	 dwWriete = -1;

	do 
	{	

	} while (!WriteFile(hFiletest,detector,120,&dwWriete,NULL));
	CloseHandle(hFiletest);






	ModifyMark = 0;
	EndDialog(hDlg,0);
	DialogBox(NULL,MAKEINTRESOURCE(IDD_DIALOG_Admin),NULL,DlgAdminPorc);
	return TRUE;
}

//�ı�����
LONG OnIDC_EDIT_AdminAdd_Type(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
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
LONG OnIDC_EDIT_AdminAdd_Code(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
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
LONG OnIDC_EDIT_AdminAdd_Info(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
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


LONG OnIDC_EDIT_Admin_Speed(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
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
LONG OnIDC_EDIT_Admin_SpeedLimit(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
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
