// Anyka IP CameraDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Anyka IP Camera.h"
#include "Anyka IP CameraDlg.h"
#include <atlconv.h>
#include <time.h>
#include "IPCameraCommand.h"
#include "NetCtrl.h"
#include "ServerSearch.h"
#include "ServerInfo.h"
#include <process.h>
#include "afxinet.h"
#include "Config_test.h"
#include "logfile.h"
#include "LogUidFile.h"
#include "PcmSpeaker.h"

typedef enum
{
	CASE_VIDEO = 0,		
	CASE_IRCUT_ON,
	CASE_IRCUT_OFF,
	CASE_MONITOR,   //
	CASE_INTERPHONE,//�Խ�
	CASE_HEAD,      //��̨
	CASE_SD,
	CASE_WIFI,
	CASE_RESET,
	//CASE_INFRARED,
	CASE_UID,
	CASE_MAC,
	//CASE_RESET,
}E_MEMORY_TYPE;


typedef enum
{
	IDS_TEST_SUCCESS,
	IDS_TEST_FAIL,
}E_RESULT_TYPE;


#define VIDEO_MODE_NAME_720P	L"720P"
#define VIDEO_MODE_NAME_D1		L"D1"
#define VIDEO_MODE_NAME_VGA		L"VGA"
#define VIDEO_MODE_NAME_QVGA	L"QVGA"

#define RTSP_PREFIX				"rtsp://"
#define SEPARATOR				"/"
#define CHAR_SEPARATOR			'/'
#define PORT_PREFIX				":"
#define TREE_ROOT_ITEM_NAME		L"�豸�б�:"
#define MAX_RTSP_URL_LEN		(MAX_PATH + 24)

#define TIMER_COMMAND			1
#define TIMER_LONG_PRESS		2

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define  TOOL_VERSOIN _T("�������Թ���_v2.0.10.03")


//just for client debug.
#define UNATTACHED_TALK
#define SBAR_PROGRAMINFO_SCALE		0.4
#define SBAR_DISPLAYINFO_SCALE		0.3

#define ID_TOOLBAR_BUTTON_PIC		WM_USER + 100
#define ID_TOOLBAR_BUTTON_REC		WM_USER + 101
#define ID_TOOLBAR_BUTTON_TAK		WM_USER + 102
#define ID_TOOLBAR_BUTTON_PLY		WM_USER + 103
#define ID_TOOLBAR_BUTTON_ZIN		WM_USER + 104
#define ID_TOOLBAR_BUTTON_ZOUT		WM_USER + 105
#define ID_TOOLBAR_BUTTON_VPLUS		WM_USER + 106
#define ID_TOOLBAR_BUTTON_VMINUS	WM_USER + 107
#define ID_TOOLBAR_BUTTON_STOP_REC	WM_USER + 108

#define ID_STATUSBAR_PROGRAMINFO	WM_USER + 200
#define ID_STATUSBAR_DISPLAYINFO1	WM_USER + 201
#define ID_STATUSBAR_DISPLAYINFO2	WM_USER + 202

#define WM_TALK_KICKOUT				WM_USER + 300
#define WM_SERVER_DISCONNECT		WM_USER + 301
#define WM_SERVER_RET_INFO			WM_USER + 302
#define ATTEMPT_OPEN_MAX			3


static HWAVEOUT dev				= NULL;
static CRITICAL_SECTION			cs;


BOOL g_start_open_flag = TRUE;
HANDLE g_hTestThread = INVALID_HANDLE_VALUE;
HANDLE g_findIP_Thread = INVALID_HANDLE_VALUE;
BOOL g_first_find_flag = TRUE;

HWND g_hWnd = 0;
BOOL g_Full_flag = TRUE;
TCHAR ip_address[260] = {0};
CInternetSession *m_pInetSession = NULL;
CFtpConnection *m_pFtpConnection = NULL; 
UINT m_uPort = 0;
UINT m_net_uPort = 0;
HANDLE g_hBurnThread_rev_data= INVALID_HANDLE_VALUE;
HANDLE g_hBurnThread_play_pcm= INVALID_HANDLE_VALUE;

BOOL play_pcm_finish_flag = FALSE;
UINT rve_param[2] = {0};
char g_send_commad = 0;
char g_test_fail_flag = 0;
char g_test_pass_flag = 0;  //0���ڲ����У� //1���Գɹ��� 2����ʧ��
char g_commad_type;
BOOL g_connet_flag = FALSE;
BOOL download_file_flag = FALSE;
BOOL download_dev_file_flag = FALSE;
BOOL g_senddata_flag = TRUE;
BOOL entern_flag = FALSE;
BOOL login_entern_flag = FALSE;
HANDLE g_heatThread= INVALID_HANDLE_VALUE;

BOOL config_uid_enable_temp=FALSE;
BOOL config_lan_mac_enable_temp=FALSE;


BOOL m_not_find_anyIP = FALSE;
BOOL m_find_anyIP = FALSE;
BOOL m_find_IP_end_flag = FALSE;
UINT m_ip_address_idex = 0;
BOOL g_sousuo_flag = FALSE;
CConfig_test g_test_config;
BOOL M_bConn;
TCHAR m_connect_ip[MAX_PATH+1] = {0};
UINT g_case_idex;
CLogFile  frmLogfile;
CLogUidFile frmLogUidFile;
UINT current_ip_idex = 0;

BOOL end_test=TRUE;

BOOL first_flag = FALSE;
BOOL init_flag=FALSE;



#define CONFIG_PATH 	L"config.txt"
#define  TEST_CONFIG_DIR           _T("test_config")

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAnykaIPCameraDlg �Ի���

unsigned int CAnykaIPCameraDlg::thread_begin( void * pParam )
{
	CAnykaIPCameraDlg * pDlg = static_cast<CAnykaIPCameraDlg *>(pParam);
	pDlg->Monitor();
	return 0;
}

CAnykaIPCameraDlg::CAnykaIPCameraDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAnykaIPCameraDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	ZeroMemory(m_pClient, SUPPORT_STREAM_CNT * sizeof(CAimer39RTSPClient*));
	ZeroMemory(m_videoDecoder, SUPPORT_STREAM_CNT * sizeof(CFfmpegEnvoy*));
	ZeroMemory(m_AudioDecoder, SUPPORT_STREAM_CNT * sizeof(CFfmpegEnvoy*));
	ZeroMemory(m_videoRender, SUPPORT_STREAM_CNT * sizeof(CVideoRender*));
	ZeroMemory(m_AudioRender, SUPPORT_STREAM_CNT * sizeof(CAudioRender*));
	ZeroMemory(m_pServerPreviews, PREVIEW_WINDOWS * sizeof(IServer*));
	ZeroMemory(&m_stKickOutParam, sizeof(KickOutMessageWParam));
	ZeroMemory(&m_stRetInfo, sizeof(RETINFO));

	for (int i = 0; i < PREVIEW_WINDOWS; ++i) m_strURL[i].clear();

	m_nRBChoosePrevIndex = -1;
	m_nAudioClientIndex = -1;
	m_nVideoFullScreenIndex = -1;
	m_nLongPressButtonID = -1;
	m_bIsSearch = FALSE;
	m_hCurrentSelectItem = NULL;
	m_runThreadFlag = FALSE;
	m_bNeedJudgeDisConnWork = TRUE;
	m_bIsInit = FALSE;
	m_bPicture = TRUE;

	m_bIsLongPress = FALSE;
	m_bIsLongPressDone = FALSE;
}

void CAnykaIPCameraDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_TreeCtrl);
	//DDX_Control(pDX, IDC_COMBO1, m_ContrastCombo);
	//DDX_Control(pDX, IDC_COMBO2, m_SaturationCombo);
	//DDX_Control(pDX, IDC_COMBO3, m_BrightnessCombo);
	//DDX_Control(pDX, IDC_COMBO4, m_acutanceCom);
	DDX_Control(pDX, IDC_LIST1, m_test_config);
}

BEGIN_MESSAGE_MAP(CAnykaIPCameraDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_32771, &CAnykaIPCameraDlg::OnPrivacyArea)
	ON_COMMAND(ID_TOOLBAR_BUTTON_PIC, &CAnykaIPCameraDlg::OnPicture)
	ON_COMMAND(ID_TOOLBAR_BUTTON_REC, &CAnykaIPCameraDlg::OnRecord)
	ON_COMMAND(ID_TOOLBAR_BUTTON_ZIN, &CAnykaIPCameraDlg::OnZoomIn)
	ON_COMMAND(ID_TOOLBAR_BUTTON_ZOUT, &CAnykaIPCameraDlg::OnZoomOut)
	ON_COMMAND(ID_TOOLBAR_BUTTON_PLY, &CAnykaIPCameraDlg::OnPlay)
	ON_COMMAND(ID_TOOLBAR_BUTTON_VMINUS, &CAnykaIPCameraDlg::OnVolumeMinus)
	ON_COMMAND(ID_TOOLBAR_BUTTON_VPLUS, &CAnykaIPCameraDlg::OnVolumePlus)
	ON_COMMAND(ID_TOOLBAR_BUTTON_STOP_REC, &CAnykaIPCameraDlg::OnStopRecord)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CAnykaIPCameraDlg::OnTvnSelchangedTree1)
	ON_NOTIFY(NM_RCLICK, IDC_TREE1, &CAnykaIPCameraDlg::OnNMRClickTree1)
	ON_WM_CLOSE()
	ON_COMMAND(ID_32773, &CAnykaIPCameraDlg::OnSearchDevice)
	ON_WM_TIMER()
	ON_COMMAND(ID_PREVIEWCHOOSE_32774, &CAnykaIPCameraDlg::OnPreviewchoose1)
	ON_COMMAND(ID_PREVIEWCHOOSE_32775, &CAnykaIPCameraDlg::OnPreviewchoose2)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CAnykaIPCameraDlg::OnCbnSelchangeContrastCombo)
	ON_CBN_SELCHANGE(IDC_COMBO3, &CAnykaIPCameraDlg::OnCbnSelchangeBrightnessCombo)
	ON_CBN_SELCHANGE(IDC_COMBO2, &CAnykaIPCameraDlg::OnCbnSelchangeSaturationCombo)
	ON_COMMAND(ID_32772, &CAnykaIPCameraDlg::OnMotionDetect)
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_BUTTON_LEFT, &CAnykaIPCameraDlg::OnBnClickedButtonLeft)
	ON_BN_CLICKED(IDC_BUTTON_UP, &CAnykaIPCameraDlg::OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_RIGHT, &CAnykaIPCameraDlg::OnBnClickedButtonRight)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, &CAnykaIPCameraDlg::OnBnClickedButtonDown)
	ON_BN_CLICKED(IDC_BUTTON_LEFTRIGHT, &CAnykaIPCameraDlg::OnBnClickedButtonLeftRight)
	ON_BN_CLICKED(IDC_BUTTON_UPDOWN, &CAnykaIPCameraDlg::OnBnClickedButtonUpDown)
	ON_BN_CLICKED(IDC_BUTTON_REPOSITION_SET, &CAnykaIPCameraDlg::OnBnClickedButtonRepositionSet)
	ON_BN_CLICKED(IDC_BUTTON_REPOSITION, &CAnykaIPCameraDlg::OnBnClickedButtonReposition)
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_PREVIEWDLGCHOOSE_TALKOPEN, &CAnykaIPCameraDlg::OnPreviewdlgchooseTalkOpen)
	ON_COMMAND(ID_PREVIEWDLGCHOOSE1_TALKCLOSE, &CAnykaIPCameraDlg::OnPreviewdlgchooseTalkClose)
	ON_WM_KEYDOWN()
	ON_WM_SYSKEYDOWN()
	ON_WM_SIZE()
	ON_MESSAGE(WM_TALK_KICKOUT, &CAnykaIPCameraDlg::OnTalkKickOutMessage)
	ON_MESSAGE(WM_SERVER_DISCONNECT, &CAnykaIPCameraDlg::OnServerDisconnect)
	ON_COMMAND(ID_PREVIEWDLGCHOOSE1_CLOSE_PREVIEW, &CAnykaIPCameraDlg::OnPreviewdlgchoose1ClosePreview)
	ON_COMMAND(ID_PREVIEWDLGCHOOSE_CLOSE_PREVIEW, &CAnykaIPCameraDlg::OnPreviewdlgchooseClosePreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_SERVER_RET_INFO, &CAnykaIPCameraDlg::OnServerRetInfo)
	//	ON_CBN_SELCHANGE(IDC_COMBO4, &CAnykaIPCameraDlg::OnCbnSelchangeCombo4)
	ON_CBN_SELCHANGE(IDC_COMBO4, &CAnykaIPCameraDlg::OnCbnSelchangeCombo4)
	ON_BN_CLICKED(IDC_RADIO_IRCUT_ON, &CAnykaIPCameraDlg::OnBnClickedRadioIrcutOn)
	ON_BN_CLICKED(IDC_BUTTON_SET, &CAnykaIPCameraDlg::OnBnClickedButtonSet)
	ON_BN_CLICKED(IDC_RADIO_IRCUT_OFF, &CAnykaIPCameraDlg::OnBnClickedRadioIrcutOff)
	ON_BN_CLICKED(IDC_BUTTON_RECOVER_DEV, &CAnykaIPCameraDlg::OnBnClickedButtonRecoverDev)
	ON_BN_CLICKED(IDC_BUTTON_CONFIGURE, &CAnykaIPCameraDlg::OnBnClickedButtonConfigure)
	ON_BN_CLICKED(IDC_BUTTON_START, &CAnykaIPCameraDlg::OnBnClickedButtonStart)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CAnykaIPCameraDlg::OnLvnItemchangedList1)
	ON_NOTIFY(NM_SETFOCUS, IDC_LIST1, &CAnykaIPCameraDlg::OnNMSetfocusList1)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CAnykaIPCameraDlg::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_WRITE_UID, &CAnykaIPCameraDlg::OnBnClickedButtonWriteUid)
	ON_BN_CLICKED(IDC_BUTTON_PASS, &CAnykaIPCameraDlg::OnBnClickedButtonPass)
	ON_BN_CLICKED(IDC_BUTTON_FAILED, &CAnykaIPCameraDlg::OnBnClickedButtonFailed)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CAnykaIPCameraDlg::OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, &CAnykaIPCameraDlg::OnBnClickedButtonNext)
	ON_EN_CHANGE(IDC_EDIT_WIFI_NAME, &CAnykaIPCameraDlg::OnEnChangeEditWifiName)
	ON_WM_HOTKEY()
END_MESSAGE_MAP()


void CAnykaIPCameraDlg::InitToolBar()
{
	UINT nArray[8];
	nArray[0] = ID_TOOLBAR_BUTTON_PIC;
	nArray[1] = ID_TOOLBAR_BUTTON_REC;
	nArray[2] = ID_TOOLBAR_BUTTON_STOP_REC;
	nArray[3] = ID_TOOLBAR_BUTTON_PLY;
	nArray[4] = ID_TOOLBAR_BUTTON_ZIN;
	nArray[5] = ID_TOOLBAR_BUTTON_ZOUT;
	nArray[6] = ID_TOOLBAR_BUTTON_VMINUS;
	nArray[7] = ID_TOOLBAR_BUTTON_VPLUS;

	m_ToolBar.CreateEx( this, TBSTYLE_FLAT, WS_CHILD|WS_VISIBLE|CBRS_TOP );
	//m_ToolBar.SetButtons( nArray, 8 );
	//m_ToolBar.SetSizes( CSize( 48, 48 ), CSize(38, 30) );
#if 0
	m_ToolBar.SetImage("res\\5-content-picture.png");
	m_ToolBar.SetImage("res\\10-device-access-switch-video.png");
	m_ToolBar.SetImage("res\\stopRecoder.png");
	m_ToolBar.SetImage("res\\huifang.png");
	m_ToolBar.SetImage("res\\+.png");
	m_ToolBar.SetImage("res\\-.png");
	m_ToolBar.SetImage("res\\yinliangjian.png");
	m_ToolBar.SetImage("res\\yinliangjia.png", true);
	m_ToolBar.SetButtonText( 0, L"����" );
	m_ToolBar.SetButtonText( 1, L"¼��" );
	m_ToolBar.SetButtonText( 2, L"ֹͣ¼��" );
	m_ToolBar.SetButtonText( 3, L"�ط�" );
	m_ToolBar.SetButtonText( 4, L"�Ŵ�" );
	m_ToolBar.SetButtonText( 5, L"��С" );
	m_ToolBar.SetButtonText( 6, L"������" );
	m_ToolBar.SetButtonText( 7, L"������" );
#endif

	RepositionBars( AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0 );
}

void CAnykaIPCameraDlg::InitStatusBar()
{
	m_StatusBar.Create(this);

	UINT nArray[3] = {ID_STATUSBAR_PROGRAMINFO, ID_STATUSBAR_DISPLAYINFO1, ID_STATUSBAR_DISPLAYINFO2};
	m_StatusBar.SetIndicators(nArray, sizeof(nArray) / sizeof(nArray[0]));

	CRect rect;
	GetWindowRect(&rect);

	//m_StatusBar.SetPaneInfo(0, ID_STATUSBAR_PROGRAMINFO,  0, rect.Width() * SBAR_PROGRAMINFO_SCALE);
	//m_StatusBar.SetPaneInfo(1, ID_STATUSBAR_DISPLAYINFO1, 0, rect.Width() * SBAR_DISPLAYINFO_SCALE);
	//m_StatusBar.SetPaneInfo(2, ID_STATUSBAR_DISPLAYINFO2, 0, rect.Width() * SBAR_DISPLAYINFO_SCALE);

	//m_StatusBar.SetPaneText( 0, L"Anyka IP Camera!");

	//RepositionBars( AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0 );
}

void CAnykaIPCameraDlg::InitTreeCtrlPosition()
{
	CRect cToolBarRect, cStatusBarRect, cWindowRect;

	GetWindowRect(&cWindowRect);
	m_ToolBar.GetWindowRect(&cToolBarRect);
	m_StatusBar.GetWindowRect(&cStatusBarRect);

	ScreenToClient(&cToolBarRect);
	ScreenToClient(&cStatusBarRect);

	//m_TreeCtrl.MoveWindow( cToolBarRect.left + 2, cToolBarRect.bottom + 3, 
	//	cWindowRect.Width() / 5, cStatusBarRect.top - cToolBarRect.bottom - 4 );
	m_TreeCtrl.MoveWindow( cToolBarRect.left + 2, cToolBarRect.bottom + 3, 
		0, cStatusBarRect.top - cToolBarRect.bottom - 4 );
}

void CAnykaIPCameraDlg::InitPreviewWindows(BOOL bNeedCreate, BOOL full_flag)
{
	CRect cTreeCtrlRect, cToolBarRect, cWindowRect;

	//m_TreeCtrl.GetWindowRect( &cTreeCtrlRect );
	//m_ToolBar.GetWindowRect( &cToolBarRect );
	GetWindowRect(&cWindowRect);

	//ScreenToClient(&cTreeCtrlRect);
	//ScreenToClient(&cToolBarRect);
	ScreenToClient(&cWindowRect);



	if (bNeedCreate)
	{
		m_Preview[0].Create( IDD_DIALOG_PREVIEW1, this );
		m_Preview[1].Create( IDD_DIALOG_PREVIEW1, this );
	}
#if 0
	//m_Preview[0].MoveWindow( cTreeCtrlRect.right + 3, cToolBarRect.bottom + 3, ( cTreeCtrlRect.Height() * 8 ) / 9, cTreeCtrlRect.Height() / 2 );
	if (full_flag)
	{
		//m_Preview[0].MoveWindow( cTreeCtrlRect.right, cToolBarRect.bottom, 700, 400);//471, 265);
		//m_Preview[0].MoveWindow( cTreeCtrlRect.right, cToolBarRect.bottom, 600, 450);//471, 265);
		m_Preview[0].MoveWindow( cWindowRect.top+39, cWindowRect.left+3, cWindowRect.right, cWindowRect.bottom);

	}
	else
	{
		//m_Preview[0].MoveWindow( cTreeCtrlRect.right, cToolBarRect.bottom, 1000, 700);//471, 265);
		//m_Preview[0].MoveWindow( cTreeCtrlRect.right, cToolBarRect.bottom, 1024, 768);//471, 265);
		m_Preview[0].MoveWindow( cWindowRect.top+39, cWindowRect.left+3, cWindowRect.right+cWindowRect.left, cWindowRect.bottom);
	}
#endif

	//m_Preview[0].MoveWindow( 0,0, 600, 500);
	CRect rcDlg1,rcDlg2,rcDlg3;
	GetClientRect(&rcDlg1);
	m_RightDlg.GetClientRect(&rcDlg2);
	m_BottomDlg.GetClientRect(&rcDlg3);
	m_Preview[0].SetWindowPos(NULL,0,0,rcDlg1.Width()-rcDlg2.Width()-10,
		rcDlg1.Height()-rcDlg3.Height()-10,SWP_NOMOVE|SWP_SHOWWINDOW);
	m_Preview[1].SetWindowPos(NULL,0,0,rcDlg1.Width()-rcDlg2.Width()-10,
		rcDlg1.Height()-rcDlg3.Height()-10,SWP_NOMOVE|SWP_SHOWWINDOW);


	//m_Preview[0].MoveWindow( cWindowRect.top+3, cWindowRect.left+3, cWindowRect.right, cWindowRect.bottom);

	//m_Preview[1].MoveWindow( cTreeCtrlRect.right + 3, cToolBarRect.bottom + 3 + cTreeCtrlRect.Height() / 2, ( cTreeCtrlRect.Height() * 8 ) / 9, cTreeCtrlRect.Height() / 2 );

	if (bNeedCreate){
		m_Preview[0].ShowWindow( SW_SHOW );
		m_Preview[1].ShowWindow( SW_SHOW );
	}
}

void CAnykaIPCameraDlg::InitComboBox()
{
	m_ContrastCombo.SelectString(3, L"3");
	m_SaturationCombo.SelectString(3, L"3");
	m_BrightnessCombo.SelectString(3, L"3");
	m_acutanceCom.SelectString(3, L"3");
}

void CAnykaIPCameraDlg::UpdateCombo()
{
	if (NULL == m_hCurrentSelectItem) return;

	IServer * pIServer = (IServer *)m_TreeCtrl.GetItemData(m_hCurrentSelectItem);
	if (NULL == pIServer) {
		AfxMessageBox(L"�޷���ȡ�豸...�����ڲ�����", 0, 0);
		return;
	}

	IMAGE_SET stImageSet;
	ZeroMemory(&stImageSet, sizeof(IMAGE_SET));
	pIServer->GetServerImageSet(stImageSet);

	CString strTemp;
	strTemp.Format(L"%d", stImageSet.nContrast);
	m_ContrastCombo.SelectString(0,strTemp);

	strTemp.Format(L"%d", stImageSet.nBrightness);
	m_BrightnessCombo.SelectString(0,strTemp);

	strTemp.Format(L"%d", stImageSet.nSaturation);
	m_SaturationCombo.SelectString(0,strTemp);

	strTemp.Format(L"%d", stImageSet.nSaturation);
	m_acutanceCom.SelectString(0, strTemp);
}

void CAnykaIPCameraDlg::InitPrivacyDialog()
{
	return;
}

// CAnykaIPCameraDlg ��Ϣ�������

static int av_lock_manager_cb(void ** mutex, enum AVLockOp lockOp)
{
	switch(lockOp) {
case AV_LOCK_CREATE:
	*mutex = (void*)CreateMutex(NULL, false, NULL);
	break;
case AV_LOCK_DESTROY:
	CloseHandle((HANDLE)*mutex);
	*mutex = NULL;
	break;
case AV_LOCK_OBTAIN:
	WaitForSingleObject((HANDLE)*mutex, INFINITE);
	break;
case AV_LOCK_RELEASE:
	ReleaseMutex((HANDLE)*mutex);
	break;
	}

	return 0;
}

void CAnykaIPCameraDlg::OnClientFinish(void * pLParam, void * pRParam)
{
	CAnykaIPCameraDlg * pthis = (CAnykaIPCameraDlg *)pLParam;
	CAimer39RTSPClient * pClient = (CAimer39RTSPClient *)pRParam;

	int iSelect = 0;
	for (; iSelect < 2; ++iSelect)
		if (pthis->m_pClient[iSelect] == pClient) break;

	if (iSelect >= 2) {
		AfxMessageBox( L"WARN! a client no under control finish\n", 0, 0 );
		return;
	}

	//pthis->m_Preview[iSelect].Invalidate();
}

void CAnykaIPCameraDlg::OnClientDisConnect(void * pLParam, void * pRParam)
{
	CAnykaIPCameraDlg * pthis = (CAnykaIPCameraDlg *)pLParam;
	CAimer39RTSPClient * pClient = (CAimer39RTSPClient *)pRParam;

#ifdef WARN_ERROR_OUT
	fprintf(stderr, 
		"WARN::####Disconnet we will do the reconnect operate, because we didn't rec any video data in last 4 second####\n");
#endif

	int iSelect = 0;
	for (; iSelect < SUPPORT_STREAM_CNT; ++iSelect)
		if (pthis->m_pClient[iSelect] == pClient) break;

	if (iSelect >= SUPPORT_STREAM_CNT) {
		AfxMessageBox( L"WARN! a client no under control disconnect\n", 0, 0 );
		return;
	}

	//pthis->m_Preview[iSelect].Invalidate();//ˢ��Preview Dialog
	if (pthis->m_pServerPreviews[iSelect])
	{
		pthis->m_pServerPreviews[iSelect]->DisConnect(); //ʹ��������Monitor�̣߳��������Ĺ�����

	}
	//pthis->CloseServer();
	g_start_open_flag = TRUE;
	//g_connet_flag = FALSE;

}

void CAnykaIPCameraDlg::OnFullScreenMessage(UINT message, WPARAM wParam, LPARAM lParam, void * pClassParam)
{
#if 0
	CAnykaIPCameraDlg * pthis = (CAnykaIPCameraDlg *)pClassParam;

	if (message == WM_LBUTTONDBLCLK ||
		(message == WM_KEYUP && wParam == VK_ESCAPE)) {
			pthis->FullScreenProcess(FALSE, pthis->m_nVideoFullScreenIndex);
	}
#endif
}

void CAnykaIPCameraDlg::OnTalkKickOut(IServer * pIServer,
									  unsigned long ulIpAddr, unsigned short usPort, void * pClassParam)
{
	CAnykaIPCameraDlg * pthis = (CAnykaIPCameraDlg *)pClassParam;

	{
		CAutoLock lock(&(pthis->m_csForKickOut));
		pthis->m_stKickOutParam.ulIpAddr = ulIpAddr;
		pthis->m_stKickOutParam.ulPort = usPort;
	}

	pthis->PostMessage(WM_TALK_KICKOUT, 0, (LPARAM)pIServer);
}

void CAnykaIPCameraDlg::OnServerReturnInfo(IServer * pIServer, RETINFO * pstRetInfo, void * pClassParam)
{
	CAnykaIPCameraDlg * pthis = (CAnykaIPCameraDlg *)pClassParam;

	{
		CAutoLock lock(&(pthis->m_csForRet));
		memcpy(&(pthis->m_stRetInfo), pstRetInfo, sizeof(RETINFO));
	}

	pthis->PostMessage(WM_SERVER_RET_INFO, 0, (LPARAM)pIServer);
}


BOOL CAnykaIPCameraDlg::OnInitDialog()
{
	TCHAR temp_buf_det[MAX_PATH] = {0};
	TCHAR temp_buf_src[MAX_PATH] = {0};
	TCHAR bufsprintf[MAX_PATH] = {0};
	CString str;


	USES_CONVERSION;

	CDialog::OnInitDialog();
	//m_Status.Set


	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	ListView_SetExtendedListViewStyle(m_test_config.m_hWnd, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);



	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	time_t t = time(0);
	struct tm * ptNow = NULL;

	char logInfoName[MAX_PATH] = {0};

	ptNow = localtime(&t);

	CreateDirectory(ConvertAbsolutePath(_T("log")), NULL);//�����ļ���
	sprintf(logInfoName, "log\\info_%04d_%02d_%02d,%02d,%02d,%02d.log", 
		ptNow->tm_year + 1900, ptNow->tm_mon + 1, ptNow->tm_mday,
		ptNow->tm_hour, ptNow->tm_min, ptNow->tm_sec);

#ifdef USE_LOG_FILE
	freopen(logInfoName, "w+t", stderr);
#else
	AllocConsole();
	freopen("CONOUT$", "w+t", stdout);
	freopen("CONIN$", "r+t", stdin);
	freopen("CONOUT$", "w+t", stderr);
#endif

	frmLogfile.SetFileName(_T("log\\test_case_info.txt"));
	frmLogfile.CheckFileSize(100*1024*1024);  // delete the log file if it is larger than 512K
	frmLogfile.InitFile();
	//frmLogfile.WriteLogFile(0,"*********************************************************************************\r\n");
	//frmLogfile.WriteLogFile(LOG_LINE_TIME | LOG_LINE_DATE,"Open ipc_test.exe\r\n");


	frmLogUidFile.SetFileName(_T("log\\test_uid_info.txt"));
	frmLogUidFile.CheckFileSize(100*1024*1024);  // delete the log file if it is larger than 512K
	frmLogUidFile.InitFile();
	frmLogUidFile.WriteLogFile(0,"*********************************************************************************\r\n");
	frmLogUidFile.WriteLogFile(LOG_LINE_TIME | LOG_LINE_DATE,"\r\n");


	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	first_flag = FALSE;
	m_Burn_UIdDlg.UID_first_flag = FALSE;
	start_test  = FALSE;
	start_flag  = FALSE;



	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	CClientDC dc(this);
	font.CreatePointFont(120,_T("����"),NULL);
	CFont* def_font=dc.SelectObject(&font);
	dc.SelectObject(def_font);
	//font.DeleteObject();

	//CClientDC dc(this);
	oldFont.CreatePointFont(90,_T("����"),NULL);
	//CFont* def_font=dc.SelectObject(&oldFont);
	//dc.SelectObject(def_font);


	/////////////////////////////////////////////
	CRect rcDlg1,rcDlg2;
	GetClientRect(&rcDlg1);
	m_RightDlg.Create(IDD_FORMVIEW,this);
	m_RightDlg.GetClientRect(&rcDlg2);
	m_RightDlg.SetWindowPos(NULL,rcDlg1.Width()-rcDlg2.Width(),0,0,0,SWP_NOSIZE|SWP_SHOWWINDOW);

	m_BottomDlg.Create(IDD_BOTTOMVIEW,this);
	m_BottomDlg.GetClientRect(&rcDlg2);
	m_BottomDlg.SetWindowPos(NULL,0,rcDlg1.Height()-rcDlg2.Height(),0,0,SWP_NOSIZE|SWP_SHOWWINDOW);


	m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//����
	m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����
	//m_BottomDlg.GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(FALSE);//����
	m_BottomDlg.GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(FALSE);//����
	m_BottomDlg.GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(FALSE);//����
	ListView_SetExtendedListViewStyle(m_RightDlg.m_test_config.m_hWnd, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);


	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	avcodec_register_all();
	av_lockmgr_register(av_lock_manager_cb);

	//start the monitor thread
	m_runThreadFlag = TRUE;
	m_bNeedJudgeDisConnWork = TRUE;
	m_MonitorThread = (HANDLE)_beginthreadex(NULL, THREAD_STACK_SIZE, thread_begin, (LPVOID)this, 0, NULL);

	m_menuTalk.LoadMenu(IDR_MENU4);

	m_ircut_flag = 1;
	((CButton *)GetDlgItem(IDC_RADIO_IRCUT_ON))->SetCheck(1);
	((CButton *)GetDlgItem(IDC_RADIO_IRCUT_OFF))->SetCheck(0);

	m_uPort = 21;
	m_net_uPort = 6789;
	m_username = (_T(""));   
	m_password = (_T(""));
	download_file_flag = FALSE;
	download_dev_file_flag = FALSE;
	g_send_commad = 0;
	g_test_fail_flag  = 0;
	g_connet_flag = FALSE;
	g_senddata_flag = TRUE;
	memset(m_connect_ip, 0, MAX_PATH);
	GetDlgItem(IDC_BUTTON_SET)->EnableWindow(FALSE);//���� 
	GetDlgItem(IDC_BUTTON_RECOVER_DEV)->EnableWindow(FALSE);//���� 

	//�ж������ļ����Ƿ���ڣ������������ô�ͽ��д���
	if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(TEST_CONFIG_DIR)))
	{
		CreateDirectory(ConvertAbsolutePath(TEST_CONFIG_DIR), NULL);//�����ļ���
	}

	GetDlgItem(IDC_EDIT_WIFI_NAME)->SetWindowText(g_test_config.m_wifi_name);
	m_BottomDlg.GetDlgItem(IDC_EDIT_WIFI_NAME)->SetWindowText(g_test_config.newest_version);
	//m_enter_test_config.GetDlgItem(IDC_TEST_TIME_MONITOR)->SetWindowText(g_test_config.m_wifi_name);
	//m_enter_test_config.GetDlgItem(IDC_TEST_TIME_RESET)->SetWindowText(g_test_config.m_wifi_name);

	str.Format(_T("UID"));//
	m_RightDlg.m_test_config.InsertColumn(0, str, LVCFMT_LEFT, 100);

	str.Format(_T("IP"));//
	m_RightDlg.m_test_config.InsertColumn(1, str, LVCFMT_LEFT, 80);

	str.Format(_T("�豸����"));//
	m_RightDlg.m_test_config.InsertColumn(2, str, LVCFMT_LEFT, 100);

	_tcscpy(temp_buf_src, ConvertAbsolutePath(_T("test_ircut")));
	str.Format(_T("%s/test_ircut"), ConvertAbsolutePath(TEST_CONFIG_DIR));
	_tcscpy(temp_buf_det, str);
	//str.Format(_T("AAA"));
	//	AfxMessageBox(str,  MB_OK);
	if (!CopyFile(temp_buf_src, temp_buf_det, FALSE))
	{
		//str.Format(bufsprintf, _T("copy  fail :%s"), ConvertAbsolutePath(_T("test_ircut")));
		//AfxMessageBox(bufsprintf, MB_OK);
		//return FALSE;
	}
	else
	{
		memset(temp_buf_det, 0, MAX_PATH);
		//sprintf(temp_buf_det, _T("%s"), ConvertAbsolutePath(_T("test_ircut")));
		_tcscpy(temp_buf_det, ConvertAbsolutePath(_T("test_ircut")));
		DeleteFile(temp_buf_det);
	}

	memset(temp_buf_det, 0, MAX_PATH);
	memset(temp_buf_src, 0, MAX_PATH);
	//str.Format(_T("BBB"));
	//AfxMessageBox(str,  MB_OK);

	_tcscpy(temp_buf_src, ConvertAbsolutePath(_T("test_recover_dev")));
	str.Format(_T("%s/test_recover_dev"), ConvertAbsolutePath(TEST_CONFIG_DIR));
	_tcscpy(temp_buf_det, str);
	if (!CopyFile(temp_buf_src, temp_buf_det, FALSE))
	{
		//str.Format(bufsprintf, _T("copy fail:%s"), ConvertAbsolutePath(_T("test_recover_dev")));
		//AfxMessageBox(bufsprintf, MB_OK);
	}
	else
	{
		memset(temp_buf_det, 0, MAX_PATH);
		//sprintf(temp_buf_det, _T("%s"), ConvertAbsolutePath(_T("test_recover_dev")));
		_tcscpy(temp_buf_det, ConvertAbsolutePath(_T("test_recover_dev")));
		DeleteFile(temp_buf_det);
	}



	//InitToolBar();
	//InitStatusBar();

	//InitTreeCtrlPosition();
	//HTREEITEM hRoot = m_TreeCtrl.InsertItem( TREE_ROOT_ITEM_NAME, TVI_ROOT, TVI_LAST );

	InitPreviewWindows(TRUE, TRUE);
	//InitComboBox();
	InitPrivacyDialog();
	//PositionTheButton();
	//PositionTheImageCombo();

	SetTimer(TIMER_COMMAND, 1000, NULL);

	m_bIsInit = TRUE;
	g_start_open_flag = TRUE;

	if (_tcscmp(g_test_config.m_mac_start_addr, g_test_config.m_mac_current_addr) > 0)
	{
		_tcsncpy(g_test_config.m_mac_current_addr, 
			g_test_config.m_mac_start_addr, _tcsclen(g_test_config.m_mac_start_addr));
	}


	Creat_find_ip_thread();


	Creat_Anyka_Test_thread();

	//read_config(CONFIG_PATH);

	SetWindowText(TOOL_VERSOIN);
	BOOL bRet;
	//��ϵͳע���ȼ�
	bRet=RegisterHotKey(m_hWnd,1001,0,VK_F1);
	bRet=RegisterHotKey(m_hWnd,1002,0,VK_F2);
	bRet=RegisterHotKey(m_hWnd,1003,0,VK_F3);
	bRet=RegisterHotKey(m_hWnd,1004,0,VK_F4);
	bRet=RegisterHotKey(m_hWnd,1005,0,VK_F6);
	bRet=RegisterHotKey(m_hWnd,1006,0,VK_F7);
	bRet=RegisterHotKey(m_hWnd,1009,0,'Y');
	bRet=RegisterHotKey(m_hWnd,1010,0,'y');
	bRet=RegisterHotKey(m_hWnd,1012,0,VK_SPACE);


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

