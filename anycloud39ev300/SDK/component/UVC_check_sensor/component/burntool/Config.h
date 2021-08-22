
#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "transc.h"
#include "anyka_types.h"


//////////////////////////////////////////////////////////////////////////
//Macro
#define MAX_BT_ADDR_LEN			12

#define MAX_ERASE_COUNT			5
#define MAX_FORMAT_COUNT		20

#define MAX_MAC_SEQU_ADDR_COUNT 50

#define MAX_PROJECT_NAME		64
#define MAX_PASSWD_LENGTH		20

#define MAX_DEVICE_NUM			16      

#define ERROR_CHIP_ID			0xFFFFFFFF

#define INVALID_GPIO			0xFE

#define NANDBOOT_CMDLINE_POS	72
#define CMDLINE_LEN				128
#define CMDLINE_FLAG			"CMDL"

//#define MODE_SFLASH	 0
//#define MODE_NEWBURN 1
//#define MODE_UPDATE  2
//#define MODE_DEBUG	 3

#define RAM_TYPE_MCP           0
#define RAM_TYPE_SDR           1
#define RAM_TYPE_DDR16         2
#define RAM_TYPE_DDR2_32       3
#define RAM_TYPE_DDR2_16       4
#define RAM_TYPE_DDR32         5
#define RAM_TYPE_DDR_16_MOBILE 6
#define RAM_TYPE_DDR_32_MOBILE 7

#define BURNED_NONE      1
#define BURNED_RESET     2
#define BURNED_POWER_OFF 3


//�˶��������10blue��producek�е�MODE_DEBUG��Ϊ5������,��Ҫ����Ϊ����spi��bin�ض�������.
#define MODE_DEBUG       4  
//ram�����ļ�·��
#define RAM_CFG_DDR    ".\\ramconfig\\ramconfig_ddr.txt"
#define RAM_CFG_DDR2   ".\\ramconfig\\ramconfig_ddr2.txt"
#define RAM_CFG_mDDR   ".\\ramconfig\\ramconfig_m_ddr.txt"
#define RAM_CFG_MCP    ".\\ramconfig\\ramconfig_mcp.txt"
#define RAM_CFG_SDR    ".\\ramconfig\\ramconfig_sdram.txt"



#define CONFIG_EXTRA_ENV_SETTINGS \
	"sf_hz=20000000\r\n" \
	"kernel_addr=0x23000\r\n" \
	"kernel_size=0x200000\r\n" \
	"fs_addr=0x230000\r\n" \
	"loadaddr=0x82008000\r\n" \
	"console=ttySAK0,115200n8\r\n" \
	"mtd_root=/dev/mtdblock1\r\n" \
	"init=/sbin/init\r\n" \
	"memsize=64M\r\n" \
	"rootfstype=squashfs\r\n" \
	"ethaddr=00:01:02:B4:36:56\r\n" \
	"setcmd=setenv bootargs console=${console} root=${mtd_root} rootfstype=${rootfstype} init=${init} mem=${memsize}\r\n" \
	"read_kernel=sf probe 0:0 ${sf_hz} 0; sf read ${loadaddr} ${kernel_addr} ${kernel_size}\r\n" \
	"boot_normal=readcfg; run read_kernel; bootm ${loadaddr}\r\n" /*go ${loadaddr}*/\
	"bootcmd=run setcmd boot_normal\r\n" \
	"vram=12M\r\n" \
	"erase_env=sf probe 0:0 ${sf_hz} 0; sf erase 0x20000 0x2000\r\n"/* erase env, use for test.*/ \
	


//////////////////////////////////////////////////////////////////////////
//Usual Enum & Struct

typedef enum
{
    MODE_NEWBURN = 1,           //new burn
	MODE_UPDATE,                //update mode
	MODE_UPDATE_SELF,           //update mode self
}E_BURN_MODE;

typedef enum
{
	E_DOWNLOAD_AK_ONLY,
	E_DOWNLOAD_IFX_ONLY,
	E_DOWNLOAD_BOTH,
}
E_DOWNLOAD_MODE;

