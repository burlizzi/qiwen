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

#define AUDIO_RECEIVE_BUFFER_SIZE   2048
#define MAX_AUDIO_BUF_NUM   160

typedef struct
{
	BYTE *m_video_receive_buf;
	UINT  m_receive_buf_len;
	BOOL  m_empty_flag;
	UINT  time;
}T_SHOW_VIDE_INFO;

enum frame_type 
{
	FRAME_TYPE_P = 0,
	FRAME_TYPE_I,
	FRAME_TYPE_AUDIO,
	FRAME_TYPE_PICTURE,
	FRAME_TYPE_TEST_MODE,
};


T_SHOW_VIDE_INFO g_play_data[MAX_AUDIO_BUF_NUM] = {0};
BYTE *g_one_audio_receive_buf = NULL;
UINT g_one_audio_receive_buf_len = 0;
UINT g_audio_read_current_buf_idex = 0;
UINT g_audio_write_current_buf_idex = 0;
UINT g_time_seconds = 0;

HWAVEOUT g_hwo = NULL;
HANDLE g_hBurnThread_record_wav = INVALID_HANDLE_VALUE;
HANDLE g_hBurnThread_play_wav = INVALID_HANDLE_VALUE;

BOOL g_net_conect_success = FALSE;


typedef enum
{
	CASE_VIDEO = 0,	
	CASE_IRCUT_ONOFF,
	//CASE_IRCUT_ON,
	//CASE_IRCUT_OFF,
	//CASE_MONITOR,   //
	CASE_INTERPHONE,//�Խ�
	CASE_HEAD,      //��̨
	//CASE_RESET,
	CASE_SD,
	CASE_WIFI,
	//CASE_INFRARED,
	CASE_UID,
	CASE_MAC,
	//CASE_RESET,
}E_MEMORY_TYPE;

typedef enum
{
	CASE_SPEAKER = 0,   //����
	CASE_CLOUD_DECK,   //��̨
	CASE_WIFI_CONNECT,   //wifi����
	CASE_SD_CARD,    //SD��
	CASE_MONITOR,   //����
	//CASE_INTERPHONE,  //�Խ���һ���Ҫ
	CASE_FOCUS_EFFECT,   //������Ч��
	CASE_IRCUT,     //IRCUT�ͺ����
	CASE_RESET,    //��λ
	CASE_MAC_BURN,  //mac��¼
	CASE_UID_BURN,  //uid��¼
	CASE_ATTR_TYPE_NUM
}T_TEST_INFO;

typedef enum {
	 CMD_COMMAND = 1,	
	 CMD_RESPONSE,
     CMD_TYPE_NUM
} T_CMD_TYPE;

typedef struct
{
char *head_name ;  //��ͷ��ʶ��VCMD��
enum T_TEST_INFO  test_type;
enum T_CMD_TYPE  cmd_type;
UINT   datalen;//���ݳ���
char*   data;    //����
short  check_sum;
char   *end_name;  //��β��ʶ��VEND��
}T_TEST_DATA;

typedef enum
{
	IDS_TEST_SUCCESS,
	IDS_TEST_FAIL,
}E_RESULT_TYPE;


typedef struct
{
	UINT thread_idex;		
	UINT IP_idex;
}T_IDEX_INFO;

typedef struct
{
	char UID[MAC_ADDRESS_LEN];
	char MAC[MAC_ADDRESS_LEN];
}T_UID_MAC_INFO;



#define      HOTKEY_F1   1001
#define      HOTKEY_F2   1002
#define      HOTKEY_F3   1003
#define      HOTKEY_F4   1004
#define      HOTKEY_F6   1006
#define      HOTKEY_F7   1007
#define      HOTKEY_F8   1008
#define      HOTKEY_FY   1009
#define      HOTKEY_Fy   1010
#define      HOTKEY_F11  1011
#define      HOTKEY_FESCAPE   1012
#define      HOTKEY_FNULL   1013


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


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TRANS_STACKSIZE		(1024*300)


#define  TOOL_VERSOIN  _T("IPCTest_tool_v5.0.04")


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


extern HINSTANCE _hInstance;
extern unsigned char *g_net_buf ;
 CFile g_file_fp;

char g_update_flag[UPDATE_MAX_NUM] = {0};
BOOL g_uid_running = FALSE;
BOOL g_start_running = FALSE;
BOOL g_next_running = FALSE;
BOOL g_reset_running = FALSE;
extern BOOL start_test_flag;
BOOL g_start_success_flag = FALSE;

BOOL g_start_open_flag = TRUE;
BOOL g_test_finish_endrtsp_flag = false;
HANDLE g_hTestThread = INVALID_HANDLE_VALUE;
HANDLE g_findIP_Thread = INVALID_HANDLE_VALUE;
HANDLE g_test_wait_data = INVALID_HANDLE_VALUE;
BOOL g_first_find_flag = TRUE;
BOOL g_config_start_flag = TRUE;
BOOL g_reset_test_finish = False;
HANDLE         g_wait;
BOOL no_burn_uid =FALSE;

//char g_video_buf[MAX_VIDEO_LEN] = {0};
char *g_video_buf = NULL;
UINT g_video_buf_len = 0;
BOOL g_start_record_flag = FALSE;

HWND g_hWnd = 0;
BOOL g_Full_flag = TRUE;
TCHAR ip_address[AP_ADDRESS_LEN] = {0};
CInternetSession *m_pInetSession[UPDATE_MAX_NUM] = {NULL};
CFtpConnection *m_pFtpConnection[UPDATE_MAX_NUM] = {NULL}; 
UINT m_uPort = 0;
UINT m_net_uPort = 0;
//HANDLE g_hBurnThread_rev_data= INVALID_HANDLE_VALUE;
HANDLE g_hBurnThread_play_pcm= INVALID_HANDLE_VALUE;
HANDLE m_pcm_play_handle = INVALID_HANDLE_VALUE;
HANDLE m_download_handle = INVALID_HANDLE_VALUE;
char g_download_sd_wifi_flag = 0;
BOOL g_monitor_end_flag = TRUE;
BOOL play_pcm_finish_flag = FALSE;
//UINT rve_param[2] = {0};
char g_send_commad[UPDATE_MAX_NUM] = {0};
char g_test_fail_flag[UPDATE_MAX_NUM] = {0};
char g_test_pass_flag[UPDATE_MAX_NUM] = {0};  //0���ڲ����У� //1���Գɹ��� 2����ʧ��
char g_commad_type;
BOOL g_connet_flag = FALSE;
BOOL g_download_ptz_misc = TRUE;
BOOL download_file_flag = FALSE;
BOOL download_dev_file_flag = FALSE;
BOOL g_senddata_flag = TRUE;
BOOL entern_flag = FALSE;
BOOL login_entern_flag = FALSE;
//HANDLE g_heatThread= INVALID_HANDLE_VALUE;
HANDLE m_test_monitor_handle = INVALID_HANDLE_VALUE;
BOOL g_sd_test_success = FALSE;
BOOL g_test_monitor_flag = FALSE;
BOOL g_close_monitor = False;
HANDLE m_retest_sd_handle = INVALID_HANDLE_VALUE;
HANDLE m_retest_wifi_handle = INVALID_HANDLE_VALUE;

BOOL config_uid_enable_temp=FALSE;
BOOL config_lan_mac_enable_temp=FALSE;
BOOL g_finish_find_flag = TRUE;

BOOL g_sd_test_pass = TRUE;
BOOL g_wifi_test_pass = TRUE;
BOOL g_reset_test_pass = TRUE;

BOOL m_not_find_anyIP = FALSE;
BOOL m_find_anyIP = FALSE;
BOOL m_find_IP_end_flag = FALSE;
UINT m_ip_address_idex = 0;
BOOL g_sousuo_flag = FALSE;
CConfig_test g_test_config;
BOOL M_bConn;
TCHAR m_connect_ip[MAX_PATH+1] = {0};
TCHAR m_connect_uid[MAC_ADDRESS_LEN] = {0};
//TCHAR g_download_filename[MAX_PATH] = {0};
CString g_src_filename;
CString g_dst_filename;
UINT g_case_idex;
CLogFile  frmLogfile;
CLogUidFile frmLogUidFile;
UINT current_ip_idex = 0;

BOOL end_test=false;

BOOL g_move_test_connect_flag = FALSE;
BOOL first_flag = FALSE;
BOOL init_flag=FALSE;
BOOL no_put_flie_flag = FALSE;
BOOL g_test_finish_flag = FALSE;
BOOL g_pre_flag = FALSE;

long g_sd_size = 0;
T_SSID_INFO *g_ssid_info = NULL;
UINT g_ssid_num = 0;

HANDLE g_server_Thread = INVALID_HANDLE_VALUE;
HANDLE g_record_video_Thread = INVALID_HANDLE_VALUE;
HANDLE g_hupdateThread = INVALID_HANDLE_VALUE;
HANDLE g_heatThread[UPDATE_MAX_NUM] = {INVALID_HANDLE_VALUE};
HANDLE g_hBurnThread_rev_data[UPDATE_MAX_NUM] = {INVALID_HANDLE_VALUE};
HANDLE g_update_finish_Thread[UPDATE_MAX_NUM] = {INVALID_HANDLE_VALUE};
HANDLE g_all_updateThread[UPDATE_MAX_NUM] = {INVALID_HANDLE_VALUE};
HANDLE g_check_MAC_Thread = INVALID_HANDLE_VALUE;
char  m_update_flag[UPDATE_MAX_NUM] = {0};
T_IDEX_INFO g_param[UPDATE_MAX_NUM] = {0};
UINT g_updateing_num = 0; 
HANDLE g_handle = INVALID_HANDLE_VALUE;   //�ź���
BOOL g_update_all_flag = FALSE;
BOOL g_update_all_finish_flag = FALSE;
BOOL g_finish_flag  = FALSE;
UINT g_finish_idex  = 0;

UINT rve_param[UPDATE_MAX_NUM][2] = {0};
UINT heat_param[UPDATE_MAX_NUM][2] = {0};
UINT update_param[UPDATE_MAX_NUM][2] = {0};
BOOL one_update_finish = FALSE;
UINT g_time3 = 0;
T_IMAGE_INFO cut_out_on = {0};


#define    BUF_MAX_LEN  64*1024
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
	ON_BN_CLICKED(IDC_BUTTON_WRITE_MAC, &CAnykaIPCameraDlg::OnBnClickedButtonWriteMac)
	ON_BN_CLICKED(IDC_BUTTON_PASS, &CAnykaIPCameraDlg::OnBnClickedButtonPass)
	ON_BN_CLICKED(IDC_BUTTON_FAILED, &CAnykaIPCameraDlg::OnBnClickedButtonFailed)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CAnykaIPCameraDlg::OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, &CAnykaIPCameraDlg::OnBnClickedButtonNext)
	ON_EN_CHANGE(IDC_EDIT_WIFI_NAME, &CAnykaIPCameraDlg::OnEnChangeEditWifiName)
	ON_WM_HOTKEY()
	ON_WM_DESTROY()
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


UINT g_test_file_idex = 0;
void save_yuv_file(char *buf, UINT buf_len) 
{
	CFile g_file_fp_temp;
	TCHAR path[40] = {0};
#if 1
	//swprintf(path, _T("L:\\first_frame_test\\first_test_%d"), g_test_file_idex);
	//swprintf(path, _T("L:\\first_frame_test\\first_test_1"));
	swprintf(path, _T("L:\\yuv\\yuv_%d"), g_test_file_idex);
	g_file_fp_temp.Open(path,CFile::modeCreate|CFile::modeReadWrite);
	g_file_fp_temp.Write(buf,buf_len);
	g_file_fp_temp.Close();
	g_test_file_idex++;
#endif
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

#if 1
	m_Preview[0].SetWindowPos(NULL,0,0,rcDlg1.Width()-rcDlg2.Width()-10,
		rcDlg1.Height()-rcDlg3.Height()-10,SWP_NOMOVE|SWP_SHOWWINDOW);
	m_Preview[1].SetWindowPos(NULL,0,0,rcDlg1.Width()-rcDlg2.Width()-10,
		rcDlg1.Height()-rcDlg3.Height()-10,SWP_NOMOVE|SWP_SHOWWINDOW);
#else
	m_Preview[0].MoveWindow(rcDlg2.Width(), rcDlg3.Height(), rcDlg1.Width()-rcDlg2.Width()-10, rcDlg1.Height()-rcDlg3.Height()-10 );
	m_Preview[0].SetWindowPos(NULL,rcDlg2.Width(),rcDlg3.Height(),rcDlg1.Width()-rcDlg2.Width()-10,
		rcDlg1.Height()-rcDlg3.Height()-10,SWP_NOMOVE|SWP_SHOWWINDOW);
	m_Preview[1].SetWindowPos(NULL,rcDlg2.Width(),rcDlg3.Height(),rcDlg1.Width()-rcDlg2.Width()-10,
		rcDlg1.Height()-rcDlg3.Height()-10,SWP_NOMOVE|SWP_SHOWWINDOW);
		
#endif


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
		if (g_net_buf != NULL)
		{
			//memset(g_net_buf, 0, YUV_WIDTH_960P*YUV_HEIGHT_960P*4);
		}
		
	}
	g_start_open_flag = TRUE;
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


BOOL CAnykaIPCameraDlg::get_system_info()
{
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	OSVERSIONINFOEX os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if (GetVersionEx((OSVERSIONINFO *) &os))
	{
		//CString str;
		//str.Format(_T("ma:%d, ni:%d"), os.dwMajorVersion,os.dwMinorVersion);
		//AfxMessageBox(str, MB_OK);

		if (os.dwMajorVersion == 6 && os.dwMinorVersion == 2)
		{
			g_test_config.win10_flag = TRUE;//WIN10
		}
		else
		{
			g_test_config.win10_flag = FALSE;//��WIN10
		}
	}

	return TRUE;
}
//��������
BOOL CAnykaIPCameraDlg::ftp_StorePassword()
{
	HANDLE hFile;
	char buf[1024];
	DWORD dwWrite;
	int i;

	//prepare data
	memset(buf, 0, 1024);
	memcpy(buf, &passwd_ctrl, sizeof(passwd_ctrl));
	for(i = 0; i < 1024; i++)
	{
		buf[i] = buf[i] ^ (i*i + 5*i +1);
	}

	//write file
	hFile = CreateFile(g_test_config.ConvertAbsolutePath(_T("password.cfg")) , GENERIC_WRITE | GENERIC_READ , 
		FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL , 
		CREATE_ALWAYS , FILE_ATTRIBUTE_HIDDEN , NULL);

	if (hFile == INVALID_HANDLE_VALUE) 
	{
		return FALSE;
	}	
	//д���뵽�ĵ���
	if(!WriteFile(hFile,buf, 1024, &dwWrite, NULL))
	{
		CloseHandle(hFile);
		return FALSE;
	}

	CloseHandle(hFile);
	return TRUE;
}

//��ȡ����
BOOL CAnykaIPCameraDlg::ftp_GetPassword()
{
	HANDLE hFile;
	char buf[1024];
	DWORD dwRead;
	int i;

	//fetch data
	hFile = CreateFile(g_test_config.ConvertAbsolutePath(_T("password.cfg")), GENERIC_READ , FILE_SHARE_READ , NULL , 
		OPEN_EXISTING , FILE_ATTRIBUTE_HIDDEN , NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	//��ȡ����
	if(!ReadFile(hFile, buf, 1024, &dwRead, NULL))
	{
		CloseHandle(hFile);
		return FALSE;
	}

	//extract password
	for(i = 0; i < 1024; i++)
	{
		buf[i] = buf[i] ^ (i*i + 5*i +1);
	}

	memcpy(&passwd_ctrl, buf, sizeof(passwd_ctrl));

	CloseHandle(hFile);
	return TRUE;
}


BOOL CAnykaIPCameraDlg::get_password_and_name(void) 
{
	TCHAR username[MAX_PASSWD_LENGTH+1] = {0};
	TCHAR password[MAX_PASSWD_LENGTH+1] = {0};
	CString str;
	BOOL change_password = FALSE;

	memset(username, 0, MAX_PASSWD_LENGTH);
	memset(password, 0, MAX_PASSWD_LENGTH);
	m_RightDlg.GetDlgItemText(IDC_EDIT_NAME, str);
	if (!str.IsEmpty())
	{
		_tcsncpy(username, str, str.GetLength());
		if (_tcslen(username) == _tcslen(passwd_ctrl.use_name))
		{
			if (_tcsncmp(username, passwd_ctrl.use_name, _tcslen(username)) != 0)
			{
				//��¼
				change_password = TRUE;
			}
		}
		else
		{
			//��¼
			change_password = TRUE;
		}
	}
	else
	{

		_tcscpy(username, _T(""));
		change_password = TRUE;
	}

	m_RightDlg.GetDlgItemText(IDC_EDIT_PASSWORD, str);
	if (!str.IsEmpty())
	{
		_tcsncpy(password, str, str.GetLength());
		if (_tcslen(password) == _tcslen(passwd_ctrl.password))
		{
			if (_tcsncmp(password, passwd_ctrl.password, _tcslen(password)) != 0)
			{
				//��¼
				change_password = TRUE;
			}

		}
		else
		{
			//��¼
			change_password = TRUE;
		}
	}
	else
	{

		_tcscpy(password, _T(""));
		change_password = TRUE;
	}

	if (change_password )
	{
		memset(passwd_ctrl.use_name, 0, MAX_PASSWD_LENGTH);
		memset(passwd_ctrl.password, 0, MAX_PASSWD_LENGTH);
		_tcsncpy(passwd_ctrl.use_name , username, _tcslen(username));
		_tcsncpy(passwd_ctrl.password , password, _tcslen(password));
		ftp_StorePassword();
	}
	return TRUE;
}


BOOL CAnykaIPCameraDlg::OnInitDialog()
{
	CString sPath;
	TCHAR temp_buf_det[MAX_PATH] = {0};
	TCHAR temp_buf_src[MAX_PATH] = {0};
	TCHAR bufsprintf[MAX_PATH] = {0};
	char buf[2] = {0};
	//TCHAR *test_pcm_name = _T("pcm_test_c.pcm");
	CString str;
	//HANDLE hFile ;
	//BOOL bRet;
	int nPos;
	UINT i = 0;


	USES_CONVERSION;

	//test_pcm_buf(test_pcm_name);

	//HANDLE hFile = CreateFile(_T("H:\\cloud39E_tool\\toolsrc\\IPCTest_Tool\\pcm_test.pcm"), GENERIC_WRITE, FILE_SHARE_READ , NULL, 
	//				OPEN_ALWAYS , FILE_ATTRIBUTE_NORMAL , NULL);//�����ļ�
	//CloseHandle(hFile);


	

	
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
	ListView_SetExtendedListViewStyle(m_test_wifi_list.m_hWnd, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	GetModuleFileName(NULL,sPath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);
	sPath.ReleaseBuffer();
	nPos=sPath.ReverseFind ('\\');
	sPath=sPath.Left(nPos+1);
	_tcsncpy(path_module, sPath, MAX_PATH);

	g_handle = CreateSemaphore(NULL, 1, 1, NULL);

	get_system_info();

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

	 //hFile = CreateFile(ConvertAbsolutePath(_T("pcm_test.pcm")), GENERIC_WRITE, FILE_SHARE_READ , NULL, 
	//				OPEN_ALWAYS , FILE_ATTRIBUTE_NORMAL , NULL);//�����ļ�
	//CloseHandle(hFile);


#ifdef USE_LOG_FILE
	freopen(T2A(ConvertAbsolutePath(A2T((logInfoName)))), "w+t", stderr);
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
	next_test_flag = FALSE;



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
#if 0
	CRect rcDlg1,rcDlg2;
	GetClientRect(&rcDlg1);
	m_RightDlg.Create(IDD_FORMVIEW,this);
	m_RightDlg.GetClientRect(&rcDlg2);
	m_RightDlg.SetWindowPos(NULL,rcDlg1.Width()-rcDlg2.Width(),0,0,0,SWP_NOSIZE|SWP_SHOWWINDOW);

	m_BottomDlg.Create(IDD_BOTTOMVIEW,this);
    m_BottomDlg.GetClientRect(&rcDlg2);
	m_BottomDlg.SetWindowPos(NULL,0,rcDlg1.Height()-rcDlg2.Height(),0,0,SWP_NOSIZE|SWP_SHOWWINDOW);

#else

	CRect rcDlg1,rcDlg2,rcDlg3;
	GetClientRect(&rcDlg1);


	m_BottomDlg.Create(IDD_BOTTOMVIEW,this);
	m_BottomDlg.GetClientRect(&rcDlg2);
	m_BottomDlg.MoveWindow(0,0,rcDlg2.Width(),rcDlg2.Height());
	m_BottomDlg.SetWindowPos(NULL,0,0,rcDlg2.Width(),rcDlg2.Height(),SWP_NOSIZE|SWP_SHOWWINDOW);


	m_RightDlg.Create(IDD_FORMVIEW,this);
	m_RightDlg.GetClientRect(&rcDlg3);
	m_RightDlg.MoveWindow(0,rcDlg2.Height(),rcDlg3.Width(),rcDlg3.Height());
	m_RightDlg.SetWindowPos(NULL,0,rcDlg2.Height(),rcDlg3.Width(),rcDlg3.Height(),SWP_NOSIZE|SWP_SHOWWINDOW);

#endif	


	m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(FALSE);//����
	m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//����
	m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����
	//m_BottomDlg.GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(FALSE);//����
	//m_BottomDlg.GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(FALSE);//����
	//m_BottomDlg.GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(FALSE);//����

	//m_BottomDlg.GetDlgItem(IDC_BUTTON_AUTO_MOVE)->EnableWindow(FALSE);//����
	//m_BottomDlg.GetDlgItem(IDC_BUTTON_UP)->EnableWindow(FALSE);//����	
	//m_BottomDlg.GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow(FALSE);//����	
	//m_BottomDlg.GetDlgItem(IDC_BUTTON_LEFT)->EnableWindow(FALSE);//����	
	//m_BottomDlg.GetDlgItem(IDC_BUTTON_RIGHT)->EnableWindow(FALSE);//����
	
	//ListView_SetExtendedListViewStyle(m_RightDlg.m_test_config.m_hWnd, LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	ListView_SetExtendedListViewStyle(m_RightDlg.m_test_config.m_hWnd,   LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	//ListView_SetExtendedListViewStyle(m_RightDlg.m_test_wifi_list.m_hWnd, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	ListView_SetExtendedListViewStyle(m_BottomDlg.m_test_wifi_list.m_hWnd, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);


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
	
	for (i = 0; i < UPDATE_MAX_NUM; i++)
	{
		g_test_fail_flag[i]  = 0;
		g_send_commad[i]  = 0;;
	
	}
	g_test_config.config_voice_send_enable = 0; //�Խ�
	g_test_config.config_head_enable = 0;

	g_connet_flag = FALSE;
	g_senddata_flag = TRUE;
	memset(m_connect_ip, 0, MAX_PATH);
	memset(m_connect_uid, 0, MAC_ADDRESS_LEN);
	GetDlgItem(IDC_BUTTON_SET)->EnableWindow(FALSE);//���� 
	GetDlgItem(IDC_BUTTON_RECOVER_DEV)->EnableWindow(FALSE);//���� 


	if (g_test_config.need_tool_printf)
	{
		m_BottomDlg.GetDlgItem(IDC_BUTTON_PRINTF_SET)->EnableWindow(TRUE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_PRINTF)->EnableWindow(TRUE);//����
	}
	else
	{
		m_BottomDlg.GetDlgItem(IDC_BUTTON_PRINTF_SET)->EnableWindow(FALSE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_PRINTF)->EnableWindow(FALSE);//����
	}

	//�ж������ļ����Ƿ���ڣ������������ô�ͽ��д���
	if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(SERAIL_CONFIG)))
	{
		CreateDirectory(ConvertAbsolutePath(SERAIL_CONFIG), NULL);//�����ļ���
	}

	//�ж������ļ����Ƿ���ڣ������������ô�ͽ��д���
	if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(SERAIL_CONFIG_BAK)))
	{
		CreateDirectory(ConvertAbsolutePath(SERAIL_CONFIG_BAK), NULL);//�����ļ���
	}
#if 0
	//�ж������ļ����Ƿ���ڣ������������ô�ͽ��д���
	if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(TEST_CONFIG_DIR)))
	{
		CreateDirectory(ConvertAbsolutePath(TEST_CONFIG_DIR), NULL);//�����ļ���
	}
#endif

	GetDlgItem(IDC_EDIT_WIFI_NAME)->SetWindowText(g_test_config.m_wifi_name);
	m_BottomDlg.GetDlgItem(IDC_EDIT_WIFI_NAME)->SetWindowText(g_test_config.newest_version);
	//m_enter_test_config.GetDlgItem(IDC_TEST_TIME_MONITOR)->SetWindowText(g_test_config.m_wifi_name);
	//m_enter_test_config.GetDlgItem(IDC_TEST_TIME_RESET)->SetWindowText(g_test_config.m_wifi_name);
	
	str.Format(_T("id"));//
	m_RightDlg.m_test_config.InsertColumn(0, str, LVCFMT_LEFT, 40);

	str.Format(_T("UID"));//
	m_RightDlg.m_test_config.InsertColumn(1, str, LVCFMT_LEFT, 80);

	str.Format(_T("IP"));//
	m_RightDlg.m_test_config.InsertColumn(2, str, LVCFMT_LEFT, 80);

	str.Format(_T("name"));//
	m_RightDlg.m_test_config.InsertColumn(3, str, LVCFMT_LEFT, 60);

	str.Format(_T("version"));//
	m_RightDlg.m_test_config.InsertColumn(4, str, LVCFMT_LEFT, 60);

	#if 0
	str.Format(_T("id"));//
	m_RightDlg.m_test_wifi_list.InsertColumn(0, str, LVCFMT_LEFT, 30);

	str.Format(_T("ssid"));//
	m_RightDlg.m_test_wifi_list.InsertColumn(1, str, LVCFMT_LEFT, 130);

	str.Format(_T("quality"));//
	m_RightDlg.m_test_wifi_list.InsertColumn(2, str, LVCFMT_LEFT, 120);
	#endif

	str.Format(_T("id"));//
	m_BottomDlg.m_test_wifi_list.InsertColumn(0, str, LVCFMT_LEFT, 30);

	str.Format(_T("ssid"));//
	m_BottomDlg.m_test_wifi_list.InsertColumn(1, str, LVCFMT_LEFT, 130);

	str.Format(_T("quality"));//
	m_BottomDlg.m_test_wifi_list.InsertColumn(2, str, LVCFMT_LEFT, 120);


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

//	((CButton *)m_RightDlg.GetDlgItem(IDC_CHECK_UPDATE_FLAG))->SetCheck(g_test_config.update_find_flag);
	

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


	memset(passwd_ctrl.use_name, 0, sizeof(TCHAR)*MAX_PASSWD_LENGTH);
	memset(passwd_ctrl.password, 0, sizeof(TCHAR)*MAX_PASSWD_LENGTH);
	_tcscpy(passwd_ctrl.use_name, _T("root"));
	_tcscpy(passwd_ctrl.password, _T("cloud39ev200")); //�����ǿ�

	//��ȡ����
	ftp_GetPassword();

	//��ʾ����
	str.Format(_T("%s"), passwd_ctrl.use_name);//
	m_RightDlg.SetDlgItemText(IDC_EDIT_NAME, str);
	str.Format(_T("%s"), passwd_ctrl.password);//
	m_RightDlg.SetDlgItemText(IDC_EDIT_PASSWORD, str);

	m_bIsInit = TRUE;
	g_start_open_flag = TRUE;

	if (_tcscmp(g_test_config.m_mac_start_addr, g_test_config.m_mac_current_addr) > 0)
	{
		memset(g_test_config.m_mac_current_addr, 0, MAX_PATH);
		_tcsncpy(g_test_config.m_mac_current_addr, 
			g_test_config.m_mac_start_addr, _tcsclen(g_test_config.m_mac_start_addr));
	}
	
	if (g_test_config.m_test_ircut_time[0] == 0)
	{
		memcpy(buf, "3", 1);
		_tcscpy(g_test_config.m_test_ircut_time, A2T(buf));
	}
	str.Format(_T("%d"), g_test_config.m_test_ircut_time);
	GetDlgItemText(IDC_TEST_TIME_IRCUT, str);

	Creat_Anyka_Test_thread();

	Creat_test_monitor_thread();
	
	if (g_test_config.need_tool_printf)
	{
		creat_server_thread();
	}
	

	//Creat_find_ip_thread();

	//read_config(CONFIG_PATH);

	SetWindowText(TOOL_VERSOIN);

#if 0	
	//��ϵͳע���ȼ�
	bRet=RegisterHotKey(m_hWnd,HOTKEY_F1,0,VK_F1);
	bRet=RegisterHotKey(m_hWnd,HOTKEY_F2,0,VK_F2);
	bRet=RegisterHotKey(m_hWnd,HOTKEY_F3,0,VK_F3);
	bRet=RegisterHotKey(m_hWnd,HOTKEY_F4,0,VK_F4);
	bRet=RegisterHotKey(m_hWnd,HOTKEY_F6,0,VK_F6);
	bRet=RegisterHotKey(m_hWnd,HOTKEY_F7,0,VK_F7);
	
	//bRet=RegisterHotKey(m_hWnd,HOTKEY_FY,0,VK_RETURN);
	//bRet=RegisterHotKey(m_hWnd,HOTKEY_FY,0,'Y');
	//bRet=RegisterHotKey(m_hWnd,HOTKEY_Fy,0,'y');
	bRet=RegisterHotKey(m_hWnd,HOTKEY_FNULL,0,VK_SPACE);
	bRet=RegisterHotKey(m_hWnd,HOTKEY_F8,0,VK_F8);
	bRet=RegisterHotKey(m_hWnd,HOTKEY_F11,0,VK_F11);
	//bRet=RegisterHotKey(m_hWnd,HOTKEY_FESCAPE,0,VK_ESCAPE);
	//bRet=RegisterHotKey(m_hWnd,HOTKEY_FY,0,VK_RETURN);
	
#endif

	

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
	//m_Preview[1].UpdateWindow();

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
	
	if (g_test_config.need_tool_printf)
	{
		Close_server_thread();
	}
	
	
	g_monitor_end_flag = TRUE;
	m_ClientSocket.Rev_Socket_Close(0);
	close_pcm_play_thread();

	close_test_monitor_thread();

	Close_Anyka_Test_thread();
	

	g_close_monitor = TRUE;

	WiatForMonitorThreadEnd();

	for (int i = 0; i < 2; ++i) {
		CloseTheStream(i, TRUE);
	}

	CoUninitialize();

	av_lockmgr_register(NULL);
	KillTimer(TIMER_COMMAND);

	WSACleanup();
	Sleep(500);

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
		memset(m_path, 0, MAX_PATH);
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

		memset(m_path, 0, MAX_PATH);
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
			memset(ip_address, 0, AP_ADDRESS_LEN);
			_tcscpy(ip_address, subRight);
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
		CloseServer(0);
		return FALSE;
	}

	return TRUE;
}



BOOL CAnykaIPCameraDlg::Anyka_find_ip_thread() 
{
	UINT i = 0; 

	CServerSearch search;

	while (1)
	{
		On_find_ip();
		for(i = 0; i < 30; i++)
		{
			Sleep(1000);
			if(!g_connet_flag)
			{
				break;
			}
		}
		
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
	TCHAR *buf_RTSP_end = _T("/vs0");  //main_ch //ch0_0.264
	TCHAR *buf_ONVIF_end = _T("/ch0_0.264");//video0.sdp
	//TCHAR *buf_end = _T("//video1.sdp");
	UINT len = 0, idex = 0, i = 0;
	//TCHAR buf[100] = {0};


	while (1)
	{

		if (g_hTestThread == INVALID_HANDLE_VALUE)
		{
			CloseTheStream(0, TRUE);
			break;
		}

		if(g_test_finish_endrtsp_flag)
		{
			CloseTheStream(0, TRUE);
		}

		if (g_start_open_flag)
		{
			if (check_ip(m_connect_ip) && m_connect_ip[0] != 0)
			{

				Sleep(100);
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
				if(g_test_config.onvif_or_rtsp_flag)
				{
					len = _tcsclen(buf_RTSP_end);
					_tcsncpy(&ip_address[idex], buf_RTSP_end, len);
				}
				else
				{
					len = _tcsclen(buf_ONVIF_end);
					_tcsncpy(&ip_address[idex], buf_ONVIF_end, len);
				}
					

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
		Sleep(200);
	}

	return TRUE;
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

	Close_Anyka_Test_thread();

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

	if (g_reset_test_finish)
	{
		g_reset_test_finish = FALSE;
		m_RightDlg.m_test_pass_btn.SetFocus();
	}

	if (g_test_config.need_tool_printf)
	{
		m_BottomDlg.GetDlgItem(IDC_BUTTON_PRINTF_SET)->EnableWindow(TRUE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_PRINTF)->EnableWindow(TRUE);//����
	}
	else
	{
		m_BottomDlg.GetDlgItem(IDC_BUTTON_PRINTF_SET)->EnableWindow(FALSE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_PRINTF)->EnableWindow(FALSE);//����
	}


	if (g_test_config.need_close_monitor)
	{
		m_BottomDlg.GetDlgItem(IDC_BUTTON_CLOSE_MONITOR)->EnableWindow(TRUE);//����
	}
	else
	{
		m_BottomDlg.GetDlgItem(IDC_BUTTON_CLOSE_MONITOR)->EnableWindow(FALSE);//����
	}

	if (g_test_config.need_close_monitor)
	{
		m_BottomDlg.GetDlgItem(IDC_BUTTON_CLOSE_MONITOR)->EnableWindow(TRUE);//����
	}
	else
	{
		m_BottomDlg.GetDlgItem(IDC_BUTTON_CLOSE_MONITOR)->EnableWindow(FALSE);//����
	}


	

	if(!g_finish_flag && g_sousuo_flag && !g_update_all_flag)
	//if(m_ip_address_idex > 0)
	{
		g_sousuo_flag = FALSE;
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

			//m_RightDlg.m_test_config.InsertItem(i, _T(""));
			//m_RightDlg.m_test_config.SetCheck(i, g_update_flag[i]);
			strTemp.Format(_T("%d"), i+1);
			m_RightDlg.m_test_config.InsertItem(i, strTemp);
			strTemp.Format(_T("%s"), g_test_config.m_last_config[i].Current_IP_UID);
			m_RightDlg.m_test_config.SetItemText(i, 1, strTemp);
			m_RightDlg.m_test_config.SetItemText(i, 2, g_test_config.m_last_config[i].Current_IP_address_buffer);
			m_RightDlg.m_test_config.SetItemText(i, 3, g_test_config.m_last_config[i].Current_IP_diver_name);
			m_RightDlg.m_test_config.SetItemText(i, 4, g_test_config.m_last_config[i].Current_IP_version);

			m_find_anyIP = TRUE;
		}
	}

	if (!g_finish_flag && m_not_find_anyIP)
	{
		m_not_find_anyIP = FALSE;
		M_bConn = FALSE;
		m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
		AfxMessageBox(_T("û���������κ��豸!"));
	}
	else
	{
		if (m_find_anyIP)// && g_first_find_flag)
		{
			m_find_anyIP = FALSE;
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
			memset(m_connect_uid, 0, MAC_ADDRESS_LEN);
			len = _tcsclen(g_test_config.m_last_config[current_ip_idex].Current_IP_UID);
			_tcsncpy(m_connect_uid, g_test_config.m_last_config[current_ip_idex].Current_IP_UID, len);
			g_test_monitor_flag = TRUE;
			m_RightDlg.SetDlgItemText(IDC_EDIT_IP, g_test_config.m_last_config[current_ip_idex].Current_IP_address_buffer);
			m_RightDlg.SetDlgItemText(IDC_EDIT_UID, g_test_config.m_last_config[current_ip_idex].Current_IP_UID);
			m_RightDlg.SetDlgItemText(IDC_EDIT_MAC, g_test_config.m_last_config[current_ip_idex].Current_IP_MAC);
			m_RightDlg.SetDlgItemText(IDC_EDIT_VERSION2, g_test_config.m_last_config[current_ip_idex].Current_IP_version);
			
		}
	}


	if(g_finish_find_flag && !start_flag)
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_FIND_IP)->EnableWindow(TRUE);//����
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


	//����
	if (g_update_all_flag)
	{
		for(i = 0; i < m_ip_address_idex; i++)
		{
			if (m_update_flag[i] == 1)
			{
				m_RightDlg.m_test_config.SetItemText(i, 3, _T("�����ɹ�"));
			}
			else if (m_update_flag[i] == 2)
			{
				m_RightDlg.m_test_config.SetItemText(i, 3, _T("����ʧ��"));
				
			}
			else if (m_update_flag[i] == 3)
			{

				m_RightDlg.m_test_config.SetItemText(i, 3, _T("������"));
			}
			else if (m_update_flag[i] == 4)
			{
				m_RightDlg.m_test_config.SetItemText(i, 3, _T("��������������"));
			}
			else
			{
			}
		}
	}
#if 0
	if (g_update_all_finish_flag)
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_UPDATE_ALL)->EnableWindow(TRUE);
	}

	UINT num = m_RightDlg.m_test_config.GetItemCount();

	for (i =0; i < num; i++)
	{
		BOOL ret = (BOOL)m_RightDlg.m_test_config.GetCheck(i);

		if (ret == TRUE)
		{
			g_update_flag[i] = 1;
		}
		else
		{
			g_update_flag[i] = 0;
		}
	}
