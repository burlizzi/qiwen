// printf_set.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "printf_set.h"
#include "Config_test.h"
#include "WinSpool.h"


extern CConfig_test g_test_config;



// Cprintf_set �Ի���

IMPLEMENT_DYNAMIC(Cprintf_set, CDialog)

Cprintf_set::Cprintf_set(CWnd* pParent /*=NULL*/)
	: CDialog(Cprintf_set::IDD, pParent)
	, m_printf_set(0)
{

}

Cprintf_set::~Cprintf_set()
{
}

void Cprintf_set::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Cprintf_set, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SET_PRINTF, &Cprintf_set::OnBnClickedButtonSetPrintf)
	ON_BN_CLICKED(IDOK, &Cprintf_set::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO_1, &Cprintf_set::OnCbnSelchangeCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO_2, &Cprintf_set::OnCbnSelchangeCombo2)
	ON_CBN_SELCHANGE(IDC_COMBO_3, &Cprintf_set::OnCbnSelchangeCombo3)
	ON_CBN_SELCHANGE(IDC_COMBO_4, &Cprintf_set::OnCbnSelchangeCombo4)
	ON_CBN_SELCHANGE(IDC_COMBO_5, &Cprintf_set::OnCbnSelchangeCombo5)
	ON_CBN_SELCHANGE(IDC_COMBO_NUM, &Cprintf_set::OnCbnSelchangeComboNum)
	ON_BN_CLICKED(IDC_RADIO_PRINTF_LAB, &Cprintf_set::OnBnClickedRadioPrintfLab)
	ON_BN_CLICKED(IDC_RADIO_PRINTF_IMG, &Cprintf_set::OnBnClickedRadioPrintfImg)
	ON_EN_CHANGE(IDC_EDIT_WITH, &Cprintf_set::OnEnChangeEditWith)
	ON_EN_CHANGE(IDC_EDIT_HEITH, &Cprintf_set::OnEnChangeEditHeith)
	ON_EN_CHANGE(IDC_EDIT_WITH_OFFSET, &Cprintf_set::OnEnChangeEditWithOffset)
	ON_EN_CHANGE(IDC_EDIT_HEITH_OFFSET, &Cprintf_set::OnEnChangeEditHeithOffset)
	ON_EN_CHANGE(IDC_EDIT_PID, &Cprintf_set::OnEnChangeEditPid)
	ON_EN_CHANGE(IDC_EDIT_CHAR_HEIGHT, &Cprintf_set::OnEnChangeEditCharHeight)
	ON_EN_CHANGE(IDC_EDIT_1, &Cprintf_set::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_RADIO_UID, &Cprintf_set::OnBnClickedRadioUid)
	ON_BN_CLICKED(IDC_RADIO_MAC, &Cprintf_set::OnBnClickedRadioMac)
END_MESSAGE_MAP()


// Cprintf_set ��Ϣ�������

void Cprintf_set::OnBnClickedButtonSetPrintf()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	DWORD dwflags=PD_ALLPAGES|PD_NOPAGENUMS|PD_USEDEVMODECOPIES|PD_SELECTION|PD_HIDEPRINTTOFILE;
	CPrintDialog dlg(FALSE, dwflags, NULL);
	if(dlg.DoModal() == IDOK)
	{
		CString str = dlg.GetDeviceName();
		_tcscpy(g_test_config.PrinterName, str);
		SetDlgItemText(IDC_EDIT_PRINTER_NAME, g_test_config.PrinterName);
		SetDefaultPrinter(g_test_config.PrinterName);
	}
}

