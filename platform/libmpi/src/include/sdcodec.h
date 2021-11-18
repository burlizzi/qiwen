/**
* @file	sdcodec.h
* @brief	Anyka Sound Device Module interfaces header file.
*
* This file declare Anyka Sound Device Module interfaces.\n
* Copyright (C) 2014 Anyka (Guangzhou) Microelectronics Technology Co., Ltd.
* @author	Deng Zhou
* @date	2014-02-21
* @version V0.0.1
* @ref
*/

#ifndef __SOUND_DEVICE_CODEC_H__
#define __SOUND_DEVICE_CODEC_H__

#include "medialib_global.h"

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup AUDIOLIB Audio library
 * @ingroup ENG
 */
/*@{*/


/* @{@name Define audio version*/
/**	Use this to define version string */	
#define AUDIOCODEC_VERSION_STRING		(T_U8 *)"AudioCodec Version V1.16.00_svn5418"
/** @} */
 
#ifdef _WIN32
// #define _SD_MODULE_MIDI_SUPPORT
#define _SD_MODULE_MP3_SUPPORT
#define _SD_MODULE_ENC_MP3_SUPPORT
#define _SD_MODULE_WMA_SUPPORT
#define _SD_MODULE_APE_SUPPORT
#define _SD_MODULE_FLAC_SUPPORT
#define _SD_MODULE_PCM_SUPPORT
#define _SD_MODULE_ADPCM_SUPPORT
#define _SD_MODULE_ENC_ADPCM_SUPPORT
#define _SD_MODULE_AAC_SUPPORT
#define _SD_MODULE_OGG_VORBIS_SUPPORT
#define _SD_MODULE_AMR_SUPPORT
#define _SD_MODULE_AMR_ENC_SUPPORT
#define _SD_MODULE_ENC_AAC_SUPPORT
#define _SD_MODULE_RA8LBR_SUPPORT
#define _SD_MODULE_DRA_SUPPORT
#define _SD_MODULE_AC3_SUPPORT
#define _SD_MODULE_G711_SUPPORT
#define _SD_MODULE_G711_ENC_SUPPORT
#define _SD_MODULE_SBC_SUPPORT
#define _SD_MODULE_SBC_ENC_SUPPORT
#define _SD_MODULE_SPEEX_SUPPORT
#define _SD_MODULE_SPEEX_ENC_SUPPORT
#define _SD_MODULE_SPEEX_WB_SUPPORT
#define _SD_MODULE_SPEEX_WB_ENC_SUPPORT
#define _SD_MODULE_GETSPECTRUM_SUPPORT
#endif 

#define _SD_AUDEC_VOLDB_Q 10

typedef enum
{
	_SD_MEDIA_TYPE_UNKNOWN ,
	_SD_MEDIA_TYPE_MIDI ,
	_SD_MEDIA_TYPE_MP3 ,
	_SD_MEDIA_TYPE_AMR ,
	_SD_MEDIA_TYPE_AAC ,
	_SD_MEDIA_TYPE_WMA ,
	_SD_MEDIA_TYPE_PCM ,
	_SD_MEDIA_TYPE_ADPCM_IMA ,
	_SD_MEDIA_TYPE_ADPCM_MS ,
	_SD_MEDIA_TYPE_ADPCM_FLASH ,
	_SD_MEDIA_TYPE_APE ,
	_SD_MEDIA_TYPE_FLAC ,
	_SD_MEDIA_TYPE_OGG_FLAC ,
	_SD_MEDIA_TYPE_RA8LBR ,
	_SD_MEDIA_TYPE_DRA,
	_SD_MEDIA_TYPE_OGG_VORBIS,
	_SD_MEDIA_TYPE_AC3,
	_SD_MEDIA_TYPE_PCM_ALAW,
	_SD_MEDIA_TYPE_PCM_ULAW,
	_SD_MEDIA_TYPE_SBC,
    _SD_MEDIA_TYPE_MSBC,
	_SD_MEDIA_TYPE_SPEEX,
	_SD_MEDIA_TYPE_SPEEX_WB,
	_SD_MEDIA_TYPE_OPUS	
}T_AUDIO_TYPE;

typedef enum
{
	_SD_BUFFER_FULL ,
	_SD_BUFFER_WRITABLE ,
	_SD_BUFFER_WRITABLE_TWICE ,
	_SD_BUFFER_ERROR
}T_AUDIO_BUF_STATE;

typedef enum
{
	_STREAM_BUF_LEN = 0,
	_STREAM_BUF_REMAIN_DATA,
	_STREAM_BUF_MIN_LEN
}T_AUDIO_INBUF_STATE;

typedef enum
{
    _SD_ENC_SAVE_FRAME_HEAD = 0,
    _SD_ENC_CUT_FRAME_HEAD  = 1
}T_AUDIO_ENC_FRMHEAD_STATE;

