#pragma once


#include "resource.h"
#include "afxwin.h"
#include "Error_Dlg.h"


// CCBurn_UIdDlg �Ի���

class CCBurn_UIdDlg : public CDialog
{
	DECLARE_DYNAMIC(CCBurn_UIdDlg)

public:

	CError_Dlg Error_Dlg;
	CCBurn_UIdDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCBurn_UIdDlg();
	CString m_str_uid;

// �Ի�������
	enum { IDD = IDD_DIALOG_BURN_UID };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:

	BOOL UID_PASS_flag;

	BOOL UID_first_flag;
	void Set_confim();
	BOOL SaveUid();
	BOOL WriteUid();
	BOOL OnCheck();


	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnEnChangeEditQrCode();
	afx_msg void OnEnKillfocusEditQrCode();
	CEdit m_uid_edit;
	virtual BOOL OnInitDialog();
};