BOOL CAnykaIPCameraDlg::PreTranslateMessage(MSG * pMsg)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (pMsg->message == WM_KEYDOWN)
	{
		switch(pMsg->wParam) 
		{
		case VK_RETURN:
			return TRUE;
		case VK_ESCAPE:
			return TRUE;
		}
	}

	//������̨��ť�������¼����˴�����������ǽ��������̨��ť�ϵĲ���ת���������ڴ��������ĺô����ǲ���Ҫ���¼̳�CButton������ʵ������ɡ�
	if ((pMsg->message == WM_LBUTTONDOWN) || (pMsg->message == WM_LBUTTONUP) || (pMsg->message == WM_MOUSEMOVE)) {
		CWnd * pButtonLeft = GetDlgItem(IDC_BUTTON_LEFT);
		CWnd * pButtonRight = GetDlgItem(IDC_BUTTON_RIGHT);
		CWnd * pButtonUp = GetDlgItem(IDC_BUTTON_UP);
		CWnd * pButtonDown = GetDlgItem(IDC_BUTTON_DOWN);

		HWND hLeftWnd = pButtonLeft->GetSafeHwnd();
		HWND hRightWnd = pButtonRight->GetSafeHwnd();
		HWND hUpWnd = pButtonUp->GetSafeHwnd();
		HWND hDownWnd = pButtonDown->GetSafeHwnd();

		if ((pMsg->hwnd == hLeftWnd) || (pMsg->hwnd == hRightWnd) || (pMsg->hwnd == hUpWnd) || (pMsg->hwnd == hDownWnd)) {
			POINT point = {0, 0};
			point.x = GET_X_LPARAM(pMsg->lParam);
			point.y = GET_Y_LPARAM(pMsg->lParam);

			::ClientToScreen(pMsg->hwnd, &point);
			ScreenToClient(&point);
			SendMessage(pMsg->message, pMsg->wParam, MAKELPARAM(point.x, point.y));
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CAnykaIPCameraDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CAnykaIPCameraDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1)/ 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}

	m_Preview[0].UpdateWindow();
	m_Preview[1].UpdateWindow();

	CRect rcDlg1,rcDlg2,rcDlg3;
	GetClientRect(&rcDlg1);
	m_RightDlg.GetClientRect(&rcDlg2);
	m_RightDlg.SetWindowPos(NULL,rcDlg1.Width()-rcDlg2.Width(),0,0,0,SWP_NOSIZE|SWP_SHOWWINDOW);

	m_BottomDlg.GetClientRect(&rcDlg3);
	m_BottomDlg.SetWindowPos(NULL,0,rcDlg1.Height()-rcDlg3.Height(),0,0,SWP_NOSIZE|SWP_SHOWWINDOW);

	m_RightDlg.UpdateWindow();
	m_BottomDlg.UpdateWindow();

}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CAnykaIPCameraDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAnykaIPCameraDlg::VideoFunctionOpenProcess(VIDEOFUNCTION enVFun)
{
	int nCount = m_Search.GetServerCount();
	if (nCount <= 0) {
		AfxMessageBox(L"��ǰû���������κη���������û�н�����������!��ȷ�������д��ڷ������������������.", 0 ,0 );
		return;
	}

	if (!CanDoTheJob()) {
		AfxMessageBox(L"��ǰѡ����豸û�н���Ԥ������ѡ��һ������Ԥ��״̬���豸���п���", 0 ,0 );
		return;
	}

	IServer * pIServer = NULL;
	char strIPAddr[MAX_IP_LEN] = {0};
	unsigned int nLen = MAX_IP_LEN;

	m_bNeedJudgeDisConnWork = FALSE;
	CAutoLock lock(&m_csForServerConnect);
	::SuspendThread(m_MonitorThread);

	IServer * pCurIServer = (IServer *)m_TreeCtrl.GetItemData(m_hCurrentSelectItem);

	if (enVFun == VF_PLAY) {
		m_RecordPlayDlg.PutServerEntry(pCurIServer);
	}

	for (int i = 0; i < nCount; ++i) {
		m_Search.GetServer(i, &pIServer);
		pIServer->GetServerIp(strIPAddr, &nLen);

		if (enVFun == VF_PRIVACY_AREA)
			m_PrivacyDialog.PutServerEntry(strIPAddr, pIServer);
		else if (enVFun == VF_MOTION_DETECT)
			m_MotionDetectDlg.PutServerEntry(strIPAddr, pIServer);
	}

	for (int i = 0; i < SUPPORT_STREAM_CNT; ++i) {
		CloseTheStream(i, TRUE);
	}

	IServer * apTempServer[PREVIEW_WINDOWS] = {NULL};

	for (int i = 0; i < PREVIEW_WINDOWS; ++i) {
		if (m_pServerPreviews[i]) {
			m_pServerPreviews[i]->DisConnect();
			apTempServer[i] = m_pServerPreviews[i];
			m_pServerPreviews[i] = NULL;
		}
	}

	if (enVFun == VF_PRIVACY_AREA)
		m_PrivacyDialog.DoModal();
	else if (enVFun == VF_MOTION_DETECT)
		m_MotionDetectDlg.DoModal();
	else if (enVFun == VF_PLAY)
		m_RecordPlayDlg.DoModal();
	else return;

	for (int i = 0; i < PREVIEW_WINDOWS; ++i) {
		if (apTempServer[i]){
			m_pServerPreviews[i] = apTempServer[i];
		}
	}

	//�����������ܣ�������´�Ԥ�����Ĳ�����
	::ResumeThread(m_MonitorThread);
	m_bNeedJudgeDisConnWork = TRUE;
}

void CAnykaIPCameraDlg::OnPrivacyArea()
{
	// TODO: �ڴ���������������
	VideoFunctionOpenProcess(VF_PRIVACY_AREA);
}

void CAnykaIPCameraDlg::OnMotionDetect()
{
	// TODO: �ڴ���������������
	//m_MotionDetectDlg.DoModal();
	VideoFunctionOpenProcess(VF_MOTION_DETECT);
}

void CAnykaIPCameraDlg::OnPicture()
{
	// TODO: �ڴ���������������
	if (!m_hCurrentSelectItem) {
		AfxMessageBox( L"δѡ���κ��豸�������豸�б���ѡ��һ���豸!", 0, 0 );
		return;
	}

	if (!CanDoTheJob()) {
		AfxMessageBox(L"��ǰѡ����豸û�н���Ԥ������ѡ��һ������Ԥ��״̬���豸���п���", 0 ,0 );
		return;
	}

	IServer * pIServer = (IServer *)m_TreeCtrl.GetItemData(m_hCurrentSelectItem);
	if (NULL == pIServer) {
		AfxMessageBox(L"�޷���ȡ�豸...�����ڲ�����", 0, 0);
		return;
	}

	pIServer->SendTakePic();
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_BUTTON_PIC, FALSE);
	m_bPicture = FALSE;
}

void CAnykaIPCameraDlg::OnPlay()
{	
	VideoFunctionOpenProcess(VF_PLAY);
}

void CAnykaIPCameraDlg::OnRecord()
{
	// TODO: �ڴ���������������
	if (!m_hCurrentSelectItem) {
		AfxMessageBox( L"δѡ���κ��豸�������豸�б���ѡ��һ���豸!", 0, 0 );
		return;
	}

	if (!CanDoTheJob()) {
		AfxMessageBox(L"��ǰѡ����豸û�н���Ԥ������ѡ��һ������Ԥ��״̬���豸���п���", 0 ,0 );
		return;
	}

	IServer * pIServer = (IServer *)m_TreeCtrl.GetItemData(m_hCurrentSelectItem);
	if (NULL == pIServer) {
		AfxMessageBox(L"�޷���ȡ�豸...�����ڲ�����", 0, 0);
		return;
	}

	pIServer->SendRecode();
}

void CAnykaIPCameraDlg::OnStopRecord()
{
	// TODO: �ڴ���������������
	if (!m_hCurrentSelectItem) {
		AfxMessageBox( L"δѡ���κ��豸�������豸�б���ѡ��һ���豸!", 0, 0 );
		return;
	}

	if (!CanDoTheJob()) {
		AfxMessageBox(L"��ǰѡ����豸û�н���Ԥ������ѡ��һ������Ԥ��״̬���豸���п���", 0 ,0 );
		return;
	}

	IServer * pIServer = (IServer *)m_TreeCtrl.GetItemData(m_hCurrentSelectItem);
	if (NULL == pIServer) {
		AfxMessageBox(L"�޷���ȡ�豸...�����ڲ�����", 0, 0);
		return;
	}

	pIServer->SendStopRecode();
}

void CAnykaIPCameraDlg::OnZoomIn()
{
	// TODO: �ڴ���������������
	if (!m_hCurrentSelectItem) {
		AfxMessageBox( L"δѡ���κ��豸�������豸�б���ѡ��һ���豸!", 0, 0 );
		return;
	}

	if (!CanDoTheJob()) {
		AfxMessageBox(L"��ǰѡ����豸û�н���Ԥ������ѡ��һ������Ԥ��״̬���豸���п���", 0 ,0 );
		return;
	}

	IServer * pIServer = (IServer *)m_TreeCtrl.GetItemData(m_hCurrentSelectItem);
	if (NULL == pIServer) {
		AfxMessageBox(L"�޷���ȡ�豸...�����ڲ�����", 0, 0);
		return;
	}

	HTREEITEM hSelectItem = m_TreeCtrl.GetSelectedItem();
	if (m_TreeCtrl.GetChildItem(hSelectItem)) {
		AfxMessageBox(L"��ǰ���豸�б���ѡ�������������ѡ���豸�µ���һ��������Zoom In/out������", 0, 0);
		return;
	}

	unsigned int iStreamSelect = (unsigned int)m_TreeCtrl.GetItemData(hSelectItem);
	ZOOM Zoom = iStreamSelect << 1;
	Zoom |= ZOOM_IN;

	pIServer->SendZoomInOut(Zoom);
}

void CAnykaIPCameraDlg::OnZoomOut()
{
	// TODO: �ڴ���������������
	if (!m_hCurrentSelectItem) {
		AfxMessageBox( L"δѡ���κ��豸�������豸�б���ѡ��һ���豸!", 0, 0 );
		return;
	}

	if (!CanDoTheJob()) {
		AfxMessageBox(L"��ǰѡ����豸û�н���Ԥ������ѡ��һ������Ԥ��״̬���豸���п���", 0 ,0 );
		return;
	}

	IServer * pIServer = (IServer *)m_TreeCtrl.GetItemData(m_hCurrentSelectItem);
	if (NULL == pIServer) {
		AfxMessageBox(L"�޷���ȡ�豸...�����ڲ�����", 0, 0);
		return;
	}

	HTREEITEM hSelectItem = m_TreeCtrl.GetSelectedItem();
	if (m_TreeCtrl.GetChildItem(hSelectItem)) {
		AfxMessageBox(L"��ǰ���豸�б���ѡ�������������ѡ���豸�µ���һ��������Zoom In/out������", 0, 0);
		return;
	}

	unsigned int iStreamSelect = (unsigned int)m_TreeCtrl.GetItemData(hSelectItem);
	ZOOM Zoom = iStreamSelect << 1;
	Zoom |= ZOOM_OUT;

	pIServer->SendZoomInOut(Zoom);
}

void CAnykaIPCameraDlg::OnVolumeMinus()
{
	// TODO: �ڴ���������������
	IServer * pIServer = NULL;

	if (m_nAudioClientIndex < 0) {
		if (!m_hCurrentSelectItem) {
			AfxMessageBox( L"δѡ���κ��豸�������豸�б���ѡ��һ���豸!", 0, 0 );
			return;
		}

		if (!CanDoTheJob()) {
			AfxMessageBox(L"��ǰѡ����豸û�н���Ԥ������ѡ��һ������Ԥ��״̬���豸���п���", 0 ,0 );
			return;
		}

		pIServer = (IServer *)m_TreeCtrl.GetItemData(m_hCurrentSelectItem);
	}else {
		pIServer = m_pServerPreviews[m_nAudioClientIndex];
	}

	if (NULL == pIServer) {
		AfxMessageBox(L"�޷���ȡ�豸...�����ڲ�����", 0, 0);
		return;
	}

	pIServer->SendVolumeCtrl(VOLUME_MINUS);
}

void CAnykaIPCameraDlg::OnVolumePlus()
{
	// TODO: �ڴ���������������
	IServer * pIServer = NULL;

	if (m_nAudioClientIndex < 0) {
		if (!m_hCurrentSelectItem) {
			AfxMessageBox( L"δѡ���κ��豸�������豸�б���ѡ��һ���豸!", 0, 0 );
			return;
		}

		if (!CanDoTheJob()) {
			AfxMessageBox(L"��ǰѡ����豸û�н���Ԥ������ѡ��һ������Ԥ��״̬���豸���п���", 0 ,0 );
			return;
		}

		pIServer = (IServer *)m_TreeCtrl.GetItemData(m_hCurrentSelectItem);
	}else {
		pIServer = m_pServerPreviews[m_nAudioClientIndex];
	}

	if (NULL == pIServer) {
		AfxMessageBox(L"�޷���ȡ�豸...�����ڲ�����", 0, 0);
		return;
	}

	pIServer->SendVolumeCtrl(VOLUME_PLUS);
}

void CAnykaIPCameraDlg::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strText = m_TreeCtrl.GetItemText(pNMTreeView->itemNew.hItem);
	if (m_TreeCtrl.GetChildItem(pNMTreeView->itemNew.hItem)) {
		if (m_TreeCtrl.GetParentItem(pNMTreeView->itemNew.hItem)) {
			m_hCurrentSelectItem = pNMTreeView->itemNew.hItem;
			UpdateCombo();
		}
	}else {
		m_hCurrentSelectItem = m_TreeCtrl.GetParentItem(pNMTreeView->itemNew.hItem);
		UpdateCombo();
	}

	*pResult = 0;
}

void CAnykaIPCameraDlg::OnNMRClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
#if 0
	POINT pos;
	*pResult = -1;

	if ( !GetCursorPos( &pos ) )
		return;

	m_TreeCtrl.ScreenToClient( &pos );

	UINT nFlag;
	HTREEITEM hItem = m_TreeCtrl.HitTest( pos, &nFlag );
	int MenuID = 0;

	if ( ( hItem != NULL ) && ( TVHT_ONITEM & nFlag ) ) {
		m_TreeCtrl.Select( hItem, TVGN_CARET );
		if (m_TreeCtrl.GetChildItem(hItem)) {
			MenuID = IDR_MENU2;
			if (HTREEITEM hParent = m_TreeCtrl.GetParentItem(hItem)) {
				m_hCurrentSelectItem = hItem;
				UpdateCombo();
			}
		}
		else{
			if (TREE_ROOT_ITEM_NAME == m_TreeCtrl.GetItemText(hItem))
				MenuID = IDR_MENU2;
			else {
				MenuID = IDR_MENU3;
				m_hCurrentSelectItem = m_TreeCtrl.GetParentItem(hItem);
				UpdateCombo();
			}
		}
	}else {
		MenuID = IDR_MENU2;
	}

	CMenu menu, *pm;
	if (!menu.LoadMenu(MenuID)) {
		AfxMessageBox( L"�޷����ز˵���\n", 0, 0 );
		return;
	}

	pm = menu.GetSubMenu(0);
	GetCursorPos( &pos );
	pm->TrackPopupMenu(TPM_LEFTALIGN, pos.x, pos.y, this);
#endif
	*pResult = 0;

}

void CAnykaIPCameraDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_runThreadFlag = FALSE;
	WiatForMonitorThreadEnd();

	for (int i = 0; i < 2; ++i) {
		CloseTheStream(i, TRUE);
	}

	CoUninitialize();

	av_lockmgr_register(NULL);
	KillTimer(TIMER_COMMAND);

	WSACleanup();

#ifndef USE_LOG_FILE
	FreeConsole();
#endif

	CDialog::OnClose();
}

void CAnykaIPCameraDlg::OnSearchDevice()
{
	// TODO: �ڴ���������������
	m_TreeCtrl.DeleteAllItems();

	HTREEITEM hRoot = m_TreeCtrl.InsertItem( TREE_ROOT_ITEM_NAME, TVI_ROOT, TVI_LAST );
	m_hCurrentSelectItem = NULL;

	if (m_nAudioClientIndex != -1){
		OnPreviewdlgchooseTalkClose();
	}

	{
		CAutoLock lock(&m_csForServerConnect);
		m_Search.DeleteAllServer();
		ZeroMemory(m_pServerPreviews, PREVIEW_WINDOWS * sizeof(IServer*));
	}

	for (int i = 0; i < SUPPORT_STREAM_CNT; ++i) {
		CloseTheStream(i, TRUE);
	}

	for (int i = 0; i < PREVIEW_WINDOWS; ++i) m_strURL[i].clear();

	m_nAudioClientIndex = -1;

	m_Search.Search();
	m_bIsSearch = TRUE;
}

#define DEVICE_PREFIX	L"�豸%d:%s(%s)"




TCHAR *CAnykaIPCameraDlg::ConvertAbsolutePath(LPCTSTR path)
{
	CString sPath;
	CString filePath;

	if (path[0] == '\0')
	{
		return NULL;
	}
	else if ((':' == path[1]) || (('\\'==path[0]) && ('\\'==path[1])))
	{
		_tcsncpy(m_path, path, MAX_PATH);
	}
	else
	{
		GetModuleFileName(NULL,sPath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);

		sPath.ReleaseBuffer ();
		int nPos;
		nPos=sPath.ReverseFind ('\\');
		sPath=sPath.Left (nPos+1);

		filePath = sPath + path;

		_tcsncpy(m_path, filePath, MAX_PATH);
	}

	return m_path;
}

BOOL CAnykaIPCameraDlg::read_config(LPCTSTR file_path)
{
	//��ȡ�����ļ��Ĳ���
	CString str;
	//int k;
	BOOL ret = TRUE;
	DWORD read_len = 1;


	//��ȡ�ļ�������
	if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(file_path)))
	{
		return FALSE;
	}

	USES_CONVERSION;

	//�������ļ�
	HANDLE hFile = CreateFile(ConvertAbsolutePath(file_path), GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, 0, NULL);
	if(INVALID_HANDLE_VALUE == hFile)
	{
		return FALSE;
	}

#ifdef _UNICODE
	//USHORT head;
	//ReadFile(hFile, &head, 2, &read_len, NULL);
#endif

	//����һ��һ�ж�ȡ����
	while(read_len > 0)
	{
		int pos;
		CString subLeft, subRight;
		TCHAR ch = 0;
		TCHAR text[1024];
		int index = 0;
		UINT will_len = sizeof(TCHAR);

		while(read_len > 0 && ch != '\n')
		{
			ret = ReadFile(hFile, &ch, will_len, &read_len, NULL);
			text[index++] = ch;
		}
		text[index] = 0;

		str = text;
		int len = str.GetLength();

		//discard the lines that is blank or begin with '#'
		str.TrimLeft();
		if(str.IsEmpty() || '#' == str[0])
		{
			continue;
		}

		pos = str.Find('=');

		subLeft = str.Left(pos);
		subRight = str.Right(str.GetLength() - pos - 1);

		subLeft.TrimLeft();
		subLeft.TrimRight();
		subRight.TrimLeft();
		subRight.TrimRight();

		//nandflash
		if (_T("IP address") == subLeft)
		{
			_tcsncpy(ip_address, subRight, 260);
		}
	}

	CloseHandle(hFile);

	return TRUE;

}


BOOL CAnykaIPCameraDlg::Anyka_connet() 
{

	TCHAR addr_buf[50] = {0};
	UINT len = 0, i = 0, idex = 0, Ip_start_idex = 0;
	BOOL first_flag = TRUE;
	/*
	len = _tcsclen(ip_address);
	if (len > 50)
	{
	AfxMessageBox(_T("��ȡ�ģɣе�ַ�Ǵ����"));
	return FALSE;
	}

	g_send_commad = 0;
	for (i = 0; i < len ; i++)
	{
	if (ip_address[i] == '/')
	{
	if (first_flag)
	{
	first_flag = FALSE;
	Ip_start_idex = i + 2;
	}
	idex++;
	if (idex == 3)
	{
	break;
	}
	}
	}

	_tcsncpy(addr_buf, &ip_address[7], i - Ip_start_idex);
	*/
	if (!ConnetServer(m_connect_ip, 0))
	{
		CloseServer();
		return FALSE;
	}

	return TRUE;
}



BOOL CAnykaIPCameraDlg::Anyka_find_ip_thread() 
{
	CServerSearch search;

	while (1)
	{
		On_find_ip();
		Sleep(30000);
	}
}

DWORD WINAPI Creat_find_ip_main(LPVOID lpParameter) 
{
	CAnykaIPCameraDlg testDlg;

	if (!testDlg.Anyka_find_ip_thread())
	{
		return 0;
	}

	return 1;
}


BOOL CAnykaIPCameraDlg::Creat_find_ip_thread() 
{
	UINT idex = 0;
	g_hWnd = m_Preview[0].m_hWnd;

	if (g_findIP_Thread != INVALID_HANDLE_VALUE)
	{
		Close_find_ip_thread();
		g_findIP_Thread = INVALID_HANDLE_VALUE;
	}

	g_findIP_Thread = CreateThread(NULL, 0, Creat_find_ip_main, &idex, 0, NULL);
	if (g_findIP_Thread == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	return TRUE;

}

void CAnykaIPCameraDlg::Close_find_ip_thread() 
{
	if(g_findIP_Thread != INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_findIP_Thread);
		g_findIP_Thread = INVALID_HANDLE_VALUE;
	}

}

