// PageFormat.cpp : implementation file
//

#include "stdafx.h"
#include "burntool.h"
#include "PageFormat.h"
#include "fs.h"
#include "medium.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CConfig theConfig;
extern CBurnToolApp theApp;
extern BOOL    g_bEraseMode;

T_U16 special_char[] = {0x3a, 0x2a, 0x3f, 0x5c, 0x22, 0x3c, 0x3e, 0x7c, 0x2e, 0x0};   //:*?\"<>|.

/////////////////////////////////////////////////////////////////////////////
// CPageFormat property page

IMPLEMENT_DYNCREATE(CPageFormat, CPropertyPage)

CPageFormat::CPageFormat() : CPropertyPage(CPageFormat::IDD)
{
	//{{AFX_DATA_INIT(CPageFormat)
		// NOTE: the ClassWizard will add member initialization here
		f_initFlag = FALSE;
	//}}AFX_DATA_INIT
}

CPageFormat::~CPageFormat()
{
}

void CPageFormat::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageFormat)
	DDX_Control(pDX, IDC_LIST_FS_DISK_FORMAT, m_fs_disk_format_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageFormat, CPropertyPage)
	//{{AFX_MSG_MAP(CPageFormat)
	ON_BN_CLICKED(IDC_BUTTON_LOW_FORMAT, OnButtonLowFormat)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageFormat message handlers
BOOL CPageFormat::get_config_data(CConfig &config)
{
	TCHAR temp[DOWNLOAD_BIN_FILENAME_SIZE+2] = {0};
	CString str;
	CString str_temp;
	UINT i; //, n, m;
	//char volume_temp[12] = {0};

	int count = m_fs_disk_format_list.GetItemCount();
	//config.format_count = count;
	config.partition_count = count;

	USES_CONVERSION;

	//if(config.format_data != NULL)//format_data�������ڴ�
	//{
	//	delete[] config.format_data;//�ͷ�
	//	config.format_data = NULL;//�ÿ�
	//}
	//config.format_data = new T_PARTION_INFO[config.format_count];//����
	//memset(config.format_data, 0, sizeof(T_PARTION_INFO)*config.format_count);//��0

	//if(config.spi_format_data != NULL)//spi_format_data�������ڴ�
	//{
	//	delete[] config.spi_format_data;//�ͷ�
	//	config.spi_format_data = NULL;//�ÿ�
//	}
//	config.spi_format_data = new T_SPIFLASH_PARTION_INFO[config.format_count];//����
//	memset(config.spi_format_data, 0, sizeof(T_SPIFLASH_PARTION_INFO)*config.format_count);//��0


	if(config.partition_data != NULL)//format_data�������ڴ�
	{
		delete[] config.partition_data;//�ͷ�
		config.partition_data = NULL;//�ÿ�
	}
	config.partition_data = new T_ALL_PARTITION_INFO[config.partition_count];//����
	memset(config.partition_data, 0, sizeof(T_ALL_PARTITION_INFO)*config.partition_count);//��0


	//���������ڴ�
	//if (NULL != config.pVolumeLable)
	//{
	//	delete[] config.pVolumeLable;//�ͷ�
	//	config.pVolumeLable = NULL;//�ÿ�
	//}		
	//config.pVolumeLable = new T_VOLUME_LABLE[config.format_count];//����
	//memset(config.pVolumeLable, 0, config.format_count*sizeof(T_VOLUME_LABLE));

	for(i = 0; i < config.partition_count; i++)
	{

		str = m_fs_disk_format_list.GetItemText(i, 1);
        str.MakeUpper();//���д
		memset(config.partition_data[i].Disk_Name, 0, (DOWNLOAD_BIN_FILENAME_SIZE + 2)*sizeof(TCHAR));
		memset(temp, 0, (DOWNLOAD_BIN_FILENAME_SIZE+2)*sizeof(TCHAR));
		
		_tcsncpy(temp, str, DOWNLOAD_BIN_FILENAME_SIZE);
		if(_tcslen(temp) > DOWNLOAD_BIN_FILENAME_SIZE)
		{
			str_temp.Format(_T("%s len more than %d"), str, DOWNLOAD_BIN_FILENAME_SIZE);
			AfxMessageBox(str_temp, MB_OK);
		}

		_tcsncpy(config.partition_data[i].Disk_Name, temp, DOWNLOAD_BIN_FILENAME_SIZE);


        str = m_fs_disk_format_list.GetItemText(i, 2);
        if(str == _T("DATA"))
		{
			config.partition_data[i].type = FHA_DATA_TYPE;//DATA
		}
		else if(str == _T("BIN"))
		{
			config.partition_data[i].type = FHA_BIN_TYPE;//BIN
		}
		else
		{
			config.partition_data[i].type = FHA_FS_TYPE;//FS
		}

		str = m_fs_disk_format_list.GetItemText(i, 3);
		config.partition_data[i].Size = atoi(T2A(str));//��С

    
        str = m_fs_disk_format_list.GetItemText(i, 4);
        if(str == "NO")
        {
            config.partition_data[i].r_w_flag = FHA_READ_WRITE;//��׼
        }
        else
        {
            config.partition_data[i].r_w_flag = FHA_ONLY_READ;//ֻ��
        }

		str = m_fs_disk_format_list.GetItemText(i, 5);
        if(str == "YES")
        {
            config.partition_data[i].hidden_fag = FHA_HIDDEN;//��׼
        }
        else
        {
            config.partition_data[i].hidden_fag = FHA_NOT_HIDDEN;//ֻ��
        }

		/*
		str = m_fs_disk_format_list.GetItemText(i, 6);
		//���������Ϊ����֤������Ƿ�����зǷ����ַ�
		memset(volume_temp, 0, sizeof(config.pVolumeLable[i].volume_lable));//��0
		memcpy(volume_temp, T2A(str), sizeof(config.pVolumeLable[i].volume_lable)-1);//����
		
		for (m = 0; volume_temp[m] != 0; m++)//�ж�volume_temp�Ƿ������:*?\"<>|.
		{
			for (n = 0; n < 9; n++)
			{
				if (volume_temp[m] == special_char[n])//ÿһ���ַ������ж�
				{
					//����У���ô����ʾ������մ˾����
					AfxMessageBox(theApp.GetString(IDS_VOLUME_LABLE), MB_OK);
					return FALSE;
				}
			}
		}
		memset(config.pVolumeLable[i].volume_lable, 0, sizeof(config.pVolumeLable[i].volume_lable));//
		memcpy(config.pVolumeLable[i].volume_lable, T2A(str), sizeof(config.pVolumeLable[i].volume_lable)-1);
		*/

	}


	//GetDlgItemText(IDC_EDIT_NONFS_RESV, str);
	//config.nonfs_res_size = atoi(T2A(str));//���ļ�ϵͳ��

	//GetDlgItemText(IDC_EDIT_FS_RESERVE, str);
	//config.fs_res_blocks = atoi(T2A(str));//�ļ�ϵͳ��

	return TRUE;
}