/* ���� SPEEX ���뷵�ص����ݴ����ʽ */
typedef enum{
	AKENC_PACK_LENTAG = 0,  //2�ֽ�֡�� + ��������
	AKENC_PACK_OGG = 1,       //ogg�������
    AKENC_PACK_RAWDATA,    //���κθ�����Ϣ�Ĵ���������
    AKENC_PACK_LENSYNC,     //1�ֽ�ͬ���� + 1�ֽ�֡�� + 1�ֽ�CRC���� + ��������
}T_AKENC_PACKET_FORMAT;
#define SPEEX_PACK_LENTAG 	AKENC_PACK_LENTAG
#define SPEEX_PACK_OGG 		AKENC_PACK_OGG
#define SPEEX_PACK_RAWDATA 	AKENC_PACK_RAWDATA
#define SPEEX_PACK_LENSYNC 	AKENC_PACK_LENSYNC


typedef struct
{
	MEDIALIB_CALLBACK_FUN_MALLOC			Malloc;
	MEDIALIB_CALLBACK_FUN_FREE				Free;
	MEDIALIB_CALLBACK_FUN_PRINTF			printf;
	MEDIALIB_CALLBACK_FUN_RTC_DELAY			delay;
	MEDIALIB_CALLBACK_FUN_CMMBSYNCTIME		cmmbsynctime;
	MEDIALIB_CALLBACK_FUN_CMMBAUDIORECDATA  cmmbaudiorecdata;
    MEDIALIB_CALLBACK_FUN_INVALID_DCACHE    invDcache;
}T_AUDIO_CB_FUNS;

typedef struct
{
    T_AUDIO_CB_FUNS cb;
    T_U32	m_Type;
}T_AUDIO_LOG_INPUT;

typedef struct
{
    // in
    // user set the quality of extracted sbc frame
    //  0: (default) high quality, 1: middle quality, 2: half of stereo,
    //  other: set bitpool, see spc spec
    T_U8 g_sbc_extract_bitpool;
    // in
    //  0: (default) encode when necessary. frame sizes may vary.
    //  1: force encode. frame size is constant.
    T_U8 g_sbc_extract_force_encode;

    // out
    // mode of current frame
    //  0: mono, 1: dual, 2: stereo, 3: joint stereo
    T_U8 g_sbc_frame_mode;
    // out
    // extract frame size
    T_S16 g_sbc_extract_frame_size; 
    // out
    // sbc ordinally frame size
    T_S16 g_sbc_frame_size; 
    // out
    // extract frame data buffer
    T_U8 g_sbc_extract_frame_buf[200]; 

}T_AUDIO_SBC_EXTRACT;

typedef struct
{
	T_U32	m_Type;				//media type
	T_U32	m_SampleRate;		//sample rate, sample per second
	T_U16	m_Channels;			//channel number
	T_U16	m_BitsPerSample;	//bits per sample

	T_U32   m_InbufLen;         //input buffer length
	T_U8    *m_szData; 
	T_U32   m_szDataLen;

	union {
		struct
		{
            // cmmb_adts_flag: 
            // bit[1]: �Ƿ�֧��CMMB¼�� 
            // bit[2]: �Ƿ�֧��CMMB��SBR����
            // bit[3]: �Ƿ���Ҫ����A2DP AAC��payload head
            // ���磺
            //       ��ͨAAC��������                ��Ϊ0��
            //       CMMB��SBR����(��¼�ƣ�         ��Ϊ1��
            //       CMMB��SBR����+¼��             ��Ϊ2;
            //       CMMB��SBR���루��¼�ƣ�        ��Ϊ4��
            //       CMMB��SBR����+¼��             ��Ϊ6��
            //       A2DP AAC��Ҫ����payload head   ��Ϊ8
			T_U32	cmmb_adts_flag;
		}m_aac;
		struct  
		{
			T_U32	nFileSize;
		} m_midi;
        struct
        {
            T_U32   ExtractFlag; // 0: normal decode (no extract), 1: extract left channel, 2: extract right channel, 3: extract and mix
            /* 
              setSWdec:
              ������Ӳ�⹦�ܵ�оƬ��ָ������⻹��Ӳ�� -- 0: Ĭ��Ӳ��, 1:ָ��Ϊ��⣻
              ��Ӧû��Ӳ�⹦�ܵ�оƬ�����������������ʲô��������⡣
            */
            T_U8    setSWdec; 
            T_AUDIO_SBC_EXTRACT *tExtractStruct;
        }m_sbc;
        struct  
        {
            T_U32	enhancer;
            T_U32	highpass;
            int  headflag; //SPEEX_WB_PACKET_FORMAT
        } m_speexwb;
	}m_Private;
    /*
    Ϊ���ڸ�ƽ̨���¿��ʱ��ͷ�ļ������޸ģ�����ĳ�FOR_SPOTLIGHT 
    ��Ϊspotlightƽ̨��sbc������ƽֱ̨�ӵ�����Ƶ�⣬��sbc����ƽ̨����ý��⣻
    ��spotlightƽ̨��û��������������sbc����ʱ��Ҫ�������µ�����������
    �����ڷ�spotlightƽ̨��ƽ̨��������ֱ�ӵ�����Ƶ����룬ƽ̨��Ҳ�Ѿ���������������Ҫ��Ƶ��������������
    */
#if 1 //def FOR_SPOTLIGHT //BLUETOOTH_PLAY 
    /* 
    decode volume enable::
    0: ��Ƶ��������ʱ�����������ƣ�Ĭ�Ͻ���ԭʼ�������
    1: ��Ƶ��������ʱ������������, ��������ֵ�� decVolume ��ֵ�����ⲿ������������ֵ
    2: ��Ƶ��������ʱ������������, ��������ֵ�� decVoldb ��ֵ�� ���ⲿ����dbֵ
    */
    T_U32  decVolEna;   
    /* 
    ������������ֵ, ��ֵ��ʽΪ(T_S32)(x.xx*(1<<10)), x.xx=[0.00~7.99]
    �������õ�����ֵ��Ҫ����1.00*(1<<10)����Ϊ�������ܻᵼ�������������������ʧ��
    */
	T_U32  decVolume;   // decode volume value::   this volume is effective, when decVolCtl==1
    /* 
    ��������DB, ��ֵ��ʽΪ(T_S32)(x.xx*(1<<10)), x.xx=[-60.00~8.00]
    �������õ�����ֵ��Ҫ����0db����Ϊ�������ܻᵼ�������������������ʧ��
    �� x.xxx<=-79db, ����������� ��x.xxx>8.0, ���ܻᵼ�����������
    */
    T_S32 decVoldb;
#endif
}T_AUDIO_IN_INFO;