#endif

	//��ʾ���Խ��
	if (1)
	{
		CString str;

		if(g_test_config.config_sd_enable)
		{
			if (g_test_config.m_test_sd_flag == 1)
			{
				m_RightDlg.SetDlgItemText(IDC_STATIC_TSET_SD, _T("�ɹ�"));

				str.Format(_T("%lld byte"), g_test_config.m_sd_size);
				m_RightDlg.SetDlgItemText(IDC_STATIC_SD_INFO, str);

			}
			else if (g_test_config.m_test_sd_flag == 2)
			{
				m_RightDlg.SetDlgItemText(IDC_STATIC_TSET_SD, _T("ʧ��"));
				m_RightDlg.SetDlgItemText(IDC_STATIC_SD_INFO, _T("sd������ʧ��"));
			}
			else
			{
				m_RightDlg.SetDlgItemText(IDC_STATIC_TSET_SD, _T("δ����"));
				m_RightDlg.SetDlgItemText(IDC_STATIC_SD_INFO, _T(""));
			}
		}
		else
		{
			m_RightDlg.SetDlgItemText(IDC_STATIC_TSET_SD, _T("δ����"));
			m_RightDlg.SetDlgItemText(IDC_STATIC_SD_INFO, _T(""));
		}

		if(g_test_config.config_wifi_enable)
		{
			if (g_test_config.m_test_wifi_flag == 1)
			{
				g_test_config.m_test_wifi_flag = 3;

				m_RightDlg.SetDlgItemText(IDC_STATIC_TEST_WIFI, _T("�ɹ�"));
				m_BottomDlg.m_test_wifi_list.DeleteAllItems();
				for(i = 0; i < g_test_config.m_ssid_num; i++)
				{	
					str.Format(_T("%d"), i+1);
					m_BottomDlg.m_test_wifi_list.InsertItem(i, str);
					m_BottomDlg.m_test_wifi_list.SetItemText(i, 1,A2T(g_test_config.m_ssid_info[i].wify_ssid_name));
					str.Format(_T("%d"), g_test_config.m_ssid_info[i].quality);
					m_BottomDlg.m_test_wifi_list.SetItemText(i, 2,str);
				}

			}
			else if (g_test_config.m_test_wifi_flag == 2)
			{
				m_RightDlg.SetDlgItemText(IDC_STATIC_TEST_WIFI, _T("ʧ��"));
			}
			else if (g_test_config.m_test_wifi_flag == 0)
			{
				m_RightDlg.SetDlgItemText(IDC_STATIC_TEST_WIFI, _T("δ����"));
			}
		}
		else
		{
			m_RightDlg.SetDlgItemText(IDC_STATIC_TEST_WIFI, _T("δ����"));
		}

		if (g_test_config.m_test_reset_flag == 1)
		{
			m_RightDlg.SetDlgItemText(IDC_STATIC_reset, _T("�ɹ�"));
		}
		else if (g_test_config.m_test_reset_flag == 2)
		{
			m_RightDlg.SetDlgItemText(IDC_STATIC_reset, _T("ʧ��"));
		}
		else
		{
			m_RightDlg.SetDlgItemText(IDC_STATIC_reset, _T("δ����"));
		}

		if (g_test_config.m_test_MAC_flag == 1)
		{
			m_RightDlg.SetDlgItemText(IDC_STATIC_TEST_MAC, _T("�ɹ�"));
		}
		else if (g_test_config.m_test_MAC_flag == 2)
		{
			m_RightDlg.SetDlgItemText(IDC_STATIC_TEST_MAC, _T("ʧ��"));
		}
		else
		{
			m_RightDlg.SetDlgItemText(IDC_STATIC_TEST_MAC, _T("δд��"));
		}

		if (g_test_config.m_test_UID_flag == 1)
		{
			m_RightDlg.SetDlgItemText(IDC_STATIC_TEST_UID, _T("�ɹ�"));
		}
		else if (g_test_config.m_test_UID_flag == 2)
		{
			m_RightDlg.SetDlgItemText(IDC_STATIC_TEST_UID, _T("ʧ��"));
		}
		else
		{
			m_RightDlg.SetDlgItemText(IDC_STATIC_TEST_UID, _T("δд��"));
		}
	}

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


	memset(strURL, 0, MAX_RTSP_URL_LEN);
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
	//TCHAR *username = _T("root");
	//TCHAR *password = _T("cloud39ev200");

	if(m_pInetSession[idex] != NULL)
	{
		delete m_pInetSession[idex];
		m_pInetSession[idex] = NULL;
	}
	m_pInetSession[idex] = new CInternetSession(AfxGetAppName(), 1, PRE_CONFIG_INTERNET_ACCESS);
	try
	{
		// addr       ftp�������ĵ�ַ  LPCTSTR
		// username   ��½�û���       LPCTSTR 
		// password   ����            LPCTSTR
		// port       �˿�            UINT
		if(m_pFtpConnection[idex] != NULL)
		{
			m_pFtpConnection[idex]->Close();
			delete m_pFtpConnection[idex];
			m_pFtpConnection[idex] = NULL;
		}

		get_password_and_name();

		m_pFtpConnection[idex] = m_pInetSession[idex]->GetFtpConnection(addr, passwd_ctrl.use_name, passwd_ctrl.password, 21 ,g_test_config.win10_flag);
		//m_pFtpConnection[idex] = m_pInetSession[idex]->GetFtpConnection(addr, username, password, 21 ,g_test_config.win10_flag);
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
		m_pFtpConnection[idex] = NULL;
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
		//AfxMessageBox(_T("Socket_Create fail"));
		return FALSE;
	}
	//if(m_ClientSocket.Connect(addr, 6789))
	if(m_ClientSocket.Socket_Connect(T2A(addr), 6789, idex))	
	{
#if 0
		//����ftp������
		//����ftp������
		if (!Connet_FTPServer(addr, idex))
		{
			//AfxMessageBox(_T("Connet_FTPServer fail"));
			return FALSE;
		}

		if(!create_thread_heat(addr, idex))
		{
			//AfxMessageBox(_T("���������߳�ʧ��"));
			return FALSE;
		}

#endif

		if (!Creat_find_result_thread(idex))
		{
			//AfxMessageBox(_T("�������������߳�ʧ��"));
			return FALSE;
		}
		g_senddata_flag = FALSE;

	}
	else
	{
		//AfxMessageBox(_T("Socket_Connect fail"));
		return FALSE;
	}

	g_connet_flag = TRUE;

	return TRUE;
}

BOOL CAnykaIPCameraDlg::Anyka_Test_check_no_info(UINT timeout)
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
		if (time2 - time1 > timeout)
		{
			g_test_pass_flag[0] = 0;
			//AfxMessageBox(_T("��ʱ(30s)û�з���ȷ������"));
			return FALSE;
		}
		if (g_test_pass_flag[0]  == 1)
		{

			g_test_pass_flag[0]  = 0;
			return TRUE;
		}
		else if (g_test_pass_flag[0]  == 2)
		{
			g_test_pass_flag[0]  = 0;
			return FALSE;
		}
	}

}


BOOL CAnykaIPCameraDlg::Anyka_Test_check_info_update(UINT timeout, UINT idex)
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
		if (time2 - time1 > timeout)
		{
			g_test_pass_flag[idex]  = 0;
			AfxMessageBox(_T("��ʱ(30s)û�з���ȷ������"));
			return FALSE;
		}
		if (g_test_pass_flag[idex]  == 1)
		{

			g_test_pass_flag[idex]  = 0;
			return TRUE;
		}
		else if (g_test_pass_flag[idex]  == 2)
		{
			g_test_pass_flag[idex]  = 0;
			return FALSE;
		}
	}

}

BOOL CAnykaIPCameraDlg::Anyka_Test_UID_check_info(UINT timeout)
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
		if (time2 - time1 > timeout)
		{
			AfxMessageBox(_T("UIDд�볬ʱ(30s)û�з���ȷ������"));
			return FALSE;
		}
		if (g_test_config.m_test_UID_flag == 1)
		{
			return TRUE;
		}
		else if (g_test_config.m_test_UID_flag == 2)
		{
			return FALSE;
		}
	}

}


BOOL CAnykaIPCameraDlg::Anyka_Test_mac_check_info(UINT timeout)
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
		if (time2 - time1 > timeout)
		{
			AfxMessageBox(_T("MAC��ʱ(30s)û�з���ȷ������"));
			return FALSE;
		}
		if (g_test_config.m_test_MAC_flag == 1)
		{
			return TRUE;
		}
		else if (g_test_config.m_test_MAC_flag == 2)
		{
			return FALSE;
		}
	}

}

BOOL CAnykaIPCameraDlg::Anyka_Test_sd_check_info(UINT timeout)
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
		if (time2 - time1 > timeout)
		{
			AfxMessageBox(_T("sd���Գ�ʱ(30s)û�з���ȷ������"));
			return FALSE;
		}
		if (g_test_config.m_test_sd_flag == 1)
		{
			return TRUE;
		}
		else if (g_test_config.m_test_sd_flag == 2)
		{
			return FALSE;
		}
	}

}

BOOL CAnykaIPCameraDlg::Anyka_Test_wifi_check_info(UINT timeout)
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
		if (time2 - time1 > timeout)
		{
			AfxMessageBox(_T("wifi���Գ�ʱ(30s)û�з���ȷ������"));
			return FALSE;
		}
		if (g_test_config.m_test_wifi_flag == 1)
		{
			return TRUE;
		}
		else if (g_test_config.m_test_wifi_flag == 2)
		{
			return FALSE;
		}
	}

}


BOOL CAnykaIPCameraDlg::Anyka_Test_check_info(UINT timeout)
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
		if (time2 - time1 > timeout)
		{
			g_test_pass_flag[0]  = 0;
			AfxMessageBox(_T("��ʱ(30s)û�з���ȷ������"));
			return FALSE;
		}
		if (g_test_pass_flag[0]  == 1)
		{

			g_test_pass_flag[0]  = 0;
			return TRUE;
		}
		else if (g_test_pass_flag[0]  == 2)
		{
			g_test_pass_flag[0]  = 0;
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
#if 0
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

	g_test_pass_flag[0] = 0;

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
	if (!Anyka_Test_check_info(10000))
	{
		AfxMessageBox(_T("����ʧ��"), MB_OK);
		return FALSE;
	}
	g_test_pass_flag[0] = 0;
	MessageBox(_T("���óɹ�"), MB_OK);
#endif
	MessageBox(_T("��֧��"), MB_OK);
	return TRUE;
}


void CAnykaIPCameraDlg::CloseServer_image(UINT idex) 
{
	//�������С����̨ת���߳�ֹͣ
	//Send_cmd(TEST_ENABLE_KEY, 1, "test_ptz_cmd", "9");


	if(m_pFtpConnection[idex] != NULL)
	{
		m_pFtpConnection[idex]->Close();
		delete m_pFtpConnection[idex];
		m_pFtpConnection[idex] = NULL;
	}

	if(m_pInetSession[idex] != NULL)
	{
		m_pInetSession[idex]->Close();
		delete m_pInetSession[idex];
		m_pInetSession[idex] = NULL;
	}

	g_send_commad[idex] = 0;
	g_test_fail_flag[idex]  = 0;
	download_file_flag = FALSE;
	download_dev_file_flag = FALSE;

	close_thread_heat(idex);
	close_thread_rev_data(idex);
	m_ClientSocket.Heat_Socket_Close(idex);
	m_ClientSocket.Socket_Close(idex);

	g_connet_flag = FALSE;

	g_monitor_end_flag = true;
	close_pcm_play_thread();

	g_finish_find_flag = TRUE;
	start_flag  = FALSE;
	next_test_flag = TRUE;
	g_config_start_flag = TRUE;

	g_move_test_connect_flag = FALSE;
	start_test_flag = FALSE;
	g_start_success_flag = FALSE;
}

void CAnykaIPCameraDlg::CloseServer(UINT idex) 
{
	
	//Send_cmd(TEST_COMMAND_FINISH, 0, NULL, NULL);
	if (!g_update_all_flag)
	{
		Send_cmd(TEST_ENABLE_KEY, 1, NULL, NULL, 0, idex);
		Sleep(100);
	}

	//�������С����̨ת���߳�ֹͣ
	//Send_cmd(TEST_ENABLE_KEY, 1, "test_ptz_cmd", "9");


	if(m_pFtpConnection[idex] != NULL)
	{
		m_pFtpConnection[idex]->Close();
		delete m_pFtpConnection[idex];
		m_pFtpConnection[idex] = NULL;
	}

	if(m_pInetSession[idex] != NULL)
	{
		m_pInetSession[idex]->Close();
		delete m_pInetSession[idex];
		m_pInetSession[idex] = NULL;
	}

	g_send_commad[idex] = 0;
	g_test_fail_flag[idex]  = 0;
	download_file_flag = FALSE;
	download_dev_file_flag = FALSE;

	close_thread_heat(idex);
	close_thread_rev_data(idex);
	m_ClientSocket.Heat_Socket_Close(idex);
	m_ClientSocket.Socket_Close(idex);

	if(g_connet_flag)
	{
		g_start_open_flag = false;
		g_test_finish_endrtsp_flag = true;
		memset(m_connect_ip, 0, MAX_PATH);
		memset(m_connect_uid, 0, MAC_ADDRESS_LEN);
		
	}

	g_connet_flag = FALSE;

	g_monitor_end_flag = true;
	close_pcm_play_thread();
	
	if (!g_update_all_flag)
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(FALSE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(TRUE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_FIND_IP)->EnableWindow(TRUE);//����
		m_RightDlg.GetDlgItem(IDC_LIST1)->EnableWindow(TRUE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_CONFIGURE)->EnableWindow(TRUE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_WRITE_UID)->EnableWindow(TRUE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);;//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);;//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->SetWindowText(_T("��ʼ����"));
	}
	

	//m_BottomDlg.GetDlgItem(IDC_BUTTON_AUTO_MOVE)->EnableWindow(FALSE);//����
	//m_BottomDlg.GetDlgItem(IDC_BUTTON_UP)->EnableWindow(FALSE);//����	
	//m_BottomDlg.GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow(FALSE);//����	
	//m_BottomDlg.GetDlgItem(IDC_BUTTON_LEFT)->EnableWindow(FALSE);//����	
	//m_BottomDlg.GetDlgItem(IDC_BUTTON_RIGHT)->EnableWindow(FALSE);//����
	g_finish_find_flag = TRUE;
	start_flag  = FALSE;
	next_test_flag = TRUE;
	g_config_start_flag = TRUE;

	g_move_test_connect_flag = FALSE;
	start_test_flag = FALSE;
	g_start_success_flag = FALSE;

	

}

void CAnykaIPCameraDlg::OnBnClickedButtonSet()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//���Ƚ�������

#if 0
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
#endif

}


#if 0
DWORD WINAPI find_result_main(void *lpParameter)
{
	char commad_type;
	char id_type;
	//char *head_name = "VCMD";
	//char *end_name = "VEND";
	CRightCtrlDlg  TestToolDlg;
	CClientSocket m_ClientSocket_heat;
	int len = 0;
	char *lpBuf = NULL;
	UINT nBufLen = 256;
	UINT i,j;
	short check_sum = 0;
	short check_sum_temp = 0;
	UINT len_temp = 0;
	char *data = NULL;
	CString data2;
	char *saved_header_data = NULL;
	UINT incomplete_buflen = 0;
	int count = 0;
	int param_len = 0;
	CString str;
	BOOL start_data_flag = FALSE;
	BOOL end_data_flag = FALSE;
	int incomplete_len = 0;
	int incomplete_datalen = 0;
	UINT idex = 0;


	lpBuf = new char[nBufLen + 1];
	if (lpBuf == NULL)
	{
		AfxMessageBox(_T("malloc receive buf fail"), MB_OK);
		return -1;
	}

	saved_header_data = new char[nBufLen + 1];
	if (saved_header_data == NULL)
	{
		AfxMessageBox(_T("malloc receive buf fail"), MB_OK);
		return -1;
	}

	data = new char[nBufLen + 1];
	if (data == NULL)
	{
		AfxMessageBox(_T("malloc receive buf fail"), MB_OK);
		return -1;
	}
	//��ѯ���Խ��
	while (1)
	{
		if (g_hBurnThread_rev_data[idex] != INVALID_HANDLE_VALUE)
		{
			commad_type = 0;//��ʼ��
			id_type = 0;

			memset(lpBuf,0,nBufLen + 1);
			memset(saved_header_data,0,nBufLen + 1);
			memset(data,0,nBufLen + 1);

			//��������
			len = m_ClientSocket_heat.Socket_Receive(lpBuf, nBufLen, 0);
			if (len == -1)
			{
				continue;
			}
			//�Խ��յ������ݰ����д���
			if(len> 0 && (UINT)len<= nBufLen)
			{
				for(i = 0;i<(UINT)len;i++)
				{
					if(i == len)
					{
						break;
					}
					if((len > 4) || (incomplete_datalen > 4))
					{
						if(incomplete_datalen > 4)
						{
							len = incomplete_datalen;
						}
						for(j = i;j<= len - 4;j++)
						{
							//�ж����ݰ����Ƿ��а�ͷ
							if(lpBuf[j] == 'V' && lpBuf[j + 1] == 'C' && lpBuf[j + 2] == 'M' && lpBuf[j + 3] == 'D')
							{
								start_data_flag = TRUE;
								count += 4;
								break;
							}
						}
						//����а�ͷ
						if(start_data_flag)
						{
							start_data_flag = FALSE;

							//�ж����ݰ��Ƿ��а�β
							for(int k = j + 3; k<= len - 4; k++)
							{
								if(lpBuf[k] == 'V' && lpBuf[k + 1] == 'E' && lpBuf[k + 2] == 'N' && lpBuf[k + 3] == 'D')
								{
									end_data_flag = TRUE;
									param_len = k -j + 4 + 1;
									break;
								}
							}
							//����а�β
							if(end_data_flag)
							{
								end_data_flag = FALSE; 

								//������ؽṹ��Ա����
								strncpy(&id_type,&lpBuf[j+3],1);
								count += 1;
								strncpy(&commad_type,&lpBuf[j+4],1);
								count += 1;
								
#if 1								
								//�ж��������ֲ�������
								switch(id_type)
								{
								case CASE_SPEAKER:
									if(CMD_RESPONSE == commad_type)
									{
										strncpy((char *)&len_temp, &lpBuf[j+5], 2);
										count += 2;
										if (len_temp > 0)
										{
											strncpy(data, &lpBuf[j+7], len_temp);
											count += len_temp;
										}
										strncpy((char *)&check_sum,&lpBuf[j+7+len_temp],2);
										count += 2;
										for(int k = j-1;k <= (UINT)len - 6;k++)
										{
											check_sum_temp += lpBuf[k];
										}
										if(check_sum != check_sum_temp)
										{
											return FALSE;
										}
									}
									break;
								case CASE_CLOUD_DECK:
									if(CMD_RESPONSE == commad_type)
									{
										strncpy((char *)&len_temp, &lpBuf[j+5], 2);
										count += 2;
										if (len_temp > 0)
										{
											strncpy(data, &lpBuf[j+7], len_temp);
											count += len_temp;
										}
										strncpy((char *)&check_sum,&lpBuf[j+7+len_temp],2);
										count += 2;
										for(int k =j-1; k<(UINT)len-6;k++)
										{
											check_sum_temp += lpBuf[k];
										}
										if(check_sum != check_sum_temp)
										{
											return FALSE;
										}
									}
									break;
								case CASE_WIFI_CONNECT:
									if(CMD_RESPONSE == commad_type)
									{
										strncpy((char *)&len_temp, &lpBuf[j+5], 2);
										count += 2;
										if (len_temp > 0)
										{
											strncpy(data, &lpBuf[j+7], len_temp);

											//str.Format(_T("%s"),data);
											//TestToolDlg.GetDlgItem(IDC_EDIT_WIFI)->SetWindowText(str);
											//return 1;

											count += len_temp;
										}
										strncpy((char *)&check_sum,&lpBuf[j+7+len_temp],2);
										count += 2;
										if(data != 0)
										{
											data2.Format(_T("%s"),data);
											TestToolDlg.SetDlgItemText(IDC_STATIC_text_wifi,data2);
											
										}
										else
										{
											return FALSE;
										}
										for(int k =j-1;k<(UINT)len-6;k++)
										{
											check_sum_temp += lpBuf[k];
										}
										if(check_sum != check_sum_temp)
										{
											return FALSE;
										}
										
									}
									break;
								case CASE_SD_CARD:
									if(CMD_RESPONSE == commad_type)
									{
										strncpy((char *)&len_temp, &lpBuf[j+5], 2);
										count += 2;
										if (len_temp > 0)
										{
											strncpy(data, &lpBuf[j+7], len_temp);

											//str.Format(_T("%s"),data);
											//TestToolDlg.GetDlgItem(IDC_EDIT_WIFI)->SetWindowText(str);
											//return 1;

											count += len_temp;
										}
										strncpy((char *)&check_sum,&lpBuf[j+7+len_temp],2);
										count += 2;
										if (data != 0)
										{
											data2.Format(_T("%s"),data);
											TestToolDlg.SetDlgItemText(IDC_STATIC_text_sd,data2);
											
										}
										else
										{
											return FALSE;
										}
										for(int k =j-1;k<(UINT)len-6;k++)
										{
											check_sum_temp += lpBuf[k];
										}
										if(check_sum != check_sum_temp)
										{
											return FALSE;
										}
									}
									break;
								case CASE_MONITOR:
									if(CMD_RESPONSE == commad_type)
									{
										strncpy((char *)&len_temp, &lpBuf[j+5], 2);
										count += 2;
										if (len_temp > 0)
										{
											strncpy(data, &lpBuf[j+7], len_temp);
											count += len_temp;
										}
										strncpy((char *)&check_sum,&lpBuf[j+7+len_temp],2);
										count += 2;
										for(int k =j-1;k<(UINT)len-6;k++)
										{
											check_sum_temp += lpBuf[k];
										}
										if(check_sum != check_sum_temp)
										{
											return FALSE;
										}
									}
									break;
								case CASE_FOCUS_EFFECT:
									if(CMD_RESPONSE == commad_type)
									{
										strncpy((char *)&len_temp, &lpBuf[j+5], 2);
										count += 2;
										if (len_temp > 0)
										{
											strncpy(data, &lpBuf[j+7], len_temp);
											count += len_temp;
										}
										strncpy((char *)&check_sum,&lpBuf[j+7+len_temp],2);
										count += 2;
										for(int k =j-1;k<(UINT)len-6;k++)
										{
											check_sum_temp += lpBuf[k];
										}
										if(check_sum != check_sum_temp)
										{
											return FALSE;
										}
									}
									break;
								case CASE_IRCUT:
									if(CMD_RESPONSE == commad_type)
									{
										strncpy((char *)&len_temp, &lpBuf[j+5], 2);
										count += 2;
										if (len_temp > 0)
										{
											strncpy(data, &lpBuf[j+7], len_temp);
											count += len_temp;
										}
										strncpy((char *)&check_sum,&lpBuf[j+7+len_temp],2);
										count += 2;
										for(int k =j-1;k<(UINT)len-6;k++)
										{
											check_sum_temp += lpBuf[k];
										}
										if(check_sum != check_sum_temp)
										{
											return FALSE;
										}
									}
									break;
								case CASE_RESET:
									if(CMD_RESPONSE == commad_type)
									{
										strncpy((char *)&len_temp, &lpBuf[j+5], 2);
										count += 2;
										if (len_temp > 0)
										{
											strncpy(data, &lpBuf[j+7], len_temp);

											//str.Format(_T("%s"),data);
											//TestToolDlg.GetDlgItem(IDC_EDIT_WIFI)->SetWindowText(str);
											//return 1;

											count += len_temp;
										}
										strncpy((char *)&check_sum,&lpBuf[j+7+len_temp],2);
										count += 2;
										if(data != 0)
										{
											data2.Format(_T("%s"),data);
											TestToolDlg.SetDlgItemText(IDC_STATIC_reset,data2);
											
										}
										else
										{
											return FALSE;
										}
										for(int k =j-1;k<(UINT)len-6;k++)
										{
											check_sum_temp += lpBuf[k];
										}
										if(check_sum != check_sum_temp)
										{
											return FALSE;
										}
									}
									break;
								case CASE_MAC_BURN:
									if(CMD_RESPONSE == commad_type)
									{
										strncpy((char *)&len_temp, &lpBuf[j+5], 2);
										count += 2;
										if (len_temp > 0)
										{
											strncpy(data, &lpBuf[j+7], len_temp);
											count += len_temp;
										}
										strncpy((char *)&check_sum,&lpBuf[j+7+len_temp],2);
										count += 2;
										for(int k =j-1;k<(UINT)len-6;k++)
										{
											check_sum_temp += lpBuf[k];
										}
										if(check_sum != check_sum_temp)
										{
											return FALSE;
										}
									}
									break;
								case CASE_UID_BURN:
									if(CMD_RESPONSE == commad_type)
									{
										strncpy((char *)&len_temp, &lpBuf[j+5], 2);
										count += 2;
										if (len_temp > 0)
										{
											strncpy(data, &lpBuf[j+7], len_temp);
											count += len_temp;
										}
										strncpy((char *)&check_sum,&lpBuf[j+7+len_temp],2);
										count += 2;
										for(int k =j-1;k<(UINT)len-6;k++)
										{
											check_sum_temp += lpBuf[k];
										}
										if(check_sum != check_sum_temp)
										{
											return FALSE;
										}
									}
									break;							
								}//switch����
#endif
								if(param_len == (count + 4))
								{
									break;
								}
								else
								{
									return FALSE;
								}
							}//if(end_data_flag)����					
							else //����а�ͷû��β���������ݣ�������һ�β�ѯ
							{
								incomplete_buflen = len;
								saved_header_data = lpBuf;
								break;
							}
						}//if(start_data_flag)����
						else //������ݰ�û�а�ͷ
						{
							//�ж����ݰ��Ƿ��а�β
							for(int k = j + 3; k<= len - 4; k++)
							{
								if(lpBuf[k] == 'V' && lpBuf[k + 1] == 'E' && lpBuf[k + 2] == 'N' && lpBuf[k + 3] == 'D')
								{
									end_data_flag = TRUE;
									break;
								}
							}
							if(end_data_flag)
							{
								incomplete_len += len;
								//�γ�һ����ɵ����ݰ�
								strcat(saved_header_data,lpBuf);

								//������ؽṹ��Ա����
								strncpy(&id_type,&saved_header_data[j+3],1);
								count += 1;
								strncpy(&commad_type,&saved_header_data[j+4],1);
								count += 1;
#if 1							
								//�ж��������ֲ�������
								switch(id_type)
								{
								case CASE_SPEAKER:
									if(CMD_RESPONSE == commad_type)
									{
										strncpy((char *)&len_temp, &saved_header_data[j+5], 2);
										count += 2;
										if (len_temp > 0)
										{
											strncpy(data, &saved_header_data[j+7], len_temp);
											count += len_temp;
										}
										strncpy((char *)&check_sum,&saved_header_data[j+7+len_temp],2);
										count += 2;
										for(int k = j-1;k <= (UINT)len - 6;k++)
										{
											check_sum_temp += saved_header_data[k];
										}
										if(check_sum != check_sum_temp)
										{
											return FALSE;
										}
									}
									break;
								case CASE_CLOUD_DECK:
									if(CMD_RESPONSE == commad_type)
									{
										strncpy((char *)&len_temp, &saved_header_data[j+5], 2);
										count += 2;
										if (len_temp > 0)
										{
											strncpy(data, &saved_header_data[j+7], len_temp);
											count += len_temp;
										}
										strncpy((char *)&check_sum,&saved_header_data[j+7+len_temp],2);
										count += 2;
										for(int k =j-1; k<(UINT)len-6;k++)
										{
											check_sum_temp += saved_header_data[k];
										}
										if(check_sum != check_sum_temp)
										{
											return FALSE;
										}
									}
									break;
								case CASE_WIFI_CONNECT:
									if(CMD_RESPONSE == commad_type)
									{
										strncpy((char *)&len_temp, &saved_header_data[j+5], 2);
										count += 2;
										if (len_temp > 0)
										{
											strncpy(data, &saved_header_data[j+7], len_temp);

											//str.Format(_T("%s"),data);
											//TestToolDlg.GetDlgItem(IDC_EDIT_WIFI)->SetWindowText(str);
											//return 1;

											count += len_temp;
										}
										strncpy((char *)&check_sum,&saved_header_data[j+7+len_temp],2);
										count += 2;
										for(int k =j-1;k<(UINT)len-6;k++)
										{
											check_sum_temp += saved_header_data[k];
										}
										if(check_sum != check_sum_temp)
										{
											return FALSE;
										}
									}
									break;
								case CASE_SD_CARD:
									if(CMD_RESPONSE == commad_type)
									{
										strncpy((char *)&len_temp, &saved_header_data[j+5], 2);
										count += 2;
										if (len_temp > 0)
										{
											strncpy(data, &saved_header_data[j+7], len_temp);

											//str.Format(_T("%s"),data);
											//TestToolDlg.GetDlgItem(IDC_EDIT_WIFI)->SetWindowText(str);
											//return 1;

											count += len_temp;
										}
										strncpy((char *)&check_sum,&saved_header_data[j+7+len_temp],2);
										count += 2;
										for(int k =j-1;k<(UINT)len-6;k++)
										{
											check_sum_temp += saved_header_data[k];
										}
										if(check_sum != check_sum_temp)
										{
											return FALSE;
										}
									}
									break;
								case CASE_MONITOR:
									if(CMD_RESPONSE == commad_type)
									{
										strncpy((char *)&len_temp, &saved_header_data[j+5], 2);
										count += 2;
										if (len_temp > 0)
										{
											strncpy(data, &saved_header_data[j+7], len_temp);
											count += len_temp;
										}
										strncpy((char *)&check_sum,&saved_header_data[j+7+len_temp],2);
										count += 2;
										for(int k =j-1;k<(UINT)len-6;k++)
										{
											check_sum_temp += saved_header_data[k];
										}
										if(check_sum != check_sum_temp)
										{
											return FALSE;
										}
									}
									break;
								case CASE_FOCUS_EFFECT:
									if(CMD_RESPONSE == commad_type)
									{
										strncpy((char *)&len_temp, &saved_header_data[j+5], 2);
										count += 2;
										if (len_temp > 0)
										{
											strncpy(data, &saved_header_data[j+7], len_temp);
											count += len_temp;
										}
										strncpy((char *)&check_sum,&saved_header_data[j+7+len_temp],2);
										count += 2;
										for(int k =j-1;k<(UINT)len-6;k++)
										{
											check_sum_temp += saved_header_data[k];
										}
										if(check_sum != check_sum_temp)
										{
											return FALSE;
										}
									}
									break;
								case CASE_IRCUT:
									if(CMD_RESPONSE == commad_type)
									{
										strncpy((char *)&len_temp, &saved_header_data[j+5], 2);
										count += 2;
										if (len_temp > 0)
										{
											strncpy(data, &saved_header_data[j+7], len_temp);
											count += len_temp;
										}
										strncpy((char *)&check_sum,&saved_header_data[j+7+len_temp],2);
										count += 2;
										for(int k =j-1;k<(UINT)len-6;k++)
										{
											check_sum_temp += saved_header_data[k];
										}
										if(check_sum != check_sum_temp)
										{
											return FALSE;
										}
									}
									break;
								case CASE_RESET:
									if(CMD_RESPONSE == commad_type)
									{
										strncpy((char *)&len_temp, &saved_header_data[j+5], 2);
										count += 2;
										if (len_temp > 0)
										{
											strncpy(data, &saved_header_data[j+7], len_temp);

											//str.Format(_T("%s"),data);
											//TestToolDlg.GetDlgItem(IDC_EDIT_WIFI)->SetWindowText(str);
											//return 1;

											count += len_temp;
										}
										strncpy((char *)&check_sum,&saved_header_data[j+7+len_temp],2);
										count += 2;
										for(int k =j-1;k<(UINT)len-6;k++)
										{
											check_sum_temp += saved_header_data[k];
										}
										if(check_sum != check_sum_temp)
										{
											return FALSE;
										}
									}
									break;
								case CASE_MAC_BURN:
									if(CMD_RESPONSE == commad_type)
									{
										strncpy((char *)&len_temp, &saved_header_data[j+5], 2);
										count += 2;
										if (len_temp > 0)
										{
											strncpy(data, &saved_header_data[j+7], len_temp);
											count += len_temp;
										}
										strncpy((char *)&check_sum,&saved_header_data[j+7+len_temp],2);
										count += 2;
										for(int k =j-1;k<(UINT)len-6;k++)
										{
											check_sum_temp += saved_header_data[k];
										}
										if(check_sum != check_sum_temp)
										{
											return FALSE;
										}
									}
									break;
								case CASE_UID_BURN:
									if(CMD_RESPONSE == commad_type)
									{
										strncpy((char *)&len_temp, &saved_header_data[j+5], 2);
										count += 2;
										if (len_temp > 0)
										{
											strncpy(data, &saved_header_data[j+7], len_temp);
											count += len_temp;
										}
										strncpy((char *)&check_sum,&saved_header_data[j+7+len_temp],2);
										count += 2;
										for(int k =j-1;k<(UINT)len-6;k++)
										{
											check_sum_temp += saved_header_data[k];
										}
										if(check_sum != check_sum_temp)
										{
											return FALSE;
										}
									}
									break;							
								}//switch����
#endif
								if(param_len == (count + 4))
								{
									break;
								}
								else
								{
									return FALSE;
								}
							}//���ݰ�û�а�ͷ�а�β���if(end_data_flag)����
							else
							{
								incomplete_len += len;
								if(incomplete_len > nBufLen)
								{
									return FALSE;
								}
								strcat(saved_header_data,lpBuf);
								break;
							}
						}//���ݰ�û�а�ͷelse����						
					}//if(len > 4)����
					else
					{
						incomplete_datalen += len;
						saved_header_data = lpBuf;
						break;						
					}
				}//for(i = 0;i<(UINT)len;i++)ѭ������
			}//if(len> 0 && (UINT)len<= nBufLen)����
		}//if (m_find_result_handle != INVALID_HANDLE_VALUE)����
		else
		{
			Sleep(2);
		}
	}//whileѭ������
	delete[] lpBuf;
	delete[] saved_header_data;
	delete[] data;
	return TRUE;
}
#else