BOOL CAnykaIPCameraDlg::check_ip(TCHAR *buf) 
{
	UINT len = 0, i = 0;

	len = _tcsclen(buf);
	for (i = 0; i < len; i++)
	{
		if (buf[i] >= '0' && buf[i] <= '9')
		{
		}
		else if (buf[i] == '.')
		{

		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CAnykaIPCameraDlg::Anyka_Test_thread() 
{
	TCHAR *buf_head = _T("rtsp://");
	TCHAR *buf_end = _T("//ch0_0.264");
	//TCHAR *buf_end = _T("//video1.sdp");
	UINT len = 0, idex = 0, i = 0;
	TCHAR buf[100] = {0};

	while (1)
	{

		Sleep(1000);
		if (g_start_open_flag)
		{
			if (check_ip(m_connect_ip) && m_connect_ip[0] != 0)
			{

				Sleep(1000);
				//system("arp -d >1.txt");
				WinExec("cmd.exe /c arp -d >1.txt", SW_HIDE);
				//�������ļ�
				//read_config(CONFIG_PATH);
				i++;

				memset(ip_address , 0, AP_ADDRESS_LEN);
				idex = 0;
				len = _tcsclen(buf_head);

				_tcsncpy(&ip_address[idex], buf_head, len);
				idex = idex + len;
				len = _tcsclen(m_connect_ip);
				_tcsncpy(&ip_address[idex], m_connect_ip, len);
				idex = idex + len;
				len = _tcsclen(buf_end);
				_tcsncpy(&ip_address[idex], buf_end, len);

				g_start_open_flag = FALSE;

				if (!OnPreviewchoose_test())
				{
					g_start_open_flag = TRUE;
				}
				else
				{
					/*
					if (!Anyka_connet())
					{
					AfxMessageBox(_T("IRCUT����������ʧ��"), MB_OK);
					g_connet_flag = FALSE;	
					}
					else
					{
					g_connet_flag = TRUE;
					}
					*/
				}
			}
		}
	}
}

/*
BOOL CAnykaIPCameraDlg::Anyka_Test_thread() 
{
while (1)
{
Sleep(1000);
if (g_start_open_flag)
{
Sleep(1000);
//system("arp -d >1.txt");
WinExec("cmd.exe /c arp -d >1.txt", SW_HIDE);
//�������ļ�
read_config(CONFIG_PATH);
g_start_open_flag = FALSE;
if (!OnPreviewchoose_test())
{
g_start_open_flag = TRUE;
}
else
{
if (!Anyka_connet())
{
AfxMessageBox(_T("IRCUT����������ʧ��"), MB_OK);
g_connet_flag = FALSE;	
}
else
{
g_connet_flag = TRUE;
}
}
}
}
}*/


DWORD WINAPI Creat_Anyka_Test_main(LPVOID lpParameter) 
{
	CAnykaIPCameraDlg testDlg;

	if (!testDlg.Anyka_Test_thread())
	{
		return 0;
	}

	return 1;
}

BOOL CAnykaIPCameraDlg::Creat_Anyka_Test_thread() 
{
	UINT idex = 0;
	g_hWnd = m_Preview[0].m_hWnd;

	if (g_hTestThread != INVALID_HANDLE_VALUE)
	{
		Close_Anyka_Test_thread();
		g_hTestThread = INVALID_HANDLE_VALUE;
	}

	g_hTestThread = CreateThread(NULL, 0, Creat_Anyka_Test_main, &idex, 0, NULL);
	if (g_hTestThread == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	return TRUE;

}

void CAnykaIPCameraDlg::Close_Anyka_Test_thread() 
{
	if(g_hTestThread != INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_hTestThread);
		g_hTestThread = INVALID_HANDLE_VALUE;
	}

}


BOOL flag_tool = TRUE;
void CAnykaIPCameraDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	USES_CONVERSION;
	static int SearchCnt = 0;
	static int pictureWaitCnt = 0;

	UINT i = 0, j = 0, len = 0;
	CString   strTemp;  

	if(g_sousuo_flag)
	{
		//g_sousuo_show_flag = FALSE;
		//m_ip_address_idex = 2;

		m_RightDlg.m_test_config.DeleteAllItems();
		for(i = 0; i < m_ip_address_idex; i++)
		{
			/*strTemp.Format(_T("%s"), g_test_config.m_current_config[i].Current_IP_UID);
			m_RightDlg.m_test_config.InsertItem(i, strTemp);
			m_RightDlg.m_test_config.SetItemText(i, 1, 
				g_test_config.m_current_config[i].Current_IP_address_buffer);
			m_RightDlg.m_test_config.SetItemText(i, 2, 
				g_test_config.m_current_config[i].Current_IP_diver_name);*/

			strTemp.Format(_T("%s"), g_test_config.m_last_config[i].Current_IP_UID);
			m_RightDlg.m_test_config.InsertItem(i, strTemp);
			m_RightDlg.m_test_config.SetItemText(i, 1, g_test_config.m_last_config[i].Current_IP_address_buffer);
			m_RightDlg.m_test_config.SetItemText(i, 2, g_test_config.m_last_config[i].Current_IP_diver_name);

			m_find_anyIP = TRUE;
		}
	}

	if (m_not_find_anyIP)
	{
		m_not_find_anyIP = FALSE;
		M_bConn = FALSE;
		m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
		//AfxMessageBox(_T("û���������κ��豸!"));
	}
	else
	{
		if (m_find_anyIP && g_first_find_flag)
		{
			g_first_find_flag = FALSE;
			//current_ip_idex = 0;
			/*len = _tcsclen(g_test_config.m_current_config[current_ip_idex].Current_IP_address_buffer);
			_tcsncpy(m_connect_ip, 
				g_test_config.m_current_config[current_ip_idex].Current_IP_address_buffer, len);

			m_RightDlg.SetDlgItemText(IDC_EDIT_IP, 
				g_test_config.m_current_config[current_ip_idex].Current_IP_address_buffer);
			m_RightDlg.SetDlgItemText(IDC_EDIT_UID, 
				g_test_config.m_current_config[current_ip_idex].Current_IP_UID);
			m_RightDlg.SetDlgItemText(IDC_EDIT_MAC, 
				g_test_config.m_current_config[current_ip_idex].Current_IP_MAC);
			m_RightDlg.SetDlgItemText(IDC_EDIT_VERSION2,
				g_test_config.m_current_config[current_ip_idex].Current_IP_version);*/

			len = _tcsclen(g_test_config.m_last_config[current_ip_idex].Current_IP_address_buffer);
			memset(m_connect_ip, 0, MAX_PATH);
			_tcsncpy(m_connect_ip, g_test_config.m_last_config[current_ip_idex].Current_IP_address_buffer, len);

			m_RightDlg.SetDlgItemText(IDC_EDIT_IP, g_test_config.m_last_config[current_ip_idex].Current_IP_address_buffer);
			m_RightDlg.SetDlgItemText(IDC_EDIT_UID, g_test_config.m_last_config[current_ip_idex].Current_IP_UID);
			m_RightDlg.SetDlgItemText(IDC_EDIT_MAC, g_test_config.m_last_config[current_ip_idex].Current_IP_MAC);
			m_RightDlg.SetDlgItemText(IDC_EDIT_VERSION2, g_test_config.m_last_config[current_ip_idex].Current_IP_version);

		}
	}


	if (0)//flag_tool)
	{
		flag_tool = FALSE;
		Creat_Anyka_Test_thread();
	}

	if (!g_connet_flag)
	{
		GetDlgItem(IDC_BUTTON_SET)->EnableWindow(FALSE);//���� 
		GetDlgItem(IDC_BUTTON_RECOVER_DEV)->EnableWindow(FALSE);//���� 

	}
	else
	{
		GetDlgItem(IDC_BUTTON_SET)->EnableWindow(TRUE);//���� 
		GetDlgItem(IDC_BUTTON_RECOVER_DEV)->EnableWindow(TRUE);//���� 
	}

	m_BottomDlg.SetDlgItemText(IDC_EDIT_PRESENT, g_test_config.m_mac_current_addr);
	m_BottomDlg.SetDlgItemText(IDC_EDIT_WIFI_NAME, g_test_config.newest_version);

	strTemp.Format(_T("%d"), g_test_config.test_num);
	m_BottomDlg.SetDlgItemText(IDC_EDIT_BURN_NUM, strTemp);

#if 1

#if 0
	if (g_start_open_flag)
	{
		Sleep(1000);
		//system("arp -d >1.txt");
		WinExec("cmd.exe /c arp -d >1.txt", SW_HIDE);
		//�������ļ�
		read_config(CONFIG_PATH);
		g_start_open_flag = FALSE;
		if (!OnPreviewchoose_test())
		{
			g_start_open_flag = TRUE;
		}
	}
#endif

#else


	if (nIDEvent == TIMER_COMMAND) {
		if (m_bIsSearch && (SearchCnt < 3)) {
			m_Search.Search();
			++SearchCnt;
		}

		if (SearchCnt == 3) {
			SearchCnt = 0;
			m_bIsSearch = FALSE;
			int nCount = 0;
			if (!(nCount = m_Search.GetServerCount())) {
				AfxMessageBox(L"δ�������κη�����!", 0, 0);
			}else {
				char strServerID[MAX_ID_LEN] = {0};
				char strServerIP[MAX_IP_LEN] = {0};
				unsigned int len = MAX_ID_LEN;
				STREAMMODE mode = STREAM_MODE_MAX;
				BOOL bFind = FALSE;
				HTREEITEM hRoot = m_TreeCtrl.GetRootItem();
				CString strServerIDShow;

				for (int i = 0; i < nCount; ++i) {
					IServer * pIServer;
					m_Search.GetServer(i, &pIServer);

					len = MAX_ID_LEN;
					ZeroMemory(strServerID, len);
					pIServer->GetServerID(strServerID, &len);

					len = MAX_IP_LEN;
					ZeroMemory(strServerIP, len);
					pIServer->GetServerIp(strServerIP, &len);

					strServerIDShow.Format(DEVICE_PREFIX, i, A2W(strServerID), A2W(strServerIP));

					HTREEITEM hDevice = m_TreeCtrl.InsertItem(strServerIDShow, hRoot);

					unsigned int nCnt = 0;
					pIServer->GetServerStreamCnt(nCnt);

					for (unsigned int j = 0; j < nCnt; ++j) {
						pIServer->GetServerStreamMode(j, mode);
						if (mode >= STREAM_MODE_MAX) {
							continue;
						}

						if (mode == STREAM_MODE_VIDEO_720P)	m_TreeCtrl.SetItemData(m_TreeCtrl.InsertItem(VIDEO_MODE_NAME_720P, hDevice), (DWORD_PTR)j);
						else if (mode == STREAM_MODE_VIDEO_VGA) m_TreeCtrl.SetItemData(m_TreeCtrl.InsertItem(VIDEO_MODE_NAME_VGA, hDevice), (DWORD_PTR)j);
						else if (mode == STREAM_MODE_VIDEO_QVGA) m_TreeCtrl.SetItemData(m_TreeCtrl.InsertItem(VIDEO_MODE_NAME_QVGA, hDevice), (DWORD_PTR)j);
						else if (mode == STREAM_MODE_VIDEO_D1) m_TreeCtrl.SetItemData(m_TreeCtrl.InsertItem(VIDEO_MODE_NAME_D1, hDevice), (DWORD_PTR)j);
						else continue;
					}

					m_TreeCtrl.SetItemData(hDevice, (DWORD_PTR)pIServer);
				}//for (int i = 0; i < nCount; ++i)

				AfxMessageBox(L"�����ɹ�!", 0, 0);
				m_TreeCtrl.Expand(m_TreeCtrl.GetRootItem(), TVE_EXPAND);
			}//if (!(nCount = m_Search.GetServerCount())) else
		}//SearchCnt == 3

		//show fps and bits rate info in the Status Bar
		char strDeviceID[MAX_ID_LEN] = {0};
		unsigned int nLen = MAX_ID_LEN;
		WCHAR strInfo[1024] = {0};

		for (int i = 0; i < SUPPORT_STREAM_CNT; ++i) {
			m_StatusBar.SetPaneText(  i + 1, L"");
			if (m_pClient[i] != NULL && m_videoRender[i] != NULL) {
				if (m_pServerPreviews[i]) {
					nLen = MAX_ID_LEN;
					m_pServerPreviews[i]->GetServerID(strDeviceID, &nLen);

					double dBR = m_pClient[i]->GetBitsRatePerSec() / (double)1000;
					if (dBR < 1.0) dBR = 0.0;

					_sntprintf_s(strInfo, 1024, 1024, L"%s:%dFps,%0.2fKbps", A2W(strDeviceID), m_videoRender[i]->getFpsOneSec(), dBR);
					m_StatusBar.SetPaneText(i + 1, strInfo);
				}
			}
		}

		//��Ч���չ�������ť���������û��������ٵ�ʹ�����չ��ܡ�
		if (!m_bPicture) {
			++pictureWaitCnt;
			if (pictureWaitCnt > 1) {
				m_bPicture = TRUE;
				pictureWaitCnt = 0;
				m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_BUTTON_PIC, TRUE);
			}
		}

#ifdef USE_LOG_FILE
		fflush(stderr);
#endif
	} //nIDEvent == TIMER_COMMAND

	if (nIDEvent == TIMER_LONG_PRESS) {
		if (!m_bIsLongPress) {//������һ��500ms����ʱ������COTINUE��Ϣ
			if (m_nLongPressButtonID == IDC_BUTTON_LEFT) {
				CameraMovement(CMT_STEP_LEFT_CONTINUE);
				m_bIsLongPress = TRUE;
			}else if (m_nLongPressButtonID == IDC_BUTTON_RIGHT) {
				CameraMovement(CMT_STEP_RIGHT_CONTINUE);
				m_bIsLongPress = TRUE;
			}else if (m_nLongPressButtonID == IDC_BUTTON_UP) {
				CameraMovement(CMT_STEP_UP_CONTINUE);
				m_bIsLongPress = TRUE;
			}else if (m_nLongPressButtonID == IDC_BUTTON_DOWN) {
				CameraMovement(CMT_STEP_DOWN_CONTINUE);
				m_bIsLongPress = TRUE;
			}else {
				//�û����ٳ���
			}
		}
	}
#endif

	CDialog::OnTimer(nIDEvent);
}

char * CAnykaIPCameraDlg::MakeRTSPUrl()
{
	WCHAR astrMsg[100] = {0};
	static char strURL[MAX_RTSP_URL_LEN] = { 0 };
	unsigned int iStreamSelect = 0, nCnt = 0, nPort = 0;

	IServer * pIServer = (IServer *)m_TreeCtrl.GetItemData(m_hCurrentSelectItem);
	HTREEITEM hSelectItem = m_TreeCtrl.GetSelectedItem();
	iStreamSelect = (unsigned int)m_TreeCtrl.GetItemData(hSelectItem);

	char strStreamName[MAX_STREAM_NAME] = {0};
	char strIPAddr[MAX_IP_LEN];
	unsigned int len = MAX_IP_LEN;

	ZeroMemory(strURL, sizeof(strURL));

	strncpy(strURL, RTSP_PREFIX, strlen(RTSP_PREFIX));
	pIServer->GetServerIp(strIPAddr, &len);
	strncat(strURL, strIPAddr, len);

	pIServer->GetServerStreamPort(nPort);
	if (nPort) {
		strncat(strURL, PORT_PREFIX, strlen(PORT_PREFIX));
		char strPort[10] = {0};
		sprintf(strPort, "%d", nPort);
		strncat(strURL, strPort, strlen(strPort));
	}

	strncat(strURL, SEPARATOR, strlen(SEPARATOR));

	len = MAX_STREAM_NAME;
	pIServer->GetServerStreamName(iStreamSelect, strStreamName, &len);
	strncat(strURL, strStreamName, len);

	return strURL;
}

int CAnykaIPCameraDlg::CloseTheStream(int iSelect, BOOL bNeedCloseAudio)
{
	if (iSelect > 2 || iSelect < 0) return -1;

	CAutoLock lock(&m_csForOpenCloseStream);
	if (m_pClient[iSelect]) {
		m_pClient[iSelect]->Close();
		delete m_pClient[iSelect];
	}

	m_pClient[iSelect] = NULL;

	if (m_videoDecoder[iSelect]) delete m_videoDecoder[iSelect];
	m_videoDecoder[iSelect] = NULL;
	if (m_videoRender[iSelect]) delete m_videoRender[iSelect];
	m_videoRender[iSelect] = NULL;

	if ((iSelect == m_nAudioClientIndex) && bNeedCloseAudio) TempCloseTalk(iSelect);
	return 0;
}

#define MAX_WAIT_CNT	20

int CAnykaIPCameraDlg::OpenTheStream(int iSelect, const char * strURL, BOOL bNeedENotify)
{
	USES_CONVERSION;

	WCHAR astrMsg[300] = {0};
	int iErrorCode = 0;

	if (strURL == NULL) {
		if (bNeedENotify)
			AfxMessageBox( L"�޷��򿪿�rtsp��ַ!", 0, 0 );

		return 0;
	}

	unsigned int iStreamChoose = 0, iSCnt = 0, len = MAX_STREAM_NAME;
	int iFps = 0;

	const char * pWhere = NULL;
#if 0
	m_pServerPreviews[iSelect]->GetServerStreamCnt(iSCnt);

	pWhere = strrchr(strURL, CHAR_SEPARATOR);
	if (pWhere == NULL) 
		iSCnt = 0;

	pWhere += 1;
	char strStreamName[MAX_STREAM_NAME] = {0};

	for (iStreamChoose = 0; iStreamChoose < iSCnt; ++iStreamChoose) {
		len = MAX_STREAM_NAME;
		ZeroMemory(strStreamName, MAX_STREAM_NAME * sizeof(char));

		if (m_pServerPreviews[iSelect]->GetServerStreamName(iStreamChoose, strStreamName, &len) < 0)
			continue;

		if (strcmp(pWhere, strStreamName) == 0)
			break;
	}

	if (iStreamChoose < iSCnt)
		m_pServerPreviews[iSelect]->GetStreamFps(iStreamChoose, iFps);
	else
#endif
		iFps = 30;

	CAutoLock lock(&m_csForOpenCloseStream);

	if (m_nVideoFullScreenIndex == -1) {
		if (m_pClient[iSelect] != NULL)	
			CloseTheStream(iSelect, TRUE);
	}else {//full screen, and we recv a disconnect message.
		if (m_nVideoFullScreenIndex == iSelect) {
			if (m_pClient[iSelect]) {
				m_pClient[iSelect]->Close();
				delete m_pClient[iSelect];
			}

			m_pClient[iSelect] = NULL;

			if (m_videoDecoder[iSelect]) delete m_videoDecoder[iSelect];
			m_videoDecoder[iSelect] = NULL;

			if (iSelect == m_nAudioClientIndex) TempCloseTalk(m_nAudioClientIndex);
		}
	}


	m_pClient[iSelect] = CAimer39RTSPClient::CreateNew();
	if (NULL == m_pClient[iSelect]) {
		if (bNeedENotify)
			AfxMessageBox( L"�޷�������...�ڴ治��!", 0, 0 );
		return -1;
	}

	m_pClient[iSelect]->RegisterFinishCallback(OnClientFinish, this);
	m_pClient[iSelect]->RegisterDisConnCallback(OnClientDisConnect, this);

	iErrorCode = m_pClient[iSelect]->OpenURL(strURL);
	if (iErrorCode < 0) {
		if (bNeedENotify) {
			_sntprintf(astrMsg, 300, L"OpenURL %s error! error = %s", 
				A2W(strURL), A2W(m_pClient[iSelect]->GetLastError()));
			AfxMessageBox( astrMsg, 0, 0 );
		}
		return -1;
	}

	int nWaitCnt = 0;
	bool isPrepare = false;
	while (!isPrepare) {
		iErrorCode = m_pClient[iSelect]->IsPrepare(isPrepare);
		if ((iErrorCode != 0) || (nWaitCnt >= MAX_WAIT_CNT)) {
			if (bNeedENotify) {
				if ((iErrorCode == 0) && (nWaitCnt >= MAX_WAIT_CNT)) 
					_sntprintf(astrMsg, 300, L"���ӷ�����%s, ��ʱ��", A2W(strURL));
				else
					_sntprintf(astrMsg, 300, L"OpenURL %s error! error = %s", 
					A2W(strURL), A2W(m_pClient[iSelect]->GetLastError()));

				//AfxMessageBox( astrMsg, 0, 0 );
			}

			m_pClient[iSelect]->Close();
			delete m_pClient[iSelect];
			m_pClient[iSelect] = NULL;
			return -1;
		}

		++nWaitCnt;
		Sleep(100);
	}

	unsigned int iStreamCnt = 0;
	STREAM_TYPE type = STREAM_AUDIO;
	m_pClient[iSelect]->GetStreamCount(iStreamCnt);

	for (unsigned int i = 0; i < iStreamCnt; ++i) {
		m_pClient[iSelect]->GetStreamType(i, type);

		if (type == STREAM_AUDIO) {
			//Ԥ����ʼʱĬ�ϲ�������Ƶ
		}else if (type == STREAM_VIDEO) {
			int nReChooseSyncClock = -1;

			m_videoDecoder[iSelect] = CFfmpegEnvoy::createNew();
			if ((m_videoRender[iSelect] == NULL) || (m_nVideoFullScreenIndex == -1)) {
				m_videoRender[iSelect] = CVideoRender::createNew();
				if ((iErrorCode = m_videoRender[iSelect]->OpenRender(g_hWnd)) < 0) {    //m_Preview[iSelect].m_hWnd
					fprintf(stderr, "OpenTheStream::OpenRender error!\n");
					return iErrorCode;
				}
				m_videoRender[iSelect]->SetFillMode(KeepAspectRatio);
			}else {
				m_videoRender[iSelect]->Reset();
			}

			m_videoRender[iSelect]->SetServerStreamFps(iFps);

			m_videoDecoder[iSelect]->OpenFfmpeg();
			//clock sync use
			/*for (int i = 0; i < PREVIEW_WINDOWS; ++i) {
			if (i == iSelect) continue;

			if (m_pServerPreviews[i] == m_pServerPreviews[iSelect] && m_SyncClock[i].IsStart())
			nReChooseSyncClock = i;
			}

			if (nReChooseSyncClock < 0) {
			m_SyncClock[iSelect].ReInit();
			m_videoRender[iSelect]->setClock(&m_SyncClock[iSelect]);
			}else {
			m_videoRender[iSelect]->setClock(&m_SyncClock[nReChooseSyncClock]);
			}*/

			m_pClient[iSelect]->RegisterSink(type, m_videoDecoder[iSelect]);
			m_videoDecoder[iSelect]->RegisterSink(m_videoRender[iSelect], SINK_VIDEO);
		}
	}

	m_pClient[iSelect]->Play();

	if (m_videoDecoder[iSelect])
		m_videoDecoder[iSelect]->Start();

	//if (m_AudioDecoder[iSelect])
	//m_AudioDecoder[iSelect]->Start();

	return 0;
}

int CAnykaIPCameraDlg::RegisterThePreviewServer(IServer * pIServer, int iSelect, const char * strURL)
{
	USES_CONVERSION;
	if (NULL == pIServer || iSelect > PREVIEW_WINDOWS || iSelect < 0) return -1;

	if (m_pServerPreviews[iSelect] == pIServer) {
		if (strcmp((m_strURL[iSelect].c_str()), strURL)) {
			m_strURL[iSelect].clear();
			m_strURL[iSelect] = strURL;
		}

		return 0; // already registered
	}

	CAutoLock lock(&m_csForServerConnect);

	if (m_pServerPreviews[iSelect]) {
		m_pServerPreviews[iSelect]->DisConnect();
	}

	if (pIServer->Connect() < 0) {
		WCHAR astrMsg[100] = {0};
		char strIPAddr[MAX_IP_LEN] = {0};
		unsigned int nLen = MAX_IP_LEN;

		pIServer->GetServerIp(strIPAddr, &nLen);

		_sntprintf(astrMsg, 100, L"�޷����ӵ�������%s", A2W(strIPAddr));
		AfxMessageBox( astrMsg, 0, 0 );
		return -1;
	}

	int iRet = pIServer->SendGetServerInfo();
	if (iRet < 0) {
		pIServer->DisConnect();
		return -1;
	}

	BOOL bIsRespond = FALSE;
	pIServer->GetServerRespondComplete(bIsRespond);
	int iAttemptOpenCnt = 0;

	while(!bIsRespond && iAttemptOpenCnt < ATTEMPT_OPEN_MAX) {
		Sleep(200);
		pIServer->GetServerRespondComplete(bIsRespond);
		++iAttemptOpenCnt;
	}

	if (bIsRespond) {
		IMAGE_SET stImageSet = {0};
		pIServer->GetServerImageSet(stImageSet);
		if ((stImageSet.nBrightness == 255) && 
			(stImageSet.nContrast == 255) && (stImageSet.nSaturation == 255)) { // the server don't want us to connect to it, because the server is connect limit was reached.
				WCHAR astrMsg[100] = {0};
				char strIPAddr[MAX_IP_LEN] = {0};
				unsigned int nLen = MAX_IP_LEN;

				pIServer->GetServerIp(strIPAddr, &nLen);

				_sntprintf(astrMsg, 100, L"������%s���������Ѿ��ﵽ���ޣ���������ֹ���ǵ�����!", A2W(strIPAddr));
				AfxMessageBox(astrMsg, 0, 0);

				pIServer->DisConnect();
				return -1;
		}
	}

	m_pServerPreviews[iSelect] = pIServer;
	m_strURL[iSelect].clear();
	m_strURL[iSelect] = strURL;
	//m_pServerPreviews[iSelect]->SetCurrentPlayURL(strURL);

	m_pServerPreviews[iSelect]->SetServerRetCallBack(OnServerReturnInfo, this);

	return 0;
}

int CAnykaIPCameraDlg::UnregisterThePreviewServer(int iSelect)
{
	if (iSelect > PREVIEW_WINDOWS || iSelect < 0) return -1;

	if (m_pServerPreviews[iSelect] == NULL) return 0; // already unregistered

	BOOL bNeedDisConnect = TRUE;

	CAutoLock lock(&m_csForServerConnect);

	for (int iIndex = 0; iIndex < PREVIEW_WINDOWS; ++iIndex) {
		if (iIndex == iSelect) continue;
		if (m_pServerPreviews[iIndex] == m_pServerPreviews[iSelect])
			bNeedDisConnect = FALSE;
	}

	if (bNeedDisConnect)
		m_pServerPreviews[iSelect]->DisConnect();

	m_pServerPreviews[iSelect]->SetServerRetCallBack(NULL, NULL);
	m_pServerPreviews[iSelect] = NULL;
	m_strURL[iSelect].clear();

	return 0;
}


BOOL CAnykaIPCameraDlg::OnPreviewchoose_test()
{
	// TODO: �ڴ���������������
	int ret = 0;
	//IServer * pIServer = (IServer *)m_TreeCtrl.GetItemData(m_hCurrentSelectItem);
	//const char * strURL = "rtsp://172.22.5.8/vs1";//MakeRTSPUrl();
	char strURL[MAX_RTSP_URL_LEN] = {0};

	USES_CONVERSION;


	memcpy(strURL, T2A(ip_address), MAX_RTSP_URL_LEN);

	if (0)//RegisterThePreviewServer(pIServer, 0, strURL) < 0)
	{
		return FALSE;
	}

	if ((ret = OpenTheStream(0, strURL)) < 0) 
	{
		//UnregisterThePreviewServer(0);

		if (ret == -2) 
		{
			AfxMessageBox(L"D3D9 class initialize failed!");
		}
		return FALSE;
	}

	if (m_nAudioClientIndex == 0)
		m_nAudioClientIndex = -1;


	return TRUE;
}



void CAnykaIPCameraDlg::OnPreviewchoose1()
{
	// TODO: �ڴ���������������
	int ret = 0;
	IServer * pIServer = (IServer *)m_TreeCtrl.GetItemData(m_hCurrentSelectItem);
	const char * strURL = MakeRTSPUrl();

	if (RegisterThePreviewServer(pIServer, 0, strURL) < 0) return;

	if ((ret = OpenTheStream(0, strURL)) < 0) {
		UnregisterThePreviewServer(0);

		if (ret == -2) {
			AfxMessageBox(L"D3D9 class initialize failed!");
		}
	}

	if (m_nAudioClientIndex == 0)
		m_nAudioClientIndex = -1;
}

void CAnykaIPCameraDlg::OnPreviewchoose2()
{
	// TODO: �ڴ���������������
	int ret = 0;
	IServer * pIServer = (IServer *)m_TreeCtrl.GetItemData(m_hCurrentSelectItem);
	const char * strURL = MakeRTSPUrl();

	if (RegisterThePreviewServer(pIServer, 1, strURL) < 0) return;

	if ((ret = OpenTheStream(1, strURL)) < 0){
		UnregisterThePreviewServer(1);

		if (ret == -2) {
			AfxMessageBox(L"D3D9 class initialize failed!");
		}
	}

	if (m_nAudioClientIndex == 1)
		m_nAudioClientIndex = -1;
}

void CAnykaIPCameraDlg::MakeAndSendImageSet()
{
	int nIndex = m_ContrastCombo.GetCurSel();

	CString strContrast, strSaturation, strBrightness, strAcutance;
	m_ContrastCombo.GetLBText(nIndex, strContrast);

	nIndex = m_SaturationCombo.GetCurSel();
	m_SaturationCombo.GetLBText(nIndex, strSaturation);

	nIndex = m_BrightnessCombo.GetCurSel();
	m_BrightnessCombo.GetLBText(nIndex, strBrightness);

	nIndex = m_acutanceCom.GetCurSel();
	m_acutanceCom.GetLBText(nIndex, strAcutance);

	int iContrast = 0, iSaturation = 0, iBrightness = 0, iAcutance=0;
	iContrast = _ttoi(strContrast);
	iSaturation = _ttoi(strSaturation);
	iBrightness = _ttoi(strBrightness);
	iAcutance = _ttoi(strAcutance);

	IMAGE_SET stImageSet = {0};

	if (!m_hCurrentSelectItem) {
		AfxMessageBox( L"δѡ���κ��豸�������豸�б���ѡ��һ���豸!", 0, 0 );
		return;
	}

	stImageSet.nBrightness = iBrightness;
	stImageSet.nContrast = iContrast;
	stImageSet.nSaturation = iSaturation;
	stImageSet.nReserve = iAcutance;

	IServer * pIServer = (IServer *)m_TreeCtrl.GetItemData(m_hCurrentSelectItem);
	pIServer->SendImageSet(stImageSet);
}

void CAnykaIPCameraDlg::OnCbnSelchangeContrastCombo()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	MakeAndSendImageSet();
}

void CAnykaIPCameraDlg::OnCbnSelchangeBrightnessCombo()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	MakeAndSendImageSet();
}

void CAnykaIPCameraDlg::OnCbnSelchangeSaturationCombo()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	MakeAndSendImageSet();
}

BOOL CAnykaIPCameraDlg::CanDoTheJob()
{
	if (!m_hCurrentSelectItem) return FALSE;

	IServer * pIServer = (IServer *)m_TreeCtrl.GetItemData(m_hCurrentSelectItem);

	for (int i = 0; i < PREVIEW_WINDOWS; ++i) {
		if (pIServer == m_pServerPreviews[i]) return TRUE;
	}

	return FALSE;
}

int CAnykaIPCameraDlg::OnAudioInOpen(int nAudioClientIndex)
{
	if (nAudioClientIndex > SUPPORT_STREAM_CNT || nAudioClientIndex < 0) return -1;

	if (NULL == m_pClient[nAudioClientIndex]) return -1;

	if (m_AudioDecoder[0]) {
		for (int i = 0; i < PREVIEW_WINDOWS; ++i) {
			if (m_pClient[i]) {
				m_pClient[i]->UnregisterSink(m_AudioDecoder[0]);
			}
		}

		m_AudioDecoder[0]->UnregisterSink(m_AudioRender[0], SINK_AUDIO);
		delete m_AudioDecoder[0];
	}

	m_AudioDecoder[0] = NULL;

	if (m_AudioRender[0]) {
		//Audio Render time sync use
		for (int i = 0; i < PREVIEW_WINDOWS; ++i) {
			if ((i != nAudioClientIndex) && (m_videoRender[i])) m_videoRender[i]->SetAudioRender(NULL);
		}

		delete m_AudioRender[0];
	}

	m_AudioRender[0] = NULL;

	m_AudioDecoder[0] = CFfmpegEnvoy::createNew();
	m_AudioRender[0] = CAudioRender::createNew();

	m_AudioDecoder[0]->OpenFfmpeg();
	m_AudioRender[0]->OpenRender();
	//clock sync use
	/*int nReChooseSyncClock = -1;
	for (int i = 0; i < PREVIEW_WINDOWS; ++i) {
	if (i == nAudioClientIndex) continue;

	if (m_pServerPreviews[i] == m_pServerPreviews[nAudioClientIndex] && m_SyncClock[i].IsStart())
	nReChooseSyncClock = i;
	}

	if (nReChooseSyncClock >= 0)
	m_AudioRender[0]->setClock(&m_SyncClock[nReChooseSyncClock]);
	else 
	m_AudioRender[0]->setClock(&m_SyncClock[nAudioClientIndex]);*/

	//Audio Render time sync use
	if (m_videoRender[nAudioClientIndex]) {
		m_videoRender[nAudioClientIndex]->SetAudioRender(m_AudioRender[0]);
	}

	m_pClient[nAudioClientIndex]->RegisterSink(STREAM_AUDIO, m_AudioDecoder[0]);
	m_AudioDecoder[0]->RegisterSink(m_AudioRender[0], SINK_AUDIO);

	if (m_AudioDecoder[0]) {
		m_AudioDecoder[0]->Start();
	}

	return 0;
}

int CAnykaIPCameraDlg::OnAudioInClose(int nAudioClientIndex)
{
	//Audio Render time sync use
	if (m_videoRender[nAudioClientIndex]) m_videoRender[nAudioClientIndex]->SetAudioRender(NULL);
	if (m_pClient[nAudioClientIndex]) m_pClient[nAudioClientIndex]->UnregisterSink(m_AudioDecoder[0]);


	if (m_AudioDecoder[0]) delete m_AudioDecoder[0];
	m_AudioDecoder[0] = NULL;

	if (m_AudioRender[0]) delete m_AudioRender[0];
	m_AudioRender[0] = NULL;
	return 0;
}

LRESULT CAnykaIPCameraDlg::OnServerDisconnect(WPARAM wParam, LPARAM lParam)
{
	if (ServerDisConnect((int)lParam) < 0 ) return -1;
	else return 0;
}

void CAnykaIPCameraDlg::UpdateTreeNode(IServer * UpdateServer)
{
	if (!UpdateServer) return;

	HTREEITEM hRoot = m_TreeCtrl.GetRootItem();
	HTREEITEM hItem = NULL, hChildItem = NULL;

	if (!(hItem = m_TreeCtrl.GetChildItem(hRoot))) {
		return;
	}

	IServer * pServer = NULL;
	while(hItem) {
		pServer = (IServer *)m_TreeCtrl.GetItemData(hItem);
		if (pServer == UpdateServer)
			break;

		hItem = m_TreeCtrl.GetNextItem(hItem, TVGN_NEXT);
	}

	if (hItem == NULL) return;

	hChildItem = m_TreeCtrl.GetChildItem(hItem);

	STREAMMODE mode = STREAM_MODE_MAX;
	int iStreamNum = 0;

	while(hChildItem) {
		iStreamNum = m_TreeCtrl.GetItemData(hChildItem);

		UpdateServer->GetServerStreamMode(iStreamNum, mode);
		if (mode >= STREAM_MODE_MAX) {
			continue;
		}

		if (mode == STREAM_MODE_VIDEO_720P)	m_TreeCtrl.SetItemText(hChildItem, VIDEO_MODE_NAME_720P);
		else if (mode == STREAM_MODE_VIDEO_VGA) m_TreeCtrl.SetItemText(hChildItem, VIDEO_MODE_NAME_VGA);
		else if (mode == STREAM_MODE_VIDEO_QVGA) m_TreeCtrl.SetItemText(hChildItem, VIDEO_MODE_NAME_QVGA);
		else if (mode == STREAM_MODE_VIDEO_D1) m_TreeCtrl.SetItemText(hChildItem, VIDEO_MODE_NAME_D1);
		else continue;

		hChildItem = m_TreeCtrl.GetNextItem(hChildItem, TVGN_NEXT);
	}
}

int CAnykaIPCameraDlg::ServerDisConnect(int iSelect)
{
	if (m_pServerPreviews[iSelect] == NULL) return -1;

	if (m_pServerPreviews[iSelect]->IsDisConnect())
	{
		int iRet = m_pServerPreviews[iSelect]->Connect();
		if (iRet < 0) {
#ifdef WARN_ERROR_OUT
			char strIPAddr[MAX_IP_LEN] = {0};
			unsigned int nLen = MAX_IP_LEN;

			m_pServerPreviews[iSelect]->GetServerIp(strIPAddr, &nLen);
			fprintf(stderr, "WARN::####Disconnet server : %s connect failed####\n", strIPAddr);
#endif
			m_pServerPreviews[iSelect]->DisConnect();
			return -1;
		}

		//���������ɹ�����Ҫ���·��ͻ�ȡServerInfo������Ա��ȡ���µķ���˵Ļ���������Ϣ��
		iRet = m_pServerPreviews[iSelect]->SendGetServerInfo();
		if (iRet < 0) {
#ifdef WARN_ERROR_OUT
			char strIPAddr[MAX_IP_LEN] = {0};
			unsigned int nLen = MAX_IP_LEN;

			m_pServerPreviews[iSelect]->GetServerIp(strIPAddr, &nLen);
			fprintf(stderr, "WARN::####Disconnet server : %s Send Get Server Info failed####\n", strIPAddr);
#endif
			m_pServerPreviews[iSelect]->DisConnect();
			return -1;
		}

		BOOL bIsRespond = FALSE;
		//������Ƿ�ظ������Ļ���������Ϣ��
		m_pServerPreviews[iSelect]->GetServerRespondComplete(bIsRespond);
		int iAttemptOpenCnt = 0;

		while(!bIsRespond && iAttemptOpenCnt < ATTEMPT_OPEN_MAX) {
			Sleep(200);
			m_pServerPreviews[iSelect]->GetServerRespondComplete(bIsRespond);
			++iAttemptOpenCnt;
		}

		if (iAttemptOpenCnt >= ATTEMPT_OPEN_MAX && !bIsRespond) {
#ifdef WARN_ERROR_OUT
			char strIPAddr[MAX_IP_LEN] = {0};
			unsigned int nLen = MAX_IP_LEN;

			m_pServerPreviews[iSelect]->GetServerIp(strIPAddr, &nLen);
			fprintf(stderr, "WARN::####Disconnet server : %s Send Get Server Info time out####\n", strIPAddr);
#endif

			goto Next;
		}else if (bIsRespond) {
			IMAGE_SET stImageSet = {0};
			m_pServerPreviews[iSelect]->GetServerImageSet(stImageSet);
			if ((stImageSet.nBrightness == 255) && 
				(stImageSet.nContrast == 255) && (stImageSet.nSaturation == 255)) { // the server don't want us to connect to it, because the server is connect limit was reached.
					AfxMessageBox(L"���������������Ѿ��ﵽ���ޣ���������ֹ���ǵ�����!", 0, 0);

					CloseTheStream(iSelect, TRUE);
					if (iSelect == m_nAudioClientIndex)
						m_nAudioClientIndex = -1;

					m_pServerPreviews[iSelect]->DisConnect();
					m_pServerPreviews[iSelect] = NULL;
					m_strURL[iSelect].clear();

					return -1;
			}
		}

		UpdateTreeNode(m_pServerPreviews[iSelect]);
	}

Next:
	if (m_strURL[iSelect].empty()) {
		AfxMessageBox(L"�ӷ������л�ȡ�����Ѿ��ڲ��ŵ�rtsp��ַ��������󽫵�������ʧ�ܣ��޷�������Ƶ����", 0, 0);
#ifdef WARN_ERROR_OUT
		fprintf(stderr, "WARN::####can't get the play url from server, can't play the stream####\n");
#endif
		CloseTheStream(iSelect, TRUE);
		if (iSelect == m_nAudioClientIndex)
			m_nAudioClientIndex = -1;

		m_pServerPreviews[iSelect]->DisConnect();
		m_pServerPreviews[iSelect] = NULL;
		return -1;
	}

	if (OpenTheStream(iSelect, m_strURL[iSelect].c_str(), FALSE) < 0) {
#ifdef WARN_ERROR_OUT
		fprintf(stderr, "WARN::####open stream error!####\n");
#endif
		m_pServerPreviews[iSelect]->DisConnect();
		return -1;	
	}

	if (iSelect == m_nAudioClientIndex) { //��Client�����ڶԽ���Client
		m_nRBChoosePrevIndex = m_nAudioClientIndex;
		OnPreviewdlgchooseTalkOpen();
	}

#ifdef WARN_ERROR_OUT
	char strIPAddr[MAX_IP_LEN] = {0};
	unsigned int nLen1 = MAX_IP_LEN;

	m_pServerPreviews[iSelect]->GetServerIp(strIPAddr, &nLen1);
	fprintf(stderr, "WARN::####Disconnet server : %s connect success####\n", strIPAddr);
#endif

	return 0;
}