void Cprintf_set::OnBnClickedOk()
{
	CString str;
	BOOL flag = FALSE;

	USES_CONVERSION;

	GetDlgItemText(IDC_EDIT_PRINTER_NAME, str);
	_tcscpy(g_test_config.PrinterName, str);

	GetDlgItemText(IDC_EDIT_WITH, str);
     g_test_config.LabWidth = atof(T2A(str));

	 GetDlgItemText(IDC_EDIT_HEITH, str);
	 g_test_config.LabHeight = atof(T2A(str));

	 GetDlgItemText(IDC_EDIT_WITH_OFFSET, str);
	 g_test_config.OffsetV = atof(T2A(str));

	 GetDlgItemText(IDC_EDIT_HEITH_OFFSET, str);
	 g_test_config.OffsetH = atof(T2A(str));

	 GetDlgItemText(IDC_EDIT_WITH_OFFSET_SN, str);
	 g_test_config.SN_OffsetV = atof(T2A(str));

	 GetDlgItemText(IDC_EDIT_HEITH_OFFSET_SN, str);
	 g_test_config.SN_OffsetH = atof(T2A(str));

	 GetDlgItemText(IDC_EDIT_CHAR_HEIGHT, str);
	 g_test_config.char_height = atoi(T2A(str));
	 if(g_test_config.char_height == 0)
	 {

		 g_test_config.char_height = 3;
	 }


	 flag = ((CButton *)GetDlgItem(IDC_RADIO_UID))->GetCheck();
	 if (flag)
	 {
		g_test_config.printf_char_mode = 1;
	 }
	 else
	 {
		g_test_config.printf_char_mode = 0;
	 }


	 GetDlgItemText(IDC_EDIT_PID, str);
	 if (str.GetLength() != 10)
	 {
		 SetDlgItemText(IDC_EDIT_PID, _T(""));
		 AfxMessageBox(_T("PID�д�����"), MB_OK);
		 memset(g_test_config.PID, 0 , 32*sizeof(TCHAR));
	 }
	 else
	 {
		 memset(g_test_config.PID, 0 , 32*sizeof(TCHAR));
		 _tcscpy(g_test_config.PID, str);
	 }
		
	 //0
	 GetDlgItemText(IDC_COMBO_1, str);	
	 if (_tcscmp(str, _T("����ӡ")) == 0)
	 {
		g_test_config.config_printf_cfg[0].line_idex = LINE_0;
	 }
	 else if (_tcscmp(str, _T("��һ��")) == 0)
	 {
		 g_test_config.config_printf_cfg[0].line_idex = LINE_1;
	 }
	 else if (_tcscmp(str, _T("�ڶ���")) == 0)
	 {
		 g_test_config.config_printf_cfg[0].line_idex = LINE_2;
	 }
	 else if (_tcscmp(str, _T("������")) == 0)
	 {
		 g_test_config.config_printf_cfg[0].line_idex = LINE_3;
	 }
	 else if (_tcscmp(str, _T("������")) == 0)
	 {
		 g_test_config.config_printf_cfg[0].line_idex = LINE_4;
	 }
	 else if (_tcscmp(str, _T("������")) == 0)
	 {
		 g_test_config.config_printf_cfg[0].line_idex = LINE_5;
	 }
	 else
	 {
		g_test_config.config_printf_cfg[0].line_idex = LINE_0;
	 }

	 GetDlgItemText(IDC_EDIT_ID, str);
	 _tcscpy(g_test_config.config_printf_cfg[0].content_data, str);

	 GetDlgItemText(IDC_EDIT_HEITH1, str);
	 g_test_config.config_printf_cfg[0].fon_height = atof(T2A(str));

	 //1 
	 GetDlgItemText(IDC_COMBO_2, str);	
	 if (_tcscmp(str, _T("����ӡ")) == 0)
	 {
		 g_test_config.config_printf_cfg[1].line_idex = LINE_0;
	 }
	 else if (_tcscmp(str, _T("��һ��")) == 0)
	 {
		 g_test_config.config_printf_cfg[1].line_idex = LINE_1;
	 }
	 else if (_tcscmp(str, _T("�ڶ���")) == 0)
	 {
		 g_test_config.config_printf_cfg[1].line_idex = LINE_2;
	 }
	 else if (_tcscmp(str, _T("������")) == 0)
	 {
		 g_test_config.config_printf_cfg[1].line_idex = LINE_3;
	 }
	 else if (_tcscmp(str, _T("������")) == 0)
	 {
		 g_test_config.config_printf_cfg[1].line_idex = LINE_4;
	 }
	 else if (_tcscmp(str, _T("������")) == 0)
	 {
		 g_test_config.config_printf_cfg[1].line_idex = LINE_5;
	 }
	 else
	 {
		 g_test_config.config_printf_cfg[1].line_idex = LINE_0;
	 }

	 GetDlgItemText(IDC_EDIT_PASSWORD, str);
	 _tcscpy(g_test_config.config_printf_cfg[1].content_data, str);

	 GetDlgItemText(IDC_EDIT_HEITH2, str);
	 g_test_config.config_printf_cfg[1].fon_height = atof(T2A(str));	

	 //2 
	 GetDlgItemText(IDC_COMBO_3, str);	
	 if (_tcscmp(str, _T("����ӡ")) == 0)
	 {
		 g_test_config.config_printf_cfg[2].line_idex = LINE_0;
	 }
	 else if (_tcscmp(str, _T("��һ��")) == 0)
	 {
		 g_test_config.config_printf_cfg[2].line_idex = LINE_1;
	 }
	 else if (_tcscmp(str, _T("�ڶ���")) == 0)
	 {
		 g_test_config.config_printf_cfg[2].line_idex = LINE_2;
	 }
	 else if (_tcscmp(str, _T("������")) == 0)
	 {
		 g_test_config.config_printf_cfg[2].line_idex = LINE_3;
	 }
	 else if (_tcscmp(str, _T("������")) == 0)
	 {
		 g_test_config.config_printf_cfg[2].line_idex = LINE_4;
	 }
	 else if (_tcscmp(str, _T("������")) == 0)
	 {
		 g_test_config.config_printf_cfg[2].line_idex = LINE_5;
	 }
	 else
	 {
		 g_test_config.config_printf_cfg[2].line_idex = LINE_0;
	 }

	 GetDlgItemText(IDC_EDIT_1, str);
	 _tcscpy(g_test_config.config_printf_cfg[2].content_data, str);

	 GetDlgItemText(IDC_EDIT_HEITH3, str);
	 g_test_config.config_printf_cfg[2].fon_height = atof(T2A(str));

	 //3
	 GetDlgItemText(IDC_COMBO_4, str);	
	 if (_tcscmp(str, _T("����ӡ")) == 0)
	 {
		 g_test_config.config_printf_cfg[3].line_idex = LINE_0;
	 }
	 else if (_tcscmp(str, _T("��һ��")) == 0)
	 {
		 g_test_config.config_printf_cfg[3].line_idex = LINE_1;
	 }
	 else if (_tcscmp(str, _T("�ڶ���")) == 0)
	 {
		 g_test_config.config_printf_cfg[3].line_idex = LINE_2;
	 }
	 else if (_tcscmp(str, _T("������")) == 0)
	 {
		 g_test_config.config_printf_cfg[3].line_idex = LINE_3;
	 }
	 else if (_tcscmp(str, _T("������")) == 0)
	 {
		 g_test_config.config_printf_cfg[3].line_idex = LINE_4;
	 }
	 else if (_tcscmp(str, _T("������")) == 0)
	 {
		 g_test_config.config_printf_cfg[3].line_idex = LINE_5;
	 }
	 else
	 {
		 g_test_config.config_printf_cfg[3].line_idex = LINE_0;
	 }

	 GetDlgItemText(IDC_EDIT_2, str);
	 _tcscpy(g_test_config.config_printf_cfg[3].content_data, str);

	 GetDlgItemText(IDC_EDIT_HEITH4, str);
	 g_test_config.config_printf_cfg[3].fon_height = atof(T2A(str));

	 //4
	 GetDlgItemText(IDC_COMBO_5, str);	
	 if (_tcscmp(str, _T("����ӡ")) == 0)
	 {
		 g_test_config.config_printf_cfg[4].line_idex = LINE_0;
	 }
	 else if (_tcscmp(str, _T("��һ��")) == 0)
	 {
		 g_test_config.config_printf_cfg[4].line_idex = LINE_1;
	 }
	 else if (_tcscmp(str, _T("�ڶ���")) == 0)
	 {
		 g_test_config.config_printf_cfg[4].line_idex = LINE_2;
	 }
	 else if (_tcscmp(str, _T("������")) == 0)
	 {
		 g_test_config.config_printf_cfg[4].line_idex = LINE_3;
	 }
	 else if (_tcscmp(str, _T("������")) == 0)
	 {
		 g_test_config.config_printf_cfg[4].line_idex = LINE_4;
	 }
	 else if (_tcscmp(str, _T("������")) == 0)
	 {
		 g_test_config.config_printf_cfg[4].line_idex = LINE_5;
	 }
	 else
	 {
		 g_test_config.config_printf_cfg[4].line_idex = LINE_0;
	 }

	 GetDlgItemText(IDC_EDIT_3, str);
	 _tcscpy(g_test_config.config_printf_cfg[4].content_data, str);

	 GetDlgItemText(IDC_EDIT_HEITH5, str);
	 g_test_config.config_printf_cfg[4].fon_height = atof(T2A(str));



	 GetDlgItemText(IDC_COMBO_NUM, str);	
	 g_test_config.page_num = atoi(T2A(str));

	
	 g_test_config.Write_printf_cfg(CONFIG_PRINTF);
	 //Set_printer_info();

	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnOK();
}