BOOL CAnykaIPCameraDlg::decode_data_pack(char *buf, UINT buf_len)
{
	UINT ckeck_sum = 0, i = 0, buf_sum = 0, data_len = 0;
	unsigned char temp = 0, test_type = 0, cmd_type = 0;
	CString str;

	test_type = buf[0];
	cmd_type = buf[1];
	memcpy(&data_len, &buf[2], sizeof(UINT));

	if (test_type >= CASE_ATTR_TYPE_NUM)
	{
		AfxMessageBox(_T("��ȡ���Ĳ���ģ�������д�"), MB_OK);	
		return FALSE;
	}

	if (CMD_RESPONSE != cmd_type)
	{
		if (test_type == CASE_SPEAKER)
		{
			str.Format(_T("����ģ�飬��ȡ���������д�"));
		}
		else if (test_type == CASE_CLOUD_DECK)
		{
			str.Format(_T("��̨ģ�飬��ȡ���������д�"));
		}
		else if (test_type == CASE_WIFI_CONNECT)
		{
			str.Format(_T("wifiģ�飬��ȡ���������д�"));
		}
		else if (test_type == CASE_SD_CARD)
		{
			str.Format(_T("sdģ�飬��ȡ���������д�"));
		}
		else if (test_type == CASE_MONITOR)
		{
			str.Format(_T("����ģ�飬��ȡ���������д�"));
		}
		else if (test_type == CASE_INTERPHONE)
		{
			str.Format(_T("�Խ�ģ�飬��ȡ���������д�"));
		}
		else if (test_type == CASE_FOCUS_EFFECT)
		{
			str.Format(_T("��Ƶ����ģ�飬��ȡ���������д�"));
		}
		else if (test_type == CASE_IRCUT)
		{
			str.Format(_T("IRCUTģ�飬��ȡ���������д�"));
		}
		else if (test_type == CASE_RESET)
		{
			str.Format(_T("��λ��ģ�飬��ȡ���������д�"));
		}
		else if (test_type == CASE_MAC_BURN)
		{
			str.Format(_T("MAC��дģ�飬��ȡ���������д�"));
		}
		else if (test_type == CASE_UID_BURN)
		{
			str.Format(_T("UID��дģ�飬��ȡ���������д�"));
		}
		AfxMessageBox(str, MB_OK);
		return FALSE;
	}

	//�ȼ��check_sum
	for (i = 0; i < buf_len - 4; i++)
	{
		temp = buf[i];
		ckeck_sum = ckeck_sum + temp;

	}
	
	memcpy(&buf_sum,  &buf[buf_len - 4], sizeof(UINT));

	//str.Format(_T("buf:%02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x"), 
	//	buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8], buf[9], buf[10], buf[11], buf[12], buf[13], buf[14], buf[15], buf[16], buf[17], buf[18] );
	//AfxMessageBox(str, MB_OK);

	//str.Format(_T("test_type:%d, buf_len:%d, buf_sum:%d, ckeck_sum:%d"), test_type, buf_len, buf_sum, ckeck_sum);
	//AfxMessageBox(str, MB_OK);


	if (buf_sum != ckeck_sum)
	{
		if (test_type == CASE_SPEAKER)
		{
			str.Format(_T("����ģ�飬��ȡcheck sumֵ�д�, src:%d, dst:%d"), ckeck_sum, buf_sum);
		}
		else if (test_type == CASE_CLOUD_DECK)
		{
			str.Format(_T("��̨ģ�飬��ȡcheck sumֵ�д�, src:%d, dst:%d"), ckeck_sum, buf_sum);
		}
		else if (test_type == CASE_WIFI_CONNECT)
		{
			str.Format(_T("wifiģ�飬��ȡcheck sumֵ�д�, src:%d, dst:%d"), ckeck_sum, buf_sum);
		}
		else if (test_type == CASE_SD_CARD)
		{
			str.Format(_T("sdģ�飬��ȡcheck sumֵ�д�, src:%d, dst:%d"), ckeck_sum, buf_sum);
		}
		else if (test_type == CASE_MONITOR)
		{
			str.Format(_T("����ģ�飬��ȡcheck sumֵ�д�, src:%d, dst:%d"), ckeck_sum, buf_sum);
		}
		else if (test_type == CASE_INTERPHONE)
		{
			str.Format(_T("�Խ�ģ�飬��ȡcheck sumֵ�д�, src:%d, dst:%d"), ckeck_sum, buf_sum);
		}
		else if (test_type == CASE_FOCUS_EFFECT)
		{
			str.Format(_T("��Ƶ����ģ�飬��ȡcheck sumֵ�д�, src:%d, dst:%d"), ckeck_sum, buf_sum);
		}
		else if (test_type == CASE_IRCUT)
		{
			str.Format(_T("IRCUTģ�飬��ȡcheck sumֵ�д�, src:%d, dst:%d"), ckeck_sum, buf_sum);
		}
		else if (test_type == CASE_RESET)
		{
			str.Format(_T("��λ��ģ�飬��ȡcheck sumֵ�д�, src:%d, dst:%d"), ckeck_sum, buf_sum);
		}
		else if (test_type == CASE_MAC_BURN)
		{
			str.Format(_T("MAC��дģ�飬��ȡcheck sumֵ�д�, src:%d, dst:%d"), ckeck_sum, buf_sum);
		}
		else if (test_type == CASE_UID_BURN)
		{
			str.Format(_T("UID��дģ�飬��ȡcheck sumֵ�д�, src:%d, dst:%d"), ckeck_sum, buf_sum);
		}
		AfxMessageBox(str, MB_OK);
		return FALSE;
	}

	if (data_len > 0)
	{
		//Data�ĸ�ʽ��
		//����ֵ��1���ֽڣ�+ ���ݳ��ȣ�4���ֽڣ�+����
		//ע�����ֻ�з���ֵ��datalen����1������ʹ���5
		//Ŀǰ�з������ݵ�ֻ��sd��wifi:
		//Sd�Ľṹ��8���ֽڵ�������С(long��)
		//Wifi�Ľṹ��4���ֽڵ�wifi���� +wifi���� (wifi����wifiǿ�ȵĽṹ�� * wifi����)
		
		if (test_type == CASE_SPEAKER)
		{
			g_test_config.m_test_speaker_flag = buf[6];
		}
		else if (test_type == CASE_CLOUD_DECK)
		{
			g_test_config.m_test_cloud_flag = buf[6];
		}
		else if (test_type == CASE_WIFI_CONNECT)
		{
			g_test_config.m_test_wifi_flag = buf[6];
			//��ȡWIFI������
			if (g_test_config.m_test_wifi_flag == 1)
			{
				memcpy(&g_test_config.m_ssid_num, &buf[7], sizeof(UINT));
				memcpy(&g_test_config.m_ssid_info, &buf[11], sizeof(T_SSID_INFO)*g_test_config.m_ssid_num);
			}
		}
		else if (test_type == CASE_SD_CARD)
		{
			g_test_config.m_test_sd_flag = buf[6];
			//��ȡsd��������С
			if (g_test_config.m_test_sd_flag == 1)
			{
				memcpy(&g_test_config.m_sd_size, &buf[7], sizeof(long));
			}
		}
		else if (test_type == CASE_MONITOR)
		{
			g_test_config.m_test_monitor_flag = buf[6];
		}
		else if (test_type == CASE_INTERPHONE)
		{
			g_test_config.m_test_play_flag = buf[6];
		}
		else if (test_type == CASE_FOCUS_EFFECT)
		{
			g_test_config.m_test_focus_flag = buf[6];
		}
		else if (test_type == CASE_IRCUT)
		{
			g_test_config.m_test_ircut_flag = buf[6];
		}
		else if (test_type == CASE_RESET)
		{
			g_test_config.m_test_reset_flag = buf[6];
		}
		else if (test_type == CASE_MAC_BURN)
		{
			g_test_config.m_test_MAC_flag = buf[6];
		}
		else if (test_type == CASE_UID_BURN)
		{
			g_test_config.m_test_UID_flag = buf[6];
		}

	}
	return TRUE;

}

/*

typedef struct
{
char     *head_name ;  //��ͷ��ʶ��VCMD��
enum T_TEST_INFO  test_type;
enum T_CMD_TYPE  cmd_type;
UINT   datalen;//���ݳ���
char*   data;    //����
short  check_sum;
char   *end_name;  //��β��ʶ��VEND��
}T_TEST_DATA;

*/

DWORD WINAPI find_result_main(void *lpParameter)
{
	char commad_type;
	char id_type;
	//char *head_name = "VCMD";
	//char *end_name = "VEND";
	CAnykaIPCameraDlg  *TestToolDlg = (CAnykaIPCameraDlg  *)lpParameter;
	int len = 0;
	char *lpBuf = NULL;
	UINT nBufLen = 1024*1024;
	char *one_pack_buf = NULL;
	UINT i,j;
	BOOL frame_head_flag = FALSE;
	BOOL frame_end_flag = FALSE;
	UINT idex = 0;

	lpBuf = new char[nBufLen + 1];
	if (lpBuf == NULL)
	{
		AfxMessageBox(_T("malloc receive buf fail"), MB_OK);
		return -1;
	}

	one_pack_buf = new char[nBufLen + 1];
	if (one_pack_buf == NULL)
	{
		AfxMessageBox(_T("malloc receive buf fail"), MB_OK);
		return -1;
	}
	//��ѯ���Խ��
	while (1)
	{
		if (g_hBurnThread_rev_data[idex] != INVALID_HANDLE_VALUE)
		{
			commad_type = 0;//��ʼ��
			id_type = 0;


			if (!g_connet_flag)
			{
				break;
			}

			memset(lpBuf,0,nBufLen + 1);
			memset(one_pack_buf,0,nBufLen + 1);

			//��������
			len = TestToolDlg->m_ClientSocket.Socket_Receive(lpBuf, nBufLen, idex);
			if (len == -1)
			{
				Sleep(100);
				continue;
			}

			//����Ƿ�һ�������İ�
			if(len> 0 && (UINT)len<= nBufLen)
			{
				for(i = 0; i<(UINT)len;)
				{
					if (!g_connet_flag)
					{
						break;
					}

					if(lpBuf[i] == 'V' && lpBuf[i + 1] == 'C' && lpBuf[i + 2] == 'M' && lpBuf[i + 3] == 'D')
					{
						//���������ȡ������֡ͷ��־����ô��ǰһ֡��������
						frame_head_flag = TRUE;
						i = i + 4;
						idex = 0;
					}

					if(lpBuf[i] == 'V' && lpBuf[i + 1] == 'E' && lpBuf[i + 2] == 'N' && lpBuf[i + 3] == 'D')
					{
						frame_end_flag = TRUE;
						i = i + 4;
					}
					
					if (frame_head_flag && !frame_end_flag)
					{
						one_pack_buf[idex] = lpBuf[i];
						i++;
						idex++;
					}

					//�����һ�����������Ϳ��Խ������������հ���
					if (frame_head_flag && frame_end_flag)
					{
						TestToolDlg->decode_data_pack(one_pack_buf, idex);
						frame_head_flag = FALSE;
						frame_end_flag = FALSE;
						memset(one_pack_buf,0,nBufLen + 1);
						idex = 0;
					}

				}
			}
		}
		else
		{
			break;
		}
	}

	delete[] lpBuf;
	delete[] one_pack_buf;
}
#endif

BOOL CAnykaIPCameraDlg::Creat_find_result_thread(UINT idex)
{
	close_find_result_thread(idex);

	g_hBurnThread_rev_data[idex] = CreateThread(NULL, 0, find_result_main, this, 0, NULL);
	if (g_hBurnThread_rev_data[idex] == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("������ѯ�����߳�ʧ��,����"), MB_OK);
		return FALSE;
	}
	return TRUE;

}

void CAnykaIPCameraDlg::close_find_result_thread(UINT idex) 
{
	if(g_hBurnThread_rev_data[idex] != INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_hBurnThread_rev_data[idex]);
		g_hBurnThread_rev_data[idex] = INVALID_HANDLE_VALUE;
	}

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

	memcpy(&idex, buf_temp, 1);

	//��ȡ��������
	while (1)
	{
		Sleep(1000);
		if (g_hBurnThread_rev_data[idex] != INVALID_HANDLE_VALUE)
		{
			if (g_send_commad[idex] == 1)
			{
				commad_type = 0;  //��ʼ��
				//time1 = GetTickCount();
				memset(lpBuf,0,256);
				ret = m_ClientSocket_heat.Socket_Receive(lpBuf, nBufLen, idex);
				//frmLogfile_temp.WriteLogFile(0,"Socket_Receive ret:%d\n", ret);
				if (ret == -1)
				{
#if 0
					if (g_hBurnThread_rev_data== INVALID_HANDLE_VALUE && g_send_commad[idex] == 0)
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
					g_send_commad[idex]  = 0;
					return FALSE;
#endif
					continue;
				}
				//frmLogfile.WriteLogFile(0,"ccccc commad_type:%d\n", commad_type);
				//lpBuf�ṹ�ǰ�T_NET_INFO�ṹ�ŷŵ�
				strncpy(&commad_type, &lpBuf[2], 1);
				if(commad_type == TEST_RESPONSE)
				{
					//frmLogfile.WriteLogFile(0,"aaaaa commad_type:%d\n", commad_type);
					memset(g_param, 0, 256);

					if (!TestToolDlg.decode_command(lpBuf, &g_commad_type, NULL, g_param))
					{
						g_test_pass_flag[idex] = 2;	
					}
					else
					{
						if (g_param[0] == 49)  //49 ��ʾ1
						{
							g_test_pass_flag[idex] = 1;
						}
						else
						{
							g_test_pass_flag[idex] = 2;
						}
					}
					//frmLogfile.WriteLogFile(0,"bbbbb commad_type:%d\n", commad_type);
				}
			}
		}
	}

	return 1;
}


BOOL CAnykaIPCameraDlg::create_thread_rev_data(UINT idex) 
{


	if (g_hBurnThread_rev_data[idex] != INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_hBurnThread_rev_data[idex]);
		g_hBurnThread_rev_data[idex] = INVALID_HANDLE_VALUE;
	}

	//param = &idex;
	memcpy(rve_param[idex], &idex, sizeof(UINT));
	g_hBurnThread_rev_data[idex] = CreateThread(NULL, 0, check_rev_date_thread, rve_param[idex], 0, NULL);
	if (g_hBurnThread_rev_data[idex] == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	return TRUE;
}

void CAnykaIPCameraDlg::close_thread_rev_data(UINT idex) 
{
	if(g_hBurnThread_rev_data[idex] != INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_hBurnThread_rev_data[idex]);
		g_hBurnThread_rev_data[idex] = INVALID_HANDLE_VALUE;
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
	//char g_param[256] = {0};
	UINT idex = 0;
	UINT *buf_temp = (UINT *)lpParameter;

	memcpy(&idex, buf_temp, 1);

	time1 = GetTickCount();
	time2 = GetTickCount();
	//��ȡ��������
	while (1)
	{
		Sleep(4000);
		if (g_heatThread != INVALID_HANDLE_VALUE)
		{
			commad_type = 0;  //��ʼ��
			ret = m_ClientSocket_heat.Heat_Socket_Receive(lpBuf, nBufLen, idex);
		}
	}


	return 1;
}


BOOL CAnykaIPCameraDlg::create_thread_heat(LPCTSTR addr, UINT idex) 
{

	USES_CONVERSION;


	//����һ�������̵߳�socket
	if (!m_ClientSocket.Heat_Socket_Create(idex))
	{
		//AfxMessageBox(_T("��������sokectʧ��"), MB_OK);
		return FALSE;
	}


	//if(m_ClientSocket.Connect(addr, 6789))
	if(!m_ClientSocket.Heat_Socket_Connect(T2A(addr), m_net_uPort + 1, idex))	
	{
		//AfxMessageBox(_T("sokect����ʧ��"), MB_OK);
		return FALSE;
	}

	if (g_heatThread[idex] != INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_heatThread[idex]);
		g_heatThread[idex] = INVALID_HANDLE_VALUE;
	}
	
	memcpy(heat_param[idex], &idex, sizeof(UINT));
	g_heatThread[idex] = CreateThread(NULL, 0, check_heartbeat_thread, heat_param[idex], 0, NULL);
	if (g_heatThread[idex] == INVALID_HANDLE_VALUE)
	{
		//AfxMessageBox(_T("���������߳�ʧ��"), MB_OK);
		return FALSE;
	}

	return TRUE;
}

void CAnykaIPCameraDlg::close_thread_heat(UINT idex) 
{
	if(g_heatThread[idex] != INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_heatThread[idex]);
		g_heatThread[idex] = INVALID_HANDLE_VALUE;
	}
}


BOOL CAnykaIPCameraDlg::Send_cmd(char commad_type, BOOL auto_test_flag, char *file_name, char *param, UINT param_len, UINT idex)
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
	CString str;


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
		//for (i = 0; param[i] != 0; i++)
		{
			if(param_len != 0)
			{
				no_data_flag = FALSE;
				//break;
			}
		}
	}

	if (param != NULL)// && !no_data_flag)
	{
		len_data = param_len; //strlen(param);
		if(len_data != 0 )
		{
			trance.data_param = (char *)malloc(len_data + 2+1);
			if (trance.data_param == NULL)
			{
				return FALSE;
			}
			memset(trance.data_param, 0, len_data + 2 + 1);

			strncpy(trance.data_param, (char *)&len_data, 2);
			//strncpy(&trance.data_param[2], param, len_data);

			for (i = 0; i < (UINT)(len_data); i++)
			{
				trance.data_param[i+2] = param[i];

			}
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


	lpBuf = ( char *)malloc(nBufLen + 1);
	if (lpBuf == NULL)
	{
		return FALSE;
	}

	//���
	strncpy((char *)lpBuf, ( char *)&trance.len, 2);
	strncpy(&lpBuf[2], &trance.commad_type, 1);
	strncpy(&lpBuf[3], ( char  *)&trance.auto_test_flag, 1);
	if (len_name != 0 && file_name != NULL && !no_filename_flag)
	{
		strncpy(&lpBuf[4], ( char *)&len_name, 2);
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
		for (i = 0; i < len_data; i++)
		{
			lpBuf[4+len_name+2 + i] = param[i];
		}
		//strncpy(&lpBuf[4+len_name+2], param, len_data);
		//strncpy(&lpBuf[5+len_name+4], trance.data_param, len_data+4);
	}
	else
	{
		strncpy(&lpBuf[4 + len_name], temp, 2);
	}
	len_data = len_data + 2;  //��Ϊ����4���ֽڵĳ���


	strncpy(&lpBuf[4+len_name+len_data], (char *)&trance.check_sum, 2);


//	str.Format(_T("%02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x ")
//		,lpBuf[0],  lpBuf[1],lpBuf[2],lpBuf[3],lpBuf[4],lpBuf[5],lpBuf[6],lpBuf[7],lpBuf[8],lpBuf[9],lpBuf[10],lpBuf[11],lpBuf[12],lpBuf[13],lpBuf[14],lpBuf[15]);
//	AfxMessageBox(str, MB_OK);

	for(i = 0; i < 5; i++)
	{
		ret = m_ClientSocket.Socket_Send((char *)lpBuf, nBufLen, idex);
		if(!ret)
		{
			Sleep(100);
			continue;
		}
		else
		{
			break;
		}
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
	g_send_commad[0]  = 1;

	memset(test_Name, 0,  MAX_PATH);
	_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
	_tcscat(test_Name, _T("//"));
	_tcscat(test_Name, file_name);

	//�ж��ļ��Ƿ����
	if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(test_Name)))
	{
		g_send_commad[0] = 0;
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
		if (!m_pFtpConnection[0]->PutFile(strSourceName, strDestName, FTP_TRANSFER_TYPE_BINARY, 1))   
		{
			AfxMessageBox(_T("Error no auto test putting file,������С��"), MB_OK);  
			download_dev_file_flag = FALSE;
			g_send_commad[0] = 0;
			return;
		}
		download_dev_file_flag = TRUE;
	}

	//��������
	name_len = strlen(T2A(file_name));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(file_name), name_len);

	g_test_pass_flag[0] = 0;
	if (!Send_cmd(TEST_COMMAND, 1, name_buf, NULL, 0, 0))
	{
		AfxMessageBox(_T("Send_cmd��fail "), MB_OK); 
		g_send_commad[0] = 0;
		return ;
	}

	//���շ���ֵ
	if (!Anyka_Test_check_info(10000))
	{
		AfxMessageBox(_T("����ʧ��"), MB_OK);
		g_send_commad[0] = 0;
		return ;
	}
	g_test_pass_flag[0] = 0;

	if (!Send_cmd(TEST_COMMAND_FINISH, 1, NULL, NULL, 0, 0))
	{
		AfxMessageBox(_T("���³ɹ�, С���Զ�����ʧ�ܣ����ֶ�����"), MB_OK);  
	}
	else
	{
		if (!Anyka_Test_check_info(10000))
		{
			AfxMessageBox(_T("���³ɹ�, С���Զ�����ʧ�ܣ����ֶ�����"), MB_OK);  
		}
		else
		{
			MessageBox(_T("���óɹ�"), MB_OK);
		}
	}
	g_send_commad[0] = 0;
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

		if (g_test_config.need_tool_printf)
		{
			creat_server_thread();
		}
		else
		{
			Close_server_thread();
		}
	}
}


void CAnykaIPCameraDlg::On_find_ip_update()
{
	CServerSearch search;
	UINT i = 0;

	if (!g_finish_flag)
	{	
		//g_sousuo_flag = TRUE;
		m_ip_address_idex = 0;
		m_find_IP_end_flag = FALSE;

		for (i = 0; i < MAX_PATH; i++)
		{
			memset(g_test_config.m_current_config[i].Current_IP_UID, 0, MAC_ADDRESS_LEN);
			memset(g_test_config.m_current_config[i].Current_IP_address_buffer, 0, IP_ADDRE_LEN);
			memset(g_test_config.m_current_config[i].Current_IP_diver_name, 0, MAX_PATH);
			memset(g_test_config.m_current_config[i].Current_IP_version, 0, MAC_ADDRESS_LEN);
		}
	}

	search.Broadcast_Search();

}


void CAnykaIPCameraDlg::On_find_ip()
{
	CServerSearch search;
	UINT i = 0;

	//g_sousuo_flag = TRUE;
	g_finish_find_flag = FALSE;
	g_test_finish_endrtsp_flag = false;
	g_start_open_flag = TRUE;
	g_update_all_finish_flag = FALSE;
	//g_sousuo_flag = FALSE;
	m_ip_address_idex = 0;
	m_find_IP_end_flag = FALSE;

	for (i = 0; i < MAX_PATH; i++)
	{
		memset(g_test_config.m_current_config[i].Current_IP_UID, 0, MAC_ADDRESS_LEN);
		memset(g_test_config.m_current_config[i].Current_IP_address_buffer, 0, IP_ADDRE_LEN);
		memset(g_test_config.m_current_config[i].Current_IP_diver_name, 0, MAX_PATH);
		memset(g_test_config.m_current_config[i].Current_IP_version, 0, MAC_ADDRESS_LEN);
	}
	memset(m_connect_ip, 0, MAX_PATH);

	search.Broadcast_Search();

}

void CAnykaIPCameraDlg::OnBnClickedButtonStart()
{
	UINT i = 0;
	TCHAR *sd_file_name = _T("test_mmc");
	TCHAR *wifi_file_name = _T("test_wifi");
	TCHAR *monitor_file_name = _T("test_monitor");
	

	// TODO: �ڴ���ӿؼ�֪ͨ����������

	// TODO: Add your control notification handler code here


	g_start_success_flag = FALSE;
	if (check_ip(m_connect_ip) && m_connect_ip[0] == 0 
		&& m_connect_ip[1] == 0 && m_connect_ip[2] == 0
		&& m_connect_ip[3] == 0 && m_connect_ip[4] == 0
		&& m_connect_ip[5] == 0 && m_connect_ip[6] == 0
		&& m_connect_ip[7] == 0 && m_connect_ip[8] == 0)
	{
		next_test_flag = TRUE;
		start_flag  = FALSE;
		AfxMessageBox(_T("��ѡ��һ��IP��ַ"), MB_OK);
		return ;
	}

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
		m_BottomDlg.GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(TRUE);//����
		//m_BottomDlg.GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(FALSE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
		//m_BottomDlg.GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(FALSE);//����
		start_flag  = FALSE;
		next_test_flag = TRUE;
		return;
	}

	frmLogfile.WriteLogFile(0,"*********************************************************************************\r\n");
	frmLogfile.WriteLogFile(LOG_LINE_TIME | LOG_LINE_DATE,"Open ipc_test.exe\r\n");

	//m_RightDlg.m_test_wifi_list.DeleteAllItems();
	m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->ShowWindow(SW_SHOW);
	//m_RightDlg.UpdateWindow();

	//m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);//����
	m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("ͨ��(Enter)"));//���� 
	m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->SetWindowText(_T("��ͨ��(�ո�)"));//����
	m_RightDlg.m_Status.SetFontColor(0);
	m_RightDlg.m_test_Status.SetFontColor(0);
	m_RightDlg.m_test_2_Status.SetFontColor(0);
	m_BottomDlg.GetDlgItem(IDC_BUTTON_CONFIGURE)->EnableWindow(FALSE);//����

	if(0)//g_test_config.config_lan_mac_enable==FALSE&&g_test_config.config_uid_enable==FALSE)
	{
		g_test_config.config_lan_mac_enable=config_lan_mac_enable_temp;
		g_test_config.config_uid_enable=config_uid_enable_temp;
	}
	
	//�������С����̨ת���߳�ֹͣ
	//Send_cmd(TEST_ENABLE_KEY, 1, "test_ptz_cmd", "9");
	//Auto_move_disconnect(0);

	//Sleep(2000);
	
	if (!g_connet_flag)
	{
		if (!ConnetServer(m_connect_ip, 0))
		{
			g_connet_flag = FALSE;
			AfxMessageBox(_T("��������ʧ�ܣ�����"), MB_OK);
			m_BottomDlg.GetDlgItem(IDC_BUTTON_CONFIGURE)->EnableWindow(TRUE);//����
			m_BottomDlg.GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(TRUE);//����
			//m_BottomDlg.GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(FALSE);//����
			m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//����
			m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����
			m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
			//m_BottomDlg.GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(FALSE);//����
			//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(FALSE);//����
			start_flag  = FALSE;
			CloseServer(0);
			return ;
		}
	}
	
	//����sd��⹦��
	//if (!case_sd())
	if (0)//!g_sd_test_success)
	{
		g_connet_flag = FALSE;
		AfxMessageBox(_T("sd���ʧ��"), MB_OK);
		m_BottomDlg.GetDlgItem(IDC_BUTTON_CONFIGURE)->EnableWindow(TRUE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(FALSE);//����
		//m_BottomDlg.GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(FALSE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
		//m_BottomDlg.GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(FALSE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(FALSE);//����
		start_flag  = FALSE;
		CloseServer(0);
		return ;
	}

	g_connet_flag = TRUE;

	if(g_test_config.config_sd_enable)
	{
		
		//����sd�ļ���wifi�ļ�
		if(0)//if(!download_file(sd_file_name))
		//if(!download_file_thread(sd_file_name))
		{
			g_connet_flag = FALSE;
			AfxMessageBox(_T("����sd���Գ���ʧ��"), MB_OK);
			m_BottomDlg.GetDlgItem(IDC_BUTTON_CONFIGURE)->EnableWindow(TRUE);//����
			m_BottomDlg.GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(FALSE);//����
			m_BottomDlg.GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(FALSE);//����
			m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//����
			m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����
			m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
			//m_BottomDlg.GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(FALSE);//����
			m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(FALSE);//����
			start_flag  = FALSE;
			CloseServer(0);
			return ;
		}
	}

	//Sleep(500);
	if(g_test_config.config_wifi_enable)
	{
		if(0)//if(!download_file(wifi_file_name))
		//if(!download_file_thread(wifi_file_name))
		{
			g_connet_flag = FALSE;
			AfxMessageBox(_T("����wifi���Գ���ʧ��"), MB_OK);
			m_BottomDlg.GetDlgItem(IDC_BUTTON_CONFIGURE)->EnableWindow(TRUE);//����
			m_BottomDlg.GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(FALSE);//����
			m_BottomDlg.GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(FALSE);//����
			m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//����
			m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����
			m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
			//m_BottomDlg.GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(FALSE);//����
			m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(FALSE);//����
			start_flag  = FALSE;
			CloseServer(0);
			return ;
		}
		
	}

	if (g_test_config.config_voice_rev_enable)
	{
		if(0)//if(!download_file(monitor_file_name))
			//if(!download_file_thread(wifi_file_name))
		{
			g_connet_flag = FALSE;
			AfxMessageBox(_T("���ؼ������Գ���ʧ��"), MB_OK);
			m_BottomDlg.GetDlgItem(IDC_BUTTON_CONFIGURE)->EnableWindow(TRUE);//����
			m_BottomDlg.GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(FALSE);//����
			m_BottomDlg.GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(FALSE);//����
			m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//����
			m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����
			m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
			//m_BottomDlg.GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(FALSE);//����
			m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(FALSE);//����
			start_flag  = FALSE;
			CloseServer(0);
			return ;
		}
	}


	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T(""));//���� 
	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT2)->SetWindowText(_T(""));

	g_connet_flag = TRUE;
	if (The_first_case())
	{
		g_finish_find_flag = FALSE;
		m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(FALSE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(FALSE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_FIND_IP)->EnableWindow(FALSE);//����
		m_RightDlg.GetDlgItem(IDC_LIST1)->EnableWindow(FALSE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_WRITE_UID)->EnableWindow(FALSE);//����

		//m_BottomDlg.GetDlgItem(IDC_BUTTON_AUTO_MOVE)->EnableWindow(TRUE);//����	
		//m_BottomDlg.GetDlgItem(IDC_BUTTON_UP)->EnableWindow(TRUE);//����	
		//m_BottomDlg.GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow(TRUE);//����	
		//m_BottomDlg.GetDlgItem(IDC_BUTTON_LEFT)->EnableWindow(TRUE);//����	
		//m_BottomDlg.GetDlgItem(IDC_BUTTON_RIGHT)->EnableWindow(TRUE);//����	

		m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->SetWindowText(_T("��������"));

		
		if (g_test_config.config_reset_enable && g_case_idex == CASE_RESET)
		{
		}
		else
		{
			if (!g_reset_test_pass && g_test_config.config_reset_enable && g_case_idex == CASE_RESET)
			{
				g_reset_test_pass = TRUE;
				m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//����
				m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->ShowWindow(SW_HIDE);//����
			}
			else if (!g_sd_test_pass && g_test_config.config_sd_enable && g_case_idex == CASE_SD)
			{
				g_sd_test_pass = TRUE;
				m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//����
				m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->ShowWindow(SW_HIDE);//����
			}
			else if (!g_wifi_test_pass && g_test_config.config_wifi_enable && g_case_idex == CASE_WIFI)
			{
				g_wifi_test_pass = TRUE;
				m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//����
				m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->ShowWindow(SW_HIDE);//����
			}
			else
			{
				m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//����
				m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->ShowWindow(SW_SHOW);//����

			}

			m_BottomDlg.GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(TRUE);//����
			m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
			m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->ShowWindow(SW_SHOW);//����
			//m_BottomDlg.GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(TRUE);//����
			
		}
		g_start_success_flag = TRUE; 
	}
	else
	{
		g_connet_flag = FALSE;
		m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_CONFIGURE)->EnableWindow(TRUE);//����
		CloseServer(0);
		start_flag  = FALSE;
		g_start_success_flag = FALSE; 

	}
	g_test_config.config_video_test_pass = FALSE;
	//g_test_config.config_voice_rev_test_pass = FALSE;  //����
	g_test_config.config_voice_send_test_pass = FALSE; //�Խ�
	g_test_config.config_head_test_pass = FALSE;
	//g_test_config.config_sd_test_pass = FALSE;
	//g_test_config.config_wifi_test_pass = FALSE;
	g_test_config.config_red_line_test_pass = FALSE;
	g_test_config.config_reset_test_pass = FALSE;
	g_test_config.config_lan_mac_test_pass = FALSE;
	g_test_config.config_uid_test_pass = FALSE;
	g_test_config.config_ircut_on_test_pass = FALSE;
	g_test_config.config_ircut_off_test_pass = FALSE;
	g_test_config.config_ircut_onoff_test_pass = FALSE;
	
	start_test  = TRUE;
	g_config_start_flag = FALSE;

	if (g_test_config.config_reset_enable && g_case_idex == CASE_RESET)
	{
	}
	else
	{
		m_RightDlg.m_test_pass_btn.SetFocus();
	}

	
	return;

}


void CAnykaIPCameraDlg::show_IP_info()
{
	UINT i = 0, len = 0, num = 0;
	CString str;

	if (m_RightDlg.m_test_config.GetSelectedCount() < 1)
	{
		return;
	}
	num = m_RightDlg.m_test_config.GetItemCount();
#if 0
	for (i =0; i < num; i++)
	{
		BOOL ret = (BOOL)m_RightDlg.m_test_config.GetCheck(i);

		if (ret == TRUE)
		{
			g_update_flag[i] = 1;
		}
		else
		{
			g_update_flag[i] = 0;
		}
	}
#endif 

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
				memset(m_connect_ip, 0, MAX_PATH);
				_tcsncpy(m_connect_ip, g_test_config.m_current_config[i].Current_IP_address_buffer, len);
			}*/


			if (_tcscmp(m_connect_ip, g_test_config.m_last_config[i].Current_IP_address_buffer) != 0)
			{
				current_ip_idex = i;
				g_start_open_flag = TRUE;	
				len = _tcsclen(g_test_config.m_last_config[i].Current_IP_address_buffer);
				memset(m_connect_ip, 0, MAX_PATH);
				_tcsncpy(m_connect_ip, g_test_config.m_last_config[i].Current_IP_address_buffer, len);
				memset(m_connect_uid, 0, MAC_ADDRESS_LEN);
				len = _tcsclen(g_test_config.m_last_config[current_ip_idex].Current_IP_UID);
				_tcsncpy(m_connect_uid, g_test_config.m_last_config[current_ip_idex].Current_IP_UID, len);
				find_IP_CloseServer(0);
				g_test_monitor_flag = TRUE;

				g_test_config.m_test_sd_flag = 0; //sd, 0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��
				g_test_config.m_test_wifi_flag = 0; //wifi,0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��
				g_test_config.m_test_reset_flag = 0; //0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��
				g_test_config.m_test_ircut_flag = 0; //0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��
				g_test_config.m_test_MAC_flag = 0; //0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��
				g_test_config.m_test_UID_flag = 0; //0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��
				g_test_config.m_test_speaker_flag = 0; //0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��
				g_test_config.m_test_monitor_flag = 0; //������0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��
				g_test_config.m_test_cloud_flag = 0; //��̨�� 0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��
				g_test_config.m_test_play_flag = 0; //�Խ���0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��

			}

			if(g_test_finish_endrtsp_flag)
			{
				g_start_open_flag = TRUE;	
				g_test_finish_endrtsp_flag = false;
				current_ip_idex = i;
				memset(m_connect_ip, 0, MAX_PATH);
				len = _tcsclen(g_test_config.m_last_config[i].Current_IP_address_buffer);
				_tcsncpy(m_connect_ip, g_test_config.m_last_config[i].Current_IP_address_buffer, len);
				memset(m_connect_uid, 0, MAC_ADDRESS_LEN);
				len = _tcsclen(g_test_config.m_last_config[current_ip_idex].Current_IP_UID);
				_tcsncpy(m_connect_uid, g_test_config.m_last_config[current_ip_idex].Current_IP_UID, len);
				find_IP_CloseServer(0);
				g_test_monitor_flag = TRUE;
				g_test_config.m_test_sd_flag = 0; //sd, 0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��
				g_test_config.m_test_wifi_flag = 0; //wifi,0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��
				g_test_config.m_test_reset_flag = 0; //0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��
				g_test_config.m_test_ircut_flag = 0; //0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��
				g_test_config.m_test_MAC_flag = 0; //0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��
				g_test_config.m_test_UID_flag = 0; //0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��
				g_test_config.m_test_speaker_flag = 0; //0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��
				g_test_config.m_test_monitor_flag = 0; //������0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��
				g_test_config.m_test_cloud_flag = 0; //��̨�� 0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��
				g_test_config.m_test_play_flag = 0; //�Խ���0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��

			}
			next_test_flag = TRUE;
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
	//char param_buf[2] = {0};
	CString str;
	TCHAR *file_name = _T("test_close");

	USES_CONVERSION;

#if 0
	if (!g_connet_flag)
	{
		CloseServer(0);
		m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(FALSE);//����
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
	g_send_commad[0] = 1;

	m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(FALSE);//����
	m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//���� 
	m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����

	memset(test_Name, 0,  MAX_PATH);
	_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
	_tcscat(test_Name, _T("/"));
	_tcscat(test_Name, file_name);

	//�ж��ļ��Ƿ����
	if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(test_Name)))
	{
		CloseServer(0);
		g_send_commad[0] = 0;
		m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
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


	if (m_pFtpConnection[0] == NULL || !m_pFtpConnection[0]->PutFile(strSourceName, strDestName, FTP_TRANSFER_TYPE_BINARY, 1))   
	{
		CloseServer(0);
		m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
		AfxMessageBox(_T("�رչ��� PutFile fail"), MB_OK);  
		download_dev_file_flag = FALSE;
		g_send_commad[0] = 0;
		return;
	}

	//��������
	name_len = strlen(T2A(file_name));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(file_name), name_len);

	g_test_pass_flag[0] = 0;
	if (!Send_cmd(TEST_COMMAND, 1, name_buf, NULL, 0))
	{
		CloseServer(0);
		m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
		AfxMessageBox(_T("�رչ��� Send_cmd fail "), MB_OK); 
		g_send_commad[0] = 0;
		return ;
	}

	//���շ���ֵ
	if (!Anyka_Test_check_info(10000))
	{
		CloseServer(0);
		m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
		AfxMessageBox(_T("�ر�ʧ��"), MB_OK);
		g_send_commad[0] = 0;
		return ;
	}
#endif
	
	//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
	//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
	//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
	g_test_pass_flag[0] = 0;
	g_send_commad[0] = 0;

	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CloseServer(0);
	//GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(FALSE);//����
	m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(FALSE);//����
	//m_BottomDlg.GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(FALSE);//����
	m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//����
	m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����
	m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
	//m_BottomDlg.GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(FALSE);//����
	m_BottomDlg.GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(TRUE);//����
	start_test = FALSE;
	start_flag = FALSE;

	MessageBox(_T("�رճɹ�"), MB_OK);
}

