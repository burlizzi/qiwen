// CBurn_UIdDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CBurn_UIdDlg.h"
#include "Config_test.h"
#include "LogUidFile.h"
#include "Anyka IP CameraDlg.h"




CLogUidFile m_frmLogUidFile;

// CCBurn_UIdDlg �Ի���

extern CConfig_test g_test_config;

IMPLEMENT_DYNAMIC(CCBurn_UIdDlg, CDialog)

CCBurn_UIdDlg::CCBurn_UIdDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCBurn_UIdDlg::IDD, pParent)
{

}

CCBurn_UIdDlg::~CCBurn_UIdDlg()
{
}

void CCBurn_UIdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	Set_confim();
	DDX_Control(pDX, IDC_EDIT_QR_CODE, m_uid_edit);
	DDX_Text(pDX, IDC_EDIT_QR_CODE, m_str_uid);
}


BEGIN_MESSAGE_MAP(CCBurn_UIdDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CCBurn_UIdDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CCBurn_UIdDlg::OnBnClickedCancel)
	ON_EN_CHANGE(IDC_EDIT_QR_CODE, &CCBurn_UIdDlg::OnEnChangeEditQrCode)
	ON_EN_KILLFOCUS(IDC_EDIT_QR_CODE, &CCBurn_UIdDlg::OnEnKillfocusEditQrCode)
END_MESSAGE_MAP()



void CCBurn_UIdDlg::Set_confim(void)
{
	SetDlgItemText(IDC_EDIT_MAC, g_test_config.m_mac_current_addr);
}

// CCBurn_UIdDlg ��Ϣ�������

void CCBurn_UIdDlg::OnBnClickedOk()
{
	

	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//CDialog::OnOK();
	CString str;
	char* dst=new char[100];
	UID_first_flag = FALSE;
	GetDlgItemText(IDC_EDIT_QR_CODE, str);
	CString uid=str.Mid(40,16);
	dst=(LPSTR)(LPCTSTR)uid;
	if(str.GetLength()!=56)
	{
		UID_PASS_flag = FALSE;
		AfxMessageBox(_T("UID������������ȷ��UID��"),MB_OK);
		GetDlgItem(IDC_EDIT_QR_CODE)->SetWindowText(_T(""));
		return ;
	}
	for(int i=0;i<20;i=i+2)
	{
		if((dst[12+i]<='F'&&dst[12+i]>='A')||(dst[12+i]>='0'&&dst[12+i]<='9'))
		{
		}
		else
		{
			UID_PASS_flag = FALSE;
			AfxMessageBox(_T("UID�Ƿ�����������ȷ��UID��"),MB_OK);
			GetDlgItem(IDC_EDIT_QR_CODE)->SetWindowText(_T(""));
			return ;
		}
	}
	
	if(!SaveUid())
	{
		return;
	}

	CDialog::OnOK();
	UID_PASS_flag = TRUE;
	_tcscpy(g_test_config.m_uid_number,str);
	//SaveUid();
}

void CCBurn_UIdDlg::OnBnClickedCancel()
{
	UID_first_flag = FALSE;
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialog::OnCancel();
}