typedef struct
{
	T_AUDIO_CB_FUNS		cb_fun;
	T_AUDIO_IN_INFO		m_info;
    T_AUDIO_CHIP_ID     chip;

    T_VOID              *ploginInfo;
}T_AUDIO_DECODE_INPUT;

typedef struct
{
	volatile T_U8	*pwrite;	//pointer of write pos
	T_U32	free_len;	//buffer free length
	volatile T_U8	*pstart;	//buffer start address
	T_U32	start_len;	//start free length
}T_AUDIO_BUFFER_CONTROL;

/* AAC pfofile */
typedef enum 
{
    AAC_PROFILE_MP = 0,		/* unsupport */
    AAC_PROFILE_LC = 1,
    AAC_PROFILE_SSR = 2     /* unsupport */
}T_AUDIO_AACPROFILE;

/* AAC stream information */
typedef struct
{
    T_AUDIO_AACPROFILE profile;
    T_S32   sampleRate;
    T_S32   channel;
}T_AUDEC_AACSTREAMINFO;

typedef enum{ AMR_ENC_MR475 = 0,
			AMR_ENC_MR515,
			AMR_ENC_MR59,
			AMR_ENC_MR67,
			AMR_ENC_MR74,
			AMR_ENC_MR795,
			AMR_ENC_MR102,
			AMR_ENC_MR122,

			AMR_ENC_MRDTX,

			AMR_ENC_N_MODES	/* number of (SPC) modes */

			} T_AUDIO_AMR_ENCODE_MODE ;


typedef struct
{
	T_U32	m_Type;			//media type
	T_U16	m_nChannel;		//������(2)��������(1)
	T_U16	m_BitsPerSample;//16 bit�̶�(16)
	T_U32	m_nSampleRate;	//������(8000)
	union{
		struct{
			T_AUDIO_AMR_ENCODE_MODE mode;
		}m_amr_enc;
		struct{
			T_U32 enc_bits;
		}m_adpcm;
		struct{
			T_U32 bitrate;
			T_BOOL mono_from_stereo;
		}m_mp3;
        struct{
            T_U32   bitrate;
            T_U16	 m_nChannelOut;
            T_U8    cutAdtsHead;      //T_AUDIO_ENC_FRMHEAD_STATE�ĳ�Ա֮һ����ʶ�����Ƿ���Ҫ����ADTSͷ
		}m_aac;
        struct{
            // recommanded config:
            //  16 blocks, 8 subbands, allocation_method = loudness,
            //  -------------------------------------------
            //  | channel_mode |   mono    | joint stereo |
            //  | sample_rate  | 44.1 | 48 | 44.1 | 48    |
            //  | bitpool      | 31   | 29 | 53   | 51    |
            //  | frame_length | 70   | 66 | 119  | 115   |
            //  -------------------------------------------
            T_U8 channel_mode; // 0: mono, 1: dual, 2: stereo, 3: joint stereo
            T_U8 blocks; // 4,8,12,16
            T_U8 subbands; // 4, 8
            T_U8 allocation_method; // 0: loudness, 1: snr
            T_U8 bitpool;
        }m_sbc;
		struct{
			T_U32 bitrate;   
			T_BOOL cbr;
			T_BOOL dtx_disable;
			char *comments[64];
		}m_speex;
        struct{
            T_BOOL cbr;  //1-CBR(�̶�������)�� 0-VBR(�������)
            T_BOOL dtx_disable;
            T_U32 bitrate;//target bitrate. 0: auto set(15000).
            T_U32 quality;//[0,10]: set quality, overwrite bitrate; 0xff: auto set.
            T_U32 complexity;//[1,10]: set complexity, overwrite bitrate; 0: auto set.
            T_U32 plctuning;//[0,100],Tell the encoder to optimize encoding for a certain percentage of packet loss
            T_U32 highpass;//Set the high-pass filter on(1) or off(0)
            char *comments[64];
            T_U8  headflag; //T_AKENC_PACKET_FORMAT
        }m_speexwb;
		struct{
			T_U32  bitrate;     
			T_BOOL cbr;  		//1-CBR(�̶�������)�� 0-VBR(�������)
			T_BOOL dtx_enable;  //0-��ʹ��DTX��1-����DTX
			T_S16  application; //2048:VOIP    2049:AUDIO  2051:LOWDELAY OTHERS:error
			T_S16  signalType;  //3001:VOICE  3002:MUSIC  -1000:AUTO   OTHERS:error
			T_S8   complexity;  //0-10
			T_U8   headflag;    //T_AKENC_PACKET_FORMAT, Ŀǰֻ���� RAWDATA
			T_U32  stacksz;     // stackaddr's memory size
			T_U8   *stackaddr;  //memory for opus encoder stack
		}m_opus;
	}m_private;
	T_U32 encEndFlag;
}T_AUDIO_ENC_IN_INFO;