void CAnykaIPCameraDlg::OnBnClickedButtonWriteMac()
{
	UINT i = 0,  len = 0;

	if (check_ip(m_connect_ip) && m_connect_ip[0] == 0 
		&& m_connect_ip[1] == 0 && m_connect_ip[2] == 0
		&& m_connect_ip[3] == 0 && m_connect_ip[4] == 0
		&& m_connect_ip[5] == 0 && m_connect_ip[6] == 0
		&& m_connect_ip[7] == 0 && m_connect_ip[8] == 0)
	{
		start_flag  = FALSE;
		AfxMessageBox(_T("��ѡ��һ��IP��ַ"), MB_OK);
		return ;
	}

	if(!g_test_config.config_lan_mac_enable)
	{
		AfxMessageBox(_T("���ڹ����������й�ѡLan MAC"), MB_OK);
		return;
	}
	

	m_BottomDlg.GetDlgItem(IDC_BUTTON_WRITE_MAC)->EnableWindow(FALSE);
	
	
	if (!case_mac())
	{
		//frmLogfile.WriteLogFile(0,"MAC��¼ʧ��,MAC:%s\r\n", T2A(burnUid));
		//frmLogUidFile.WriteLogFile(0,"UID:%s, NG\r\n", T2A(burnUid));
		//CloseServer(0);
		
		m_BottomDlg.GetDlgItem(IDC_BUTTON_WRITE_MAC)->EnableWindow(TRUE);
		
		return;
	}
		

	m_BottomDlg.GetDlgItem(IDC_BUTTON_WRITE_MAC)->EnableWindow(TRUE);
}

void CAnykaIPCameraDlg::OnBnClickedButtonWriteUid()
{
	UINT i = 0,  len = 0;

	if (check_ip(m_connect_ip) && m_connect_ip[0] == 0 
		&& m_connect_ip[1] == 0 && m_connect_ip[2] == 0
		&& m_connect_ip[3] == 0 && m_connect_ip[4] == 0
		&& m_connect_ip[5] == 0 && m_connect_ip[6] == 0
		&& m_connect_ip[7] == 0 && m_connect_ip[8] == 0)
	{
		start_flag  = FALSE;
		AfxMessageBox(_T("��ѡ��һ��IP��ַ"), MB_OK);
		return ;
	}
	
	if(!g_test_config.config_uid_enable)
	{
		AfxMessageBox(_T("���ڹ����������й�ѡUID��¼"),MB_OK);
		return;
	}

	len = _tcslen(g_test_config.m_last_config[current_ip_idex].Current_IP_UID);
	for (i = 0; i < len; i++)
	{
		if (g_test_config.m_last_config[current_ip_idex].Current_IP_UID[i] != 0)
		{
			break;
		}
	}
	if ( i != len)
	{
		if (IDOK != AfxMessageBox(_T("���豸�Ѵ�����UID���Ƿ�ȷ�ϼ���д��uid��"), MB_OKCANCEL))
		{
			return;
		}
		
	}

	m_BottomDlg.GetDlgItem(IDC_BUTTON_WRITE_UID)->EnableWindow(FALSE);//����
	//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(FALSE);//����
	//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//����
	//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����
	//m_BottomDlg.GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(FALSE);//����
	//m_RightDlg.GetDlgItem(IDC_BUTTON_FIND_IP)->EnableWindow(FALSE);//����
	//m_RightDlg.GetDlgItem(IDC_LIST1)->EnableWindow(FALSE);//����
	//m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);//����

	g_finish_find_flag = FALSE;

	// TODO: �ڴ���ӿؼ�֪����������
	if(IDOK == m_Burn_UIdDlg.DoModal() && m_Burn_UIdDlg.UID_PASS_flag)
	{
		TCHAR burnUid[MAX_PATH+1] = {0};
		UINT len = 0;

		memset(burnUid, 0, MAX_PATH);
		len = _tcslen(g_test_config.m_uid_number);
		_tcsncpy(burnUid, &g_test_config.m_uid_number[len - g_test_config.uid_download_len], g_test_config.uid_download_len);

		USES_CONVERSION;


		if (!g_connet_flag)
		{
			//if (!ConnetServer(m_connect_ip, 0))
			{
				AfxMessageBox(_T("��������ʧ�ܣ�����"), MB_OK);
				//g_connet_flag = FALSE;
				//CloseServer(0);
				//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(FALSE);//����
				//m_BottomDlg.GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(TRUE);//����
				//m_RightDlg.GetDlgItem(IDC_BUTTON_FIND_IP)->EnableWindow(TRUE);//����
				//m_RightDlg.GetDlgItem(IDC_LIST1)->EnableWindow(TRUE);//����
				//m_BottomDlg.GetDlgItem(IDC_BUTTON_CONFIGURE)->EnableWindow(TRUE);//����
				m_BottomDlg.GetDlgItem(IDC_BUTTON_WRITE_UID)->EnableWindow(TRUE);//����
				//m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
				//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);;//���� 
				//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);;//����
				return ;
			}
			//g_connet_flag = TRUE;
		}
		if (!case_uid())
		{
			frmLogfile.WriteLogFile(0,"UID��¼ʧ��,UID:%s\r\n", T2A(burnUid));
			//frmLogUidFile.WriteLogFile(0,"UID:%s, NG\r\n", T2A(burnUid));
			//CloseServer(0);
			//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(FALSE);//����
			//m_BottomDlg.GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(TRUE);//����
			//m_RightDlg.GetDlgItem(IDC_BUTTON_FIND_IP)->EnableWindow(TRUE);//����
			//m_RightDlg.GetDlgItem(IDC_LIST1)->EnableWindow(TRUE);//����
			//m_BottomDlg.GetDlgItem(IDC_BUTTON_CONFIGURE)->EnableWindow(TRUE);//����
			m_BottomDlg.GetDlgItem(IDC_BUTTON_WRITE_UID)->EnableWindow(TRUE);//����
			//m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
			//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);;//���� 
			//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);;//����
			return;
		}
		
		//CloseServer(0);

		
		frmLogfile.WriteLogFile(0,"UID��¼�ɹ�,UID:%s\r\n", T2A(burnUid));
		//frmLogUidFile.WriteLogFile(0,"UID:%s, OK\r\n", T2A(burnUid));
	}
	//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(FALSE);//����
	//m_BottomDlg.GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(TRUE);//����
	//m_RightDlg.GetDlgItem(IDC_BUTTON_FIND_IP)->EnableWindow(TRUE);//����
	//m_RightDlg.GetDlgItem(IDC_LIST1)->EnableWindow(TRUE);//����
	//m_BottomDlg.GetDlgItem(IDC_BUTTON_CONFIGURE)->EnableWindow(TRUE);//����
	m_BottomDlg.GetDlgItem(IDC_BUTTON_WRITE_UID)->EnableWindow(TRUE);//����
	//m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
	//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);;//���� 
	//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);;//����

}


BOOL CAnykaIPCameraDlg::case_ircut_onoff_test(void)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strSourceName, strDestName;
	TCHAR test_Name[MAX_PATH] = {0};
	TCHAR DestName_temp[MAX_PATH] = {0};
	TCHAR DestName[50] =_T("/tmp/");
	UINT name_len = 0;
	char name_buf[MAX_PATH] = {0};
	char param_buf[10] = {0};
	CString str;
	TCHAR *file_name = _T("test_ircut_on_off");

	USES_CONVERSION;

	if (!g_connet_flag)
	{
		AfxMessageBox(_T("����û�������ϣ�����"), MB_OK);
		return FALSE;
	}


	memset(test_Name, 0,  MAX_PATH);
	_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
	_tcscat(test_Name, _T("/"));
	_tcscat(test_Name, file_name);

	//�ж��ļ��Ƿ����
	if(0)//if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(test_Name)))
	{
		str.Format(_T("%s no exist,����"), test_Name);
		AfxMessageBox(str, MB_OK); 
		return FALSE;
	}

	//�����ļ�
	memset(DestName_temp, 0, MAX_PATH);
	_tcsncpy(DestName_temp, DestName, sizeof(DestName));
	strSourceName.Format(_T("%s"), ConvertAbsolutePath(test_Name));
	_tcscat(DestName_temp, file_name);
	strDestName.Format(_T("%s"), DestName_temp);

	if(1)//!no_put_flie_flag)
	{
		if(0)//if (!put_file_by_ftp(strSourceName, strDestName))  
			//if (!download_file_thread(strSourceName, strDestName))
		{
			AfxMessageBox(_T("case_ircut_on_off_test putting file fail"), MB_OK);  
			no_put_flie_flag  = true;
			download_file_flag = FALSE;
			return FALSE;
		}
	}
	no_put_flie_flag  = false;
	//��������
	name_len = strlen(T2A(file_name));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(file_name), name_len);

	g_test_pass_flag[0] = 0;
	g_send_commad[0] = 1;


	memset(param_buf, 0, 10);
	
	
	//GetDlgItemText(IDC_TEST_TIME_IRCUT, str);
	if (g_test_config.m_test_ircut_time[0] == 0)
	{
		memcpy(param_buf, "3", 1);
	}
	else
	{
		name_len = strlen(T2A(g_test_config.m_test_ircut_time));
		memcpy(param_buf, T2A(g_test_config.m_test_ircut_time), name_len);
	}
	
	if (!Send_cmd(TEST_COMMAND, 2, name_buf, param_buf, name_len, 0))
	{
		AfxMessageBox(_T("Send_cmd��fail "), MB_OK); 
		return FALSE;
	}


	Anyka_Test_check_no_info(10000);
	g_test_pass_flag[0] = 0;
	g_send_commad[0] = 0;

	return TRUE;
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


	memset(test_Name, 0,  MAX_PATH);
	_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
	_tcscat(test_Name, _T("/"));
	_tcscat(test_Name, file_name);

	//�ж��ļ��Ƿ����
	if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(test_Name)))
	{
		str.Format(_T("%s no exist,����"), test_Name);
		AfxMessageBox(str, MB_OK); 
		return FALSE;
	}

	//�����ļ�
	memset(DestName_temp, 0, MAX_PATH);
	_tcsncpy(DestName_temp, DestName, sizeof(DestName));
	strSourceName.Format(_T("%s"), ConvertAbsolutePath(test_Name));
	_tcscat(DestName_temp, file_name);
	strDestName.Format(_T("%s"), DestName_temp);

	if(1)//!no_put_flie_flag)
	{
		if (!put_file_by_ftp(strSourceName, strDestName))  
		//if (!download_file_thread(strSourceName, strDestName))
		{
			AfxMessageBox(_T("case_ircut_test putting file fail"), MB_OK);  
			no_put_flie_flag  = true;
			download_file_flag = FALSE;
			return FALSE;
		}
	}
	no_put_flie_flag  = false;
	//��������
	name_len = strlen(T2A(file_name));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(file_name), name_len);

	g_test_pass_flag[0] = 0;
	g_send_commad[0] = 1;


	memset(param_buf, 0, 2);

	if (m_ircut_flag)
	{
		memcpy(param_buf, "1", 1);
	}
	else
	{
		memcpy(param_buf, "2", 1);
	}

	if(m_ircut_flag)
	{
		if (0)//!Send_cmd(TEST_COMMAND, 0, name_buf, param_buf, 0))
		{
			AfxMessageBox(_T("Send_cmd��fail "), MB_OK); 
			return FALSE;
		}
	}
	else
	{
		if (0)//!Send_cmd(TEST_COMMAND, 2, name_buf, param_buf, 0))
		{
			AfxMessageBox(_T("Send_cmd��fail "), MB_OK); 
			return FALSE;
		}
	}
	

	Anyka_Test_check_no_info(10000);
	g_test_pass_flag[0] = 0;
	g_send_commad[0] = 0;

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
	UINT pcm_rve_param = 0;

	if (g_hBurnThread_play_pcm != INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_hBurnThread_play_pcm);
		g_hBurnThread_play_pcm = INVALID_HANDLE_VALUE;
	}
	//param = &idex;
	g_hBurnThread_play_pcm = CreateThread(NULL, 0, paly_pcm_thread, &pcm_rve_param, 0, NULL);
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
 
int CAnykaIPCameraDlg::close_pcm(HWAVEOUT hwo) 
{
	if(hwo!=0)
	{
		waveOutClose(hwo);
		hwo = 0;
	}

	if(g_wait!=INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_wait);
		g_wait = INVALID_HANDLE_VALUE;
	}
	return 1;
}

HWAVEOUT CAnykaIPCameraDlg::open_pcm(void) 
{
	int             cnt = 0;
	HWAVEOUT        hwo;
	//WAVEHDR         wh;
	WAVEFORMATEX    wfx;
	DWORD  high = 0;
	BOOL ret = FALSE;
	DWORD read_len = 0;
	CPcmSpeaker ps; 

	wfx.wFormatTag = WAVE_FORMAT_PCM;//���ò��������ĸ�ʽ
	wfx.nChannels = 1;//������Ƶ�ļ���ͨ������
	wfx.nSamplesPerSec = 8500;//8500;//����ÿ���������źͼ�¼ʱ������Ƶ��
	wfx.nAvgBytesPerSec = 16000;//���������ƽ�����ݴ�����,��λbyte/s�����ֵ���ڴ��������С�Ǻ����õ�
	wfx.nBlockAlign = 2;//���ֽ�Ϊ��λ���ÿ����
	wfx.wBitsPerSample = 16;
	wfx.cbSize = sizeof(wfx);;//������Ϣ�Ĵ�С
	g_wait = CreateEvent(NULL, 0, 0, NULL);

	MMRESULT ret_temp = waveOutOpen(NULL, WAVE_MAPPER, &wfx,
		NULL, NULL, WAVE_FORMAT_QUERY);
	if (MMSYSERR_NOERROR != ret_temp) {
		return 0;
	}

	waveOutOpen(&hwo, WAVE_MAPPER, &wfx, (DWORD_PTR)g_wait, 0L, CALLBACK_EVENT);//��һ�������Ĳ�����Ƶ���װ�������лط�
	
	return hwo;
}

int CAnykaIPCameraDlg::play_pcm_buf(HWAVEOUT hwo, char *buf, UINT buf_len) 
{
	WAVEHDR header;
	ZeroMemory(&header, sizeof(WAVEHDR));
	header.dwBufferLength = buf_len;
	header.lpData = buf;

	waveOutPrepareHeader(hwo, &header, sizeof(WAVEHDR));
	waveOutWrite(hwo, &header, sizeof(WAVEHDR));
	//Sleep(500);
	while(waveOutUnprepareHeader(hwo,&header,sizeof(WAVEHDR)) ==WAVERR_STILLPLAYING)
	{
		Sleep(10);
	}

	
	return 1;
}


int CAnykaIPCameraDlg::play_pcm_test(TCHAR *pcm_file_name) 
{
	int             cnt = 0;
	HWAVEOUT        hwo;
	//WAVEHDR         wh;
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

//CStdioFile *g_pFile = NULL;

BOOL CAnykaIPCameraDlg::pcm_play_main() 
{
	int ret = 0;
	//char lpBuf[BUF_MAX_LEN] = {0};
	char *lpBuf = NULL;
	UINT nBufLen = BUF_MAX_LEN;
	CClientSocket m_ClientSocket;
	DWORD   write_len = 0;
	UINT idex = 0, time3 = 0, time4 = 0;
	HWAVEOUT hwo;
	CString str;


	//HANDLE hFile = CreateFile(g_test_config.ConvertAbsolutePath(_T("pcm_test.pcm")), GENERIC_WRITE, FILE_SHARE_READ , NULL, 
	//				OPEN_ALWAYS , FILE_ATTRIBUTE_NORMAL , NULL);//�����ļ�
	
	HGLOBAL hMem = GlobalAlloc(GMEM_ZEROINIT, BUF_MAX_LEN);
	if(NULL == hMem)
	{
		//CloseHandle(hFile);
		return FALSE;
	}

	lpBuf = (char *)GlobalLock(hMem);

	hwo = open_pcm();
	
	//
	while(1)
	{

		if(m_pcm_play_handle == INVALID_HANDLE_VALUE)
		{
			break;
		}

		if(g_monitor_end_flag)//ֹͣ�˳�
		{
			break;
		}
		
		//��������
		memset(lpBuf, 0, BUF_MAX_LEN);
		//time3 = GetTickCount();
		ret = m_ClientSocket.Rev_Socket_Receive(lpBuf, nBufLen, 0);
		//time4 = GetTickCount();
		//str.Format(_T("rev time = %d \r\n"), time4 - time3);//ƽ̨����
		//g_pFile->WriteString(str);
		if (ret == -1)
		{
			//str.Format(_T("%%%%%%%%%%%%%%%%%%%%%%%%%%%%\r\n"));//ƽ̨����
			//g_pFile->WriteString(str);
			Sleep(10);
			continue;
		}
		else
		{
			//������Ƶ����
			//time3 = GetTickCount();
			play_pcm_buf(hwo, lpBuf, ret);
			//time4 = GetTickCount();
			//str.Format(_T("play time = %d \r\n"), time4 - time3);//ƽ̨����
			//g_pFile->WriteString(str);
			//str.Format(_T("ret = %d\r\n"),ret);//ƽ̨����
			//g_pFile->WriteString(str);
			//WriteFile(hFile, (LPVOID)lpBuf, nBufLen, &write_len, NULL);
		}
	}

	close_pcm(hwo);
	GlobalUnlock(lpBuf);
	GlobalFree(hMem);
	//CloseHandle(hFile);
	return TRUE;
}


DWORD WINAPI pcm_play_thread(LPVOID lpParameter)
{
	CClientSocket m_ClientSocket;
	CAnykaIPCameraDlg pcm_play;

	pcm_play.pcm_play_main();

	pcm_play.close_pcm_play_thread();
	m_ClientSocket.Rev_Socket_Close(0);
	//if(g_pFile)
	//{
	//	g_pFile->Close();
	//	delete g_pFile;
	//	g_pFile = NULL;
	//}

	return 1;
}


void CAnykaIPCameraDlg::close_pcm_play_thread() 
{
	if(m_pcm_play_handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_pcm_play_handle);
		m_pcm_play_handle = INVALID_HANDLE_VALUE;
	}
}


BOOL CAnykaIPCameraDlg::Creat_pcm_play_thread(BOOL flag)
{
	//������С�����ӵ�socket
	CString str;
	BOOL connet_flag = false;
	UINT i = 0;

	USES_CONVERSION;

	// TODO: Add your control notification handler code here
	//m_ClientSocket.Create(0, SOCK_STREAM, NULL);
	
	if(flag)
	{
		Sleep(1000);
	}
	//Sleep(2000);

	for(i = 0; i < 1; i++)
	{
		if (!m_ClientSocket.Rev_Socket_Create(0))
		{
			if(flag)
			{
				//m_RightDlg.m_test_Status.SetFontColor(RGB(255,0,0));
				//m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("monitor Socket_Create fail"));
				//AfxMessageBox(_T("monitor Socket_Create fail"));
			}
			
			return FALSE;
		}

		
		if(m_ClientSocket.Rev_Socket_Connect(T2A(m_connect_ip), 6791, 0))
		{
			connet_flag = true;
			break;
		}
		//m_ClientSocket.Rev_Socket_Close(0);

		//Sleep(500);
		//case_monitor();

	}
	//if(m_ClientSocket.Connect(addr, 6789))
	if(connet_flag)	
	{
		//g_pFile = new CStdioFile(ConvertAbsolutePath(_T("info_log.txt")), 
		//CFile::modeCreate | CFile::modeWrite | CFile::typeBinary | CFile::shareDenyNone);

		close_pcm_play_thread();

		m_pcm_play_handle = CreateThread(NULL, 0, pcm_play_thread, 0, 0, NULL);
		if (m_pcm_play_handle == INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}

	}
	else
	{
		if(flag)
		{
			//m_RightDlg.m_test_Status.SetFontColor(RGB(255,0,0));
			//m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("monitor Socket_Connect fail"));
			//AfxMessageBox(_T("monitor Socket_Connect fail"));
		}
		return FALSE;
	}

	return TRUE;
}

BOOL CAnykaIPCameraDlg::case_monitor()
{
	//����һ���߳�
	g_monitor_end_flag = FALSE;
	g_test_config.config_voice_rev_test_pass = FALSE;
	if(!Creat_pcm_play_thread(FALSE))
	{
		//AfxMessageBox(_T("��������ʧ�ܣ�����ִ��̫�죬С��socket��û������"), MB_OK); 
		m_RightDlg.m_test_Status.SetFontColor(RGB(255,0,0));
		m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("��������ʧ�ܣ��鿴�豸��socket�Ƿ��Ѵ�������"));
		g_send_commad[0] = 0;
		return FALSE;
#if 0
		if (!case_monitor_ReTest())
		{
			g_send_commad[0] = 0;
			return FALSE;
		}
#endif

	}
	g_test_config.config_voice_rev_test_pass = TRUE;
	g_send_commad[0] = 0;
	return TRUE;
}

BOOL CAnykaIPCameraDlg::case_monitor_ReTest()
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
	g_send_commad[0] = 1;

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

	memset(g_test_config.m_test_monitor_time, 0, MAC_ADDRESS_LEN);
	_tcscpy(g_test_config.m_test_monitor_time, str);


	//�ж��ļ��Ƿ����
	if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(test_Name)))
	{
		g_send_commad[0] = 0;
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

	if(0)//!no_put_flie_flag )
	{
		if (!put_file_by_ftp(strSourceName, strDestName))  
		{
			AfxMessageBox(_T("monitor���� PutFile fail"), MB_OK);
			download_dev_file_flag = FALSE;
			no_put_flie_flag  = true;
			g_send_commad[0] = 0;
			return FALSE;
		}
	}
	no_put_flie_flag = false;

	//��������
	name_len = strlen(T2A(file_name));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(file_name), name_len);
	//memset(test_param, 0, MAX_PATH);
	//_tcscpy(test_param, test_pcm_name);

	memset(test_param, 0, MAX_PATH);
	_tcscpy(test_param, g_test_config.m_test_monitor_time);

	g_test_pass_flag[0] = 0;
	if (!Send_cmd(TEST_COMMAND, 0, name_buf, T2A(test_param), strlen( T2A(test_param)),0))
	{
		AfxMessageBox(_T("monitor���� Send_cmd fail "), MB_OK); 
		g_send_commad[0] = 0;
		return FALSE;
	}

	
	//����һ���߳�
	g_monitor_end_flag = FALSE;
	if(!Creat_pcm_play_thread(TRUE))
	{
		AfxMessageBox(_T("��������ʧ�٣��п�������ԭ����socketʧ�ܣ���ִ����һ���ز�"), MB_OK); 
		g_send_commad[0] = 0;
		return FALSE;

	}

	g_send_commad[0] = 0;

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
	g_send_commad[0] = 1;
	//���ز��Ե���Ƶ�ļ�
	memset(test_Name, 0,  MAX_PATH);
	_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
	_tcscat(test_Name, _T("/"));
	_tcscat(test_Name, test_pcm_name);

#if 1
	//�ж��ļ��Ƿ����
	if(0)//if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(test_Name)))
	{
		g_send_commad[0] = 0;
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
	if(0)//if (!put_file_by_ftp(strSourceName, strDestName))  
	//if (!download_file_thread(strSourceName, strDestName))  
	{
		AfxMessageBox(_T(" PutFile test_pcm.mp3 fail"), MB_OK);  
		download_dev_file_flag = FALSE;
		g_send_commad[0] = 0;
		return;
	}

#endif

	//���ز��Գ���
	memset(test_Name, 0,  MAX_PATH);
	_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
	_tcscat(test_Name, _T("//"));
	_tcscat(test_Name, file_name);

	//�ж��ļ��Ƿ����
	if(0)//if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(test_Name)))
	{
		g_send_commad[0] = 0;
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

	if(!no_put_flie_flag )
	{
		if(0)//if (!put_file_by_ftp(strSourceName, strDestName))  
		//if (!download_file_thread(strSourceName, strDestName))  
		{
			AfxMessageBox(_T("PutFile test_interphone fail"), MB_OK);  
			download_dev_file_flag = FALSE;
			g_send_commad[0] = 0;
			no_put_flie_flag  = true;
			return ;
		}
	}
	no_put_flie_flag = false;

	//��������
	name_len = strlen(T2A(file_name));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(file_name), name_len);

	memset(test_param, 0, MAX_PATH);
	_tcscpy(test_param, test_pcm_name);

	g_test_pass_flag[0] = 0;
	if (0)//!Send_cmd(TEST_COMMAND, 0, name_buf, T2A(test_param), 0))
	{
		AfxMessageBox(_T("�Խ����� Send_cmd fail "), MB_OK); 
		g_send_commad[0] = 0;
		return ;
	}
	Anyka_Test_check_no_info(1000);
	g_test_pass_flag[0] = 0;
	g_send_commad[0] = 0;
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
	g_send_commad[0] = 1;

	memset(test_Name, 0,  MAX_PATH);
	_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
	_tcscat(test_Name, _T("/"));
	_tcscat(test_Name, file_name);

	//�ж��ļ��Ƿ����
	if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(test_Name)))
	{
		g_send_commad[0] = 0;
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

	if(1)//!no_put_flie_flag )
	{
		if (!put_file_by_ftp(strSourceName, strDestName))  
		//if (!download_file_thread(strSourceName, strDestName)) 
		{
			AfxMessageBox(_T("PutFile test_ptz fail"), MB_OK);  
			download_dev_file_flag = FALSE;
			g_send_commad[0] = 0;
			no_put_flie_flag  = true;
			return ;
		}
	}
	no_put_flie_flag = false;

	//��������
	name_len = strlen(T2A(file_name));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(file_name), name_len);

	g_test_pass_flag[0] = 0;
	if (!Send_cmd(TEST_COMMAND, 0, name_buf, NULL, 0, 0))
	{
		AfxMessageBox(_T("��̨���� Send_cmd fail "), MB_OK); 
		g_send_commad[0] = 0;
		return ;
	}
	Anyka_Test_check_no_info(1000);
	g_test_pass_flag[0] = 0;
	g_send_commad[0] = 0;
	
}
BOOL CAnykaIPCameraDlg::case_sd()
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
	TCHAR *test_sd_name = _T("sd_size.txt");
	UINT time1 = 0, time2 = 0;
	UINT wait_time = 0;


	USES_CONVERSION;
	if (!g_connet_flag)
	{
		AfxMessageBox(_T("����û�������ϣ�����"), MB_OK);
		return FALSE;
	}

	//g_test_config.config_sd_test_pass = FALSE;
	//g_send_commad[0] = 1;
	//m_RightDlg.GetDlgItem(IDC_STATIC_SDZISE)->SetWindowText(_T(""));
	//m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->ShowWindow(SW_HIDE);
	//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(FALSE);//����
	//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//���� 
	//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����

#if 0
	memset(test_Name, 0,  MAX_PATH);
	_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
	_tcscat(test_Name, _T("/"));
	_tcscat(test_Name, file_name);

	//�ж��ļ��Ƿ����
	if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(test_Name)))
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
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

	if(!no_put_flie_flag)
	{
		if (m_pFtpConnection == NULL || !m_pFtpConnection->PutFile(strSourceName,
			strDestName, FTP_TRANSFER_TYPE_BINARY, 1))   
		{
			m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
			m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
			m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
			AfxMessageBox(_T("sd���� PutFile fail"), MB_OK);  
			download_dev_file_flag = FALSE;
			g_send_commad = 0;
			return;
		}
	}
	no_put_flie_flag = false;
#endif
	if(g_test_config.sd_reset_flag)
	{
		//��������
		name_len = strlen(T2A(file_name));
		memset(name_buf, 0, MAX_PATH);
		memcpy(name_buf, T2A(file_name), name_len);

		g_test_pass_flag[0] = 0;
		g_test_config.m_test_sd_flag = 0;
		if (!Send_cmd(TEST_COMMAND, 1, name_buf, NULL, 0, 0))
		{
			//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
			//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
			//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
			AfxMessageBox(_T("sd test fail Send_cmd fail "), MB_OK); 
			g_send_commad[0] = 0;
			return FALSE;
		}
		start_test = FALSE;

		//Sleep(3000);

		//���շ���ֵ
		if (!Anyka_Test_sd_check_info(30000))
		{
			start_test = TRUE;
			//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
			//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
			//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
			//AfxMessageBox(_T("sd����ʧ��"), MB_OK);
			//m_RightDlg.m_test_Status.SetFontColor(RGB(255,0,0));
			//m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->ShowWindow(SW_SHOW);
			//m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("sd������ʧ��"));
			//m_RightDlg.GetDlgItem(IDC_STATIC_SDZISE)->SetWindowText(_T("sd������ʧ��"));
			
			g_send_commad[0] = 0;
			//Sleep(1000);
			//case_main(FALSE);
			return FALSE;
		}
		return TRUE;

		wait_time = 6000;
	}
	else
	{
		wait_time = 3000;
	}

#if 0
	//��ȡsd���������ļ�
	memset(DestName_temp, 0, MAX_PATH);
	strDestName.Format(_T("%s"), ConvertAbsolutePath(test_sd_name));

	_tcsncpy(DestName_temp, DestName, sizeof(DestName));
	_tcscat(DestName_temp, test_sd_name);
	strSourceName.Format(_T("%s"), DestName_temp);
	time1 = GetTickCount();
	while (1)
	{
		//��ʱ����
		
		time2 = GetTickCount();
		if (time2 - time1 > wait_time )//2000)
		{
			//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
			//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//����
			//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
			m_RightDlg.m_test_Status.SetFontColor(RGB(255,0,0));
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("sd������ʧ��"));
			m_RightDlg.GetDlgItem(IDC_STATIC_SDZISE)->SetWindowText(_T("sd������ʧ��"));
		    m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->ShowWindow(SW_SHOW);
			AfxMessageBox(_T("��ȡsd�ļ���ʱ�˳�"), MB_OK);  
			g_send_commad[0] = 0;
			return FALSE;
		}

		if (m_pFtpConnection[0] != NULL )
		{
			if (!m_pFtpConnection[0]->GetFile(strSourceName, strDestName, FALSE, 
				FILE_ATTRIBUTE_NORMAL, FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD, 1))   
			{
				continue;
			}
			else
			{
				break;
			}
		}

		//���ڲ����豸ʱ,����豸��������,����ʼ����sd��wifi�Ĳ���,����ʱ�͵㻻��һ̨ʱ,��ʱ����ͻ����,��ʱӦ���س���.
		if (!g_connet_flag)
		{
			return TRUE;
		}

		Sleep(100);
	}

	//�ж��ļ��Ƿ����
	DWORD faConfig = GetFileAttributes(ConvertAbsolutePath(test_sd_name));
	if(0xFFFFFFFF == faConfig)
	{
		//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
		//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		m_RightDlg.m_test_Status.SetFontColor(RGB(255,0,0));
		m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("sd������ʧ��"));
		m_RightDlg.GetDlgItem(IDC_STATIC_SDZISE)->SetWindowText(_T("sd������ʧ��"));
		m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->ShowWindow(SW_SHOW);
		AfxMessageBox(_T("û�л�ȡС���ϵ�sd�����ļ� "), MB_OK); 
		g_send_commad[0] = 0;
		return FALSE;
	}
	else
	{
		faConfig &= ~FILE_ATTRIBUTE_READONLY;//����ļ���ֻ������Ҫ���ֻ��
		faConfig &= ~FILE_ATTRIBUTE_SYSTEM;  //����ļ���ϵͳ����ô���ϵͳ
		faConfig &= ~FILE_ATTRIBUTE_TEMPORARY;//���������ʱ����ôҲҪ�����ʱ
		SetFileAttributes(ConvertAbsolutePath(test_sd_name), faConfig);	
	}

	//���sd�ļ�����ȡsd����
	if(!decode_file(ConvertAbsolutePath(test_sd_name)) || g_sd_size == 0)
	{
		remove(T2A(ConvertAbsolutePath(test_sd_name)));
		//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
		//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		m_RightDlg.m_test_Status.SetFontColor(RGB(255,0,0));
		m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("sd������ʧ��"));
		m_RightDlg.GetDlgItem(IDC_STATIC_SDZISE)->SetWindowText(_T("sd������ʧ��"));
		m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->ShowWindow(SW_SHOW);
		AfxMessageBox(_T("��ȡС���ϵ�sd�����ļ��Ǵ���� "), MB_OK); 
		g_send_commad[0] = 0;
		return FALSE;
	}

	remove(T2A(ConvertAbsolutePath(test_sd_name)));
	m_RightDlg.m_test_Status.SetFontColor(RGB(0,255,0));
	//MessageBox(_T("sd���Գɹ�"), MB_OK);
	str.Format(_T("sd�����Գɹ�,����Ϊ(��λ��K)��%ld"), g_sd_size);
	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(str);
	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->ShowWindow(SW_SHOW);
	m_RightDlg.GetDlgItem(IDC_STATIC_SDZISE)->SetWindowText(str);
	g_test_config.config_sd_test_pass = TRUE;
	start_test = TRUE;
	//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
	//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
	//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
	g_test_pass_flag[0] = 0;
	g_send_commad[0] = 0;
	//Sleep(2000);
	//case_main(TRUE);
#endif

	return TRUE;
}


BOOL CAnykaIPCameraDlg::case_wifi()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strSourceName, strDestName,strTemp;
	TCHAR test_Name[MAX_PATH] = {0};
	TCHAR DestName_temp[MAX_PATH] = {0};
	TCHAR DestName[50] =_T("/tmp/");
	UINT name_len = 0;
	char name_buf[MAX_PATH] = {0};
	char param_buf[2] = {0};
	CString str;
	TCHAR *file_name = _T("test_wifi");
	TCHAR test_param[MAX_PATH];
	TCHAR *test_wifi_name = _T("ap_list.txt");
	UINT time1 = 0, time2 = 0, i = 0;

	USES_CONVERSION;
	if (!g_connet_flag)
	{
		AfxMessageBox(_T("����û�������ϣ�����"), MB_OK);
		return FALSE;
	}
	g_test_config.config_wifi_test_pass = FALSE;
	//m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->ShowWindow(SW_HIDE);
	//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(FALSE);//����
	//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//���� 
	//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����

	g_send_commad[0] = 1;
#if 0
	memset(test_Name, 0,  MAX_PATH);
	_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
	_tcscat(test_Name, _T("/"));
	_tcscat(test_Name, file_name);


	GetDlgItemText(IDC_EDIT_WIFI_NAME, str);
	if (str.IsEmpty())
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
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
		m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
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

	
	if(!no_put_flie_flag)
	{
		if (m_pFtpConnection == NULL || !m_pFtpConnection->PutFile(strSourceName,
			strDestName, FTP_TRANSFER_TYPE_BINARY, 1))   
		{
			m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
			m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//����
			m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
			AfxMessageBox(_T("wifi���� PutFile fail"), MB_OK);
			download_dev_file_flag = FALSE;
			g_send_commad = 0;
			return;
		}
	}
	no_put_flie_flag = false;
#endif

	if(g_test_config.wifi_reset_flag)
	{
		//��������
		name_len = strlen(T2A(file_name));
		memset(name_buf, 0, MAX_PATH);
		memcpy(name_buf, T2A(file_name), name_len);

		memset(test_param, 0, MAX_PATH);
		_tcscpy(test_param, g_test_config.m_wifi_name);

		g_test_pass_flag[0] = 0;
		g_test_config.m_test_wifi_flag = 0;
		if (!Send_cmd(TEST_COMMAND, 1, name_buf, T2A(test_param), strlen(T2A(test_param)), 0))
		{
			//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
			//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
			//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
			AfxMessageBox(_T("wifi���� Send_cmd fail "), MB_OK);
			g_send_commad[0] = 0;
			return  FALSE;
		}

		start_test = FALSE;

		Sleep(500);

		//���շ���ֵ
		if (!Anyka_Test_wifi_check_info(30000))
		{
			start_test = TRUE;
			//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
			//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
			//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
			//AfxMessageBox(_T("wifi����ʧ��"), MB_OK);
			//m_RightDlg.m_test_Status.SetFontColor(RGB(255,0,0));
			//m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("wifi����ʧ��"));
			//m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->ShowWindow(SW_SHOW);
			g_send_commad[0] = 0;
			//Sleep(2000);
			//case_main(FALSE);
			return FALSE;
		}
		return TRUE;
	}