BOOL Cprintf_set::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString str;

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	SetDlgItemText(IDC_EDIT_PRINTER_NAME, g_test_config.PrinterName);

	str.Format(_T("%0.2f"), g_test_config.LabWidth);
	SetDlgItemText(IDC_EDIT_WITH, str);
	str.Format(_T("%0.2f"), g_test_config.LabHeight);
	SetDlgItemText(IDC_EDIT_HEITH, str);
	str.Format(_T("%0.2f"), g_test_config.OffsetV);
	SetDlgItemText(IDC_EDIT_WITH_OFFSET, str);
	str.Format(_T("%0.2f"), g_test_config.OffsetH);
	SetDlgItemText(IDC_EDIT_HEITH_OFFSET, str);
	str.Format(_T("%0.2f"), g_test_config.SN_OffsetV);
	SetDlgItemText(IDC_EDIT_WITH_OFFSET_SN, str);
	str.Format(_T("%0.2f"), g_test_config.SN_OffsetH);
	SetDlgItemText(IDC_EDIT_HEITH_OFFSET_SN, str);
	
	str.Format(_T("%d"), g_test_config.char_height);
	SetDlgItemText(IDC_EDIT_CHAR_HEIGHT, str);
	

	str.Format(_T("%s"), g_test_config.PID);
	SetDlgItemText(IDC_EDIT_PID, str);

	if (g_test_config.printf_char_mode == 1)
	{
		((CButton *)GetDlgItem(IDC_RADIO_UID))->SetCheck(1);
		((CButton *)GetDlgItem(IDC_RADIO_MAC))->SetCheck(0);
	}
	else
	{
		((CButton *)GetDlgItem(IDC_RADIO_UID))->SetCheck(0);
		((CButton *)GetDlgItem(IDC_RADIO_MAC))->SetCheck(1);
	}
	
	//0
	if (g_test_config.config_printf_cfg[0].line_idex == LINE_0)
	{
		SetDlgItemText(IDC_COMBO_1, _T("����ӡ"));
	}
	else if (g_test_config.config_printf_cfg[0].line_idex == LINE_1)
	{
		SetDlgItemText(IDC_COMBO_1, _T("��һ��"));
	}
	else if (g_test_config.config_printf_cfg[0].line_idex == LINE_2)
	{
		SetDlgItemText(IDC_COMBO_1, _T("�ڶ���"));
	}
	else if (g_test_config.config_printf_cfg[0].line_idex == LINE_3)
	{
		SetDlgItemText(IDC_COMBO_1, _T("������"));
	}
	else if (g_test_config.config_printf_cfg[0].line_idex == LINE_4)
	{
		SetDlgItemText(IDC_COMBO_1, _T("������"));
	}
	else if (g_test_config.config_printf_cfg[0].line_idex == LINE_5)
	{
		SetDlgItemText(IDC_COMBO_1, _T("������"));
	}
	else
	{
		SetDlgItemText(IDC_COMBO_1, _T("����ӡ"));
	}
	
	str.Format(_T("%s"), g_test_config.config_printf_cfg[0].content_data);
	SetDlgItemText(IDC_EDIT_ID, str);

	str.Format(_T("%0.2f"), g_test_config.config_printf_cfg[0].fon_height);
	SetDlgItemText(IDC_EDIT_HEITH1,str );



	//1
	if (g_test_config.config_printf_cfg[1].line_idex == LINE_0)
	{
		SetDlgItemText(IDC_COMBO_2, _T("����ӡ"));
	}
	else if (g_test_config.config_printf_cfg[1].line_idex == LINE_1)
	{
		SetDlgItemText(IDC_COMBO_2, _T("��һ��"));
	}
	else if (g_test_config.config_printf_cfg[1].line_idex == LINE_2)
	{
		SetDlgItemText(IDC_COMBO_2, _T("�ڶ���"));
	}
	else if (g_test_config.config_printf_cfg[1].line_idex == LINE_3)
	{
		SetDlgItemText(IDC_COMBO_2, _T("������"));
	}
	else if (g_test_config.config_printf_cfg[1].line_idex == LINE_4)
	{
		SetDlgItemText(IDC_COMBO_2, _T("������"));
	}
	else if (g_test_config.config_printf_cfg[1].line_idex == LINE_5)
	{
		SetDlgItemText(IDC_COMBO_2, _T("������"));
	}
	else
	{
		SetDlgItemText(IDC_COMBO_2, _T("����ӡ"));
	}

	str.Format(_T("%s"), g_test_config.config_printf_cfg[1].content_data);
	SetDlgItemText(IDC_EDIT_PASSWORD, str);

	str.Format(_T("%0.2f"), g_test_config.config_printf_cfg[1].fon_height);
	SetDlgItemText(IDC_EDIT_HEITH2, str);


	//2
	if (g_test_config.config_printf_cfg[2].line_idex == LINE_0)
	{
		SetDlgItemText(IDC_COMBO_3, _T("����ӡ"));
	}
	else if (g_test_config.config_printf_cfg[2].line_idex == LINE_1)
	{
		SetDlgItemText(IDC_COMBO_3, _T("��һ��"));
	}
	else if (g_test_config.config_printf_cfg[2].line_idex == LINE_2)
	{
		SetDlgItemText(IDC_COMBO_3, _T("�ڶ���"));
	}
	else if (g_test_config.config_printf_cfg[2].line_idex == LINE_3)
	{
		SetDlgItemText(IDC_COMBO_3, _T("������"));
	}
	else if (g_test_config.config_printf_cfg[2].line_idex == LINE_4)
	{
		SetDlgItemText(IDC_COMBO_3, _T("������"));
	}
	else if (g_test_config.config_printf_cfg[2].line_idex == LINE_5)
	{
		SetDlgItemText(IDC_COMBO_3, _T("������"));
	}
	else
	{
		SetDlgItemText(IDC_COMBO_3, _T("����ӡ"));
	}

	str.Format(_T("%s"), g_test_config.config_printf_cfg[2].content_data);
		SetDlgItemText(IDC_EDIT_1, str);

	str.Format(_T("%0.2f"), g_test_config.config_printf_cfg[2].fon_height);
		SetDlgItemText(IDC_EDIT_HEITH3, str);

	//3
	if (g_test_config.config_printf_cfg[3].line_idex == LINE_0)
	{
		SetDlgItemText(IDC_COMBO_4, _T("����ӡ"));
	}
	else if (g_test_config.config_printf_cfg[3].line_idex == LINE_1)
	{
		SetDlgItemText(IDC_COMBO_4, _T("��һ��"));
	}
	else if (g_test_config.config_printf_cfg[3].line_idex == LINE_2)
	{
		SetDlgItemText(IDC_COMBO_4, _T("�ڶ���"));
	}
	else if (g_test_config.config_printf_cfg[3].line_idex == LINE_3)
	{
		SetDlgItemText(IDC_COMBO_4, _T("������"));
	}
	else if (g_test_config.config_printf_cfg[3].line_idex == LINE_4)
	{
		SetDlgItemText(IDC_COMBO_4, _T("������"));
	}
	else if (g_test_config.config_printf_cfg[3].line_idex == LINE_5)
	{
		SetDlgItemText(IDC_COMBO_4, _T("������"));
	}
	else
	{
		SetDlgItemText(IDC_COMBO_4, _T("����ӡ"));
	}

	str.Format(_T("%s"), g_test_config.config_printf_cfg[3].content_data);
		SetDlgItemText(IDC_EDIT_2, str);

	str.Format(_T("%0.2f"), g_test_config.config_printf_cfg[3].fon_height);
		SetDlgItemText(IDC_EDIT_HEITH4, str);

	//4
	if (g_test_config.config_printf_cfg[4].line_idex == LINE_0)
	{
		SetDlgItemText(IDC_COMBO_5, _T("����ӡ"));
	}
	else if (g_test_config.config_printf_cfg[4].line_idex == LINE_1)
	{
		SetDlgItemText(IDC_COMBO_5, _T("��һ��"));
	}
	else if (g_test_config.config_printf_cfg[4].line_idex == LINE_2)
	{
		SetDlgItemText(IDC_COMBO_5, _T("�ڶ���"));
	}
	else if (g_test_config.config_printf_cfg[4].line_idex == LINE_3)
	{
		SetDlgItemText(IDC_COMBO_5, _T("������"));
	}
	else if (g_test_config.config_printf_cfg[4].line_idex == LINE_4)
	{
		SetDlgItemText(IDC_COMBO_5, _T("������"));
	}
	else if (g_test_config.config_printf_cfg[4].line_idex == LINE_5)
	{
		SetDlgItemText(IDC_COMBO_5, _T("������"));
	}
	else
	{
		SetDlgItemText(IDC_COMBO_5, _T("����ӡ"));
	}

	str.Format(_T("%s"), g_test_config.config_printf_cfg[4].content_data);
		SetDlgItemText(IDC_EDIT_3, str);

	str.Format(_T("%0.2f"), g_test_config.config_printf_cfg[4].fon_height);
		SetDlgItemText(IDC_EDIT_HEITH5, str);

	str.Format(_T("%d"), g_test_config.page_num);
	SetDlgItemText(IDC_COMBO_NUM, str);
	
	OnCbnSelchangeCombo1();
	OnCbnSelchangeCombo2();
	OnCbnSelchangeCombo3();
	OnCbnSelchangeCombo4();
	OnCbnSelchangeCombo5();

	
	if (g_test_config.is_printf_lab)
	{
		((CButton *)GetDlgItem(IDC_RADIO_PRINTF_LAB))->SetCheck(TRUE);

		((CButton *)GetDlgItem(IDC_COMBO_1))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_COMBO_2))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_COMBO_3))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_COMBO_4))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_COMBO_5))->EnableWindow(TRUE);

		((CButton *)GetDlgItem(IDC_EDIT_ID))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_EDIT_PASSWORD))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_EDIT_1))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_EDIT_2))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_EDIT_3))->EnableWindow(TRUE);

		((CButton *)GetDlgItem(IDC_EDIT_HEITH1))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_EDIT_HEITH2))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_EDIT_HEITH3))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_EDIT_HEITH4))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_EDIT_HEITH5))->EnableWindow(TRUE);

		((CButton *)GetDlgItem(IDC_EDIT_WITH))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_EDIT_HEITH))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_EDIT_WITH_OFFSET))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_EDIT_HEITH_OFFSET))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_EDIT_PID))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_EDIT_CHAR_HEIGHT))->EnableWindow(FALSE);
	} 
    else
	{
		((CButton *)GetDlgItem(IDC_RADIO_PRINTF_IMG))->SetCheck(TRUE);

		((CButton *)GetDlgItem(IDC_COMBO_1))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_COMBO_2))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_COMBO_3))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_COMBO_4))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_COMBO_5))->EnableWindow(FALSE);

		((CButton *)GetDlgItem(IDC_EDIT_ID))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_EDIT_PASSWORD))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_EDIT_1))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_EDIT_2))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_EDIT_3))->EnableWindow(FALSE);


		((CButton *)GetDlgItem(IDC_EDIT_HEITH1))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_EDIT_HEITH2))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_EDIT_HEITH3))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_EDIT_HEITH4))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_EDIT_HEITH5))->EnableWindow(FALSE);

		((CButton *)GetDlgItem(IDC_EDIT_WITH))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_EDIT_HEITH))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_EDIT_WITH_OFFSET))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_EDIT_HEITH_OFFSET))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_EDIT_PID))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_EDIT_CHAR_HEIGHT))->EnableWindow(TRUE);
	} 
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void Cprintf_set::OnCbnSelchangeCombo1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	GetDlgItemText(IDC_COMBO_1, str);	
	if (_tcscmp(str, _T("����ӡ")) == 0)
	{
		GetDlgItem(IDC_EDIT_ID)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_HEITH1)->EnableWindow(FALSE);

	}
	else
	{
		GetDlgItem(IDC_EDIT_ID)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_HEITH1)->EnableWindow(TRUE);
	}
}

