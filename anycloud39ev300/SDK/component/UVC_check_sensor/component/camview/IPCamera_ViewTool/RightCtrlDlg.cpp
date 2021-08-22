// RightCtrlDlg.cpp : ʵ���ļ�

#include "stdafx.h"
#include "Anyka IP Camera.h"
#include "Anyka IP CameraDlg.h"
#include "RightCtrlDlg.h"
#include "Config_test.h"


// CRightCtrlDlg �Ի���

extern BOOL no_put_flie_flag ;
extern BOOL g_test_finish_flag ;
extern CConfig_test g_test_config;
extern BOOL g_pre_flag;
extern BOOL g_test_monitor_flag;
extern BOOL g_update_all_flag;
extern BOOL g_finish_flag;
extern BOOL one_update_finish ;
extern CFile g_file_fp;
extern UINT g_max_frame_idex;
extern BOOL g_stop_play_flag ;
extern BOOL g_start_play_flag;
extern CFile g_play_fp;
BOOL g_need_re_open_flag = TRUE;
extern LONGLONG g_current_Off;
extern BOOL g_play_first_frame_flag;
extern char g_update_flag[UPDATE_MAX_NUM];
extern BOOL g_start_recode_flag;



IMPLEMENT_DYNAMIC(CRightCtrlDlg, CDialog)

CRightCtrlDlg::CRightCtrlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRightCtrlDlg::IDD, pParent)
{

}

CRightCtrlDlg::~CRightCtrlDlg()
{
}

void CRightCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRightCtrlDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CONNET, &CRightCtrlDlg::OnBnClickedButtonConnet)
	ON_BN_CLICKED(IDC_BUTTON_SET, &CRightCtrlDlg::OnBnClickedButtonSet)
	ON_BN_CLICKED(IDC_RADIO_VGA, &CRightCtrlDlg::OnBnClickedRadioVga)
	ON_BN_CLICKED(IDC_RADIO_720P, &CRightCtrlDlg::OnBnClickedRadio720p)
	ON_BN_CLICKED(IDC_RADIO_960P, &CRightCtrlDlg::OnBnClickedRadio960p)
	ON_BN_CLICKED(IDC_RADIO_CLINTE, &CRightCtrlDlg::OnBnClickedRadioClinte)
	ON_BN_CLICKED(IDC_RADIO_SERVER, &CRightCtrlDlg::OnBnClickedRadioServer)
	ON_BN_CLICKED(IDC_CHECK_SAVE_VIDEO, &CRightCtrlDlg::OnBnClickedCheckSaveVideo)
	ON_BN_CLICKED(IDC_BUTTON_VIDEO_PATH, &CRightCtrlDlg::OnBnClickedButtonVideoPath)
	ON_EN_CHANGE(IDC_EDIT_SAVE_VIDEO, &CRightCtrlDlg::OnEnChangeEditSaveVideo)
	ON_BN_CLICKED(IDC_BUTTON_PLAY_VIDEO, &CRightCtrlDlg::OnBnClickedButtonPlayVideo)
	ON_EN_CHANGE(IDC_EDIT_FRAME, &CRightCtrlDlg::OnEnChangeEditFrame)
	ON_EN_CHANGE(IDC_EDIT_VIDEO_FRAME, &CRightCtrlDlg::OnEnChangeEditVideoFrame)
	ON_BN_CLICKED(IDC_CHECK_SET_FRAME_PLAY, &CRightCtrlDlg::OnBnClickedCheckSetFramePlay)
	ON_EN_CHANGE(IDC_EDIT_PLAY_FRAME, &CRightCtrlDlg::OnEnChangeEditPlayFrame)
	ON_BN_CLICKED(IDC_BUTTON1, &CRightCtrlDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_PLAY_FIRST_FRAME, &CRightCtrlDlg::OnBnClickedButtonPlayFirstFrame)
	ON_CBN_SELCHANGE(IDC_COMBO_SAMPLES, &CRightCtrlDlg::OnCbnSelchangeComboSamples)
	ON_CBN_EDITCHANGE(IDC_COMBO_SAMPLES, &CRightCtrlDlg::OnCbnEditchangeComboSamples)
	ON_CBN_EDITUPDATE(IDC_COMBO_SAMPLES, &CRightCtrlDlg::OnCbnEditupdateComboSamples)
	ON_CBN_SELENDCANCEL(IDC_COMBO_SAMPLES, &CRightCtrlDlg::OnCbnSelendcancelComboSamples)
	ON_BN_CLICKED(IDC_BUTTON_RECODE, &CRightCtrlDlg::OnBnClickedButtonRecode)