void CAnykaIPCameraDlg::Monitor()
{
	vector<int> vecAlreadyDo;
	BOOL bIsContinue = FALSE;
	BOOL bIsAgain = FALSE;

	while(TRUE) {

		if (!m_runThreadFlag) break;

		if (!m_bNeedJudgeDisConnWork){
			Sleep(1000);//1 second
			continue;
		}

		vecAlreadyDo.clear();

		for (int i = 0; i < PREVIEW_WINDOWS; ++i) { //��������
			CAutoLock lock(&m_csForServerConnect);

			bIsAgain = FALSE;

			if (m_nVideoFullScreenIndex != -1) { //�����ȫ��ģʽ��ֻ�ж�����ȫ�����ŵ�Server�Ƿ�Ͽ�
				if (m_pServerPreviews[m_nVideoFullScreenIndex] && 
					m_pServerPreviews[m_nVideoFullScreenIndex]->IsDisConnect()) {
#ifdef WARN_ERROR_OUT					
						char strIPAddr[MAX_IP_LEN] = {0};
						unsigned int nLen = MAX_IP_LEN;

						m_pServerPreviews[m_nVideoFullScreenIndex]->GetServerIp(strIPAddr, &nLen);
						fprintf(stderr, "WARN::####Disconnet server : %s start reconnect####\n", strIPAddr);
#endif
						ServerDisConnect(m_nVideoFullScreenIndex);
				}

				break;
			}

			bIsContinue = FALSE;

			for (unsigned int k = 0; k < vecAlreadyDo.size(); ++k) {
				if (i == vecAlreadyDo[k]) {
					bIsContinue = TRUE;
					break;
				}
			}

			if (bIsContinue) continue;

			if (m_pServerPreviews[i] && m_pServerPreviews[i]->IsDisConnect()) {

#ifdef WARN_ERROR_OUT
				char strIPAddr[MAX_IP_LEN] = {0};
				unsigned int nLen = MAX_IP_LEN;

				m_pServerPreviews[i]->GetServerIp(strIPAddr, &nLen);
				fprintf(stderr, "WARN::####Disconnet server : %s start reconnect####\n", strIPAddr);
#endif
				for (int j = 0; j < PREVIEW_WINDOWS; ++j) {//ͬһ�������Ĳ�ͬ����ͬ����ͬʱ���Ԥ����
					if ((i != j) && (m_pServerPreviews[j] == m_pServerPreviews[i])) {
						if (ServerDisConnect(j) < 0) {
							bIsAgain = TRUE;
							break;
						}

						vecAlreadyDo.push_back(j);
					}
				}

				if (bIsAgain) continue;

				if (ServerDisConnect(i) < 0) continue;
				vecAlreadyDo.push_back(i);
			}
		}

		Sleep(1000);//1 second
	}
}

void CAnykaIPCameraDlg::FullScreenProcess(BOOL bIsFullScreen, int iSelect)
{
#if 0
	CAutoLock lock(&m_csForServerConnect);

	if (iSelect < 0 || iSelect >= PREVIEW_WINDOWS) return;
	if (m_videoRender[iSelect] == NULL) return;

	int iIndex = 0;

	if (bIsFullScreen) {
		if (m_pServerPreviews[iSelect]->IsDisConnect()) return;

		int ret = m_videoRender[iSelect]->FullScreen(TRUE, OnFullScreenMessage, (void *)(this));
		if (ret < 0) return;

		m_nVideoFullScreenIndex = iSelect;
		for (; iIndex < PREVIEW_WINDOWS; ++iIndex) {
			if ((iIndex != iSelect) && m_pServerPreviews[iIndex]) {
				CloseTheStream(iIndex, TRUE);
			}
		}
	}else{
		for (; iIndex < PREVIEW_WINDOWS; ++iIndex) {
			if (iIndex != iSelect && m_pServerPreviews[iIndex]){ 
				if (!m_strURL[iIndex].empty()) {
					if (OpenTheStream(iIndex, m_strURL[iIndex].c_str(), FALSE) < 0) break;
				}

				if (iIndex == m_nAudioClientIndex) { //��Client��ȫ��ǰ������Ƶ��Client
					m_nRBChoosePrevIndex = m_nAudioClientIndex;
					OnPreviewdlgchooseTalkOpen();
				}
			}
		}

		m_nVideoFullScreenIndex = -1;
		m_videoRender[iSelect]->FullScreen(FALSE, NULL, NULL);

	}
#endif
}

void CAnykaIPCameraDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
#if 0
	CRect rect;

	int iSelect = 0;
	for (; iSelect < PREVIEW_WINDOWS; ++iSelect) {
		m_Preview[iSelect].GetWindowRect(&rect);
		ScreenToClient(&rect);

		if (rect.PtInRect(point)) break;
	}

	FullScreenProcess(TRUE, iSelect);

	CDialog::OnLButtonDblClk(nFlags, point);
#endif
}

void CAnykaIPCameraDlg::CameraMovement(CAMERAMOVEMENT movement)
{
	if (!CanDoTheJob()) return;

	IServer * pIServer = (IServer *)m_TreeCtrl.GetItemData(m_hCurrentSelectItem);
	if (pIServer == NULL) {
		AfxMessageBox(L"Can't get the server from tree list.\n");
		return;
	}

	pIServer->SendCameraMovement(movement);
}

void CAnykaIPCameraDlg::OnBnClickedButtonLeft()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_bIsLongPressDone) {
		m_bIsLongPressDone = FALSE;
		return;
	}

	CameraMovement(CMT_STEP_LEFT);
}

void CAnykaIPCameraDlg::OnBnClickedButtonUp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_bIsLongPressDone) {
		m_bIsLongPressDone = FALSE;
		return;
	}

	CameraMovement(CMT_STEP_UP);
}

void CAnykaIPCameraDlg::OnBnClickedButtonRight()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_bIsLongPressDone) {
		m_bIsLongPressDone = FALSE;
		return;
	}

	CameraMovement(CMT_STEP_RIGHT);
}

void CAnykaIPCameraDlg::OnBnClickedButtonDown()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_bIsLongPressDone) {
		m_bIsLongPressDone = FALSE;
		return;
	}

	CameraMovement(CMT_STEP_DOWN);
}

void CAnykaIPCameraDlg::OnBnClickedButtonLeftRight()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CameraMovement(CMT_RUN_LEFT_RIGHT);
}

void CAnykaIPCameraDlg::OnBnClickedButtonUpDown()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CameraMovement(CMT_RUN_UP_DOWN);
}

void CAnykaIPCameraDlg::OnBnClickedButtonRepositionSet()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CameraMovement(CMT_SET_REPOSITION);
}

void CAnykaIPCameraDlg::OnBnClickedButtonReposition()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CameraMovement(CMT_RUN_REPOSITION);
}

void CAnykaIPCameraDlg::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CDialog::OnRButtonDblClk(nFlags, point);
}

void CAnykaIPCameraDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CRect rect;
	int iSelect = 0;
	CMenu * pm = NULL;

	for (; iSelect < PREVIEW_WINDOWS; ++iSelect) {
		m_Preview[iSelect].GetWindowRect(&rect);
		ScreenToClient(&rect);

		if (rect.PtInRect(point)) break;
	}

	if ((iSelect >= PREVIEW_WINDOWS) || (NULL == m_pServerPreviews[iSelect])) goto end;

	m_nRBChoosePrevIndex = iSelect;

#ifdef UNATTACHED_TALK
	if (m_NetTalk.GetTalkServer() == m_pServerPreviews[iSelect])
		pm = m_menuTalk.GetSubMenu(1);
	else
		pm = m_menuTalk.GetSubMenu(0);
#else
	if (m_NetTalk.IsTalk() && (m_NetTalk.GetTalkServer() == m_pServerPreviews[iSelect]))
		pm = m_menuTalk.GetSubMenu(1);
	else
		pm = m_menuTalk.GetSubMenu(0);
#endif

	ClientToScreen(&point);
	pm->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	ScreenToClient(&point);

end:
	CDialog::OnRButtonDown(nFlags, point);
}

void CAnykaIPCameraDlg::OnPreviewdlgchooseTalkOpen()
{
	// TODO: �ڴ���������������
	USES_CONVERSION;
	WCHAR astrMsg[100] = {0};

	unsigned int nLen = MAX_ID_LEN;
	char strServerID[MAX_ID_LEN] = {0};

	CAutoLock lock(&m_csForTalkOpen);

	m_nAudioClientIndex = -1;

	m_pServerPreviews[m_nRBChoosePrevIndex]->GetServerID(strServerID, &nLen);

	char strIPAddr[MAX_IP_LEN] = {0};
	nLen = MAX_IP_LEN;

	m_pServerPreviews[m_nRBChoosePrevIndex]->GetServerIp(strIPAddr, &nLen);

	if (OnAudioInOpen(m_nRBChoosePrevIndex) < 0){
		_sntprintf(astrMsg, 100, L"Can't listen audio from %s server, IP = %s!\n",
			A2W(strServerID), A2W(strIPAddr));
		AfxMessageBox( astrMsg, 0, 0 );
		return;
	}

	if (m_NetTalk.IsTalk()) m_NetTalk.StopTalk();

	int ret = m_NetTalk.Talk(m_pServerPreviews[m_nRBChoosePrevIndex], OnTalkKickOut, this);
#ifdef UNATTACHED_TALK
#else
	if (ret < 0) {
		_sntprintf(astrMsg, 100, L"Can't talk to %s server, IP = %s!\n", A2W(strServerID), A2W(strIPAddr));
		OnAudioInClose(m_nRBChoosePrevIndex);
		AfxMessageBox(astrMsg, 0, 0);
		return;
	}
#endif

	m_nAudioClientIndex = m_nRBChoosePrevIndex;
}

void CAnykaIPCameraDlg::TempCloseTalk(int iIndex)
{
	OnAudioInClose(iIndex);
	m_NetTalk.StopTalk();
}

void CAnykaIPCameraDlg::OnPreviewdlgchooseTalkClose()
{
	// TODO: �ڴ���������������
	TempCloseTalk(m_nAudioClientIndex);
	m_nAudioClientIndex = -1;
}

LRESULT CAnykaIPCameraDlg::OnTalkKickOutMessage(WPARAM wParam, LPARAM lParam)
{
	USES_CONVERSION;

	if (lParam == NULL) return 0;

	IServer * pTalkServer = (IServer *)lParam;

	if ((m_nAudioClientIndex == -1) || (pTalkServer != m_NetTalk.GetTalkServer())) return 0;

	unsigned long ulSendAudioAddr = 0;
	unsigned short usPort = 0;
	m_NetTalk.GetSendAudioSocketIp(ulSendAudioAddr, usPort);

	if ((ulSendAudioAddr != 0) && (usPort != 0)) {
		CAutoLock lock(&m_csForKickOut);
		if ((ulSendAudioAddr != m_stKickOutParam.ulIpAddr) 
			|| ((unsigned long)usPort != m_stKickOutParam.ulPort))
			return 0;
	}

	unsigned int nLen = MAX_ID_LEN;
	char strServerID[MAX_ID_LEN] = {0};
	m_pServerPreviews[m_nAudioClientIndex]->GetServerID(strServerID, &nLen);

	char strIPAddr[MAX_IP_LEN] = {0};
	nLen = MAX_IP_LEN;
	m_pServerPreviews[m_nAudioClientIndex]->GetServerIp(strIPAddr, &nLen);

	OnAudioInClose(m_nAudioClientIndex);
	m_NetTalk.StopTalk(FALSE);
	m_nAudioClientIndex = -1;

	WCHAR astrMsg[1024] = {0};
	_sntprintf(astrMsg, 100, L"�����ͻ��˺ͷ�����:[%s(IP=%s)]��������˫��Խ����ӣ����ͻ��˵ĶԽ����ӱ��������߳���\n", A2W(strServerID), A2W(strIPAddr));
	AfxMessageBox(astrMsg, 0, 0);

	return 0;
}

void CAnykaIPCameraDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CAnykaIPCameraDlg::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CDialog::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

void CAnykaIPCameraDlg::RepositionWidget()
{
	if (!m_bIsInit) return;

	//RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	//Status bar
	//CRect rect;
	//GetWindowRect(&rect);

	//m_StatusBar.SetPaneInfo(0, ID_STATUSBAR_PROGRAMINFO,  0, rect.Width() * SBAR_PROGRAMINFO_SCALE);
	//m_StatusBar.SetPaneInfo(1, ID_STATUSBAR_DISPLAYINFO1, 0, rect.Width() * SBAR_DISPLAYINFO_SCALE);
	//m_StatusBar.SetPaneInfo(2, ID_STATUSBAR_DISPLAYINFO2, 0, rect.Width() * SBAR_DISPLAYINFO_SCALE);

	//Tree List
	//InitTreeCtrlPosition();

	//Preview Windows
	if (g_Full_flag)
	{
		g_Full_flag = FALSE;
	}
	else
	{
		g_Full_flag = TRUE;
	}
	InitPreviewWindows(FALSE, g_Full_flag);



	//button
	//PositionTheButton();

	//Combo
	//PositionTheImageCombo();
}

void CAnykaIPCameraDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	RepositionWidget();
	// TODO: �ڴ˴������Ϣ����������
	if (IsWindow(m_RightDlg.m_hWnd))
	{
		CRect rcDlg1,rcDlg2;
		GetClientRect(&rcDlg1);
		m_RightDlg.GetClientRect(&rcDlg2);
		m_RightDlg.SetWindowPos(NULL,rcDlg1.Width()-rcDlg2.Width(),0,0,0,SWP_NOSIZE|SWP_SHOWWINDOW);
	}

	if (IsWindow(m_BottomDlg.m_hWnd))
	{
		CRect rcDlg1,rcDlg2;
		GetClientRect(&rcDlg1);
		m_BottomDlg.GetClientRect(&rcDlg2);
		m_BottomDlg.SetWindowPos(NULL,0,rcDlg1.Height()-rcDlg2.Height(),0,0,SWP_NOSIZE|SWP_SHOWWINDOW);
	}

	if (IsWindow(m_Preview[0].m_hWnd))
	{
		CRect rcDlg1,rcDlg2,rcDlg3;
		GetClientRect(&rcDlg1);
		m_RightDlg.GetClientRect(&rcDlg2);
		m_BottomDlg.GetClientRect(&rcDlg3);
		m_Preview[0].SetWindowPos(NULL,0,0,rcDlg1.Width()-rcDlg2.Width()-10,
			rcDlg1.Height()-rcDlg3.Height()-10,SWP_NOMOVE|SWP_SHOWWINDOW);
	}
}

#define BUTTON_WIDTH_APART		1
#define BUTTON_HEIGHT_APART		1

void CAnykaIPCameraDlg::PositionTheButton()
{
	CRect cPreview1Rect, cWindowRect;

	GetWindowRect(&cWindowRect);

	m_Preview[0].GetWindowRect(&cPreview1Rect);

	ScreenToClient(&cPreview1Rect);
	ScreenToClient(&cWindowRect);

	int nWidthMid = (cWindowRect.right - cPreview1Rect.right) / 2;
	int nHeightMid = (cPreview1Rect.bottom - cPreview1Rect.top) / 2;

	CButton * pUpButton =  (CButton *)GetDlgItem(IDC_BUTTON_UP);
	CButton * pLeftButton = (CButton *)GetDlgItem(IDC_BUTTON_LEFT);
	CButton * pRightButton = (CButton *)GetDlgItem(IDC_BUTTON_RIGHT);
	CButton * pDownButton = (CButton *)GetDlgItem(IDC_BUTTON_DOWN);
	CButton * pLRButton =  (CButton *)GetDlgItem(IDC_BUTTON_LEFTRIGHT);
	CButton * pUDButton =  (CButton *)GetDlgItem(IDC_BUTTON_UPDOWN);
	CButton * pSetButton =  (CButton *)GetDlgItem(IDC_BUTTON_REPOSITION_SET);
	CButton * pRepositionButton =  (CButton *)GetDlgItem(IDC_BUTTON_REPOSITION);

	CRect cButtonRect;
	pUpButton->GetWindowRect(&cButtonRect);
	ScreenToClient(&cButtonRect);

	int x = cPreview1Rect.right + (nWidthMid - (cButtonRect.Width() / 2));
	int y = cPreview1Rect.top + (nHeightMid - (cButtonRect.Height() / 2)) - cButtonRect.Height() * 2;
	pUpButton->MoveWindow(x, y, cButtonRect.Width(), cButtonRect.Height());

	pUpButton->GetWindowRect(&cButtonRect);
	ScreenToClient(&cButtonRect);

	x = cButtonRect.left - cButtonRect.Width() + 2 * BUTTON_WIDTH_APART;
	y = cButtonRect.bottom - 2 * BUTTON_HEIGHT_APART;
	pLeftButton->MoveWindow(x, y, cButtonRect.Width(), cButtonRect.Height());

	x = cButtonRect.right - 2 * BUTTON_WIDTH_APART;
	pRightButton->MoveWindow(x, y, cButtonRect.Width(), cButtonRect.Height());

	x = cButtonRect.left;
	y += (cButtonRect.Height() - 2 * BUTTON_WIDTH_APART);
	pDownButton->MoveWindow(x, y, cButtonRect.Width(), cButtonRect.Height());

	pLeftButton->GetWindowRect(&cButtonRect);
	ScreenToClient(&cButtonRect);

	x = cButtonRect.right - cButtonRect.Width() / 2;
	y = cButtonRect.bottom + (2 * BUTTON_HEIGHT_APART) + cButtonRect.Height();
	pLRButton->MoveWindow(x, y, cButtonRect.Width(), cButtonRect.Height());

	pLRButton->GetWindowRect(&cButtonRect);
	ScreenToClient(&cButtonRect);

	x = cButtonRect.right + BUTTON_WIDTH_APART;
	pUDButton->MoveWindow(x, cButtonRect.top, cButtonRect.Width(), cButtonRect.Height());

	pLRButton->GetWindowRect(&cButtonRect);
	ScreenToClient(&cButtonRect);

	x = cButtonRect.left;
	y = cButtonRect.bottom + BUTTON_HEIGHT_APART;
	pSetButton->MoveWindow(x, y, cButtonRect.Width(), cButtonRect.Height());

	pUDButton->GetWindowRect(&cButtonRect);
	ScreenToClient(&cButtonRect);

	x = cButtonRect.left;
	y = cButtonRect.bottom + BUTTON_HEIGHT_APART;
	pRepositionButton->MoveWindow(x, y, cButtonRect.Width(), cButtonRect.Height());
}

#define COMBO_HEIGHT_APART	8
#define COMBO_WIDTH_APART	1

void CAnykaIPCameraDlg::PositionTheImageCombo()
{
	CRect cPreview2Rect, cWindowRect;

	GetWindowRect(&cWindowRect);

	m_Preview[1].GetWindowRect(&cPreview2Rect);

	ScreenToClient(&cPreview2Rect);
	ScreenToClient(&cWindowRect);

	int nWidthMid = (cWindowRect.right - cPreview2Rect.right) / 2;
	int nHeightMid = (cPreview2Rect.bottom - cPreview2Rect.top) / 2;

	CWnd * pStaticText1 = GetDlgItem(IDC_STATIC1);
	CWnd * pStaticText2 = GetDlgItem(IDC_STATIC2);
	CWnd * pStaticText3 = GetDlgItem(IDC_STATIC4);
	CWnd * pStaticText4 = GetDlgItem(IDC_STATIC5);

	CRect cTextRect;
	pStaticText1->GetWindowRect(&cTextRect);
	ScreenToClient(&cTextRect);

	CRect cComboRect;
	m_ContrastCombo.GetWindowRect(&cComboRect);
	ScreenToClient(&cComboRect);

	int x = cPreview2Rect.right + 
		nWidthMid - (cTextRect.Width() +  cComboRect.Width() + COMBO_WIDTH_APART) / 2;
	int y = cPreview2Rect.top + 
		nHeightMid - ((cComboRect.Height() + COMBO_HEIGHT_APART) / 2) - cComboRect.Height() * 2;

	pStaticText1->MoveWindow(x, y + (cComboRect.Height() / 2) - (cTextRect.Height() / 2),
		cTextRect.Width(), cTextRect.Height());
	m_ContrastCombo.MoveWindow(x + cTextRect.Width() + COMBO_WIDTH_APART, 
		y, cComboRect.Width(), cComboRect.Height());

	pStaticText1->GetWindowRect(&cTextRect);
	ScreenToClient(&cTextRect);

	m_ContrastCombo.GetWindowRect(&cComboRect);
	ScreenToClient(&cComboRect);

	x = cTextRect.left;
	y = cComboRect.bottom + COMBO_HEIGHT_APART + (cComboRect.Height() / 2) - (cTextRect.Height() / 2);
	pStaticText2->MoveWindow(x, y, cTextRect.Width(), cTextRect.Height());
	x = cComboRect.left;
	y = cComboRect.bottom + COMBO_HEIGHT_APART;
	m_SaturationCombo.MoveWindow(x, y, cComboRect.Width(), cComboRect.Height());

	pStaticText2->GetWindowRect(&cTextRect);
	ScreenToClient(&cTextRect);

	m_SaturationCombo.GetWindowRect(&cComboRect);
	ScreenToClient(&cComboRect);

	x = cTextRect.left;
	y = cComboRect.bottom + COMBO_HEIGHT_APART + (cComboRect.Height() / 2) - (cTextRect.Height() / 2);
	pStaticText3->MoveWindow(x, y, cTextRect.Width(), cTextRect.Height());
	x = cComboRect.left;
	y = cComboRect.bottom + COMBO_HEIGHT_APART;
	m_BrightnessCombo.MoveWindow(x, y, cComboRect.Width(), cComboRect.Height());

	pStaticText3->GetWindowRect(&cTextRect);
	ScreenToClient(&cTextRect);

	m_BrightnessCombo.GetWindowRect(&cComboRect);
	ScreenToClient(&cComboRect);

	x = cTextRect.left;
	y = cComboRect.bottom + COMBO_HEIGHT_APART + (cComboRect.Height() / 2) - (cTextRect.Height() / 2);
	pStaticText4->MoveWindow(x, y, cTextRect.Width(), cTextRect.Height());
	x = cComboRect.left;
	y = cComboRect.bottom + COMBO_HEIGHT_APART;
	m_acutanceCom.MoveWindow(x, y, cComboRect.Width(), cComboRect.Height());
}

int CAnykaIPCameraDlg::ShutDownTheStream(int iSelect)
{
	CAutoLock lock(&m_csForServerConnect);
	CloseTheStream(iSelect, TRUE);

	if (iSelect == m_nAudioClientIndex)
		m_nAudioClientIndex = -1;
	return UnregisterThePreviewServer(iSelect);
}

void CAnykaIPCameraDlg::OnPreviewdlgchoose1ClosePreview()
{
	// TODO: �ڴ���������������
	ShutDownTheStream(m_nRBChoosePrevIndex);
}

void CAnykaIPCameraDlg::OnPreviewdlgchooseClosePreview()
{
	// TODO: �ڴ���������������
	ShutDownTheStream(m_nRBChoosePrevIndex);
}

//�����˳�ʱʹ�ô˺����ȴ�Monitor�߳̽�������ֹ�ȴ��̵߳Ĺ�������Ϣ�����󲻱���������ɵĳ���������
void CAnykaIPCameraDlg::WiatForMonitorThreadEnd()
{
	DWORD result;
	MSG msg;

	while(TRUE) {
		//MsgWaitForMultipleObjects API�ȴ�Ŀ���߳̽���ʱ������ȴ����߳��յ���Ϣ���򷵻ء�
		result = MsgWaitForMultipleObjects(1, &m_MonitorThread, FALSE, INFINITE, QS_ALLINPUT);
		if (result == WAIT_OBJECT_0)
			break;//�ȴ���Ŀ���߳̽����ˡ�
		else {
			PeekMessage(&msg, NULL, 0, 0, PM_REMOVE); //�ȴ������б��߳��յ��˴�����Ϣ��
			DispatchMessage(&msg);
		}
	}
}

void CAnykaIPCameraDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
#if 0
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CWnd * pButtonLeft = GetDlgItem(IDC_BUTTON_LEFT);
	CWnd * pButtonRight = GetDlgItem(IDC_BUTTON_RIGHT);
	CWnd * pButtonUp = GetDlgItem(IDC_BUTTON_UP);
	CWnd * pButtonDown = GetDlgItem(IDC_BUTTON_DOWN);

	m_nLongPressButtonID = -1;
	m_bIsLongPress = FALSE;

	CRect rect;
	pButtonLeft->GetWindowRect(rect);
	ScreenToClient(rect);
	if (rect.PtInRect(point))
		m_nLongPressButtonID = IDC_BUTTON_LEFT;

	pButtonRight->GetWindowRect(rect);
	ScreenToClient(rect);
	if (rect.PtInRect(point))
		m_nLongPressButtonID = IDC_BUTTON_RIGHT;

	pButtonUp->GetWindowRect(rect);
	ScreenToClient(rect);
	if (rect.PtInRect(point))
		m_nLongPressButtonID = IDC_BUTTON_UP;

	pButtonDown->GetWindowRect(rect);
	ScreenToClient(rect);
	if (rect.PtInRect(point))
		m_nLongPressButtonID = IDC_BUTTON_DOWN;

	if (m_nLongPressButtonID != -1) { //LBUTTONDWON����̨��ť��
		SetTimer(TIMER_LONG_PRESS, 500, NULL);
	}

	CDialog::OnLButtonDown(nFlags, point);
#endif
}

void CAnykaIPCameraDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
#if 0
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (m_bIsLongPress) {//������������Ҫ����STOP��Ϣ�������
		KillTimer(TIMER_LONG_PRESS);

		if (m_nLongPressButtonID == IDC_BUTTON_LEFT) {
			CameraMovement(CMT_STEP_LEFT_CONTINUE_STOP);
			m_bIsLongPressDone = TRUE;
		}else if (m_nLongPressButtonID == IDC_BUTTON_RIGHT) {
			CameraMovement(CMT_STEP_RIGHT_CONTINUE_STOP);
			m_bIsLongPressDone = TRUE;
		}else if (m_nLongPressButtonID == IDC_BUTTON_UP) {
			CameraMovement(CMT_STEP_UP_CONTINUE_STOP);
			m_bIsLongPressDone = TRUE;
		}else if (m_nLongPressButtonID == IDC_BUTTON_DOWN) {
			CameraMovement(CMT_STEP_DOWN_CONTINUE_STOP);
			m_bIsLongPressDone = TRUE;
		}else {
		}
	}

	m_nLongPressButtonID = -1;
	m_bIsLongPress = FALSE;

	CDialog::OnLButtonUp(nFlags, point);
#endif
}

#define PROCESS_MOVE_OUT(x) \
{\
	KillTimer(TIMER_LONG_PRESS);\
	CameraMovement((x));\
	m_bIsLongPressDone = TRUE;\
	m_nLongPressButtonID = -1;\
	m_bIsLongPress = FALSE;\
}

void CAnykaIPCameraDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (m_bIsLongPress) {//���������У�����Ƴ���̨��ť����н��������Ĵ���
		CWnd * pButtonLeft = GetDlgItem(IDC_BUTTON_LEFT);
		CWnd * pButtonRight = GetDlgItem(IDC_BUTTON_RIGHT);
		CWnd * pButtonUp = GetDlgItem(IDC_BUTTON_UP);
		CWnd * pButtonDown = GetDlgItem(IDC_BUTTON_DOWN);

		CRect RectLeftWnd, RectRightWnd, RectUpWnd, RectDownWnd;
		pButtonLeft->GetWindowRect(RectLeftWnd);
		ScreenToClient(RectLeftWnd);

		pButtonRight->GetWindowRect(RectRightWnd);
		ScreenToClient(RectRightWnd);

		pButtonUp->GetWindowRect(RectUpWnd);
		ScreenToClient(RectUpWnd);

		pButtonDown->GetWindowRect(RectDownWnd);
		ScreenToClient(RectDownWnd);

		if (m_nLongPressButtonID == IDC_BUTTON_LEFT && !RectLeftWnd.PtInRect(point)) {
			PROCESS_MOVE_OUT(CMT_STEP_LEFT_CONTINUE_STOP);
		}else if (m_nLongPressButtonID == IDC_BUTTON_RIGHT && !RectRightWnd.PtInRect(point)) {
			PROCESS_MOVE_OUT(CMT_STEP_RIGHT_CONTINUE_STOP);
		}else if (m_nLongPressButtonID == IDC_BUTTON_UP && !RectUpWnd.PtInRect(point)) {
			PROCESS_MOVE_OUT(CMT_STEP_UP_CONTINUE_STOP);
		}else if (m_nLongPressButtonID == IDC_BUTTON_DOWN && !RectDownWnd.PtInRect(point)) {
			PROCESS_MOVE_OUT(CMT_STEP_DOWN_CONTINUE_STOP);
		}else {
		}
	}

	CDialog::OnMouseMove(nFlags, point);
}

//����������˷��ص���Ϣ��
LRESULT CAnykaIPCameraDlg::OnServerRetInfo(WPARAM wParam, LPARAM lParam)
{
	USES_CONVERSION;

	if (lParam == NULL) return 0;

	IServer * pRetInfoServer = (IServer *)lParam;

	int i = 0;
	for (; i < PREVIEW_WINDOWS; ++i) {
		if (pRetInfoServer == m_pServerPreviews[i]) break;
	}

	if (i >= PREVIEW_WINDOWS) //this return info server current no preview 
		return 0;

	unsigned int nLen = MAX_ID_LEN;
	char strServerID[MAX_ID_LEN] = {0};
	pRetInfoServer->GetServerID(strServerID, &nLen);

	char strIPAddr[MAX_IP_LEN] = {0};
	nLen = MAX_IP_LEN;
	pRetInfoServer->GetServerIp(strIPAddr, &nLen);

	CString strInfo, strServerInfo;
	strInfo.Format(L"������%s(%s)����:");

	RETINFO stInfo = {0};

	{
		CAutoLock lock(&m_csForRet);
		memcpy(&stInfo, &m_stRetInfo, sizeof(RETINFO));
	}

	GetStringFromRetInfo(m_stRetInfo, strServerInfo);

	if (strServerInfo.GetLength() <= 0)
		return 0;

	strInfo.Append(strServerInfo);

	AfxMessageBox(strServerInfo);

	return 0;
}


void CAnykaIPCameraDlg::OnCbnSelchangeCombo4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	MakeAndSendImageSet();
}

void CAnykaIPCameraDlg::OnBnClickedRadioIrcutOn()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_ircut_flag = 1;
}

void CAnykaIPCameraDlg::OnBnClickedRadioIrcutOff()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_ircut_flag = 0;
}


BOOL CAnykaIPCameraDlg::Connet_FTPServer(LPCTSTR addr, UINT idex) 
{

	if(m_pInetSession != NULL)
	{
		m_pInetSession->Close();
		delete m_pInetSession;
		m_pInetSession = NULL;
	}
	m_pInetSession = new CInternetSession(AfxGetAppName(), 1, PRE_CONFIG_INTERNET_ACCESS);
	try
	{
		// addr       ftp�������ĵ�ַ  LPCTSTR
		// username   ��½�û���       LPCTSTR 
		// password   ����            LPCTSTR
		// port       �˿�            UINT
		if(m_pFtpConnection != NULL)
		{
			m_pFtpConnection->Close();
			delete m_pFtpConnection;
			m_pFtpConnection = NULL;
		}
		m_pFtpConnection = m_pInetSession->GetFtpConnection(addr, m_username, m_password, 21,FALSE);
		//m_pFtpConnection = m_pInetSession->GetFtpConnection(addr, m_username, m_password, m_uPort);
	}

	catch(CInternetException *pEx)//����½���ɹ����׳��쳣������������쳣�Ĵ���
	{
		TCHAR szError[1024] = {0};

		if(pEx->GetErrorMessage(szError,1024))
		{
			AfxMessageBox(szError);
		}
		else
		{
			AfxMessageBox(_T("There was an exception"));
		}
		pEx->Delete();
		m_pFtpConnection = NULL;
		return FALSE;
	}

	return TRUE;
}


