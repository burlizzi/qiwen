
/**
 * @file 
 * @brief:
 *
 * This file provides 
 * Copyright (C) 2017 Anyka (GuangZhou) Software Technology Co., Ltd.
 * @author 
 * @date 2017-2-09
 * @version 1.0
 */

#include "print.h"
#include <string.h>  //for "list.h"  size_t
#include "list.h"
#include "ioctl.h"

#ifndef __DEVICE_H__
#define __DEVICE_H__

#define OS_TICK (5)


#define DEV_MAJOR_ID_BEGIN 100
#define DEV_MINOR_ID_BEGIN 0

#define DEV_ID_MAJOR_MAX (DEV_MAJOR_ID_BEGIN + 30)
#define DEV_ID_MINOR_MAX (DEV_MINOR_ID_BEGIN + 4)


#define PLATFORM_DEV_MAX_NB  ((DEV_ID_MAJOR_MAX - DEV_MAJOR_ID_BEGIN + 1)*(DEV_ID_MINOR_MAX - DEV_MINOR_ID_BEGIN+ 1))


#define MAJOR_ID_OFFSET  (16)
#define MINORMASK	((1U << MAJOR_ID_OFFSET) - 1)

#define MAJOR(dev)	((unsigned int) ((dev) >> MAJOR_ID_OFFSET))
#define MINOR(dev)	((unsigned int) ((dev) & MINORMASK))



typedef long    T_DrvMutex;
typedef long    T_DrvSem;
typedef void    (*f_Drv_Complete)(int dev_id);   



typedef struct
{
	/*
	* device_id:��16λλ���豸�ţ���16λΪ���豸�ţ����Ψһ���豸��
	*/
	int  device_id;


	/*
	* drv_open_func :
	*
	* dev_id  : �豸��
	* data     : �豸������Ҫ��������Ľṹ���ַ����uart�Ĳ����ʵ�
	*
	*return    : �ɹ������豸��
	*              ʧ�ܷ��ظ������������ִ���ο�ϵͳ��Error Code
	*/
	int            (*drv_open_func) (int dev_id, void *data);

	/*
	* drv_close_func :
	* dev_id  : �豸��
	*
	*return    : �ɹ�����0
	*              ʧ�ܷ���-1
	*/
    int            (*drv_close_func)(int dev_id);

	/*
	* drv_read_func:
	* dev_id  : �豸��
	* data   : ������ݵĵ�ַ
	* len     : �����ݵĳ���
	*
	*return  : �ɹ������Ѷ���Ч���ݵĳ���
	*            ʧ�ܷ��ظ������������ִ���ο�ϵͳ��Error Code
	*/	
    int            (*drv_read_func) (int dev_id, void *data, unsigned int len);

	/*
	* drv_write_func:
	* dev_id  : �豸��
	* data   : ������ݵĵ�ַ
	* len     : д���ݵĳ���
	*
	*return  : �ɹ�������д��Ч���ݵĳ���
	*            ʧ�ܷ��ظ������������ִ���ο�ϵͳ��Error Code
	*/
    int            (*drv_write_func)(int dev_id, const void  *data, unsigned int len);

	/*
	* drv_ioctl_func:
	* dev_id  : �豸��	
	* cmd   : ioctl���
	*           bit31-30  : ��д����.
	*             0      0   : û�в���
	*             0      1   : д
	*             1      0   : ��
	*             1      1   : ��д
	*
	*		bit29-16  : ��������
	*		bit15-8    :����һ���豸���������Ҫһһ��Ӧ
	*	       bit 7-0    :ÿһ�������Ӧ��Ӧ����
	* 		
	*data   : iocltl��������Ľṹ���ַ
	*
	*return  : �ɹ����ظ���ʵ�ʵ������豸����(���Ľ�)
	*            ʧ�ܷ��ظ������������ִ���ο�ϵͳ��Error Code
	*/	
	int            (*drv_ioctl_func)(int dev_id, unsigned long cmd, void *data);
	
	/*
	*read_sem:�ź��������첽֪ͨ
	*/	
	T_DrvSem       read_sem;

	/*
	*read_complete:ע��ص�������ʵ���첽֪ͨ����������Ϊread_sem
	*/		
	f_Drv_Complete read_complete;

	/*
	*read_sem:�ź��������첽֪ͨ
	*/		
	T_DrvSem       write_sem;
	
	/*
	*write_complete:ע��ص�������ʵ���첽֪ͨ����������Ϊwrite_sem
	*/	
	f_Drv_Complete write_complete;

	/*
	*�����豸������Ҫ�������������������ȵļ���
	*/		
	void		   *drv_data;	

	/*
	*������Ӧ���豸��
	*/		
	void		   *devcie;	
	
}
T_DEV_DRV_HANDLER;





typedef struct
{
	/*
	* dev_open_flg:�豸�Ƿ�򿪡�
	*/
	volatile bool dev_open_flg; 
	
	/*
	* dev_name:ָ���豸���ַ�����ָ��(�豸���ƾ�����࣬�ַ�̫��ֱ��Ӱ��ƥ���ʱ��)
	*/	
	char *dev_name;	
	
	/*
	* dev_id:��16λλ���豸�ţ���16λΪ���豸�ţ����Ψһ���豸��
	*/
	int dev_id; 

	/*
	*read_mutex_lock:�����ٽ���Դ�ı���
	*/
	T_DrvMutex     read_mutex_lock;
	
	/*
	*write_mutex_lock:�����ٽ���Դ�ı���
	*/	
	T_DrvMutex     write_mutex_lock;

	/*
	*ioctl_mutex_lock:�����ٽ���Դ�ı���
	*/	
	T_DrvMutex     ioctl_mutex_lock;
	
	/*
	* dev_data:����ÿ���豸��˽�����ݡ�
	*/	
	void *dev_data;

	/*
	* drv_handler:ָ���豸�������ṹ
	*/		
    T_DEV_DRV_HANDLER *drv_handler;

	/*
	* list:�豸����ڵ�
	*/		
	struct list_head list;
}T_DEV_INFO;



/** 
 * @brief  alloc  devcie  ID
 *
 * @author KeJianping
 * @date 2017-2-09
 * @param devcie[in] device .
 * @param major[in] 0, or you know the major number.
 * @param name[in] devcie's name.
 * @return  int
 * @retval  < 0 :  failed
 * @retval = 0 : successful
 */
int dev_alloc_id(T_DEV_INFO *devcie, int major, const char* name);


/** 
 * @brief    devcie  register
 *
 * @author KeJianping
 * @date 2017-2-09
 * @param dev_id[in] device  ID.
 * @param devcie[in] device.
 * @return  int
 * @retval  = 0 :  failed
 * @retval  = 1 : successful
 */
bool dev_drv_reg(int dev_id, T_DEV_INFO *devcie);

 
#endif // #ifndef __DEVICE_H__