typedef enum
{
	E_CONFIG_NAND,
	E_CONFIG_SFLASH,
	E_CONFIG_SD,
	E_CONFIG_DEBUG,
	E_CONFIG_JTAG,
	E_CONFIG_SPI_NAND,
	E_CONFIG_SPI_SD,
}
E_MODE_CONFIG_INDEX;


typedef enum
{
	DISK_TYPE_NORFLASH = 0,
	DISK_TYPE_NANDFLASH,	
	DISK_TYPE_U_DISK,
	DISK_TYPE_RESERVER,
}E_DISK_TYPE;

typedef enum
{
	MEMORY_TYPE_SDRAM = 0,		//SDRAM
	MEMORY_TYPE_SRAM ,			//SRAM
}E_MEMORY_TYPE;

typedef enum
{
	INIT_USB_GPIO = 0,			//��ʼ��USB��ʽ��GPIO
	INIT_USB_REGISTER,			//��ʼ��USB��ʽ���Ĵ���
}E_INIT_USB_TYPE;
/*
typedef enum
{
    MODE_NEWBURN = 1,
    MODE_UPDATE,
}E_BURN_MODE;
*/
typedef struct
{
	TCHAR pc_path[MAX_PATH];
	UINT ld_addr;
	TCHAR file_name[MAX_PATH];
	BOOL bCompare;
	BOOL  bBackup;
}T_DOWNLOAD_NAND;

typedef struct
{
	BOOL bCompare;
	TCHAR pc_path[MAX_PATH];
	TCHAR udisk_path[MAX_PATH];
}T_DOWNLOAD_UDISK;

typedef struct
{
	BOOL bCompare;
	TCHAR pc_path[MAX_PATH];
	TCHAR disk_name[MAX_PATH];
}T_DOWNLOAD_MTD;

typedef struct
{
	BOOL bCompare;
	TCHAR pc_path[MAX_PATH];
}T_DOWNLOAD_SPIFLASH_IMG;

#define  maccurrentlow       _T("mac_current_low")
#define  sequencecurrentlow  _T("serial_current_low")
#define  otpserialcurrent  _T("otp_serial_current")

#pragma pack(1)
//NandFlash��������
typedef T_NAND_PHY_INFO T_NAND_PHY_INFO_TRANSC;
typedef T_SFLASH_PHY_INFO T_SFLASH_PHY_INFO_TRANSC;
//Ƭѡ
typedef struct
{
	BYTE	b_chip_select_loop;
	BYTE	b_chip_select[4];
}T_CHIP_SELECT_TRANSC;
/*
//��ʽ��
typedef struct 
{
	char Disk_Name;				//�̷���
    BOOL bOpenZone;				//
    UINT ProtectType;			//	
    UINT ZoneType;				//
	UINT Size;
}T_FORMAT_DATA;
*/
typedef struct 
{
    char Disk_Name;				//�̷���
	char volume_lable[12];
}T_VOLUME_LABLE;

typedef struct
{
	TCHAR Disk_Name[DOWNLOAD_BIN_FILENAME_SIZE + 2];				//�̷���
	UINT Size;	
    char type;				//data , bin, fs
    char r_w_flag;			//	r_w or onlyread
    char hidden_fag;				//
	char rvs1;	
}T_ALL_PARTITION_INFO;

//д����
typedef struct 
{
	E_DISK_TYPE Disk_Type;		//
	int Start_Address;			//
	int End_Address;			//	
	int Backup_Start_Address;	//	
	int Backup_End_Address;		//	
	int Data_Length;			//
	int Backup_Map_Address;		//
	int map_index;
}T_DATA_WRITE_TRANSC;

//д�ļ�
typedef struct 
{
	TCHAR File_Path[MAX_PATH+1];
	int File_length;
	int File_mode;
	int File_time;
}T_FILE_WRITE_TRANSC;

#pragma pack()

typedef struct 
{
	int main_version;
	int sub_version1;
	int sub_version2;
	int version_reserve;
}T_VERSION_DATA;

