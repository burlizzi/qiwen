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
extern UINT current_ip_idex;

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
	ON_BN_CLICKED(IDC_RADIO_FIND_FILE, &CCBurn_UIdDlg::OnBnClickedRadioFindFile)
	ON_BN_CLICKED(IDC_RADIO_CHAR, &CCBurn_UIdDlg::OnBnClickedRadioChar)
	ON_EN_CHANGE(IDC_EDIT_CHAR_LEN, &CCBurn_UIdDlg::OnEnChangeEditCharLen)
END_MESSAGE_MAP()



void CCBurn_UIdDlg::Set_confim(void)
{
	SetDlgItemText(IDC_EDIT_MAC, g_test_config.m_mac_current_addr);
}

// CCBurn_UIdDlg ��Ϣ�������

void CCBurn_UIdDlg::OnBnClickedOk()
{
	

	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialog::OnOK();
	CString str;
	char* dst=new char[100];
	UINT len_temp = 0;

	UID_first_flag = FALSE;

	USES_CONVERSION;
	if (g_test_config.uid_download_len == 0 || g_test_config.uid_download_len > 100)
	{
		AfxMessageBox(_T("�ַ����ȵ�ֵ�д�,��ΧӦ��1~100"));
		return;
	}

	GetDlgItemText(IDC_EDIT_QR_CODE, str);
	if (str.IsEmpty())
	{
		AfxMessageBox(_T("UIDΪ��,����"));
		return;
	}

	/*CString uid=str.Mid(40,16);*/
	dst=(LPSTR)(LPCTSTR)str;
	len_temp = str.GetLength();
	memset(g_test_config.m_uid_number,0, MAX_PATH);

	if (g_test_config.uid_download_mode == 1)
	{
		if (g_test_config.uid_download_len > str.GetLength())
		{
			UID_PASS_flag = FALSE;
			AfxMessageBox(_T("UID���ȱ����ó��Ȼ�С,���飡"),MB_OK);
			GetDlgItem(IDC_EDIT_QR_CODE)->SetWindowText(_T(""));
			return ;
		}
	}
	else
	{
		if(str.GetLength() != g_test_config.uid_download_len)
		{

			UID_PASS_flag = FALSE;
			AfxMessageBox(_T("UID���Ȳ��������õ��ַ����ȣ�"),MB_OK);
			GetDlgItem(IDC_EDIT_QR_CODE)->SetWindowText(_T(""));
			return ;
		}
		for(int i=0;i<g_test_config.uid_download_len*2;i=i+2)
		{
			if((dst[i]<='Z'&&dst[i]>='A')||(dst[i]>='0'&&dst[i]<='9'))
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
	}
	
	UID_PASS_flag = TRUE;
	_tcscpy(g_test_config.m_uid_number,str);
	SaveUidFile();
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
	UINT j = 0;
	int loop = (g_test_config.uid_download_len+2)*2;
	char read_uid_buf[UID_DATA_LEN] = {0};
	//char serial[16];
	//TCHAR serial_temp[100];
	//char serial_temp[100];
	char* dst=new char[100];
	GetDlgItemText(IDC_EDIT_QR_CODE, str);
	//CString uid=str.Mid(40,16);
	dst=(LPSTR)(LPCTSTR)str;
	//memset(serial_temp, 0, 100);
	
	//memcpy(serial_temp, str, 100)

	/*if(dst[0] == 'T' && dst[2] == 'V' && 
		dst[4] == '6' && dst[6] == '0' &&
		dst[8] == '5' && dst[10] == 'F')
	{
	}
	else
	{
		MessageBox(_T("UID������������ȷ��UID��"),MB_OK);
		return FALSE;
	}*/



	//itoa(GetDlgItemText(IDC_EDIT_QR_CODE, str),dst,10);
	//uid=str.Mid(40,16);
	//dst=(LPSTR)(LPCTSTR)uid;
	CFile fp;
	fp.Open(TEXT("uid_config.txt"),CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite);
#if 1
	len = fp.GetLength();
	char* buff=new char[len + 4];
	memset(buff,0,len + 4);
	fp.Read(buff,len);
	BOOL exit_flag = FALSE;
	UINT idex = 0;
	UINT idex_uid = 0;

	//���бȽϣ����ҵ����з�Ϊ��־
	while(1)
	{
		if (len <= idex)
		{
			break;
		}
		idex_uid = 0;
		while(1)
		{
			//�ҵ�һ��
			if(buff[idex] == '\r' &&  buff[idex + 2] == '\n')
			{
				idex += 4;
				break;
			}
			if (idex_uid <= g_test_config.uid_download_len*2)
			{
				read_uid_buf[idex_uid] = buff[idex];
			}

			idex_uid++;
			idex++;
		}

		if (idex_uid == g_test_config.uid_download_len*2)
		{
			//�Ƚ϶���UID
			for (j = 0; j < g_test_config.uid_download_len*2; j++)
			{
				if (read_uid_buf[j] != dst[j])
				{
					break;
				}
			}
			if (j == g_test_config.uid_download_len*2)
			{
				//UID�Ѵ���
				exit_flag = TRUE;
				break;
			}
		}
	}

	if (buff != NULL)
	{
		delete[] buff;//�ͷ�
	}

	if (exit_flag)
	{
		AfxMessageBox(_T("UID�Ѵ���"),MB_OK);
		fp.Close();
		return FALSE;
	}

	fp.Close();

#else
	len = fp.GetLength();
	/*if(len==0)
	{
	fp.Write(dst,32);
	fp.Close();
	return TRUE;
	}*/
	char* buff=new char[len];
	fp.Read(buff,len);
	for(i=0;i<(len/loop);i++)
	{
#if 1
		for (j = 0; j < g_test_config.uid_download_len; j++)
		{
			if (buff[i*loop + j*2] != dst[j*2])
			{
				break;
			}
		}

		if (j == g_test_config.uid_download_len)
		{
			delete[] buff;//�ͷ�
			AfxMessageBox(_T("UID�Ѵ��ڣ�"),MB_OK);
			fp.Close();
			return FALSE;
		}
#else
		if(buff[i*44]==dst[0] && buff[i*44+2]==dst[2] && buff[i*44+4]==dst[4] && buff[i*44+6]==dst[6] && 
			buff[i*44+8]==dst[8] && buff[i*44+10]==dst[10] && buff[i*44+12]==dst[12] && buff[i*44+14]==dst[14] && 
			buff[i*44+16]==dst[16] && buff[i*44+18]==dst[18] && buff[i*44+20]==dst[20] && buff[i*44+22]==dst[22] && 
			buff[i*44+24]==dst[24] && buff[i*44+26]==dst[26] && buff[i*44+28]==dst[28] && buff[i*44+30]==dst[30] &&
			buff[i*44+32]==dst[32] && buff[i*44+34]==dst[34] && buff[i*44+36]==dst[36] && buff[i*44+38]==dst[38])

		 //if (strncmp(buff+i*32,dst,32) == 0)//��������¼��uid���Ƚ�
		 {
			delete[] buff;//�ͷ�
			AfxMessageBox(_T("UID�Ѵ��ڣ�"),MB_OK);
			fp.Close();
			 return FALSE;
		 }
#endif
		 //fp.Write(serial,16); 
	} 
	/*fp.SeekToEnd();
	fp.Write(dst,32);*/

	fp.Close();
	delete[] buff;//�ͷ�
#endif


	

	return TRUE;
}


BOOL CCBurn_UIdDlg::WriteUid_MAC()
{
	CString str;
	CFile fp;
	TCHAR serial_temp[100] = {0};
	UINT mac_len = 0;
	UINT idex = 0;
	CAnykaIPCameraDlg pP;
	UINT surbuf_len = 0, i = 0, j = 0;
	TCHAR tmpBuf[MAC_ADDRESS_LEN+1] = {0};
	TCHAR tmpBuf_1[2+1] = {0};


	surbuf_len = wcslen(g_test_config.m_last_config[current_ip_idex].Current_IP_MAC);
	for (i = 0; i < surbuf_len; i++)
	{
		if (g_test_config.m_last_config[current_ip_idex].Current_IP_MAC[i] == 58)// :
		{
			continue;
		}

		tmpBuf_1[0] = g_test_config.m_last_config[current_ip_idex].Current_IP_MAC[i]; //ֱ�Ӹ�ֵ
		
		_tcsncpy(&tmpBuf[j], tmpBuf_1, 1);
		j++;
	}

	UINT len = _tcslen(g_test_config.m_uid_number);
	memset(serial_temp,0, 100 );	
	_tcsncpy(&serial_temp[idex], &g_test_config.m_uid_number[len - g_test_config.uid_download_len], g_test_config.uid_download_len);
	idex += g_test_config.uid_download_len;
	_tcsncat(&serial_temp[idex], _T(", "), 2);
	idex += 2;
	mac_len = _tcslen(tmpBuf);
	_tcsncat(&serial_temp[idex], tmpBuf, mac_len);
	idex += mac_len;
	_tcsncat(&serial_temp[idex], _T("\r\n"), 2);
	idex += 2;

	//�ж��ļ��Ƿ�ֻ�������޸�
	DWORD faConfig = GetFileAttributes(pP.ConvertAbsolutePath(_T("uid_mac_config.txt"))); 
	if(0xFFFFFFFF != faConfig)
	{
		faConfig &= ~FILE_ATTRIBUTE_READONLY;//����ļ���ֻ������Ҫ���ֻ��
		faConfig &= ~FILE_ATTRIBUTE_SYSTEM;  //����ļ���ϵͳ����ô���ϵͳ
		faConfig &= ~FILE_ATTRIBUTE_TEMPORARY;//���������ʱ����ôҲҪ�����ʱ
		SetFileAttributes(pP.ConvertAbsolutePath(_T("uid_mac_config.txt")), faConfig);
	}

	fp.Open(_T("uid_mac_config.txt"),CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite);
	fp.SeekToEnd();
	fp.Write(serial_temp,idex*2);	
	fp.Close();

	return TRUE;

}


BOOL CCBurn_UIdDlg::WriteUid()
{
	CString str;
	CFile fp;
	TCHAR serial_temp[100] = {0};


	CAnykaIPCameraDlg pP;

	UINT len = _tcslen(g_test_config.m_uid_number);
	memset(serial_temp,0, 100 );	
	_tcsncpy(serial_temp, &g_test_config.m_uid_number[len - g_test_config.uid_download_len], g_test_config.uid_download_len);
	_tcsncat(&serial_temp[g_test_config.uid_download_len], _T("\r\n"), 2);

	//�ж��ļ��Ƿ�ֻ�������޸�
	DWORD faConfig = GetFileAttributes(pP.ConvertAbsolutePath(_T("uid_config.txt"))); 
	if(0xFFFFFFFF != faConfig)
	{
		faConfig &= ~FILE_ATTRIBUTE_READONLY;//����ļ���ֻ������Ҫ���ֻ��
		faConfig &= ~FILE_ATTRIBUTE_SYSTEM;  //����ļ���ϵͳ����ô���ϵͳ
		faConfig &= ~FILE_ATTRIBUTE_TEMPORARY;//���������ʱ����ôҲҪ�����ʱ
		SetFileAttributes(pP.ConvertAbsolutePath(_T("uid_config.txt")), faConfig);
	}

	fp.Open(_T("uid_config.txt"),CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite);
	fp.SeekToEnd();
	fp.Write(serial_temp,(g_test_config.uid_download_len + 2)*2);	
	fp.Close();

	return TRUE;

}


BOOL CCBurn_UIdDlg::SaveUidFile()
{
	CString str;
	CFile fp;
	TCHAR serial_temp[100] = {0};
	TCHAR uid_path[MAX_PATH*2+1] = {0};

	CAnykaIPCameraDlg pP; // = (CAnykaIPCameraDlg *)GetParent();

	USES_CONVERSION;

	UINT len = _tcslen(g_test_config.m_uid_number);
	memset(serial_temp,0, 100 );	
	_tcsncpy(serial_temp, &g_test_config.m_uid_number[len - g_test_config.uid_download_len], g_test_config.uid_download_len);
	_tcsncat(&serial_temp[g_test_config.uid_download_len], _T("\r\n"), 2);

	swprintf(uid_path, _T("%s/uid.txt"), pP.ConvertAbsolutePath(SERAIL_CONFIG));
	//str.Format(_T("E:\\NetCamera_new\\Anyka IP Camera\\Anyka IP Camera\\Debug\\serial_config\\%s.txt"),serial_temp);
	//�ж��ļ��Ƿ�ֻ�������޸�
	DWORD faConfig = GetFileAttributes(uid_path); 
	if(0xFFFFFFFF != faConfig)
	{
		faConfig &= ~FILE_ATTRIBUTE_READONLY;//����ļ���ֻ������Ҫ���ֻ��
		faConfig &= ~FILE_ATTRIBUTE_SYSTEM;  //����ļ���ϵͳ����ô���ϵͳ
		faConfig &= ~FILE_ATTRIBUTE_TEMPORARY;//���������ʱ����ôҲҪ�����ʱ
		SetFileAttributes(uid_path, faConfig);
	}

	DeleteFile(uid_path);

	if(fp.Open(uid_path,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite))
	{
		//fp.SeekToEnd();
		fp.Write(T2A(serial_temp),(g_test_config.uid_download_len + 2));
		fp.Close();
	}
	

	return TRUE;

}

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
		if (g_test_config.uid_download_mode == 0)
		{
			if(len == g_test_config.uid_download_len)
			{
				if(!SaveUid())
				{
					SetDlgItemText(IDC_EDIT_QR_CODE, _T(""));
				}
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
	if (g_test_config.uid_download_mode == 0)
	{
		if(str.GetLength()!=g_test_config.uid_download_len)
		{
			MessageBox(_T("UID������������ȷ��UID��"),MB_OK);
			return FALSE;
		}
	}
	
	return TRUE;

}




BOOL CCBurn_UIdDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString str;

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_uid_edit.SetFocus();//���������ȱʡѡ��
	m_uid_edit.SetWindowText(_T(""));
	m_uid_edit.SetSel(0,-1);

	return FALSE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CCBurn_UIdDlg::OnBnClickedRadioFindFile()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BOOL flag = FALSE;

	flag = ((CButton *)GetDlgItem(IDC_RADIO_FIND_FILE))->GetCheck();
	if (flag)
	{
		g_test_config.uid_download_mode = 1;
	} 
	else
	{
		g_test_config.uid_download_mode = 0;
	}
}

void CCBurn_UIdDlg::OnBnClickedRadioChar()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BOOL flag = FALSE;

	flag = ((CButton *)GetDlgItem(IDC_RADIO_CHAR))->GetCheck();
	if (flag)
	{
		g_test_config.uid_download_mode = 0;
	} 
	else
	{
		g_test_config.uid_download_mode = 1;
	}
}

void CCBurn_UIdDlg::OnEnChangeEditCharLen()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}
