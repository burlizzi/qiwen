// EditEx.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LimitEdit.h"


// CLimitEdit

IMPLEMENT_DYNAMIC(CLimitEdit, CEdit)

CLimitEdit::CLimitEdit()
{

}

CLimitEdit::~CLimitEdit()
{
}


BEGIN_MESSAGE_MAP(CLimitEdit, CEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()



// CLimitEdit ��Ϣ�������



void CLimitEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	TCHAR ch=nChar;
	if(!(ch>=_T('0')&&ch<=_T('9')|| ch==VK_BACK))
	{
		return;
	}
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}