#if 0
	//��ȡwify�ļ���Ϣ
	memset(DestName_temp, 0, MAX_PATH);
	strDestName.Format(_T("%s"), ConvertAbsolutePath(test_wifi_name));

	_tcsncpy(DestName_temp, DestName, sizeof(DestName));
	_tcscat(DestName_temp, test_wifi_name);
	strSourceName.Format(_T("%s"), DestName_temp);
	time1 = GetTickCount();
	while (1)
	{
		//��ʱ����
		Sleep(500);
		time2 = GetTickCount();
		if (time2 - time1 > 20000)
		{
			//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
			//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//����
			//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
			m_RightDlg.m_test_Status.SetFontColor(RGB(255,0,0));
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("wifi����ʧ��"));
		    m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->ShowWindow(SW_SHOW);
			AfxMessageBox(_T("��ȡwifi�ļ���ʱ�˳�(20S)"), MB_OK);  
			g_send_commad[0] = 0;
			return FALSE;
		}

		if (m_pFtpConnection[0] != NULL )
		{
			if (!m_pFtpConnection[0]->GetFile(strSourceName, strDestName, 
				FALSE, FILE_ATTRIBUTE_NORMAL, FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD, 1))   
			{
				continue;
			}
			else
			{
				break;
			}
		}
		
		//���ڲ����豸ʱ,����豸��������,����ʼ����sd��wifi�Ĳ���,����ʱ�͵㻻��һ̨ʱ,��ʱ����ͻ����,��ʱӦ���س���.
		if (!g_connet_flag)
		{
			return TRUE;
		}
	}

	//�ж��ļ��Ƿ����
	DWORD faConfig = GetFileAttributes(ConvertAbsolutePath(test_wifi_name));
	if(0xFFFFFFFF == faConfig)
	{
		//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
		//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		AfxMessageBox(_T("û�л�ȡС���ϵ�wifi�ļ� "), MB_OK); 
		g_send_commad[0] = 0;
		return FALSE;
	}
	else
	{
		faConfig &= ~FILE_ATTRIBUTE_READONLY;//����ļ���ֻ������Ҫ���ֻ��
		faConfig &= ~FILE_ATTRIBUTE_SYSTEM;  //����ļ���ϵͳ����ô���ϵͳ
		faConfig &= ~FILE_ATTRIBUTE_TEMPORARY;//���������ʱ����ôҲҪ�����ʱ
		SetFileAttributes(ConvertAbsolutePath(test_wifi_name), faConfig);
	}

	//���wifi�ļ���
	if(!decode_file(ConvertAbsolutePath(test_wifi_name)) || g_ssid_num == 0)
	{
		remove(T2A(ConvertAbsolutePath(test_wifi_name)));
		//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
		//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		AfxMessageBox(_T("��ȡС���ϵ�wifi�ļ��Ǵ���� "), MB_OK); 
		g_send_commad[0] = 0;
		return FALSE;
	}

	remove(T2A(ConvertAbsolutePath(test_wifi_name)));


	//��ʾwifi��Ϣ
	//m_RightDlg.GetDlgItem(IDC_LIST_WIFI)->EnableWindow(TRUE);//����
	for(i = 0; i < g_ssid_num; i++)
	{	
#if 0
		memset(DestName_temp, 0, MAX_PATH);
		strTemp.Format(_T("%d"), i+1);
		m_RightDlg.m_test_wifi_list.InsertItem(i, strTemp);
		//strTemp.Format(_T("%s"), g_ssid_info[i].wify_ssid_name);
		//_tcsncpy(DestName_temp, DestName, sizeof(DestName));
		m_RightDlg.m_test_wifi_list.SetItemText(i, 1,A2T(g_ssid_info[i].wify_ssid_name));
		//m_RightDlg.m_test_wifi_list.SetItemText(i, 1,_T("С��·����"));
		strTemp.Format(_T("%d"), g_ssid_info[i].quality);
		m_RightDlg.m_test_wifi_list.SetItemText(i, 2,strTemp);
#endif

		m_BottomDlg.m_test_wifi_list.InsertItem(i, strTemp);
		//strTemp.Format(_T("%s"), g_ssid_info[i].wify_ssid_name);
		//_tcsncpy(DestName_temp, DestName, sizeof(DestName));
		m_BottomDlg.m_test_wifi_list.SetItemText(i, 1,A2T(g_ssid_info[i].wify_ssid_name));
		//m_RightDlg.m_test_wifi_list.SetItemText(i, 1,_T("С��·����"));
		strTemp.Format(_T("%d"), g_ssid_info[i].quality);
		m_BottomDlg.m_test_wifi_list.SetItemText(i, 2,strTemp);
	}


	//MessageBox(_T("wifi���Գɹ�"), MB_OK);
	m_RightDlg.m_test_Status.SetFontColor(RGB(0,255,0));
	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("wifi���Գɹ�"));
	//m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->ShowWindow(SW_SHOW);
	g_test_config.config_wifi_test_pass = TRUE;
	start_test = TRUE;
	//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
	//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
	//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
	g_test_pass_flag[0] = 0;
	g_send_commad[0] = 0;
	//Sleep(2000);
	//case_main(TRUE);

	//m_RightDlg.UpdateWindow();
	//m_BottomDlg.UpdateWindow();
#endif
	return TRUE;
}

BOOL CAnykaIPCameraDlg::case_rev()
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
		return FALSE;
	}
	g_send_commad[0] = 1;

	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->ShowWindow(SW_HIDE);

	memset(test_Name, 0,  MAX_PATH);
	_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
	_tcscat(test_Name, _T("/"));
	_tcscat(test_Name, file_name);
#if 0
	GetDlgItemText(IDC_EDIT_WIFI_NAME, str);
	if(init_flag)
	{
		str=g_test_config.m_test_reset_time;
	}
	if (str.IsEmpty())
	{
		//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
		//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		g_send_commad[0] = 0;
		str.Format(_T("��λ������,ʱ��Ϊ�գ�����д������Ҫ��ʱ����"));
		AfxMessageBox(str, MB_OK); 
		return FALSE;
	}
	memset(g_test_config.m_test_reset_time, 0, MAC_ADDRESS_LEN);
	_tcscpy(g_test_config.m_test_reset_time, str);
#endif

	//�ж��ļ��Ƿ����
	if(0)//if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(test_Name)))
	{
		g_send_commad[0] = 0;
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
	if(1)//!no_put_flie_flag)
	{
		if(0)//if (!put_file_by_ftp(strSourceName, strDestName))  
		//if (!download_file_thread(strSourceName, strDestName))
		{
			//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
			//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
			//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
			AfxMessageBox(_T("Error no auto test putting file,������С��"), MB_OK);  
			download_dev_file_flag = FALSE;
			g_send_commad[0] = 0;
			no_put_flie_flag  = true;
			return FALSE;
		}
	}
	no_put_flie_flag = false;


	//��������
	name_len = strlen(T2A(file_name));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(file_name), name_len);

	memset(test_param, 0, MAX_PATH);
	_tcscpy(test_param, g_test_config.m_test_reset_time);

	g_test_pass_flag[0] = 0;
	if (!Send_cmd(TEST_COMMAND, 1, name_buf, T2A(test_param), strlen(T2A(test_param)), 0))
	{
		AfxMessageBox(_T("Send_cmd��fail "), MB_OK); 
		g_send_commad[0] = 0;
		return FALSE;
	}

#if 1
	Create_Test_Wait_Data();
#else

	Sleep(500);

	//���շ���ֵ
	if (!Anyka_Test_check_info(30000))
	{
		//AfxMessageBox(_T("����ʧ��"), MB_OK);
		m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("����ʧ��"));
		m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->ShowWindow(SW_SHOW);
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
#endif

	return TRUE;
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

	if (!put_file_by_ftp(strSourceName, strDestName))  
	//if (!download_file_thread(strSourceName, strDestName))
	{
		AfxMessageBox(_T("Download_UpdateFile put_file_by_ftp fail"));
		return false;
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
	CHAR tmpBuf_temp[MAC_ADDRESS_LEN+1] = {0};
	TCHAR tmpBuf_1[MAC_ADDRESS_LEN+1] = {0};
	int  surbuf_len = 0;
	int  i = 0;
	int idex = 0;
	BOOL flag = FALSE;
	UINT tempmac = 0;

	lower_to_upper(surbuf, tmpBuf);
	surbuf_len = wcslen(surbuf);
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
	_tcsncpy(dstbuf, tmpBuf, MAC_ADDRESS_LEN);

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
#if 0
	m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(FALSE);//����
	m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//���� 
	m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//���� 
#endif

	if (!g_connet_flag)
	{
		//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
		//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//����
		//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_WRITE_MAC)->EnableWindow(TRUE);
		AfxMessageBox(_T("����û�������ϣ�����"), MB_OK);
		return FALSE;
	}

	//�Ƚ�MAC��ַ�Ĵ�С
	if (_tcsncmp(g_test_config.m_mac_current_addr, g_test_config.m_mac_end_addr, 17) == 0)
	{
		AfxMessageBox(_T("MAC��ַ�ѳ���MAC��ַ���趨��Χ������"), MB_OK);
		//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//����
		//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_WRITE_MAC)->EnableWindow(TRUE);
		g_send_commad[0] = 0;
		return FALSE;
	}

	g_send_commad[0] = 1;
	if(0)//!no_put_flie_flag )
	{
		if (!Download_UpdateFile(buf_update_name))
		{
			//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
			//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//����
			//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
			m_BottomDlg.GetDlgItem(IDC_BUTTON_WRITE_MAC)->EnableWindow(TRUE);
			g_send_commad[0] = 0;
			no_put_flie_flag  = true;
			return FALSE;
		}

		if (!Download_UpdateFile(buf_mac_name))
		{
			//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
			//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//����
			//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
			m_BottomDlg.GetDlgItem(IDC_BUTTON_WRITE_MAC)->EnableWindow(TRUE);
			g_send_commad[0] = 0;
			no_put_flie_flag  = true;
			return FALSE;
		}
	}
	no_put_flie_flag = false;

	name_len = strlen(T2A(buf_mac_name));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(buf_mac_name), name_len);

	memset(buf_temp, 0, MAX_PATH);

	g_test_config.m_test_MAC_flag = 0;
	if (!Send_cmd(TEST_COMMAND, 1, name_buf, T2A(g_test_config.m_mac_current_addr), strlen( T2A(g_test_config.m_mac_current_addr)), 0))
	{
		//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
		//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//���� 
		m_BottomDlg.GetDlgItem(IDC_BUTTON_WRITE_MAC)->EnableWindow(TRUE);
		g_send_commad[0] = 0;
		AfxMessageBox(_T("Error putting file"));
		return FALSE;
	}

	//g_test_config.m_mac_current_addr����1
	Mac_Addr_add_1(g_test_config.m_mac_current_addr);

	g_test_config.Write_current_macConfig(CONFIG_CURRENT_MAC);

	if (!Anyka_Test_mac_check_info(30000))
	{
		//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
		//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//���� 
		m_BottomDlg.GetDlgItem(IDC_BUTTON_WRITE_MAC)->EnableWindow(TRUE);
		g_send_commad[0] = 0;
		//AfxMessageBox(_T("MAC��¼ʧ��"), MB_OK);
		return FALSE;
	}
	//MessageBox(_T("MAC���Գɹ�"), MB_OK);
	//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
	//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
	//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//���� 
	m_BottomDlg.GetDlgItem(IDC_BUTTON_WRITE_MAC)->EnableWindow(TRUE);
	g_test_pass_flag[0] = 0;
	g_send_commad[0] = 0;

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

	if((m_pFtpConnection[0] != NULL) && (m_pInetSession[0] != NULL))
	{
		//�����ļ�
		//�������¼���кţ�

		//memset(serial_temp, 0, MAX_PATH);
		//len = _tcslen(g_test_config.m_uid_number);
		//_tcsncpy(serial_temp, &g_test_config.m_uid_number[len - 20], 20);
		//swprintf(serial_buf, _T("%s/uid.txt"), SERAIL_CONFIG);
		//strSourceName.Format(_T("%s"), g_test_config.ConvertAbsolutePath(serial_buf));

		if (g_test_config.uid_download_mode == 1)
		{
			memset(serial_temp, 0, MAX_PATH+1);
			len = _tcslen(g_test_config.m_uid_number);
			_tcsncpy(serial_temp, &g_test_config.m_uid_number[len - g_test_config.uid_download_len], g_test_config.uid_download_len);
			swprintf(serial_buf, _T("%s/%s.conf"), SERAIL_CONFIG, serial_temp);
			strSourceName.Format(_T("%s"), g_test_config.ConvertAbsolutePath(serial_buf));
			
			UINT config_len = _tcslen(serial_buf);
			UINT config_idex_len = _tcslen(SERAIL_CONFIG); 

			_tcsncpy(g_test_config.serial_file_name, &serial_buf[config_idex_len+1], config_len - config_idex_len-1);



		}
		else
		{
			memset(serial_temp, 0, MAX_PATH+1);
			len = _tcslen(g_test_config.m_uid_number);
			_tcsncpy(serial_temp, &g_test_config.m_uid_number[len - g_test_config.uid_download_len], g_test_config.uid_download_len);
			swprintf(serial_buf, _T("%s/uid.txt"), SERAIL_CONFIG);
			strSourceName.Format(_T("%s"), g_test_config.ConvertAbsolutePath(serial_buf));

			_tcsncpy(g_test_config.serial_file_name, _T("uid.txt"), 12);
		}

		

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
		if (m_pFtpConnection[0] == NULL || !m_pFtpConnection[0]->PutFile(strSourceName, 
			strDestName, FTP_TRANSFER_TYPE_BINARY, 1))   
		{
			AfxMessageBox(_T("Error auto test putting file"));   
			return FALSE;
		}

#if 0
		if (g_test_config.uid_download_mode == 1)
		{
			memset(srcfileName, 0, MAX_PATH);
			memset(dstfileName, 0, MAX_PATH);
			//�����к���¼�ɹ��󣬰Ѵ��ļ������������ļ�����
			swprintf(srcfileName, _T("%s/%s.conf"), g_test_config.ConvertAbsolutePath(SERAIL_CONFIG), serial_temp);
			swprintf(dstfileName, _T("%s/%s.conf"), g_test_config.ConvertAbsolutePath(SERAIL_CONFIG_BAK), serial_temp);

			if (MoveFile(srcfileName, dstfileName) == 0)
			{
				AfxMessageBox(_T("�ƶ��ļ�ʧ��,���Ա����ļ������Ѵ��ڴ��ļ�,����"));   
				return FALSE;
			}
		}
		
#endif
	}
	return TRUE;
}


BOOL CAnykaIPCameraDlg::compare_serialfile(TCHAR *srcfile, TCHAR *dstfile)
{
	CFile fp_src;
	CFile fp_dst;
	UINT len_src = 0;
	UINT len_dst = 0;
	char* buff_src = NULL;
	char* buff_dst = NULL;
	UINT i = 0;


	fp_src.Open(g_test_config.ConvertAbsolutePath(srcfile), CFile::modeRead);
	len_src = fp_src.GetLength();
	
	fp_dst.Open(g_test_config.ConvertAbsolutePath(dstfile), CFile::modeRead);
	len_dst = fp_dst.GetLength();


    if((len_src==0)||(len_dst==0))
	{
		fp_src.Close();
		fp_dst.Close();
		AfxMessageBox(_T("��ȡ��uid�ļ�����Ϊ0"), MB_OK);
		return FALSE;
	}

	if (len_src != len_dst)
	{
		fp_src.Close();
		fp_dst.Close();
		AfxMessageBox(_T("д��Ͷ�����uid�ļ����Ȳ�һ�����Ƚϳ���"), MB_OK);
		return FALSE;
	}
	

	buff_src=new char[len_src];
	fp_src.Read(buff_src,len_src);

	buff_dst=new char[len_dst];
	fp_dst.Read(buff_dst,len_dst);
	
	for (i = 0; i < len_src; i++)
	{
		if (buff_src[i] != buff_dst[i])
		{
			delete[]buff_src;
			delete[]buff_dst;

			fp_src.Close();
			fp_dst.Close();

			AfxMessageBox(_T("����uid�ļ������ݱȽϲ�һ��"), MB_OK);
			return FALSE;
		}
	}

	delete[]buff_src;
	delete[]buff_dst;

	fp_src.Close();
	fp_dst.Close();

	return TRUE;
}

BOOL CAnykaIPCameraDlg::get_and_compare_serialfile_toftp(void)
{
	CString strSourceName, strDestName;
	TCHAR DestName[MAX_PATH] = _T("/tmp");
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

	if((m_pFtpConnection[0] != NULL) && (m_pInetSession[0] != NULL))
	{
		memset(serial_temp, 0, MAX_PATH+1);
		len = _tcslen(g_test_config.m_uid_number);
		_tcsncpy(serial_temp, &g_test_config.m_uid_number[len - g_test_config.uid_download_len], g_test_config.uid_download_len);
		swprintf(serial_buf, _T("%s/%s.conf"), SERAIL_CONFIG, serial_temp);
		strSourceName.Format(_T("%s"), g_test_config.ConvertAbsolutePath(serial_buf));

		//�ж��ļ��Ƿ����
		if(0xFFFFFFFF == GetFileAttributes(g_test_config.ConvertAbsolutePath(serial_buf)))
		{
			CString str;
			str.Format(_T("compare %s no exist"), g_test_config.ConvertAbsolutePath(serial_buf));
			AfxMessageBox(str);   
			return FALSE;
		}
		//��ȡ�ļ�
		strDestName.Format(_T("%s/%s.conf"), DestName, serial_temp);

		swprintf(temp_buf, _T("%s.conf"), serial_temp);
		strSourceName.Format(_T("%s"), g_test_config.ConvertAbsolutePath(temp_buf));

		if (m_pFtpConnection[0] == NULL || !m_pFtpConnection[0]->GetFile(strDestName,strSourceName, FTP_TRANSFER_TYPE_BINARY, 1))   
		{
			AfxMessageBox(_T("Error auto test putting file"));   
			return FALSE;
		}


		//�Ƚ��ļ�
		if (!compare_serialfile(serial_buf,temp_buf))
		{
			return FALSE;
		}
		DWORD faConfig = GetFileAttributes(strSourceName); 
		if(0xFFFFFFFF != faConfig)
		{
			faConfig &= ~FILE_ATTRIBUTE_READONLY;//����ļ���ֻ������Ҫ���ֻ��
			faConfig &= ~FILE_ATTRIBUTE_SYSTEM;  //����ļ���ϵͳ����ô���ϵͳ
			faConfig &= ~FILE_ATTRIBUTE_TEMPORARY;//���������ʱ����ôҲҪ�����ʱ
			SetFileAttributes(strSourceName, faConfig);
		}
		
		if(!DeleteFile(g_test_config.ConvertAbsolutePath(strSourceName)))
		{
			AfxMessageBox(_T("ɾ���ļ�ʧ��"));   
			return FALSE;
		}
		memset(srcfileName, 0, MAX_PATH);
		memset(dstfileName, 0, MAX_PATH);
		//�����к���¼�ɹ��󣬰Ѵ��ļ������������ļ�����
		swprintf(srcfileName, _T("%s/%s.conf"), g_test_config.ConvertAbsolutePath(SERAIL_CONFIG), serial_temp);
		swprintf(dstfileName, _T("%s/%s.conf"), g_test_config.ConvertAbsolutePath(SERAIL_CONFIG_BAK), serial_temp);

		if (MoveFile(srcfileName, dstfileName) == 0)
		{
			AfxMessageBox(_T("�ƶ��ļ�ʧ��,���Ա����ļ������Ѵ��ڴ��ļ�,����"));   
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CAnykaIPCameraDlg::case_uid()
{

	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UINT serial_num_len = 0;
	TCHAR test_param[MAX_PATH] = {0} ;
	TCHAR *buf_uid_name = _T("test_pushid");
	TCHAR test_Name[MAX_PATH] = {0};
	UINT name_len = 0;
	char name_buf[MAX_PATH] = {0};
	CString strSourceName, strDestName;
	//CString uidStr;
	TCHAR DestName[50] =_T("/tmp/");
	TCHAR DestName_temp[MAX_PATH] = {0};
	TCHAR burnUid[MAX_PATH+1] = {0};
	UINT len = 0;
	CString str;
	BOOL file_no_exist_flag = FALSE;
	TCHAR serial_buf[MAX_PATH*2+1] = {0};
	TCHAR srcfileName[MAX_PATH] = {0};
	TCHAR serial_temp[MAX_PATH+1] = {0};
	TCHAR dstfileName[MAX_PATH*2 + 1] = {0};
	TCHAR fileName[MAX_PATH] = {0};
	TCHAR temp_buf[MAX_PATH] = {0};
	DWORD lasterror = 0;
	unsigned char *uid_buf = NULL;
	UINT uid_len = 0, idex = 0;
	USES_CONVERSION;

	//m_CBurnUidDlg.m_uid_edit.GetWindowTextW(uidStr);
	//CString Uid=uidStr.Mid(40);
	//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(FALSE);//����
	//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//���� 
	//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����

	if (!g_connet_flag)
	{
		//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
		//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		AfxMessageBox(_T("����û�������ϣ�����"), MB_OK);
		return FALSE;
	}
	g_send_commad[0] = 1;
	serial_num_len = _tcslen(g_test_config.m_uid_number);
	if (g_test_config.m_uid_number[0] == 0 && serial_num_len == 0)
	{
		CString str;
		g_send_commad[0] = 0;
		str.Format(_T("���к�Ϊ�գ���ɨ�����к�"));
		AfxMessageBox(str); 
		//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
		//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		return FALSE;
	}


	//�ж��ļ��Ƿ����
	memset(test_Name, 0,  sizeof(TEST_CONFIG_DIR));
	_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
	_tcscat(test_Name, _T("/"));
	_tcscat(test_Name, buf_uid_name);
	if(0)//0xFFFFFFFF == GetFileAttributes(g_test_config.ConvertAbsolutePath(test_Name)))
	{
		CString str;
		g_send_commad[0] = 0;
		str.Format(_T("%s no exist"), buf_uid_name);
		AfxMessageBox(str);   
		//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
		//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		return FALSE;
	}

	name_len = strlen(T2A(buf_uid_name));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(buf_uid_name), name_len);
	if (g_test_config.uid_download_mode == 1)
	{
		memset(serial_temp, 0, MAX_PATH+1);
		len = _tcslen(g_test_config.m_uid_number);
		_tcsncpy(serial_temp, &g_test_config.m_uid_number[len - g_test_config.uid_download_len], g_test_config.uid_download_len);
		swprintf(serial_buf, _T("%s/%s.conf"), SERAIL_CONFIG, serial_temp);
		strSourceName.Format(_T("%s"), g_test_config.ConvertAbsolutePath(serial_buf));
			
		UINT config_len = _tcslen(serial_buf);
		UINT config_idex_len = _tcslen(SERAIL_CONFIG); 
	
		CFile fp;
		fp.Open(g_test_config.ConvertAbsolutePath(serial_buf),CFile::modeRead);
		if(fp.m_hFile == INVALID_HANDLE_VALUE)
		{
			AfxMessageBox(_T("no the file"), MB_OK);
			return FALSE;
		}
		len = fp.GetLength();
		if (len == 0)
		{
			AfxMessageBox(_T("��ȡuid�ļ����ݵĳ���Ϊ0"), MB_OK);
			return FALSE;
		}
		char* buff=new char[len + 4];
		memset(buff,0,len + 4);
		fp.Read(buff,len);
		fp.Close();

		//UID��uid data����һ��BUf	
		uid_len = strlen(T2A(serial_temp));
		uid_buf = (unsigned char *)malloc(uid_len + 4 + len + 4 + 1);
		memset(uid_buf, 0, uid_len + 4 + len + 4 + 1);
	
		memcpy(&uid_buf[idex], &uid_len, sizeof(UINT));
		idex += sizeof(UINT);
		memcpy(&uid_buf[idex], T2A(serial_temp), uid_len);
		idex += uid_len;
		memcpy(&uid_buf[idex], &len, sizeof(UINT));
		idex += sizeof(UINT);
		memcpy(&uid_buf[idex], buff, len);
		idex += len;

		//str.Format(_T("uid_buf:%02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x ")
		//	,uid_buf[0],  uid_buf[1],uid_buf[2],uid_buf[3],uid_buf[4],uid_buf[5],uid_buf[6],uid_buf[7],uid_buf[8],uid_buf[9],uid_buf[10],uid_buf[11],uid_buf[12],uid_buf[13],uid_buf[14],uid_buf[15]);
		//AfxMessageBox(str, MB_OK);

		if (!Send_cmd(TEST_COMMAND, 1, name_buf, (char *)uid_buf, idex, 0))
		{
			delete[] buff;
			free(uid_buf);
			g_send_commad[0] = 0;
			AfxMessageBox(_T("������¼UID����ʧ��"), MB_OK);
			return FALSE;
		}	
		delete[] buff;
		free(uid_buf);

	}
	else
	{
		len = _tcslen(g_test_config.m_uid_number);
		memset(serial_temp, 0, (MAX_PATH+1)*sizeof(TCHAR));
		_tcsncpy(serial_temp, &g_test_config.m_uid_number[len - g_test_config.uid_download_len], g_test_config.uid_download_len);

		if (!Send_cmd(TEST_COMMAND, 1, name_buf, T2A(serial_temp),strlen(T2A(serial_temp)), 0))
		{
			AfxMessageBox(_T("������¼UID����ʧ��"), MB_OK);
			return FALSE;
		}

	}




#if 0
	//�����ļ�
	memset(DestName_temp, 0, MAX_PATH);
	_tcsncpy(DestName_temp, DestName, sizeof(DestName));
	strSourceName.Format(_T("%s"), g_test_config.ConvertAbsolutePath(test_Name));
	_tcscat(DestName_temp, buf_uid_name);

	strDestName.Format(_T("%s"), DestName_temp);
	if(0)//!no_put_flie_flag )
	{
		if (m_pFtpConnection[0] == NULL || !m_pFtpConnection[0]->PutFile(strSourceName, 
			strDestName, FTP_TRANSFER_TYPE_BINARY, 1))   
		{
			m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
			m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
			m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
			g_send_commad[0] = 0;
			no_put_flie_flag  = true;
			AfxMessageBox(_T("Error auto test putting file"));
			return FALSE;
		}
	}
	no_put_flie_flag = false;

	//�Ȱ����кŵ��ļ�ͨ��ftp����С���ϡ�
	if (!Download_serialfile_toftp())
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		g_send_commad[0] = 0;
		AfxMessageBox(_T("�������к��ļ�ʧ�ܣ�����"));
		return FALSE;
	}
	

	name_len = strlen(T2A(buf_uid_name));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(buf_uid_name), name_len);

	memset(test_param, 0, MAX_PATH);
	_tcscpy(test_param, g_test_config.serial_file_name);

	Sleep(100);
	g_test_pass_flag[0] = 0;
	if (!Send_cmd(TEST_COMMAND, 1, name_buf, T2A(test_param), 0))
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		g_send_commad[0] = 0;
		AfxMessageBox(_T("Error Send_cmd file"));
		return FALSE;
	}

	g_test_config.config_uid_test_pass = TRUE;

	memset(burnUid, 0, MAX_PATH);
	len = _tcslen(g_test_config.m_uid_number);
	_tcsncpy(burnUid, &g_test_config.m_uid_number[len - g_test_config.uid_download_len], g_test_config.uid_download_len);

#endif

	//���շ���ֵ
	if (!Anyka_Test_UID_check_info(30000))
	{
		//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
		//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		g_send_commad[0] = 0;
		//AfxMessageBox(_T("UID��¼ʧ��"), MB_OK);

		frmLogUidFile.WriteLogFile(0,"UID:%s, NG\r\n",T2A(burnUid));
		return FALSE;
	}
#if 1
	//ͨ��FTP��ȡetc/jffs2�����ͬ���ļ�������PC�Ͻ��бȽ�
	if (g_test_config.uid_download_mode == 1)
	{
		memset(serial_temp, 0, MAX_PATH+1);
		len = _tcslen(g_test_config.m_uid_number);
		_tcsncpy(serial_temp, &g_test_config.m_uid_number[len - g_test_config.uid_download_len], g_test_config.uid_download_len);
		swprintf(serial_buf, _T("%s/%s.conf"), g_test_config.ConvertAbsolutePath(SERAIL_CONFIG), serial_temp);
		swprintf(dstfileName, _T("%s/%s.conf"), g_test_config.ConvertAbsolutePath(SERAIL_CONFIG_BAK), serial_temp);

		if (MoveFile(serial_buf, dstfileName) == 0)
		{
			AfxMessageBox(_T("�ƶ��ļ�ʧ��,���Ա����ļ������Ѵ��ڴ��ļ�,����"));   
			return FALSE;
		}
		if(0) //(!get_and_compare_serialfile_toftp())
		{
			m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
			m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
			m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
			g_send_commad[0] = 0;
			frmLogUidFile.WriteLogFile(0,"UID:%s, NG\r\n",T2A(burnUid));
			return FALSE;
		}
	}
#endif

	//MessageBox(_T("UID��¼�ɹ�"), MB_OK);

	frmLogUidFile.WriteLogFile(0,"UID:%s, OK\r\n",T2A(burnUid));
	if (g_test_config.uid_download_mode == 1)
	{
		m_CBurnUidDlg.WriteUid_MAC();
	}
	else
	{
		m_CBurnUidDlg.WriteUid();
	}
	
	//m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
	//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
	//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
	g_send_commad[0] = 0;
	g_test_pass_flag[0] = 0;


	return TRUE;

}

BOOL CAnykaIPCameraDlg::enter_case_uid(UINT *burn_flag)
{	
	UINT len = 0;
	UINT i = 0;

	len = _tcslen(g_test_config.m_last_config[current_ip_idex].Current_IP_UID);
	for (i = 0; i < len; i++)
	{
		if (g_test_config.m_last_config[current_ip_idex].Current_IP_UID[i] != 0)
		{
			break;
		}
	}
	if ( i != len)
	{
		if (IDOK != AfxMessageBox(_T("���豸�Ѵ�����UID���Ƿ�ȷ�ϼ���д��uid��"), MB_OKCANCEL))
		{
			no_burn_uid =TRUE;
			*burn_flag = 1;
			return TRUE;
		}
	}
	no_burn_uid =FALSE;



	g_uid_running = TRUE;
	if (IDOK == m_Burn_UIdDlg.DoModal())
	{
		if (case_uid())
		{
			g_uid_running = FALSE;
			*burn_flag = 0;
			return TRUE;
		}
		else
		{
			g_uid_running = FALSE;
			return FALSE;
		}
		no_burn_uid =FALSE;
		
	}
	no_burn_uid =TRUE;
	g_uid_running = FALSE;
	*burn_flag = 1;
	return TRUE;

}

void  CAnykaIPCameraDlg::show_test_info(BOOL end_flag)
{
	CString str;
	CRect rcDlg1;
	GetClientRect(&rcDlg1);
	BOOL test_fail = FALSE;
	BOOL is_test_flag = FALSE;
	

	
	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->SetFont(&font);
	//font.DeleteObject();

	str.Format(_T(""));
	if (!g_test_config.config_video_test_pass && g_test_config.config_video_enable)
	{
		test_fail = TRUE;
		str += _T("ͼ��ģ�飬");
	}
	
	if(!g_test_config.config_ircut_onoff_test_pass && g_test_config.config_ircut_enable)
	{
		test_fail = TRUE;
		str += _T("IRCUT���͹أ�");
	}
	/*
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
	*/
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

	if(g_test_config.config_video_enable 
		|| g_test_config.config_ircut_enable
		|| g_test_config.config_voice_rev_enable
		|| g_test_config.config_voice_send_enable
		|| g_test_config.config_head_enable
		|| g_test_config.config_sd_enable
		|| g_test_config.config_wifi_enable
		|| g_test_config.config_reset_enable)
	{
		is_test_flag = true;
	}
	
	if(is_test_flag)
	{
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
	}

	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_HIDE);

	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->SetWindowText(str);

	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_SHOW);

	m_RightDlg.UpdateWindow();
	
}

#if 1
void  CAnykaIPCameraDlg::ChangeTestItem(int case_idex)
{

	/*oldFont.CreateFont(1,1,0,0,FW_HEAVY,FALSE,FALSE,
        FALSE,GB2312_CHARSET,OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
        FIXED_PITCH|FF_MODERN, _T("����"));
	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->SetFont(&oldFont);*/


#if 0	
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
	case CASE_IRCUT_ONOFF:
		if (g_test_config.config_ircut_enable)
		{
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->ShowWindow(SW_HIDE);
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_HIDE);

			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->SetWindowText(L"");
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->SetWindowText(L"");
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->SetWindowText(L"IR-cut���ܿ�������");
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->SetWindowText(L"��Ƶ��ɫ�Ƿ��Ⱥڰ�,��ת��ɫ��������Ƿ�ȫ����");

			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->ShowWindow(SW_SHOW);
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_SHOW);

			m_RightDlg.UpdateWindow();
		}

		break;
		/*
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
		*/
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

			m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("��¼UID(Enter)"));//���� 
			m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->SetWindowText(_T("����¼UID(�ո�)"));//����


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

			m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("��¼MAC(Enter)"));//���� 
			m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->SetWindowText(_T("����¼MAC(�ո�)"));//���� 


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
			m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("ͨ��(Enter)"));//���� 
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
#endif
}

#endif

BOOL CAnykaIPCameraDlg::Send_cmand(UINT case_idex)
{

	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T(""));
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(g_test_config.config_video_enable && case_idex == CASE_VIDEO)
	{
		//��������
	}
	else if(g_test_config.config_ircut_enable && case_idex == CASE_IRCUT_ONOFF)
	{
		//��������
		case_ircut_onoff_test();
	}
	/*
	else if(g_test_config.config_ircut_enable && case_idex == CASE_IRCUT_ON)
	{
		//��������
		case_ircut_on();
	}
	else if(g_test_config.config_ircut_enable && case_idex == CASE_IRCUT_OFF)
	{
		//��������
		case_ircut_off();
	}
	*/
	else if(g_test_config.config_voice_rev_enable && case_idex == CASE_MONITOR)
	{
		//��������
		close_minitor();
		Sleep(1000);
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
		//case_head();
		Auto_move("6");
	}
	else if(g_test_config.config_sd_enable && case_idex == CASE_SD)
	{
		//��������
		if (!case_sd())
		{
			g_sd_test_pass = FALSE;
		}
		else
		{
			g_sd_test_pass = TRUE;
		}
		
	}
	else if(g_test_config.config_wifi_enable && case_idex == CASE_WIFI)
	{
		//��������
		if (!case_wifi())
		{
			g_wifi_test_pass = FALSE;
		}
		else
		{
			g_wifi_test_pass = TRUE;
		}
	}
	else if(g_test_config.config_video_enable && case_idex == CASE_VIDEO)
	{

	}
	/*else if(g_test_config.config_red_line_enable && case_idex == CASE_INFRARED)
	{
	}*/
	else if(g_test_config.config_reset_enable && case_idex == CASE_RESET)
	{
		if (!case_rev())
		{
			g_reset_test_pass = FALSE;
		}
		else
		{
			g_reset_test_pass = TRUE;
		}
	}



	return TRUE;
}

BOOL CAnykaIPCameraDlg::The_first_case()
{
	UINT burn_flag = 0;
	BOOL pass_flag = FALSE;

	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (g_test_config.config_video_enable)
	{
		g_case_idex = CASE_VIDEO;
		ChangeTestItem(g_case_idex);
	}
	else if(g_test_config.config_ircut_enable)
	{
		//g_case_idex = CASE_IRCUT_ON;
		g_case_idex = CASE_IRCUT_ONOFF;
		ChangeTestItem(g_case_idex);
		//��������
		//case_ircut_on();
		case_ircut_onoff_test();
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
		//case_head();
		Auto_move("6");
	}
	else if(g_test_config.config_sd_enable)
	{
		g_case_idex = CASE_SD;
		ChangeTestItem(g_case_idex);
		//��������
		if (!case_sd())
		{
			g_sd_test_pass = FALSE;
		}
		else
		{
			g_sd_test_pass = TRUE;
		}
		
	}
	else if(g_test_config.config_wifi_enable)
	{
		g_case_idex = CASE_WIFI;
		ChangeTestItem(g_case_idex);
		//��������
		if (!case_wifi())
		{
			g_wifi_test_pass = FALSE;
		}
		else
		{
			g_wifi_test_pass = TRUE;
		}
	}
	else if(g_test_config.config_reset_enable)
	{
		g_case_idex = CASE_RESET;
		ChangeTestItem(g_case_idex);
		if (!case_rev())
		{
			g_reset_test_pass = FALSE;
		}
		else
		{
			g_reset_test_pass = TRUE;
		}
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("ͨ��(Enter)"));//���� 
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
#if 1
		if (enter_case_uid(&burn_flag))
		{
			if (burn_flag == 1)
			{
				//ȡ������UID
				GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("����¼UID"));//���� 
				write_test_info(FALSE, FALSE, g_case_idex);
			}
			else
			{
				GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("��¼UID�ɹ�"));//���� 
				write_test_info(TRUE, TRUE, g_case_idex);
				pass_flag = TRUE;
			}
		}
		else
		{
			GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("��¼UIDʧ��"));//���� 
			write_test_info(FALSE, TRUE, g_case_idex);
		}
		g_case_idex++;
		find_next_idex();
		ChangeTestItem(g_case_idex);
		if (g_case_idex > CASE_MAC)
		{
			finish_test(pass_flag);
			m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("�������(Enter)"));//����  
			m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->ShowWindow(SW_HIDE);
			g_test_finish_flag = true;
		}
		g_case_idex--;

