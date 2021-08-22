// ClientSocket.cpp : implementation file
//

#include "stdafx.h"
#include "anyka_TestTool.h"
#include "ClientSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//SOCKET m_hSocket_Client[MAX_PATH] = {INVALID_SOCKET};
//SOCKET g_hSocket_Client = INVALID_SOCKET;
SOCKET g_hSocket_update_finish_hanlde[UPDATE_MAX_NUM] = {INVALID_SOCKET};
SOCKET g_hSocket_heat[UPDATE_MAX_NUM] = {INVALID_SOCKET};
SOCKET g_hSocket_hanlde[UPDATE_MAX_NUM] = {INVALID_SOCKET};



/////////////////////////////////////////////////////////////////////////////
// CClientSocket

CClientSocket::CClientSocket()
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

CClientSocket::~CClientSocket()
{
	WSACleanup( );
}

BOOL CClientSocket::Socket_Create(UINT idex)  
{
	//int status;
	//unsigned long cmd = 0;

	// m_hSocket������Socket���󣬴���һ������TCP/IP��Socket����������ֵ�����ñ����� 
	if(g_hSocket_hanlde[idex] != INVALID_SOCKET)
	{
		closesocket(g_hSocket_hanlde[idex]);
		g_hSocket_hanlde[idex] = INVALID_SOCKET;
	}

	g_hSocket_hanlde[idex] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (g_hSocket_hanlde[idex] == INVALID_SOCKET) 
	{ 
		return FALSE; 
	}

	unsigned long ul = 1;
	int result;
	result = ioctlsocket(g_hSocket_hanlde[idex], FIONBIO, &ul);
	if (result == SOCKET_ERROR) 
	{
		return FALSE;
	}

	return TRUE;  
}

BOOL CClientSocket::Socket_Close(UINT idex)//�ر�Socket���� 
{  
	if (closesocket(g_hSocket_hanlde[idex]) == SOCKET_ERROR) 	
	{  
		return FALSE; 
	} 
	g_hSocket_hanlde[idex] = INVALID_SOCKET;
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
	result = ioctlsocket(g_hSocket_hanlde[idex], FIONBIO, &ul);
	if (result == SOCKET_ERROR) 
	{
		return FALSE;
	}

#endif
	result = connect(g_hSocket_hanlde[idex], (SOCKADDR*)&m_sockaddr, sizeof(m_sockaddr));
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
		FD_SET(g_hSocket_hanlde[idex], &fd_con);
		FD_SET(g_hSocket_hanlde[idex], &fd_con_w);
		timeout.tv_sec = 1; //2s ���ӳ�ʱ
		timeout.tv_usec = 100*1000;
		result = select(g_hSocket_hanlde[idex] + 1, &fd_con_w, &fd_con, 0, &timeout);
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
	if (ioctlsocket(g_hSocket_hanlde[idex], FIONBIO, &ul) < 0)
	{
		return FALSE;	
	}
#endif
#endif
	
	return TRUE; 
}  
 
///////////////////////////////////////////////////////  
int CClientSocket::Socket_Bind(char* strIP, unsigned int iPort, UINT idex)//�󶨺����� 
{  
	
	if(strlen(strIP) == 0 || iPort == 0) 
	{
		return FALSE;  
	}
	
	memset(&m_sockaddr,0, sizeof(m_sockaddr)); 
	
	m_sockaddr.sin_family = AF_INET;  
	
	m_sockaddr.sin_addr.s_addr = inet_addr(strIP); 
	
	m_sockaddr.sin_port = htons(iPort);  
	
	if (bind(g_hSocket_hanlde[idex], (SOCKADDR*)&m_sockaddr, sizeof(m_sockaddr)) == SOCKET_ERROR ) 	
	{  
		return FALSE; 
	}  
	
	return TRUE; 
}  

//////////////////////////////////////////  
int CClientSocket::Socket_Accept(SOCKET s)//�������Ӻ�����SΪ����Socket�������� 
{   

	int Len = sizeof(m_rsockaddr);  
 
	memset(&m_rsockaddr, 0, sizeof(m_rsockaddr));  
 
	if((m_hSocket = accept(s, (SOCKADDR*)&m_rsockaddr, &Len)) == INVALID_SOCKET) 
	{  
		return FALSE; 
	}  
 
	return TRUE; 
}  


