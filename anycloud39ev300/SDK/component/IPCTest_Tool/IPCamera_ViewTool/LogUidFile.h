#pragma once

#ifndef  _LOG_UID_FILE_H_
#define  _LOG_UID_FILE_H_

#define  LOG_LINE_TIME  1
#define  LOG_LINE_DATE  (1<<1)


class CLogUidFile
{
public:
    CLogUidFile();   
    CLogUidFile(TCHAR *name);   
    ~CLogUidFile();

protected:
    HANDLE  hLogFile;
    TCHAR   file_name[MAX_PATH];

public:
    BOOL    InitFile();//ʵʼ��
    DWORD   WriteLogFile(UCHAR wFlag, const char *fmt, ...);//д����
    UINT    ReadLogFile();//��LOG
    VOID    SetFileName(TCHAR *name);//�����ļ���
    BOOL    CheckFileSize(DWORD file_max_size);//����ļ���С

protected:
    BOOL    GetCurPCPath(PTCHAR curPCPath, int buf_len);//��ȡ��ǰ·��
};

#endif