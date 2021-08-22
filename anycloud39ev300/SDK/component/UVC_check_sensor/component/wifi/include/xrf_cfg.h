#ifndef  APP_CFG_MODULE_PRESENT
#define  APP_CFG_MODULE_PRESENT


/*
*********************************************************************************************************
*                                            TASK PRIORITIES
* UCOSÿ���߳����ȼ�������ͬ����������ͳһ����
*********************************************************************************************************
*/
#define WORK_QUEUE_MAX_SIZE 4
enum TASK_PRIO{
TASK_UNUSED_PRIO = 10,
TASK_MOAL_WROK_PRIO,
	
TASK_TIMER_TASKLET_PRIO,
REASSOCIATION_THREAD_PRIO, 

TASK_TCP_RECV_PRIO,
TASK_TCP_SEND_PRIO,
TASK_TCP_ACCEPT_PRIO,
TASK_SDIO_THREAD_PRIO = 250,
};


/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*��λWORD
*********************************************************************************************************
*/
#define TASK_TCPIP_THREAD_STACK_SIZE		4096
#define TIMER_TASKLET_STACK_SIZE			4096
#define MOAL_WROK_STACK_SIZE				4096
#define REASSOCIATION_THREAD_STACK_SIZE     4096
#define TASK_SDIO_STACK_SIZE				4096
#define TASK_TCP_RECV_STACK_SIZE			4096
#define TASK_TCP_SEND_STACK_SIZE			4096
#define TASK_ACCEPT_STACK_SIZE				4096
//������ֵ������Ķ�


/*
*********************************************************************************************************
*                                      kernel ���泣����Դֵ����
*�������monitor���ȷ�����ʵ���ֵ
*********************************************************************************************************
*/

#define OS_TICK_RATE_HZ		200

#endif