typedef struct
{
	T_U16	wFormatTag;
	T_U16	nChannels;
	T_U32	nSamplesPerSec;

	union {
		struct {
			T_U32	nAvgBytesPerSec;
			T_U16	nBlockAlign;
			T_U16	wBitsPerSample;
			T_U16	nSamplesPerPacket;
		} m_adpcm;
	}m_Private;
	
}T_AUDIO_ENC_OUT_INFO;

typedef struct
{
	T_VOID *buf_in;
	T_VOID *buf_out;
	T_U32 len_in;
	T_U32 len_out;
}T_AUDIO_ENC_BUF_STRC;

typedef struct
{
	T_AUDIO_CB_FUNS		cb_fun;
	T_AUDIO_ENC_IN_INFO	enc_in_info;
    T_AUDIO_CHIP_ID     chip;

    T_VOID              *ploginInfo;
}T_AUDIO_REC_INPUT;

typedef enum 
{
	_SD_BM_NORMAL = 0,
	_SD_BM_ENDING = 1,
    _SD_BM_LIVE = 1
} T_AUDIO_BUFFER_MODE;


/**
 * @brief	��ȡ������汾��Ϣ.
 * @author	Deng Zhou
 * @date	2008-04-21
 * @param	[in] T_VOID
 * @return	T_S8 *
 * @retval	���ؿ�汾��
 */
T_S8 *_SD_GetAudioCodecVersionInfo(void);

/**
 * @brief	��ȡ������汾��Ϣ, ����֧����Щ��ʽ�ı����.
 * @author  Tang Xuechai
 * @date	2014-05-05
 * @param	[in] T_AUDIO_CB_FUNS
 * @return	T_S8 *
 * @retval	���ؿ�汾��
 */
T_S8 *_SD_GetAudioCodecVersions(T_AUDIO_CB_FUNS *cb);

/**
 * @brief	���ý����������䴫�����������Ա��ڵ��ûص�ʹ��
 * @author	Deng Zhou
 * @date	2007-10-15
 * @param	[in] audio_input:
 * ������Ϣ������ṹ
 * @param	[in] T_VOID *pHandle:
 * ����ľ��
 * @return	T_VOID *
 * @retval	������Ƶ���ڲ�����ṹ��ָ�룬�ձ�ʾʧ��
 */
T_VOID _SD_SetHandle(T_VOID *audio_decode, T_VOID *pHandle);

/**
 * @brief	����Ƶ�����豸.
 * @author	Deng Zhou
 * @date	2007-10-15
 * @param	[in] audio_input:
 * ������Ϣ������ṹ
 * @param	[in] audio_output:
 * Ҫ��pcm������ṹ
 * @return	T_VOID *
 * @retval	������Ƶ���ڲ�����ṹ��ָ�룬�ձ�ʾʧ��
 */
T_VOID *_SD_Decode_Open(T_AUDIO_DECODE_INPUT *audio_input, T_AUDIO_DECODE_OUT *audio_output);

/**
 * @brief	���û��֡ͷ���ļ�ͷ��AAC raw data���������������������Ϣ.
 * @author	Tang Xuechai
 * @date	2015-03-31
 * @param	[in] audio_decode:
 *          ��Ƶ������ڲ����뱣��ṹ����_SD_Decode_Open()���ص�ָ��
 * @param	[in] info:
 *          AAC ����������Ϣ
 * @return	T_S32
 * @retval	T_TRUE: ���óɹ��� 
 * @retval	T_FALSE:����ʧ��
 */
T_S32 _SD_Decode_SetAACStreamInfo(T_VOID *audio_decode, T_AUDEC_AACSTREAMINFO *info);

/**
 * @brief	��Ƶ����.
 * @author	Deng Zhou
 * @date	2007-10-15
 * @param	[in] audio_decode:
 * ��Ƶ������ڲ����뱣��ṹ
 * @param	[in] audio_output:
 * Ҫ��pcm������ṹ
 * @return	T_S32
 * @retval	������Ƶ����������Ƶ���ݴ�С����byteΪ��λ
 */
