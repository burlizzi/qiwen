#include "StdAfx.h"
#include "ClientSocket.h"


int g_hSocket_hanlde = INVALID_SOCKET;


CClientSocket::CClientSocket(void)
{
	WSADATA wsaD;
	int result;
	UINT i = 0;
	WORD wVersionRequested;


	// m_LastError�������ַ�������,��ʼ���������������˵�����ַ�����  	
	// ��ʼ������sockaddr_in�ṹ������ǰ�ߴ�ſͻ��˵�ַ�����߶�Ӧ�ڷ������˵�ַ; 

	memset(&m_sockaddr, 0, sizeof(m_sockaddr));  
	memset(&m_rsockaddr, 0, sizeof(m_rsockaddr)); 

	wVersionRequested = MAKEWORD( 2, 2 );


	result = WSAStartup(0x0202, &wsaD); //��ʼ��WinSocket��̬���ӿ�
	if(result != 0) // ��ʼ��ʧ�ܣ�  	
	{ 
		return; 
	} 
}

CClientSocket::~CClientSocket(void)
{
	WSACleanup( );
}

BOOL CClientSocket::Socket_Create(UINT idex)  
{
	//int status;
	//unsigned long cmd = 0;

	// m_hSocket������Socket���󣬴���һ������TCP/IP��Socket����������ֵ�����ñ����� 
	if(g_hSocket_hanlde != INVALID_SOCKET)
	{
		closesocket(g_hSocket_hanlde);
		g_hSocket_hanlde = INVALID_SOCKET;
	}

	g_hSocket_hanlde = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (g_hSocket_hanlde < 0) 
	{ 
		return FALSE; 
	}

	unsigned long ul = 1;
	int result;
	result = ioctlsocket(g_hSocket_hanlde, FIONBIO, &ul);
	if (result == SOCKET_ERROR) 
	{
		return FALSE;
	}

	return TRUE;  
}

BOOL CClientSocket::Socket_Close(UINT idex)//�ر�Socket���� 
{  
	if (closesocket(g_hSocket_hanlde) == SOCKET_ERROR) 	
	{  
		return FALSE; 
	} 
	g_hSocket_hanlde = INVALID_SOCKET;
	//file://����sockaddr_in �ṹ������  
	memset(&m_sockaddr, 0, sizeof(sockaddr_in));  
	memset(&m_rsockaddr, 0, sizeof(sockaddr_in)); 

	return TRUE; 
}  

/////////////////////////////////////////  
int CClientSocket::Socket_Connect(char *lIPAddress, unsigned int iPort, UINT idex)//�������Ӻ����� 
{
	int result;
	unsigned long ul = 1;

	m_sockaddr.sin_addr.s_addr = inet_addr(lIPAddress); 
	m_sockaddr.sin_family = AF_INET; 
	m_sockaddr.sin_port = htons( iPort );  
#if 0
	//connect��������ʽ
	ul = 1;
	result = ioctlsocket(g_hSocket_hanlde, FIONBIO, &ul);
	if (result == SOCKET_ERROR) 
	{
		return FALSE;
	}

#endif
	result = connect(g_hSocket_hanlde, (SOCKADDR*)&m_sockaddr, sizeof(m_sockaddr));
	if (1)//result < 0) 
	{
		DWORD ret = GetLastError();
		ret = ret;
		//m_bDisConnect = TRUE;
	}

#if 1
	INT try_times = 0;
	do 
	{
		struct timeval timeout;
		fd_set fd_con, fd_con_w;

		FD_ZERO(&fd_con);
		FD_ZERO(&fd_con_w);
		FD_SET(g_hSocket_hanlde, &fd_con);
		FD_SET(g_hSocket_hanlde, &fd_con_w);
		timeout.tv_sec = 1; //2s ���ӳ�ʱ
		timeout.tv_usec = 100*1000;
		result = select(g_hSocket_hanlde + 1, &fd_con_w, &fd_con, 0, &timeout);
		try_times ++;
		//	Sleep(100);
	} while(0); // ((result == 0) && (try_times < 10));
	if (result <= 0) 
	{
		DWORD ret = WSAGetLastError();
		return FALSE;
	}
	else
	{
		DWORD ret = WSAGetLastError();
		ret = FALSE;

	}

	//m_stSockInfo.recv_fd = m_stSockInfo.send_fd;

#if 1
	//���������ʽ
	ul = 0;
	if (ioctlsocket(g_hSocket_hanlde, FIONBIO, &ul) < 0)
	{
		return FALSE;	
	}
#endif
#endif

	return TRUE; 
}  

////////////////////////////////////////////////////  
int CClientSocket::Socket_Send( char* strData, int iLen,  UINT idex )//���ݷ��ͺ����� 
{  
	if(strData == NULL || iLen == 0) 
	{	
		return FALSE;  
	}

	if( send(g_hSocket_hanlde, strData, iLen, 0) == SOCKET_ERROR ) 
	{  
		return FALSE; 
	}  

	return TRUE; 
}  

/////////////////////////////////////////////////////  
int CClientSocket::Socket_Receive( char* strData, int iLen, UINT idex )//���ݽ��պ����� 
{  
	int len = 0; 
	int ret = 0; 
	CString str;

	if(strData == NULL) 
	{
		return -1; 
	}

	ret = recv(g_hSocket_hanlde, strData, iLen, 0); 
	return ret; 
} 