BOOL CCBurn_UIdDlg::SaveUid()
{
	CString str;
	int i = 0, len = 0;
	//char serial[16];
	//TCHAR serial_temp[100];
	//char serial_temp[100];
	char* dst=new char[100];
	GetDlgItemText(IDC_EDIT_QR_CODE, str);
	CString uid=str.Mid(40,16);
	dst=(LPSTR)(LPCTSTR)uid;
	//memset(serial_temp, 0, 100);
	
	//memcpy(serial_temp, str, 100)

	if(dst[0] == 'T' && dst[2] == 'V' && 
		dst[4] == '6' && dst[6] == '0' &&
		dst[8] == '5' && dst[10] == 'F')
	{
	}
	else
	{
		Error_Dlg.flag = 0;
		Error_Dlg.DoModal();
		//AfxMessageBox(_T("UID������������ȷ��UID��"),MB_OK);
		return FALSE;
	}



	//itoa(GetDlgItemText(IDC_EDIT_QR_CODE, str),dst,10);
	//uid=str.Mid(40,16);
	//dst=(LPSTR)(LPCTSTR)uid;
	CFile fp;
	fp.Open(TEXT("uid_config.txt"),CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite);
	len = fp.GetLength();
	/*if(len==0)
	{
		fp.Write(dst,32);
		fp.Close();
		return TRUE;
	}*/
	char* buff=new char[len];
	fp.Read(buff,len);
	for(i=0;i<len/36;i++)
	{
		if(buff[i*36]==dst[0] && buff[i*36+2]==dst[2] && buff[i*36+4]==dst[4] && buff[i*36+6]==dst[6] && 
			buff[i*36+8]==dst[8] && buff[i*36+10]==dst[10] && buff[i*36+12]==dst[12] && buff[i*36+14]==dst[14] && 
			buff[i*36+16]==dst[16] && buff[i*36+18]==dst[18] && buff[i*36+20]==dst[20] && buff[i*36+22]==dst[22] && 
			buff[i*36+24]==dst[24] && buff[i*36+26]==dst[26] && buff[i*36+28]==dst[28] && buff[i*36+30]==dst[30])
		 {
			 //AfxMessageBox(_T("UID�Ѵ��ڣ�"),MB_OK);
			 Error_Dlg.flag = 1;
			 Error_Dlg.DoModal();
			 fp.Close();
			 return FALSE;
		 }
		 //fp.Write(serial,16); 
	} 
	/*fp.SeekToEnd();
	fp.Write(dst,32);*/

	fp.Close();

	return TRUE;
}

BOOL CCBurn_UIdDlg::WriteUid()
{
	CString str;
	CFile fp;
	TCHAR serial_temp[100] = {0};

	UINT len = _tcslen(g_test_config.m_uid_number);
	memset(serial_temp,0, 100 );	
	_tcsncpy(serial_temp, &g_test_config.m_uid_number[len - 16], 16);
	_tcsncat(&serial_temp[16], _T("\r\n"), 2);

	fp.Open(TEXT("uid_config.txt"),CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite);
	fp.SeekToEnd();
	fp.Write(serial_temp,36);	
	fp.Close();

	return TRUE;

}


#if 0

//��ȡ�����ļ��Ĳ���
BOOL CCBurn_UIdDlg::SaveUid()
{
	CString str;
	int i = 0, len = 0;
	TCHAR uid[16];
	DWORD read_len = 1;
	BOOL ret = TRUE;
	CAnykaIPCameraDlg AnykaIPCameraDlg;

	USES_CONVERSION;

	GetDlgItemText(IDC_EDIT_QR_CODE, str);
	if(str.IsEmpty())
	{
		MessageBox(_T("UIDΪ�գ���������ȷ��UID��"), MB_OK);
		return FALSE;
	}

	if(str.GetLength()!=56)
	{
		MessageBox(_T("UID���Ȳ��ԣ���������ȷ��UID��"), MB_OK);
		return FALSE;
	}

	memset(g_test_config.m_uid_number, 0, MAX_PATH);
	_tcscpy(g_test_config.m_uid_number,  str);
	memset(uid, 0, 16);
	_tcsncpy(uid,  &g_test_config.m_uid_number[40], 16);
	
	if(uid[0] == 'T' && uid[1] == 'V' && 
		uid[2] == '6' && uid[3] == '0' &&
		uid[4] == '5' && uid[5] == 'F')
	{
	}
	else
	{
		MessageBox(_T("UID������������ȷ��UID��"), MB_OK);
		return FALSE;
	}

	DWORD faConfig = GetFileAttributes(UID_CONFIG_FILE); 
	if(0xFFFFFFFF != faConfig)
	{
		faConfig &= ~FILE_ATTRIBUTE_READONLY;//����ļ���ֻ������Ҫ���ֻ��
		faConfig &= ~FILE_ATTRIBUTE_SYSTEM;  //����ļ���ϵͳ����ô���ϵͳ
		faConfig &= ~FILE_ATTRIBUTE_TEMPORARY;//���������ʱ����ôҲҪ�����ʱ
		SetFileAttributes(UID_CONFIG_FILE, faConfig);
	}
	else
	{
		return true;
	}

	//�������ļ�
	HANDLE hFile = CreateFile(UID_CONFIG_FILE, GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, 0, NULL);
	if(INVALID_HANDLE_VALUE == hFile)
	{
		return FALSE;
	}

	//����һ��һ�ж�ȡ����
	while(read_len > 0)
	{
		CString subLeft, subRight;
		TCHAR ch = 0;
		TCHAR text[100];
		int index = 0;
		UINT will_len = sizeof(TCHAR);

		while(read_len > 0 && ch != '\n')
		{
			ret = ReadFile(hFile, &ch, will_len, &read_len, NULL);
			text[index++] = ch;
		}
		text[index] = 0;

		str = text;
		int len = str.GetLength();

		//discard the lines that is blank or begin with '#'
		str.TrimLeft();
		if(str.IsEmpty() || '#' == str[0])
		{
			continue;
		}

		for(i = 0; i < 16; i++)
		{
			if(uid[i] != text[i])
			{
				break;
			}
		}

		if(i == 16)//_tcsncpy(uid,  str, 16) == 0)
		{
			MessageBox(_T("UID�Ѵ���"),MB_OK);
			CloseHandle(hFile);
			return FALSE;
		}
	}

	CloseHandle(hFile);

	return TRUE;
}