T_S32 _SD_Decode(T_VOID *audio_decode, T_AUDIO_DECODE_OUT *audio_output);

/**
 * @brief	�ر���Ƶ�����豸.
 * @author	Deng Zhou
 * @date	2007-10-15
 * @param	[in] audio_decode:
 * ��Ƶ������ڲ����뱣��ṹ
 * @return	T_S32
 * @retval	AK_TRUE :  �رճɹ�
 * @retval	AK_FLASE :  �ر��쳣
 */
T_S32 _SD_Decode_Close(T_VOID *audio_decode);

/**
 * @brief	��Ƶ����seek.
 * @author	Deng Zhou
 * @date	2007-10-15
 * @param	[in] audio_decode:
 * ��Ƶ������ڲ����뱣��ṹ
 * @return	T_S32
 * @retval	AK_TRUE :  seek�ɹ�
 * @retval	AK_FLASE :  seek�쳣
 */
T_S32 _SD_Decode_Seek(T_VOID *audio_decode, T_AUDIO_SEEK_INFO *seek_info);

// #ifdef BLUETOOTH_PLAY
/**
 * @brief	set digital volume
 * @author	Tang Xuechai
 * @date    2012-02-29
 * @param	[in] audio_decode: ��Ƶ������ڲ����뱣��ṹ
 * @param   [in] volume: Ŀ������ֵ��
 *  ��������ֵ, volume��ֵ��ʽΪ(T_S32)(x.xx*(1<<10)), x.xx=[0.00~7.99]
 *  �������õ�����ֵ��Ҫ����1.00*(1<<10)����Ϊ�������ܻᵼ�������������������ʧ��
 * @return	T_S32
 * @retval	AK_TRUE :  set volume sucess
 * @retval	AK_FLASE :	set volume fail
 */
T_S32 _SD_Decode_SetDigVolume(T_VOID *audio_decode, T_U32 volume);

/**
 * @brief	set digital volume
 * @author	Tang Xuechai
 * @date    2012-02-29
 * @param	[in] audio_decode: ��Ƶ������ڲ����뱣��ṹ
 * @param   [in] volume: Ŀ������DBֵ��
 *   ����DBֵ, ��ֵ��ʽΪ(T_S32)(x.xx*(1<<10)), x.xx=[-100.00~8.00], ��[-60.00~8.00]֮�䲽��1db��Ч
 *   �������õ�����ֵ��Ҫ����0db����Ϊ�������ܻᵼ�������������������ʧ��
 *   �� x.xxx<=-79db, ����������� ��x.xxx>8.0, ���ܻᵼ�����������
 * @return	T_S32
 * @retval	AK_TRUE :  set volume sucess
 * @retval	AK_FLASE :	set volume fail
 */
T_S32 _SD_Decode_SetDigVolumeDB(T_VOID *audio_decode, T_S32 volume);

/**
 * @brief	decode one packet data
 * @author	Tang Xuechai
 * @date    2012-02-30
 * @param	[in] audio_decode: decode struct, get from _SD_Decode_Open
 * @param   [in] in: in data stream
 * @param   [in] isize: in data stream length
 * @param   [in/out] audio_output: output information and pcm
 * @return	T_S32
 * @retval	<=0 : decode error
 * @retval	>0 :  output pcm size (byte)
 */
//T_S32 _SD_Decode_OnePacket(T_VOID *audio_decode, T_U8 *in, T_U32 isize, T_AUDIO_DECODE_OUT *audio_output);
//#endif

/**
 * @brief	���ý��뻺����С�ӳٳ���.
 * @author	Tang Xuechai
 * @date	      2012-4-20
 * @param	[in] audio_decode:
 * ��Ƶ������ڲ����뱣��ṹ
 * @param	[in] len:
 * Ŀ�껺���ӳ�����
 * @return	
 */
T_U32 _SD_SetInbufMinLen(T_VOID *audio_decode, T_U32 len);

/**
 * @brief	���ý��뻺�幤��ģʽ.
 * @author	Deng Zhou
 * @date	2009-8-7
 * @param	[in] audio_decode:
 * ��Ƶ������ڲ����뱣��ṹ
 * @param	[in] bufmode:
 * ���幤��ģʽ
 * @return	
 */
T_S32 _SD_SetBufferMode(T_VOID *audio_decode, T_AUDIO_BUFFER_MODE buf_mode);

/**
 * @brief	��ȡwma���������ͣ�LPC/Mid/High rate����
 * @author	Li Jun
 * @date	2010-1-14
 * @param	[in] audio_decode:
 * ��Ƶ������ڲ����뱣��ṹ
 * @return  ���ر��������ͣ�0/1/2�ֱ��ӦLPC/Mid/High rate	
 */
T_S32 _SD_GetWMABitrateType(T_VOID *audio_codec);