void Cprintf_set::OnCbnSelchangeCombo2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	GetDlgItemText(IDC_COMBO_2, str);	
	if (_tcscmp(str, _T("����ӡ")) == 0)
	{
		GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_HEITH2)->EnableWindow(FALSE);

	}
	else
	{
		GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_HEITH2)->EnableWindow(TRUE);
	}
}

void Cprintf_set::OnCbnSelchangeCombo3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	GetDlgItemText(IDC_COMBO_3, str);	
	if (_tcscmp(str, _T("����ӡ")) == 0)
	{
		GetDlgItem(IDC_EDIT_1)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_HEITH3)->EnableWindow(FALSE);

	}
	else
	{
		GetDlgItem(IDC_EDIT_1)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_HEITH3)->EnableWindow(TRUE);
	}
}

void Cprintf_set::OnCbnSelchangeCombo4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	GetDlgItemText(IDC_COMBO_4, str);	
	if (_tcscmp(str, _T("����ӡ")) == 0)
	{
		GetDlgItem(IDC_EDIT_2)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_HEITH4)->EnableWindow(FALSE);

	}
	else
	{
		GetDlgItem(IDC_EDIT_2)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_HEITH4)->EnableWindow(TRUE);
	}
}

void Cprintf_set::OnCbnSelchangeCombo5()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	GetDlgItemText(IDC_COMBO_5, str);	
	if (_tcscmp(str, _T("����ӡ")) == 0)
	{
		GetDlgItem(IDC_EDIT_3)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_HEITH5)->EnableWindow(FALSE);

	}
	else
	{
		GetDlgItem(IDC_EDIT_3)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_HEITH5)->EnableWindow(TRUE);
	}
}



