#ifndef HAL_AK3918_H
#define HAL_AK3918_H

#define CLEAR(x) memset(&(x), 0, sizeof(x))

typedef struct _ENC_INPUT_PAR
{
	unsigned long	width;			//ʵ�ʱ���ͼ��Ŀ�ȣ��ܱ�4����
	unsigned long	height;			//ʵ�ʱ���ͼ��ĳ��ȣ��ܱ�2���� 
	unsigned char   kbpsmode;
	signed long	qpHdr;			//��ʼ��QP��ֵ
	signed long	iqpHdr;			//��ʼ��i֡��QPֵ
	signed long minQp;		//��̬���ʲ���[20,25]
	signed long maxQp;		//��̬���ʲ���[45,50]
	signed long framePerSecond; //֡��
	signed long	bitPerSecond;	//Ŀ��bps
	unsigned long 	video_tytes;
	unsigned long	size;
}T_ENC_INPUT;

struct tagRECORD_VIDEO_FONT_CTRL {
	unsigned char *y;
	unsigned char *u;
	unsigned char *v;
	unsigned long color;
	unsigned long width;
};

int ak3918_init_dma_memory();
int camera_open(int width, int height);
int encode_open(T_ENC_INPUT *pencInput);
int video_process_start();

int SetBrightness(int bright);
int SetGAMMA(int cid);
int SetSATURATION(int sat);



#endif
