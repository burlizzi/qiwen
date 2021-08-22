 // Lang.cpp: implementation of the CLang class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "burntool.h"
#include "Lang.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define UNICODE_TXT_HEAD 0xFEFF//

extern CBurnToolApp theApp;//

//ע��
//���������
//��дʱҪע��λ��,
//���Ȳ�Ҫ����50

TCHAR *g_strCN[] = 
{
_T("��¼����"),//
_T("����ģʽ"),//

_T("�ļ�(&F)"),//
_T("�����ļ�(&I)"),//
_T("�����ļ�(&O)"),//
_T("����(&S)"),//
_T("��������"),//
_T("�޸�����"),//
_T("��ͼ(&V)"),//
_T("����ѡ��"),//
_T("COM����"),
_T("����(&T)"),
_T("Spi Nor Flash���߾�������(&I)"),
_T("Spi Nor Flash����ض�(&S)"),
_T("ϵͳBIN�ļ��ض�(&B)"),
_T("�����ļ�(&C)"),
_T("����(&H)"),
_T("����BurnTool (&A)"),

_T("��ʼ"),
_T("����"),
_T("����"),
_T("����"),
_T("�͸�"),

_T("״̬"),
_T("���"),
_T("����"),
_T("״̬"),
_T("ʱ��"),
_T("���к�"),
_T("MAC��ַ"),
_T("�豸"),

_T("��ǰ����:"),
_T("��������:"),
_T("��¼����:"),
_T("�ۼ�ͨ��:"),
_T("�ۼ�ʧ��:"),
_T("���"),
_T("��¼״̬"),
_T("USB�豸:"),
_T("U��:"),
_T("�Զ���¼"),
_T("U����¼"),
_T("������������(ֻ���豸0,��������һ��)"),
_T("����ͨ��"),
_T("U��massboot"),

_T("������"),
_T("�з���"),
_T("�������û���������:"),
_T("�û���:"),
_T("����:"),
_T("�޸�����"),
_T("�û�:"),
_T("������:"),
_T("������:"),
_T("ȷ������:"),
_T("�������!"),
_T("ȷ�����벻һ��"),
_T("�޸�����ɹ�"),


_T("׼������"),
_T("��������׼������"),
_T("��ʼ��¼"),
_T("��ʼ����RAM����"),
_T("����RAM����ʧ��"),
_T("��ʼ���üĴ���"),
_T("���üĴ���ʧ��"),
_T("��ȡ�Ĵ���ʧ��"),
_T("���üĴ����ɹ�"),
_T("��ʼ��ʼ��USB"),
_T("��ʼ��USBʧ��"),
_T("��¼���"),
_T("��ʼ����RAM"),
_T("����RAMʧ��"),
_T("��ʼ����producer"),
_T("����producerʧ��"),
_T("����producer��usb����ʧ��"),
_T("����producer�ȴ���ʱ"),
_T("����producer�ɹ�"),
_T("������ϢΪ��"),
_T("��ȡý��������Ϣʧ��"),
_T("��ȡ���п���Ϣʧ��"),
_T("��ʼ��������"),
_T("��������ʧ��"),
_T("����ý��ʧ��"),
_T("��ʼ����"),
_T("����ʧ��"),
_T("��ʼ��ȡNandflashоƬid"),
_T("��ȡNandflashоƬidʧ��"),
_T("��ʼ����NandflashоƬ����"),
_T("����NandflashоƬ����ʧ��"),
_T("��¼�����б�û�д˲���"),
_T("��һ����չ��������"),

_T("��ʼ��ȡspiflashоƬid"),//
_T("��ȡspiflashоƬidʧ��"),//
_T("��ʼ����spiflashоƬ����"),//
_T("����spiflashоƬ����ʧ��"),//

_T("��ʼ����"),
_T("����ʧ��"),
_T("��ʼ��ʽ��"),
_T("��ʽ��ʧ��"),
_T("��ʼ�����ļ�"),
_T("�����ļ�ʧ��"),
_T("��ʼ��ȡ�����ļ�"),
_T("��ȡ�����ļ�ʧ��"),

_T("��Ŀ���ƣ�"),
_T("����ͨ������"),
_T("ƽ̨���ͣ�"),
_T("�򿪴���"),
_T("��������"),
_T("��һ�����ںţ�"),
_T("�����ʣ�"),
_T("����������"),
_T("��ַ��"),
_T("���ݣ�"),
_T("��¼MAC��ַ"),
_T("ǿ��д��MAC��ַ"),
_T("��ǰ��MAC��ַ��λ"),
_T("��ʼ��ַ(0x):"),
_T("������ַ(0x):"),
_T("��¼���к�"),
_T("ǿ��д�����к�"),
_T("��ʼǰ׺(0x):"),//
_T("����ǰ׺(0x):"),//
_T("���к�:"),//

_T("��¼������ַ"),//
_T("ǿ����¼������ַ"),//
_T("������ʼ��ַ:"),//
_T("����������ַ:"),//

_T("���"),
_T("Producer: "),
_T("boot: "),
_T("boot(37L): "),
_T("Bios: "),

_T("��¼�ļ����ļ�ϵͳ����: "),
_T("��¼BIN�ļ�:"),
_T("��¼�ļ�ϵͳ��������:"),
_T("��¼Spi Nor Flash����:"),

_T("���"),
_T("�Ƿ�Ƚ�"),
_T("��"),
_T("��"),
_T("�ļ���PC·��"),
_T("�����ļ�"),
_T("����Ŀ¼"),
_T("�ļ�����·��"),
_T("��ʼ��ַ"),
_T("������ַ"),
_T("������ʼλ��"),
_T("���ݽ���λ��"),
_T("�̷�"),

_T("���ӵ�ַ"),
_T("�ļ���"),
_T("�Ƿ񱸷�"),
_T("��"),
_T("��"),
_T("bin����С(MB)"),

_T("оƬ���ͣ�"),
_T("USB2.0"),
_T("��Ƶ(pll)"),
_T("��ȡ�����ļ�"),
_T("����ģʽ"),
//_T("֧���Զ�����"),
//_T("ϵͳƵ��(M):"),
_T("����洢��"),
_T("����GPIO"),
_T("ģʽ"),
_T("��¼"),
_T("����..."),
_T("����..."),
_T("  ��:"),
_T("  ��:"),
_T("֧��USB Boot��ʽ"),
_T("Power_off GPIO:"),
_T("BIOS���е�ַ:"),//
_T("�ļ�ϵͳ������:"),//
_T("RAM����:"),
_T("��С(M):"),
_T("Bank��"),
_T("Row Length:"),//
_T("Col Address:"),//
_T("Ƭѡ:"),
_T("�Զ���"),
_T("Ƭѡ0"),
_T("Ƭѡ1"),
_T("Ƭѡ2"),
_T("Ƭѡ3"),
_T("��ѭ����"),
_T("��ʼ��USB"),
_T("ʹ��GPIO:"),
_T("д�Ĵ���"),
_T("�Ĵ���:"),
_T("λ:"),

_T("��ʽ���б�"),
_T("���"),
_T("������ʶ"),
_T("��������"),
_T("ֻ����־"),
_T("���ر�־"),
_T("������С(K)"),
_T("�����"),
_T("�ļ�ϵͳ������(block):"),
_T("���ļ�ϵͳ������(M)��"),

_T("Nandflash �б�:"),
_T("SpiNandflash �б�:"),
_T("SPI �б�:"),

_T("��ʼ����Infineonģ�����"),//
_T("����Infineonģ�����ʧ��"),//
_T("�豸ͬ���ɹ�"),//
_T("����Ŀ���豸ʧ��"),//
_T("��ʼ����fls�ļ�"),
_T("��ʼ����eep�ļ�"),//
_T("����fls�ļ�ʧ��"),//
_T("����eep�ļ�ʧ��"),//

_T("����ģʽ"),
_T("ֻ��AK����"),
_T("ֻ��Infineon����"),
_T("���߶���"),
_T("�����������"),
_T("�����ʣ�"),
_T("����fls�ļ���"),
_T("����eep�ļ���"),
_T("���"),
_T("GPIO����"),
_T("GPIO����ʧ��"),//
_T("���ò�����producerʧ��"),//

_T("��ʼ����bin"),
_T("����binʧ��"),//
_T("��ʼ����img"),
_T("����imgʧ��"),
_T("�����̷��뾵����̷���һ��"),
_T("��ʼ����boot"),//
_T("����bootʧ��"),//


_T("��ʼдmac��ַ�밲ȫ��"),//
_T("дmac��ַ�밲ȫ��ʧ��"),
_T("��ʼд���кŵ�ַ�밲ȫ��"),//
_T("д���кŵ�ַ�밲ȫ��ʧ��"),//

_T("��ʼ���ر�ƵС����"),//
_T("���ر�ƵС����ʧ��"),//
_T("���ر�ƵС����ʱ"),//
_T("���ر�ƵС�����usb����ʧ��"),//

_T("������¼ģʽ"),
_T("������¼ģʽʧ��"),
_T("���ò���ģʽ"),
_T("���ò���ģʽʧ��"),
_T("����Nand����"),
_T("����Nand����ʧ��"),
_T("���ð�ȫ��"),
_T("���ð�ȫ��ʧ��"),
_T("bin�ض�ʧ��"),
_T("��ȡ������Ϣʧ��"),

_T("����ȫ���е�mac��ַ"),
_T("����ȫ���е�mac��ַ����Ч��"),//
_T("����ȫ���е����кŵ�ַ"),
_T("����ȫ���е����кŵ�ַ����Ч��"),//


_T("��¼mac��ַ�Ƚ�"),
_T("��¼mac��ַ�������ֵ"),
_T("��¼���кŵ�ַ�Ƚ�"),
_T("��¼���кŵ�ַ�������ֵ"),	

_T("���ñ�����"),
_T("���ñ�����ʧ��"),
_T("����������"),
_T("����������ʧ��"),
_T("��ֹ��¼�߳�"),
_T("��ֹ��¼�߳�ʧ��"),

_T("������16���Ƶ�����"),
_T("MAC��ַ��ʽ������00:00:00"),
_T("MAC��ʼ��ַ��λ���ַ���������8!!"),
_T("MAC��ʼ��ַ��λ���ַ���������8!!"),
_T("MAC������ַ��λ���ַ���������8!!"),
_T("MAC��ʼ��ַ��λ���ַ���������8!!"),
_T("�ַ������в���16���Ƶ�!!!!"),
_T("MAC��ʼ��λ�ͽ�����ַ�ĸ�λ����ͬ!!"),//
_T("MAC��ʼ��λ���ڽ�����ַ�ĵ�λ!!"),//
_T("���к���ʼ��λ���ַ���������32!!"),//
_T("���к���ʼ��λ���ַ���������6!!"),//
_T("���кŽ�����λ���ַ���������10!!"),//
_T("���кŽ�����λ���ַ���������6!!"),//
_T("���к���ʼ�ĸ�λ������ĸ�λ����ͬ!!"),//
_T("���к���ʼ�ĵ�λ���ڽ����ĵ�λ!!"),//
_T("��ǰ��mac��ַ�Ѹ�λ�ɹ�����ǰֵ���ڿ�ʼ��ֵ!"),//
_T("��ǰ�����к��Ѹ�λ�ɹ�����ǰֵ���ڿ�ʼ��ֵ!"),//

_T("����ͨ���ĵ�ַ"),
_T("ͨ���ĵ�ַ����"),

_T("��������"),
_T("Դ�ļ���:"),
_T("Ŀ���ļ���:"),
_T("���"),
_T("���"),
_T("�����ļ���:"),
_T("������(MByte):"),
_T("ҳ��С(Byte):"),
_T("������С(Byte):"),
_T("������Ϣ:"),
_T("׼����������.."),
_T("���ڽ��о���������..."),
_T("����"),
_T("����"),
_T("���������ɹ�!!"),
_T("��������ʧ��!!"),
_T("���ڻ�ȡ�ļ�����Ϣ!!"),
_T("��ȡ�ļ�����Ϣʧ��!!"),
_T("��������С����!!"),
_T("������Ϣ����!!"),

_T("Դ�ļ����ǿյ�!"),
_T("Ŀ���ļ����ǿյ�!"),
_T("�����ļ����ǿյ�!"),
_T("��������С�ǿյ�!"),
_T("������С�ǿյ�!"),
_T("ҳ��С�ǿյ�!"),

_T("����nand gpioceʧ��!"),//
_T("����channel��ַʧ��!"),//
_T("���ô�������ʧ��!"),//
_T("���ش�������ʧ��!"),//

_T("������������!"),
_T("���������ɹ�!"),
_T("��������ʧ��!"),
_T("��ɾ������������ѡ��!"),//

_T("�������ڽ�����,�Ƿ�Ҫ�رգ�"),//

_T("����У���ַ���"),
_T("�ļ�����ɹ�!"),
_T("�ļ�����ʧ��!"),//
_T("���Ե�..."),
_T("�ļ������ɹ�!"),//
_T("�ļ�����ʧ��!"),//

_T("�͸��ƻ�֮ǰ����Ϣ,ȷ��Ҫ���豸���и�ʽ����?"),//
_T("SPI ����"),
_T("��ѡ��SPIFLASH�ͺţ�"),
_T("�ͺţ�"),
_T("û�п�ѡ���spi�ͺ�"),
_T("�򿪰����ĵ�ʧ��"),

_T("�豸�͸�"),
_T("ȷ��"),
_T("ȡ��"),
_T("�����ľ�������ܰ������µ�9���ַ�"),//

_T("��mac��ַ���鲥��ַ�����޸�"),//
_T("��mac��ַ����ȫ��0"),//
_T("��ȡhighIDʧ��, produce�汾�Ƿ�̫����"),//
_T("��������ǰͳ�����еĿ��п�"),//
_T("����ͨ���Ľ���(nand/sd)������һ��"),//
//_T("��ͨ���Ľ���������С��ͬ,�ſ��Զ�̨��¼,��ȷ��"),
_T("���ÿ��ͨ���Ľ���������С�Ƿ�һ��"),
_T("bin�ض����"),
_T("bin�ض�, �Ƚ���massboot, �ٰ���ʼ"),
_T("spiflash�ض����"),
_T("spiflash�ض�ʧ��"),
_T("spiflash�ض�, �Ƚ���massboot, �ٰ���ʼ"),
_T("��ʼ�ض�����spiflash����"),

_T("��OTP�����к�"),
_T("��OTP�����к�ʧ��"),
_T("дOTP�����к�"),
_T("дOTP�����к�ʧ��"),
_T("OTP�����к��Ѵ���"),

_T("������û�д��ļ���"),
_T("�¾���ʱ��ȡ������С����"),
_T("��ʼд������������"),
_T("д�����������ݳ���"),

_T("��������BIN�ļ�����һ��"),
_T("����SPI�����ļ�ʧ��"),
_T("SPI�����ļ�ֻ������һ��"),
_T("SPI�����ļ�����"),
_T("SPI�����ļ��������"),

_T("��¼ǰ�Ȳ���(����������Ĵ�С���в�,�˹�������Ҫ���԰�64K�����Խ�ʡʱ��)"),
_T("��¼���̲���Ҫ������(�˹�����Ϊ��ȫ�µ�spi��¼ʱ����Ҫ�����Խ�ʡʱ��)"),

_T("���ֳ������ֵ8"),
_T("��ʼ������"),
_T("������ʧ��"),
_T("����ļ�ϵͳ����ʧ��"),

_T("��ʼ��ȡ������ַ"),
_T("������ַ���ڽ�����ַ"),
_T("������ַ�����д�"),
_T("������ַ�д�"),
_T("д������ַ����"),
_T("��ʼд������ַ"),

_T("���Ŀ��������ܿ���"),

NULL
};

