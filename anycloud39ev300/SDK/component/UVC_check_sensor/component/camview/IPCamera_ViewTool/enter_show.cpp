// enter_show.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "enter_show.h"


// Center_show �Ի���

extern BOOL g_check_picture_flag;

IMPLEMENT_DYNAMIC(Center_show, CDialog)

Center_show::Center_show(CWnd* pParent /*=NULL*/)
	: CDialog(Center_show::IDD, pParent)
{

}

Center_show::~Center_show()
{
}

void Center_show::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Center_show, CDialog)
	ON_BN_CLICKED(IDOK, &Center_show::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &Center_show::OnBnClickedCancel)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// Center_show ��Ϣ�������

void Center_show::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	
	show_img.DoModal();
	OnOK();
}

void Center_show::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnCancel();
}

void Center_show::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialog::OnClose();
}