void Cprintf_set::Set_printer_info()
{
	DWORD   Errno,buf[2000],len=sizeof(PRINTER_INFO_2);   
	PRINTER_INFO_2   pinfo2;   
//	TCHAR   PrinterName[128];   
//	LPDEVMODE     lpmod;
	DEVMODE     devmod;   
//	PRINTDLG       pda;   
	PRINTER_DEFAULTS   pDefault;
	HANDLE     hPrinter;
	CSize curPaperSize;
#if 0
	memset(&pda,0,sizeof(pda));
	pda.lStructSize   =   sizeof(pda);
	//PD_RETURNDEFAULTʹ֮���Ĭ�ϴ�ӡ���������Ҳ���ʾ�Ի���   
	pda.Flags   =   PD_RETURNDEFAULT|PD_ALLPAGES   |   PD_USEDEVMODECOPIES   |   PD_NOPAGENUMS   |   PD_HIDEPRINTTOFILE   |   PD_NOSELECTION; 
	PrintDlg(&pda);   //   ���ȱʡ��ӡ������   
	Errno=GetLastError();   
	if(Errno==ERROR_SUCCESS)
	{
		if(pda.hDevMode   !=NULL)
		{   
			lpmod = (LPDEVMODE)GlobalLock(pda.hDevMode);   
			//strcpy(PrinterName,(char   *)lpmod->dmDeviceName   );//���ȱʡ��ӡ������   
			GlobalUnlock(pda.hDevMode);   
		}   
	}   
	else
	{   
		return;   
	}   
#endif
	memset(&pDefault,0,sizeof(pDefault));   
	memset(&devmod,0,sizeof(devmod));   
	memset(&pinfo2,0,sizeof(pinfo2));   
	pDefault.DesiredAccess   =PRINTER_ALL_ACCESS;   
	if(!OpenPrinter( g_test_config.PrinterName, &hPrinter, NULL ) )  
	{  
		AfxMessageBox(_T("���ô�ӡ��ʱ��ʧ��"), MB_OK);
		return;  
	} 
	
	Errno=GetLastError();   
	if(Errno) 
	{
		return;  
	}

	
	GetPrinter(hPrinter,2,(BYTE   *)buf,sizeof(buf),&len);   
	if(Errno)   
	{
		return;  
	}
	else
	{   
		memcpy((void* )&pinfo2,buf,sizeof(pinfo2));   
 		pinfo2.pDevMode->dmPaperWidth     =   260;   
 		pinfo2.pDevMode->dmPaperLength     =   130;   
 		//pinfo2.pDevMode->dmOrientation   =   Orientation;  
#if 0
		map<CString ,int >::iterator iter;; 
		iter = resMap.find(paperSize);
		if(iter == resMap.end())
		{
			AfxMessageBox(_T("��ӡ����֧�ֵ�ǰֽ������"));
			return FALSE;
		}
		pinfo2.pDevMode->dmPaperSize = iter->second;
		curDmPaperSize = iter->second; //do not forget,pleaz
#endif
	}   
	SetPrinter(hPrinter,2,(unsigned   char   *)&pinfo2,NULL);   
	// 	Errno=::GetLastError();   
	// 	if(Errno)   
	// 		return  false;   
	ClosePrinter(hPrinter);   
	//curPaperSize = GetPaperSize();
	return;   
}
void Cprintf_set::OnCbnSelchangeComboNum()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void Cprintf_set::OnBnClickedRadioPrintfLab()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BOOL check = FALSE;
	check = ((CButton *)GetDlgItem(IDC_RADIO_PRINTF_LAB))->GetCheck();
	if (check)
	{
		((CButton *)GetDlgItem(IDC_COMBO_1))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_COMBO_2))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_COMBO_3))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_COMBO_4))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_COMBO_5))->EnableWindow(TRUE);

		((CButton *)GetDlgItem(IDC_EDIT_ID))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_EDIT_PASSWORD))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_EDIT_1))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_EDIT_2))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_EDIT_3))->EnableWindow(TRUE);


		((CButton *)GetDlgItem(IDC_EDIT_HEITH1))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_EDIT_HEITH2))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_EDIT_HEITH3))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_EDIT_HEITH4))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_EDIT_HEITH5))->EnableWindow(TRUE);


		((CButton *)GetDlgItem(IDC_EDIT_WITH))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_EDIT_HEITH))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_EDIT_WITH_OFFSET))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_EDIT_HEITH_OFFSET))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_EDIT_PID))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_EDIT_CHAR_HEIGHT))->EnableWindow(FALSE);

		g_test_config.is_printf_lab = TRUE;
	} 
}