//ע��
//�����Ӣ��
//��дʱҪע��λ��,
//���Ȳ�Ҫ����50

TCHAR *g_strEN[] = 
{
_T("Burn Tool"),
_T("Update Mode"),

_T("File(&F)"),
_T("Import File(&I)"),
_T("Export File(&O)"),
_T("Setting(&S)"),
_T("Config Setting"),
_T("Modify Password"),
_T("View(&V)"),
_T("Language"),
_T("COM Window"),
_T("Tool(&T)"),
_T("Creat Spi Nor Flash image off line(&I)"),
_T("Upload Spi Nor Flash image(&S)"),
_T("Upload Bin(&B)"),
_T("ConfigFiles(&C)"),
_T("Help(&H)"),
_T("About BurnTool (&A)"),

_T("Start"),//
_T("Setting"),//
_T("Import"),//
_T("Export"),
_T("Format"),//

_T("Status"),
_T("Number"),
_T("Progress"),//
_T("Status"),
_T("Timer"),//
_T("serial NO"),//
_T("mac addr"),//
_T("Device"),//

_T("Current Link:"),
_T("Available Link:"),//
_T("Burn Count:"),///
_T("Total Pass:"),//
_T("Total Fail:"),//
_T("Clear"),
_T("Burn State"),//
_T("USBNum:"),//
_T("Udisk:"),
_T("Auto Burn"),//
_T("UDiskBurn"),//
_T("online make_image(only device0 && make once)"),//
_T("No Fixed Channels"),//
_T("UDiskBurn_Massboot"),//

_T("Producer"),
_T("Researcher"),
_T("Input the User Name and Password:"),
_T("User Name:"),//
_T("Password:"),
_T("Change Password"),//
_T("User:"),
_T("Old Password:"),
_T("New Password:"),
_T("Confirm Password:"),
_T("Error Password!"),
_T("Confirm Password Error!"),//
_T("Modify Password Success"),//


_T("Ready"),
_T("Udisk update Ready"),
_T("start burn"),//
_T("start to set RAM parameter"),//
_T("Fail to set RAM parameter"),
_T("Begin to set registers"),
_T("Fail to set registers"),
_T("Fail to get registers"),
_T("Set registers Sucessfully"),
_T("Begin to init USB"),
_T("Fail to init USB"),
_T("Burn task complete"),//
_T("start test RAM"),
_T("test RAM fail"),
_T("Begin to download producer"),//
_T("Fail to download producer"),
_T("Fail to connet usb after download producer"),//
_T("loading producer time out"),
_T("Download producer Succcessfully"),
_T("partition info is null"),
_T("get medium data info fail"),
_T("get free block info fail"),
_T("Start to mount disk"),
_T("Fail to mount disk"),
_T("malloc medium fail"),
_T("Start to Test"),
_T("Fail to test"),//
_T("Begin to get nandflash chip id"),
_T("Fail to get nandflash chip id"),//
_T("Begin to set nandflash chip parameter"),
_T("Fail to set nandflash chip parameter"),//
_T("the spi parameter is not in spiflash list"),
_T("set the ex parameter fail"),


_T("Begin to get spiflash chip id"),
_T("Fail to get spiflash chip id"),
_T("Begin to set spiflash chip parameter"),
_T("Fail to set spiflash chip parameter"),

_T("Begin to erase"),
_T("Fail to erase"),
_T("Begin to format"),
_T("Fail to format"),//
_T("Begin to download file"),
_T("Fail to download file"),//
_T("Start to get activation file"),//
_T("Fail to get activation file"),

_T("Project Name: "),
_T("Channel Number: "),
_T("Platform Tpye: "),
_T("Open Com"),
_T("COM Number: "),
_T("COM Base: "),
_T("Baudrate: "),
_T("Config Command Line"),
_T("Addr:"),
_T("Data:"),//

_T("Burn MAC Addr"),
_T("Force Write MAC Addr"),//
_T("Current MAC Reset"),
_T("Start Addr(0x):"),
_T("End Addr(0x):"),
_T("Burn Serial"),
_T("Force Write Serial"),
_T("Startprefix(0x):"),
_T("Endprefix(0x):"),//
_T("Serial:"),

_T("burn blue addr"),//
_T("force write blue addr"),//
_T("blue start:"),//
_T("blue end:"),//

_T("Browse"),
_T("Producer: "),
_T("Boot: "),
_T("Boot(37L): "),
_T("Bios:"),

_T("Burn Fs File"),
_T("Burn Bin"),//
_T("Burn Fs Img"),
_T("Burn Flash Img"),

_T("N.O."),//
_T("bCompare"),
_T("YES"),//
_T("NO"),//
_T("PC Path"),//
_T("Download File"),//
_T("Download Directory"),
_T("UDisk Path"),
_T("Start Address"),
_T("End Address"),
_T("Backup Start Address"),
_T("Backup End Address"),//
_T("Disk Name"),

_T("Link Address"),
_T("File Name"),
_T("bBackup"),//
_T("YES"),
_T("NO"),
_T("bin area size(KB)"),//

_T("Chip Type: "),
_T("USB2.0"),
_T("PLLFrep_change"),
_T("Get Activation File"),
_T("Update Mode"),
//_T("Auto Download"),
//_T("Chip Clock(M):"),
_T("RAM"),
_T("SET GPIO"),
_T("MODE"),
_T("BURNED"),
_T("Import..."),
_T("Export..."),
_T("Row:"),
_T("Column:"),
_T("USB Boot"),
_T("Power off GPIO"),//
_T("BIOS Start Address"),
_T("FS Reserver Blocks"),//
_T("RAM Type: "),
_T("Size(M): "),
_T("Bank: "),//
_T("Row Length: "),//
_T("Col Address: "),
_T("ChipSel:"),
_T("Manual"),
_T("Chip0"),
_T("Chip1"),
_T("Chip2"),//
_T("Chip3"),
_T("All"),//
_T("Init USB"),//
_T("Plug GPIO"),
_T("Write Register"),//
_T("Register: "),
_T("Bit: "),

_T("Driver List: "),
_T("N.O."),
_T("Symbol"),
_T("Type"),
_T("Readonly"),
_T("Hidden"),
_T("Size(K)"),
_T("volume"),
_T("FS Reserver Blocks"),
_T("NonFS Reserve Size"),

_T("Nandflash List: "),//
_T("SpiNandflash List: "),//
_T("Spiflash List:"),//

_T("Start Infineon module burn"),
_T("Infineon module burn fail"),
_T("device_synchronized!"),
_T("failed to boot target board"),//
_T("Start to download fls file"),
_T("Start to download eep file"),//
_T("failed to download fls file"),//
_T("failed to download eep file"),//

_T("download mode"),
_T("AK only"),
_T("Infineon only"),
_T("Both"),
_T("Burn Setting"),
_T("Baudrate: "),
_T("download fls:"),
_T("download eep:"),//
_T("Browse"),
_T("GPIO Setting"),//
_T("GPIO Setting_fail"),//
_T("fail to set param to producer"),

_T("start to download bin"),
_T("fail to download bin"),
_T("start to download img"),
_T("fail to download img"),
_T("format driver name and img driver name diffrent"),
_T("start to download boot"),
_T("fail to download boot"),


_T("start write mac addr into asa"),
_T("write mac addr into asa fail"),
_T("start write serial addr into asa"),
_T("write serial addr into asa fail"),

_T("Begin to download change clk"),//
_T("Fail to download change clk"),//
_T("loading change clk time out"),//
_T("Fail to connet usb after download change clk"),//

_T("set burn mode"),
_T("set burn mode fail"),//
_T("set burn erase mode"),//
_T("set burn erase mode fail"),//
_T("set nand para"),
_T("set nand para fail"),//
_T("set security area"),
_T("set security area fail"),//
_T("bin upload fail"),
_T("get bad block information fail"),//

_T("read mac addr in asafile"),
_T("read mac addr  error"),
_T("read serial addr in asafile"),
_T("read serial addr error"),	

_T("burn mac addr compare"),//
_T("burn mac addr beyond the most"),//
_T("burn serial addr compare"),//
_T("burn serial addr beyond the most"),	//

_T("set reserve area"),//
_T("set reserve area fail"),//
_T("create partition"),//
_T("create partition fail"),//
_T("close"),//
_T("close fail"),

_T("pls input the num must be 0~9��A~F��a~f"),//
_T("MAC Addr format must be 00:00:00"),
_T("mac start addr high Length less than 8 !!"),
_T("mac start addr low Length less than 8 !!"),
_T("mac end addr high Length less than 8 !!"),
_T("mac end addr low Length less than 8 !!"),
_T("the string is not 0~9��A~F��a~f !!!!"),
_T("h_mac_start_addr and h_end_addr is differnt!!"),//
_T("l_mac_start addr is more than l_mac_end_addr !!"),//
_T("h_serial_start_addr str Length less than 10 !!"),///
_T("l_serial_start_addr str Length less than 6 !!"),//
_T("h_serial_end_addr str Length less than 10 !!"),//
_T("l_serial_end_addr str Length less than 6 !!"),//
_T("h_serial_start_addr and h_end_addr is differnt!!"),//
_T("l_serial_start_addr is more than l_end_addr!!"),//
_T("mac addr  reset succeed, current = start !!"),//
_T("current serial  reset succeed, current = start !!"),//


_T("start set channel id"),
_T("get the channel id is error"),//

_T("Image Create Box"),
_T("sourcefolder:"),
_T("destfolder:"),//
_T("Browse"),//
_T("Browse"),//
_T("Imagename:"),//
_T("capacity(MByte):"),//
_T("pagesize(Byte):"),//
_T("sectorsize(Byte):"),//
_T("make information:"),//
_T("Ready Image make..."),//
_T("Image making..."),//
_T("Begin"),//
_T("End"),//
_T("Image make success!!"),//
_T("Image make fail!!"),//
_T("getting folder info!!"),//
_T("get folder info fail!!"),//
_T("capacity size is no enough!!"),//
_T("param error!!"),//

_T("source folder empty!"),//
_T("Destination folder empty!"),//
_T("image name empty!"),
_T("capacity empty!"),
_T("sector empty!"),
_T("page size empty!"),


_T("set nand gpioce fail!"),
_T("download channel addr fail!"),
_T("set com mode fail!"),
_T("download com mode fail!"),

_T("image making"),
_T("make image success!"),
_T("make image fail!"),
_T("finish make image and reset!"),

_T("Process is running, make sure to close or no!"),

_T("Input verify string"),
_T("Import file success!"),
_T("Import file fail!"),
_T("wait..."),
_T("Export file success!"),
_T("Export file fail!"),

_T("All data will be lost, make sure to format or not"),
_T("Spiflash Mirror"),
_T("Choose the spi,please!"),
_T("SPI:"),
_T("No SPI can choose"),
_T("Open help file error"),

_T("Physical Format"),
_T("OK"),
_T("Cancel"),
_T("driver volume name must not be"),

_T("the mac addr is special addr��please modification"),//
_T("the mac addr can not be zero"),//
_T("get highID fail,produce is not too old"),//
_T("before creat disk to get all free block"),//
_T("the capacity of the nand/sd is differnt"),//
//_T("the capacity must be same and to burn,please check"),
_T("check the capacity of ervery device"),
_T("bin upload complete"),
_T("bin upload, and pelease enter massboot to start"),
_T("spiflash upload complete"),
_T("spiflash upload fail"),
_T("spiflash upload, and pelease enter massboot to start"),
_T("start upload spiflash all data"),

_T("read OTP serial"),
_T("read OTP serial fail"),
_T("write OTP serial"),
_T("write OTP serial fail"),
_T("OTP serial is existed"),

_T("no find bin name in partition"),
_T("get partiton size error"),
_T("start burn ENV"),
_T("burn ENV FAIL"),

_T("partition name and bin name diffrent"),
_T("download spiflash img fail"),
_T("SPI img file only one"),
_T("download spiflash img start"),
_T("download spiflash img finish"),

_T("Erase For Start Burn(erase all partition size,erase size is 64K)"),
_T("No Need Erase For Burn(the spiflash is new,no need erase)"),

_T("name len more than 8"),
_T("start erase partition"),
_T("erase partition fail"),
_T("check fs type fail"),

_T("get blue addr start"),
_T("blue addr more than end addr"),
_T("blue addr len error"),
_T("blue addr error"),
_T("write blue addr fail"),
_T("start write blue addr"),

_T("erase block more than total num"),

NULL
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLang::CLang()
{
	m_stringAlloc = NULL;//
	m_offsetAlloc = NULL;//

	m_langCount = 2;
	m_langName[0] = _T("����");//��ʼ������
	m_langName[1] = _T("English");//��ʼ��Ӣ��

	m_activeLang = 0xFFFFFFFF;//
}

CLang::~CLang()
{	
	if(m_stringAlloc)
	{
		delete[] m_stringAlloc;//�ͷ�
		m_stringAlloc = NULL;//�ÿ�
	}

	if(m_offsetAlloc)//
	{
		delete[] m_offsetAlloc;//�ͷ�
		m_offsetAlloc = NULL;//�ÿ�
	}
}

BOOL CLang::Init(CString langName)
{
	ScanLang(theApp.ConvertAbsolutePath(_T("Lang")));//�ļ���
	
	UINT i;
	UINT activeLang = 1;//

	for(i = 0; i < m_langCount; i++)
	{
		if(m_langName[i] == langName)//
		{
			activeLang = i;//
		}
	}

	if(FALSE == ChangeLang(activeLang))//�任����
	{
		LoadDefaultString(FALSE);//
		m_activeLang = 1;
	}

	return TRUE;
}

CString CLang::GetString(UINT strID)
{
	if(NULL==m_offsetAlloc || NULL==m_stringAlloc || strID >= IDS_MAX)
	{
		return CString(_T(""));//
	}

	CString theStr;
	theStr.Format(_T("%s"), m_stringAlloc+m_offsetAlloc[strID]);//

	return theStr;
}

BOOL CLang::LoadDefaultString(BOOL bCN)
{
	TCHAR **pStr = NULL;
	UINT strCount = IDS_MAX;

	if(bCN)
	{
		pStr = g_strCN;//��
	}
	else
	{
		pStr = g_strEN;//Ӣ
	}

	m_offsetAlloc = new	UINT[strCount];//����
	if(NULL == m_offsetAlloc)
	{
		return FALSE;//
	}

	m_stringAlloc = new TCHAR[strCount * LANG_CHINA_ENGLISH_LEN];//����
	if(NULL == m_stringAlloc)//
	{
		return FALSE;
	}

	memset(m_stringAlloc, 0, sizeof(TCHAR)*strCount*LANG_CHINA_ENGLISH_LEN);//��0

	UINT i, offset;//
	UINT len;

	offset = 0;//
	i = 0;
	while(i < strCount && pStr[i])
	{
		len = _tcslen(pStr[i])*sizeof(TCHAR);
		memcpy(m_stringAlloc+offset, pStr[i], len);//����
		m_stringAlloc[offset+len] = 0;

		m_offsetAlloc[i] = offset;//ƫ��

		offset += len+1;//
		i++;//
	}

	if(i < strCount)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CLang::LoadStringFromFile(CString strPath)
{
	HANDLE hFile;
	DWORD dwSize;
	DWORD read_count;
	UINT len;
	
	if(0xFFFFFFFF == GetFileAttributes(strPath))//����
	{
		return FALSE;
	}

	hFile = CreateFile(strPath, GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, 0, NULL);//����
	if(INVALID_HANDLE_VALUE == hFile)//
	{
		return FALSE;
	}

	dwSize = GetFileSize(hFile, NULL);//��ȡ�ļ�����
	if(0xFFFFFFFF == dwSize)
	{
		CloseHandle(hFile);//�ر�
		return FALSE;
	}

	if(m_stringAlloc)//
	{
		delete[] m_stringAlloc;//
		m_stringAlloc = NULL;
	}
	
	// Alloc Space for String buffer
	len = dwSize/sizeof(TCHAR) + 2;//��		
	m_stringAlloc = new TCHAR[len];
	if(NULL == m_stringAlloc)
	{
		CloseHandle(hFile);//
		return FALSE;
	}
	memset(m_stringAlloc, 0, len);//��0

	if(NULL == m_offsetAlloc)//
	{
		m_offsetAlloc = new UINT[IDS_MAX];//
		if(NULL == m_offsetAlloc)
		{
			CloseHandle(hFile);//
			return FALSE;
		}
	}

	ReadFile(hFile, m_stringAlloc, dwSize, &read_count, NULL);//��ȡ

	CloseHandle(hFile);//�ر�

	UINT indexString = 0, indexOffset = 0;//
	while(indexOffset < IDS_MAX)//
	{
		while(indexString<len && '"' != m_stringAlloc[indexString++]);//
		m_offsetAlloc[indexOffset++] = indexString;//
		
		while(indexString<len && '"' != m_stringAlloc[indexString++]);//
		m_stringAlloc[indexString-1] = 0;//

		if(indexString > len)
		{
			break;
		}
	}

	if(indexOffset < IDS_MAX)
	{
		return FALSE;
	}
	
	return TRUE;
}

BOOL CLang::WriteString()
{
	CString str;
	UINT i;

	if(NULL == m_stringAlloc || NULL == m_offsetAlloc)//
	{
		return FALSE;
	}

	CreateDirectory(theApp.ConvertAbsolutePath(_T("Lang")), NULL);//�����ļ���

	str.Format(_T("Lang\\%s.txt"), m_langName[m_activeLang]);
	if(0xFFFFFFFF != GetFileAttributes(theApp.ConvertAbsolutePath(str)))//����
	{
		return TRUE;
	}

	//open file
	CStdioFile *pFile;
	pFile = new CStdioFile(theApp.ConvertAbsolutePath(str), 
		CFile::modeCreate | CFile::modeWrite | CFile::typeBinary | CFile::shareDenyNone);
	if(NULL == pFile)
	{
		return FALSE;
	}

	USHORT head = UNICODE_TXT_HEAD;
	pFile->Write(&head, 2);
	
	pFile->WriteString(_T("Lang"));//
	pFile->WriteString(_T("{\r\n"));//

	for(i = 0; i < IDS_MAX; i++)
	{
		str.Format(_T("\"%s\",\r\n"), m_stringAlloc+m_offsetAlloc[i]);//
		pFile->WriteString(str);
	}

	pFile->WriteString(_T("}\r\n"));//

	pFile->Close();
	delete pFile;

	return TRUE;
}

UINT CLang::ScanLang(CString dirPath)
{
	CString langName;
	CString fileName;
	BOOL bFound;
	UINT pos;
	CFileFind cFF;
	
	bFound = cFF.FindFile(dirPath+"\\*.txt");
	if(bFound)
	{
		do
		{
			bFound = cFF.FindNextFile();//����һ��
			
			fileName = cFF.GetFileTitle();//
			if(fileName != _T("����") && fileName != _T("English"))//����ʱ
			{
				pos = fileName.Find('.');//
				langName = fileName.Left(pos);//
				m_langName[m_langCount] = fileName;//
				m_langCount++;
			}
			
		}
		while(bFound);//
	}
	else
	{
		return 0;
	}

	return m_langCount-2;//
}

BOOL CLang::ChangeLang(UINT langID)
{
	if(langID == m_activeLang || langID >= m_langCount)//
	{
		return FALSE;
	}

	CString strLangFile;

	strLangFile.Format(theApp.ConvertAbsolutePath(_T("lang\\%s.txt")), m_langName[langID]);
	if(LoadStringFromFile(strLangFile))//����
	{
		m_activeLang = langID;//
	}
	else
	{
		if(langID < 2)//
		{
			LoadDefaultString(langID==0);//
			m_activeLang = langID;
		}
		else
		{
			return FALSE;
		}
	}
	
	return TRUE;
}