typedef struct
{
	TCHAR pm_password[MAX_PASSWD_LENGTH+1];	
	TCHAR rd_password[MAX_PASSWD_LENGTH+1];	
}T_SEC_CTRL;


typedef struct
{
	BYTE type;		//RAM����
	UINT size;					//RAM��С
	UINT banks;					//RAM Banks
	UINT row;					//RAM row
	UINT column;				//RAM Column
	UINT control_addr;			//RAM Control register address
	UINT control_value;			//RAM Control value
}T_RAM_PARAM;

typedef struct
{
	int address;				//�Ĵ�����ַ
	int value;					//�Ĵ���ֵ
}T_SET_REGISTER_DATA;

typedef struct 
{
	BOOL bCmdLine;
	UINT cmdAddr;
	char CmdData[CMDLINE_LEN+1];
}T_CMD_LINE;

class CConfig
{
public:
	CConfig();
	~CConfig();

public:
	BOOL No_need_add_flag;

	void DefaultConfig();
	BOOL ReadConfig(LPCTSTR file_path);
	BOOL WriteConfig(LPCTSTR file_path);
	void Cansel_Config_init(void);

	BOOL StorePassword();
	BOOL GetPassword();

	UINT hex2int(const char *str);
	LONG64 hex2long64(TCHAR *str);
	BOOL Otp_serial_add(TCHAR *surbuf, TCHAR *dstbuf);
//	TCHAR * ConverPathToAbsolute(TCHAR *path);

protected:
	BOOL get_download_udisk(UINT index, CString subRight);
	BOOL get_download_nand(UINT index, CString subRight);
	BOOL get_download_mtd(UINT index, CString subRight);

	BOOL get_format_data(UINT index, CString subRight);
	BOOL get_spi_format_data(UINT index, CString subRight); //linux spi
	BOOL get_nand_data(UINT index, CString subRight);
	BOOL get_spinand_data(UINT index, CString subRight);

	BOOL get_sflash_data(UINT index, CString subRight);
	BOOL get_burned_gpio(CString subRight);
    BOOL get_burned_rtc(CString subRight);
	BOOL get_download_spi_img(UINT index, CString subRight);
public:
	BOOL ConfigNandbootCmdline(BOOL bClear);
    BOOL write_config_addr(LPCTSTR file_path, TCHAR *pBufname, TCHAR *pBuffalg, TCHAR *pBufaddr, UINT channelID);
	BOOL read_config_addr(LPCTSTR file_path, TCHAR *pBufAddr, TCHAR *pGAddr, TCHAR *pCurAddr);
	BOOL read_currentdevicenum_addr(LPCTSTR file_path, TCHAR *pBufname, TCHAR *pBuffalg, TCHAR *pBufaddr, UINT channelID);
	VOID lower_to_upper(TCHAR *surbuf, TCHAR *dstbuf);
	VOID ChangeBurnMode(T_MODE_CONTROL *pModeCtrl);
	INT get_float_num(const char *str);
	INT float_num(const char *str);

	BOOL Read_blue_cur_addr(LPCTSTR file_path, char *blue_addr_empty_flag);
	BOOL get_blue_addr(UINT index, CString subRight);
	BOOL Write_blue_addr_log(LPCTSTR file_path, TCHAR *bule_addr_buf);
	T_BOOL Blue_Address_add(TCHAR *surbuf, TCHAR *dstbuf);

	//bios version control
	T_VERSION_DATA version_ctrl;

	//project name
	TCHAR			project_name[MAX_PROJECT_NAME+1];

	//UI Control
	BOOL			bUI_ctrl;

	//Sec ctrl	
	T_SEC_CTRL		passwd_ctrl;

	//device number
	UINT			device_num;

	// com mode
	UINT			com_mode;

