#pragma once

#include "resource.h"
#include "afxwin.h"
#include "show_img.h"

// Center_show �Ի���

class Center_show : public CDialog
{
	DECLARE_DYNAMIC(Center_show)

public:
	Center_show(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~Center_show();

// �Ի�������
	enum { IDD = IDD_DIALOG_ENTER_SHOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	Cshow_img show_img;


	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnClose();
};