END_MESSAGE_MAP()


// CRightCtrlDlg ��Ϣ�������


BOOL CRightCtrlDlg::OnInitDialog()
{

	CString str;
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	m_save_video_flag = TRUE;
	m_start_stop_flag = TRUE;
	SetDlgItemText(IDC_BUTTON_RECODE, _T("�����Խ�"));

	m_Status.SubclassDlgItem(IDC_STATIC_TEST_CONTENT,this);

	if (g_test_config.server_flag)
	{
		((CButton *)GetDlgItem(IDC_RADIO_SERVER))->SetCheck(0);
		((CButton *)GetDlgItem(IDC_RADIO_CLINTE))->SetCheck(1);
		((CButton *)GetDlgItem(IDC_IPADDRESS_IP))->ShowWindow(TRUE);
		((CButton *)GetDlgItem(IDC_STATIC_IP))->ShowWindow(TRUE);
		((CButton *)GetDlgItem(IDC_IPADDRESS_IP))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_STATIC_IP))->EnableWindow(TRUE);
		
	}
	else
	{
		((CButton *)GetDlgItem(IDC_RADIO_SERVER))->SetCheck(1);
		((CButton *)GetDlgItem(IDC_RADIO_CLINTE))->SetCheck(0);
		((CButton *)GetDlgItem(IDC_IPADDRESS_IP))->ShowWindow(FALSE);
		((CButton *)GetDlgItem(IDC_STATIC_IP))->ShowWindow(FALSE);
		((CButton *)GetDlgItem(IDC_IPADDRESS_IP))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_STATIC_IP))->EnableWindow(FALSE);
		
	}

	if (g_test_config.save_video_enable)
	{
		((CButton *)GetDlgItem(IDC_CHECK_SAVE_VIDEO))->SetCheck(1);
	} 
	else
	{
		((CButton *)GetDlgItem(IDC_CHECK_SAVE_VIDEO))->SetCheck(0);
	}


	((CButton *)GetDlgItem(IDC_CHECK_SET_FRAME_PLAY))->SetCheck(g_test_config.set_play_frame_flag);
	if (g_test_config.set_play_frame_flag)
	{
		GetDlgItem(IDC_EDIT_PLAY_FRAME)->EnableWindow(TRUE);
		str.Format(_T("%d"), g_test_config.play_frame_num);
		SetDlgItemText(IDC_EDIT_PLAY_FRAME, str);
	}
	else
	{
		GetDlgItem(IDC_EDIT_PLAY_FRAME)->EnableWindow(FALSE);
	}

	m_play_video_flag = TRUE;

	SetDlgItemText(IDC_EDIT_SAVE_VIDEO, g_test_config.m_video_path);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

HBRUSH CRightCtrlDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
#if 0
	switch(pWnd->GetDlgCtrlID())
	{
		case IDC_STATIC_TEST_TITLE:
		case IDC_STATIC_TEST_CONTENT:
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(0,255,0));
			
	
			return (HBRUSH)GetStockObject(HOLLOW_BRUSH);
		
	}
#endif
	// TODO:  �ڴ˸��� DC ���κ�����

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}

void CRightCtrlDlg::OnStnClickedStaticTestContent()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void CRightCtrlDlg::OnNMSetfocusList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}

void CRightCtrlDlg::OnBnClickedButtonConnet()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CAnykaIPCameraDlg * pP = (CAnykaIPCameraDlg *)GetParent();
	((CButton *)GetDlgItem(IDC_BUTTON_CONNET))->EnableWindow(FALSE);
	//��������
	
	if (m_start_video_flag)
	{

		pP->close_play_video_thread();
		pP->close_play_first_frame_thread();

		if (g_test_config.save_video_enable)
		{
			//���������ļ����߳�
			if (!pP->start_save_video())
			{
				return;
			}
		}

		if (!pP->connet_video_net())
		{
			((CButton *)GetDlgItem(IDC_BUTTON_CONNET))->EnableWindow(TRUE);
			return;
		}
		m_start_video_flag = FALSE;
		
		SetDlgItemText(IDC_BUTTON_CONNET, _T("ֹͣ��Ƶ"));
	}
	else
	{
		if (g_test_config.save_video_enable)
		{
			pP->stop_save_video();
		}
		pP->Close_video_net(0);
		m_start_video_flag = TRUE;
		SetDlgItemText(IDC_BUTTON_CONNET, _T("������Ƶ"));
	}
	

	((CButton *)GetDlgItem(IDC_BUTTON_CONNET))->EnableWindow(TRUE);
}


