/**
 * @file medialib_global.h
 * @brief Define the global public types for media lib, video lib and audio lib
 *
 * Copyright (C) 2020 Anyka (Guangzhou) Microelectronics Technology Co., Ltd.
 * @update date 2020-06-11
 * @version 4.3
 */

#ifndef _MEDIA_LIB_GLOBAL_H_
#define _MEDIA_LIB_GLOBAL_H_

#include "anyka_types.h"

typedef struct
{
    T_U16   ResourceID;
    T_U8    *Buff;
    T_U32   Resource_len;
}T_AUDIO_LOADRESOURCE_CB_PARA;

typedef T_VOID (*MEDIALIB_CALLBACK_FUN_PRINTF)(T_pCSTR format, ...);

#if 0
typedef T_S32   SEM_ID;
typedef SEM_ID (*MEDIALIB_CALLBACK_FUN_SEM_CREATE)(T_U32 nSemType, T_U32 nMaxLockCount);
typedef T_BOOL (*MEDIALIB_CALLBACK_FUN_SEM_TAKE)(SEM_ID semID, T_S32 nTimeOut);
typedef T_BOOL (*MEDIALIB_CALLBACK_FUN_SEM_GIVE)(SEM_ID semID);
typedef T_BOOL (*MEDIALIB_CALLBACK_FUN_SEM_FLUSH)(SEM_ID semID);
typedef T_BOOL (*MEDIALIB_CALLBACK_FUN_SEM_DELETE)(SEM_ID semID);
#endif

// file operation
typedef T_S32   (*MEDIALIB_CALLBACK_FUN_OPEN)(T_pVOID lpData);
typedef T_S32   (*MEDIALIB_CALLBACK_FUN_READ)(T_S32 hFile, T_pVOID buf, T_S32 size);
typedef T_S32   (*MEDIALIB_CALLBACK_FUN_WRITE)(T_S32 hFile, T_pVOID buf, T_S32 size);
typedef T_S32   (*MEDIALIB_CALLBACK_FUN_SEEK)(T_S32 hFile, T_S32 offset, T_S32 whence); 
typedef T_S32   (*MEDIALIB_CALLBACK_FUN_TELL)(T_S32 hFile);
typedef T_VOID  (*MEDIALIB_CALLBACK_FUN_CLOSE)(T_S32 hFile);
typedef T_S32   (*MEDIALIB_CALLBACK_FUN_FILE_HANDLE_EXIST)(T_S32 hFile);
typedef T_U32   (*MEDIALIB_CALLBACK_FUN_FILE_GET_LENGTH)(T_S32 hFile);
typedef T_BOOL  (*MEDIALIB_CALLBACK_FUN_FILESYS_ISBUSY)(void);

// memory operation
typedef T_pVOID (*MEDIALIB_CALLBACK_FUN_MALLOC)(T_U32 size);
typedef T_VOID  (*MEDIALIB_CALLBACK_FUN_FREE)(T_pVOID mem);
typedef T_pVOID (*MEDIALIB_CALLBACK_FUN_DMA_MALLOC)(T_U32 size);
typedef T_VOID  (*MEDIALIB_CALLBACK_FUN_DMA_FREE)(T_pVOID mem);
typedef T_pVOID (*MEDIALIB_CALLBACK_FUN_DMA_MEMCPY)(T_pVOID dest, T_pCVOID src, T_U32 size);
typedef T_pVOID (*MEDIALIB_CALLBACK_FUN_VADDR_TO_PADDR)(T_pVOID mem); 
typedef T_U32   (*MEDIALIB_CALLBACK_FUN_MAP_ADDR)(T_U32 phyAddr, T_U32 size); 
typedef T_VOID  (*MEDIALIB_CALLBACK_FUN_UNMAP_ADDR)(T_U32 addr, T_U32 size);

// register operation
typedef T_VOID  (*MEDIALIB_CALLBACK_FUN_REG_BITS_WRITE)(T_U32 phyAddr, T_U32 val, T_U32 mask);

// Dcache operation
typedef T_VOID  (*MEDIALIB_CALLBACK_FUN_FLUSH_DCACHE)(void); // clean and invalidate D-cache
typedef T_VOID  (*MEDIALIB_CALLBACK_FUN_FLUSH_DCACHE_RANGE) (T_U32 start, T_U32 size); // start should be 32byte aligned
typedef T_VOID  (*MEDIALIB_CALLBACK_FUN_INVALID_DCACHE) (T_VOID); // invalidate D-cache
typedef T_VOID  (*MEDIALIB_CALLBACK_FUN_INVALID_DCACHE_RANGE) (T_U32 start, T_U32 size);

// hardware mutex
typedef T_pVOID (*MEDIALIB_CALLBACK_FUN_HW_LOCK)(T_S32 hw_id);
typedef T_S32   (*MEDIALIB_CALLBACK_FUN_HW_UNLOCK)(T_pVOID hLock);

// others
typedef T_VOID  (*MEDIALIB_CALLBACK_FUN_LOADRESOURCE)(T_AUDIO_LOADRESOURCE_CB_PARA *pPara);  //������Դ
typedef T_VOID  (*MEDIALIB_CALLBACK_FUN_RELEASERESOURCE)(T_U8 *Buff);    //�ͷ��������Դ

typedef T_BOOL  (*MEDIALIB_CALLBACK_FUN_RTC_DELAY) (T_U32 ulTicks);


