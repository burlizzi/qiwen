// NetCtrl.h: interface for the CNetCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETCTRL_H__280D9032_62DE_4686_9C3A_8790E62B6026__INCLUDED_)
#define AFX_NETCTRL_H__280D9032_62DE_4686_9C3A_8790E62B6026__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#define TRANS_STACKSIZE		(1024*300)

#define ISP_ATTR_ID_SIZE		2
#define ISP_CMD_TYPE_SIZE		2

#define ISP_PACKET_HEAD_SIZE	(ISP_ATTR_ID_SIZE+ISP_CMD_TYPE_SIZE)
#define ISP_PARM_LEN_SIZE		4

typedef enum {
	//ǰn��ΪISPģ��ṹ��id
	ISP_BB = 0,   			//��ƽ��
	ISP_LSC,				//��ͷУ��
	ISP_RAW_LUT,			//raw gamma
	ISP_NR,					//NR
	ISP_3DNR,				//3DNR

	ISP_GB,					//��ƽ��
	ISP_DEMO,				//DEMOSAIC
	ISP_GAMMA,				//GAMMA
	ISP_CCM,				//��ɫУ��
	ISP_FCS,				//FCS

	ISP_WDR,				//WDR
	//ISP_EDGE,				//EDGE
	ISP_SHARP,				//SHARP
	ISP_SATURATION,			//���Ͷ�
	ISP_CONTRAST,			//�Աȶ�

	ISP_RGB2YUV,			//rgb to yuv
	ISP_YUVEFFECT,			//YUVЧ��
	ISP_DPC,				//����У��
	ISP_ZONE_WEIGHT,		//Ȩ��ϵ��
	ISP_AF,					//AF

	ISP_WB,					//WB
	ISP_EXP,				//Expsoure
	ISP_MISC,				//����
	ISP_Y_GAMMA,			//y gamma
	ISP_HUE,				//hue

	//ͳ�ƽ��
	ISP_3DSTAT,				//3D����ͳ��
	ISP_AESTAT,				//AEͳ��
	ISP_AFSTAT,				//AFͳ��
	ISP_AWBSTAT,			//AWBͳ��

	ISP_SENSOR,				//sensor����

	//������һЩ����ֵ����չֵ
	ISP_PARM_CODE,			//����������
	ISP_REGISTER,			//�Ĵ�������

	ISP_RAW_IMG,			//һ֡rawͼ������
	ISP_YUV_IMG,			//һ֡yuvͼ������
	ISP_ENCODE_IMG,			//һ֡encodeͼ������

	ISP_CFG_DATA,			//day cfg data
	
	ISP_HEARTBEAT,			//heartbeat

    ISP_ATTR_TYPE_NUM
} T_ATTR_TYPE;

typedef enum {
    CMD_GET = 0,
	CMD_REPLY,	// get�Ļظ�
	CMD_SET,
	CMD_RET,	//set�Ľ���ظ�
	CMD_GET_TXT,
	CMD_REPLY_TXT,

    CMD_TYPE_NUM
} T_CMD_TYPE;

typedef void (*RECVCB)(short addr_id, short cmd_type, char* pData, int len, void *pParam);

class CNetCtrl  
{
public:	
	bool m_bConnect;
	HANDLE m_recvtask;


	HANDLE m_hBurnThread_rev_data;
	BOOL test_pass_flag;
	BOOL heat_close_flag;
	HANDLE m_thread_heat;
	BOOL g_test_flag;

	
	CNetCtrl();
	virtual ~CNetCtrl();
	bool TcpClientConnect(unsigned long remote_ip, UINT remote_port);
	bool TcpClientClose();
	bool Socket_close();
	bool IsConnected();
	bool SendCommand(short addr_id, short cmd_type, char* pData, int len);
	int SetRecvCallBack(RECVCB pCB, void *pParam);
	int Socket_Send( char* strData, int iLen );//���ݷ��ͺ����� 
	BOOL creat_socket(char *lIPAddress, UINT remote_port);


	BOOL Heat_Socket_Create(void);  
	BOOL Heat_Socket_Close(void); //�ر�Socket���� 
	int Heat_Socket_Connect(char *lIPAddress, unsigned int iPort); //�������Ӻ����� 
	int Heat_Socket_Bind(char* strIP, unsigned int iPort); //�󶨺����� 
	int Heat_Socket_Send( char* strData, int iLen ); //���ݷ��ͺ����� 
	int Heat_Socket_Receive(SOCKET hSocket_heat,  char* strData, int iLen ); //���ݽ��պ����� 

	BOOL create_thread_heat(char *ipaddr, UINT net_uPort);
	void close_thread_heat(); 

	SOCKET m_hSocket_heat;
	SOCKADDR_IN m_sockaddr;
	SOCKADDR_IN m_rsockaddr;

	SOCKET m_tcp_client;
	RECVCB m_pRecvCB;
	void *m_pRecvCBP;
};

#endif // !defined(AFX_NETCTRL_H__280D9032_62DE_4686_9C3A_8790E62B6026__INCLUDED_)
