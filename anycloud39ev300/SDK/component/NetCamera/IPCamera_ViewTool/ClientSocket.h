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

};