BOOL CPageFormat::set_config_item(CConfig &config)
{
	CString str;
	CString strIndex;
	CString str_temp;
	int len = 0;
	TCHAR buf_temp[DOWNLOAD_BIN_FILENAME_SIZE + 2] = {0};
	//char buf[12] = {0};

	USES_CONVERSION;

	for(UINT i = 0; i< config.partition_count; i++)
	{		
		strIndex.Format(_T("%d"), i + 1);
		m_fs_disk_format_list.InsertItem(i, strIndex);//���
		
		str.Format(_T("%s"),config.partition_data[i].Disk_Name);
		if(str.GetLength() > (DOWNLOAD_BIN_FILENAME_SIZE + 1))
		{
			str_temp.Format(_T("%s len more than %d"), config.partition_data[i].Disk_Name, DOWNLOAD_BIN_FILENAME_SIZE);
			AfxMessageBox(str_temp, MB_OK);
		}
		memset(buf_temp, 0, (DOWNLOAD_BIN_FILENAME_SIZE + 2)*sizeof(TCHAR));
		_tcsncpy(buf_temp, config.partition_data[i].Disk_Name, DOWNLOAD_BIN_FILENAME_SIZE);

		str.Format(_T("%s"), buf_temp);//����
        str.MakeUpper();//��д
		m_fs_disk_format_list.SetItemText(i, 1, str);

        if(FHA_DATA_TYPE == config.partition_data[i].type)
        {
            m_fs_disk_format_list.SetItemText(i, 2, _T("DATA"));//�û�
        }
		else if (FHA_BIN_TYPE == config.partition_data[i].type)
        {
            m_fs_disk_format_list.SetItemText(i, 2, _T("BIN"));//�û�
		}
        else
        {
            m_fs_disk_format_list.SetItemText(i, 2, _T("FS"));//���û�
        }

		str.Format(_T("%d"), config.partition_data[i].Size);//������С
		m_fs_disk_format_list.SetItemText(i, 3, str);//A2T(buf));

        if(config.partition_data[i].r_w_flag == FHA_READ_WRITE)
        {
            m_fs_disk_format_list.SetItemText(i, 4, _T("NO"));//��׼
        }
        else 
        {
            m_fs_disk_format_list.SetItemText(i, 4, _T("YES"));//ֻ��
        }

		if(config.partition_data[i].hidden_fag == FHA_HIDDEN)
        {
            m_fs_disk_format_list.SetItemText(i, 5, _T("YES"));//��׼
        }
        else 
        {
            m_fs_disk_format_list.SetItemText(i, 5, _T("NO"));//ֻ��
        }

        //if(config.format_data[i].ZoneType == ZT_PRIMARY)//
        //{
        //    m_fs_disk_format_list.SetItemText(i, 4, _T("PRIMARY"));//��
        //}
        //else if(config.format_data[i].ZoneType == ZT_SLAVE)//
        //{
        //    m_fs_disk_format_list.SetItemText(i, 4, _T("SLAVE"));//��
        //}
		
		//m_fs_disk_format_list.SetItemText(i, 6, A2T(config.pVolumeLable[i].volume_lable));//���
	}
	
	//str.Format(_T("%d"), config.nonfs_res_size);//���ļ�ϵͳ��
	//SetDlgItemText(IDC_EDIT_NONFS_RESV, str);
	
	
	//str.Format(_T("%d"), config.fs_res_blocks);//�ļ�ϵͳ��
	//SetDlgItemText(IDC_EDIT_FS_RESERVE, str);
	
	return TRUE;
}

