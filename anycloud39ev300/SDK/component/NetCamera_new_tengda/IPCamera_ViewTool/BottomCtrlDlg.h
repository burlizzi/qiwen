#pragma once

// CBottomCtrlDlg �Ի���

class CBottomCtrlDlg : public CDialog
{
	DECLARE_DYNAMIC(CBottomCtrlDlg)

public:
	CBottomCtrlDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CBottomCtrlDlg();

	// �Ի�������
	enum { IDD = IDD_BOTTOMVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonConfigure();
	afx_msg void OnBnClickedButtonWriteUid();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnBnClickedButtonNext();
	afx_msg void OnBnClickedButtonReset();
	afx_msg void OnEnChangeEditPresent();
};