BOOL CAnykaIPCameraDlg::ConnetServer(LPCTSTR addr, UINT idex) 
{
	// TODO: Add your control notification handler code here
	CString str;

	USES_CONVERSION;

	// TODO: Add your control notification handler code here
	//m_ClientSocket.Create(0, SOCK_STREAM, NULL);
	if (!m_ClientSocket.Socket_Create(idex))
	{
		AfxMessageBox(_T("Socket_Create fail"));
		return FALSE;
	}


	//if(m_ClientSocket.Connect(addr, 6789))
	if(m_ClientSocket.Socket_Connect(T2A(addr), 6789, idex))	
	{
		//����ftp������
		//����ftp������
		if (!Connet_FTPServer(addr, idex))
		{
			AfxMessageBox(_T("Connet_FTPServer fail"));
			return FALSE;
		}

		if (!create_thread_heat(addr, idex))
		{
			AfxMessageBox(_T("���������߳�ʧ��"));
			return FALSE;
		}


		if (!create_thread_rev_data(idex))
		{
			AfxMessageBox(_T("�������������߳�ʧ��"));
			return FALSE;
		}
		g_senddata_flag = FALSE;

	}
	else
	{
		AfxMessageBox(_T("Socket_Connect fail"));
		return FALSE;
	}

	return TRUE;
}


BOOL CAnykaIPCameraDlg::Anyka_Test_check_info(void)
{
	UINT time1 = 0;
	UINT time2 = 0;
	UINT delaytime = 0;

	USES_CONVERSION;

	time1 = GetTickCount();
	while (1)
	{
		time2 = GetTickCount();
		Sleep(50);
		if (time2 - time1 > 30000)
		{
			g_test_pass_flag = 0;
			AfxMessageBox(_T("��ʱ(10s)û�з���ȷ������"));
			return FALSE;
		}
		if (g_test_pass_flag == 1)
		{

			g_test_pass_flag = 0;
			return TRUE;
		}
		else if (g_test_pass_flag == 2)
		{
			g_test_pass_flag = 0;
			return FALSE;
		}
	}

}

BOOL CAnykaIPCameraDlg::find_file_indir(TCHAR *file_name, UINT *name_len) 
{

	CFileFind ff;
	CString filename;
	CString szDir;
	DWORD  len = 0;

	szDir.Format(_T("%s/*"), ConvertAbsolutePath(TEST_CONFIG_DIR));
	BOOL res = ff.FindFile(szDir);
	while( res )
	{
		res = ff.FindNextFile();
		filename = ff.GetFileName();
		len = ff.GetLength();

		if(!ff.IsDirectory() && !ff.IsDots())
		{
			_tcscpy(file_name, filename);
			*name_len = len;
			ff.Close(); 
			return TRUE;
		}
	}
	*name_len = len;
	ff.Close(); 
	return FALSE;
}


BOOL CAnykaIPCameraDlg::OnSend_data()
{
	CString strSourceName, strDestName;
	TCHAR test_Name[MAX_PATH] = {0};
	TCHAR DestName_temp[MAX_PATH] = {0};
	TCHAR DestName[50] =_T("/tmp/");
	UINT name_len = 0;
	char name_buf[MAX_PATH] = {0};
	char param_buf[2] = {0};
	CString str;
	TCHAR *file_name = _T("test_ircut");

	USES_CONVERSION;

	//if (!find_file_indir(file_name,&name_len))
	//{
	//	AfxMessageBox(_T("û���ҵ��κ��ļ���������С��"), MB_OK);
	//	return FALSE;
	//}


	memset(test_Name, 0,  MAX_PATH);
	_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
	_tcscat(test_Name, _T("//"));
	_tcscat(test_Name, file_name);

	//�ж��ļ��Ƿ����
	if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(test_Name)))
	{
		str.Format(_T("%s no exist,������С��"), test_Name);
		AfxMessageBox(str, MB_OK); 
		return FALSE;
	}

	//�����ļ�
	memset(DestName_temp, 0, MAX_PATH);
	_tcsncpy(DestName_temp, DestName, sizeof(DestName));
	strSourceName.Format(_T("%s"), ConvertAbsolutePath(test_Name));
	_tcscat(DestName_temp, file_name);
	strDestName.Format(_T("%s"), DestName_temp);

	if (!download_file_flag)
	{
		if (!m_pFtpConnection->PutFile(strSourceName, strDestName, FTP_TRANSFER_TYPE_BINARY, 1))   
		{
			AfxMessageBox(_T("Error no auto test putting file,������С��"), MB_OK);  
			download_file_flag = FALSE;
			return FALSE;
		}
		download_file_flag = TRUE;
	}

	//��������
	name_len = strlen(T2A(file_name));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(file_name), name_len);

	g_test_pass_flag = 0;

	memset(param_buf, 0, 2);

	if (m_ircut_flag)
	{
		memcpy(param_buf, "1", 1);
	}
	else
	{
		memcpy(param_buf, "2", 1);
	}


	if (!Send_cmd(TEST_COMMAND, 1, name_buf, param_buf))
	{
		AfxMessageBox(_T("Send_cmd��fail "), MB_OK); 
		return FALSE;
	}

	//���շ���ֵ
	if (!Anyka_Test_check_info())
	{
		AfxMessageBox(_T("����ʧ��"), MB_OK);
		return FALSE;
	}
	g_test_pass_flag = 0;
	MessageBox(_T("���óɹ�"), MB_OK);
	return TRUE;
}

void CAnykaIPCameraDlg::CloseServer(void) 
{

	if(m_pInetSession != NULL)
	{
		m_pInetSession->Close();
		delete m_pInetSession;
		m_pInetSession = NULL;
	}

	if(m_pFtpConnection != NULL)
	{
		m_pFtpConnection->Close();
		delete m_pFtpConnection;
		m_pFtpConnection = NULL;
	}
	g_send_commad = 0;
	g_test_fail_flag  = 0;
	download_file_flag = FALSE;
	download_dev_file_flag = FALSE;

	close_thread_heat(0);
	close_thread_rev_data();
	m_ClientSocket.Heat_Socket_Close(0);
	m_ClientSocket.Socket_Close(0);

}

void CAnykaIPCameraDlg::OnBnClickedButtonSet()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//���Ƚ�������


	if (!g_connet_flag)
	{
		AfxMessageBox(_T("����û�������ϣ�����"), MB_OK);
		return;
	}
	g_send_commad = 1;
	g_senddata_flag = TRUE;


	//����������ȥ
	if (!OnSend_data())
	{
		g_senddata_flag = FALSE;
		g_send_commad = 0;
		return;
	}
	g_senddata_flag = FALSE;
	g_send_commad = 0;


}

BOOL CAnykaIPCameraDlg::decode_command(char *lpBuf, char *commad_type, char *file_name, char *param)
{
	//���¶���ṹ
	int len_temp = 0;
	int param_len_temp = 0;
	int nFlags = 0;
	T_NET_INFO trance = {0};
	int ret = 0, i = 0;
	char auto_test_flag = 0;
	short check_sum = 0;
	short check_sum_temp = 0;


	//lpBuf�ṹ�ǰ�T_NET_INFO�ṹ�ŷŵ�
	strncpy((char *)&trance.len, lpBuf, 2);

	strncpy(&trance.commad_type, &lpBuf[2], 1);
	*commad_type = trance.commad_type;

	strncpy((char *)&trance.auto_test_flag, &lpBuf[3], 1);
	auto_test_flag = trance.auto_test_flag;

	strncpy((char *)&len_temp, &lpBuf[4], 2);
	if (len_temp > 0 && file_name != NULL)
	{
		strncpy(file_name, &lpBuf[6], len_temp);
	}

	strncpy((char *)&param_len_temp, &lpBuf[6 + len_temp], 2);
	if (param_len_temp > 0 && param != NULL)
	{
		strncpy(param, &lpBuf[8 + len_temp], param_len_temp);
	}

	strncpy((char *)&check_sum, &lpBuf[8 + len_temp + param_len_temp], 2);

	//����
	for (i = 2; i < trance.len - 2; i++)
	{
		check_sum_temp += lpBuf[i];
	}

	if (check_sum != check_sum_temp)
	{
		return FALSE;
	}

	return TRUE;
}





DWORD WINAPI check_rev_date_thread(LPVOID lpParameter)
{
	char commad_type; 
	char *file_name = NULL;
	char *param = NULL;
	CAnykaIPCameraDlg  TestToolDlg;
	CClientSocket m_ClientSocket_heat;
	int ret = 0;
	char lpBuf[256] = {0};
	UINT nBufLen = 256;
	UINT time1 = 0;
	UINT time2 = 0;
	char g_param[256] = {0};
	UINT idex = 0;
	UINT *buf_temp = (UINT *)lpParameter;

	memcpy(&idex, buf_temp, sizeof(UINT));

	//��ȡ��������
	while (1)
	{
		Sleep(100);
		if (g_hBurnThread_rev_data != INVALID_HANDLE_VALUE)
		{
			if (g_send_commad == 1)
			{
				commad_type = 0;  //��ʼ��
				//time1 = GetTickCount();
				ret = m_ClientSocket_heat.Socket_Receive(lpBuf, nBufLen, idex);
				//frmLogfile_temp.WriteLogFile(0,"Socket_Receive ret:%d\n", ret);
				if (ret == -1)
				{
					continue;
#if 0
					if (g_hBurnThread_rev_data== INVALID_HANDLE_VALUE && g_send_commad== 0)
					{
						return TRUE;
					}
					g_test_fail_flag  = 1;
					g_test_pass_flag = 2;
					//g_connet_success_flag = FALSE;
					AfxMessageBox(_T("�������ݴ���������С��������"));
					if (g_hBurnThread_rev_data != INVALID_HANDLE_VALUE)
					{
						CloseHandle(g_hBurnThread_rev_data);
						g_hBurnThread_rev_data = INVALID_HANDLE_VALUE;
					}
					g_send_commad = 0;
					return FALSE;
#endif
				}
				//lpBuf�ṹ�ǰ�T_NET_INFO�ṹ�ŷŵ�
				strncpy(&commad_type, &lpBuf[2], 1);
				if(commad_type == TEST_RESPONSE)
				{
					//frmLogfile_temp.WriteLogFile(0,"commad_type:%d\n", commad_type);
					memset(g_param, 0, 256);
					if (!TestToolDlg.decode_command(lpBuf, &g_commad_type, NULL, g_param))
					{
						g_test_pass_flag = 2;	
					}
					else
					{
						if (g_param[0] == 49)  //49 ��ʾ1
						{
							g_test_pass_flag = 1;
						}
						else
						{
							g_test_pass_flag = 2;
						}
					}
				}
			}
		}
	}

	return 1;
}


BOOL CAnykaIPCameraDlg::create_thread_rev_data(UINT idex) 
{


	if (g_hBurnThread_rev_data != INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_hBurnThread_rev_data);
		g_hBurnThread_rev_data = INVALID_HANDLE_VALUE;
	}

	//param = &idex;
	g_hBurnThread_rev_data = CreateThread(NULL, 0, check_rev_date_thread, rve_param, 0, NULL);
	if (g_hBurnThread_rev_data == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	return TRUE;
}

void CAnykaIPCameraDlg::close_thread_rev_data() 
{
	if(g_hBurnThread_rev_data!= INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_hBurnThread_rev_data);
		g_hBurnThread_rev_data = INVALID_HANDLE_VALUE;
	}
}


DWORD WINAPI check_heartbeat_thread(LPVOID lpParameter)
{
	char commad_type; 
	char *file_name = NULL;
	char *param = NULL;
	CClientSocket m_ClientSocket_heat;
	int ret = 0;
	char lpBuf[256] = {0};
	UINT nBufLen = 256;
	UINT time1 = 0;
	UINT time2 = 0;
	char g_param[256] = {0};
	UINT idex = 0;
	UINT *buf_temp = (UINT *)lpParameter;

	//memcpy(&idex, buf_temp, 4);

	time1 = GetTickCount();
	time2 = GetTickCount();
	//��ȡ��������
	while (1)
	{
		Sleep(4000);
		if (g_heatThread != INVALID_HANDLE_VALUE)
		{
			commad_type = 0;  //��ʼ��
			ret = m_ClientSocket_heat.Heat_Socket_Receive(lpBuf, nBufLen, 0);
		}
	}


	return 1;
}


BOOL CAnykaIPCameraDlg::create_thread_heat(LPCTSTR addr, UINT idex) 
{
	UINT heat_param = 0;

	USES_CONVERSION;


	//����һ�������̵߳�socket
	if (!m_ClientSocket.Heat_Socket_Create(idex))
	{
		AfxMessageBox(_T("��������sokectʧ��"), MB_OK);
		return FALSE;
	}


	//if(m_ClientSocket.Connect(addr, 6789))
	if(!m_ClientSocket.Heat_Socket_Connect(T2A(addr), m_net_uPort + 1, idex))	
	{
		AfxMessageBox(_T("sokect����ʧ��"), MB_OK);
		return FALSE;
	}

	if (g_heatThread != INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_heatThread);
		g_heatThread = INVALID_HANDLE_VALUE;
	}

	g_heatThread = CreateThread(NULL, 0, check_heartbeat_thread, &heat_param, 0, NULL);
	if (g_heatThread == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("���������߳�ʧ��"), MB_OK);
		return FALSE;
	}

	return TRUE;
}

void CAnykaIPCameraDlg::close_thread_heat(UINT idex) 
{
	if(g_heatThread != INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_heatThread);
		g_heatThread = INVALID_HANDLE_VALUE;
	}
}


BOOL CAnykaIPCameraDlg::Send_cmd(char commad_type, BOOL auto_test_flag, char *file_name, char *param)
{
	char *lpBuf = NULL;
	char *lpBuf_temp = NULL;
	short nBufLen = 0;
	UINT i = 0;
	int nFlags = 0;
	short len_name = 0;
	short len_data = 0;
	short check_sum_temp = 0;
	T_NET_INFO trance = {0};
	char temp[5] = {0};
	int ret = 0;
	BOOL no_data_flag = TRUE;
	BOOL no_filename_flag = TRUE;


	USES_CONVERSION;

	trance.data_param = NULL;
	trance.file_name = NULL;


	nBufLen = 2;
	trance.commad_type = commad_type;
	nBufLen += 1;
	check_sum_temp += commad_type;

	trance.auto_test_flag = auto_test_flag;
	nBufLen += 1;
	check_sum_temp += auto_test_flag;

	if (file_name != NULL)
	{
		for (i = 0; i < MAX_PATH && file_name[i] != 0; i++)
		{
			if(file_name[i] != '0')
			{
				no_filename_flag = FALSE;
				break;
			}
		}
	}


	if (file_name != NULL)// && !no_filename_flag)
	{
		len_name = strlen(file_name);
		if(len_name != 0 )
		{
			trance.file_name = (char *)malloc(len_name + 2 + 1);
			if (trance.file_name == NULL)
			{
				return FALSE;
			}
			memset(trance.file_name, 0, len_name + 2 + 1);

			strncpy(trance.file_name, (char *)&len_name, 2);
			strncpy(&trance.file_name[2], file_name, len_name);
			nBufLen += (len_name + 2);
			for (i = 0; i < (UINT)(len_name + 2); i++)
			{
				check_sum_temp += trance.file_name[i];
			}
		}
	}
	else
	{
		nBufLen += 2;
	}

	if (param != NULL)
	{
		for (i = 0; i < MAX_PATH && param[i] != 0; i++)
		{
			if(param[i] != '0')
			{
				no_data_flag = FALSE;
				break;
			}
		}
	}

	if (param != NULL)// && !no_data_flag)
	{
		len_data = strlen(param);
		if(len_data != 0 )
		{
			trance.data_param = (char *)malloc(len_data + 2+1);
			if (trance.data_param == NULL)
			{
				return FALSE;
			}
			memset(trance.data_param, 0, len_data + 2 + 1);

			strncpy(trance.data_param, (char *)&len_data, 2);
			strncpy(&trance.data_param[2], param, len_data);
			nBufLen += (len_data + 2);
			for (i = 0; i < (UINT)(len_data + 2); i++)
			{
				check_sum_temp += trance.data_param[i];
			}
		}
	}
	else
	{
		nBufLen += 2;
	}

	trance.check_sum = check_sum_temp;
	nBufLen +=  2;
	trance.len = nBufLen;


	lpBuf = (char *)malloc(nBufLen + 1);
	if (lpBuf == NULL)
	{
		return FALSE;
	}

	//���
	strncpy(lpBuf, (char *)&trance.len, 2);
	strncpy(&lpBuf[2], &trance.commad_type, 1);
	strncpy(&lpBuf[3], (char *)&trance.auto_test_flag, 1);
	if (len_name != 0 && file_name != NULL && !no_filename_flag)
	{
		strncpy(&lpBuf[4], (char *)&len_name, 2);
		strncpy(&lpBuf[4+2], file_name, len_name);
		//strncpy(&lpBuf[5], trance.file_name, len_name+4);
	}
	else
	{
		strncpy(&lpBuf[4], temp, 2);
	}
	len_name = len_name + 2;//��Ϊ����4���ֽڵĳ���


	if (len_data != 0 && param != NULL && !no_data_flag)
	{
		strncpy(&lpBuf[4+len_name], (char *)&len_data, 2);
		strncpy(&lpBuf[4+len_name+2], param, len_data);
		//strncpy(&lpBuf[5+len_name+4], trance.data_param, len_data+4);
	}
	else
	{
		strncpy(&lpBuf[4 + len_name], temp, 2);
	}
	len_data = len_data + 2;  //��Ϊ����4���ֽڵĳ���


	strncpy(&lpBuf[4+len_name+len_data], (char *)&trance.check_sum, 2);

	for (i = 0; i < 5; i++)
	{
		ret = m_ClientSocket.Socket_Send(lpBuf, nBufLen, 0);
		if (ret)
		{
			break;
		}
		Sleep(100);
	}

	if (!ret)
	{
		return FALSE;
	}

	if (trance.data_param)
	{
		free(trance.data_param);
	}

	if (trance.file_name)
	{
		free(trance.file_name);
	}

	if (lpBuf)
	{
		free(lpBuf);
	}

	if (lpBuf_temp)
	{
		free(lpBuf_temp);
	}

	return TRUE;
}


void CAnykaIPCameraDlg::OnBnClickedButtonRecoverDev()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strSourceName, strDestName;
	TCHAR test_Name[MAX_PATH] = {0};
	TCHAR DestName_temp[MAX_PATH] = {0};
	TCHAR DestName[50] =_T("/tmp/");
	UINT name_len = 0;
	char name_buf[MAX_PATH] = {0};
	char param_buf[2] = {0};
	CString str;
	TCHAR *file_name = _T("test_recover_dev");

	USES_CONVERSION;

	//if (!find_file_indir(file_name,&name_len))
	//{
	//	AfxMessageBox(_T("û���ҵ��κ��ļ���������С��"), MB_OK);
	//	return FALSE;
	//}

	if (!g_connet_flag)
	{
		AfxMessageBox(_T("����û�������ϣ�����"), MB_OK);
		return;
	}
	g_send_commad = 1;

	memset(test_Name, 0,  MAX_PATH);
	_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
	_tcscat(test_Name, _T("//"));
	_tcscat(test_Name, file_name);

	//�ж��ļ��Ƿ����
	if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(test_Name)))
	{
		g_send_commad = 0;
		str.Format(_T("%s no exist,������С��"), test_Name);
		AfxMessageBox(str, MB_OK); 
		return;
	}

	//�����ļ�
	memset(DestName_temp, 0, MAX_PATH);
	_tcsncpy(DestName_temp, DestName, sizeof(DestName));
	strSourceName.Format(_T("%s"), ConvertAbsolutePath(test_Name));
	_tcscat(DestName_temp, file_name);
	strDestName.Format(_T("%s"), DestName_temp);

	if (!download_dev_file_flag)
	{
		if (!m_pFtpConnection->PutFile(strSourceName, strDestName, FTP_TRANSFER_TYPE_BINARY, 1))   
		{
			AfxMessageBox(_T("Error no auto test putting file,������С��"), MB_OK);  
			download_dev_file_flag = FALSE;
			g_send_commad = 0;
			return;
		}
		download_dev_file_flag = TRUE;
	}

	//��������
	name_len = strlen(T2A(file_name));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(file_name), name_len);

	g_test_pass_flag = 0;
	if (!Send_cmd(TEST_COMMAND, 1, name_buf, NULL))
	{
		AfxMessageBox(_T("Send_cmd��fail "), MB_OK); 
		g_send_commad = 0;
		return ;
	}

	//���շ���ֵ
	if (!Anyka_Test_check_info())
	{
		AfxMessageBox(_T("����ʧ��"), MB_OK);
		g_send_commad = 0;
		return ;
	}
	g_test_pass_flag = 0;

	if (!Send_cmd(TEST_COMMAND_FINISH, 1, NULL, NULL))
	{
		AfxMessageBox(_T("���³ɹ�, С���Զ�����ʧ�ܣ����ֶ�����"), MB_OK);  
	}
	else
	{
		if (!Anyka_Test_check_info())
		{
			AfxMessageBox(_T("���³ɹ�, С���Զ�����ʧ�ܣ����ֶ�����"), MB_OK);  
		}
		else
		{
			MessageBox(_T("���óɹ�"), MB_OK);
		}
	}
	g_send_commad = 0;
}


void CAnykaIPCameraDlg::OnBnClickedButtonConfigure()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strUser;
	int index = 0;

	//index=m_login_test_config.m_combo_username.GetCurSel();//��ȡѡ��������
	//m_login_test_config.m_combo_username.GetLBText(index,strUser);

	if (g_test_config.config_auto_login)
	{
		m_login_test_config.m_login_password.Format(_T("%s"),g_test_config.pass_word);
	}
	login_entern_flag = TRUE;
	entern_flag = TRUE;
	if(m_login_test_config.DoModal() == IDOK)
	{
		//CComboBox* userComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_USER);
		//index=userComboBox->GetCurSel();
		//userComboBox->GetLBText(index,strUser);
		//userComboBox->GetWindowTextW(strUser);
		if ((m_login_test_config.m_login_user=="������")&&
			(_tcscmp(m_login_test_config.m_login_password, g_test_config.pass_word) == 0))
		{
			user_producer=TRUE;
			m_enter_test_config.DoModal();
			//m_enter_test_config.UpdateWindow();
		}
		else if((m_login_test_config.m_login_user=="�з���")&&
			(_tcscmp(m_login_test_config.m_login_password, g_test_config.pass_word_research) == 0))
		{
			user_producer=FALSE;
			m_enter_test_config.DoModal();
		}
		else
		{
			AfxMessageBox(_T("�������������"));
		}
		if (!g_test_config.config_auto_login)
		{
			m_login_test_config.m_login_password = _T("");
		}
	}
}


void CAnykaIPCameraDlg::On_find_ip()
{
	CServerSearch search;
	UINT i = 0;

	//g_sousuo_flag = TRUE;

	g_sousuo_flag = FALSE;
	m_ip_address_idex = 0;
	m_find_IP_end_flag = FALSE;

	for (i = 0; i < MAX_PATH; i++)
	{
		memset(g_test_config.m_current_config[i].Current_IP_UID, 0, MAC_ADDRESS_LEN);
		memset(g_test_config.m_current_config[i].Current_IP_address_buffer, 0, IP_ADDRE_LEN);
		memset(g_test_config.m_current_config[i].Current_IP_diver_name, 0, MAX_PATH);
		memset(g_test_config.m_current_config[i].Current_IP_version, 0, MAC_ADDRESS_LEN);
	}

	search.Broadcast_Search();

}

void CAnykaIPCameraDlg::OnBnClickedButtonStart()
{
	UINT i = 0;
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	// TODO: Add your control notification handler code here

	frmLogfile.WriteLogFile(0,"*********************************************************************************\r\n");
	frmLogfile.WriteLogFile(LOG_LINE_TIME | LOG_LINE_DATE,"Open ipc_test.exe\r\n");


	m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);//����
	m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("ͨ��(Y/y)"));//���� 
	m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->SetWindowText(_T("��ͨ��(�ո�)"));//����
	m_RightDlg.m_Status.SetFontColor(0);

	if(g_test_config.config_lan_mac_enable==FALSE&&g_test_config.config_uid_enable==FALSE)
	{
		g_test_config.config_lan_mac_enable=config_lan_mac_enable_temp;
		g_test_config.config_uid_enable=config_uid_enable_temp;
	}

#if 0
	//�ж�����汾�Ƿ�һ��
	for(i = 0; i < MAC_ADDRESS_LEN; i++)
	{
		if(g_test_config.m_last_config[current_ip_idex].Current_IP_version[i] != g_test_config.newest_version[i])
		{
			break;
		}
	}
	if(i != MAC_ADDRESS_LEN)
	{
		AfxMessageBox(_T("����汾�Ų�һ�£�����"), MB_OK);
		m_BottomDlg.GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(FALSE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(FALSE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(FALSE);//����
		return;
	}
#endif


	CloseServer();
	if (!ConnetServer(m_connect_ip, 0))
	{
		g_connet_flag = FALSE;
		AfxMessageBox(_T("��������ʧ�ܣ�����"), MB_OK);
		m_BottomDlg.GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(FALSE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(FALSE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(FALSE);//����
		CloseServer();
		return ;
	}
	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T(""));//���� 
	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT2)->SetWindowText(_T(""));
	Sleep(500);
	g_connet_flag = TRUE;
	if (The_first_case())
	{

		m_BottomDlg.GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(TRUE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(TRUE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(TRUE);//����

		m_RightDlg.m_test_pass_btn.SetFocus();
	}
	else
	{
		g_connet_flag = FALSE;
		m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
		CloseServer();
	}

	g_test_config.config_video_test_pass = FALSE;
	g_test_config.config_voice_rev_test_pass = FALSE;  //����
	g_test_config.config_voice_send_test_pass = FALSE; //�Խ�
	g_test_config.config_head_test_pass = FALSE;
	g_test_config.config_sd_test_pass = FALSE;
	g_test_config.config_wifi_test_pass = FALSE;
	g_test_config.config_red_line_test_pass = FALSE;
	g_test_config.config_reset_test_pass = FALSE;
	g_test_config.config_lan_mac_test_pass = FALSE;
	g_test_config.config_uid_test_pass = FALSE;
	g_test_config.config_ircut_on_test_pass = FALSE;
	g_test_config.config_ircut_off_test_pass = FALSE;
	start_test  = TRUE;

	return;

}


void CAnykaIPCameraDlg::show_IP_info()
{
	UINT i = 0, len = 0;
	CString str;

	if (m_RightDlg.m_test_config.GetSelectedCount() < 1)
	{
		return;
	}

	for (i = m_RightDlg.m_test_config.GetItemCount()-1; i >=0; i--)
	{
		if(m_RightDlg.m_test_config.GetItemState(i, LVIS_SELECTED))
		{
			/*m_RightDlg.SetDlgItemText(IDC_EDIT_IP, 
				g_test_config.m_current_config[i].Current_IP_address_buffer);
			m_RightDlg.SetDlgItemText(IDC_EDIT_UID, 
				g_test_config.m_current_config[i].Current_IP_UID);
			m_RightDlg.SetDlgItemText(IDC_EDIT_MAC, 
				g_test_config.m_current_config[i].Current_IP_MAC);
			m_RightDlg.SetDlgItemText(IDC_EDIT_VERSION2, 
				g_test_config.m_current_config[i].Current_IP_version);*/

			m_RightDlg.SetDlgItemText(IDC_EDIT_IP, g_test_config.m_last_config[i].Current_IP_address_buffer);
			m_RightDlg.SetDlgItemText(IDC_EDIT_UID, g_test_config.m_last_config[i].Current_IP_UID);
			m_RightDlg.SetDlgItemText(IDC_EDIT_MAC, g_test_config.m_last_config[i].Current_IP_MAC);
			m_RightDlg.SetDlgItemText(IDC_EDIT_VERSION2, g_test_config.m_last_config[i].Current_IP_version);

			/*if (_tcscmp(m_connect_ip, g_test_config.m_current_config[i].Current_IP_address_buffer) != 0)
			{
				current_ip_idex = i;
				g_start_open_flag = TRUE;	
				len = _tcsclen(g_test_config.m_current_config[i].Current_IP_address_buffer);
				_tcsncpy(m_connect_ip, g_test_config.m_current_config[i].Current_IP_address_buffer, len);
			}*/


			if (_tcscmp(m_connect_ip, g_test_config.m_last_config[i].Current_IP_address_buffer) != 0)
			{
				current_ip_idex = i;
				g_start_open_flag = TRUE;	
				len = _tcsclen(g_test_config.m_last_config[i].Current_IP_address_buffer);
				memset(m_connect_ip, 0, MAX_PATH);
				_tcsncpy(m_connect_ip, g_test_config.m_last_config[i].Current_IP_address_buffer, len);
			}

			break;
		}
	}
}

void CAnykaIPCameraDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
	show_IP_info();
}

void CAnykaIPCameraDlg::OnNMSetfocusList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
	//show_IP_info();
}

void CAnykaIPCameraDlg::OnBnClickedButtonClose()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strSourceName, strDestName;
	TCHAR test_Name[MAX_PATH] = {0};
	TCHAR DestName_temp[MAX_PATH] = {0};
	TCHAR DestName[50] =_T("/tmp/");
	UINT name_len = 0;
	char name_buf[MAX_PATH] = {0};
	char param_buf[2] = {0};
	CString str;
	TCHAR *file_name = _T("test_close");

	USES_CONVERSION;
	if (!g_connet_flag)
	{
		CloseServer();
		m_BottomDlg.GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(FALSE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(FALSE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(TRUE);//����
		start_test = FALSE;
		start_flag = FALSE;
		AfxMessageBox(_T("����û�������ϣ�����"), MB_OK);
		return;
	}
	g_send_commad = 1;

	m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//���� 
	m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����

	memset(test_Name, 0,  MAX_PATH);
	_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
	_tcscat(test_Name, _T("/"));
	_tcscat(test_Name, file_name);

	//�ж��ļ��Ƿ����
	if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(test_Name)))
	{
		CloseServer();
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����
		g_send_commad = 0;
		str.Format(_T("�رչ���%s no exist"), test_Name);
		AfxMessageBox(str, MB_OK); 
		return;
	}

	//�����ļ�
	memset(DestName_temp, 0, MAX_PATH);
	_tcsncpy(DestName_temp, DestName, sizeof(DestName));
	strSourceName.Format(_T("%s"), ConvertAbsolutePath(test_Name));
	_tcscat(DestName_temp, file_name);
	strDestName.Format(_T("%s"), DestName_temp);


	if (m_pFtpConnection == NULL || !m_pFtpConnection->PutFile(strSourceName, strDestName, FTP_TRANSFER_TYPE_BINARY, 1))   
	{
		CloseServer();
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
		AfxMessageBox(_T("�رչ��� PutFile fail"), MB_OK);  
		download_dev_file_flag = FALSE;
		g_send_commad = 0;
		return;
	}

	//��������
	name_len = strlen(T2A(file_name));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(file_name), name_len);

	g_test_pass_flag = 0;
	if (!Send_cmd(TEST_COMMAND, 1, name_buf, NULL))
	{
		CloseServer();
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
		AfxMessageBox(_T("�رչ��� Send_cmd fail "), MB_OK); 
		g_send_commad = 0;
		return ;
	}

	//���շ���ֵ
	if (!Anyka_Test_check_info())
	{
		CloseServer();
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
		AfxMessageBox(_T("�ر�ʧ��"), MB_OK);
		g_send_commad = 0;
		return ;
	}
	MessageBox(_T("�رճɹ�"), MB_OK);
	m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
	m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
	g_test_pass_flag = 0;
	g_send_commad = 0;

	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CloseServer();
	//GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(FALSE);//����
	m_BottomDlg.GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(FALSE);//����
	m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//����
	m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����
	m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
	m_BottomDlg.GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(FALSE);//����
	m_BottomDlg.GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(TRUE);//����
	start_test = FALSE;
	start_flag = FALSE;
}

void CAnykaIPCameraDlg::OnBnClickedButtonWriteUid()
{
	// TODO: �ڴ���ӿؼ�֪����������
	if(IDOK == m_Burn_UIdDlg.DoModal() && m_Burn_UIdDlg.UID_PASS_flag)
	{
		if (!ConnetServer(m_connect_ip, 0))
		{
			AfxMessageBox(_T("��������ʧ�ܣ�����"), MB_OK);
			g_connet_flag = FALSE;
			CloseServer();
			return ;
		}
		g_connet_flag = TRUE;
		case_uid();
		CloseServer();

		TCHAR burnUid[MAX_PATH+1] = {0};
		UINT len = 0;

		memset(burnUid, 0, MAX_PATH+1);
		len = _tcslen(g_test_config.m_uid_number);
		_tcsncpy(burnUid, &g_test_config.m_uid_number[len - 16], 16);

		USES_CONVERSION;
		frmLogfile.WriteLogFile(0,"UID��¼�ɹ�,UID:%s\r\n", T2A(burnUid));
		frmLogUidFile.WriteLogFile(0,"UID��¼�ɹ�,UID:%s\r\n", T2A(burnUid));
	}

}