void Cprintf_set::OnBnClickedRadioPrintfImg()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BOOL check = FALSE;
	check = ((CButton *)GetDlgItem(IDC_RADIO_PRINTF_IMG))->GetCheck();
	if (check)
	{
		((CButton *)GetDlgItem(IDC_COMBO_1))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_COMBO_2))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_COMBO_3))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_COMBO_4))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_COMBO_5))->EnableWindow(FALSE);

		((CButton *)GetDlgItem(IDC_EDIT_ID))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_EDIT_PASSWORD))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_EDIT_1))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_EDIT_2))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_EDIT_3))->EnableWindow(FALSE);


		((CButton *)GetDlgItem(IDC_EDIT_HEITH1))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_EDIT_HEITH2))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_EDIT_HEITH3))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_EDIT_HEITH4))->EnableWindow(FALSE);
		((CButton *)GetDlgItem(IDC_EDIT_HEITH5))->EnableWindow(FALSE);

		((CButton *)GetDlgItem(IDC_EDIT_WITH))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_EDIT_HEITH))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_EDIT_WITH_OFFSET))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_EDIT_HEITH_OFFSET))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_EDIT_PID))->EnableWindow(TRUE);
		((CButton *)GetDlgItem(IDC_EDIT_CHAR_HEIGHT))->EnableWindow(TRUE);
		
		

		g_test_config.is_printf_lab = FALSE;
	} 

}

