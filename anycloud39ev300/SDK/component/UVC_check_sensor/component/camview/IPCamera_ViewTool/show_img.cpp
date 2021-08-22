// show_img.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "show_img.h"


// Cshow_img �Ի���

#define BMP_HEADINFO_LEN	54
#define CLIP255(x) ((x)>0?((x)<255?(x):255):0)

#define IMG_SHOW_MUTIL	2

#define YUV_WIDTH_640P	640
#define YUV_HEIGHT_480P	480
#define YUV_WIDTH_720P	1280
#define YUV_HEIGHT_720P	720
#define YUV_WIDTH_960P	1280
#define YUV_HEIGHT_960P	960


#define IMG_SHOW_LEFT	10
#define IMG_SHOW_TOP	10
#define IMG_SHOW_WIDTH	640
#define IMG_SHOW_HEIGHT	480


typedef enum {
	IMG_MODE_480P = 0,
	IMG_MODE_720P,
	IMG_MODE_960P,

	IMG_MODE_NUM
} T_IMG_MODE;

IMPLEMENT_DYNAMIC(Cshow_img, CDialog)


#define   HIMETRIC_INCH 2540    
LPPICTURE   gpPicture;
LPVOID   pvData   =   NULL;
HGLOBAL   hGlobal;
HANDLE   hFile;
DWORD   dwFileSize = 0;

Cshow_img::Cshow_img(CWnd* pParent /*=NULL*/)
	: CDialog(Cshow_img::IDD, pParent)
{

}

Cshow_img::~Cshow_img()
{
	if (m_pPict)
	{
		m_pPict.Release();
	}
}

void Cshow_img::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Cshow_img, CDialog)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// Cshow_img ��Ϣ�������
void Cshow_img::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CDialog::OnPaint()

	CDC* pDC;   
	pDC=GetDC();   
	draw_Picture(pDC);
	//ShowPicture(pDC,_T("F:\\test.JPG"),0,0,640,480);  
	//ShowPicture(pDC,m_imgpath,0,0,640,480); 
	
}

void Cshow_img::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//GlobalFree(pvData);
	if(gpPicture)    
		gpPicture->Release();

	CDialog::OnClose();
}


BOOL Cshow_img::OnInitDialog()
{

	hFile =CreateFile(m_imgpath,   GENERIC_READ,   0,   NULL,   OPEN_EXISTING,   0,   NULL);    
	_ASSERTE(INVALID_HANDLE_VALUE != hFile);    

	//ȡ���ļ���С    
	dwFileSize = GetFileSize(hFile,   NULL);    
	_ASSERTE(-1 != dwFileSize);  

	if (dwFileSize == 0 || dwFileSize == 0xFFFFFFFF)
	{
		AfxMessageBox(_T("the picture file len is error,pls check"), MB_OK);
		return FALSE;
	}

	//�����ļ���С�����ڴ�    
	hGlobal = GlobalAlloc(GMEM_MOVEABLE,   dwFileSize);    
	_ASSERTE(NULL != hGlobal);    

	pvData   =   GlobalLock(hGlobal);    
	_ASSERTE(NULL != pvData);    
	DWORD   dwBytesRead = 0;    
	//��ȡ�ļ�������ȫ���ڴ�    
	BOOL bRead = ReadFile(hFile,   pvData,   dwFileSize,   &dwBytesRead,   NULL);    
	_ASSERTE(FALSE != bRead);    
	GlobalUnlock(hGlobal);    
	CloseHandle(hFile); 

	LPSTREAM   pstm   =   NULL;  
	HRESULT hr = CreateStreamOnHGlobal(hGlobal,   TRUE,   &pstm);    
	_ASSERTE(SUCCEEDED(hr) && pstm);  

	//ͨ��ͼ���ļ�����IPicture   ����    
	hr   =   OleLoadPicture(pstm,   dwFileSize,   FALSE,   IID_IPicture,   (LPVOID   *)&gpPicture);    
	_ASSERTE(SUCCEEDED(hr) && gpPicture);

	pstm->Release();    
	 
	return TRUE;
}