/**
 * @brief	�����Ƶ�����ڲ�������free�ռ��С.
 * @author	Deng Zhou
 * @date	2007-10-15
 * @param	[in] audio_decode:
 * ��Ƶ������ڲ����뱣��ṹ
 * @param	[in] buffer_control:
 * ��Ƶ�����ڲ�������״̬�ṹ
 * @return	T_AUDIO_BUF_STATE
 * @retval	������״̬
 */
T_AUDIO_BUF_STATE _SD_Buffer_Check(T_VOID *audio_decode, T_AUDIO_BUFFER_CONTROL *buffer_control);

/**
 * @brief	������Ƶ�����ڲ�������дָ��.
 * @author	Deng Zhou
 * @date	2007-10-15
 * @param	[in] audio_decode:
 * ��Ƶ������ڲ����뱣��ṹ
 * @param	[in] len:
 * ����Ƶ�����ڲ�������д�볤��
 * @return	T_S32
 * @retval	AK_TRUE : ���³ɹ�
 * @retval	AK_FLASE : ����ʧ��
 */
T_S32 _SD_Buffer_Update(T_VOID *audio_decode, T_U32 len);

/**
 * @brief	�����Ƶ�����ڲ�������.
 * @author	Deng Zhou
 * @date	2007-10-15
 * @param	[in] audio_decode:
 * ��Ƶ������ڲ����뱣��ṹ
 * @return	T_S32
 * @retval	AK_TRUE : ����ɹ�
 * @retval	AK_FLASE : ���ʧ��
 */
T_S32 _SD_Buffer_Clear(T_VOID *audio_decode);

/**
 * @brief	��¼���豸.
 * @author	Deng Zhou
 * @date	2007-10-15
 * @param	[in] enc_input:
 * ¼��������Ϣ�ṹ
 * @param	[in] enc_output:
 * ¼�������Ϣ�ṹ
 * @return	T_VOID *
 * @retval	¼����Ƶ�ڲ��ṹָ�룬Ϊ�ձ�ʾ��ʧ��
 */
T_VOID *_SD_Encode_Open(T_AUDIO_REC_INPUT *enc_input, T_AUDIO_ENC_OUT_INFO *enc_output);

/**
 * @brief	��¼�õ�pcm���ݽ��б���.
 * @author	Deng Zhou
 * @date	2007-10-15
 * @param	[in] audio_record:  ¼����Ƶ���ڲ��ṹ
 * @param	[in] enc_buf_strc:  �������bufferָ�볤�Ƚṹ
 * @return	T_S32 
 * @retval	
 *   >0 : ����SBC���룬 ���뷵��ֵ�ĵ�16λ�����ݳ��ȣ���16λ�Ǳ���֡��;
 *        ����������ʽ�����뷵��ֵ�ĵ�16λ�����ݳ��ȣ���16λ��0;
 *   =0 : ����Ч�����������
 *   <0 : ���������������Ч�������
 */
T_S32 _SD_Encode(T_VOID *audio_encode, T_AUDIO_ENC_BUF_STRC *enc_buf_strc);

/**
 * @brief	�ر�¼���豸.
 * @author	Deng Zhou
 * @date	2007-10-15
 * @param	[in] audio_encode:
 * ¼����Ƶ���ڲ��ṹ
 * @return	T_S32 
 * @retval	AK_TRUE : �رճɹ�
 * @retval	AK_FALSE : �ر�ʧ��
 */
T_S32 _SD_Encode_Close(T_VOID *audio_encode);

/**
 * @brief	��ȡ�����ʱ��.
 * @author	Deng Zhou
 * @date	2007-10-15
 * @param	[in] audio_codec:
 * �������ڲ��ṹ
 * @param	[in] codec_flag:
 * ������־ 0������ 1������
 * @return	T_S32 
 * @retval	��ȡ��ʱ��
 */
T_S32 _SD_GetCodecTime(T_VOID *audio_codec, T_U8 codec_flag);


/**
 * @brief	��ȡ��ǰ���뻺���е�����������.
 * @author	Li Jun
 * @date	2007-10-15
 * @param	[in] audio_codec:
 * �������ڲ��ṹ
 * @param	[in] T_U8 *pBuf:
 * �洢�����Ļ���
 * @param	[in] T_U32 *len:
 * �洢�����Ļ����������ĳ���
 * @return	T_VOID 
 */
T_VOID _SD_LogBufferSave(T_U8 *pBuf, T_U32 *len,T_VOID *audio_codec);

/**
 * @brief	�Դ�������ʱ����ƵPCM�ź�, ������Ƶ�ײ�ԭַ����. 
 *          �ýӿڻ����wma�������е�fftģ��,������WMA����ģ�鿪��ʱ����ʹ��
 * @author	Li Jun
 * @date	2011-4-14
 * @param	[in] T_S32 *data
 * ʱ����ƵPCM����    
 * @param	[in] T_U16 size
 * ʱ����ƵPCM���ݵĳ���
 * @param	[in] T_AUDIO_CB_FUNS *cbfun
 * �ص������ṹ��,��Ҫ��malloc,free��printf������
 * @return	T_S32 
 * AK_FALSE �����ڴ����ʧ�ܶ���FALSE
 * AK_TRUE  ����Ƶ�����ݳɹ�,Ƶ��������data��, ��Ч������size/2
 */