void Cprintf_set::OnEnChangeEditWith()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	USES_CONVERSION;

	CString str;
	GetDlgItemText(IDC_EDIT_WITH, str);

	if (str.IsEmpty())
	{
		return;
	}

	g_test_config.LabWidth = atof(T2A(str));


}

void Cprintf_set::OnEnChangeEditHeith()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	USES_CONVERSION;

	CString str;
	GetDlgItemText(IDC_EDIT_HEITH, str);

	if (str.IsEmpty())
	{
		return;
	}

	g_test_config.LabHeight = atof(T2A(str));
}

void Cprintf_set::OnEnChangeEditWithOffset()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	USES_CONVERSION;

	CString str;
	GetDlgItemText(IDC_EDIT_WITH_OFFSET, str);

	if (str.IsEmpty())
	{
		return;
	}

	g_test_config.OffsetV = atof(T2A(str));
}

void Cprintf_set::OnEnChangeEditHeithOffset()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	USES_CONVERSION;

	CString str;
	GetDlgItemText(IDC_EDIT_HEITH_OFFSET, str);

	if (str.IsEmpty())
	{
		return;
	}

	g_test_config.OffsetH = atof(T2A(str));
}

void Cprintf_set::OnEnChangeEditPid()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	USES_CONVERSION;

	CString str;
	GetDlgItemText(IDC_EDIT_PID, str);

	if (str.IsEmpty())
	{
		return;
	}

	if (str.GetLength() > 10)
	{
		SetDlgItemText(IDC_EDIT_PID, _T(""));
		AfxMessageBox(_T("PID�ĳ��ȳ���10"), MB_OK);
		return;
	}

	memset(g_test_config.PID, 0 , 32*sizeof(TCHAR));
	_tcscpy(g_test_config.PID, str);
}

void Cprintf_set::OnEnChangeEditCharHeight()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	USES_CONVERSION;

	CString str;
	GetDlgItemText(IDC_EDIT_CHAR_HEIGHT, str);

	if (str.IsEmpty())
	{
		return;
	}

	g_test_config.char_height = atoi(T2A(str));

	
	
}

void Cprintf_set::OnEnChangeEdit1()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void Cprintf_set::OnBnClickedRadioUid()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	BOOL flag = FALSE;

	flag = ((CButton *)GetDlgItem(IDC_RADIO_UID))->GetCheck();
	if (flag)
	{
		g_test_config.printf_char_mode = 1;
	} 
	else
	{
		g_test_config.printf_char_mode = 0;
	}
}

void Cprintf_set::OnBnClickedRadioMac()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	BOOL flag = FALSE;

	flag = ((CButton *)GetDlgItem(IDC_RADIO_MAC))->GetCheck();
	if (flag)
	{
		g_test_config.printf_char_mode = 0;
	} 
	else
	{
		g_test_config.printf_char_mode = 1;
	}
}
