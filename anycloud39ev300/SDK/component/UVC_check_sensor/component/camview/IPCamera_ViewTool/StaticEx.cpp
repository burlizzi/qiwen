#include "StdAfx.h"
#include "StaticEx.h"

IMPLEMENT_DYNAMIC(CStaticEx, CStatic)

CStaticEx::CStaticEx()
{
	m_foreColor=0;
	m_font = new CFont;
    m_font->CreateFontIndirect(&m_lf);
}

CStaticEx::~CStaticEx()
{
}


BEGIN_MESSAGE_MAP(CStaticEx, CStatic)
    ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

 

// CStaticEx ��Ϣ�������
HBRUSH CStaticEx::CtlColor(CDC* pDC, UINT idex)
{
	pDC->SetBkMode(TRANSPARENT);
	//pDC->SetBkMode(236,233,216);
	//pDC->SetBkColor(RGB(236,233,216));
	pDC->SetTextColor(m_foreColor);
	
	
	return (HBRUSH)GetStockObject(HOLLOW_BRUSH);
}


void CStaticEx::SetFontColor(COLORREF clr)
{
	m_foreColor=clr;
}


LRESULT CStaticEx::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(WM_SETTEXT == message)
	{
		CRect rect;
		GetWindowRect(&rect);
		CWnd *pParentWnd = GetParent();
		
		if(pParentWnd)
		{
			pParentWnd->ScreenToClient(&rect);
			pParentWnd->InvalidateRect(&rect);
		}
	}
	return CStatic::DefWindowProc(message, wParam, lParam);
}


BOOL CStaticEx::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	return TRUE;//CStatic::OnEraseBkgnd(pDC);
}