#include "stdafx.h"
#include "ServerInfo.h"

WCHAR * strFunction[COMM_TYPE_CNT][10] = 
			{{L"¼��", L"�Խ�", L"����", L"�ƶ����", L"ֹͣ¼��", NULL, NULL, NULL, NULL, NULL},
			 {L"ͼ������", L"ISP��������", L"��˽�ڵ�", L"��������", L"��������", L"��̨", NULL, NULL, NULL, NULL},
			 {L"¼���ļ���ȡ", L"���������Ϣ��ȡ", L"��˽�ڵ������ȡ", L"�ƶ���������ȡ", L"ͼ�����û�ȡ", L"¼���ļ��б��ȡ", L"ISP������ȡ", NULL, NULL, NULL}};

WCHAR * strError[ERROR_MAX] = {L"δ֪ԭ�����", L"�޴洢�豸", L"�޲ɼ��豸", L"������豸", L"SD������ʧ��",
							   L"SD��ֻ��", L"�ļ��򿪴���", L"�ļ�д����", L"�ļ�������"};

int GetStringFromRetInfo(RETINFO & retInfo, CString & strServerInfo)
{
	strServerInfo.Empty();

	if ((retInfo.nCommandType >= COMM_TYPE_CNT) || (retInfo.nSubCommandType >= 10))
		return -1;

	WCHAR * pstrFun = strFunction[retInfo.nCommandType][retInfo.nSubCommandType];
	if (pstrFun) {
		strServerInfo.Format(L"%s", pstrFun);
	}

	if (retInfo.retType == RET_WARN_CODE) {
		strServerInfo.Append(L"[����]:");
	}else if (retInfo.retType == RET_SUCCESS) {
		strServerInfo.Append(L":����ɹ�!");
		return 0;
	}else if (retInfo.retType == RET_ERROR_CODE) {
		strServerInfo.Append(L"[����]:");
	}

	if (retInfo.retType == RET_ERROR_CODE) {
		if (retInfo.nCode >= ERROR_MAX)
			return -1;

		strServerInfo.Append(strError[retInfo.nCode]);
	}

	return 0;
}