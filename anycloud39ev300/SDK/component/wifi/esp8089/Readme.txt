
����������
1.��ak_es8089.tar.gz ��ѹ�� /kernel/drivers/net/wireless/   �ο������е�Makefile����Kconfig  �޸����е�Makefile ����Kconfig��
2.����es8089mmcpatch�ֶ��޸�������ļ���
3.�������ÿ�������Եģ����ÿ���������ļ�mach-cloud39e_ak3916e_128pin_mnbd.c  ,�ɲο������е�.c�ļ���
/* akwifi platform data */
struct akwifi_platform_data akwifi_pdata = {
	.gpio_init = ak_gpio_set,
	.gpio_cs = {    
         .pin        = -1,
         .pulldown   = AK_PULLDOWN_DISABLE,
         .pullup     = -1,
         .value      = AK_GPIO_OUT_HIGH,   // cs ��DD��?��
         .dir        = AK_GPIO_DIR_OUTPUT,
		.int_pol    = -1,
    },
	.gpio_on = {
		.pin		= AK_GPIO_50,
		.pulldown	= -1,
		.pullup		= AK_PULLUP_DISABLE,
		.value		= AK_GPIO_OUT_HIGH,
		.dir		= AK_GPIO_DIR_OUTPUT,
		.int_pol	= -1,
	},
	.gpio_off = {
		.pin		= AK_GPIO_50,
		.pulldown	= -1,
		.pullup		= AK_PULLUP_DISABLE,
		.value		= AK_GPIO_OUT_LOW,
		.dir		= AK_GPIO_DIR_OUTPUT,
		.int_pol	= -1,
	},
	.power_on_delay   = 2000,
	.power_off_delay  = 200,
};



�ں����ã�
Linux/arm 3.4.35 Kernel Configuration 
                  ---��Device Drivers     
                         --�� MMC/SD/SDIO card support
                               <*>   SDIO WIFI support  
                               [*]   ANYKA MMC/SD/SDIO Card Interface support  