void CRightCtrlDlg::OnBnClickedButtonSet()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CAnykaIPCameraDlg * pP = (CAnykaIPCameraDlg *)GetParent();

	//��������
	((CButton *)GetDlgItem(IDC_BUTTON_SET))->EnableWindow(FALSE);
	pP->set_video_param();
	((CButton *)GetDlgItem(IDC_BUTTON_SET))->EnableWindow(TRUE);
}

void CRightCtrlDlg::OnBnClickedRadioVga()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	((CButton *)GetDlgItem(IDC_RADIO_VGA))->SetCheck(TRUE);
	((CButton *)GetDlgItem(IDC_RADIO_720P))->SetCheck(FALSE);
	((CButton *)GetDlgItem(IDC_RADIO_960P))->SetCheck(FALSE);
	g_test_config.video_parm.video_size = TYPE_VIDEO_VGA;
}

void CRightCtrlDlg::OnBnClickedRadio720p()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	((CButton *)GetDlgItem(IDC_RADIO_VGA))->SetCheck(FALSE);
	((CButton *)GetDlgItem(IDC_RADIO_720P))->SetCheck(TRUE);
	((CButton *)GetDlgItem(IDC_RADIO_960P))->SetCheck(FALSE);
	g_test_config.video_parm.video_size = TYPE_VIDEO_720P;
}

void CRightCtrlDlg::OnBnClickedRadio960p()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	((CButton *)GetDlgItem(IDC_RADIO_VGA))->SetCheck(FALSE);
	((CButton *)GetDlgItem(IDC_RADIO_720P))->SetCheck(FALSE);
	((CButton *)GetDlgItem(IDC_RADIO_960P))->SetCheck(TRUE);
	g_test_config.video_parm.video_size = TYPE_VIDEO_960P;
}

void CRightCtrlDlg::OnBnClickedRadioClinte()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CAnykaIPCameraDlg * pP = (CAnykaIPCameraDlg *)GetParent();
	BOOL check = ((CButton *)GetDlgItem(IDC_RADIO_CLINTE))->GetCheck();
	if (check)
	{
		((CButton *)GetDlgItem(IDC_RADIO_SERVER))->SetCheck(0);
		((CButton *)GetDlgItem(IDC_RADIO_CLINTE))->SetCheck(1);
		((CButton *)GetDlgItem(IDC_IPADDRESS_IP))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_STATIC_IP))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_IPADDRESS_IP))->ShowWindow(TRUE);
		((CButton *)GetDlgItem(IDC_STATIC_IP))->ShowWindow(TRUE);
		g_test_config.server_flag = 1;	
		pP->close_server_video_thread();

	}

}

void CRightCtrlDlg::OnBnClickedRadioServer()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CAnykaIPCameraDlg * pP = (CAnykaIPCameraDlg *)GetParent();
	BOOL check = ((CButton *)GetDlgItem(IDC_RADIO_SERVER))->GetCheck();
	if (check)
	{
		((CButton *)GetDlgItem(IDC_RADIO_SERVER))->SetCheck(1);
		((CButton *)GetDlgItem(IDC_RADIO_CLINTE))->SetCheck(0);

		((CButton *)GetDlgItem(IDC_IPADDRESS_IP))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_STATIC_IP))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_IPADDRESS_IP))->ShowWindow(FALSE);
		((CButton *)GetDlgItem(IDC_STATIC_IP))->ShowWindow(FALSE);
		g_test_config.server_flag = 0;

		//��������˵�socket
		pP->create_server_video_thread();
		
	}
}

void CRightCtrlDlg::OnBnClickedCheckSaveVideo()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	CAnykaIPCameraDlg * pP = (CAnykaIPCameraDlg *)GetParent();
	
	

	BOOL check = ((CButton *)GetDlgItem(IDC_CHECK_SAVE_VIDEO))->GetCheck();
	if (check)
	{
		GetDlgItemText(IDC_EDIT_SAVE_VIDEO, str);
		if (str.IsEmpty())
		{
			((CButton *)GetDlgItem(IDC_CHECK_SAVE_VIDEO))->SetCheck(0);
			g_test_config.save_video_enable = 0;
			AfxMessageBox(_T("����ѡ��һ��������Ƶ��·��"), MB_OK);
			return;
		}
		memset(g_test_config.m_video_path, 0, MAX_PATH + 1);
		_tcscpy(g_test_config.m_video_path, str);

		g_test_config.save_video_enable = 1;
	}
	else
	{
		g_test_config.save_video_enable = 0;
	}

}