#endif

	}
	else if(g_test_config.config_lan_mac_enable)
	{
		g_case_idex = CASE_MAC;
		ChangeTestItem(g_case_idex);
#if 1
		if (case_mac())
		{
			GetDlgItem(IDC_STATIC_TEST_POINT2)->SetWindowText(_T("��¼MAC�ɹ�"));//���� 
			write_test_info(TRUE, TRUE, g_case_idex);
			pass_flag = TRUE;
		}
		else
		{
			GetDlgItem(IDC_STATIC_TEST_POINT2)->SetWindowText(_T("��¼MACʧ��"));//���� 
			write_test_info(FALSE, TRUE, g_case_idex);
		}

		g_case_idex++;
		finish_test(pass_flag);
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("�������(Enter)"));//����  
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->ShowWindow(SW_HIDE);
		g_test_finish_flag = true;
#endif
	}
	else
	{
		AfxMessageBox(_T("û����Ӧ�����ã�����"), MB_OK);
		return FALSE;
	}

	return TRUE;
}

BOOL CAnykaIPCameraDlg::close_test()
{
	CString str;

	g_test_config.test_num++;
	str.Format(_T("%d"), g_test_config.test_num);
	m_BottomDlg.GetDlgItem(IDC_EDIT_BURN_NUM)->SetWindowText(str);

	start_flag = FALSE;
	
	CloseServer(0);
	g_test_pass_flag[0] = 2;
	m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(FALSE);//����
	//m_BottomDlg.GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(FALSE);//����
	m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//����
	m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����
	m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
	//m_BottomDlg.GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(FALSE);//����
	g_pre_flag = FALSE;
	return true;
}


BOOL CAnykaIPCameraDlg::finish_test(BOOL passs_flag)
{
	

	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_TITLE)->SetWindowText(L"���Խ��");
	show_test_info(TRUE);

	if(!g_test_config.config_uid_enable) //)
	{
		m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("����Ҫ��¼UID"));	
	}
	else
	{
		if (g_test_config.config_uid_test_pass)
		{
			m_RightDlg.m_test_Status.SetFontColor(RGB(0,255,0));
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("��¼UID�����"));
		}
		else
		{
			if(no_burn_uid)
			{
				m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("����Ҫ��¼UID"));
			}
			else
			{
				m_RightDlg.m_test_Status.SetFontColor(RGB(255,0,0));
				m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("��¼UIDʧ��"));
			}
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
			m_RightDlg.m_test_2_Status.SetFontColor(RGB(0,255,0));
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT2)->SetWindowText(L"��¼MAC�����");
		}
		else
		{
	
			m_RightDlg.m_test_2_Status.SetFontColor(RGB(255,0,0));
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT2)->SetWindowText(_T("��¼MACʧ��"));
		}
	}
	
	return TRUE;

}
void CAnykaIPCameraDlg::find_next_idex()
{
	UINT i = 0;

	//��ȡ��һ��case_idex
	for (i = g_case_idex; i <= CASE_MAC; i++)
	{
		/*
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
		*/
		if ( i == CASE_IRCUT_ONOFF)
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
				//���ڿ���ʱ�ͽ��м����Ĳ��ԣ����Դ˴�����Ҫ�ٲ���
				g_case_idex++;
				//break;
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
				//����sd�ڿ�ʼ�ͽ��в����ˣ����Դ˴��������ٲ���
				g_case_idex++;
				//break;
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
				//����wifi�ڿ�ʼ�ͽ��в����ˣ����Դ˴��������ٲ���
				g_case_idex++;
				//break;
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

BOOL CAnykaIPCameraDlg::find_pre_idex()
{
	UINT i = 0;
	BOOL idex_flag = false;

	//��ȡ��һ��case_idex
	for (i = g_case_idex; i >=0; i--)
	{
		if ( i == CASE_VIDEO)
		{
			if(!g_test_config.config_video_enable) //
			{
				break;
			}
			else
			{
				idex_flag = true;
				break;
			}
		}
		else if ( i == CASE_IRCUT_ONOFF)
		{
			if(!g_test_config.config_ircut_enable) //
			{
				g_case_idex--;
			}
			else
			{
				idex_flag = true;
				break;
			}
		}
		/*
		else if ( i == CASE_IRCUT_ON)
		{
			if(!g_test_config.config_ircut_enable) //
			{
				g_case_idex--;
			}
			else
			{
				idex_flag = true;
				break;
			}
		}
		else if ( i == CASE_IRCUT_OFF)
		{
			if(!g_test_config.config_ircut_enable) //
			{
				g_case_idex--;
			}
			else
			{
				idex_flag = true;
				break;
			}
		}
		*/
		else if( i == CASE_MONITOR)
		{
			if(!g_test_config.config_voice_rev_enable) //
			{
				g_case_idex--;
			}
			else
			{
				g_case_idex--;
				//idex_flag = true;
				//break;
			}
		}
		else if( i == CASE_INTERPHONE)
		{
			if(!g_test_config.config_voice_send_enable) //
			{
				g_case_idex--;
			}
			else
			{
				idex_flag = true;
				break;
			}
		}
		else if( i == CASE_HEAD)
		{
			if(!g_test_config.config_head_enable) //
			{
				g_case_idex--;
			}
			else
			{
				idex_flag = true;
				break;
			}
		}
		else if( i == CASE_SD)
		{
			if(!g_test_config.config_sd_enable) //
			{
				g_case_idex--;
			}
			else
			{
				g_case_idex--;
				//idex_flag = true;
				//break;
			}
		}
		else if( i == CASE_WIFI)
		{
			if(!g_test_config.config_wifi_enable) //
			{
				g_case_idex--;
			}
			else
			{
				g_case_idex--;
				//idex_flag = true;
				//break;
			}
		}
		else if( i == CASE_RESET)
		{
			if(!g_test_config.config_reset_enable) //
			{
				g_case_idex--;
			}
			else
			{
				idex_flag = true;
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
				g_case_idex--;
			}
			else
			{
				idex_flag = true;
				break;
			}
		}
		else if( i == CASE_MAC)
		{
			if(!g_test_config.config_lan_mac_enable) //
			{
				g_case_idex--;
			}
			else
			{
				idex_flag = true;
				break;
			}
		}

	}

	if(idex_flag)
	{
		return true;
	}
	return false;

}

void CAnykaIPCameraDlg::pre_case_main(BOOL pass_flag)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BOOL case_flag = FALSE;
	UINT i = 0, burn_flag = 0;
	CString str;

	
	//g_monitor_end_flag = TRUE;

	//str.Format(_T("3:%d"), g_case_idex);

	//AfxMessageBox(str, MB_OK);
	//�������
	if (g_case_idex == 0)
	{
		str.Format(_T("���ǵ�һ����������"));
	    AfxMessageBox(str, MB_OK);
		return;
	}

	g_case_idex--;

	if(!find_pre_idex())
	{
		str.Format(_T("���ǵ�һ����������"));
	    AfxMessageBox(str, MB_OK);
		return;
	}

	//��ʾ��һ��
	ChangeTestItem(g_case_idex);

	if (g_case_idex == CASE_UID)
	{
		if(pass_flag)
		//if (g_test_config.config_uid_enable)
		{
			if (enter_case_uid(&burn_flag))
			{
				if (burn_flag == 1)
				{
					//ȡ������UID
					m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("����¼UID"));//����
					write_test_info(FALSE, FALSE, g_case_idex);
				}
				else
				{
					m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("��¼UID�ɹ�"));//����
					write_test_info(TRUE, TRUE, g_case_idex);
				}
			}
			else
			{
				m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("��¼UIDʧ��"));//����
				write_test_info(FALSE, TRUE, g_case_idex);
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
			m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("�������(Enter)"));//����  
			m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->ShowWindow(SW_HIDE);
			g_test_finish_flag = true;
		}
		g_case_idex--;
	}
	else if(g_case_idex == CASE_MAC)
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
				write_test_info(TRUE, TRUE, g_case_idex);
			}
			else
			{
				m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT2)->SetWindowText(_T("��¼MACʧ��"));//���� 
				write_test_info(FALSE, TRUE, g_case_idex);
			}
		} 
		else
		{
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT2)->SetWindowText(_T("����¼MAC"));//���� 
			write_test_info(FALSE, TRUE,g_case_idex);
		}
		g_case_idex++;
		find_next_idex();
		ChangeTestItem(g_case_idex);

		end_test=TRUE;
		finish_test(pass_flag);
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("�������(Enter)"));//����  
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->ShowWindow(SW_HIDE);
		g_test_finish_flag = true;
		g_case_idex--;
		//GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("ͨ��"));//���� 
		//GetDlgItem(IDC_BUTTON_FAILED)->SetWindowText(_T("��ͨ��"));//���� 
	}	
	else
	{
		//Sleep(1000);
		//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("ͨ��(Enter)"));//���� 
		//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->SetWindowText(_T("��ͨ��(�ո�)"));//���� 
		//������һ������
		//Send_cmand(g_case_idex);

		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("ͨ��(Enter)"));//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->SetWindowText(_T("��ͨ��(�ո�)"));//���� 
		//������һ������
		m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(FALSE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->ShowWindow(FALSE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->ShowWindow(FALSE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->ShowWindow(FALSE);//����

		Send_cmand(g_case_idex);

		if (g_test_config.config_reset_enable && g_case_idex == CASE_RESET)
		{
		}
		else
		{
			if (!g_reset_test_pass && g_test_config.config_reset_enable && g_case_idex == CASE_RESET)
			{
				g_reset_test_pass = TRUE;
			}
			else if (!g_sd_test_pass && g_test_config.config_sd_enable && g_case_idex == CASE_SD)
			{
				g_sd_test_pass = TRUE;
			}
			else if (!g_wifi_test_pass && g_test_config.config_wifi_enable && g_case_idex == CASE_WIFI)
			{
				g_wifi_test_pass = TRUE;
			}
			else
			{
				m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
				m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->ShowWindow(SW_SHOW);//���� 
			}

			m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
			m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
			m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->ShowWindow(SW_SHOW);//����
			m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->ShowWindow(SW_SHOW);//����
		}
	}

}


BOOL  CAnykaIPCameraDlg::check_test_false(void)
{
	BOOL test_fail = FALSE;

	if (!g_test_config.config_video_test_pass && g_test_config.config_video_enable)
	{
		test_fail = TRUE;
	}
	if(!g_test_config.config_ircut_onoff_test_pass && g_test_config.config_ircut_enable)
	{
		test_fail = TRUE;
	}
	/*
	if(!g_test_config.config_ircut_on_test_pass && g_test_config.config_ircut_enable)
	{
		test_fail = TRUE;
	}
	if(!g_test_config.config_ircut_off_test_pass && g_test_config.config_ircut_enable)
	{
		test_fail = TRUE;
	}
	*/
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



void CAnykaIPCameraDlg::case_main(BOOL pass_flag)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BOOL case_flag = FALSE;
	UINT i = 0, burn_flag = 0;
	CString str;

	m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(true);//����

	//g_monitor_end_flag = TRUE;

	g_pre_flag = TRUE;
	if(g_test_finish_flag)
	{
		g_test_finish_flag = false;
		end_test = false;
		close_test();
		return;
	}


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
		write_test_info(pass_flag, TRUE, g_case_idex);
		//str.Format(_T("1:%d"), g_case_idex);

		//AfxMessageBox(str, MB_OK);
		if (end_test != TRUE)
		{
			//str.Format(_T("2:%d"), g_case_idex);

			//AfxMessageBox(str, MB_OK);
			g_case_idex++;	
		}
	}
	else
	{
		if (end_test != TRUE)
		{
			//str.Format(_T("2:%d"), g_case_idex);

			//AfxMessageBox(str, MB_OK);
			g_case_idex++;	
		}
		write_test_info(pass_flag, TRUE, g_case_idex);
	}
	find_next_idex();

	//str.Format(_T("3:%d"), g_case_idex);

	//AfxMessageBox(str, MB_OK);
	//�������
	if (g_case_idex >= CASE_MAC+1)
	{
		finish_test(pass_flag);
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->ShowWindow(TRUE);//����  
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//����  
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("�������(Enter)"));//����  
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->ShowWindow(SW_HIDE);
		g_test_finish_flag = true;
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
		no_burn_uid = TRUE;
		finish_test(FALSE);
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->ShowWindow(TRUE);//����  
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("�������(Enter)"));//����  
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->ShowWindow(SW_HIDE);
		g_test_finish_flag = true;
		return;
	}

	if (g_case_idex == CASE_UID)
	{
		if(1)//pass_flag)
		{
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetFont(&font);
			if (enter_case_uid(&burn_flag))
			{
				if (burn_flag == 1)
				{
					//ȡ������UID
					m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("����¼UID"));//����
					write_test_info(FALSE, FALSE, g_case_idex);
				}
				else
				{
					m_RightDlg.m_test_Status.SetFontColor(RGB(0,255,0));
					m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("��¼UID�ɹ�"));//����
					write_test_info(TRUE, TRUE, g_case_idex);
				}
			}
			else
			{
				m_RightDlg.m_test_Status.SetFontColor(RGB(255,0,0));
				m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("��¼UIDʧ��"));//����
				write_test_info(FALSE, TRUE, g_case_idex);
				
			}
		}
		else
		{
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("����¼UID"));//���� 
		}
		g_case_idex++;
		find_next_idex();
		ChangeTestItem(g_case_idex);

		//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("ͨ��"));//���� 
		//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->SetWindowText(_T("��ͨ��"));//���� 

		if (g_case_idex > CASE_MAC)
		{
			finish_test(pass_flag);
			m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->ShowWindow(TRUE);//����  
			m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
			m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("�������(Enter)"));//����  
			m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->ShowWindow(SW_HIDE);
			g_test_finish_flag = true;
		}
		g_case_idex--;
	}
	else if(g_case_idex == CASE_MAC)
	{
		//m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_HIDE);
		//show_test_info(FALSE);
		//m_RightDlg.GetDlgItem(IDC_STATIC_TEST_CONTENT)->ShowWindow(SW_SHOW);
		//m_RightDlg.UpdateWindow();

		if (pass_flag)
		{
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT2)->SetFont(&font);
			
			if (case_mac())
			{
				m_RightDlg.m_test_2_Status.SetFontColor(RGB(0,255,0));
				m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT2)->SetWindowText(_T("��¼MAC�ɹ�"));//���� 
				write_test_info(TRUE, TRUE, g_case_idex);
			}
			else
			{
				m_RightDlg.m_test_2_Status.SetFontColor(RGB(255,0,0));
				m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT2)->SetWindowText(_T("��¼MACʧ��"));//���� 
				write_test_info(FALSE, TRUE, g_case_idex);
			}
		} 
		else
		{
			m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT2)->SetWindowText(_T("����¼MAC"));//���� 
			write_test_info(FALSE, FALSE, g_case_idex);
		}
		g_case_idex++;
		//find_next_idex();
		//ChangeTestItem(g_case_idex);

		finish_test(pass_flag);
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->ShowWindow(TRUE);//����  
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("�������(Enter)"));//����  
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->ShowWindow(SW_HIDE);
		g_test_finish_flag = true;
		g_case_idex--;
		//m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("ͨ��"));//���� 
		//m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->SetWindowText(_T("��ͨ��"));//���� 

		//if (g_case_idex > CASE_MAC+1)
		//{
			
			/*OnBnClickedButtonNext();
			OnBnClickedButtonStart();*/
		//}
		//GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("ͨ��"));//���� 
		//GetDlgItem(IDC_BUTTON_FAILED)->SetWindowText(_T("��ͨ��"));//���� 
	}	
//	else if(g_case_idex == CASE_MAC+1)
//	{
//		g_case_idex++;
//		end_test=TRUE;
//		finish_test(pass_flag);
//	}
	else
	{
		//Sleep(1000);
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->SetWindowText(_T("ͨ��(Enter)"));//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->SetWindowText(_T("��ͨ��(�ո�)"));//���� 
		//������һ������
		m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(FALSE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->ShowWindow(FALSE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->ShowWindow(FALSE);//���� 
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->ShowWindow(FALSE);//����

		Send_cmand(g_case_idex);

		if (g_test_config.config_reset_enable && g_case_idex == CASE_RESET)
		{
		}
		else
		{
			if (!g_reset_test_pass && g_test_config.config_reset_enable && g_case_idex == CASE_RESET)
			{
				g_reset_test_pass = TRUE;
			}
			else if (!g_sd_test_pass && g_test_config.config_sd_enable && g_case_idex == CASE_SD)
			{
				g_sd_test_pass = TRUE;
			}
			else if (!g_wifi_test_pass && g_test_config.config_wifi_enable && g_case_idex == CASE_WIFI)
			{
				g_wifi_test_pass = TRUE;
			}
			else
			{
				m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
				m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->ShowWindow(SW_SHOW);//���� 
			}

			m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
			m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
			m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->ShowWindow(SW_SHOW);//����
			m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->ShowWindow(SW_SHOW);//����
			
		}
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
		CloseServer(0);
		m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(FALSE);//����
		//m_BottomDlg.GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(FALSE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//����
		m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
		//m_BottomDlg.GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(FALSE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(TRUE);//����
		start_test = FALSE;
		start_flag = FALSE;
		return;
	}
	g_send_commad[0] = 1;

	m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(FALSE);//����
	m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//���� 
	m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����

	memset(test_Name, 0,  MAX_PATH);
	_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
	_tcscat(test_Name, _T("//"));
	_tcscat(test_Name, file_name);

	//�ж��ļ��Ƿ����
	if(0)//0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(test_Name)))
	{
		g_send_commad[0] = 0;
		str.Format(_T("%s no exist,������С��"), test_Name);
		AfxMessageBox(str, MB_OK); 
		m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
		CloseServer(0);
		return;
	}

	//�����ļ�
	memset(DestName_temp, 0, MAX_PATH);
	_tcsncpy(DestName_temp, DestName, sizeof(DestName));
	strSourceName.Format(_T("%s"), ConvertAbsolutePath(test_Name));
	_tcscat(DestName_temp, file_name);
	strDestName.Format(_T("%s"), DestName_temp);

	if (0)//!download_dev_file_flag)
	{
		if (!m_pFtpConnection[0]->PutFile(strSourceName, strDestName, FTP_TRANSFER_TYPE_BINARY, 1))   
		{
			AfxMessageBox(_T("Error no auto test putting file,������С��"), MB_OK);  
			download_dev_file_flag = FALSE;
			m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
			g_send_commad[0] = 0;
			CloseServer(0);
			return;
		}
		download_dev_file_flag = TRUE;
	}


	//��������
	name_len = strlen(T2A(file_name));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(file_name), name_len);

	g_test_pass_flag[0] = 0;
	if (!Send_cmd(TEST_COMMAND, 1, name_buf, NULL, 0, 0))
	{
		AfxMessageBox(_T("Send_cmd��fail "), MB_OK); 
		m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
		CloseServer(0);
		g_send_commad[0] = 0;
		return ;
	}

	//���շ���ֵ
	if (!Anyka_Test_check_info(10000))
	{
		AfxMessageBox(_T("����ʧ��"), MB_OK);
		m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
		CloseServer(0);
		g_send_commad[0] = 0;
		return ;
	}
	g_test_pass_flag[0] = 0;

	if (!Send_cmd(TEST_COMMAND_FINISH, 1, NULL, NULL, 0, 0))
	{
		AfxMessageBox(_T("���³ɹ�, С���Զ�����ʧ�ܣ����ֶ�����"), MB_OK);  
	}
	else
	{
		if (!Anyka_Test_check_info(10000))
		{
			AfxMessageBox(_T("���³ɹ�, С���Զ�����ʧ�ܣ����ֶ�����"), MB_OK);  
		}
		else
		{
			MessageBox(_T("���óɹ�"), MB_OK);
		}
	}
	g_test_pass_flag[0] = 0;
	g_send_commad[0] = 0;

	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CloseServer(0);
	//GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(FALSE);//����
	m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(FALSE);//����
	//m_BottomDlg.GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(FALSE);//����
	m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);//����
	m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);//����
	m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
	//m_BottomDlg.GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(FALSE);//����
	m_BottomDlg.GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(TRUE);//����
	start_test = FALSE;
	start_flag = FALSE;
	return;
}



void CAnykaIPCameraDlg::write_test_info(BOOL pass_flag, BOOL Isburn_flag, UINT case_idex)
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
	/*
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
	*/
	else if (case_idex == CASE_IRCUT_ONOFF)
	{
		if(g_test_config.config_ircut_enable)
		{
			if (pass_flag)
			{
				g_test_config.config_ircut_onoff_test_pass = TRUE;
				frmLogfile.WriteLogFile(0,"IRCUT���ܹرղ���ͨ��\r\n");
			}
			else
			{
				g_test_config.config_ircut_onoff_test_pass = FALSE;
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
			TCHAR burnUid[MAX_PATH+1] = {0};
			UINT len = 0;
			USES_CONVERSION;
			if (pass_flag)
			{
				g_test_config.config_uid_test_pass = TRUE;

				memset(burnUid, 0, MAX_PATH);
				len = _tcslen(g_test_config.m_uid_number);
				_tcsncpy(burnUid, &g_test_config.m_uid_number[len - g_test_config.uid_download_len], g_test_config.uid_download_len);
				
				frmLogfile.WriteLogFile(0,"UID burn pass,UID:%s\r\n", T2A(burnUid));
				//frmLogUidFile.WriteLogFile(0,"UID:%s, OK\r\n", T2A(burnUid));
			}
			else
			{
				if (Isburn_flag)
				{
					memset(burnUid, 0, MAX_PATH);
					len = _tcslen(g_test_config.m_uid_number);
					_tcsncpy(burnUid, &g_test_config.m_uid_number[len - g_test_config.uid_download_len], g_test_config.uid_download_len);

					frmLogfile.WriteLogFile(0,"UID burn fail,UID:%s\r\n", T2A(burnUid));
					//frmLogUidFile.WriteLogFile(0,"UID:%s, NG\r\n", T2A(burnUid));
				}
				else
				{
					frmLogfile.WriteLogFile(0,"no burn UID\r\n");
					//frmLogfile.WriteLogFile(0,"no burn UID\r\n");
				}
				g_test_config.config_uid_test_pass = FALSE;
				
			}
		}
	}
	else if (case_idex == CASE_MAC)
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
		//start_test = FALSE;
		AfxMessageBox(_T("�������һ̨��Ŷ"), MB_OK);
		//start_test = TRUE;
		current_ip_idex--;
		next_test_flag = TRUE;
		return;
	}
	else
	{
		m_RightDlg.GetDlgItem(IDC_STATIC_SD_INFO)->SetWindowText(_T(""));
		m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T(""));//���� 
	//	m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT2)->SetWindowText(_T(""));
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
		g_test_finish_endrtsp_flag = false;
		g_start_open_flag = TRUE;	
		len = _tcsclen(g_test_config.m_last_config[current_ip_idex].Current_IP_address_buffer);
		memset(m_connect_ip, 0, MAX_PATH);
		_tcsncpy(m_connect_ip, g_test_config.m_last_config[current_ip_idex].Current_IP_address_buffer, len);
		memset(m_connect_uid, 0, MAC_ADDRESS_LEN);
		len = _tcsclen(g_test_config.m_last_config[current_ip_idex].Current_IP_UID);
		_tcsncpy(m_connect_uid, g_test_config.m_last_config[current_ip_idex].Current_IP_UID, len);
		find_IP_CloseServer(0);
		g_test_monitor_flag = TRUE;
		m_RightDlg.SetDlgItemText(IDC_EDIT_IP, g_test_config.m_last_config[current_ip_idex].Current_IP_address_buffer);
		m_RightDlg.SetDlgItemText(IDC_EDIT_UID, g_test_config.m_last_config[current_ip_idex].Current_IP_UID);
		m_RightDlg.SetDlgItemText(IDC_EDIT_MAC, g_test_config.m_last_config[current_ip_idex].Current_IP_MAC);
		m_RightDlg.SetDlgItemText(IDC_EDIT_VERSION2, g_test_config.m_last_config[current_ip_idex].Current_IP_version);
		g_test_config.m_test_sd_flag = 0; //sd, 0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��
		g_test_config.m_test_wifi_flag = 0; //wifi,0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��
		g_test_config.m_test_reset_flag = 0; //0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��
		g_test_config.m_test_ircut_flag = 0; //0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��
		g_test_config.m_test_MAC_flag = 0; //0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��
		g_test_config.m_test_UID_flag = 0; //0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��
		g_test_config.m_test_speaker_flag = 0; //0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��
		g_test_config.m_test_monitor_flag = 0; //������0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��
		g_test_config.m_test_cloud_flag = 0; //��̨�� 0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��
		g_test_config.m_test_play_flag = 0; //�Խ���0��ʾû�в��ԣ�1��ɲ��Գɹ���2��ʾ����ʧ��
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

	if (nHotKeyId== HOTKEY_F1)
	{
		if(g_config_start_flag && !g_uid_running && !g_next_running && !g_reset_running)
		{
			OnBnClickedButtonConfigure();
		}
	}
	else if(nHotKeyId== HOTKEY_F2)
	{
		if(g_config_start_flag && !g_next_running && !g_reset_running)
		{
			g_uid_running = TRUE;
			OnBnClickedButtonWriteUid();
			g_uid_running = FALSE;

		}
	}
	else if(nHotKeyId== HOTKEY_F3)
	{
		if(!start_flag && !g_uid_running && !g_next_running && !g_reset_running)
		{
			start_flag = TRUE;
			no_put_flie_flag = false;
			g_test_finish_flag = false;
			OnBnClickedButtonStart();
		}
	}
	else if(nHotKeyId== HOTKEY_F4)
	{
		if(start_test && !g_uid_running && !g_next_running && !g_reset_running)
		{
			OnBnClickedButtonClose();
		}
	}
	else if(nHotKeyId== HOTKEY_F6)
	{
		if(!start_test && next_test_flag && !g_uid_running && !g_next_running && !g_reset_running)
		{
			g_next_running = TRUE;
			OnBnClickedButtonNext();
			g_next_running = FALSE;
		}
	}
	else if(nHotKeyId== HOTKEY_F7)
	{
		if(start_test && !g_uid_running && !g_next_running && !g_reset_running)
		{
			OnBnClickedButtonReset();
		}
	}
	/*
	else if(nHotKeyId ==  HOTKEY_FY || nHotKeyId== HOTKEY_Fy)//HOTKEY_FESCAPE)//
	{
		if(start_test && !g_uid_running && !g_next_running && !g_reset_running)
		{
			case_main(TRUE);
			m_RightDlg.m_test_pass_btn.SetFocus();
		}
	}
*/
	else if(nHotKeyId== HOTKEY_FNULL)
	{
		if(start_test && !g_uid_running && !g_test_finish_flag && !g_next_running && !g_reset_running)
		{
			case_main(FALSE);
			m_RightDlg.m_test_pass_btn.SetFocus();
		}
	}
	else if(nHotKeyId== HOTKEY_F11)
	{
		if(start_test && g_pre_flag && !g_uid_running && !g_next_running && !g_reset_running)
		{
			m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//����
			m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
			m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
			m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->ShowWindow(SW_SHOW);
			if(no_put_flie_flag)
			{
				no_put_flie_flag = false;
			}
			else
			{
				no_put_flie_flag = true;
			}
			g_test_finish_flag = false;
			pre_case_main(TRUE);
			m_RightDlg.m_test_pass_btn.SetFocus();
		}
	}
	else if(nHotKeyId== HOTKEY_F8 )
	{
		if(!start_flag && !g_uid_running && !g_next_running && !g_reset_running)
		{
			m_RightDlg.OnBnClickedButtonFindIp();
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

BOOL CAnykaIPCameraDlg::get_ssid_data(UINT index, CString subRight)
{
	CString str;
	int nPos;

	//�ж��Ƿ�Խ����
	if(index >= g_ssid_num)
	{
		return FALSE;
	}

	USES_CONVERSION;


	//total_size
	nPos = subRight.Find(';');
	if(nPos <= 0 )
	{
		return FALSE;
	}

	str = subRight.Left(nPos);
	subRight = subRight.Mid(nPos+1);

	str.TrimLeft();
	str.TrimRight();
	strcpy(g_ssid_info[index].wify_ssid_name, T2A(str));//spi�Ĳ�����������


	//total_size
	nPos = subRight.Find(':');
	if(nPos <= 0 )
	{
		return FALSE;
	}

	str = subRight.Left(nPos);
	subRight = subRight.Mid(nPos+1);

	subRight.TrimLeft();
	subRight.TrimRight();
	g_ssid_info[index].quality = atoi(T2A(subRight));////spi disk erase size��for linux

	return TRUE;
}



BOOL CAnykaIPCameraDlg::decode_file(TCHAR *file_name)
{
	CString str;
	BOOL ret = TRUE;
	DWORD read_len = 1;
	UINT ssid_num = 0;
	UINT ssid_idex = 0;


	//��ȡ�ļ�������
	if(0xFFFFFFFF == GetFileAttributes(file_name))
	{
		return FALSE;
	}

	USES_CONVERSION;

	//�������ļ�
	HANDLE hFile = CreateFile(file_name, GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, 0, NULL);
	if(INVALID_HANDLE_VALUE == hFile)
	{
		return FALSE;
	}

	//����һ��һ�ж�ȡ����
	while(read_len > 0)
	{
		int pos;
		CString subLeft, subRight;
		char ch = 0;
		char text[1024];
		int index = 0;

		while(read_len > 0 && ch != '\n')
		{
			ret = ReadFile(hFile, &ch, sizeof(char), &read_len, NULL);
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

		pos = str.Find(':');

		subLeft = str.Left(pos);
		subRight = str.Right(str.GetLength() - pos - 1);

		subLeft.TrimLeft();
		subLeft.TrimRight();
		subRight.TrimLeft();
		subRight.TrimRight();

		//project name
		if(_T("total") == subLeft)
		{
			g_ssid_num = atoi(T2A(subRight));//��BLOCKΪ��λ
			if(g_ssid_num == 0xFFFFFFFF)
			{
				CloseHandle(hFile);
				//str.Format(_T("ap_list.txt�ļ��д�����"));
				//AfxMessageBox(str, MB_OK); 
				return FALSE;
			}

			if(g_ssid_num > 0)
			{
				if(g_ssid_info)
				{
					delete g_ssid_info;
					g_ssid_info = NULL;
				}
				g_ssid_info = new T_SSID_INFO[g_ssid_num];
				memset(g_ssid_info, 0, 	ssid_num*sizeof(T_SSID_INFO));

			}
			
		}
		else if(_T("sdcard_size") == subLeft)
		{
			g_sd_size = atol(T2A(subRight));//��BLOCKΪ��λ
		}
		else if(_T("ssid") == subLeft.Left(4) && g_ssid_num > 0)
		{
			str = subLeft.Mid(4);
			str.TrimLeft();

			//��ȡspi�Ĳ�����ֵ
			if(ssid_idex >= 0 && ssid_idex < (int)g_ssid_num)
			{
				get_ssid_data(ssid_idex, subRight);
				ssid_idex++;
			}		
		}

	}

	CloseHandle(hFile);

	return TRUE;

}

BOOL CAnykaIPCameraDlg::download_file(TCHAR *file_name)
{
	CString strSourceName, strDestName;
	TCHAR test_Name[MAX_PATH] = {0};
	TCHAR DestName_temp[MAX_PATH] = {0};
	TCHAR DestName[50] =_T("/tmp/");
	TCHAR test_param[MAX_PATH];
	CString str;
	UINT name_len = 0, i = 0, temp = 0;
	char name_buf[MAX_PATH] = {0};

	USES_CONVERSION;

	memset(test_Name, 0,  MAX_PATH);
	temp = sizeof(TEST_CONFIG_DIR);
	_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
	_tcscat(test_Name, _T("/"));
	_tcscat(test_Name, file_name);

	//�ж��ļ��Ƿ����
	if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(test_Name)))
	{
		g_send_commad[0] = 0;
		str.Format(_T("sd���� %s no exist"), test_Name);
		AfxMessageBox(str, MB_OK); 
		return false;
	}

	//�����ļ�
	memset(DestName_temp, 0, MAX_PATH);
	temp = sizeof(DestName);
	_tcsncpy(DestName_temp, DestName, sizeof(DestName));
	strSourceName.Format(_T("%s"), ConvertAbsolutePath(test_Name));
	_tcscat(DestName_temp, file_name);
	strDestName.Format(_T("%s"), DestName_temp);
	
	//AfxMessageBox(_T("333333"), MB_OK);
	//m_pFtpConnection->Remove(strDestName);
	//AfxMessageBox(_T("44444"), MB_OK);

	//AfxMessageBox(_T("111111"), MB_OK);
	//if (!put_file_by_ftp(strSourceName, strDestName))   
	if (!download_file_thread(strSourceName, strDestName))
	{
		AfxMessageBox(_T("putting file fail"), MB_OK);  
		download_file_flag = FALSE;
		return FALSE;
	}
	//AfxMessageBox(_T("222222"), MB_OK);

	name_len = strlen(T2A(file_name));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(file_name), name_len);

	if (memcmp(name_buf, "test_wifi", name_len) == 0)
	{
		memset(test_param, 0, MAX_PATH);
		_tcscpy(test_param, g_test_config.m_wifi_name);

		if (!Send_cmd(TEST_COMMAND, 2, name_buf, T2A(test_param), strlen(T2A(test_param)), 0))
		{
			g_send_commad[0] = 0;
			AfxMessageBox(_T("Send_cmd fail "), MB_OK); 
			return false;
		}
	}
	else if (memcmp(name_buf, "test_monitor", name_len) == 0)
	{
		if (!Send_cmd(TEST_COMMAND, 2, name_buf, NULL, 0, 0))
		{
			g_send_commad[0] = 0;
			AfxMessageBox(_T("Send_cmd fail "), MB_OK); 
			return false;
		}
	}
	else
	{
		if (!Send_cmd(TEST_COMMAND, 2, name_buf, NULL, 0, 0))
		{
			g_send_commad[0] = 0;
			AfxMessageBox(_T("Send_cmd fail "), MB_OK); 
			return false;
		}
	}

	return true;
}


BOOL CAnykaIPCameraDlg::put_file_by_ftp(CString strSourceName, CString strDestName)
{
	CString str;
	UINT i = 0;

	if(m_pFtpConnection[0] == NULL)
	{
		AfxMessageBox(_T("putting file m_pFtpConnection null"), MB_OK);  
		return false;
	}

	if (!m_pFtpConnection[0]->PutFile(strSourceName, strDestName, FTP_TRANSFER_TYPE_BINARY|INTERNET_FLAG_RELOAD, 1))   
	{
		//str.Format(_T("error:%d"), GetLastError());
		//AfxMessageBox(str, MB_OK);  
		return false;
	}

	return true;
}



int CAnykaIPCameraDlg::test_pcm_buf(TCHAR *pcm_file_name) 
{
	int             cnt = 0;
	//HWAVEOUT        hwo;
	//WAVEHDR         wh;
	//WAVEFORMATEX    wfx;
	//HANDLE          wait;
	DWORD  high = 0;
	int file_len = 0,index = 0, will_len = 0;
	char *buf = NULL;
	BOOL ret = FALSE;
	DWORD read_len = 0;
	CPcmSpeaker ps; 
	HWAVEOUT hwo;

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


	hwo = open_pcm();

	play_pcm_buf(hwo, buf, index);
	close_pcm(hwo);
	CloseHandle(hFile);
	GlobalUnlock(buf);
	GlobalFree(hMem);
	return TRUE;
}


DWORD WINAPI download_thread(LPVOID lpParameter)
{
	CClientSocket m_ClientSocket;
	CAnykaIPCameraDlg ipc_dlg;

#if 0
	if(!ipc_dlg.download_file(g_download_filename))
	{
		ipc_dlg.close_download_thread();
		g_download_sd_wifi_flag = 2;
	}
#else
	//Sleep(1000);
	if(!ipc_dlg.put_file_by_ftp(g_src_filename, g_dst_filename))
	{
		ipc_dlg.close_download_thread();
		g_download_sd_wifi_flag = 2;
	}
	
#endif


	ipc_dlg.close_download_thread();
	g_download_sd_wifi_flag = 1;
	return 1;
}

void CAnykaIPCameraDlg::close_download_thread() 
{
	if(m_download_handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_download_handle);
		m_download_handle = INVALID_HANDLE_VALUE;
	}
}

