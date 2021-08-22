#pragma once


#include "resource.h"
#include "afxwin.h"
#include <atlbase.h>
#include <afxpriv2.h>


// Cshow_img �Ի���

class Cshow_img : public CDialog
{
	DECLARE_DYNAMIC(Cshow_img)

public:
	Cshow_img(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~Cshow_img();

// �Ի�������
	enum { IDD = IDD_DIALOG_SHOW_IMG };

protected:
	CComQIPtr<IPicture> m_pPict;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CRect m_ImgRect;
	TCHAR m_imgpath[260];
	
	afx_msg void OnPaint();
	afx_msg void OnClose();
	BOOL OnInitDialog();
	void draw_Picture(CDC *pDC);

	void ShowPicture(CDC *pDC, CString m_strBRoute, int x, int y, int width, int height);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