BOOL CPageFormat::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	CString str;

	SetupDisplay();//
	
	ListView_SetExtendedListViewStyle(m_fs_disk_format_list.m_hWnd, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT); 

	str = theApp.GetString(IDS_FORMAT_NUMBER);
	m_fs_disk_format_list.InsertColumn(0, str, LVCFMT_LEFT, 40);//���к�

	str = theApp.GetString(IDS_FORMAT_DISK_SYMBOL);
	m_fs_disk_format_list.InsertColumn(1, str, LVCFMT_LEFT, 80);//����

	str = theApp.GetString(IDS_FORMAT_USER_DISK);
    m_fs_disk_format_list.InsertColumn(2, str, LVCFMT_LEFT, 60);//�û����

    m_fs_disk_format_list.SetSubItemPopItem(2, T_POP_CMBBOX, _T("DATA|BIN|FS|"));

	str = theApp.GetString(IDS_FORMAT_DISK_SIZE);
	m_fs_disk_format_list.InsertColumn(3, str, LVCFMT_LEFT, 80);//��С

	str = theApp.GetString(IDS_FORMAT_DISK_INFOR);
	m_fs_disk_format_list.InsertColumn(4, str, LVCFMT_LEFT, 80);

    m_fs_disk_format_list.SetSubItemPopItem(4, T_POP_CMBBOX, _T("NO|YES|"));//��Ϣ

	str = theApp.GetString(IDS_FORMAT_HIDE_FLAG);
	m_fs_disk_format_list.InsertColumn(5, str, LVCFMT_LEFT, 80);
    m_fs_disk_format_list.SetSubItemPopItem(5, T_POP_CMBBOX, _T("NO|YES|"));//��Ϣ
	
	
	//str = theApp.GetString(IDS_FORMAT_DISK_ATTR);
    //m_fs_disk_format_list.InsertColumn(4, str, LVCFMT_LEFT, 80);//����

    //m_fs_disk_format_list.SetSubItemPopItem(4, T_POP_CMBBOX, _T("PRIMARY|SLAVE|"));

	//str = theApp.GetString(IDS_FORMAT_DISK_VOLUME);
	//m_fs_disk_format_list.InsertColumn(6, str, LVCFMT_LEFT, 60);//���
	
	set_config_item(theConfig);//ʵʼ����
	
	f_initFlag = TRUE;
	ShowLowFormat();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CPageFormat::SetupDisplay()
{
	CString str;

	str = theApp.GetString(IDS_FORMAT_LIST);//
	GetDlgItem(IDC_STATIC_FS_FORMAT)->SetWindowText(str);//�����б�

	//str = theApp.GetString(IDS_FORMAT_FS_RESV);//
	//GetDlgItem(IDC_STATIC_FS_RESERVE)->SetWindowText(str);//�ļ�ϵͳ������

	//str = theApp.GetString(IDS_FORMAT_NONFS_RESV);//
	//GetDlgItem(IDC_STATIC_NONFS_RESV)->SetWindowText(str);//���ļ�ϵͳ������

	str = theApp.GetString(IDS_BUTTON_LOW_FORMAT);//
	GetDlgItem(IDC_BUTTON_LOW_FORMAT)->SetWindowText(str);//�͸�

}

void CPageFormat::ShowLowFormat()
{
	if (!f_initFlag)//
	{
		return;
	}
	if (theConfig.burn_mode == E_CONFIG_NAND 
		|| theConfig.burn_mode == E_CONFIG_SPI_NAND
		|| theConfig.burn_mode == E_CONFIG_SFLASH)//
	{
		((CButton*)GetDlgItem(IDC_BUTTON_LOW_FORMAT))->ShowWindow(TRUE);//nand��ʾ�͸�
	}
	else
	{
		((CButton*)GetDlgItem(IDC_BUTTON_LOW_FORMAT))->ShowWindow(FALSE);//����
	}


}

void CPageFormat::OnButtonLowFormat() 
{
	// TODO: Add your control notification handler code here
	//	CUpdateBase update;

	//CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();

	//((CButton*)GetDlgItem(ID_TBTN_FORMAT))->EnableWindow(FALSE);//��ʽ���У������ٴε��
	
	if(IDOK == MessageBox(theApp.GetString(IDS_PHYSICAL_FORMAT),NULL,MB_OKCANCEL|MB_ICONEXCLAMATION))
	{
		g_bEraseMode = TRUE;
		//BT_Start();
		/*
		if (!BT_Start())
		{
            g_bEraseMode = FALSE;
		}
		*/
	}
	else
	{
		g_bEraseMode = FALSE;
	}
}