BOOL CAnykaIPCameraDlg::case_ircut_test(BOOL m_ircut_flag)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strSourceName, strDestName;
	TCHAR test_Name[MAX_PATH] = {0};
	TCHAR DestName_temp[MAX_PATH] = {0};
	TCHAR DestName[50] =_T("/tmp/");
	UINT name_len = 0;
	char name_buf[MAX_PATH] = {0};
	char param_buf[2] = {0};
	CString str;
	TCHAR *file_name = _T("test_ircut");

	USES_CONVERSION;

	if (!g_connet_flag)
	{
		AfxMessageBox(_T("����û�������ϣ�����"), MB_OK);
		return FALSE;
	}

	g_send_commad = 1;

	memset(test_Name, 0,  MAX_PATH);
	_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
	_tcscat(test_Name, _T("/"));
	_tcscat(test_Name, file_name);

	//�ж��ļ��Ƿ����
	if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(test_Name)))
	{
		g_send_commad = 0;
		str.Format(_T("%s no exist,������С��"), test_Name);
		AfxMessageBox(str, MB_OK); 
		return FALSE;
	}

	//�����ļ�
	memset(DestName_temp, 0, MAX_PATH);
	_tcsncpy(DestName_temp, DestName, sizeof(DestName));
	strSourceName.Format(_T("%s"), ConvertAbsolutePath(test_Name));
	_tcscat(DestName_temp, file_name);
	strDestName.Format(_T("%s"), DestName_temp);


	if (m_pFtpConnection == NULL || !m_pFtpConnection->PutFile(strSourceName, 
		strDestName, FTP_TRANSFER_TYPE_BINARY, 1))   
	{
		g_send_commad = 0;
		AfxMessageBox(_T("Error no auto test putting file,������С��"), MB_OK);  
		download_file_flag = FALSE;
		return FALSE;
	}
	//��������
	name_len = strlen(T2A(file_name));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(file_name), name_len);

	g_test_pass_flag = 0;

	memset(param_buf, 0, 2);

	if (m_ircut_flag)
	{
		memcpy(param_buf, "1", 1);
	}
	else
	{
		memcpy(param_buf, "2", 1);
	}

	if (!Send_cmd(TEST_COMMAND, 0, name_buf, param_buf))
	{
		g_send_commad = 0;
		AfxMessageBox(_T("Send_cmd��fail "), MB_OK); 
		return FALSE;
	}

	Anyka_Test_check_info();
	g_send_commad = 0;

	return TRUE;
}

void CAnykaIPCameraDlg::case_video()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}
void CAnykaIPCameraDlg::case_ircut_off()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	case_ircut_test(FALSE);
}
void CAnykaIPCameraDlg::case_ircut_on()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	case_ircut_test(TRUE);
}


DWORD WINAPI paly_pcm_thread(LPVOID lpParameter)
{
	int             cnt;
	HWAVEOUT        hwo;
	WAVEHDR         wh;
	WAVEFORMATEX    wfx;
	HANDLE          wait;
	DWORD  high = 0;
	int file_len = 0,index = 0;
	char *buf = NULL;
	BOOL ret = FALSE;
	DWORD read_len = 0;

	wfx.wFormatTag = WAVE_FORMAT_PCM;//���ò��������ĸ�ʽ
	wfx.nChannels = 1;//������Ƶ�ļ���ͨ������
	wfx.nSamplesPerSec = 8000;//����ÿ���������źͼ�¼ʱ������Ƶ��
	wfx.nAvgBytesPerSec = 16000;//���������ƽ�����ݴ�����,��λbyte/s�����ֵ���ڴ��������С�Ǻ����õ�
	wfx.nBlockAlign = 2;//���ֽ�Ϊ��λ���ÿ����
	wfx.wBitsPerSample = 16;
	wfx.cbSize = 0;//������Ϣ�Ĵ�С
	wait = CreateEvent(NULL, 0, 0, NULL);
	waveOutOpen(&hwo, WAVE_MAPPER, &wfx, (DWORD_PTR)wait, 0L, CALLBACK_EVENT);//��һ�������Ĳ�����Ƶ���װ�������лط�
	waveOutReset(dev);
	InitializeCriticalSection(&cs);

	//fopen_s(&thbgm, "paomo.pcm", "rb");
	//cnt = fread(buf, sizeof(char), 1024 * 1024 * 4, thbgm);//��ȡ�ļ�4M�����ݵ��ڴ������в��ţ�ͨ��������ֵ��޸ģ������߳̿ɱ��������Ƶ���ݵ�ʵʱ���䡣��Ȼ���ϣ��������������Ƶ�ļ���Ҳ��Ҫ��������΢��һ��
	HANDLE hFile = CreateFile(g_test_config.ConvertAbsolutePath(_T("monitor_pcm.pcm")), 
		GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if(INVALID_HANDLE_VALUE == hFile)
	{
		waveOutReset(dev); 
		waveOutClose(hwo);
		LeaveCriticalSection( &cs );
		DeleteCriticalSection(&cs);
		return 0;
	}
	file_len = GetFileSize(hFile, &high);

	HGLOBAL hMem = GlobalAlloc(GMEM_ZEROINIT, file_len + 1);
	if(NULL == hMem)
	{
		waveOutReset(dev); 
		waveOutClose(hwo);
		LeaveCriticalSection( &cs );
		DeleteCriticalSection(&cs);
		return 0;
	}

	buf = (char *)GlobalLock(hMem);

	//buf = (char *)malloc(file_len + 1);
	memset(buf, 0, file_len + 1);

	while(index < file_len)
	{

		ret = ReadFile(hFile, &buf[index], 4*1024, &read_len, NULL);

		index = index + read_len;

		if (ret == 0 || index == file_len)
		{
			break;
		}
	}

	int dolenght = 0;
	int playsize = 4*1024;
	cnt = file_len;
	while (cnt) {//��һ������Ҫ�ر�ע�������ѭ������֮���ܻ�̫����ʱ��ȥ����ȡ����֮��Ĺ�������Ȼ��ÿ��ѭ���ļ�϶���С����ա�������
		wh.lpData = buf + dolenght;
		wh.dwBufferLength = playsize;
		wh.dwFlags = 0L;
		wh.dwLoops = 1L;
		EnterCriticalSection( &cs );
		waveOutPrepareHeader(hwo, &wh, sizeof(WAVEHDR));//׼��һ���������ݿ����ڲ���
		waveOutWrite(hwo, &wh, sizeof(WAVEHDR));//����Ƶý���в��ŵڶ�������whָ��������
		WaitForSingleObject(wait, INFINITE);//�������hHandle�¼����ź�״̬����ĳһ�߳��е��øú���ʱ���߳���ʱ��������ڹ����INFINITE�����ڣ��߳����ȴ��Ķ����Ϊ���ź�״̬����ú�����������
		dolenght = dolenght + playsize;
		cnt = cnt - playsize;
		if (cnt <=  0 )
		{
			break;
		}
	}
	SetEvent(wait);
	waveOutClose(hwo);
	LeaveCriticalSection( &cs );
	DeleteCriticalSection(&cs);
	CloseHandle(hFile);
	GlobalUnlock(buf);
	GlobalFree(hMem);

	play_pcm_finish_flag = TRUE;
	return 1;
}


BOOL CAnykaIPCameraDlg::create_paly_pcm_thread_data(UINT idex) 
{
	UINT rve_param = 0;

	if (g_hBurnThread_play_pcm != INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_hBurnThread_play_pcm);
		g_hBurnThread_play_pcm = INVALID_HANDLE_VALUE;
	}
	//param = &idex;
	g_hBurnThread_play_pcm = CreateThread(NULL, 0, paly_pcm_thread, &rve_param, 0, NULL);
	if (g_hBurnThread_play_pcm == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	return TRUE;
}

void CAnykaIPCameraDlg::close_thread_paly_pcm() 
{
	if(g_hBurnThread_play_pcm != INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_hBurnThread_play_pcm);
		g_hBurnThread_play_pcm = INVALID_HANDLE_VALUE;
	}
}


int CAnykaIPCameraDlg::play_pcm_test(TCHAR *pcm_file_name) 
{
	int             cnt = 0;
	HWAVEOUT        hwo;
	WAVEHDR         wh;
	WAVEFORMATEX    wfx;
	HANDLE          wait;
	DWORD  high = 0;
	int file_len = 0,index = 0, will_len = 0;
	char *buf = NULL;
	BOOL ret = FALSE;
	DWORD read_len = 0;
	CPcmSpeaker ps; 

	wfx.wFormatTag = WAVE_FORMAT_PCM;//���ò��������ĸ�ʽ
	wfx.nChannels = 1;//������Ƶ�ļ���ͨ������
	wfx.nSamplesPerSec = 8000;//����ÿ���������źͼ�¼ʱ������Ƶ��
	wfx.nAvgBytesPerSec = 16000;//���������ƽ�����ݴ�����,��λbyte/s�����ֵ���ڴ��������С�Ǻ����õ�
	wfx.nBlockAlign = 2;//���ֽ�Ϊ��λ���ÿ����
	wfx.wBitsPerSample = 16;
	wfx.cbSize = 0;//������Ϣ�Ĵ�С
	wait = CreateEvent(NULL, 0, 0, NULL);
	waveOutOpen(&hwo, WAVE_MAPPER, &wfx, (DWORD_PTR)wait, 0L, CALLBACK_EVENT);//��һ�������Ĳ�����Ƶ���װ�������лط�

	//fopen_s(&thbgm, "paomo.pcm", "rb");
	//cnt = fread(buf, sizeof(char), 1024 * 1024 * 4, thbgm);//��ȡ�ļ�4M�����ݵ��ڴ������в��ţ�ͨ��������ֵ��޸ģ������߳̿ɱ��������Ƶ���ݵ�ʵʱ���䡣��Ȼ���ϣ��������������Ƶ�ļ���Ҳ��Ҫ��������΢��һ��
	HANDLE hFile = CreateFile(g_test_config.ConvertAbsolutePath(pcm_file_name), 
		GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, 0, NULL);
	if(INVALID_HANDLE_VALUE == hFile)
	{
		//waveOutClose(hwo);
		return FALSE;
	}
	file_len = GetFileSize(hFile, &high);

	HGLOBAL hMem = GlobalAlloc(GMEM_ZEROINIT, file_len + 1);
	if(NULL == hMem)
	{
		//waveOutClose(hwo);
		return FALSE;
	}

	buf = (char *)GlobalLock(hMem);

	//buf = (char *)malloc(file_len + 1);
	memset(buf, 0, file_len + 1);

	while(index < file_len)
	{
		if(file_len - index > 4*1024)
		{
			will_len = 4*1024;
		}
		else
		{
			will_len = file_len - index;
		}
		ret = ReadFile(hFile, &buf[index], will_len, &read_len, NULL);
		if (!ret && will_len != read_len)
		{
			//waveOutClose(hwo);
			CloseHandle(hFile);
			GlobalUnlock(buf);
			GlobalFree(hMem);
			return FALSE;
		}
		index = index + read_len;

		if (ret == 0 || index == file_len)
		{
			break;
		}
	}

#if 0
	int dolenght = 0;
	int playsize = 4*1024;
	cnt = file_len;
	while (cnt) {//��һ������Ҫ�ر�ע�������ѭ������֮���ܻ�̫����ʱ��ȥ����ȡ����֮��Ĺ�������Ȼ��ÿ��ѭ���ļ�϶���С����ա�������
		wh.lpData = buf + dolenght;
		wh.dwBufferLength = playsize;
		wh.dwFlags = 0L;
		wh.dwLoops = 1L;
		waveOutPrepareHeader(hwo, &wh, sizeof(WAVEHDR));//׼��һ���������ݿ����ڲ���
		waveOutWrite(hwo, &wh, sizeof(WAVEHDR));//����Ƶý���в��ŵڶ�������whָ��������
		WaitForSingleObject(wait, INFINITE);//�������hHandle�¼����ź�״̬����ĳһ�߳��е��øú���ʱ���߳���ʱ��������ڹ����INFINITE�����ڣ��߳����ȴ��Ķ����Ϊ���ź�״̬����ú�����������
		dolenght = dolenght + playsize;
		cnt = cnt - playsize;
		if (cnt <=  0 )
		{
			break;
		}
	}
	waveOutClose(hwo);


#else
	WAVEHDR header;
	ZeroMemory(&header, sizeof(WAVEHDR));
	header.dwBufferLength = index;
	header.lpData = buf;
	waveOutPrepareHeader(hwo, &header, sizeof(WAVEHDR));
	waveOutWrite(hwo, &header, sizeof(WAVEHDR));
	Sleep(500);
	while(waveOutUnprepareHeader(hwo,&header,sizeof(WAVEHDR)) ==WAVERR_STILLPLAYING)
		Sleep(100);

#endif

	waveOutClose(hwo);

	CloseHandle(hFile);
	GlobalUnlock(buf);
	GlobalFree(hMem);
	return TRUE;
}

int CAnykaIPCameraDlg::play_pcm(TCHAR *pcm_file_name) 
{
	//int             cnt;
	//HWAVEOUT        hwo;
	//WAVEHDR         wh;
	//WAVEFORMATEX    wfx;
	//*H*/ANDLE          wait;
	DWORD  high = 0;
	int file_len = 0,index = 0, will_len = 0;
	char *buf = NULL;
	BOOL ret = FALSE;
	DWORD read_len = 0;
	CPcmSpeaker ps; 
#if 0
	wfx.wFormatTag = WAVE_FORMAT_PCM;//���ò��������ĸ�ʽ
	wfx.nChannels = 1;//������Ƶ�ļ���ͨ������
	wfx.nSamplesPerSec = 8000;//����ÿ���������źͼ�¼ʱ������Ƶ��
	wfx.nAvgBytesPerSec = 16000;//���������ƽ�����ݴ�����,��λbyte/s�����ֵ���ڴ��������С�Ǻ����õ�
	wfx.nBlockAlign = 2;//���ֽ�Ϊ��λ���ÿ����
	wfx.wBitsPerSample = 16;
	wfx.cbSize = 0;//������Ϣ�Ĵ�С
	wait = CreateEvent(NULL, 0, 0, NULL);
	waveOutOpen(&hwo, WAVE_MAPPER, &wfx, (DWORD_PTR)wait, 0L, CALLBACK_EVENT);//��һ�������Ĳ�����Ƶ���װ�������лط�
#endif
	//fopen_s(&thbgm, "paomo.pcm", "rb");
	//cnt = fread(buf, sizeof(char), 1024 * 1024 * 4, thbgm);//��ȡ�ļ�4M�����ݵ��ڴ������в��ţ�ͨ��������ֵ��޸ģ������߳̿ɱ��������Ƶ���ݵ�ʵʱ���䡣��Ȼ���ϣ��������������Ƶ�ļ���Ҳ��Ҫ��������΢��һ��
	HANDLE hFile = CreateFile(g_test_config.ConvertAbsolutePath(pcm_file_name), 
		GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if(INVALID_HANDLE_VALUE == hFile)
	{
		//waveOutClose(hwo);
		return FALSE;
	}
	file_len = GetFileSize(hFile, &high);

	HGLOBAL hMem = GlobalAlloc(GMEM_ZEROINIT, file_len + 1);
	if(NULL == hMem)
	{
		//waveOutClose(hwo);
		return FALSE;
	}

	buf = (char *)GlobalLock(hMem);

	//buf = (char *)malloc(file_len + 1);
	memset(buf, 0, file_len + 1);

	while(index < file_len)
	{
		if(file_len - index > 4*1024)
		{
			will_len = 4*1024;
		}
		else
		{
			will_len = file_len - index;
		}
		ret = ReadFile(hFile, &buf[index], will_len, &read_len, NULL);
		if (!ret && will_len != read_len)
		{
			//waveOutClose(hwo);
			CloseHandle(hFile);
			GlobalUnlock(buf);
			GlobalFree(hMem);
			return FALSE;
		}
		index = index + read_len;

		if (ret == 0 || index == file_len)
		{
			break;
		}
	}

	ps.init(1, 8000, 16);
	ps.toSpeaker(buf, file_len, 1);

#if 0
	int dolenght = 0;
	int playsize = 4*1024;
	cnt = file_len;
	while (cnt) {//��һ������Ҫ�ر�ע�������ѭ������֮���ܻ�̫����ʱ��ȥ����ȡ����֮��Ĺ�������Ȼ��ÿ��ѭ���ļ�϶���С����ա�������
		wh.lpData = buf + dolenght;
		wh.dwBufferLength = playsize;
		wh.dwFlags = 0L;
		wh.dwLoops = 1L;
		waveOutPrepareHeader(hwo, &wh, sizeof(WAVEHDR));//׼��һ���������ݿ����ڲ���
		waveOutWrite(hwo, &wh, sizeof(WAVEHDR));//����Ƶý���в��ŵڶ�������whָ��������
		WaitForSingleObject(wait, INFINITE);//�������hHandle�¼����ź�״̬����ĳһ�߳��е��øú���ʱ���߳���ʱ��������ڹ����INFINITE�����ڣ��߳����ȴ��Ķ����Ϊ���ź�״̬����ú�����������
		dolenght = dolenght + playsize;
		cnt = cnt - playsize;
		if (cnt <=  0 )
		{
			break;
		}
	}
	waveOutClose(hwo);
#endif

	CloseHandle(hFile);
	GlobalUnlock(buf);
	GlobalFree(hMem);
	return TRUE;
}

BOOL CAnykaIPCameraDlg::case_monitor()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strSourceName, strDestName;
	TCHAR test_Name[MAX_PATH] = {0};
	TCHAR DestName_temp[MAX_PATH] = {0};
	TCHAR test_param[MAX_PATH] = {0};
	TCHAR DestName[50] =_T("/tmp/");
	UINT name_len = 0;
	char name_buf[MAX_PATH] = {0};
	//char param_buf[2] = {0};
	CString str;
	TCHAR *file_name = _T("test_monitor");
	UINT time1 = 0;
	UINT time2 = 0;
	TCHAR *test_pcm_name = _T("monitor_pcm.pcm");

	USES_CONVERSION;
	if (!g_connet_flag)
	{
		AfxMessageBox(_T("����û�������ϣ�����"), MB_OK);
		return FALSE;
	}
	g_send_commad = 1;

	memset(test_Name, 0,  MAX_PATH);
	_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
	_tcscat(test_Name, _T("/"));
	_tcscat(test_Name, file_name);

	//m_enter_test_config.GetDlgItemText(IDC_TEST_TIME_MONITOR, str);
	GetDlgItemText(IDC_EDIT_WIFI_NAME, str);
	if(init_flag)
	{
		str=g_test_config.m_test_monitor_time;
	}
	if (str.IsEmpty())
	{
		//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		g_send_commad = 0;
		str.Format(_T("��������, ʱ��Ϊ�գ�����д������Ҫ��ʱ����"));
		AfxMessageBox(str, MB_OK); 
		return FALSE;
	}
	memset(g_test_config.m_test_monitor_time, 0, MAC_ADDRESS_LEN);
	_tcscpy(g_test_config.m_test_monitor_time, str);


	m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//���� 
	m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����

	//�ж��ļ��Ƿ����
	if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(test_Name)))
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����

		m_RightDlg.m_test_pass_btn.SetFocus();

		g_send_commad = 0;
		str.Format(_T("monitor���� %s no exist"), test_Name);
		AfxMessageBox(str, MB_OK); 
		return FALSE;
	}

	//�����ļ�
	memset(DestName_temp, 0, MAX_PATH);
	_tcsncpy(DestName_temp, DestName, sizeof(DestName));
	strSourceName.Format(_T("%s"), ConvertAbsolutePath(test_Name));
	_tcscat(DestName_temp, file_name);
	strDestName.Format(_T("%s"), DestName_temp);


	if (m_pFtpConnection == NULL || !m_pFtpConnection->PutFile(strSourceName, 
		strDestName, FTP_TRANSFER_TYPE_BINARY, 1))   
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����

		m_RightDlg.m_test_pass_btn.SetFocus();

		AfxMessageBox(_T("monitor���� PutFile fail"), MB_OK);
		download_dev_file_flag = FALSE;
		g_send_commad = 0;
		return FALSE;
	}

	//��������
	name_len = strlen(T2A(file_name));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(file_name), name_len);
	memset(test_param, 0, MAX_PATH);
	_tcscpy(test_param, test_pcm_name);

	memset(test_param, 0, MAX_PATH+1);
	_tcscpy(test_param, g_test_config.m_test_monitor_time);

	g_test_pass_flag = 0;
	if (!Send_cmd(TEST_COMMAND, 0, name_buf, T2A(test_param)))
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����

		m_RightDlg.m_test_pass_btn.SetFocus();

		AfxMessageBox(_T("monitor���� Send_cmd fail "), MB_OK); 
		g_send_commad = 0;
		return FALSE;
	}

	Sleep(_ttoi(g_test_config.m_test_monitor_time)*1000);

	//��ȡС������Ƶ�ļ�

	memset(DestName_temp, 0, MAX_PATH);
	strDestName.Format(_T("%s"), ConvertAbsolutePath(test_pcm_name));

	_tcsncpy(DestName_temp, DestName, sizeof(DestName));
	_tcscat(DestName_temp, test_pcm_name);
	strSourceName.Format(_T("%s"), DestName_temp);
	time1 = GetTickCount();
	while (1)
	{
		//��ʱ����
		Sleep(500);
		time2 = GetTickCount();
		if (time2 - time1 > 10000)
		{
			m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//����
			m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����

			m_RightDlg.m_test_pass_btn.SetFocus();

			AfxMessageBox(_T("��ȡ�ļ���ʱ�˳�"), MB_OK);  
			g_send_commad = 0;
			return FALSE;
		}

		if (m_pFtpConnection != NULL )
		{
			if (!m_pFtpConnection->GetFile(strSourceName, strDestName, 
				FALSE, FTP_TRANSFER_TYPE_BINARY, 1))   
			{
				continue;
			}
			else
			{
				break;
			}
		}
	}

	//�ж��ļ��Ƿ����
	DWORD faConfig = GetFileAttributes(ConvertAbsolutePath(test_pcm_name));
	if(0xFFFFFFFF == faConfig)
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����

		m_RightDlg.m_test_pass_btn.SetFocus();

		AfxMessageBox(_T("û�л�ȡС���ϵ�¼���ļ� "), MB_OK); 
		g_send_commad = 0;
		return FALSE;
	}

	faConfig &= ~FILE_ATTRIBUTE_HIDDEN;  //����ļ���ϵͳ����ô���ϵͳ
	SetFileAttributes(ConvertAbsolutePath(test_pcm_name), faConfig);

#if 1

	//����PCM�ļ�
	if (!play_pcm_test(test_pcm_name))
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����

		m_RightDlg.m_test_pass_btn.SetFocus();

		AfxMessageBox(_T("����pcm�ļ�ʧ��"), MB_OK);
		g_send_commad = 0;
		return FALSE;
	}

	remove(T2A(test_pcm_name));
#endif

#if 0

	play_pcm_finish_flag = FALSE;
	create_paly_pcm_thread_data(0);


	time1 = GetTickCount();
	while (1)
	{
		Sleep(500);
		time2 = GetTickCount();
		if (time2 - time1 > 5000)
		{
			play_pcm_finish_flag = FALSE;
			break;
		}
		if (play_pcm_finish_flag)
		{
			play_pcm_finish_flag = FALSE;
			break;
		}
	}

	close_thread_paly_pcm();

#endif

	m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
	m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����

	m_RightDlg.m_test_pass_btn.SetFocus();

	g_send_commad = 0;

	return TRUE;

}

//�Խ�����
void CAnykaIPCameraDlg::case_interphone()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strSourceName, strDestName;
	TCHAR test_Name[MAX_PATH] = {0};
	TCHAR test_param[MAX_PATH] = {0};
	TCHAR DestName_temp[MAX_PATH] = {0};
	TCHAR DestName[50] =_T("/tmp/");
	UINT name_len = 0;
	char name_buf[MAX_PATH] = {0};
	char param_buf[2] = {0};
	CString str;
	TCHAR *file_name = _T("test_interphone");
	TCHAR *test_pcm_name = _T("test_pcm.mp3");

	USES_CONVERSION;
	if (!g_connet_flag)
	{
		AfxMessageBox(_T("����û�������ϣ�����"), MB_OK);
		return;
	}
	g_send_commad = 1;
	//���ز��Ե���Ƶ�ļ�
	memset(test_Name, 0,  MAX_PATH);
	_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
	_tcscat(test_Name, _T("/"));
	_tcscat(test_Name, test_pcm_name);
	//�ж��ļ��Ƿ����
	if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(test_Name)))
	{
		g_send_commad = 0;
		str.Format(_T("�Խ����� %s no exist"), test_Name);
		AfxMessageBox(str, MB_OK); 
		return;
	}
	//�����ļ�
	memset(DestName_temp, 0, MAX_PATH);
	_tcsncpy(DestName_temp, DestName, sizeof(DestName));
	strSourceName.Format(_T("%s"), ConvertAbsolutePath(test_Name));
	_tcscat(DestName_temp, test_pcm_name);
	strDestName.Format(_T("%s"), DestName_temp);
	if (m_pFtpConnection == NULL || !m_pFtpConnection->PutFile(strSourceName,
		strDestName, FTP_TRANSFER_TYPE_BINARY, 1))   
	{
		AfxMessageBox(_T("�Խ����� PutFile fail"), MB_OK);  
		download_dev_file_flag = FALSE;
		g_send_commad = 0;
		return;
	}



	//���ز��Գ���
	memset(test_Name, 0,  MAX_PATH);
	_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
	_tcscat(test_Name, _T("//"));
	_tcscat(test_Name, file_name);

	//�ж��ļ��Ƿ����
	if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(test_Name)))
	{
		g_send_commad = 0;
		str.Format(_T("�Խ����� %s no exist"), test_Name);
		AfxMessageBox(str, MB_OK); 
		return;
	}

	//�����ļ�
	memset(DestName_temp, 0, MAX_PATH);
	_tcsncpy(DestName_temp, DestName, sizeof(DestName));
	strSourceName.Format(_T("%s"), ConvertAbsolutePath(test_Name));
	_tcscat(DestName_temp, file_name);
	strDestName.Format(_T("%s"), DestName_temp);


	if (m_pFtpConnection == NULL || !m_pFtpConnection->PutFile(strSourceName,
		strDestName, FTP_TRANSFER_TYPE_BINARY, 1))   
	{
		AfxMessageBox(_T("�Խ����� PutFile fail"), MB_OK);  
		download_dev_file_flag = FALSE;
		g_send_commad = 0;
		return;
	}

	//��������
	name_len = strlen(T2A(file_name));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(file_name), name_len);

	memset(test_param, 0, MAX_PATH);
	_tcscpy(test_param, test_pcm_name);

	g_test_pass_flag = 0;
	if (!Send_cmd(TEST_COMMAND, 0, name_buf, T2A(test_param)))
	{
		AfxMessageBox(_T("�Խ����� Send_cmd fail "), MB_OK); 
		g_send_commad = 0;
		return ;
	}

	Anyka_Test_check_info();
	g_test_pass_flag = 0;
	g_send_commad = 0;
}
void CAnykaIPCameraDlg::case_head()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strSourceName, strDestName;
	TCHAR test_Name[MAX_PATH] = {0};
	TCHAR DestName_temp[MAX_PATH] = {0};
	TCHAR DestName[50] =_T("/tmp/");
	UINT name_len = 0;
	char name_buf[MAX_PATH] = {0};
	char param_buf[2] = {0};
	CString str;
	TCHAR *file_name = _T("test_ptz");

	USES_CONVERSION;
	if (!g_connet_flag)
	{
		AfxMessageBox(_T("����û�������ϣ�����"), MB_OK);
		return;
	}
	g_send_commad = 1;

	memset(test_Name, 0,  MAX_PATH);
	_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
	_tcscat(test_Name, _T("/"));
	_tcscat(test_Name, file_name);

	//�ж��ļ��Ƿ����
	if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(test_Name)))
	{
		g_send_commad = 0;
		str.Format(_T("��̨���� %s no exist"), test_Name);
		AfxMessageBox(str, MB_OK); 
		return;
	}

	//�����ļ�
	memset(DestName_temp, 0, MAX_PATH);
	_tcsncpy(DestName_temp, DestName, sizeof(DestName));
	strSourceName.Format(_T("%s"), ConvertAbsolutePath(test_Name));
	_tcscat(DestName_temp, file_name);
	strDestName.Format(_T("%s"), DestName_temp);


	if (m_pFtpConnection == NULL || !m_pFtpConnection->PutFile(strSourceName,
		strDestName, FTP_TRANSFER_TYPE_BINARY, 1))   
	{
		AfxMessageBox(_T("��̨���� PutFile fail"), MB_OK);  
		download_dev_file_flag = FALSE;
		g_send_commad = 0;
		return;
	}

	//��������
	name_len = strlen(T2A(file_name));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(file_name), name_len);

	g_test_pass_flag = 0;
	if (!Send_cmd(TEST_COMMAND, 0, name_buf, NULL))
	{
		AfxMessageBox(_T("��̨���� Send_cmd fail "), MB_OK); 
		g_send_commad = 0;
		return ;
	}

	Anyka_Test_check_info();

	g_test_pass_flag = 0;
	g_send_commad = 0;
}
void CAnykaIPCameraDlg::case_sd()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strSourceName, strDestName;
	TCHAR test_Name[MAX_PATH] = {0};
	TCHAR DestName_temp[MAX_PATH] = {0};
	TCHAR DestName[50] =_T("/tmp/");
	UINT name_len = 0;
	char name_buf[MAX_PATH] = {0};
	char param_buf[2] = {0};
	CString str;
	TCHAR *file_name = _T("test_mmc");

	USES_CONVERSION;
	if (!g_connet_flag)
	{
		AfxMessageBox(_T("����û�������ϣ�����"), MB_OK);
		return;
	}
	g_send_commad = 1;

	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->ShowWindow(SW_HIDE);

	m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//���� 
	m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����

	memset(test_Name, 0,  MAX_PATH);
	_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
	_tcscat(test_Name, _T("/"));
	_tcscat(test_Name, file_name);

	//�ж��ļ��Ƿ����
	if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(test_Name)))
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����

		m_RightDlg.m_test_pass_btn.SetFocus();

		g_send_commad = 0;
		str.Format(_T("sd���� %s no exist"), test_Name);
		AfxMessageBox(str, MB_OK); 
		return;
	}

	//�����ļ�
	memset(DestName_temp, 0, MAX_PATH);
	_tcsncpy(DestName_temp, DestName, sizeof(DestName));
	strSourceName.Format(_T("%s"), ConvertAbsolutePath(test_Name));
	_tcscat(DestName_temp, file_name);
	strDestName.Format(_T("%s"), DestName_temp);


	if (m_pFtpConnection == NULL || !m_pFtpConnection->PutFile(strSourceName,
		strDestName, FTP_TRANSFER_TYPE_BINARY, 1))   
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����

		m_RightDlg.m_test_pass_btn.SetFocus();

		AfxMessageBox(_T("sd���� PutFile fail"), MB_OK);  
		download_dev_file_flag = FALSE;
		g_send_commad = 0;
		return;
	}

	//��������
	name_len = strlen(T2A(file_name));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(file_name), name_len);

	g_test_pass_flag = 0;
	if (!Send_cmd(TEST_COMMAND, 1, name_buf, NULL))
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����

		m_RightDlg.m_test_pass_btn.SetFocus();

		AfxMessageBox(_T("sd���� Send_cmd fail "), MB_OK); 
		g_send_commad = 0;
		return ;
	}
	start_test = FALSE;

	//Sleep(3000);

	//���շ���ֵ
	if (!Anyka_Test_check_info())
	{
		start_test = TRUE;
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����

		m_RightDlg.m_test_pass_btn.SetFocus();

		//AfxMessageBox(_T("sd����ʧ��"), MB_OK);
		m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("sd������ʧ��"));
		m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->ShowWindow(SW_SHOW);
		g_send_commad = 0;
		//Sleep(1000);
		//case_main(FALSE);
		return ;
	}
	//MessageBox(_T("sd���Գɹ�"), MB_OK);
	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("sd�����Գɹ�"));
	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->ShowWindow(SW_SHOW);
	start_test = TRUE;
	m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
	m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����

	m_RightDlg.m_test_pass_btn.SetFocus();

	g_test_pass_flag = 0;
	g_send_commad = 0;
	//Sleep(2000);
	//case_main(TRUE);
}