typedef struct
{
    T_S32   real_time;

    union {
        struct{
            T_U32 nCurBitIndex;  //����Ҫ��
            T_U32 nFrameIndex;   //�ڼ�֡
        } m_ape;
        struct{
            T_U32 Indx;          //seekλ�ÿ�ʼ����ǰpage���е�packet��
            T_U32 offset;        //seek�󣬵�ǰpageʣ����Ҫ��������ݴ�С
            T_BOOL flag;         //seek��־λ��seekʱ��1
            T_U32  last_granu;   //��������һ��pageʱ�������sample�� 
            T_U32  now_granu;    //�����굱ǰpageʱ�ӳ�����sample��
            T_BOOL is_eos;       //����������־λ
            T_U32  re_data;      //seek�󣬵�ǰpage���в�����packet�Ĵ�С
            T_U32 pack_no;       //seek��λ���ڵ�ǰpage�еĵڼ���packet
            T_U32 list[255];     //��¼һ��ogg page��ÿ��packet��ƫ��λ��
        }m_speex;
        struct{
            T_U32 Indx;          //seekλ�ÿ�ʼ����ǰpage���е�packet��
            T_U32 offset;        //seek�󣬵�ǰpageʣ����Ҫ��������ݴ�С
            T_BOOL flag;         //seek��־λ��seekʱ��1
            T_U32  last_granu;   //��������һ��pageʱ�������sample�� 
            T_U32  now_granu;    //�����굱ǰpageʱ�ӳ�����sample��
            T_BOOL is_eos;       //����������־λ
            T_U32  re_data;      //seek�󣬵�ǰpage���в�����packet�Ĵ�С
            T_U32 pack_no;       //seek��λ���ڵ�ǰpage�еĵڼ���packet
            T_U32 list[255];     //��¼һ��ogg page��ÿ��packet��ƫ��λ��
        }m_speexwb;
        struct{
            T_U8    secUse;     //�Ѿ���ȡ��section��Ŀ
            T_U8    secLen;     //һ��page�а�����section��
            T_U8    tmpSec;     //�Ѿ������section��Ŀ
            T_BOOL  is_eos;     //�ǲ������һ��page
            T_BOOL  is_bos;     //�ǲ��ǵ�һ��page
            T_U8    endpack;    //��ǰpage�����һ��packet��λ��
            //�������sample����һ��64λ������Ŀǰֻȡ��32λ
            T_U32   gos;        //�����굱ǰpage�������ܵ�sample������32λ
            T_U32   high_gos;   //�����굱ǰpage�������ܵ�sample������32λ��(��ʱ���ã������Ժ���Ҫ)
            T_U8    list[255];  //��¼һ��page��ÿ��section�Ĵ�С��һ��page����ຬ��255��section
        }m_vorbis;
        struct{
            T_U32 Indx;         //seekλ�ÿ�ʼ����ǰpage���е�packet��
            T_U32 offset;       //seek�󣬵�ǰpageʣ����Ҫ��������ݴ�С
            T_BOOL flag;        //seek��־λ��seekʱ��1
            T_U32  last_granu;  //��������һ��pageʱ�������sample�� 
            T_U32  now_granu;   //�����굱ǰpageʱ�ӳ�����sample��
            T_BOOL is_eos;      //����������־λ
            T_U32  re_data;     //seek�󣬵�ǰpage���в�����packet�Ĵ�С
            T_U32 pack_no;      //seek��λ���ڵ�ǰpage�еĵڼ���packet
            T_U32 list[255];    //��¼һ��ogg page��ÿ��packet��ƫ��λ��
        }m_opus;
    }m_Private;
}T_AUDIO_SEEK_INFO;

typedef enum 
{
    AUDIOLIB_CHIP_UNKNOW,
    AUDIOLIB_CHIP_AK10XX,
    AUDIOLIB_CHIP_AK10XXC,
    AUDIOLIB_CHIP_AK10XXL,
    AUDIOLIB_CHIP_AK10XXT,
    AUDIOLIB_CHIP_AK11XX,
    AUDIOLIB_CHIP_AK37XX,
    AUDIOLIB_CHIP_AK37XXL,
    AUDIOLIB_CHIP_AK37XXC,
    AUDIOLIB_CHIP_AK39XX,
    AUDIOLIB_CHIP_AK98XX,
    AUDIOLIB_CHIP_AK39XXE,
    AUDIOLIB_CHIP_AK10XXT2,
    AUDIOLIB_CHIP_AK39XXEV2,
    AUDIOLIB_CHIP_AK39XXEV3,
    AUDIOLIB_CHIP_AK10XXT3,
    AUDIOLIB_CHIP_AK39XXEV5,
    AUDIOLIB_CHIP_AK37XXD,
    AUDIOLIB_CHIP_AK10XXF
}T_AUDIO_CHIP_ID;

typedef struct
{
	T_U32	m_SampleRate;		//sample rate, sample per second
	T_U16	m_Channels;			//channel number
	T_U16	m_BitsPerSample;	//bits per sample 

	T_U32	m_ulSize;
	T_U32	m_ulDecDataSize;
	T_U8	*m_pBuffer;
}T_AUDIO_DECODE_OUT;

//just for audio codec lib
typedef T_S32 (*MEDIALIB_CALLBACK_FUN_CMMBSYNCTIME)(T_VOID *pHandle, T_U32 timestamp);
typedef T_VOID (*MEDIALIB_CALLBACK_FUN_CMMBAUDIORECDATA)(T_VOID *pHandle, T_U8 *buf, T_S32 len);
//end of just for audio codec lib
#endif//_MEDIA_LIB_GLOBAL_H_