	//burn mode
	UINT			init_burn_mode;
	UINT			burn_mode;
	BOOL			burn_mode_spi_sd_flag;
	BOOL			init_bUpdate;
	BOOL			bUpdate;
	BOOL            init_bUDiskUpdate;
	BOOL            bUDiskUpdate;
	BOOL            bAuto_Burnflag;
	BOOL            bUDisk_Massboot;
	BOOL            bchannels_no_fixed;
	BOOL            erase_partition_size_flag;
	BOOL            no_need_erase_flag;
	
	
	//com concerned parameter
	BOOL			bOpenCOM;
	UINT			com_base;
	UINT			com_count;
	UINT			com_baud_rate;
	
	// chip concerned parameter
	E_CHIP_TYPE		init_chip_type;
	E_CHIP_TYPE		chip_type;
	BOOL			bUboot;
//	BOOL			bAutoDownload;
//	UINT			chip_clock;
	BYTE			power_off_gpio;
	BOOL			init_bUsb2;
	BOOL			bUsb2;
	BOOL            bPLL_Frep_change;
	BOOL            init_bPLL_Frep_change;
	BOOL			init_bUpdateself;
	BOOL			bUpdateself;
	BOOL			bTest_RAM;
	UINT			chip_snowbirdsE;
	BYTE            *pCheckExport;

	//online make image
	BOOL			bonline_make_image;
	
	//MAC ADDR
	BOOL			fore_write_mac_addr;  //�Ƿ�ǿ����¼�ı�־
	TCHAR			mac_start_high[MAX_MAC_SEQU_ADDR_COUNT+1];
	TCHAR			mac_start_low[MAX_MAC_SEQU_ADDR_COUNT+1];
	TCHAR			mac_end_high[MAX_MAC_SEQU_ADDR_COUNT+1];
	TCHAR			mac_end_low[MAX_MAC_SEQU_ADDR_COUNT+1];
	TCHAR			mac_current_high[MAX_MAC_SEQU_ADDR_COUNT+1];
	TCHAR			mac_current_low[MAX_MAC_SEQU_ADDR_COUNT+1];
	BOOL			macaddr_flag;   //�Ƿ���¼��־

	//SEQUENCE ADDR
	BOOL			fore_write_serial_addr; //�Ƿ�ǿ����¼�ı�־
//	TCHAR			sequence_start[MAX_MAC_SEQU_ADDR_COUNT+1];
	TCHAR	        sequence_current[MAX_MAC_SEQU_ADDR_COUNT+1];

//	TCHAR			g_sequence_current_high[32][MAX_MAC_SEQU_ADDR_COUNT+1];
//	TCHAR			g_sequence_current_low[32][MAX_MAC_SEQU_ADDR_COUNT+1];
//	TCHAR			g_mac_current_high[32][MAX_MAC_SEQU_ADDR_COUNT+1];
	TCHAR			g_mac_current_low[MAX_DEVICE_NUM][MAX_MAC_SEQU_ADDR_COUNT+1];
	TCHAR			g_mac_show_current_low[MAX_DEVICE_NUM][MAX_MAC_SEQU_ADDR_COUNT+1];
	TCHAR			g_sequence_show_current[MAX_DEVICE_NUM][MAX_MAC_SEQU_ADDR_COUNT+1];

	BOOL			sequenceaddr_flag; //�Ƿ���¼��־
	BOOL			ShowImageMaker_flag; //�Ƿ���ʾ��������˵���־

	BOOL			BT_addr_flag;   //�Ƿ���¼��־
	BOOL			fore_write_BT_addr; //�Ƿ�ǿ����¼�ı�־
	TCHAR			blue_start_addr[MAX_BT_ADDR_LEN+1];
	TCHAR			blue_end_addr[MAX_BT_ADDR_LEN+1];
	TCHAR			m_blue_show_addr[MAX_DEVICE_NUM][MAX_BT_ADDR_LEN+1];
	
	UINT            blue_address_idex;
	TCHAR			blue_cur_addr[MAX_BT_ADDR_LEN+1];
	BOOL            blue_burn_result;


	BYTE			gpio_pin;
    BYTE			gpio_pin_select;

	T_BURNED_PARAM  init_burned_param;
    T_BURNED_PARAM  burned_param;