//���������ļ���
//������ɣ����б���
//�ڹر���¼����ʱ�����б���
BOOL CCBurn_UIdDlg::WriteUid()
{
	CString str;
	INT len = 0;
	char buf[12] = {0};
	TCHAR serial_temp[100] = {0};
	//open config file
	CStdioFile *pFile;

	USES_CONVERSION;

	//��ȡ����
	DWORD faConfig = GetFileAttributes(UID_CONFIG_FILE); 
	if(0xFFFFFFFF != faConfig)
	{
		faConfig &= ~FILE_ATTRIBUTE_READONLY;//����ļ���ֻ������Ҫ���ֻ��
		faConfig &= ~FILE_ATTRIBUTE_SYSTEM;  //����ļ���ϵͳ����ô���ϵͳ
		faConfig &= ~FILE_ATTRIBUTE_TEMPORARY;//���������ʱ����ôҲҪ�����ʱ
		SetFileAttributes(UID_CONFIG_FILE, faConfig);

		
		pFile = new CStdioFile(UID_CONFIG_FILE, CFile::modeWrite | CFile::typeBinary | CFile::shareDenyNone);
		if(NULL == pFile)
		{
			return FALSE;
		}
	}
	else
	{
		pFile = new CStdioFile(UID_CONFIG_FILE, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary | CFile::shareDenyNone);
		if(NULL == pFile)
		{
			return FALSE;
		}
	}

	
	
	SetFilePointer(pFile, 0, NULL, FILE_END);//�����ļ�λ��

	len = _tcslen(g_test_config.m_uid_number);
	memset(serial_temp,0, 100 );	
	_tcsncpy(serial_temp, &g_test_config.m_uid_number[len - 16], 16);

	str.Format(_T("%s\r\n"), serial_temp);
	pFile->WriteString(str);

	pFile->Close();
	delete pFile;

	return TRUE;
}

#endif

void CCBurn_UIdDlg::OnEnChangeEditQrCode()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	CString str;

	GetDlgItemText(IDC_EDIT_QR_CODE, str);
	if(!str.IsEmpty())
	{
		UINT len = str.GetLength();
		if(len == 56)
		{
			if(!SaveUid())
			{
				SetDlgItemText(IDC_EDIT_QR_CODE, _T(""));
			}
		}
	}

}

void CCBurn_UIdDlg::OnEnKillfocusEditQrCode()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

}

BOOL CCBurn_UIdDlg::OnCheck()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	GetDlgItemText(IDC_EDIT_QR_CODE, str);
	if(str.GetLength()!=56)
	{
		MessageBox(_T("UID������������ȷ��UID��"),MB_OK);
		return FALSE;
	}
	return TRUE;

}




BOOL CCBurn_UIdDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_uid_edit.SetFocus();//���������ȱʡѡ��
	m_uid_edit.SetSel(0,-1);//�������������ȫ��ѡ��

	return FALSE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