/////////////////////////////////////////////////////  
int CClientSocket::Socket_asyncSelect(HWND hWnd, unsigned int wMsg, long lEvent, UINT idex) 
//file://�¼�ѡ������ 
{  
 
	if(!IsWindow( hWnd ) || wMsg == 0 || lEvent == 0) 
	{
		return FALSE; 
	}
	 
 
	if(WSAAsyncSelect(g_hSocket_hanlde[idex], hWnd, wMsg, lEvent) == SOCKET_ERROR ) 
	{  
		return FALSE; 
	}  
 
	return TRUE; 
}  


//////////////////////////////////////////////////// 
int CClientSocket::Socket_Listen( int iQueuedConnections, UINT idex)//���������� 
{  

	if(iQueuedConnections == 0) 
	{
		return FALSE; 
	}
 
 
	if(listen(g_hSocket_hanlde[idex], iQueuedConnections) == SOCKET_ERROR )  
	{  
		return FALSE; 
	}  

	return TRUE; 
}  


////////////////////////////////////////////////////  
int CClientSocket::Socket_Send( char* strData, int iLen,  UINT idex )//���ݷ��ͺ����� 
{  
	if(strData == NULL || iLen == 0) 
	{	
		return FALSE;  
	}
 
	if( send(g_hSocket_hanlde[idex], strData, iLen, 0) == SOCKET_ERROR ) 
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

	ret = recv(g_hSocket_hanlde[idex], strData, iLen, 0); 
	return ret; 
} 


//******************************************************************

BOOL CClientSocket::Socket_Create_update_finish(UINT idex)  
{
	//int status;
	//unsigned long cmd = 0;
	
	// m_hSocket������Socket���󣬴���һ������TCP/IP��Socket����������ֵ�����ñ����� 
	if(g_hSocket_update_finish_hanlde[idex] != INVALID_SOCKET)
	{
		closesocket(g_hSocket_update_finish_hanlde[idex]);
		g_hSocket_update_finish_hanlde[idex] = INVALID_SOCKET;
	}
	
	g_hSocket_update_finish_hanlde[idex] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (g_hSocket_update_finish_hanlde[idex] == INVALID_SOCKET) 
	{ 
		return FALSE; 
	}
	
	unsigned long ul = 1;
	int result;
	result = ioctlsocket(g_hSocket_update_finish_hanlde[idex], FIONBIO, &ul);
	if (result == SOCKET_ERROR) 
	{
		return FALSE;
	}
	
	return TRUE;  
}
BOOL CClientSocket::Socket_Close_update_finish(UINT idex)//�ر�Socket���� 
{  
	if (closesocket(g_hSocket_update_finish_hanlde[idex]) == SOCKET_ERROR) 	
	{  
		return FALSE; 
	} 
	g_hSocket_update_finish_hanlde[idex] = INVALID_SOCKET;
	//file://����sockaddr_in �ṹ������  
	memset(&m_sockaddr, 0, sizeof(sockaddr_in));  
	memset(&m_rsockaddr, 0, sizeof(sockaddr_in)); 
	
	return TRUE; 
}  
int CClientSocket::Socket_Receive_update_finish( char* strData, int iLen , UINT idex)//���ݽ��պ����� 
{  
	int len = 0; 
	int ret = 0; 
	CString str;
	
	if(strData == NULL) 
	{
		return -1; 
	}
	
	ret = recv(g_hSocket_update_finish_hanlde[idex], strData, iLen, 0); 
	return ret; 
} 

