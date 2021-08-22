#if !defined(AFX_CLIENTSOCKET_H__ABA39C9C_6BB6_4E3B_957B_0262F011E075__INCLUDED_)
#define AFX_CLIENTSOCKET_H__ABA39C9C_6BB6_4E3B_957B_0262F011E075__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClientSocket.h : header file
//

#define  UPDATE_MAX_NUM     100
#define  ONE_TIME_MAX_NUM   4
 
/////////////////////////////////////////////////////////////////////////////
// CClientSocket command target

class CClientSocket : public CSocket
{
// Attributes
public:

// Operations
public:
	CClientSocket();
	virtual ~CClientSocket();

// Overrides
public:

	SOCKADDR_IN m_sockaddr;
	SOCKADDR_IN m_rsockaddr;
//	SOCKET m_hSocket_hanlde;

	BOOL Socket_Create( UINT idex );  
	BOOL Socket_Close(  UINT idex  );//�ر�Socket���� 
	int Socket_Connect(char *lIPAddress, unsigned int iPort , UINT idex );//�������Ӻ����� 
	int Socket_Bind( char* strIP, unsigned int iPort , UINT idex);//�󶨺����� 
	int Socket_Accept( SOCKET s );//�������Ӻ�����SΪ����Socket��������
	int Socket_asyncSelect( HWND hWnd, unsigned int wMsg, long lEvent , UINT idex) ;
	int Socket_Listen( int iQueuedConnections , UINT idex);//����������
	int Socket_Send( char* strData, int iLen , UINT idex);//���ݷ��ͺ�����
	int Socket_Receive( char* strData, int iLen , UINT idex);//���ݽ��պ�����


	int Socket_Receive_update_finish( char* strData, int iLen , UINT idex);//���ݽ��պ����� 
	BOOL Socket_Close_update_finish(UINT idex);//�ر�Socket���� 
	BOOL Socket_Create_update_finish(UINT idex); 
	int Socket_Connect_update_finish(char *lIPAddress, unsigned int iPort, UINT idex);


	int Heat_Socket_Receive( char* strData, int iLen , UINT idex);//���ݽ��պ���
	int Heat_Socket_Send( char* strData, int iLen , UINT idex);//���ݷ��ͺ���
	int Heat_Socket_Bind(char* strIP, unsigned int iPort, UINT idex);//�󶨺����� 
	int Heat_Socket_Connect(char *lIPAddress, unsigned int iPort, UINT idex);//�������Ӻ����� 
	BOOL Heat_Socket_Close(UINT idex);//�ر�Socket���� 
	BOOL Heat_Socket_Create(UINT idex);  

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientSocket)
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CClientSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTSOCKET_H__ABA39C9C_6BB6_4E3B_957B_0262F011E075__INCLUDED_)