//BOOL CAnykaIPCameraDlg::download_file_thread(TCHAR *pcm_file_name) 
BOOL CAnykaIPCameraDlg::download_file_thread(CString strSourceName, CString strDestName) 
{
	UINT time1 = 0, time2 = 0;
	bool ret = false;
	CString str;

	close_download_thread();
	//memset(g_download_filename, 0, MAX_PATH);
	//_tcsncpy(g_download_filename, pcm_file_name, _tcslen(pcm_file_name));

	//memset(g_src_filename, 0, MAX_PATH);
	g_src_filename = strSourceName;

	//memset(g_dst_filename, 0, MAX_PATH);
	g_dst_filename = strDestName;
	g_download_sd_wifi_flag = 0;

	m_download_handle = CreateThread(NULL, 0, download_thread, 0, 0, NULL);
	if (m_download_handle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	time1 = GetTickCount();
	while(1)
	{
		time2 = GetTickCount();
		if(time2 - time1 > 5000)
		{
			AfxMessageBox(_T("putting file timeout"), MB_OK);  
			ret = false;
			break;
		}

		if(g_download_sd_wifi_flag == 1)
		{
			ret = true;
			break;
		}
		else if(g_download_sd_wifi_flag == 2)
		{
			ret = false;
			break;
		}

		Sleep(100);
	}

	close_download_thread();

	return ret;
}


void Test_Wait_Thread(void *argv)
{
	CAnykaIPCameraDlg* pthis = (CAnykaIPCameraDlg*)argv;
	//pthis->m_RightDlg.m_test_pass_btn.SetFocus();

	//Sleep(500);

	//���շ���ֵ
	g_reset_test_finish = FALSE;
	g_reset_running = TRUE;
	pthis->m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetFont(&pthis->font);
	if (!pthis->Anyka_Test_check_info(30000))
	{
		g_reset_test_pass = FALSE;
		//AfxMessageBox(_T("����ʧ��"), MB_OK);
		pthis->m_RightDlg.m_test_Status.SetFontColor(RGB(255,0,0));
		pthis->m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("����ʧ��"));
		pthis->m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->ShowWindow(SW_SHOW);
		pthis->m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
		//pthis->m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
		pthis->m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
		pthis->m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->ShowWindow(TRUE);//����
		//pthis->m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->ShowWindow(TRUE);//���� 
		pthis->m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->ShowWindow(TRUE);//����
		//pthis->m_RightDlg.m_test_pass_btn.SetFocus();
		g_reset_test_finish = TRUE;
		g_reset_running = FALSE;
		g_send_commad[0] = 0;
		pthis->close_test_wait();
		pthis->Invalidate(TRUE);
		//Sleep(500);
		//case_main(FALSE);
		return ;
	}
	//MessageBox(_T("���óɹ�"), MB_OK);
	pthis->m_RightDlg.m_test_Status.SetFontColor(RGB(0,255,0));
	pthis->m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("���óɹ�"));
	pthis->m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->ShowWindow(SW_SHOW);

	pthis->m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(TRUE);//����
	pthis->m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);//���� 
	pthis->m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(TRUE);//����
	pthis->m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->ShowWindow(TRUE);//����
	pthis->m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->ShowWindow(TRUE);//���� 
	pthis->m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->ShowWindow(TRUE);//����
	//pthis->m_RightDlg.m_test_pass_btn.SetFocus();
	g_reset_test_finish = TRUE;
	g_reset_running = FALSE;
	g_test_pass_flag[0] = 0;
	g_send_commad[0] = 0;
	g_reset_test_pass = TRUE;

	pthis->close_test_wait();
	pthis->Invalidate(TRUE);

	return;
}

BOOL CAnykaIPCameraDlg::Create_Test_Wait_Data(void) 
{
	close_test_wait();
	g_test_wait_data = CreateThread(NULL, TRANS_STACKSIZE, (LPTHREAD_START_ROUTINE)Test_Wait_Thread, this, 0, NULL);
	if (g_test_wait_data == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	return TRUE;
}

void CAnykaIPCameraDlg::close_test_wait() 
{
	if(g_test_wait_data!= INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_test_wait_data);
		g_test_wait_data = INVALID_HANDLE_VALUE;
	}
}
BOOL CAnykaIPCameraDlg::Auto_move(char *cmd)
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
	TCHAR *file_name = _T("test_ptz_misc");
	TCHAR *file_name_cmd = _T("test_ptz_cmd");

	USES_CONVERSION;

	if (!g_connet_flag)
	{
		AfxMessageBox(_T("����û�������ϣ�����"), MB_OK);
		return FALSE;
	}

	if (g_download_ptz_misc)
	{
		memset(test_Name, 0,  MAX_PATH);
		_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
		_tcscat(test_Name, _T("/"));
		_tcscat(test_Name, file_name);

		//�ж��ļ��Ƿ����
		if(0)//if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(test_Name)))
		{
			str.Format(_T("%s no exist,����"), test_Name);
			AfxMessageBox(str, MB_OK); 
			return FALSE;
		}


		//�����ļ�
		memset(DestName_temp, 0, MAX_PATH);
		_tcsncpy(DestName_temp, DestName, sizeof(DestName));
		strSourceName.Format(_T("%s"), ConvertAbsolutePath(test_Name));
		_tcscat(DestName_temp, file_name);
		strDestName.Format(_T("%s"), DestName_temp);

		if(0)//if (!put_file_by_ftp(strSourceName, strDestName))  
		{
			AfxMessageBox(_T("Auto_move_test putting file fail"), MB_OK);  
			no_put_flie_flag  = true;
			download_file_flag = FALSE;
			return FALSE;
		}


		memset(test_Name, 0,  MAX_PATH);
		_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
		_tcscat(test_Name, _T("/"));
		_tcscat(test_Name, file_name_cmd);

		//�ж��ļ��Ƿ����
		if(0)//if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(test_Name)))
		{
			str.Format(_T("%s no exist,����"), test_Name);
			AfxMessageBox(str, MB_OK); 
			return FALSE;
		}

		//�����ļ�
		memset(DestName_temp, 0, MAX_PATH);
		_tcsncpy(DestName_temp, DestName, sizeof(DestName));
		strSourceName.Format(_T("%s"), ConvertAbsolutePath(test_Name));
		_tcscat(DestName_temp, file_name_cmd);
		strDestName.Format(_T("%s"), DestName_temp);

		if(0)//if (!put_file_by_ftp(strSourceName, strDestName))  
		{
			AfxMessageBox(_T("Auto_move_test putting file fail"), MB_OK);  
			no_put_flie_flag  = true;
			download_file_flag = FALSE;
			return FALSE;
		}

		g_download_ptz_misc = FALSE;
	}

	no_put_flie_flag  = false;
	//��������
	name_len = strlen(T2A(file_name_cmd));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(file_name_cmd), name_len);

	g_test_pass_flag[0] = 0;
	g_send_commad[0] = 1;

	memset(param_buf, 0, 2);
	memcpy(param_buf, cmd, 1);


	if (!Send_cmd(TEST_COMMAND, 0, name_buf, param_buf, strlen(param_buf), 0))
	{
		AfxMessageBox(_T("Send_cmd��fail "), MB_OK); 
		return FALSE;
	}
	//if (memcmp(cmd, "5", 1) == 0)
	{
	//	Anyka_Test_check_info(3000);
	}
	//else
	{
		Anyka_Test_check_no_info(1000);
	}
	

	//MessageBox(_T("ÿ��10���Զ�Ѳ���ѿ���"), MB_OK);  
	g_test_pass_flag[0] = 0;
	g_send_commad[0] = 0;

	return TRUE;
}

BOOL CAnykaIPCameraDlg::Auto_stop()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	USES_CONVERSION;

	if (!g_connet_flag)
	{
		AfxMessageBox(_T("����û�������ϣ�����"), MB_OK);
		return FALSE;
	}

	g_test_pass_flag[0] = 0;
	g_send_commad[0] = 0;

	if (!Send_cmd(TEST_COMMAND, 0, NULL, NULL,0,  0))
	{
		AfxMessageBox(_T("Send_cmd��fail "), MB_OK); 
		return FALSE;
	}

	Anyka_Test_check_no_info(1000);

	//MessageBox(_T("ֹͣ�Զ�Ѳ��"), MB_OK);  
	g_test_pass_flag[0] = 0;
	g_send_commad[0] = 0;

	return TRUE;
}


#if 0
void CAnykaIPCameraDlg::Auoto_Move_CloseServer(char move_flag) 
{

	//Send_cmd(TEST_COMMAND_FINISH, 0, NULL, NULL);

	if(m_pFtpConnection != NULL)
	{
		m_pFtpConnection->Close();
		delete m_pFtpConnection;
		m_pFtpConnection = NULL;
	}

	if(m_pInetSession != NULL)
	{
		m_pInetSession->Close();
		delete m_pInetSession;
		m_pInetSession = NULL;
	}

	g_send_commad = 0;
	g_test_fail_flag  = 0;
	download_file_flag = FALSE;
	download_dev_file_flag = FALSE;

	close_thread_heat(0);
	close_thread_rev_data(0);
	m_ClientSocket.Heat_Socket_Close(0);
	m_ClientSocket.Socket_Close(0);

	g_connet_flag = FALSE;

#if 0
	m_RightDlg.GetDlgItem(IDC_BUTTON_PRE_TEST)->EnableWindow(FALSE);//����
	m_BottomDlg.GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(TRUE);//����
	m_RightDlg.GetDlgItem(IDC_BUTTON_FIND_IP)->EnableWindow(TRUE);//����
	m_RightDlg.GetDlgItem(IDC_LIST1)->EnableWindow(TRUE);//����
	m_BottomDlg.GetDlgItem(IDC_BUTTON_CONFIGURE)->EnableWindow(TRUE);//����
	m_BottomDlg.GetDlgItem(IDC_BUTTON_WRITE_UID)->EnableWindow(TRUE);//����
	m_BottomDlg.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);//����
	m_RightDlg.GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);;//���� 
	m_RightDlg.GetDlgItem(IDC_BUTTON_FAILED)->EnableWindow(FALSE);;//����
	m_BottomDlg.GetDlgItem(IDC_BUTTON_AUTO_MOVE)->EnableWindow(TRUE);//����
	
	if (0)//move_flag == 5)
	{

	}
	else
	{
		m_BottomDlg.GetDlgItem(IDC_BUTTON_UP)->EnableWindow(FALSE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow(FALSE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_LEFT)->EnableWindow(FALSE);//����
		m_BottomDlg.GetDlgItem(IDC_BUTTON_RIGHT)->EnableWindow(FALSE);//����
	}
#endif
	
	g_finish_find_flag = TRUE;
}
#endif

BOOL CAnykaIPCameraDlg::Auto_move_connect(char move_flag)
{
#if 0
	if (!g_move_test_connect_flag)
	{
		g_move_test_connect_flag = TRUE;
		if (check_ip(m_connect_ip) && m_connect_ip[0] == 0 
			&& m_connect_ip[1] == 0 && m_connect_ip[2] == 0
			&& m_connect_ip[3] == 0 && m_connect_ip[4] == 0
			&& m_connect_ip[5] == 0 && m_connect_ip[6] == 0
			&& m_connect_ip[7] == 0 && m_connect_ip[8] == 0)
		{
			g_move_test_connect_flag = FALSE;
			AfxMessageBox(_T("��ѡ��һ��IP��ַ"), MB_OK);
			return FALSE;
		}

		g_finish_find_flag = FALSE;

		// TODO: �ڴ���ӿؼ�֪����������
		if (!ConnetServer(m_connect_ip, 0))
		{
			//AfxMessageBox(_T("��������ʧ�ܣ�����"), MB_OK);
			g_connet_flag = FALSE;
			g_move_test_connect_flag = FALSE;
			Auoto_Move_CloseServer(move_flag);
			return FALSE;
		}

		g_connet_flag = TRUE;
	}
#endif
	if (!g_connet_flag)
	{
		AfxMessageBox(_T("����û�������ϣ�����"), MB_OK);
		return FALSE;
	}
	return TRUE;

}

#if 0
BOOL CAnykaIPCameraDlg::Auto_move_disconnect(char move_flag)
{
	if (g_move_test_connect_flag)
	{
		g_move_test_connect_flag = FALSE;
		Auoto_Move_CloseServer(move_flag);
	}

	return TRUE;
}
#endif

BOOL CAnykaIPCameraDlg::Auto_move_test(char move_flag)
{
	CAnykaIPCameraDlg * pP = (CAnykaIPCameraDlg *)GetParent();

	if (!Auto_move_connect(move_flag))
	{
		return FALSE;
	}
	
	if(move_flag == 5)//�Զ�
	{
		Auto_move("5");
	}
	else if(move_flag == 6)//�Զ�
	{
		Auto_move("6");
	}
	else if(move_flag == 3)//����
	{
		Auto_move("3");
	}
	else if(move_flag == 4)//����
	{
		Auto_move("4");
	}
	else if(move_flag == 1)//����
	{
		Auto_move("1");
	}
	else if(move_flag == 2)//����
	{
		Auto_move("2");
	}
	else if(move_flag == 7)//ֹͣ
	{
		Auto_move("7");
	}

	

	return TRUE;
}


BOOL CAnykaIPCameraDlg::move_start(char *idex)
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
	TCHAR *file_name = _T("test_ptz_auto");

	USES_CONVERSION;

	if (!g_connet_flag)
	{
		AfxMessageBox(_T("����û�������ϣ�����"), MB_OK);
		return FALSE;
	}

	memset(test_Name, 0,  MAX_PATH);
	_tcsncpy(test_Name, TEST_CONFIG_DIR, sizeof(TEST_CONFIG_DIR));
	_tcscat(test_Name, _T("/"));
	_tcscat(test_Name, file_name);

	//�ж��ļ��Ƿ����
	if(0xFFFFFFFF == GetFileAttributes(ConvertAbsolutePath(test_Name)))
	{
		str.Format(_T("%s no exist,����"), test_Name);
		AfxMessageBox(str, MB_OK); 
		return FALSE;
	}

	//�����ļ�
	memset(DestName_temp, 0, MAX_PATH);
	_tcsncpy(DestName_temp, DestName, sizeof(DestName));
	strSourceName.Format(_T("%s"), ConvertAbsolutePath(test_Name));
	_tcscat(DestName_temp, file_name);
	strDestName.Format(_T("%s"), DestName_temp);

	if (!put_file_by_ftp(strSourceName, strDestName))  
	{
		AfxMessageBox(_T("Auto_move_test putting file fail"), MB_OK);  
		no_put_flie_flag  = true;
		download_file_flag = FALSE;
		return FALSE;
	}

	no_put_flie_flag  = false;
	//��������
	name_len = strlen(T2A(file_name));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(file_name), name_len);

	g_test_pass_flag[0] = 0;
	g_send_commad[0] = 1;

	memset(param_buf, 0, 2);
	memcpy(param_buf, idex, 1);

	if (!Send_cmd(TEST_COMMAND, 0, name_buf, param_buf, strlen((param_buf)), 0))
	{
		AfxMessageBox(_T("Send_cmd��fail "), MB_OK); 
		return FALSE;
	}

	Anyka_Test_check_no_info(1000);

	//MessageBox(_T("ÿ��10���Զ�Ѳ���ѿ���"), MB_OK);  
	g_test_pass_flag[0] = 0;
	g_send_commad[0] = 0;

	return TRUE;
}



DWORD WINAPI test_monitor_ontime(void  *lpParameter)
{
	CAnykaIPCameraDlg* pthis = (CAnykaIPCameraDlg*)lpParameter;
	CString str;
	UINT i = 0;

	while(1)
	{

		if (m_test_monitor_handle == INVALID_HANDLE_VALUE)
		{
			break;
		}

		if(g_test_monitor_flag && !g_test_config.update_find_flag)
		{	
			pthis->m_RightDlg.m_test_Status.SetFontColor(RGB(0,0,0));
			pthis->m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T(""));
			//pthis->m_RightDlg.m_test_wifi_list.DeleteAllItems();
			pthis->m_BottomDlg.m_test_wifi_list.DeleteAllItems();
			g_test_monitor_flag = FALSE;
			if (!g_close_monitor)
			{
				pthis->close_minitor();
			}

			if(_tcscmp(g_test_config.newest_version, g_test_config.m_last_config[current_ip_idex].Current_IP_version) != 0)
			{
				pthis->m_RightDlg.m_test_Status.SetFontColor(RGB(255,0,0));
				pthis->m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("�汾�Ų�һ�£����޸����°汾�ţ�"));
				continue;
			}
			//Sleep(500);
			//socket����������
			if (!g_connet_flag)
			{
				for (i = 0; i < 3; i++)
				{
					if (!pthis->ConnetServer(m_connect_ip, 0))
					{
						Sleep(1000);
						continue;
					}
					else
					{
						break;
					}
				}

				if (i == 3)
				{
					//Sleep(1000);
					//AfxMessageBox(_T("�����̵߳�����û�������ϣ�����������"), MB_OK);
					pthis->m_RightDlg.m_test_Status.SetFontColor(RGB(255,0,0));
					pthis->m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("�����̵߳�����û��������"));
					continue;
				}
			}

			
			if(g_connet_flag)
			{
				if (g_test_config.config_voice_rev_enable)
				{
					if (pthis->case_monitor())
					{
						//pthis->m_RightDlg.m_test_Status.SetFontColor(RGB(0,255,0));
						pthis->m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("�豸�Ĳ����̵߳�������������"));
					}
					
				}
				else
				{
					//pthis->m_RightDlg.m_test_Status.SetFontColor(RGB(0,255,0));
					pthis->m_RightDlg.GetDlgItem(IDC_STATIC_TEST_POINT1)->SetWindowText(_T("�豸�Ĳ����̵߳�������������"));
				}

				if (g_test_config.config_sd_enable)
				{
					//pthis->case_sd();
				}
					//wifi����
				if(0)//g_connet_flag)
				{
					if (g_test_config.config_wifi_enable)
					{
						for (i = 0; i < 1; i++)
						{
							//Sleep(1000);
							if(!pthis->case_wifi())
							{
								continue;
							}
							else
							{
								break;
							}
						}

						if(i == 1)
						{
							//AfxMessageBox(_T("wifi����ʧ��,���ܻ�����û�в�����"), MB_OK);
						}
					}

				}
			}
		}
		Sleep(1000);

		//pthis->m_RightDlg.UpdateWindow();
		//pthis->m_BottomDlg.UpdateWindow();
	}

	//pthis->close_show_focus_ontime_thread();

	return 1;
}

BOOL CAnykaIPCameraDlg::Creat_test_monitor_thread()
{
	close_test_monitor_thread();

	m_test_monitor_handle = CreateThread(NULL, 0, test_monitor_ontime, this, 0, NULL);
	if (m_test_monitor_handle == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("�����������߳�ʧ��,����"), MB_OK);
		return FALSE;
	}
	return TRUE;

}

void CAnykaIPCameraDlg::close_test_monitor_thread() 
{
	if(m_test_monitor_handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_test_monitor_handle);
		m_test_monitor_handle = INVALID_HANDLE_VALUE;
	}

}

void CAnykaIPCameraDlg::close_minitor(void) 
{
	g_monitor_end_flag = TRUE;

	m_ClientSocket.Rev_Socket_Close(0);
	close_pcm_play_thread();

	g_close_monitor = TRUE;

	Sleep(1000);
}

void CAnykaIPCameraDlg::find_IP_CloseServer(UINT idex) 
{
	if(m_pFtpConnection[idex] != NULL)
	{
		m_pFtpConnection[idex]->Close();
		delete m_pFtpConnection[idex];
		m_pFtpConnection[idex] = NULL;
	}

	if(m_pInetSession[idex] != NULL)
	{
		m_pInetSession[idex]->Close();
		delete m_pInetSession[idex];
		m_pInetSession[idex] = NULL;
	}

	close_thread_heat(0);
	close_thread_rev_data(0);
	m_ClientSocket.Heat_Socket_Close(0);
	m_ClientSocket.Socket_Close(0);


	g_monitor_end_flag = TRUE;

	m_ClientSocket.Rev_Socket_Close(0);
	close_pcm_play_thread();

	g_close_monitor = TRUE;

	g_connet_flag = FALSE;

}
//���²���sd
DWORD WINAPI retest_sd_main(void  *lpParameter)
{
	CAnykaIPCameraDlg* pthis = (CAnykaIPCameraDlg*)lpParameter;
	CString str;

	g_test_config.sd_reset_flag = TRUE;

	if (pthis->case_sd())
	{
		g_sd_test_success = TRUE;
	}
	else
	{
		g_sd_test_success = FALSE;
	}

	pthis->close_retest_sd_thread();
	pthis->m_RightDlg.GetDlgItem(IDC_BUTTON_SD_RETEST)->EnableWindow(TRUE);//����
	g_test_config.sd_reset_flag = FALSE;
	return 1;
}

BOOL CAnykaIPCameraDlg::Creat_retest_sd_thread()
{
	close_retest_sd_thread();

	if (check_ip(m_connect_ip) && m_connect_ip[0] == 0 
		&& m_connect_ip[1] == 0 && m_connect_ip[2] == 0
		&& m_connect_ip[3] == 0 && m_connect_ip[4] == 0
		&& m_connect_ip[5] == 0 && m_connect_ip[6] == 0
		&& m_connect_ip[7] == 0 && m_connect_ip[8] == 0)
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_SD_RETEST)->EnableWindow(TRUE);//����
		next_test_flag = TRUE;
		start_flag  = FALSE;
		AfxMessageBox(_T("��ѡ��һ��IP��ַ"), MB_OK);
		return FALSE;
	}

	m_retest_sd_handle = CreateThread(NULL, 0, retest_sd_main, this, 0, NULL);
	if (m_retest_sd_handle == INVALID_HANDLE_VALUE)
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_SD_RETEST)->EnableWindow(TRUE);//����
		AfxMessageBox(_T("����SD�ز����߳�ʧ��,����"), MB_OK);
		return FALSE;
	}
	return TRUE;

}

void CAnykaIPCameraDlg::close_retest_sd_thread() 
{
	if(m_retest_sd_handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_retest_sd_handle);
		m_retest_sd_handle = INVALID_HANDLE_VALUE;
	}

}



//���²���wifi
DWORD WINAPI retest_wifi_main(void  *lpParameter)
{
	CAnykaIPCameraDlg* pthis = (CAnykaIPCameraDlg*)lpParameter;
	CString str;

	g_test_config.wifi_reset_flag = TRUE;

	pthis->case_wifi();
	pthis->close_retest_wifi_thread();
	pthis->m_RightDlg.GetDlgItem(IDC_BUTTON_WIFI_RETEST)->EnableWindow(TRUE);//����
	g_test_config.wifi_reset_flag = FALSE;
	return 1;
}

BOOL CAnykaIPCameraDlg::Creat_retest_wifi_thread()
{
	close_retest_wifi_thread();

	if (check_ip(m_connect_ip) && m_connect_ip[0] == 0 
		&& m_connect_ip[1] == 0 && m_connect_ip[2] == 0
		&& m_connect_ip[3] == 0 && m_connect_ip[4] == 0
		&& m_connect_ip[5] == 0 && m_connect_ip[6] == 0
		&& m_connect_ip[7] == 0 && m_connect_ip[8] == 0)
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_WIFI_RETEST)->EnableWindow(TRUE);//����
		next_test_flag = TRUE;
		start_flag  = FALSE;
		AfxMessageBox(_T("��ѡ��һ��IP��ַ"), MB_OK);
		return FALSE;
	}

	m_retest_wifi_handle = CreateThread(NULL, 0, retest_wifi_main, this, 0, NULL);
	if (m_retest_wifi_handle == INVALID_HANDLE_VALUE)
	{
		m_RightDlg.GetDlgItem(IDC_BUTTON_WIFI_RETEST)->EnableWindow(TRUE);//����
		AfxMessageBox(_T("����wifi�ز����߳�ʧ��,����"), MB_OK);
		return FALSE;
	}
	return TRUE;

}

void CAnykaIPCameraDlg::close_retest_wifi_thread() 
{
	if(m_retest_wifi_handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_retest_wifi_handle);
		m_retest_wifi_handle = INVALID_HANDLE_VALUE;
	}

}


BOOL CAnykaIPCameraDlg::case_get_lisence()
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
	TCHAR *file_name = _T("get_lisence");
	//TCHAR test_param[MAX_PATH];

	USES_CONVERSION;

	if (!g_connet_flag)
	{
		AfxMessageBox(_T("����û�������ϣ�����"), MB_OK);
		return FALSE;
	}
	g_send_commad[0] = 1;

	//��������
	name_len = strlen(T2A(file_name));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(file_name), name_len);

	g_test_pass_flag[0] = 0;
	if (!Send_cmd(TEST_COMMAND, 1, name_buf, NULL,0, 0))
	{
		AfxMessageBox(_T("Send_cmd��fail "), MB_OK); 
		g_send_commad[0] = 0;
		return FALSE;
	}

	MessageBox(_T("����get lisence����ɹ�"), MB_OK);
	return TRUE;
}



BOOL CAnykaIPCameraDlg::Get_update_file() 
{
	// TODO: Add your control notification handler code here
	OPENFILENAME ofn;
	TCHAR pstrFileName[260] = {0}, pstrTitleName[260] = {0};

	TCHAR szFilter[] =	TEXT ("rar Files (*.Tar;*.nb0)\0*.Tar;*.nb0\0")  \
		TEXT ("All Files (*.*)\0*.*\0\0") ;

	memset ( &ofn, 0, sizeof ( ofn ) );

	ofn.lStructSize       = sizeof (OPENFILENAME) ;
	ofn.hInstance         = _hInstance ;
	ofn.hwndOwner         = GetSafeHwnd();
	ofn.lpstrFilter       = szFilter;
	ofn.nMaxFile          = MAX_PATH ;
	ofn.lpstrDefExt       = TEXT ("bin") ;
	ofn.lpstrFile         = pstrFileName ;
	ofn.Flags             = OFN_FILEMUSTEXIST; 


	if(GetOpenFileName (&ofn))
	{
		memset(g_test_config.update_file_name, 0, MAX_PATH+1);
		_tcscpy(g_test_config.update_file_name, pstrFileName);
	
		return TRUE;
	}
	else
	{

		return FALSE;
	}
}

DWORD WINAPI check_update_finish_thread(LPVOID lpParameter)
{
	char commad_type; 
	CClientSocket m_ClientSocket_heat;
	int ret = 0;
	char lpBuf[256] = {0};
	UINT nBufLen = 256;
	UINT time1 = 0;
	UINT time2 = 0;
	UINT idex = 0;
	char *temp_comand = "test_update";
	BOOL update_flag = FALSE;


	memcpy(&idex, lpParameter, 4);
	time1 = GetTickCount();
	//��ȡ��������
	while (1)
	{
		Sleep(3000);
		if (g_update_finish_Thread[idex] != INVALID_HANDLE_VALUE)
		{
			commad_type = 0;  //��ʼ��
			ret = m_ClientSocket_heat.Socket_Receive_update_finish(lpBuf, nBufLen, idex);
			if (ret == -1)
			{
				time2 = GetTickCount();
				if (time2 - time1 > 30000)
				{
					g_test_pass_flag[idex] = 1;
					return TRUE;
				}
				else
				{
					g_test_pass_flag[idex] = 2;
					return TRUE;
				}
			}
			else
			{
				//time1 = GetTickCount();
			}
		}

	}


	return 1;
}


BOOL CAnykaIPCameraDlg::create_thread_update_finish(UINT idex) 
{
	memcpy(update_param[idex], &idex, sizeof(UINT));

	g_update_finish_Thread[idex] = CreateThread(NULL, 0, check_update_finish_thread, update_param[idex], 0, NULL);
	if (g_update_finish_Thread[idex] == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	return TRUE;
}

void CAnykaIPCameraDlg::close_thread_update_finish(UINT idex) 
{
	if(g_update_finish_Thread[idex] != INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_update_finish_Thread[idex]);
		g_update_finish_Thread[idex] = INVALID_HANDLE_VALUE;
	}
}


BOOL  CAnykaIPCameraDlg::check_update_finish_Server(LPCTSTR addr, UINT idex) 
{
	// TODO: Add your control notification handler code here
	CString str;
	HANDLE update_finish_Thread = INVALID_HANDLE_VALUE;

	USES_CONVERSION;

	// TODO: Add your control notification handler code here
	//m_ClientSocket.Create(0, SOCK_STREAM, NULL);
	if (!m_ClientSocket.Socket_Create_update_finish(idex))
	{
		AfxMessageBox(_T("�����������soketʧ��"), MB_OK);
		return FALSE;
	}


	//if(m_ClientSocket.Connect(addr, 6789))
	if(m_ClientSocket.Socket_Connect_update_finish(T2A(addr), 7890, idex))	
	{
		//�����߳�

		if (!create_thread_update_finish(idex))
		{
			AfxMessageBox(_T("���������̼߳��ʧ��"), MB_OK);
			return FALSE;
		}

		//�ر������̺߳ͽ��������߳�
		close_thread_heat(idex);
		close_thread_rev_data(idex);
	}
	else
	{
		AfxMessageBox(_T("Socket_Connect_update_finish fail"), MB_OK);
		return FALSE;
	}

	return TRUE;
}



BOOL CAnykaIPCameraDlg::Anyka_check_update_finish(UINT idex)
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
		delaytime = 480000;
		if (time2 - time1 > delaytime)
		{
			g_test_pass_flag[idex] = 0;
			AfxMessageBox(_T("��ʱ(30s)û�з���ȷ������"));  
			return FALSE;
		}
		if (g_test_pass_flag[idex] == 1)
		{
			g_test_pass_flag[idex] = 0;
			return TRUE;
		}

		if (g_test_pass_flag[idex] == 2)
		{
			g_test_pass_flag[idex] = 0;
			return FALSE;
		}
	}

}


BOOL CAnykaIPCameraDlg::Anyka_check_MAC_finish(UINT idex)
{
	UINT time1 = 0;
	UINT time2 = 0;

	UINT delaytime = 0;

	USES_CONVERSION;

	time1 = GetTickCount();
	while (1)
	{
		Sleep(100);
		time2 = GetTickCount();
		delaytime = 480000;
		if (time2 - time1 > delaytime)
		{
			g_test_pass_flag[idex] = 0;
			AfxMessageBox(_T("��ʱ(8����)û�з���ȷ������"));  
			return FALSE;
		}

		if (one_update_finish && g_time3 > 0)
		{
			if (time2 - g_time3 > 60000)
			{
				g_test_pass_flag[idex] = 0;
				AfxMessageBox(_T("��ʱ(30S)����"));  
				return FALSE;
			}
		}


		if (g_test_pass_flag[idex] == 1)
		{
			if(!one_update_finish)
			{
				one_update_finish = TRUE;//��һ̨�ɹ�
				g_time3 = time2; //GetTickCount();
			}

			g_test_pass_flag[idex] = 0;
			return TRUE;
		}

	}

}


BOOL CAnykaIPCameraDlg::Download_updatefile(TCHAR *ip_address, UINT list_idex, UINT idex) 
{
	CString str;
	CString strSourceName, strDestName;
	TCHAR DestName[50] =_T("/tmp/");
	TCHAR DestName_temp[MAX_PATH] = {0};
	UINT i = 0, name_len = 0;
	char name_buf[MAX_PATH] = {0};
	char param[50] = {0};
	TCHAR test_Name[MAX_PATH] = {0};
	TCHAR file_name[MAX_PATH] = {0};
	TCHAR test_update_name[50] = _T("test_update");
	HANDLE update_finish_Thread = INVALID_HANDLE_VALUE;
	TCHAR only_filename[MAX_PATH] = {0};
	BOOL net_test_mode = FALSE;
	UINT len = 0;

	USES_CONVERSION;
	//AfxMessageBox(_T("putfile"), MB_OK);
	//��ȡ�������ļ���
	name_len = _tcslen(g_test_config.update_file_name);
	_tcsncpy(file_name, g_test_config.update_file_name, name_len);

	for (i = name_len - 1; i >=0; i--)
	{
		if (file_name[i] == '/' || file_name[i] == '\\')
		{
			break;
		}
	}
	memset(only_filename, 0, MAX_PATH);
	_tcsncpy(only_filename, &file_name[i+1], name_len - i);
	//AfxMessageBox(_T("putfile1"), MB_OK);

	//�ж�С���Ƿ���������
	if (g_test_fail_flag[idex] == 1)
	{
		m_update_flag[list_idex] = 2;
		str.Format(_T("g_test_fail_flag[%d]��������С��"), idex);
		AfxMessageBox(str);
		return FALSE;
	}

	//�ж��ļ��Ƿ����
	memset(test_Name, 0,  MAX_PATH);
	_tcsncpy(test_Name, TEST_CONFIG_DIR, _tcslen(TEST_CONFIG_DIR));//sizeof(TEST_CONFIG));
	_tcscat(test_Name, _T("//"));
	_tcscat(test_Name, test_update_name);
	if(0xFFFFFFFF == GetFileAttributes(g_test_config.ConvertAbsolutePath(test_Name)))
	{
		m_update_flag[list_idex] = 2;
		str.Format(_T("%s no exist��������С��"), test_Name);
		AfxMessageBox(str);   
		return FALSE;
	}
	//AfxMessageBox(_T("putfile2"), MB_OK);
	//�����ļ�
	//������������
	m_update_flag[list_idex] = 4;
	memset(DestName_temp, 0, MAX_PATH);
	_tcsncpy(DestName_temp, DestName, _tcslen(DestName)); //sizeof(DestName));
	_tcscat(DestName_temp, test_update_name);
	strDestName.Format(_T("%s"), DestName_temp);
	strSourceName.Format(_T("%s"), g_test_config.ConvertAbsolutePath(test_Name));
	if (!m_pFtpConnection[idex]->PutFile(strSourceName, strDestName, FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD, 1))   
	{

		m_update_flag[list_idex] = 2;
		frmLogfile.WriteLogFile(0,"auto auto test putting %s fail \n", name_buf);
		str.Format(_T("Error auto test putting file:%d"), GetLastError());
		AfxMessageBox(str);
		return FALSE;
	}

	memset(test_Name, 0,  MAX_PATH);
	//_tcscat(test_Name, file_name);
	_tcsncpy(test_Name, file_name, _tcslen(file_name));
	if(0xFFFFFFFF == GetFileAttributes(g_test_config.ConvertAbsolutePath(test_Name)))
	{
		m_update_flag[list_idex] = 2;
		str.Format(_T("%s no exist��������С��"), file_name);
		AfxMessageBox(str);   
		return FALSE;
	}

	//�����ļ�
	//����������
	memset(DestName_temp, 0, MAX_PATH);
	_tcsncpy(DestName_temp, DestName, _tcslen(DestName));//sizeof(DestName));
	_tcscat(DestName_temp, only_filename);
	strDestName.Format(_T("%s"), DestName_temp);
	strSourceName.Format(_T("%s"), g_test_config.ConvertAbsolutePath(test_Name));
	if (!m_pFtpConnection[idex]->PutFile(strSourceName, strDestName, FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD, 1))   
	{
		m_update_flag[list_idex] = 2;
		frmLogfile.WriteLogFile(0,"auto auto test putting %s fail \n", name_buf);
		AfxMessageBox(_T("Error auto test putting file��������С��"));
		return FALSE;
	}
	
	g_send_commad[idex]  = 1;

#if 1
	//��������
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(test_update_name), strlen(T2A(test_update_name)));
	g_test_pass_flag[idex] = 0;
	if (!Send_cmd(TEST_COMMAND, 1, name_buf, T2A(only_filename),strlen(T2A(only_filename)), idex))

	{
		m_update_flag[list_idex] = 2;
		frmLogfile.WriteLogFile(0,"auto Send_cmd %s fail \n", name_buf);
		AfxMessageBox(_T("Error Send_cmd file��������С��"));   
		return FALSE;
	}
	//������

	//���շ���ֵ 

	if (!Anyka_Test_check_info_update(30000, idex))
	{
		g_send_commad[idex]  = 0;
		m_update_flag[list_idex] = 2;
		frmLogfile.WriteLogFile(0,"Anyka_Test_check_info fail \n");
		AfxMessageBox(_T("����������û�нӵ����س���������С��")); 
		return FALSE;
	}

	if (!g_test_config.net_mode)
	{
		//������
		m_update_flag[list_idex] = 3;
		g_test_pass_flag[idex] = 0;
		//����һ�����������ɵ��߳�
		if (!check_update_finish_Server(ip_address, idex))
		{
			m_update_flag[list_idex] = 2;
			AfxMessageBox(_T("�������������ɵ��߳�ʧ�ܣ�������С��"), MB_OK);
			return FALSE;
		}


		//�����ɹ�
		if (!Anyka_check_update_finish(idex))
		{
			m_update_flag[list_idex] = 2;
			frmLogfile.WriteLogFile(0,"Anyka_Test_check_info fail \n");
			//AfxMessageBox(_T("��ʱ����������С��"), MB_OK);
			return FALSE;
		}

		//�رռ���߳�
		close_thread_update_finish(idex);
	}
	else
	{
		m_update_flag[list_idex] = 3;
		g_test_pass_flag[idex] = 0;

		Sleep(10000);
		g_finish_flag = TRUE;

		if (!Anyka_check_MAC_finish(idex))
		{
			m_update_flag[list_idex] = 2;
			frmLogfile.WriteLogFile(0,"Anyka_check_MAC_finish fail \n");
			//AfxMessageBox(_T("��ʱ����������С��"), MB_OK);
			return FALSE;
		}
	}
#endif
	m_update_flag[list_idex] = 1;
	return TRUE;

}