void CAnykaIPCameraDlg::case_wifi()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strSourceName, strDestName;
	TCHAR test_Name[MAX_PATH] = {0};
	TCHAR DestName_temp[MAX_PATH] = {0};
	TCHAR DestName[50] =_T("/tmp/");
	UINT name_len = 0;
	char name_buf[MAX_PATH] = {0};
	char param_buf[2] = {0};
	CString str;
	TCHAR *file_name = _T("test_wifi");
	TCHAR test_param[MAX_PATH];

	USES_CONVERSION;
	if (!g_connet_flag)
	{
		AfxMessageBox(_T("����û�������ϣ�����"), MB_OK);
		return;
	}

	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->ShowWindow(SW_HIDE);

	m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//���� 
	m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����

	

	memset(test_Name, 0,  MAX_PATH);
	_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
	_tcscat(test_Name, _T("/"));
	_tcscat(test_Name, file_name);


	GetDlgItemText(IDC_EDIT_WIFI_NAME, str);
	if (str.IsEmpty())
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����

		m_RightDlg.m_test_pass_btn.SetFocus();

		g_send_commad = 0;
		str.Format(_T("wifi����, wifi��Ϊ�գ�����д��Ҫ���ҵ���wifi��"));
		AfxMessageBox(str, MB_OK); 
		return;
	}
	memset(g_test_config.m_wifi_name, 0, MAC_ADDRESS_LEN);
	_tcscpy(g_test_config.m_wifi_name, str);

	//�ж��ļ��Ƿ����
	if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(test_Name)))
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����

		m_RightDlg.m_test_pass_btn.SetFocus();

		g_send_commad = 0;
		str.Format(_T("wifi���� %s no exist"), test_Name);
		AfxMessageBox(str, MB_OK); 
		return;
	}

	//�����ļ�
	memset(DestName_temp, 0, MAX_PATH);
	_tcsncpy(DestName_temp, DestName, sizeof(DestName));
	strSourceName.Format(_T("%s"), ConvertAbsolutePath(test_Name));
	_tcscat(DestName_temp, file_name);
	strDestName.Format(_T("%s"), DestName_temp);


	if (m_pFtpConnection == NULL || !m_pFtpConnection->PutFile(strSourceName,
		strDestName, FTP_TRANSFER_TYPE_BINARY, 1))   
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����

		m_RightDlg.m_test_pass_btn.SetFocus();

		AfxMessageBox(_T("wifi���� PutFile fail"), MB_OK);
		download_dev_file_flag = FALSE;
		g_send_commad = 0;
		return;
	}

	//��������
	name_len = strlen(T2A(file_name));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(file_name), name_len);

	memset(test_param, 0, MAX_PATH+1);
	_tcscpy(test_param, g_test_config.m_wifi_name);

	g_send_commad = 1;

	g_test_pass_flag = 0;
	if (!Send_cmd(TEST_COMMAND, 1, name_buf, T2A(test_param)))
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����

		m_RightDlg.m_test_pass_btn.SetFocus();

		AfxMessageBox(_T("wifi���� Send_cmd fail "), MB_OK);
		g_send_commad = 0;
		return ;
	}

	start_test = FALSE;

	//Sleep(500);

	//���շ���ֵ
	if (!Anyka_Test_check_info())
	{
		start_test = TRUE;
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����

		m_RightDlg.m_test_pass_btn.SetFocus();

		//AfxMessageBox(_T("wifi����ʧ��"), MB_OK);
		m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("wifi����ʧ��"));
		m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->ShowWindow(SW_SHOW);
		g_send_commad = 0;
		//Sleep(2000);
		//case_main(FALSE);
		return ;
	}
	//MessageBox(_T("wifi���Գɹ�"), MB_OK);
	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("wifi���Գɹ�"));
	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->ShowWindow(SW_SHOW);
	start_test = TRUE;

	m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
	m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����

	m_RightDlg.m_test_pass_btn.SetFocus();

	g_test_pass_flag = 0;
	g_send_commad = 0;
	//Sleep(2000);
	//case_main(TRUE);
}

void CAnykaIPCameraDlg::case_rev()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strSourceName, strDestName;
	TCHAR test_Name[MAX_PATH] = {0};
	TCHAR DestName_temp[MAX_PATH] = {0};
	TCHAR DestName[50] =_T("/tmp/");
	UINT name_len = 0;
	char name_buf[MAX_PATH] = {0};
	char param_buf[2] = {0};
	CString str;
	TCHAR *file_name = _T("test_recover_dev");
	TCHAR test_param[MAX_PATH];

	USES_CONVERSION;

	if (!g_connet_flag)
	{
		AfxMessageBox(_T("����û�������ϣ�����"), MB_OK);
		return;
	}
	g_send_commad = 1;

	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->ShowWindow(SW_HIDE);
	m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//���� 
	m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����

	memset(test_Name, 0,  MAX_PATH);
	_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
	_tcscat(test_Name, _T("/"));
	_tcscat(test_Name, file_name);

	GetDlgItemText(IDC_EDIT_WIFI_NAME, str);
	if(init_flag)
	{
		str=g_test_config.m_test_reset_time;
	}
	if (str.IsEmpty())
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		g_send_commad = 0;
		str.Format(_T("��λ������,ʱ��Ϊ�գ�����д������Ҫ��ʱ����"));
		AfxMessageBox(str, MB_OK); 
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		return;
	}
	memset(g_test_config.m_test_reset_time, 0, MAC_ADDRESS_LEN);
	_tcscpy(g_test_config.m_test_reset_time, str);

	//�ж��ļ��Ƿ����
	if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(test_Name)))
	{
		g_send_commad = 0;
		str.Format(_T("%s no exist,������С��"), test_Name);
		AfxMessageBox(str, MB_OK); 
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		return;
	}

	//�����ļ�
	memset(DestName_temp, 0, MAX_PATH);
	_tcsncpy(DestName_temp, DestName, sizeof(DestName));
	strSourceName.Format(_T("%s"), ConvertAbsolutePath(test_Name));
	_tcscat(DestName_temp, file_name);
	strDestName.Format(_T("%s"), DestName_temp);

	/*if (!download_dev_file_flag)
	{
	if (!m_pFtpConnection->PutFile(strSourceName, strDestName, FTP_TRANSFER_TYPE_BINARY, 1))   
	{
	AfxMessageBox(_T("Error no auto test putting file,������С��"), MB_OK);  
	download_dev_file_flag = FALSE;
	g_send_commad = 0;
	return;
	}
	download_dev_file_flag = TRUE;
	}*/

	if (m_pFtpConnection == NULL || !m_pFtpConnection->PutFile(strSourceName,
		strDestName, FTP_TRANSFER_TYPE_BINARY, 1))   
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		AfxMessageBox(_T("Error no auto test putting file,������С��"), MB_OK);  
		download_dev_file_flag = FALSE;
		g_send_commad = 0;
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		return;
	}


	//��������
	name_len = strlen(T2A(file_name));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(file_name), name_len);

	memset(test_param, 0, MAX_PATH+1);
	_tcscpy(test_param, g_test_config.m_test_reset_time);

	g_test_pass_flag = 0;
	if (!Send_cmd(TEST_COMMAND, 1, name_buf, T2A(test_param)))
	{
		AfxMessageBox(_T("Send_cmd��fail "), MB_OK); 
		g_send_commad = 0;
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		return ;
	}

	Sleep(500);

	//���շ���ֵ
	if (!Anyka_Test_check_info())
	{
		//AfxMessageBox(_T("����ʧ��"), MB_OK);
		m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("����ʧ��"));
		m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->ShowWindow(SW_SHOW);
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		g_send_commad = 0;
		//Sleep(500);
		//case_main(FALSE);
		return ;
	}
	//MessageBox(_T("���óɹ�"), MB_OK);
	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("���óɹ�"));
	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->ShowWindow(SW_SHOW);
	g_test_pass_flag = 0;
	g_send_commad = 0;
	m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
	m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
	//Sleep(500);
	//case_main(TRUE);
}
//void CAnykaIPCameraDlg::case_infrared()
//{
//	// TODO: �ڴ���ӿؼ�֪ͨ����������
//}

BOOL CAnykaIPCameraDlg::Download_UpdateFile(TCHAR *name_buf)
{
	CString strSourceName, strDestName;
	TCHAR DestName[50] =_T("/tmp/");
	TCHAR DestName_temp[MAX_PATH] = {0};
	TCHAR test_Name[MAX_PATH] = {0};
	UINT i = 0;

	//��������MAC��ַ���ļ�
	memset(test_Name, 0,  sizeof(TEST_CONFIG_DIR));
	_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
	_tcscat(test_Name, _T("/"));
	_tcscat(test_Name, name_buf);
	if(0xFFFFFFFF == GetFileAttributes(g_test_config.ConvertAbsolutePath(test_Name)))
	{
		CString str;
		str.Format(_T("%s no exist"), test_Name);
		AfxMessageBox(str);   
		return FALSE;
	}

	//�����ļ�
	memset(DestName_temp, 0, MAX_PATH);
	_tcsncpy(DestName_temp, DestName, sizeof(DestName));
	strSourceName.Format(_T("%s"), g_test_config.ConvertAbsolutePath(test_Name));
	_tcscat(DestName_temp, name_buf);

	strDestName.Format(_T("%s"), DestName_temp);

	if (m_pFtpConnection == NULL)
	{
		AfxMessageBox(_T("m_pFtpConnection == NULL"));
		return FALSE;
	}
	//AfxMessageBox(_T("111111111111111111111"));
	for ( i = 0; i < 5; i++)
	{
		if (m_pFtpConnection->PutFile(strSourceName,strDestName, FTP_TRANSFER_TYPE_BINARY, 1))   
		{
			break;
		}
		Sleep(100);
	}
	if (i == 5)
	{
		AfxMessageBox(_T("Error auto test putting file"));
		return FALSE;
	}
	
	return TRUE;
}

//Сдת�ɴ�С
VOID CAnykaIPCameraDlg::lower_to_upper(TCHAR *surbuf, TCHAR *dstbuf)
{
	UINT i = 0;
	UINT nlen = 0;

	//��ȡ�ַ����ĳ���
	while (surbuf[nlen] != NULL)
	{
		nlen++;
	}

	for (i = 0; i < nlen; i++)
	{
		if(islower(surbuf[i]))//�����Сд
		{
			dstbuf[i] = toupper(surbuf[i]); //ת��
		}
		else
		{
			dstbuf[i] = surbuf[i]; //ֱ�Ӹ�ֵ
		}
	}
}

BOOL CAnykaIPCameraDlg::Otp_mac_add(TCHAR *surbuf, TCHAR *dstbuf)
{
	TCHAR tmpBuf[MAC_ADDRESS_LEN+1] = {0};
	TCHAR surbuf_add[MAC_ADDRESS_LEN+1] = {0};
	CHAR tmpBuf_temp[MAC_ADDRESS_LEN+1] = {0};
	TCHAR tmpBuf_1[MAC_ADDRESS_LEN+1] = {0};
	int  surbuf_len = 0;
	int  i = 0;
	int idex = 0;
	BOOL flag = FALSE;
	UINT tempmac = 0;

	memset(surbuf_add, 0, MAC_ADDRESS_LEN+1);
	_tcsncpy(surbuf_add, &surbuf[9],  8);
	surbuf_len = wcslen(surbuf_add);
	if (surbuf_len == 0)
	{
		return FALSE;
	}
	lower_to_upper(surbuf_add, tmpBuf);
	for (i = surbuf_len - 1; i >= 0; i--)
	{
		idex = tmpBuf[i];
		if (tmpBuf[i] == 58)// :
		{
			continue;

		}
		else if (tmpBuf[i] != 70) 
		{
			sprintf(tmpBuf_temp, "%c", tmpBuf[i]);

			//��ַ����һ
			sscanf(tmpBuf_temp, "%x", &tempmac);
			tempmac ++;
			swprintf(tmpBuf_1, _T("%x"),tempmac);

			_tcsncpy(&tmpBuf[i], &tmpBuf_1[0], 1);
			flag = TRUE;
			break;
		}
		else
		{
			tmpBuf[i] = 48;
			continue;
		}
	}
	_tcsncpy(dstbuf, surbuf, 9);
	_tcsncpy(&dstbuf[9], tmpBuf, 8);

	if (flag == FALSE)
	{
		return FALSE;
	}
	return TRUE;
}
//mac��ַ��һ
BOOL CAnykaIPCameraDlg::Mac_Addr_add_1(TCHAR *buf_temp)
{
	TCHAR tempAddrBuf[MAC_ADDRESS_LEN+1] = {0};
	TCHAR dstAddrBuf[MAC_ADDRESS_LEN+1] = {0};
	Otp_mac_add(g_test_config.m_mac_current_addr, tempAddrBuf);
	
	
	memset(dstAddrBuf, 0, MAC_ADDRESS_LEN);
	lower_to_upper(tempAddrBuf, dstAddrBuf);
	_tcscpy(g_test_config.m_mac_current_addr, dstAddrBuf);//��¼��ǰ��mac��ַ

	return TRUE;
}

BOOL CAnykaIPCameraDlg::case_mac()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TCHAR *buf_mac_name = _T("test_mac");
	TCHAR *buf_update_name = _T("updater");
	UINT name_len = 0;
	char name_buf[MAX_PATH] = {0};
	TCHAR buf_temp[MAX_PATH] = {0};


	USES_CONVERSION;

	m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//���� 
	m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//���� 

	if (!g_connet_flag)
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		AfxMessageBox(_T("����û�������ϣ�����"), MB_OK);
		return FALSE;
	}

	//�Ƚ�MAC��ַ�Ĵ�С
	if (_tcsncmp(g_test_config.m_mac_current_addr, g_test_config.m_mac_end_addr, 17) == 0)
	{
		AfxMessageBox(_T("MAC��ַ�ѳ���MAC��ַ���趨��Χ������"), MB_OK);
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		g_send_commad = 0;
		return FALSE;
	}
	
	if (!Download_UpdateFile(buf_update_name))
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		g_send_commad = 0;
		return FALSE;
	}
	
	if (!Download_UpdateFile(buf_mac_name))
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		g_send_commad = 0;
		return FALSE;
	}
	name_len = strlen(T2A(buf_mac_name));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(buf_mac_name), name_len);

	memset(buf_temp, 0, MAX_PATH);

	g_send_commad = 1;

	g_test_pass_flag = 0;
	if (!Send_cmd(TEST_COMMAND, 1, name_buf, T2A(g_test_config.m_mac_current_addr)))
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//���� 
		g_send_commad = 0;
		AfxMessageBox(_T("Error putting file"));
		return FALSE;
	}

	//g_test_config.m_mac_current_addr����1
	Mac_Addr_add_1(g_test_config.m_mac_current_addr);

	g_test_config.Write_current_macConfig(CONFIG_CURRENT_MAC);

	if (!Anyka_Test_check_info())
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//���� 
		g_send_commad = 0;
		AfxMessageBox(_T("MAC��¼ʧ��"), MB_OK);
		return FALSE;
	}
	MessageBox(_T("MAC���Գɹ�"), MB_OK);
	m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
	m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//���� 
	g_test_pass_flag = 0;
	g_send_commad = 0;

	return TRUE;

}

BOOL CAnykaIPCameraDlg::Download_serialfile_toftp(void)
{
	CString strSourceName, strDestName;
	TCHAR DestName[MAX_PATH] = _T("/tmp/");
	BOOL file_no_exist_flag = FALSE;
	TCHAR serial_buf[MAX_PATH*2+1] = {0};
	TCHAR srcfileName[MAX_PATH] = {0};
	TCHAR serial_temp[MAX_PATH+1] = {0};
	TCHAR dstfileName[MAX_PATH] = {0};
	TCHAR fileName[MAX_PATH] = {0};
	TCHAR temp_buf[MAX_PATH] = {0};
	UINT name_len = 0;
	DWORD lasterror = 0;
	UINT len = 0;

	if((m_pFtpConnection != NULL) && (m_pInetSession != NULL))
	{
		//�����ļ�
		//�������¼���кţ�

		memset(serial_temp, 0, MAX_PATH+1);
		len = _tcslen(g_test_config.m_uid_number);
		_tcsncpy(serial_temp, &g_test_config.m_uid_number[len - 16], 16);
		swprintf(serial_buf, _T("%s/%s_%s"), SERAIL_CONFIG, g_test_config.serial_file_name, serial_temp);
		strSourceName.Format(_T("%s"), g_test_config.ConvertAbsolutePath(serial_buf));

		//�ж��ļ��Ƿ����
		if(0xFFFFFFFF == GetFileAttributes(g_test_config.ConvertAbsolutePath(serial_buf)))
		{
			CString str;
			str.Format(_T("%s no exist"), g_test_config.ConvertAbsolutePath(serial_buf));
			AfxMessageBox(str);   
			return FALSE;
		}

		_tcscat(DestName, g_test_config.serial_file_name);
		strDestName.Format(_T("%s"), DestName);
		if (m_pFtpConnection == NULL || !m_pFtpConnection->PutFile(strSourceName, 
			strDestName, FTP_TRANSFER_TYPE_BINARY, 1))   
		{
			AfxMessageBox(_T("Error auto test putting file"));   
			return FALSE;
		}

#if 0 
		memset(srcfileName, 0, MAX_PATH);
		memset(dstfileName, 0, MAX_PATH);
		//�����к���¼�ɹ��󣬰Ѵ��ļ������������ļ�����
		swprintf(srcfileName, _T("%s/%s_%s"), g_test_config.ConvertAbsolutePath(SERAIL_CONFIG), g_test_config.serial_file_name, serial_temp);
		swprintf(dstfileName, _T("%s/%s_%s"), g_test_config.ConvertAbsolutePath(SERAIL_CONFIG_BAK), g_test_config.serial_file_name, serial_temp);

		if (MoveFile(srcfileName, dstfileName) == 0)
		{
			AfxMessageBox(_T("Error MoveFile file"));   
			return FALSE;
		}
#endif
	}
	return TRUE;
}


BOOL CAnykaIPCameraDlg::Check_Uid()
{
	CString str;
	int i = 0, len = 0;
	char serial[100];
	TCHAR serial_temp[16];

	memset(serial_temp, 0, 16);
	len = _tcslen(g_test_config.m_uid_number);
	_tcsncpy(serial_temp, &g_test_config.m_uid_number[len - 16], 16);
	memcpy(serial, serial_temp, 16*2);

	if(serial[0] == 'T' && serial[2] == 'V' && 
		serial[4] == '6' && serial[6] == '0' &&
		serial[8] == '5' && serial[10] == 'F')
	{
	}
	else
	{
		AfxMessageBox(_T("UID������������ȷ��UID��"),MB_OK);
		return FALSE;
	}

	CFile fp;
	fp.Open(TEXT("uid_config.txt"),CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite);
	len = fp.GetLength();
	char* buff=new char[len];
	fp.Read(buff,len);
	for(i=0;i<len/36;i++)
	{
		if(buff[i*36]==serial[0] && buff[i*36+2]==serial[2] && buff[i*36+4]==serial[4] && buff[i*36+6]==serial[6] && 
			buff[i*36+8]==serial[8] && buff[i*36+10]==serial[10] && buff[i*36+12]==serial[12] && buff[i*36+14]==serial[14] && 
			buff[i*36+16]==serial[16] && buff[i*36+18]==serial[18] && buff[i*36+20]==serial[20] && buff[i*36+22]==serial[22] && 
			buff[i*36+24]==serial[24] && buff[i*36+26]==serial[26] && buff[i*36+28]==serial[28] && buff[i*36+30]==serial[30])
		 //if (strncmp(buff+i*32,dst,32) == 0)//��������¼��uid���Ƚ�
		 {
			 AfxMessageBox(_T("UID����¼����,���飡"),MB_OK);
			 fp.Close();
			 return FALSE;
		 }
	} 

	fp.Close();
	return TRUE;
#if 0
	CString str;
	int i = 0, len = 0;
	TCHAR uid_temp[100];
	TCHAR uid[16];
	DWORD read_len = 1;
	BOOL ret = TRUE;

	USES_CONVERSION;

	memset(uid, 0, 16);
	_tcsncpy(uid,  &g_test_config.m_uid_number[40], 16);
	
	if(uid[0] == 'T' && uid[1] == 'V' && 
		uid[2] == '6' && uid[3] == '0' &&
		uid[4] == '5' && uid[5] == 'F')
	{
	}
	else
	{
		MessageBox(_T("UID������������ȷ��UID��"), MB_OK);
		return FALSE;
	}

	DWORD faConfig = GetFileAttributes(UID_CONFIG_FILE); 
	if(0xFFFFFFFF != faConfig)
	{
		faConfig &= ~FILE_ATTRIBUTE_READONLY;//����ļ���ֻ������Ҫ���ֻ��
		faConfig &= ~FILE_ATTRIBUTE_SYSTEM;  //����ļ���ϵͳ����ô���ϵͳ
		faConfig &= ~FILE_ATTRIBUTE_TEMPORARY;//���������ʱ����ôҲҪ�����ʱ
		SetFileAttributes(UID_CONFIG_FILE, faConfig);
	}
	else
	{
		return true;
	}

	//�������ļ�
	HANDLE hFile = CreateFile(UID_CONFIG_FILE, GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, 0, NULL);
	if(INVALID_HANDLE_VALUE == hFile)
	{
		return FALSE;
	}

	//����һ��һ�ж�ȡ����
	while(read_len > 0)
	{
		CString subLeft, subRight;
		TCHAR ch = 0;
		TCHAR text[100];
		int index = 0;
		UINT will_len = sizeof(TCHAR);

		while(read_len > 0 && ch != '\n')
		{
			ret = ReadFile(hFile, &ch, will_len, &read_len, NULL);
			text[index++] = ch;
		}
		text[index] = 0;

		str = text;
		int len = str.GetLength();

		//discard the lines that is blank or begin with '#'
		str.TrimLeft();
		if(str.IsEmpty() || '#' == str[0])
		{
			continue;
		}

		for(i = 0; i < 16; i++)
		{
			if(uid[i] != text[i])
			{
				break;
			}
		}

		if(i == 16)//_tcsncpy(uid,  str, 16) == 0)
		{
			CloseHandle(hFile);
			MessageBox(_T("UID����¼����,���飡"),MB_OK);
			return FALSE;
		}
		
	}
	CloseHandle(hFile);
	return TRUE;
	#endif
}

BOOL CAnykaIPCameraDlg::case_uid()
{

	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UINT serial_num_len = 0;
	TCHAR test_param[MAX_PATH];
	TCHAR *buf_uid_name = _T("test_pushid");
	TCHAR test_Name[MAX_PATH] = {0};
	UINT name_len = 0;
	char name_buf[MAX_PATH] = {0};
	CString strSourceName, strDestName;
	//CString uidStr;
	TCHAR DestName[50] =_T("/tmp/");
	TCHAR DestName_temp[MAX_PATH] = {0};

	USES_CONVERSION;

	//m_CBurnUidDlg.m_uid_edit.GetWindowTextW(uidStr);
	//CString Uid=uidStr.Mid(40);

	m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//���� 
	m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����

	if (!g_connet_flag)
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		AfxMessageBox(_T("����û�������ϣ�����"), MB_OK);
		return FALSE;
	}
	g_send_commad = 1;
	serial_num_len = _tcslen(g_test_config.m_uid_number);
	if (g_test_config.m_uid_number[0] == 0 && serial_num_len == 0)
	{
		CString str;
		g_send_commad = 0;
		str.Format(_T("���к�Ϊ�գ���ɨ�����к�"));
		AfxMessageBox(str); 
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		return FALSE;
	}

	if(!Check_Uid())
	{
		g_send_commad = 0;
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		return FALSE;
	}


	//�ж��ļ��Ƿ����
	memset(test_Name, 0,  sizeof(TEST_CONFIG_DIR));
	_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
	_tcscat(test_Name, _T("/"));
	_tcscat(test_Name, buf_uid_name);
	if(0xFFFFFFFF == GetFileAttributes(g_test_config.ConvertAbsolutePath(test_Name)))
	{
		CString str;
		g_send_commad = 0;
		str.Format(_T("%s no exist"), buf_uid_name);
		AfxMessageBox(str);   
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		return FALSE;
	}

	//�����ļ�
	memset(DestName_temp, 0, MAX_PATH);
	_tcsncpy(DestName_temp, DestName, sizeof(DestName));
	strSourceName.Format(_T("%s"), g_test_config.ConvertAbsolutePath(test_Name));
	_tcscat(DestName_temp, buf_uid_name);

	strDestName.Format(_T("%s"), DestName_temp);

	if (m_pFtpConnection == NULL || !m_pFtpConnection->PutFile(strSourceName, 
		strDestName, FTP_TRANSFER_TYPE_BINARY, 1))   
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		g_send_commad = 0;
		AfxMessageBox(_T("Error auto test putting file"));
		return FALSE;
	}


	//�Ȱ����кŵ��ļ�ͨ��ftp����С���ϡ�
	if (!Download_serialfile_toftp())
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		g_send_commad = 0;
		AfxMessageBox(_T("�������к��ļ�ʧ�ܣ�����"));
		return FALSE;
	}

	name_len = strlen(T2A(buf_uid_name));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(buf_uid_name), name_len);

	memset(test_param, 0, MAX_PATH+1);
	_tcscpy(test_param, g_test_config.serial_file_name);

	Sleep(100);
	g_test_pass_flag = 0;
	if (!Send_cmd(TEST_COMMAND, 1, name_buf, T2A(test_param)))
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		g_send_commad = 0;
		AfxMessageBox(_T("Error Send_cmd file"));
		return FALSE;
	}

	//���շ���ֵ
	if (!Anyka_Test_check_info())
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		g_send_commad = 0;
		AfxMessageBox(_T("UID��¼ʧ��"), MB_OK);
		//frmLogUidFile.WriteLogFile(0,"UID��¼ʧ��,UID:%s\r\n", Uid);
		return FALSE;
	}
	MessageBox(_T("UID��¼�ɹ�"), MB_OK);
	//frmLogUidFile.WriteLogFile(0,"UID��¼�ɹ�,UID:%s\r\n",Uid);
	m_CBurnUidDlg.WriteUid();
	m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
	m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
	g_send_commad = 0;
	g_test_pass_flag = 0;


	return TRUE;

}

BOOL CAnykaIPCameraDlg::enter_case_uid(UINT *burn_flag)
{	
	SetDlgItemText(IDC_EDIT_QR_CODE, _T(""));

	if (IDOK == m_Burn_UIdDlg.DoModal())
	{
		if (case_uid())
		{
			*burn_flag = 0;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	*burn_flag = 1;
	return TRUE;

}
BOOL  CAnykaIPCameraDlg::check_test_false(void)
{
	BOOL test_fail = FALSE;

	if (!g_test_config.config_video_test_pass && g_test_config.config_video_enable)
	{
		test_fail = TRUE;
	}
	if(!g_test_config.config_ircut_on_test_pass && g_test_config.config_ircut_enable)
	{
		test_fail = TRUE;
	}
	if(!g_test_config.config_ircut_off_test_pass && g_test_config.config_ircut_enable)
	{
		test_fail = TRUE;
	}
	if(!g_test_config.config_voice_rev_test_pass && g_test_config.config_voice_rev_enable)
	{
		test_fail = TRUE;
	}
	if(!g_test_config.config_voice_send_test_pass && g_test_config.config_voice_send_enable)
	{
		test_fail = TRUE;
	}
	if(!g_test_config.config_head_test_pass && g_test_config.config_head_enable)
	{
		test_fail = TRUE;
	}
	if(!g_test_config.config_sd_test_pass && g_test_config.config_sd_enable)
	{
		test_fail = TRUE;
	}
	if(!g_test_config.config_wifi_test_pass && g_test_config.config_wifi_enable)
	{
		test_fail = TRUE;
	}
	if(!g_test_config.config_reset_test_pass && g_test_config.config_reset_enable)
	{
		test_fail = TRUE;
	}

	return test_fail;
}


void  CAnykaIPCameraDlg::show_test_info(BOOL end_flag)
{
	CString str;
	CRect rcDlg1;
	GetClientRect(&rcDlg1);
	BOOL test_fail = FALSE;

	
	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->SetFont(&font);
	//font.DeleteObject();

	str.Format(_T(""));
	if (!g_test_config.config_video_test_pass && g_test_config.config_video_enable)
	{
		test_fail = TRUE;
		str += _T("ͼ��ģ�飬");
	}
	if(!g_test_config.config_ircut_on_test_pass && g_test_config.config_ircut_enable)
	{
		test_fail = TRUE;
		str += _T("IRCUT����");
	}
	if(!g_test_config.config_ircut_off_test_pass && g_test_config.config_ircut_enable)
	{
		test_fail = TRUE;
		str += _T("IRCUT�أ�");
	}
	if(!g_test_config.config_voice_rev_test_pass && g_test_config.config_voice_rev_enable)
	{
		test_fail = TRUE;
		str += _T("������");
	}
	if(!g_test_config.config_voice_send_test_pass && g_test_config.config_voice_send_enable)
	{
		test_fail = TRUE;
		str += _T("�Խ���");
	}
	if(!g_test_config.config_head_test_pass && g_test_config.config_head_enable)
	{
		test_fail = TRUE;
		str += _T("��̨��");
	}
	if(!g_test_config.config_sd_test_pass && g_test_config.config_sd_enable)
	{
		test_fail = TRUE;
		str += _T("sd����");
	}
	if(!g_test_config.config_wifi_test_pass && g_test_config.config_wifi_enable)
	{
		test_fail = TRUE;
		str += _T("wifi��");
	}
	/*if(!g_test_config.config_red_line_test_pass && g_test_config.config_red_line_enable)
	{
		test_fail = TRUE;
		str += _T("���⣬");

	}*/
	if(end_flag && !g_test_config.config_reset_test_pass && g_test_config.config_reset_enable)
	{
		test_fail = TRUE;
		str += _T("��λ����");
	}

	if (test_fail)
	{
		str += _T("����ʧ�ܣ�");
		m_RightDlg.m_Status.SetFontColor(RGB(255,0,0));
		//m_RightDlg.m_Status.SetFontSize(25);
		/*m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->SetFont(&font);
		font.DeleteObject();*/
	}
	else
	{
		str += _T("����ͨ����");
		m_RightDlg.m_Status.SetFontColor(RGB(0,255,0));
		//m_RightDlg.m_Status.SetFontSize(15);
		/*m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->SetFont(&font);
		font.DeleteObject();*/
	}

	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_HIDE);

	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->SetWindowText(str);

	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_SHOW);

	m_RightDlg.UpdateWindow();
	
}