void CRightCtrlDlg::OnBnClickedButtonVideoPath()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strPath;
	//��ʼ������Ƶ
	//��ȡ�ļ������·��
	if (!m_play_video_flag)
	{
		AfxMessageBox(_T("��Ƶ�������У�����ͣ"), MB_OK);
		return;
	}
	CAnykaIPCameraDlg * pP = (CAnykaIPCameraDlg *)GetParent();
	TCHAR szFilter[] =	TEXT("BIN Files(*.dat)|*.dat|") \
		TEXT("All Files (*.*)|*.*||") ;

	CFileDialog fd(FALSE, _T(".dat"), NULL, OFN_HIDEREADONLY//|OFN_OVERWRITEPROMPT
		, szFilter, NULL);


	if(IDOK == fd.DoModal())
	{
		//��ȡ SPI ���� �����·��(���ļ���)
		strPath = fd.GetPathName();
		memset(g_test_config.m_video_path, 0, (MAX_PATH + 1)*sizeof(TCHAR));
		_tcscpy(g_test_config.m_video_path, strPath);
		SetDlgItemText(IDC_EDIT_SAVE_VIDEO, strPath);
		if (m_play_video_flag)
		{
			g_need_re_open_flag = TRUE;
			pP->close_play_video_thread();
			SetDlgItemText(IDC_BUTTON_PLAY_VIDEO, _T("�ط���Ƶ"));
		}
		
	}
}

void CRightCtrlDlg::OnEnChangeEditSaveVideo()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString str;
	TCHAR path[MAX_PATH+1] = {0};

	GetDlgItemText(IDC_EDIT_SAVE_VIDEO, str);
	if (str.IsEmpty())
	{
		return;
	}
	memset(path, 0, (MAX_PATH + 1)*sizeof(TCHAR));
	_tcscpy(path, str);


	if (_tcscmp(g_test_config.m_video_path, path) != 0)
	{
		g_need_re_open_flag = TRUE;
		memset(g_test_config.m_video_path, 0, (MAX_PATH + 1)*sizeof(TCHAR));
		_tcscpy(g_test_config.m_video_path, str);
	}
	
}

void CRightCtrlDlg::OnBnClickedButtonPlayVideo()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CAnykaIPCameraDlg * pP = (CAnykaIPCameraDlg *)GetParent();
	
	g_play_first_frame_flag = FALSE;
	pP->close_play_first_frame_thread();	
	Sleep(200);
	if (m_play_video_flag)
	{
		

		//
		if (g_need_re_open_flag)
		{
			if(!pP->play_video_file())
			{
				return;
			}
			g_need_re_open_flag = FALSE;
		}
		m_play_video_flag = FALSE;
		g_start_play_flag =TRUE;
		g_stop_play_flag = FALSE;;
		SetDlgItemText(IDC_BUTTON_PLAY_VIDEO, _T("��ͣ�ط�"));
		GetDlgItem(IDC_CHECK_SET_FRAME_PLAY)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PLAY_FRAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_PLAY_FIRST_FRAME)->EnableWindow(FALSE);
		
		
	}
	else
	{
		//pP->close_play_video_thread();
		m_play_video_flag = TRUE;
		g_stop_play_flag = TRUE;
		g_start_play_flag = FALSE;
		SetDlgItemText(IDC_BUTTON_PLAY_VIDEO, _T("�ط���Ƶ"));
		GetDlgItem(IDC_CHECK_SET_FRAME_PLAY)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PLAY_FRAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_PLAY_FIRST_FRAME)->EnableWindow(TRUE);
	}
	
}

void CRightCtrlDlg::OnEnChangeEditFrame()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString str;
	UINT frame = 0;
	USES_CONVERSION;
	GetDlgItemText(IDC_EDIT_FRAME, str);
	if (str.IsEmpty())
	{
		return;
	}
	
	frame = atoi(T2A(str));
	if (frame < 1 || frame > 30)
	{
		AfxMessageBox(_T("֡�����ò�����Ч��Χ��"), MB_OK);
		return;
	}
	g_test_config.video_parm.frame = frame;

}