T_S32 _SD_GetAudioSpectrum(T_S32 *data, T_U16 size, T_AUDIO_CB_FUNS *cbfun);

/**
 * @brief    �Դ�������ʱ����ƵPCM�ź�, ������Ƶ�ײ�ԭַ����
 * @author	Tang Xuechai
 * @date	2013-11-15
 * @param	[in/out] T_S32 *data
 *               ����������ݣ�����ʵ��
 * @param	[in] T_U16 size
 *               �������ݳ��ȣ���fft���ȣ�Ҳ��ʵ���ĵ������������ֽ���
 * @param	[in] T_AUDIO_CB_FUNS *cbfun
 *               �ص�����ָ�룬��malloc��free��printf
 * @return	T_S32
 *               ���صĵ���������ĵ���һ���������ǶԳƵ�
 **/
T_S32 _SD_GetAudioSpectrum_equNum(T_S32 *data, T_U16 size, T_AUDIO_CB_FUNS *cbfun);


/**
 * @brief    �Դ�������ʱ����ƵPCM�ź�, ������Ƶ�ײ�ԭַ����. 
 * @author	Tang Xuechai
 * @date	2013-11-15
 * @param	[in/out] T_S32 *data
 *               ����������ݣ����Ǹ��������Ҷ���ʵ�����鲿��ʵ�����鲿����������˳������
 * @param	[in] T_U16 size
 *               �������ݳ��ȣ���fft���ȣ�Ҳ��ʵ���ĵ������������ֽ���
 * @param	[in] T_AUDIO_CB_FUNS *cbfun
 *               �ص�����ָ�룬��malloc��free��printf
 * @return	T_S32
 *               ���صĵ���������ĵ���һ���������ǶԳƵ�
 **/
T_S32 _SD_GetAudioSpectrumComplex(T_S32 *data, T_U16 size, T_AUDIO_CB_FUNS *cbfun);


// #if ((defined (NEWWAY_FILL_BUF)) || defined(ANDROID))
/**
 * @brief	��ȡ��Ƶ�����ڲ��������ĵ�ַָ��.
 * @author	Cheng RongFei
 * @date	2011-7-13
 * @param	[in] audio_decode:
 * �������ڲ��ṹ
 * @param	[in] len:
 * ��Ҫһ��д��buffer�����ݳ���
 * @return	T_VOID* 
 * @retval	��ȡbuffer�ĵ�ַָ��
 */
T_VOID* _SD_Buffer_GetAddr(T_VOID *audio_decode, T_U32 len);

/**
 * @brief	������Ƶ�����ڲ�������дָ��.
 * @author	Cheng RongFei
 * @date	2011-7-13
 * @param	[in] audio_decode:
 * ��Ƶ������ڲ����뱣��ṹ
 * @return	T_S32
 * @retval	AK_TRUE : ���³ɹ�
 * @retval	AK_FLASE : ����ʧ��
 */
T_S32 _SD_Buffer_UpdateAddr(T_VOID *audio_decode, T_U32 len);
// #endif

/** 
 * @brief   ��������
 * @author  Zhou Jiaqing
 * @date   2012-5-16
 * @param  [in] audio_codec:¼����Ƶ���ڲ��ṹ
 *		   [in] enc_buf_strc:�������buff
 * @return T_S32
 * @retval ����������ݳ���                                           
 */
T_S32 _SD_Encode_Last(T_VOID *audio_encode,T_AUDIO_ENC_BUF_STRC *enc_buf_strc);

/** 
 * @brief   ���ñ�����
 * @author  Tang Xuechai
 * @date   2018-2-11
 * @param  [in] audio_codec:¼����Ƶ���ڲ��ṹ
 * @return T_S32
 * @retval	AK_TRUE : ���³ɹ�
 * @retval	AK_FLASE : ����ʧ��                                       
 */
T_S32 _SD_Encode_Reset(T_VOID *audio_encode);

/** 
 * @brief   ����ʱ�������Ƿ񷵻�aac����֡ͷ����
 * @author  Tang Xuechai
 * @date   2013-5-20
 * @param  [in] audio_codec:¼����Ƶ���ڲ��ṹ
 *		   [in] flag: T_AUDIO_ENC_FRMHEAD_STATE��ö��֮һ
 *                    _SD_ENC_SAVE_FRAME_HEAD������֡ͷ����
 *                    _SD_ENC_CUT_FRAME_HEAD��������֡ͷ���ݣ�ֻ���ر������������
 * @return T_S32
 * @retval AK_TRUE: ���óɹ�  
 *         AK_FALSE: ����ʧ��
 */
T_S32 _SD_Encode_SetFramHeadFlag(T_VOID *audio_encode, int flag);

/**
 * @brief  ��������ʱ���л�����ʱ����
 *		   ֻ����ogg vorbis��������ʱʹ��       
 * @date  2012-6-6
 * @param [in] audio_decode :��Ƶ������ڲ��ṹ
 * @return T_S32
 * @retval >0: ��ȡ�ɹ�  
 *         <0: ��ȡʧ��
 *         =0: �������ݲ�����ʧ��
 */
