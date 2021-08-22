// anyka_TestTool.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "anyka_TestTool.h"
#include "anyka_TestToolDlg.h"
#include "Config_test.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CInternetSession *m_pInetSession[UPDATE_MAX_NUM];
extern CFtpConnection *m_pFtpConnection[UPDATE_MAX_NUM]; 


/////////////////////////////////////////////////////////////////////////////
// CAnyka_TestToolApp

BEGIN_MESSAGE_MAP(CAnyka_TestToolApp, CWinApp)
	//{{AFX_MSG_MAP(CAnyka_TestToolApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnyka_TestToolApp construction

CAnyka_TestToolApp::CAnyka_TestToolApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAnyka_TestToolApp object

CAnyka_TestToolApp theApp;
extern CConfig_test g_test_config;

/////////////////////////////////////////////////////////////////////////////
// CAnyka_TestToolApp initialization

BOOL CAnyka_TestToolApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CAnyka_TestToolDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	//�ر��ļ�ʱ������д���ݵ��ļ���
	//g_test_config.Write_Config(CONFIG_FILE);

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
