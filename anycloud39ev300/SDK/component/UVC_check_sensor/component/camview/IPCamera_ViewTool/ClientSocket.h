#pragma once

class CClientSocket
{
public:
	CClientSocket(void);
	~CClientSocket(void);

public:
	SOCKADDR_IN m_sockaddr;
	SOCKADDR_IN m_rsockaddr;


	BOOL Socket_Create( UINT idex );  
	BOOL Socket_Close(  UINT idex  );//�ر�Socket���� 
	int Socket_Connect(char *lIPAddress, unsigned int iPort , UINT idex );//�������Ӻ����� 
	int Socket_Send( char* strData, int iLen , UINT idex);//���ݷ��ͺ�����
	int Socket_Receive( char* strData, int iLen , UINT idex);//���ݽ��պ�����
	BOOL Heat_Socket_Create(UINT idex) ;  
	BOOL Heat_Socket_Close(UINT idex); //�ر�Socket���� 
	int Heat_Socket_Connect(char *lIPAddress, unsigned int iPort, UINT idex); //�������Ӻ�����
	int Heat_Socket_Send( char* strData, int iLen ,UINT idex); //���ݷ��ͺ����� 
	int Heat_Socket_Receive( char* strData, int iLen , UINT idex); //���ݽ��պ����� 
	BOOL Rev_Socket_Create(UINT idex);  
	BOOL Rev_Socket_Close(UINT idex);//�ر�Socket���� 
	int Rev_Socket_Connect(char *lIPAddress, unsigned int iPort, UINT idex);
	int Rev_Socket_Send( char* strData, int iLen ,UINT idex);//���ݷ��ͺ����� 
	int Rev_Socket_Receive( char* strData, int iLen , UINT idex);//���ݽ��պ�����

	int Socket_Receive_update_finish( char* strData, int iLen , UINT idex);//���ݽ��պ����� 
	BOOL Socket_Close_update_finish(UINT idex);//�ر�Socket���� 
	BOOL Socket_Create_update_finish(UINT idex); 
	int Socket_Connect_update_finish(char *lIPAddress, unsigned int iPort, UINT idex);



	int Socket_server_Receive( char* strData, int iLen );//���ݽ��պ����� 
	int Socket_server_Send( char* strData, int iLen );//���ݷ��ͺ����� 
	int Socket_server_setsockopt( void);//���ݽ��պ����� 
	int Socket_server_Listen( int iQueuedConnections );//���������� 
	int Socket_server_asyncSelect(HWND hWnd, unsigned int wMsg, long lEvent) ;
	int Socket_server_Accept();//�������Ӻ�����SΪ����Socket��������
	int Socket_server_Bind(char* strIP, unsigned int iPort);//�󶨺�����
	int Socket_server_Connect(char *lIPAddress, unsigned int iPort);//�������Ӻ����� 
	BOOL Socket_server_Close(void);//�ر�Socket���� 
	BOOL Socket_server_Create(void);

};
