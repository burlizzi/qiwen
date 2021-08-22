#pragma once
#include "afxcmn.h"
#include "StaticEx.h"
#include "afxwin.h"


// CRightCtrlDlg �Ի���

class CRightCtrlDlg : public CDialog
{
	DECLARE_DYNAMIC(CRightCtrlDlg)

public:
	CRightCtrlDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CRightCtrlDlg();

// �Ի�������
	enum { IDD = IDD_FORMVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonPass();
	afx_msg void OnBnClickedButtonFailed();
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	CListCtrl m_test_config;
	CStaticEx m_Status;
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnStnClickedStaticTestContent();
	afx_msg void OnNMSetfocusList1(NMHDR *pNMHDR, LRESULT *pResult);
	CButton m_test_pass_btn;
};
