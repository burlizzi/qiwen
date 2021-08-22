// Login_testconfig.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Login_testconfig.h"
#include "Config_test.h"


// CLogin_testconfig �Ի���
 extern BOOL login_entern_flag ;
 extern CConfig_test g_test_config;
 extern BOOL first_flag;

IMPLEMENT_DYNAMIC(CLogin_testconfig, CDialog)

CLogin_testconfig::CLogin_testconfig(CWnd* pParent /*=NULL*/)
	: CDialog(CLogin_testconfig::IDD, pParent)
	, m_login_password(_T(""))
{

}

CLogin_testconfig::~CLogin_testconfig()
{
}

void CLogin_testconfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	if (login_entern_flag)
	{
		login_entern_flag = FALSE;
		Set_confim();
	}

	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_login_password);
	DDX_Text(pDX, IDC_COMBO_USER, m_login_user);
	DDX_Control(pDX, IDC_COMBO_USER, m_combo_username);
}


BEGIN_MESSAGE_MAP(CLogin_testconfig, CDialog)
	ON_BN_CLICKED(ID_LOGIN, &CLogin_testconfig::OnBnClickedLogin)
	ON_EN_CHANGE(IDC_EDIT_USER, &CLogin_testconfig::OnEnChangeEditUser)
	ON_BN_CLICKED(IDC_CHECK_AUTO_LOGIN, &CLogin_testconfig::OnBnClickedCheckAutoLogin)
	ON_WM_CLOSE()
	ON_CBN_SELCHANGE(IDC_COMBO_USER, &CLogin_testconfig::OnCbnSelchangeComboUser)
END_MESSAGE_MAP()


// CLogin_testconfig ��Ϣ�������

void CLogin_testconfig::Set_confim()
{
	SetDlgItemText(IDC_EDIT_USER, g_test_config.ueser_name);
	((CButton *)GetDlgItem(IDC_CHECK_AUTO_LOGIN))->SetCheck(g_test_config.config_auto_login);
	if (g_test_config.config_auto_login)
	{
		SetDlgItemText(IDC_EDIT_PASSWORD, g_test_config.pass_word);
	} 
	else
	{
		SetDlgItemText(IDC_EDIT_PASSWORD, _T(""));
	}
	

}

void CLogin_testconfig::OnBnClickedLogin()
{
#if 0
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	TCHAR buf[MAX_PATH+1] = {0};

	memset(buf, 0, MAX_PATH+1);
	GetDlgItemText(IDC_EDIT_PASSWORD, str);
	
	_tcsncpy(buf, str, MAX_PATH+1);

	if (_tcscmp(buf, g_test_config.pass_word) == 0)
	{
		entern_flag = TRUE;
		m_test_config.DoModal();
	}
	else
	{
		AfxMessageBox(_T("�������������"));
	}
#endif

	CDialog::OnOK();
	
}

void CLogin_testconfig::OnEnChangeEditUser()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}

void CLogin_testconfig::OnBnClickedCheckAutoLogin()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BOOL flag = FALSE;

	flag = ((CButton *)GetDlgItem(IDC_CHECK_AUTO_LOGIN))->GetCheck();
	if (flag)
	{
		g_test_config.config_auto_login = TRUE;
	} 
	else
	{
		g_test_config.config_auto_login = FALSE;
	}
}

void CLogin_testconfig::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	first_flag = FALSE;
	CDialog::OnClose();
}

BOOL CLogin_testconfig::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_combo_username.AddString(_T("������"));
	m_combo_username.AddString(_T("�з���"));
	m_combo_username.SetCurSel(0);//Ĭ��ѡ���һ��
	SetDlgItemText(IDC_COMBO_USER,_T("������"));


	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CLogin_testconfig::OnCbnSelchangeComboUser()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	GetDlgItem(IDC_EDIT_PASSWORD)->SetWindowText(_T(""));
}