void  CAnykaIPCameraDlg::ChangeTestItem(int case_idex)
{

	/*oldFont.CreateFont(1,1,0,0,FW_HEAVY,FALSE,FALSE,
        FALSE,GB2312_CHARSET,OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
        FIXED_PITCH|FF_MODERN, _T("����"));
	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->SetFont(&oldFont);*/


	
	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->SetFont(&oldFont);

	CRect rcDlg1;
	GetClientRect(&rcDlg1);
	//oldFont.DeleteObject();
	
	switch (case_idex)
	{
	case CASE_VIDEO:
		if (g_test_config.config_video_enable)
		{
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->ShowWindow(SW_HIDE);
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_HIDE);

			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->SetWindowText(L"");
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->SetWindowText(L"");
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->SetWindowText(L"ͼ�����");
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->SetWindowText(L"�����ɫͼ���Ƿ�������������ס��ͷ��ͼ���Ƿ������ƣ������зǳ������㣿");

			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->ShowWindow(SW_SHOW);
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_SHOW);
			//oldFont.DeleteObject();
			m_RightDlg.UpdateWindow();
		}
		break;
	case CASE_IRCUT_ON:
		if (g_test_config.config_ircut_enable)
		{
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->ShowWindow(SW_HIDE);
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_HIDE);

			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->SetWindowText(L"");
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->SetWindowText(L"");
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->SetWindowText(L"IR-cut���ܿ�������");
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->SetWindowText(L"��Ƶ��ɫ�Ƿ�ڰף�������Ƿ�ȫ����");

			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->ShowWindow(SW_SHOW);
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_SHOW);

			m_RightDlg.UpdateWindow();
		}

		break;
	case CASE_IRCUT_OFF:
		if (g_test_config.config_ircut_enable)
		{
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->ShowWindow(SW_HIDE);
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_HIDE);

			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->SetWindowText(L"IR-cut���ܹرղ���");
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->SetWindowText(L"��Ƶ��ɫ�Ƿ��ɫ��������Ƿ�ȫ��");

			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->ShowWindow(SW_SHOW);
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_SHOW);

			m_RightDlg.UpdateWindow();
		}

		break;
	case CASE_MONITOR:
		if (g_test_config.config_voice_rev_enable)
		{
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->ShowWindow(SW_HIDE);
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_HIDE);

			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->SetWindowText(L"�������ܲ���");
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->SetWindowText(L"�����豸˵�����Ƿ��������������ȵ�������");

			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->ShowWindow(SW_SHOW);
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_SHOW);

			m_RightDlg.UpdateWindow();
		}
		break;
	case CASE_INTERPHONE:
		if (g_test_config.config_voice_send_enable)
		{
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->ShowWindow(SW_HIDE);
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_HIDE);

			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->SetWindowText(L"�Խ����ܲ���");
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->SetWindowText(L"�ѷ��ͶԽ����ݣ��Ƿ��������豸���ȵ�������");

			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->ShowWindow(SW_SHOW);
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_SHOW);

			m_RightDlg.UpdateWindow();
		}
		break;
	case CASE_HEAD:
		if (g_test_config.config_head_enable)
		{
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->ShowWindow(SW_HIDE);
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_HIDE);

			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->SetWindowText(L"��̨���ܲ���");
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->SetWindowText(L"��̨�Ƿ�ˮƽ�ʹ�ֱ����ת��");

			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->ShowWindow(SW_SHOW);
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_SHOW);

			m_RightDlg.UpdateWindow();
		}
		break;
	case CASE_SD:
		if (g_test_config.config_sd_enable)
		{
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->ShowWindow(SW_HIDE);
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_HIDE);

			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->SetWindowText(L"TF�����ܲ���");
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->SetWindowText(L"��ȡTF������������������������Ϊ0������Գɹ�����������TF������Ϊ0�������ʧ�ܡ�");

			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->ShowWindow(SW_SHOW);
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_SHOW);

			m_RightDlg.UpdateWindow();
		}
		break;
	case CASE_WIFI:
		if (g_test_config.config_wifi_enable)
		{
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->ShowWindow(SW_HIDE);
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_HIDE);

			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->SetWindowText(L"WiFi���ܲ���");
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->SetWindowText(L"�����ͳ���ܱ�AP�ȵ�ĸ�����������Ϊ0�������ʧ��");

			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->ShowWindow(SW_SHOW);
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_SHOW);

			m_RightDlg.UpdateWindow();
		}
		break;
		/*case CASE_INFRARED:
		if (g_test_config.config_red_line_enable)
		{
		GetDlgItem(IDC_STATIC_TEST_TITLE)->SetWindowText(L"���⹦�ܲ���");
		GetDlgItem(IDC_STATIC_TEST_CONTENT)->SetWindowText(L"��������ס�������裬����������Ƿ����");
		}
		break;*/
	case CASE_UID:
		if (g_test_config.config_uid_enable)
		{
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_HIDE);

			m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("��¼UID"));//���� 
			m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->SetWindowText(_T("����¼UID"));//����

			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->SetWindowText(L"���Խ��");
			//GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("���enter����ʼ��¼UID"));//���� 
			//GetDlgItem(IDC_STATIC_TEST_POINT2)->SetWindowText(_T("���enter��ȡ����¼UID"));//���� 
			show_test_info(FALSE);

			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_SHOW);
			m_RightDlg.UpdateWindow();
		}
		break;
	case CASE_MAC:
		if (g_test_config.config_lan_mac_enable)
		{
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_HIDE);

			m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("��¼MAC"));//���� 
			m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->SetWindowText(_T("����¼MAC"));//���� 

			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->SetWindowText(L"���Խ��");
			//GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("���enter����ʼ��¼MAC"));//���� 
			//GetDlgItem(IDC_STATIC_TEST_POINT2)->SetWindowText(_T("���enter��ȡ����¼MAC"));//���� 
			show_test_info(FALSE);

			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_SHOW);
			m_RightDlg.UpdateWindow();
		}
		break;
	case CASE_RESET:
		if (g_test_config.config_reset_enable)
		{
			m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("ͨ��(Y/y)"));//���� 
			m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->SetWindowText(_T("��ͨ��(�ո�)"));//���� 

			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->ShowWindow(SW_HIDE);
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_HIDE);

			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->SetWindowText(L"��λ�������ܲ���");
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->SetWindowText(L"�̰���λ��1�룬��ȴ��ظ���Ϣ������");

			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->ShowWindow(SW_SHOW);
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_SHOW);

			m_RightDlg.UpdateWindow();
		}
		break;

	}

}


BOOL CAnykaIPCameraDlg::Send_cmand(UINT case_idex)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(g_test_config.config_ircut_enable && case_idex == CASE_IRCUT_ON)
	{
		//��������
		case_ircut_on();
	}
	else if(g_test_config.config_ircut_enable && case_idex == CASE_IRCUT_OFF)
	{
		//��������
		case_ircut_off();
	}
	else if(g_test_config.config_voice_rev_enable && case_idex == CASE_MONITOR)
	{
		//��������
		case_monitor();
	} 
	else if(g_test_config.config_voice_send_enable && case_idex == CASE_INTERPHONE)
	{
		//��������
		case_interphone();
	} 
	else if(g_test_config.config_head_enable && case_idex == CASE_HEAD)
	{
		//��������
		case_head();
	}
	else if(g_test_config.config_sd_enable && case_idex == CASE_SD)
	{
		//��������
		case_sd();
	}
	else if(g_test_config.config_wifi_enable && case_idex == CASE_WIFI)
	{
		//��������
		case_wifi();
	}
	else if(g_test_config.config_video_enable && case_idex == CASE_VIDEO)
	{

	}
	/*else if(g_test_config.config_red_line_enable && case_idex == CASE_INFRARED)
	{
	}*/
	else if(g_test_config.config_reset_enable && case_idex == CASE_RESET)
	{
		case_rev();
	}



	return TRUE;
}

BOOL CAnykaIPCameraDlg::The_first_case()
{
	UINT burn_flag = 0;

	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (g_test_config.config_video_enable)
	{
		g_case_idex = CASE_VIDEO;
		ChangeTestItem(g_case_idex);
	}
	else if(g_test_config.config_ircut_enable)
	{
		g_case_idex = CASE_IRCUT_ON;
		ChangeTestItem(g_case_idex);
		//��������
		case_ircut_on();
	}
	else if(g_test_config.config_voice_rev_enable)
	{
		g_case_idex = CASE_MONITOR;
		ChangeTestItem(g_case_idex);
		//��������
		case_monitor();
	} 
	else if(g_test_config.config_voice_send_enable)
	{
		g_case_idex = CASE_INTERPHONE;
		ChangeTestItem(g_case_idex);

		//��������
		case_interphone();
	} 
	else if(g_test_config.config_head_enable)
	{
		g_case_idex = CASE_HEAD;
		ChangeTestItem(g_case_idex);
		//��������
		case_head();
	}
	else if(g_test_config.config_sd_enable)
	{
		g_case_idex = CASE_SD;
		ChangeTestItem(g_case_idex);
		//��������
		case_sd();
	}
	else if(g_test_config.config_wifi_enable)
	{
		g_case_idex = CASE_WIFI;
		ChangeTestItem(g_case_idex);
		//��������
		case_wifi();
	}
	else if(g_test_config.config_reset_enable)
	{
		g_case_idex = CASE_RESET;
		ChangeTestItem(g_case_idex);
		case_rev();
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("ͨ��(Y/y)"));//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->SetWindowText(_T("��ͨ��(�ո�)"));//���� 
	}
	/*else if(g_test_config.config_red_line_enable)
	{
	g_case_idex = CASE_INFRARED;
	ChangeTestItem(g_case_idex);

	}*/
	else if(g_test_config.config_uid_enable)
	{
		g_case_idex = CASE_UID;
		ChangeTestItem(g_case_idex);
#if 0
		if (enter_case_uid(&burn_flag))
		{
			if (burn_flag == 1)
			{
				//ȡ������UID
				GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("����¼UID"));//���� 
			}
			else
			{
				GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("��¼UID�ɹ�"));//���� 	
			}
		}
		else
		{
			GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("����¼UID"));//���� 
		}
		GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("ͨ��"));//���� 
		GetDlgItem(IDC_BUTTON_FAILED)->SetWindowText(_T("��ͨ��"));//���� 
#endif

	}
	else if(g_test_config.config_lan_mac_enable)
	{
		g_case_idex = CASE_MAC;
		ChangeTestItem(g_case_idex);
#if 0
		if (case_mac())
		{
			GetDlgItem(IDC_STATIC_TEST_POINT2)->SetWindowText(_T("��¼MAC�ɹ�"));//���� 
		}
		else
		{
			GetDlgItem(IDC_STATIC_TEST_POINT2)->SetWindowText(_T("��¼MACʧ��"));//���� 
		}
		GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("ͨ��"));//���� 
		GetDlgItem(IDC_BUTTON_FAILED)->SetWindowText(_T("��ͨ��"));//���� 
#endif
	}
	else
	{
		AfxMessageBox(_T("û����Ӧ�����ã�����"), MB_OK);
		return FALSE;
	}

	return TRUE;
}


BOOL CAnykaIPCameraDlg::finish_test(BOOL passs_flag)
{
	CString str;

	g_test_config.test_num++;
	str.Format(_T("%d"), g_test_config.test_num);
	m_BottomDlg.GetDlgItem(IDC_EDIT_BURN_NUM)->SetWindowText(str);

	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->SetWindowText(L"���Խ��");
	show_test_info(TRUE);

	if(!g_test_config.config_uid_enable || !passs_flag) //)
	{
		m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("����Ҫ��¼UID"));
	}
	else
	{
		if (g_test_config.config_uid_test_pass)
		{
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("��¼UID�����"));
		}
		else
		{
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("��¼UIDʧ��"));
		}

	}
	if(!g_test_config.config_lan_mac_enable || !passs_flag) //)
	{
		m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT2)->SetWindowText(_T("����Ҫ��¼MAC"));
	}
	else
	{
		if (g_test_config.config_lan_mac_test_pass)
		{
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT2)->SetWindowText(L"��¼MAC�����");
		}
		else
		{
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT2)->SetWindowText(_T("��¼MACʧ��"));
		}
	}
	start_flag = FALSE;
	m_BottomDlg.GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(FALSE);//����
	m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//����
	m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����
	m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
	m_BottomDlg.GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(FALSE);//����
	CloseServer();
	return TRUE;

}
void CAnykaIPCameraDlg::find_next_idex()
{
	UINT i = 0;

	//��ȡ��һ��case_idex
	for (i = g_case_idex; i <= CASE_MAC; i++)
	{
		if ( i == CASE_IRCUT_ON)
		{
			if(!g_test_config.config_ircut_enable) //
			{
				g_case_idex++;
			}
			else
			{
				break;
			}
		}
		else if ( i == CASE_IRCUT_OFF)
		{
			if(!g_test_config.config_ircut_enable) //
			{
				g_case_idex++;
			}
			else
			{
				break;
			}
		}
		else if( i == CASE_MONITOR)
		{
			if(!g_test_config.config_voice_rev_enable) //
			{
				g_case_idex++;
			}
			else
			{
				break;
			}
		}
		else if( i == CASE_INTERPHONE)
		{
			if(!g_test_config.config_voice_send_enable) //
			{
				g_case_idex++;
			}
			else
			{
				break;
			}
		}
		else if( i == CASE_HEAD)
		{
			if(!g_test_config.config_head_enable) //
			{
				g_case_idex++;
			}
			else
			{
				break;
			}
		}
		else if( i == CASE_SD)
		{
			if(!g_test_config.config_sd_enable) //
			{
				g_case_idex++;
			}
			else
			{
				break;
			}
		}
		else if( i == CASE_WIFI)
		{
			if(!g_test_config.config_wifi_enable) //
			{
				g_case_idex++;
			}
			else
			{
				break;
			}
		}
		else if( i == CASE_RESET)
		{
			if(!g_test_config.config_reset_enable) //
			{
				g_case_idex++;
			}
			else
			{
				break;
			}
		}
		//else if( i == CASE_INFRARED)
		//{
		//	if(!g_test_config.config_red_line_enable) //
		//	{
		//		g_case_idex++;
		//	}
		//	else
		//	{
		//		break;
		//	}
		//}
		else if( i == CASE_UID)
		{
			if(!g_test_config.config_uid_enable) //
			{
				g_case_idex++;
			}
			else
			{
				break;
			}
		}
		else if( i == CASE_MAC)
		{
			if(!g_test_config.config_lan_mac_enable) //
			{
				g_case_idex++;
			}
			else
			{
				break;
			}
		}

	}

}



void CAnykaIPCameraDlg::case_main(BOOL pass_flag)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BOOL case_flag = FALSE;
	UINT i = 0, burn_flag = 0;
	CString str;


	if(0)//pass_flag==FALSE)
	{
		config_uid_enable_temp=g_test_config.config_uid_enable;
		config_lan_mac_enable_temp=g_test_config.config_lan_mac_enable;
		g_test_config.config_uid_enable=FALSE;
		g_test_config.config_lan_mac_enable=FALSE;
	}


	//��¼���Խ��
	if (g_case_idex != CASE_UID && g_case_idex != CASE_MAC)
	{
		write_test_info(pass_flag, g_case_idex);
		//str.Format(_T("1:%d"), g_case_idex);

		//AfxMessageBox(str, MB_OK);
		if (g_case_idex != CASE_MAC +1)
		{
			//str.Format(_T("2:%d"), g_case_idex);

			//AfxMessageBox(str, MB_OK);
			g_case_idex++;	
		}
	}
	find_next_idex();

	//str.Format(_T("3:%d"), g_case_idex);

	//AfxMessageBox(str, MB_OK);
	//�������
	if (g_case_idex > CASE_MAC&& end_test==TRUE)
	{

		finish_test(pass_flag);
		/*OnBnClickedButtonNext();
		Sleep(500);
		OnBnClickedButtonStart();*/
		return;
	}

	//��ʾ��һ��
	ChangeTestItem(g_case_idex);

	//��һ�����ʧ�ܾͲ�������¼uid��mac
	if ((g_case_idex == CASE_UID || g_case_idex == CASE_MAC) && check_test_false())
	{
		finish_test(FALSE);
		return;
	}

	if (g_case_idex == CASE_UID)
	{
		if(pass_flag)
		//if (g_test_config.config_uid_enable)
		{
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T(""));
			if (enter_case_uid(&burn_flag))
			{
				if (burn_flag == 1)
				{
					//ȡ������UID
					m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("����¼UID"));//����
					write_test_info(FALSE, g_case_idex);
				}
				else
				{
					m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("��¼UID�ɹ�"));//����
					write_test_info(TRUE, g_case_idex);
				}
			}
			else
			{
				m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("��¼UIDʧ��"));//����
				write_test_info(FALSE, g_case_idex);
			}

			//GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("ͨ��"));//���� 
			//GetDlgItem(IDC_BUTTON_FAILED)->SetWindowText(_T("��ͨ��"));//���� 
		}
		//else
		//{
		//m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("����¼UID"));//���� 
		//}
		g_case_idex++;
		find_next_idex();
		ChangeTestItem(g_case_idex);

		if (g_case_idex > CASE_MAC)
		{
			finish_test(pass_flag);
			/*OnBnClickedButtonNext();
			OnBnClickedButtonStart();*/
		}
	}
	else if(g_case_idex == CASE_MAC)
	{
		m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T(""));
		g_case_idex++;
		end_test=FALSE;
		
	}
	else if(g_case_idex == CASE_MAC+1)
	{
		m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_HIDE);
		show_test_info(FALSE);
		m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_SHOW);
		m_RightDlg.UpdateWindow();
		if (pass_flag)
		{
			if (case_mac())
			{
				m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT2)->SetWindowText(_T("��¼MAC�ɹ�"));//���� 
				write_test_info(TRUE, g_case_idex);
			}
			else
			{
				m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT2)->SetWindowText(_T("��¼MACʧ��"));//���� 
				write_test_info(FALSE, g_case_idex);
			}
		} 
		else
		{
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT2)->SetWindowText(_T("����¼MAC"));//���� 
			write_test_info(FALSE, g_case_idex);
		}
		g_case_idex++;
		find_next_idex();
		ChangeTestItem(g_case_idex);

		end_test=TRUE;

		if (g_case_idex > CASE_MAC+1)
		{
			finish_test(pass_flag);
			/*OnBnClickedButtonNext();
			OnBnClickedButtonStart();*/
		}
		//GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("ͨ��"));//���� 
		//GetDlgItem(IDC_BUTTON_FAILED)->SetWindowText(_T("��ͨ��"));//���� 
	}	
	else
	{
		//Sleep(1000);
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("ͨ��(Y/y)"));//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->SetWindowText(_T("��ͨ��(�ո�)"));//���� 
		//������һ������
		Send_cmand(g_case_idex);
	}

}

void CAnykaIPCameraDlg::OnBnClickedButtonPass()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	case_main(TRUE);
}

void CAnykaIPCameraDlg::OnBnClickedButtonFailed()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	case_main(FALSE);
}


void CAnykaIPCameraDlg::OnBnClickedButtonReset()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strSourceName, strDestName;
	TCHAR test_Name[MAX_PATH] = {0};
	TCHAR DestName_temp[MAX_PATH] = {0};
	TCHAR DestName[50] =_T("/tmp/");
	UINT name_len = 0;
	char name_buf[MAX_PATH] = {0};
	char param_buf[2] = {0};
	CString str;
	TCHAR *file_name = _T("test_recover");

	USES_CONVERSION;

	if (!g_connet_flag)
	{
		AfxMessageBox(_T("����û�������ϣ�����"), MB_OK);
		return;
	}
	g_send_commad = 1;

	memset(test_Name, 0,  MAX_PATH);
	_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
	_tcscat(test_Name, _T("//"));
	_tcscat(test_Name, file_name);

	//�ж��ļ��Ƿ����
	if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(test_Name)))
	{
		g_send_commad = 0;
		str.Format(_T("%s no exist,������С��"), test_Name);
		AfxMessageBox(str, MB_OK); 
		return;
	}

	//�����ļ�
	memset(DestName_temp, 0, MAX_PATH);
	_tcsncpy(DestName_temp, DestName, sizeof(DestName));
	strSourceName.Format(_T("%s"), ConvertAbsolutePath(test_Name));
	_tcscat(DestName_temp, file_name);
	strDestName.Format(_T("%s"), DestName_temp);

	if (!download_dev_file_flag)
	{
		if (!m_pFtpConnection->PutFile(strSourceName, strDestName, FTP_TRANSFER_TYPE_BINARY, 1))   
		{
			AfxMessageBox(_T("Error no auto test putting file,������С��"), MB_OK);  
			download_dev_file_flag = FALSE;
			g_send_commad = 0;
			return;
		}
		download_dev_file_flag = TRUE;
	}


	//��������
	name_len = strlen(T2A(file_name));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(file_name), name_len);

	g_test_pass_flag = 0;
	if (!Send_cmd(TEST_COMMAND, 1, name_buf, NULL))
	{
		AfxMessageBox(_T("Send_cmd��fail "), MB_OK); 
		g_send_commad = 0;
		return ;
	}

	//���շ���ֵ
	if (!Anyka_Test_check_info())
	{
		AfxMessageBox(_T("����ʧ��"), MB_OK);
		g_send_commad = 0;
		return ;
	}
	g_test_pass_flag = 0;

	if (!Send_cmd(TEST_COMMAND_FINISH, 1, NULL, NULL))
	{
		AfxMessageBox(_T("���³ɹ�, С���Զ�����ʧ�ܣ����ֶ�����"), MB_OK);  
	}
	else
	{
		if (!Anyka_Test_check_info())
		{
			AfxMessageBox(_T("���³ɹ�, С���Զ�����ʧ�ܣ����ֶ�����"), MB_OK);  
		}
		else
		{
			MessageBox(_T("���óɹ�"), MB_OK);
		}
	}
	g_send_commad = 0;
}



void CAnykaIPCameraDlg::write_test_info(BOOL pass_flag, UINT case_idex)
{
	if (case_idex == CASE_VIDEO)
	{
		if(g_test_config.config_video_enable)
		{
			if (pass_flag)
			{
				g_test_config.config_video_test_pass = TRUE;
				frmLogfile.WriteLogFile(0,"ͼ��ģ�鹦�ܲ���ͨ��\r\n");
			}
			else
			{
				g_test_config.config_video_test_pass = FALSE;
				frmLogfile.WriteLogFile(0,"ͼ��ģ�鹦�ܲ��Բ�ͨ��\r\n");
			}
		}
	}
	else if (case_idex == CASE_IRCUT_ON)
	{
		if(g_test_config.config_ircut_enable)
		{
			if (pass_flag)
			{
				g_test_config.config_ircut_on_test_pass = TRUE;
				frmLogfile.WriteLogFile(0,"IRCUT���ܿ�������ͨ��\r\n");
			}
			else
			{
				g_test_config.config_ircut_on_test_pass = FALSE;
				frmLogfile.WriteLogFile(0,"IRCUT���ܿ������Բ�ͨ��\r\n");
			}
		}
	}
	else if (case_idex == CASE_IRCUT_OFF)
	{
		if(g_test_config.config_ircut_enable)
		{
			if (pass_flag)
			{
				g_test_config.config_ircut_off_test_pass = TRUE;
				frmLogfile.WriteLogFile(0,"IRCUT���ܹرղ���ͨ��\r\n");
			}
			else
			{
				g_test_config.config_ircut_off_test_pass = FALSE;
				frmLogfile.WriteLogFile(0,"IRCUT���ܹرղ��Բ�ͨ��\r\n");
			}
		}
	}
	else if (case_idex == CASE_MONITOR)
	{
		if(g_test_config.config_voice_rev_enable)
		{
			if (pass_flag)
			{
				g_test_config.config_voice_rev_test_pass = TRUE;
				frmLogfile.WriteLogFile(0,"�������ܲ���ͨ��\r\n");
			}
			else
			{
				g_test_config.config_voice_rev_test_pass = FALSE;
				frmLogfile.WriteLogFile(0,"�������ܲ��Բ�ͨ��\r\n");
			}
		}
	}
	else if (case_idex == CASE_INTERPHONE)
	{
		if(g_test_config.config_voice_send_enable)
		{
			if (pass_flag)
			{
				g_test_config.config_voice_send_test_pass = TRUE;
				frmLogfile.WriteLogFile(0,"�Խ����ܲ���ͨ��\r\n");
			}
			else
			{
				g_test_config.config_voice_send_test_pass = FALSE;
				frmLogfile.WriteLogFile(0,"�Խ����ܲ��Բ�ͨ��\r\n");
			}
		}
	}
	else if (case_idex == CASE_HEAD)
	{
		if(g_test_config.config_head_enable)
		{
			if (pass_flag)
			{
				g_test_config.config_head_test_pass = TRUE;
				frmLogfile.WriteLogFile(0,"��̨���ܲ���ͨ��\r\n");
			}
			else
			{
				g_test_config.config_head_test_pass = FALSE;
				frmLogfile.WriteLogFile(0,"��̨���ܲ��Բ�ͨ��\r\n");
			}
		}
	}
	else if (case_idex == CASE_SD)
	{
		if(g_test_config.config_sd_enable)
		{
			if (pass_flag)
			{
				g_test_config.config_sd_test_pass = TRUE;
				frmLogfile.WriteLogFile(0,"SD�����ܲ���ͨ��\r\n");
			}
			else
			{
				g_test_config.config_sd_test_pass = FALSE;
				frmLogfile.WriteLogFile(0,"SD�����ܲ��Բ�ͨ��\r\n");
			}
		}
	}
	else if (case_idex == CASE_WIFI)
	{
		if(g_test_config.config_wifi_enable)
		{
			if (pass_flag)
			{
				g_test_config.config_wifi_test_pass = TRUE;
				frmLogfile.WriteLogFile(0,"wifi���ܲ���ͨ��\r\n");
			}
			else
			{
				g_test_config.config_wifi_test_pass = FALSE;
				frmLogfile.WriteLogFile(0,"wifi���ܲ��Բ�ͨ��\r\n");
			}
		}
	}
	//else if (case_idex == CASE_INFRARED)
	//{
	//	if(g_test_config.config_red_line_enable)
	//	{
	//		if (pass_flag)
	//		{
	//			g_test_config.config_red_line_test_pass = TRUE;
	//			frmLogfile.WriteLogFile(0,"wifi���ܲ���ͨ��\r\n");
	//		}
	//		else
	//		{
	//			g_test_config.config_red_line_test_pass = FALSE;
	//			frmLogfile.WriteLogFile(0,"wifi���ܲ��Բ�ͨ��\r\n");
	//		}
	//	}
	//}
	else if (case_idex == CASE_UID)
	{
		if(g_test_config.config_uid_enable)
		{
			if (pass_flag)
			{
				g_test_config.config_uid_test_pass = TRUE;

				TCHAR burnUid[MAX_PATH+1] = {0};
				UINT len = 0;
				memset(burnUid, 0, MAX_PATH+1);
				len = _tcslen(g_test_config.m_uid_number);
				_tcsncpy(burnUid, &g_test_config.m_uid_number[len - 16], 16);
				//CString burnUid=m_Burn_UIdDlg.m_str_uid.Mid(40,16);
				/*CString str,struid;
				str.Format(_T("UID��¼�ɹ�,UID:%s\r\n"),g_test_config.m_uid_number);
				struid.Format(_T("UID��¼�ɹ�,UID:%s\r\n"),burnUid);*/
				USES_CONVERSION;
				frmLogfile.WriteLogFile(0,"UID��¼�ɹ�,UID:%s\r\n", T2A(burnUid));
				frmLogUidFile.WriteLogFile(0,"UID��¼�ɹ�,UID:%s\r\n", T2A(burnUid));
				/*char *cao=(LPSTR)(LPCTSTR)str;
				char *nimade=(LPSTR)(LPCTSTR)struid;
				frmLogfile.WriteLogFile(0,cao);
				frmLogUidFile.WriteLogFile(0,nimade);*/
			}
			else
			{
				g_test_config.config_uid_test_pass = FALSE;
				frmLogfile.WriteLogFile(0,"����¼UID\r\n");
				frmLogUidFile.WriteLogFile(0,"����¼UID\r\n");
			}
		}
	}
	else if (case_idex == CASE_MAC+1)
	{
		if(g_test_config.config_lan_mac_enable)
		{
			if (pass_flag)
			{
				USES_CONVERSION;
				g_test_config.config_lan_mac_test_pass = TRUE;
				frmLogfile.WriteLogFile(0,"MAC��¼�ɹ�,MAC:%s\r\n", T2A(g_test_config.m_mac_current_addr));
			}
			else
			{
				g_test_config.config_lan_mac_test_pass = FALSE;
				frmLogfile.WriteLogFile(0,"����¼MAC\r\n");
			}
		}
	}
	else if (case_idex == CASE_RESET)
	{
		if(g_test_config.config_reset_enable)
		{
			if (pass_flag)
			{
				g_test_config.config_reset_test_pass = TRUE;
				frmLogfile.WriteLogFile(0,"��λ������ͨ��\r\n");
			}
			else
			{
				g_test_config.config_reset_test_pass = FALSE;
				frmLogfile.WriteLogFile(0,"��λ�����Բ�ͨ��\r\n");
			}
			//g_case_idex++;
		}
	}
}

void CAnykaIPCameraDlg::OnBnClickedButtonNext()
{
	UINT len = 0, ip_num = 0;

	// TODO: �ڴ���ӿؼ�֪ͨ����������
	current_ip_idex++;

	m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
	m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//���� 
	m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����

	ip_num = m_RightDlg.m_test_config.GetItemCount();
	if (ip_num <= current_ip_idex)
	{
		start_test = FALSE;
		AfxMessageBox(_T("�������һ̨��Ŷ"), MB_OK);
		start_test = TRUE;
		return;
	}
	else
	{
		m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->SetWindowText(_T(""));
		m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T(""));//���� 
		m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT2)->SetWindowText(_T(""));
	}

	//if (_tcscmp(m_connect_ip, g_test_config.m_current_config[current_ip_idex].Current_IP_address_buffer) != 0)
	//{
	//	g_start_open_flag = TRUE;	
	//	len = _tcsclen(g_test_config.m_current_config[current_ip_idex].Current_IP_address_buffer);
	//	_tcsncpy(m_connect_ip,
	//		g_test_config.m_current_config[current_ip_idex].Current_IP_address_buffer, len);
	//	m_RightDlg.SetDlgItemText(IDC_EDIT_IP, 
	//		g_test_config.m_current_config[current_ip_idex].Current_IP_address_buffer);
	//	m_RightDlg.SetDlgItemText(IDC_EDIT_UID, 
	//		g_test_config.m_current_config[current_ip_idex].Current_IP_UID);
	//	m_RightDlg.SetDlgItemText(IDC_EDIT_MAC, 
	//		g_test_config.m_current_config[current_ip_idex].Current_IP_MAC);
	//	m_RightDlg.SetDlgItemText(IDC_EDIT_VERSION2, 
	//		g_test_config.m_current_config[current_ip_idex].Current_IP_version);
	//}

	if (_tcscmp(m_connect_ip, g_test_config.m_last_config[current_ip_idex].Current_IP_address_buffer) != 0)
	{
		g_start_open_flag = TRUE;	
		len = _tcsclen(g_test_config.m_last_config[current_ip_idex].Current_IP_address_buffer);
		memset(m_connect_ip, 0, MAX_PATH);
		_tcsncpy(m_connect_ip, g_test_config.m_last_config[current_ip_idex].Current_IP_address_buffer, len);
		m_RightDlg.SetDlgItemText(IDC_EDIT_IP, g_test_config.m_last_config[current_ip_idex].Current_IP_address_buffer);
		m_RightDlg.SetDlgItemText(IDC_EDIT_UID, g_test_config.m_last_config[current_ip_idex].Current_IP_UID);
		m_RightDlg.SetDlgItemText(IDC_EDIT_MAC, g_test_config.m_last_config[current_ip_idex].Current_IP_MAC);
		m_RightDlg.SetDlgItemText(IDC_EDIT_VERSION2, g_test_config.m_last_config[current_ip_idex].Current_IP_version);
	}
}

void CAnykaIPCameraDlg::OnEnChangeEditWifiName()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString str;

	GetDlgItemText(IDC_EDIT_WIFI_NAME, str);
	if (!str.IsEmpty())
	{
		memset(g_test_config.m_wifi_name, 0, MAC_ADDRESS_LEN);
		_tcscpy(g_test_config.m_wifi_name, str);
	}

}

void CAnykaIPCameraDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	if (nHotKeyId== 1001)
	{
		if(!first_flag)
		{
			first_flag = TRUE;
			OnBnClickedButtonConfigure();
		}
	}
	else if(nHotKeyId== 1002)
	{
		if(!m_Burn_UIdDlg.UID_first_flag)
		{
			m_Burn_UIdDlg.UID_first_flag = TRUE;
			OnBnClickedButtonWriteUid();
		}
	}
	else if(nHotKeyId== 1003)
	{
		if(!start_flag)
		{
			start_flag = TRUE;
			OnBnClickedButtonStart();
		}
	}
	else if(nHotKeyId== 1004)
	{
		if(start_test)
		{
			OnBnClickedButtonClose();
		}
	}
	else if(nHotKeyId== 1005)
	{
		if(start_test)
		{
			OnBnClickedButtonNext();
		}
	}
	else if(nHotKeyId== 1006)
	{
		if(start_test)
		{
			OnBnClickedButtonReset();
		}
	}
	else if(nHotKeyId== 1009||nHotKeyId== 1010)
	{
		if(start_test)
		{
			case_main(TRUE);
		}
	}
	else if(nHotKeyId== 1012)
	{
		if(start_test)
		{
			case_main(FALSE);
		}
	}
	CDialog::OnHotKey(nHotKeyId, nKey1, nKey2);
}


//static text��ɫ
HBRUSH CAnykaIPCameraDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	//�������ֶ�Ӧstatic text�ؼ��Ļ�ˢ
	static HBRUSH brush_red = ::CreateSolidBrush(RGB(255,0,0));
	static HBRUSH brush_green = ::CreateSolidBrush(RGB(0,255,0));

	enum STATIC_BKCOLOR
	{
		NULL_COLOR,
		RED_COLOR,
		GREEN_COLOR,
	};

	// TODO: Change any attributes of the DC here

	STATIC_BKCOLOR static_BkColor = NULL_COLOR;
	HBRUSH return_hbr = hbr;

	if (pWnd->GetDlgCtrlID()==IDC_STATIC_TEST_CONTENT)
	{
		/*if(g_test_pass_flag==0)
		{
		static_BkColor = GREEN_COLOR;
		}else if(g_test_pass_flag==2)
		{
		static_BkColor = RED_COLOR;
		}*/
		static_BkColor = GREEN_COLOR;
	}
	// TODO: Return a different brush if the default is not desired

	switch (static_BkColor)
	{
	case RED_COLOR:
		pDC->SetTextColor(RGB(255,0,0));
		pDC->SetBkColor(RGB(0,0,0));
		return_hbr = (HBRUSH)brush_red;
		break;
	case GREEN_COLOR:
		pDC->SetTextColor(RGB(0,255,0));
		pDC->SetBkColor(RGB(0,0,0));
		return_hbr = (HBRUSH)brush_green;
		break;
	case NULL_COLOR:
		return_hbr = hbr;
		break;
	default:
		return_hbr = hbr;
	}

#if 0
	switch(pWnd->GetDlgCtrlID())
	{
	case IDC_STATIC_TEST_TITLE:
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(0,255,0));

	case IDC_STATIC_TEST_CONTENT:
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(0,255,0));


		return (HBRUSH)GetStockObject(HOLLOW_BRUSH);

	}
#endif

	return hbr;
}