	//burn planform
	UINT	planform_tpye;
	TCHAR	init_planform_name[MAX_PROJECT_NAME+1];
	TCHAR	planform_name[MAX_PROJECT_NAME+1];

	//nand concerned parameter
	T_CHIP_SELECT_DATA chip_select_ctrl;

	//freq
	UINT	m_freq;
	
	//memory concerned parameter
	T_RAM_PARAM     init_ram_param;
	T_RAM_PARAM		ram_param;

	//usb concerned
	E_INIT_USB_TYPE	init_usb;						//��ʼ��USB
	UINT			init_usb_gpio_number;			//��ʼ��USB��ʹ�õ�GPIO��
	UINT			init_usb_register;				//��ʼ��USB��ʹ�õļĴ���
	UINT			init_usb_register_bit;			//��ʼ��USB��ʹ�õļĴ�����BITλ

	//path concerned
	TCHAR			path_module[MAX_PATH+1];
	TCHAR			path_producer[MAX_PATH+1];
	TCHAR			path_nandboot[MAX_PATH+1];
	TCHAR			path_bios[MAX_PATH+1];


	TCHAR			udisk_info[MAX_PATH];           //����11ƽ̨ʹ�þ����ļ���ͬ����Ҫ��¼ÿ���ļ����̷���Ϣ
	UINT            udiskfile_drivernum;

	//producer concerned
	UINT			producer_run_addr;

	//register concerned
	T_SET_REGISTER_DATA	*set_registers;				//�Ĵ��������б�

	//erase concerned ============= ���ܲ�����Ҫ�ˣ��ݱ���
//	int				erase_count;
//	T_ERASE_DATA	*erase_data;

	//module burn concerned
	UINT			DownloadMode;
	BOOL			bDownloadFLS;
	BOOL			bDownloadEEP;
	UINT			baudrate_module_burn;
	UINT			gpio_dtr;
	UINT			gpio_module_igt;
	UINT			gpio_module_reset;
	TCHAR			path_fls[MAX_PATH+1];
	TCHAR			path_eep[MAX_PATH+1];

	//fat system start address
	UINT			fs_res_blocks;						//�ļ�ϵͳ������
	UINT			nonfs_res_size;						//���ļ�ϵͳ������

	T_CMD_LINE      cmdLine;

    UINT            event_wait_time;
	UINT            PID;
	UINT            VID;
	//format concerned
	UINT			format_count;
	T_PARTION_INFO	*format_data;
	T_SPIFLASH_PARTION_INFO *spi_format_data; //�˱�����ר��Ϊlinuxƽ̨��spi������¼ʱʹ�õ�

	UINT			partition_count;
	T_ALL_PARTITION_INFO *partition_data;

	T_VOLUME_LABLE	*pVolumeLable;
    
	//download concern
	//download to nandflash
	UINT				download_nand_count;
	T_DOWNLOAD_NAND		*download_nand_data;
	//download to udisk
	UINT				download_udisk_count;
	T_DOWNLOAD_UDISK	*download_udisk_data;			
	//download to udisk
	UINT				download_mtd_count;
	T_DOWNLOAD_MTD		*download_mtd_data;

	UINT				download_spi_img_count;
	T_DOWNLOAD_SPIFLASH_IMG	*download_spi_img_data;
	
	UINT			file_download_total_len;

	//nand list	
	UINT				nandflash_parameter_count;		//NANDFLASH��������
	T_NAND_PHY_INFO_TRANSC *nandflash_parameter;			//NANDFLASH�����б�
	
	//spinand list	
	UINT				spi_nandflash_parameter_count;		//spi NANDFLASH��������
	T_NAND_PHY_INFO_TRANSC *spi_nandflash_parameter;			//spi NANDFLASH�����б�		


	UINT				spiflash_parameter_count;		//NANDFLASH��������
	T_SFLASH_PHY_INFO_TRANSC *spiflash_parameter;			//NANDFLASH�����б�		

    HANDLE          m_hMutex;
	HANDLE          m_hMutGetAddr;
	BOOL            first_flag;
	UINT run_time;
    UINT burn_totalnum;
};

#endif