void CRightCtrlDlg::OnEnChangeEditVideoFrame()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString str;
	UINT data_rate = 0;
	USES_CONVERSION;
	GetDlgItemText(IDC_EDIT_VIDEO_FRAME, str);
	if (str.IsEmpty())
	{
		return;
	}

	data_rate = atoi(T2A(str));
	if (data_rate < 1 || data_rate > 65536)
	{
		AfxMessageBox(_T("�������ò�����Ч��Χ��"), MB_OK);
		return;
	}
	g_test_config.video_parm.data_rate = data_rate;
}

void CRightCtrlDlg::OnBnClickedCheckSetFramePlay()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;

	BOOL check = ((CButton *)GetDlgItem(IDC_CHECK_SET_FRAME_PLAY))->GetCheck();
	if (check)
	{
		g_test_config.set_play_frame_flag = TRUE;
		GetDlgItem(IDC_EDIT_PLAY_FRAME)->EnableWindow(TRUE);
		str.Format(_T("%d"), g_test_config.play_frame_num);
		SetDlgItemText(IDC_EDIT_PLAY_FRAME, str);
	}
	else
	{
		g_test_config.set_play_frame_flag = FALSE;
		GetDlgItem(IDC_EDIT_PLAY_FRAME)->EnableWindow(FALSE);
	}
	
}

void CRightCtrlDlg::OnEnChangeEditPlayFrame()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString str;
	UINT data_rate = 0;
	USES_CONVERSION;
	GetDlgItemText(IDC_EDIT_PLAY_FRAME, str);
	if (str.IsEmpty())
	{
		return;
	}

	data_rate = atoi(T2A(str));
	if (data_rate < 1 || data_rate > 30)
	{
		AfxMessageBox(_T("�������ò�����Ч��Χ��(1~30)"), MB_OK);
		SetDlgItemText(IDC_EDIT_PLAY_FRAME, _T(""));
		return;
	}
	g_test_config.play_frame_num = data_rate;
}

void CRightCtrlDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void CRightCtrlDlg::OnBnClickedButtonPlayFirstFrame()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CAnykaIPCameraDlg * pP = (CAnykaIPCameraDlg *)GetParent();
	
	g_play_first_frame_flag = TRUE;
	pP->close_play_video_thread();
	pP->close_play_first_frame_thread();
	pP->close_play_file();
	Sleep(100);

	if(!pP->play_video_file())
	{
		return;
	}
	g_need_re_open_flag = TRUE;
	g_stop_play_flag = FALSE;

}

void CRightCtrlDlg::OnCbnSelchangeComboSamples()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	UINT data_rate = 0;
	USES_CONVERSION;
	GetDlgItemText(IDC_COMBO_SAMPLES, str);
	if (str.IsEmpty())
	{
		return;
	}

	data_rate = atoi(T2A(str));
	if (data_rate != 8000 && data_rate != 16000)
	{
		AfxMessageBox(_T("������ֻ֧��8000��16000Hz"), MB_OK);
		SetDlgItemText(IDC_COMBO_SAMPLES, _T("8000"));
		return;
	}
	g_test_config.SamplesPerSec = data_rate;

}

void CRightCtrlDlg::OnCbnEditchangeComboSamples()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
}

void CRightCtrlDlg::OnCbnEditupdateComboSamples()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

}

void CRightCtrlDlg::OnCbnSelendcancelComboSamples()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	UINT data_rate = 0;
	USES_CONVERSION;
	GetDlgItemText(IDC_COMBO_SAMPLES, str);
	if (str.IsEmpty())
	{
		return;
	}

	data_rate = atoi(T2A(str));
	if (data_rate != 8000 && data_rate != 16000)
	{
		AfxMessageBox(_T("������ֻ֧��8000��16000Hz"), MB_OK);
		SetDlgItemText(IDC_COMBO_SAMPLES, _T("8000"));
		return;
	}
	g_test_config.SamplesPerSec = data_rate;
}

void CRightCtrlDlg::OnBnClickedButtonRecode()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CAnykaIPCameraDlg * pP = (CAnykaIPCameraDlg *)GetParent();
	((CButton *)GetDlgItem(IDC_BUTTON_RECODE))->EnableWindow(FALSE);
	//��������

	if (m_start_stop_flag)
	{

		m_start_stop_flag = FALSE;
		g_start_recode_flag = TRUE;

		SetDlgItemText(IDC_BUTTON_RECODE, _T("ֹͣ�Խ�"));
	}
	else
	{
		m_start_stop_flag = TRUE;
		g_start_recode_flag = FALSE;
		SetDlgItemText(IDC_BUTTON_RECODE, _T("�����Խ�"));
	}

	((CButton *)GetDlgItem(IDC_BUTTON_RECODE))->EnableWindow(TRUE);
}