DWORD WINAPI Creat_update_thread(LPVOID lpParameter)
{
	CAnykaIPCameraDlg test_dlg;
	T_IDEX_INFO param;
	UINT i = 0;

	memcpy(&param, lpParameter, sizeof(T_IDEX_INFO));

	//srand(time(0));
	//����sokect������
	//m_update_flag[param.IP_idex] = 4;
	if (!test_dlg.ConnetServer(g_test_config.m_last_config[param.IP_idex].Current_IP_address_buffer, param.thread_idex))
	{
		AfxMessageBox(_T("ConnetServer fail"));
		m_update_flag[param.IP_idex] = 2;

		WaitForSingleObject(g_handle,INFINITE);
		g_updateing_num++;
		if (g_updateing_num == m_ip_address_idex)
		{
			//g_update_all_flag = FALSE;
			AfxMessageBox(_T("�����豸���������,�и�������ʧ��,����"));
			g_update_all_finish_flag = TRUE;
			g_finish_flag = FALSE;
			for (i = 0; i < UPDATE_MAX_NUM; i++)
			{
				m_update_flag[i] = 0;
				g_send_commad[i] = 0;
			}
		}

		ReleaseSemaphore(g_handle,1,NULL);
		test_dlg.CloseServer(param.thread_idex);
		CloseHandle(g_all_updateThread[param.thread_idex]);
		g_all_updateThread[param.thread_idex] = INVALID_HANDLE_VALUE;
		AfxMessageBox(_T("����ʧ��"));
		return FALSE;
	}

	if (!test_dlg.Download_updatefile(g_test_config.m_last_config[param.IP_idex].Current_IP_address_buffer, param.IP_idex, param.thread_idex))
	{

		m_update_flag[param.IP_idex] = 2;
		WaitForSingleObject(g_handle,INFINITE);
		g_updateing_num++;
		if (g_updateing_num == m_ip_address_idex)
		{
			//g_update_all_flag = FALSE;
			test_dlg.Close_check_MAC_thread();
			AfxMessageBox(_T("�����豸���������,�и�������ʧ��,����"));
			g_update_all_finish_flag = TRUE;
			g_finish_flag = FALSE;
			for (i = 0; i < UPDATE_MAX_NUM; i++)
			{
				m_update_flag[i] = 0;
				g_send_commad[i] = 0;
			}

		}
		ReleaseSemaphore(g_handle,1,NULL);
		test_dlg.CloseServer(param.thread_idex);
		CloseHandle(g_all_updateThread[param.thread_idex]);
		g_all_updateThread[param.thread_idex] = INVALID_HANDLE_VALUE;
		return FALSE;
	}
	m_update_flag[param.IP_idex] = 1;
	test_dlg.CloseServer(param.thread_idex);
	CloseHandle(g_all_updateThread[param.thread_idex]);
	g_all_updateThread[param.thread_idex] = INVALID_HANDLE_VALUE;

	WaitForSingleObject(g_handle,INFINITE);
	g_updateing_num++;
	if (g_updateing_num == m_ip_address_idex)
	{
		//g_update_all_flag = FALSE;

		if (g_hupdateThread != INVALID_HANDLE_VALUE)
		{
			test_dlg.Close_update_all_thread();
			g_hupdateThread = INVALID_HANDLE_VALUE;
		}
		test_dlg.Close_check_MAC_thread();
		g_finish_flag = FALSE;
		AfxMessageBox(_T("�����豸���������"));
		g_update_all_finish_flag = TRUE;
		for (i = 0; i < UPDATE_MAX_NUM; i++)
		{
			m_update_flag[i] = 0;
			g_send_commad[i] = 0;
		}

	}
	ReleaseSemaphore(g_handle,1,NULL);


	return 1;
}


DWORD WINAPI check_MAC_main(LPVOID lpParameter) 
{
	CAnykaIPCameraDlg testDlg;

	while (1)
	{
		if (g_finish_flag)
		{
			Sleep(1000);
		}
		else
		{
			Sleep(5000);
		}


		if (g_check_MAC_Thread == INVALID_HANDLE_VALUE)
		{
			break;
		}

		if (g_finish_flag)
		{
			testDlg.On_find_ip_update();
		}
	}


	return 1;
}


BOOL CAnykaIPCameraDlg::Creat_check_MAC_thread() 
{
	UINT idex = 0;

	Close_check_MAC_thread();

	g_check_MAC_Thread = CreateThread(NULL, 0, check_MAC_main, &idex, 0, NULL);
	if (g_check_MAC_Thread == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	return TRUE;

}


void CAnykaIPCameraDlg::Close_check_MAC_thread() 
{
	if(g_check_MAC_Thread != INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_check_MAC_Thread);
		g_check_MAC_Thread = INVALID_HANDLE_VALUE;
	}

}


BOOL CAnykaIPCameraDlg::update_main()
{
	UINT idex = 0, i = 0;
	UINT update_one_time = 0;
	UINT update_next_num = 0;
	UINT IP_idex = 0;

	for (i = 0; i < UPDATE_MAX_NUM; i++)
	{
		if (g_heatThread[i] != INVALID_HANDLE_VALUE)
		{
			CloseHandle(g_heatThread[i]);
			g_heatThread[i] = INVALID_HANDLE_VALUE;
		}

		if (g_hBurnThread_rev_data[i] != INVALID_HANDLE_VALUE)
		{
			CloseHandle(g_hBurnThread_rev_data[i]);
			g_hBurnThread_rev_data[i] = INVALID_HANDLE_VALUE;
		}

		if (g_all_updateThread[i] != INVALID_HANDLE_VALUE)
		{
			CloseHandle(g_all_updateThread[i]);
			g_all_updateThread[i] = INVALID_HANDLE_VALUE;
		}

		if (g_update_finish_Thread[i] != INVALID_HANDLE_VALUE)
		{
			CloseHandle(g_update_finish_Thread[i]);
			g_update_finish_Thread[i] = INVALID_HANDLE_VALUE;
		}
		g_test_pass_flag[i] = 0;
		g_test_fail_flag[i] = 0;
		m_update_flag[i] = 0;
 		g_send_commad[i] = 0;
	}


	//����һ���̣߳�����������з��㲥����ȡMAC��ַ
	if (g_test_config.net_mode)
	{
		if (!Creat_check_MAC_thread())
		{
			AfxMessageBox(_T("����������ɺ���mac��ַ�߳�ʧ�ܣ�����"));
			return FALSE;
		}
	}

	memset(g_param, 0, sizeof(T_IDEX_INFO));

	//����һ�ζ�ұһ������
	update_next_num = m_ip_address_idex;
	frmLogfile.WriteLogFile(0," m_ip_address_idex:%d \n", m_ip_address_idex);
	while (1)
	{
		Sleep(1000);
		if (update_next_num > ONE_TIME_MAX_NUM)
		{
			update_one_time = ONE_TIME_MAX_NUM;
		}
		else
		{
			update_one_time = update_next_num;
		}
		frmLogfile.WriteLogFile(0," update_one_time:%d \n", update_one_time);
		for (idex = 0; idex < update_one_time; idex++)
		{
			if (g_all_updateThread[idex] == INVALID_HANDLE_VALUE && IP_idex < m_ip_address_idex)
			{
				//����һ���߳�
				g_param[idex].IP_idex = IP_idex;
				g_param[idex].thread_idex = idex;
				Sleep(100);
				if (g_update_flag[IP_idex] == 1)
				{
					g_all_updateThread[idex] = CreateThread(NULL, 0, Creat_update_thread, &g_param[idex], 0, NULL);
					if (g_all_updateThread[idex] == INVALID_HANDLE_VALUE)
					{
						return FALSE;
					}

				}
				else
				{
					WaitForSingleObject(g_handle,INFINITE);
					g_updateing_num++;
					ReleaseSemaphore(g_handle,1,NULL);
				}
				frmLogfile.WriteLogFile(0," IP_idex:%d \n", IP_idex);
				IP_idex++;
			}
		}

		if (IP_idex >= m_ip_address_idex)
		{
			frmLogfile.WriteLogFile(0," end IP_idex:%d \n", IP_idex);
			break;
		}
	}

	return TRUE;

}


DWORD WINAPI Creat_Anyka_update_main(LPVOID lpParameter)
{
	CAnykaIPCameraDlg test_dlg = (CAnykaIPCameraDlg *)lpParameter;

	g_update_all_flag = TRUE;
	g_update_all_finish_flag = FALSE;
	g_updateing_num = 0; 

	if (!test_dlg.update_main())
	{
		return 0;
	}

	return 1;
}

BOOL CAnykaIPCameraDlg::creat_update_all_thread() 
{
	if (g_hupdateThread != INVALID_HANDLE_VALUE)
	{
		Close_update_all_thread();
		g_hupdateThread = INVALID_HANDLE_VALUE;
	}

	g_hupdateThread = CreateThread(NULL, 0, Creat_Anyka_update_main, this, 0, NULL);
	if (g_hupdateThread == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("���������߳�ʧ��"), MB_OK);
		return FALSE;
	}

	return TRUE;
}


void CAnykaIPCameraDlg::Close_update_all_thread() 
{
	if(g_hupdateThread != INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_hupdateThread);
		g_hupdateThread = INVALID_HANDLE_VALUE;
	}

}



BOOL CAnykaIPCameraDlg::case_test_user()
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
	TCHAR *file_name = _T("test_case");
	//TCHAR test_param[MAX_PATH];

	USES_CONVERSION;

	if (!g_connet_flag)
	{
		AfxMessageBox(_T("����û�������ϣ�����"), MB_OK);
		return FALSE;
	}
	g_send_commad[0] = 1;

	m_BottomDlg.SetDlgItemText(IDC_STATIC_TEST_STAUT, _T("������"));

	//��������
	name_len = strlen(T2A(file_name));
	memset(name_buf, 0, MAX_PATH);
	memcpy(name_buf, T2A(file_name), name_len);

	g_test_pass_flag[0] = 0;
	if (!Send_cmd(TEST_COMMAND, 1, name_buf, NULL, 0, 0))
	{
		m_BottomDlg.SetDlgItemText(IDC_STATIC_TEST_STAUT, _T("����ʧ��"));
		AfxMessageBox(_T("Send_cmd��fail "), MB_OK); 
		g_send_commad[0] = 0;
		return FALSE;
	}


	//���շ���ֵ
	if (!Anyka_Test_check_info(10000))
	{
		m_BottomDlg.SetDlgItemText(IDC_STATIC_TEST_STAUT, _T("����ʧ��"));//����
		g_send_commad[0] = 0;
		return FALSE;
	}

	m_BottomDlg.SetDlgItemText(IDC_STATIC_TEST_STAUT, _T("���Գɹ�"));

	return TRUE;
}


BOOL CAnykaIPCameraDlg::Close_record_video()
{
	g_start_record_flag = FALSE;

	Sleep(10);
	if (g_file_fp.m_hFile != INVALID_HANDLE_VALUE)
	{
		g_file_fp.Close();
		g_file_fp.m_hFile = INVALID_HANDLE_VALUE;
	}

	return TRUE;
}

BOOL CAnykaIPCameraDlg::Creat_record_video()
{
	CString strPath;
	TCHAR video_path[MAX_PATH+1];
	UINT i = 0;

	TCHAR szFilter[] =	TEXT("BIN Files(*.dat)|*.dat|") \
		TEXT("All Files (*.*)|*.*||") ;

	CFileDialog fd(FALSE, _T(".dat"), NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT
		, szFilter, NULL);

	if(IDOK == fd.DoModal())
	{

		strPath = fd.GetPathName();
		memset(video_path, 0, (MAX_PATH + 1)*sizeof(TCHAR));
		_tcscpy(video_path, strPath);

		if (g_file_fp.m_hFile == INVALID_HANDLE_VALUE)
		{
			for (i = 0; i < 5; i++)
			{
				if (g_file_fp.Open(video_path, CFile::modeCreate|CFile::modeReadWrite)) //,CFile::modeRead))
				{
					break;
				}
				Sleep(100);
			}

			if (i == 5)
			{
				//DWORD ret = GetLastError();
				//str.Format(_T("error: open play video file fail %d"), ret);
				//AfxMessageBox(str, MB_OK);

				AfxMessageBox(_T("creat video file fail"), MB_OK);
				return FALSE;
			}

		}
		g_start_record_flag = TRUE;
	}

	return TRUE;
}


//BOOL CAnykaIPCameraDlg::Write_record_video(char *buf, UINT buf_len)
//{
//	g_file_fp.Write(buf,buf_len);
//	return TRUE;
//}

DWORD WINAPI Creat_record_video_main(LPVOID lpParameter)
{
	CString str;
	while (1)
	{
		if (!g_start_record_flag)
		{
			break;
		}

		if (g_video_buf != NULL && g_video_buf_len != 0)
		{
			//g_file_fp.Write(g_video_buf,g_video_buf_len);
			//g_video_buf_len = 0;
			//memset(g_video_buf, 0, MAX_VIDEO_LEN);
		}

		Sleep(1000);
		
	}
	return 1;
}

BOOL CAnykaIPCameraDlg::creat_record_video_thread() 
{
	if (g_record_video_Thread != INVALID_HANDLE_VALUE)
	{
		Close_record_video_thread();
		g_record_video_Thread = INVALID_HANDLE_VALUE;
	}

	g_record_video_Thread = CreateThread(NULL, 0, Creat_record_video_main, this, 0, NULL);
	if (g_record_video_Thread == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("���������߳�ʧ��"), MB_OK);
		return FALSE;
	}

	g_start_record_flag = TRUE;

	return TRUE;
}

void CAnykaIPCameraDlg::Close_record_video_thread() 
{
	if(g_record_video_Thread != INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_record_video_Thread);
		g_record_video_Thread = INVALID_HANDLE_VALUE;
	}

}
void CAnykaIPCameraDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
}



BOOL CAnykaIPCameraDlg::Send_cmd_printf(char *param, UINT param_len)
{
	char lpBuf[1024] = {0};
	short nBufLen = param_len;
	int ret = FALSE;

	USES_CONVERSION;

	memset(lpBuf, 0, 1024);
	memcpy(lpBuf, param, param_len);
	ret = m_ClientSocket.Socket_server_Send(lpBuf, nBufLen);
	if (!ret)
	{
		return FALSE;
	}

	return TRUE;
}


BOOL CAnykaIPCameraDlg::server_rev_printf_date(void)
{
	CClientSocket m_ClientSocket;
	char *lpBuf = NULL;
	UINT nBufLen = 256;  
	T_UID_MAC_INFO uid_mac;
	int len = 0;

	USES_CONVERSION;

	lpBuf = new char[nBufLen + 1];
	if (lpBuf == NULL)
	{
		AfxMessageBox(_T("malloc receive buf fail"), MB_OK);
		return -1;
	}

	//��ȡ��������
	while (1)
	{
		Sleep(100);
		if (g_server_Thread != INVALID_HANDLE_VALUE )
		{
			memset(lpBuf,0,nBufLen + 1);

			len = m_ClientSocket.Socket_server_Receive(lpBuf, nBufLen);

			//���յ�����󣬽��лظ�uid��mac
			if (len > 0)
			{
				if (lpBuf[0] == TEST_COMMAND)
				{
					UINT buf_len =0;
					
					memset(&uid_mac, 0, sizeof(T_UID_MAC_INFO));
					memset(uid_mac.UID, 0, MAC_ADDRESS_LEN);
					//_tcscpy(uid_mac.UID, g_test_config.m_current_config->Current_IP_UID);
					memcpy(uid_mac.UID, T2A(g_test_config.m_last_config[current_ip_idex].Current_IP_UID), strlen(T2A(g_test_config.m_last_config[current_ip_idex].Current_IP_UID)));
					//memcpy(uid_mac.UID, "12345678912345678912345678912345", 32);

					memset(uid_mac.MAC, 0, MAC_ADDRESS_LEN);
					memcpy(uid_mac.MAC, T2A(g_test_config.m_last_config[current_ip_idex].Current_IP_MAC), strlen(T2A(g_test_config.m_last_config[current_ip_idex].Current_IP_MAC)));
					//memcpy(uid_mac.MAC, "aa:bb:cc:00:00:00", 17);

					if (!Send_cmd_printf((char *)&uid_mac, sizeof(uid_mac)))
					{
						AfxMessageBox(_T("send uid mac fail"), MB_OK);
					}
				}
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	free(lpBuf);

	return TRUE;
}


DWORD WINAPI Creat_server_main(LPVOID lpParameter)
{
	CAnykaIPCameraDlg  TestToolDlg;
	CClientSocket m_ClientSocket;
	UINT i =0;

	int err = -1;

	USES_CONVERSION;

	while (1)
	{
		if (g_server_Thread != INVALID_HANDLE_VALUE)
		{
			//g_enter_flag = TRUE;

			if (m_ClientSocket.Socket_server_Create() == 0)
			{
				AfxMessageBox(_T("��������˵��߳�ʧ��"), MB_OK);
			}
			else
			{
				//ʹserver ����������
				if (m_ClientSocket.Socket_server_setsockopt() != 0) 
				{
					AfxMessageBox(_T("����˵�setsockopt fail"), MB_OK);
				}
				else
				{
					if (m_ClientSocket.Socket_server_Bind(NULL, 6999) != 1)
					{
						AfxMessageBox(_T("����˵�Bind fail"), MB_OK);
					}
					else
					{
						if (m_ClientSocket.Socket_server_Listen(4) != 1 )
						{
							AfxMessageBox(_T("����˵� Listen fail"), MB_OK);
						}
						else
						{
							while(1)
							{
								if (g_server_Thread == INVALID_HANDLE_VALUE)
								{
									break;
								}

								if (m_ClientSocket.Socket_server_Accept() != 1)
								{
								}
								else
								{
									TestToolDlg.server_rev_printf_date();
								}
							}
						}
					}
				}
			}
			m_ClientSocket.Socket_server_Close();
		}
		else
		{
			m_ClientSocket.Socket_server_Close();
			break;
		}
	}
	return 1;
}


BOOL CAnykaIPCameraDlg::creat_server_thread() 
{
	if (g_server_Thread != INVALID_HANDLE_VALUE)
	{
		Close_server_thread();
		g_server_Thread = INVALID_HANDLE_VALUE;
	}

	g_server_Thread = CreateThread(NULL, 0, Creat_server_main, this, 0, NULL);
	if (g_server_Thread == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("���������߳�ʧ��"), MB_OK);
		return FALSE;
	}

	return TRUE;
}


void CAnykaIPCameraDlg::Close_server_thread() 
{
	if (g_server_Thread != INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_server_Thread);
		g_server_Thread = INVALID_HANDLE_VALUE;
	}
}

BOOL CAnykaIPCameraDlg::Send_audio_data(char *buf, UINT buf_len)
{
	char *lpBuf = NULL;
	char *head_name = "FRAM";
	char *end_name = "FEND";
	UINT nBufLen = 0;
	UINT frame_type = FRAME_TYPE_AUDIO;
	UINT i = 0;
	UINT len_data = 0;
	UINT check_sum = 0;
	UINT time = 0;
	int ret = 0;
	UINT idex= 0;

	//��ͷ��ʶBytes  ֡����Bytes	ʱ���Bytes ֡����Bytes ��Ƶ����	CRCУ��Bytes	��β��ʶBytes


	USES_CONVERSION;

	nBufLen = 4;//ͷ

	nBufLen += 4;//֡����
	nBufLen += 4;//ʱ���
	//fprintf(stderr, "buf_len:%d \n", buf_len);

	if (buf != NULL)
	{
		len_data = buf_len;
		nBufLen += 4;//����
		nBufLen += len_data;//����
		//fprintf(stderr, "nBufLen:%d \n", nBufLen);
	}
	else
	{
		AfxMessageBox(_T("audio data buf is null"), MB_OK);
		return FALSE;
	}

	nBufLen +=  4;//check sum
	nBufLen +=  4;//����


	lpBuf = (char *)malloc(nBufLen + 1);
	if (lpBuf == NULL)
	{
		return FALSE;
	}

	//���
	memset(lpBuf, 0, nBufLen + 1);
	memcpy(lpBuf, head_name, 4);
	memcpy(&lpBuf[4], &frame_type, 4);
	memcpy(&lpBuf[8], &time, 4);
	memcpy(&lpBuf[12], (char *)&len_data, 4);

	if (len_data != 0 && buf != NULL)
	{
		memcpy(&lpBuf[16], buf, len_data);;
	}

	for (i = 0; i < (UINT)(len_data + 4); i++)
	{
		check_sum += lpBuf[i + 4];
	}


	memcpy(&lpBuf[16 +len_data], (char *)&check_sum, 4);

	memcpy(&lpBuf[16 +len_data+4], end_name, 4);


	free(lpBuf);
	if(!ret)
	{
		return FALSE;
	}

	return TRUE;
}



//������Ƶ��
WAVEHDR header;
#define FRAGMENT_SIZE 512              // ��������С 
#define FRAGMENT_NUM 8                 // ����������
HWAVEIN g_hWaveIn;//������Ƶ���ݸ�ʽWave_audio���ݸ�ʽ 
WAVEFORMATEX g_wavform;//WAVEFORMATEX�ṹ�����˲�����Ƶ���ݸ�ʽ������������ṹ��Ψһ�ĸ�ʽ��Ϣ����ͬ���в�����Ƶ���ݸ�ʽ��������Ҫ�������Ϣ�ĸ�ʽ������ṹ��������һ���ṹ�ĵ�һ����Ա���Լ���������Ϣ   
WAVEHDR g_wh[FRAGMENT_NUM];  
HWAVEOUT g_hWaveOut;//�򿪻ط��豸������ 
WAVEHDR g_wavhdr;
BOOL  g_play_finish = FALSE;

//¼����
void CALLBACK waveOutProc(  HWAVEOUT hwo,UINT uMsg,DWORD_PTR dwInstance,DWORD_PTR dwParam1,DWORD_PTR dwParam2);   
// �����ص����� 
void CALLBACK waveOutProc(  HWAVEOUT hwo,UINT uMsg,DWORD_PTR dwInstance,DWORD_PTR dwParam1,DWORD_PTR dwParam2)  
{   
	//

	if (WOM_DONE == uMsg)   
	{   
		//waveOutUnprepareHeader(hwo,&header,sizeof(WAVEHDR));
		//g_play_finish = TRUE;   

	}   
}   

BOOL g_test_flag = FALSE;

UINT g_total_eln = 0;
// �������� 
void CALLBACK waveInProc(HWAVEIN hwi,UINT uMsg,DWORD_PTR dwInstance,DWORD_PTR dwParam1,DWORD_PTR dwParam2);   

// ¼���ص����� 
void CALLBACK waveInProc(HWAVEIN hwi,UINT uMsg,DWORD_PTR dwInstance,DWORD_PTR dwParam1,DWORD_PTR dwParam2)   
{   
	LPWAVEHDR pwh = (LPWAVEHDR)dwParam1;
	UINT buf_len = 0;
	//char buf[1024] = {0};
	char buf[2048] = {0};
	CAnykaIPCameraDlg Testtool;

	//fprintf(stderr, "waveInProc start \n");

	if ((WIM_DATA==uMsg))
	{   
		int temp = 0;
		//�ɵ�����Ƶ���ݷ���С����
		//fprintf(stderr, "Send_audio_data len:%d \n", pwh->dwBytesRecorded);
		buf_len = pwh->dwBytesRecorded;
		memset(buf, 0, 2048);
		memcpy(buf, (char *)pwh->lpData, buf_len);

		TRACE("Send_audio_data len:%d, %d, %02x, %02x,%02x,%02x\n", buf_len, g_total_eln, buf[0], buf[1], buf[2], buf[3]);
		Testtool.Send_audio_data(buf,  buf_len);

		waveInAddBuffer(hwi, pwh, sizeof(WAVEHDR)); 

	}   
}  


void CAnykaIPCameraDlg::WAV_In_Close(void)
{
	UINT i = 0;

	waveInStop(g_hWaveIn);//waveInStop����ֹͣ�Ĳ�����Ƶ���� 
	//ֹͣ¼�������� 
	//MMRESULT waveInReset( HWAVEIN hwi );   
	waveInReset(g_hWaveIn);//ֹͣ¼�� 
#if 1
	//������溯���� 
	//MMRESULT waveInUnprepareHeader( HWAVEIN hwi,LPWAVEHDR pwh, UINT cbwh);    
	for (i=0; i<FRAGMENT_NUM; i++)   
	{   
		waveInUnprepareHeader(g_hWaveIn, &g_wh[i], sizeof(WAVEHDR));
		if (g_wh[i].lpData != NULL)
		{
			delete[] g_wh[i].lpData;
			g_wh[i].lpData = NULL;   
		}

	}
#endif
	//�ر�¼���豸������ 
	//MMRESULT waveInClose( HWAVEIN hwi );  
	waveInClose(g_hWaveIn);  
}


void CAnykaIPCameraDlg::WAV_In_Open(void)
{
	MMRESULT ret =0;
	UINT i= 0;
	WAVEINCAPS wic;  //WAVEINCAPS�ṹ����������Ƶ�����豸������ 
	// open    
	g_wavform.wFormatTag = WAVE_FORMAT_PCM;  //WAVE_FORMAT_PCM��������� 
	g_wavform.nChannels = 1;  // ���� 
	g_wavform.nSamplesPerSec = 8000; // ����Ƶ��
	g_wavform.nAvgBytesPerSec = 8000*16/8;  // ÿ��������
	//g_wavform.nAvgBytesPerSec = g_wavform.nSamplesPerSec*16/8;
	g_wavform.wBitsPerSample = 16; // ������С 
	g_wavform.nBlockAlign = g_wavform.nChannels * g_wavform.wBitsPerSample / 8;
	g_wavform.cbSize = sizeof(g_wavform);  //��С�����ֽڣ����Ӷ���ĸ�ʽ��ϢWAVEFORMATEX�ṹ 

	//��¼���豸���� 


	ret = waveInOpen(NULL, WAVE_MAPPER, &g_wavform, NULL, NULL, WAVE_FORMAT_QUERY);

	ret = waveInOpen(&g_hWaveIn, WAVE_MAPPER, &g_wavform, (DWORD_PTR)waveInProc, 0, CALLBACK_FUNCTION);
	if (MMSYSERR_NOERROR != ret) 
	{
		//AfxMessageBox(_T("waveInOpen fail, pls ckeck"), MB_OK);
		//return;
	}

	//ʶ��򿪵�¼���豸 
	waveInGetDevCaps((UINT_PTR)g_hWaveIn, &wic, sizeof(WAVEINCAPS));  


#if 1
	// prepare buffer  
	for (i=0; i<FRAGMENT_NUM; i++)   
	{   
		g_wh[i].lpData = new char[FRAGMENT_SIZE];   
		g_wh[i].dwBufferLength = FRAGMENT_SIZE;   
		g_wh[i].dwBytesRecorded = 0;   
		g_wh[i].dwUser = NULL;   
		g_wh[i].dwFlags = 0;   
		g_wh[i].dwLoops = 1;   
		g_wh[i].lpNext = NULL;   
		g_wh[i].reserved = 0;  

		//Ϊ¼���豸׼�����溯���� 
		//MMRESULT waveInPrepareHeader(  HWAVEIN hwi,  LPWAVEHDR pwh, UINT bwh );  
		waveInPrepareHeader(g_hWaveIn, &g_wh[i], sizeof(WAVEHDR));  

		//�������豸����һ�����棺 
		//MMRESULT waveInAddBuffer(  HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh );  
		waveInAddBuffer(g_hWaveIn, &g_wh[i], sizeof(WAVEHDR));   
	}  
#endif
	//��ʼ¼������ 
	waveInStart(g_hWaveIn); //��ʼ¼�� 

}



DWORD WINAPI record_wav_main(LPVOID lpParameter)
{
	CAnykaIPCameraDlg Testtool;
	UINT nReturn = 0;
	BOOL start_record = TRUE;

	while (1)
	{

		nReturn = waveInGetNumDevs();//���������豸����Ŀ 
		if (nReturn == 0)
		{
			Sleep(100);
			continue;
		}
		//fprintf(stderr, "g_hBurnThread_record_wav:%d\n",g_hBurnThread_record_wav);
		if (g_hBurnThread_record_wav != INVALID_HANDLE_VALUE)
		{
			//��ʼ¼��
			if (1)//g_start_recode_flag)
			{
				//��ʼ¼��
				if (start_record)
				{
					Testtool.WAV_In_Open();
					start_record = FALSE;
				}

				nReturn = waveInGetNumDevs();//���������豸����Ŀ 
				if (nReturn == 0)
				{
					if (!start_record)
					{
						Testtool.WAV_In_Close();
						start_record = TRUE;
					}

					continue;
				}
			}
			else
			{
				if (!start_record)
				{
					Testtool.WAV_In_Close();
					start_record = TRUE;
				}

			}
			Sleep(100); 
		}
		else
		{
			//ֹͣ¼��
			if (!start_record)
			{
				Testtool.WAV_In_Close();
			}

			break;
		}
	}

	//Testtool.close_record_wav_thread();	
	return 1;
}

void CAnykaIPCameraDlg::close_record_wav_thread(void) 
{
	if(g_hBurnThread_record_wav != INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_hBurnThread_record_wav);
		g_hBurnThread_record_wav = INVALID_HANDLE_VALUE;
	}
}

BOOL CAnykaIPCameraDlg::creat_record_wav_thread(void) 
{

	close_record_wav_thread();
	g_hBurnThread_record_wav = CreateThread(NULL, 0, record_wav_main, this, 0, NULL);
	if (g_hBurnThread_record_wav== INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	return TRUE;
}


//���ţ����ڼ�����

#define  PLAY_WAV_NUM    40
DWORD WINAPI play_wav_main(LPVOID lpParameter)
{
	
	CAnykaIPCameraDlg Testtool;
	UINT nReturn = 0;
	BOOL start_play = TRUE;
	unsigned char *buf = NULL;
	UINT buf_len = 0, idex = 0;
	UINT idex_no_play = 0;
	UINT time1=0, time2 = 0;

	char buf_wav[AUDIO_RECEIVE_BUFFER_SIZE*PLAY_WAV_NUM+1] = {0};

	g_hwo = Testtool.open_pcm_sky();

	while (1)
	{
		if (g_hBurnThread_play_wav != INVALID_HANDLE_VALUE)
		{		
			idex = 0;
			time1 = GetTickCount();

			if (!g_net_conect_success)
			{
				Sleep(10);
				continue;
			}

			while (1)
			{
				if (g_hBurnThread_play_wav == INVALID_HANDLE_VALUE)
				{
					break;
				}

				if (idex == PLAY_WAV_NUM)
				{
					break;
				}

				time2 = GetTickCount();

				

				if (g_play_data[g_audio_read_current_buf_idex].m_empty_flag == FALSE && g_play_data[g_audio_read_current_buf_idex].m_receive_buf_len > 0)
				{
					buf_len = g_play_data[g_audio_read_current_buf_idex].m_receive_buf_len;
					memcpy(&buf_wav[idex*buf_len], g_play_data[g_audio_read_current_buf_idex].m_video_receive_buf, buf_len);
					idex++;

					memset(g_play_data[g_audio_read_current_buf_idex].m_video_receive_buf, 0 , AUDIO_RECEIVE_BUFFER_SIZE);
					g_play_data[g_audio_read_current_buf_idex].m_empty_flag = TRUE;
					g_play_data[g_audio_read_current_buf_idex].m_receive_buf_len = 0;
					g_audio_read_current_buf_idex++;

					if (g_audio_read_current_buf_idex == MAX_AUDIO_BUF_NUM)
					{
						g_audio_read_current_buf_idex = 0;
					}
				}
				else
				{
					//TRACE("no audio data\n");
					//sprintf(g_buf_printf, "no audio data\n");
					//Testtool.test_printf(g_buf_printf);
					//Sleep(10);
				}
			}

			TRACE("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@buf_len:%d, idex:%d\n", buf_len, idex);
			//sprintf(g_buf_printf, "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@buf_len:%d, idex:%d\n", buf_len, idex);
			//Testtool.test_printf(g_buf_printf);
			//if (!g_stop_video_flag)
			{
				Testtool.play_pcm_buf(g_hwo, buf_wav, buf_len*idex);
				//Sleep(16);
			}

		}
		else
		{
			//ֹͣ¼��
			Testtool.close_pcm_sky(g_hwo);
			break;
		}
	}

	//Testtool.close_play_wav_thread();	
	return 1;
}

void CAnykaIPCameraDlg::close_play_wav_thread(void) 
{
	if(g_hBurnThread_play_wav != INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_hBurnThread_play_wav);
		g_hBurnThread_play_wav = INVALID_HANDLE_VALUE;
	}
}

BOOL CAnykaIPCameraDlg::creat_play_wav_thread(void) 
{

	close_play_wav_thread();
	g_hBurnThread_play_wav = CreateThread(NULL, 0, play_wav_main, this, 0, NULL);
	if (g_hBurnThread_play_wav== INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	return TRUE;
}


int CAnykaIPCameraDlg::close_pcm_sky(HWAVEOUT hwo) 
{
	if (g_hwo == NULL)
	{
		return 0;

	}
	waveOutUnprepareHeader(hwo,&header,sizeof(WAVEHDR));
	waveOutReset(hwo);
	while(waveOutUnprepareHeader(hwo,&header,sizeof(WAVEHDR)) ==WAVERR_STILLPLAYING)
	{
	}
	waveOutClose(hwo);
	g_hwo = NULL;
	//g_open_pcm_flag =FALSE;
	return 1;
}

HWAVEOUT CAnykaIPCameraDlg::open_pcm_sky(void) 
{
	HWAVEOUT        hwo;
	WAVEFORMATEX    wfx;
	WAVEOUTCAPS woc; //WAVEINCAPS�ṹ����������Ƶ����豸������


	if (g_hwo != NULL)
	{
		return g_hwo;
	}

	wfx.wFormatTag = WAVE_FORMAT_PCM;//���ò��������ĸ�ʽ
	wfx.nChannels = 1;//������Ƶ�ļ���ͨ������
	wfx.nSamplesPerSec = 9000 ;// + 12;//����ÿ���������źͼ�¼ʱ������Ƶ��
	wfx.wBitsPerSample = 16;	
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nChannels * wfx.wBitsPerSample/8;
	wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;//���ֽ�Ϊ��λ���ÿ����
	wfx.cbSize = sizeof(WAVEFORMATEX);//������Ϣ�Ĵ�С
	waveOutOpen(&hwo, WAVE_MAPPER, &wfx,(DWORD_PTR)waveOutProc,0,CALLBACK_FUNCTION);//��һ�������Ĳ�����Ƶ���װ�������лط�
	waveOutGetDevCaps((UINT_PTR)hwo, &woc,sizeof(WAVEOUTCAPS)); 


	//g_open_pcm_flag = TRUE;
	return hwo;
}

int CAnykaIPCameraDlg::play_pcm_buf_sky(HWAVEOUT hwo, char *buf, UINT buf_len) 
{ 
	if(hwo == NULL)
	{
		return 0;
	}

	ZeroMemory(&header, sizeof(WAVEHDR));
	header.dwBufferLength = buf_len;
	header.lpData = buf;
	header.dwFlags = 0;
	header.dwLoops = 0;
	//int buf_count=0;
	waveOutPrepareHeader(hwo, &header, sizeof(WAVEHDR));
	waveOutWrite(hwo, &header, sizeof(WAVEHDR));

	return 1;
}


//
BOOL CAnykaIPCameraDlg::Send_cmd_data(char commad_type,  char *param)
{
	char *lpBuf = NULL;
	short nBufLen = 0;
	UINT i = 0;
	int nFlags = 0;
	short len_data = 0;
	short check_sum_temp = 0;
	T_PACK_INFO trance = {0};
	char temp[5] = {0};
	int ret = 0;
	BOOL no_data_flag = TRUE;



	USES_CONVERSION;

	trance.data_param = NULL;


	nBufLen = 2;
	trance.commad_type = commad_type;
	nBufLen += 1;
	check_sum_temp += commad_type;

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


	//�������ɸ��ֽڵ����ݳ���+�������
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
	if (len_data != 0 && param != NULL && !no_data_flag)
	{
		strncpy(&lpBuf[2+1], (char *)&len_data, 2);
		strncpy(&lpBuf[2+1+2], param, len_data);
		//strncpy(&lpBuf[5+len_name+4], trance.data_param, len_data+4);
	}
	else
	{
		strncpy(&lpBuf[2+1], temp, 2);
	}
	len_data = len_data + 2;  //��Ϊ���Ӹ��ֽڵĳ���


	strncpy(&lpBuf[2+1+len_data], (char *)&trance.check_sum, 2);

	ret = m_ClientSocket.Socket_Send(lpBuf, nBufLen, 0);
	if (!ret)
	{
		return FALSE;
	}

	if (trance.data_param)
	{
		free(trance.data_param);
	}

	if (lpBuf)
	{
		free(lpBuf);
	}

	return TRUE;
}