void Cshow_img::draw_Picture(CDC *pDC) 
{
	//   ͨ��ȫ���ڴ洴��   IStream*   ��ָ��    
	
	HDC   hdc;    
	hdc=pDC->GetSafeHdc(); 

	if(gpPicture)    
	{    
		//   ȡ��ͼƬ�Ŀ�͸�    
		long   hmWidth;    
		long   hmHeight;    
		gpPicture->get_Width(&hmWidth);    
		gpPicture->get_Height(&hmHeight);    
		//���ת��Ϊ����    
		int nWidth =  MulDiv(hmWidth,   GetDeviceCaps(hdc,   LOGPIXELSX),   HIMETRIC_INCH);    
		int nHeight = MulDiv(hmHeight,   GetDeviceCaps(hdc,   LOGPIXELSY),   HIMETRIC_INCH);

		RECT   rc;    
		GetClientRect(&rc);/*ȡ�ÿͻ���*/    
		gpPicture->Render(hdc,   0, 0,   rc.right - rc.left,rc.bottom - rc.top,   0,   hmHeight,   hmWidth,   -hmHeight,   &rc);
		/*��ʾͼƬ*/    
	}      
} 


void Cshow_img::ShowPicture(CDC *pDC, CString m_strBRoute, int x, int y, int width, int height) 
{    
	HGLOBAL   hGlobal;

	HANDLE   hFile =CreateFile(m_strBRoute,   GENERIC_READ,   0,   NULL,   OPEN_EXISTING,   0,   NULL);    
	_ASSERTE(INVALID_HANDLE_VALUE != hFile);    

	//ȡ���ļ���С    
	DWORD   dwFileSize = GetFileSize(hFile,   NULL);    
	_ASSERTE(-1 != dwFileSize);    

	//�����ļ���С�����ڴ�    
	hGlobal = GlobalAlloc(GMEM_MOVEABLE,   dwFileSize);    
	_ASSERTE(NULL != hGlobal);    

	pvData   =   GlobalLock(hGlobal);    
	_ASSERTE(NULL != pvData);    
	DWORD   dwBytesRead = 0;    
	//��ȡ�ļ�������ȫ���ڴ�    
	BOOL bRead = ReadFile(hFile,   pvData,   dwFileSize,   &dwBytesRead,   NULL);    
	_ASSERTE(FALSE != bRead);    
	GlobalUnlock(hGlobal);    
	CloseHandle(hFile); 

	LPSTREAM   pstm   =   NULL;    

	//   ͨ��ȫ���ڴ洴��   IStream*   ��ָ��    
	HRESULT hr = CreateStreamOnHGlobal(hGlobal,   TRUE,   &pstm);    
	_ASSERTE(SUCCEEDED(hr) && pstm);  

	//ͨ��ͼ���ļ�����IPicture   ����    
	if   (gpPicture)    
		gpPicture->Release();    

	hr   =   OleLoadPicture(pstm,   dwFileSize,   FALSE,   IID_IPicture,   (LPVOID   *)&gpPicture);    
	_ASSERTE(SUCCEEDED(hr) && gpPicture);

	pstm->Release();    
	HDC   hdc;    
	hdc=pDC->GetSafeHdc(); 

	if(gpPicture)    
	{    
		//   ȡ��ͼƬ�Ŀ�͸�    
		long   hmWidth;    
		long   hmHeight;    
		gpPicture->get_Width(&hmWidth);    
		gpPicture->get_Height(&hmHeight);    
		//���ת��Ϊ����    
		int nWidth =  MulDiv(hmWidth,   GetDeviceCaps(hdc,   LOGPIXELSX),   HIMETRIC_INCH);    
		int nHeight = MulDiv(hmHeight,   GetDeviceCaps(hdc,   LOGPIXELSY),   HIMETRIC_INCH);

		RECT   rc;    
		GetClientRect(&rc);/*ȡ�ÿͻ���*/    
		gpPicture->Render(hdc,   x,y,   rc.right - rc.left,rc.bottom - rc.top,   0,   hmHeight,   hmWidth,   -hmHeight,   &rc);
		/*��ʾͼƬ*/    
	}      
} 


void Cshow_img::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
	CRect rect;

	this->GetClientRect(&rect);

	m_ImgRect.SetRect(rect.left, rect.top, rect.right, rect.bottom);

	Invalidate();
}
