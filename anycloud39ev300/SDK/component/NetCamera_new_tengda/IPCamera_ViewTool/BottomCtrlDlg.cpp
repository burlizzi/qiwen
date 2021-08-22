// BottomCtrlDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Anyka IP Camera.h"
#include "Anyka IP CameraDlg.h"
#include "BottomCtrlDlg.h"


extern BOOL g_connet_flag ;
extern TCHAR m_connect_ip[MAX_PATH+1];

// CBottomCtrlDlg �Ի���

IMPLEMENT_DYNAMIC(CBottomCtrlDlg, CDialog)

CBottomCtrlDlg::CBottomCtrlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBottomCtrlDlg::IDD, pParent)
{

}

CBottomCtrlDlg::~CBottomCtrlDlg()
{
}

void CBottomCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBottomCtrlDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CONFIGURE, &CBottomCtrlDlg::OnBnClickedButtonConfigure)
	ON_BN_CLICKED(IDC_BUTTON_WRITE_UID, &CBottomCtrlDlg::OnBnClickedButtonWriteUid)
	ON_BN_CLICKED(IDC_BUTTON_START, &CBottomCtrlDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CBottomCtrlDlg::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, &CBottomCtrlDlg::OnBnClickedButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CBottomCtrlDlg::OnBnClickedButtonReset)
	ON_EN_CHANGE(IDC_EDIT_PRESENT, &CBottomCtrlDlg::OnEnChangeEditPresent)
END_MESSAGE_MAP()


// CBottomCtrlDlg ��Ϣ�������
void CBottomCtrlDlg::OnBnClickedButtonConfigure()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CAnykaIPCameraDlg * pP = (CAnykaIPCameraDlg *)GetParent();
	pP->OnBnClickedButtonConfigure();
}

void CBottomCtrlDlg::OnBnClickedButtonWriteUid()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CAnykaIPCameraDlg * pP = (CAnykaIPCameraDlg *)GetParent();
	pP->OnBnClickedButtonWriteUid();

	
}

void CBottomCtrlDlg::OnBnClickedButtonStart()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CAnykaIPCameraDlg * pP = (CAnykaIPCameraDlg *)GetParent();
	pP->OnBnClickedButtonStart();
}

void CBottomCtrlDlg::OnBnClickedButtonClose()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CAnykaIPCameraDlg * pP = (CAnykaIPCameraDlg *)GetParent();
	pP->OnBnClickedButtonClose();
}

void CBottomCtrlDlg::OnBnClickedButtonNext()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CAnykaIPCameraDlg * pP = (CAnykaIPCameraDlg *)GetParent();
	pP->OnBnClickedButtonNext();
}

void CBottomCtrlDlg::OnBnClickedButtonReset()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CAnykaIPCameraDlg * pP = (CAnykaIPCameraDlg *)GetParent();
	pP->OnBnClickedButtonReset();
}

void CBottomCtrlDlg::OnEnChangeEditPresent()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}