T_S32 _SD_Decode_ParseFHead(T_VOID *audio_decode);

/**
 * @brief ��������ʱ���򿪲����ļ�
 *	      ֻ����ogg vorbis��������ʹ��
 * @date 2012-6-6
 * @param [in] audio_input :��Ƶ��Ϣ����ṹ
 *        [in] audio_output:pcm��Ϣ����ṹ
 * @return T_VOID *
 * @retval �������óɹ�����T_VOIDָ�룬���򷵻�AK_NULL
 */
T_VOID *_SD_Decode_Open_Fast(T_AUDIO_DECODE_INPUT *audio_input, T_AUDIO_DECODE_OUT *audio_output);

/**
 * @brief ��ȡ��Ƶ������buf�����Ϣ
 * @date 2012-7-6
 * @param [in] audio_decode :��Ƶ����ṹ��
 *		  [in]  type: T_AUDIO_INBUF_STATE ָ����Ҫ��ȡ����Ϣ���ֱ����ȡ����ֵ��
 *	 			  _STREAM_BUF_LEN,         ������������buf��buf���ȣ�
 *				  _STREAM_BUF_REMAIN_DATA, ������������buf��ʣ��δ�������ݵĳ��ȣ�
 *				  _STREAM_BUF_MIN_LEN,     �������ؽ���������Сbuf����				
 * @return T_S32
 * @retval 0:  buffer�գ�û��ʣ������
 *         >0: buffer��ʣ��û�������ݵĳ���
 *         <0: ����ָ��Ƿ�
 */
T_S32  _SD_Get_Input_Buf_Info(T_VOID *audio_decode,T_AUDIO_INBUF_STATE type);

/** 
 * @brief   ��ȡSBC���룬��ǰ֡�Ƿ��г��ִ��������ı�ʶ
 * @author  Tang Xuechai
 * @date   2017-8-18
 * @param  [in] audio_codec:������
 * @return T_S32
 * @retval  0:   ��ǰ֡�޴�������
                  >0: ��ǰ֡�����˴�������
                  <0: �����쳣
 *         
 */
T_S32 _SD_SBC_GetFrameErrFlag(T_VOID *audio_codec);


const T_VOID *_SD_AAC_login(T_AUDIO_LOG_INPUT *plogInput);
const T_VOID *_SD_SBC_login(T_AUDIO_LOG_INPUT *plogInput);
const T_VOID *_SD_MP3_login(T_AUDIO_LOG_INPUT *plogInput);
const T_VOID *_SD_FLAC_login(T_AUDIO_LOG_INPUT *plogInput);
const T_VOID *_SD_ADPCM_login(T_AUDIO_LOG_INPUT *plogInput);
const T_VOID *_SD_APE_login(T_AUDIO_LOG_INPUT *plogInput);
const T_VOID *_SD_OGG_VORBIS_login(T_AUDIO_LOG_INPUT *plogInput);
const T_VOID *_SD_RA8LBR_login(T_AUDIO_LOG_INPUT *plogInput);
const T_VOID *_SD_MIDI_login(T_AUDIO_LOG_INPUT *plogInput);
const T_VOID *_SD_AMR_login(T_AUDIO_LOG_INPUT *plogInput);
const T_VOID *_SD_AC3_login(T_AUDIO_LOG_INPUT *plogInput);
const T_VOID *_SD_PCM_login(T_AUDIO_LOG_INPUT *plogInput);
const T_VOID *_SD_SPEEX_login(T_AUDIO_LOG_INPUT *plogInput);
const T_VOID *_SD_G711_login(T_AUDIO_LOG_INPUT *plogInput);
const T_VOID *_SD_WMA_login(T_AUDIO_LOG_INPUT *plogInput);
const T_VOID *_SD_SPXWB_login(T_AUDIO_LOG_INPUT *plogInput);

const T_VOID *_SD_G711_Encode_login(T_AUDIO_LOG_INPUT *plogInput);
const T_VOID *_SD_AAC_Encode_login(T_AUDIO_LOG_INPUT *plogInput);
const T_VOID *_SD_AMR_Encode_login(T_AUDIO_LOG_INPUT *plogInput);
const T_VOID *_SD_ADPCM_Encode_login(T_AUDIO_LOG_INPUT *plogInput);
const T_VOID *_SD_MP3_Encode_login(T_AUDIO_LOG_INPUT *plogInput);
const T_VOID *_SD_SPEEX_Encode_login(T_AUDIO_LOG_INPUT *plogInput);
const T_VOID *_SD_SBC_Encode_login(T_AUDIO_LOG_INPUT *plogInput);
const T_VOID *_SD_SPXWB_Encode_login(T_AUDIO_LOG_INPUT *plogInput);
const T_VOID *_SD_OPUS_Encode_login(T_AUDIO_LOG_INPUT *plogInput);

#ifdef __cplusplus
}
#endif

#endif

/* end of sdcodec.h */

/*@}*/