int CClientSocket::Socket_Connect_update_finish(char *lIPAddress, unsigned int iPort, UINT idex)//�������Ӻ����� 
{
	int result;
	unsigned long ul = 1;
	
	m_sockaddr.sin_addr.s_addr = inet_addr(lIPAddress); 
	m_sockaddr.sin_family = AF_INET; 
	m_sockaddr.sin_port = htons( iPort );  

	result = connect(g_hSocket_update_finish_hanlde[idex], (SOCKADDR*)&m_sockaddr, sizeof(m_sockaddr));
	if (1)//result < 0) 
	{
		DWORD ret = GetLastError();
		ret = ret;
		//m_bDisConnect = TRUE;
	}

	INT try_times = 0;
	do 
	{
		struct timeval timeout;
		fd_set fd_con, fd_con_w;
		
		FD_ZERO(&fd_con);
		FD_ZERO(&fd_con_w);
		FD_SET(g_hSocket_update_finish_hanlde[idex], &fd_con);
		FD_SET(g_hSocket_update_finish_hanlde[idex], &fd_con_w);
		timeout.tv_sec = 1; //2s ���ӳ�ʱ
		timeout.tv_usec = 100*1000;
		result = select(g_hSocket_update_finish_hanlde[idex] + 1, &fd_con_w, &fd_con, 0, &timeout);
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
#if 0
	//���������ʽ
	ul = 0;
	if (ioctlsocket(g_hSocket_update_finish_hanlde[idex], FIONBIO, &ul) < 0)
	{
		return FALSE;	
	}
#endif
	return TRUE; 
}  



////////////////////////////////////////////////////////////////////
/*             heat                            */
//�����Ǵ���������socket
BOOL CClientSocket::Heat_Socket_Create(UINT idex)  
{
	//int status;
	//unsigned long cmd = 0;
	
	// m_hSocket������Socket���󣬴���һ������TCP/IP��Socket����������ֵ�����ñ����� 
	if(g_hSocket_heat[idex] != INVALID_SOCKET)
	{
		closesocket(g_hSocket_heat[idex]);
	}
	
	if ((g_hSocket_heat[idex] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) 
	{ 
		return FALSE; 
	} 
	
	return TRUE;  
}
 
BOOL CClientSocket::Heat_Socket_Close(UINT idex)//�ر�Socket���� 
{ 
	if (closesocket(g_hSocket_heat[idex]) == SOCKET_ERROR) 	
	{  
		return FALSE; 
	} 
	//file://����sockaddr_in �ṹ������  
	//memset(&m_sockaddr, 0, sizeof(sockaddr_in));  
	//memset(&m_rsockaddr, 0, sizeof(sockaddr_in)); 

	return TRUE; 
}  

/////////////////////////////////////////   
int CClientSocket::Heat_Socket_Connect(char *lIPAddress, unsigned int iPort, UINT idex)//�������Ӻ����� 
{
	m_sockaddr.sin_addr.s_addr = inet_addr(lIPAddress); 
	m_sockaddr.sin_family = AF_INET; 
	m_sockaddr.sin_port = htons( iPort );  
	
	if(connect(g_hSocket_heat[idex], (SOCKADDR*)&m_sockaddr, sizeof(m_sockaddr)) == SOCKET_ERROR ) 	
	{  
		return FALSE; 
		
	}  
	
	return TRUE; 
}  
 
///////////////////////////////////////////////////////  
int CClientSocket::Heat_Socket_Bind(char* strIP, unsigned int iPort, UINT idex)//�󶨺����� 
{  
	
	if(strlen(strIP) == 0 || iPort == 0) 
	{
		return FALSE;  
	}
	
	memset(&m_sockaddr,0, sizeof(m_sockaddr)); 
	
	m_sockaddr.sin_family = AF_INET;  
	
	m_sockaddr.sin_addr.s_addr = inet_addr(strIP); 
	
	m_sockaddr.sin_port = htons(iPort);  
	
	if (bind(g_hSocket_heat[idex], (SOCKADDR*)&m_sockaddr, sizeof(m_sockaddr)) == SOCKET_ERROR ) 	
	{  
		return FALSE; 
	}  
	
	return TRUE; 
}  


////////////////////////////////////////////////////  
int CClientSocket::Heat_Socket_Send( char* strData, int iLen ,UINT idex)//���ݷ��ͺ����� 
{  
	if(strData == NULL || iLen == 0) 
	{	
		return FALSE;  
	}
 
	if( send(g_hSocket_heat[idex], strData, iLen, 0) == SOCKET_ERROR ) 
	{  
		return FALSE; 
	}  
 
	return TRUE; 
}  

/////////////////////////////////////////////////////  
int CClientSocket::Heat_Socket_Receive( char* strData, int iLen , UINT idex)//���ݽ��պ����� 
{  
	int len = 0; 
	int ret = 0;  

	if(strData == NULL) 
	{
		return -1; 
	}

	ret = recv(g_hSocket_heat[idex], strData, iLen, 0); 
	return ret; 
}  

 

// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CClientSocket, CSocket)
	//{{AFX_MSG_MAP(CClientSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CClientSocket member functions
