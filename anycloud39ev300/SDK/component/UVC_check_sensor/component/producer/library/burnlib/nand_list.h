/**
 * @filename nand_list.h
 * @brief: AK3223M interrupt
 *
 * This file describe what is in the table of nand list
 * Copyright (C) 2006 Anyka (GuangZhou) Software Technology Co., Ltd.
 * @author  zhaojiahuan
 * @modify chenyanyan
 * @date    2007-1-10
 * @version 1.0
 * @ref
 */

#ifndef __CHIP_NFC_3224__
#define __CHIP_NFC_3224__

#include	"anyka_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup NandFlash Architecture NandFlash Interface
 *  @ingroup Architecture
 */
/*@{*/

/**
* @BRIEF    Nandflash info define
* @AUTHOR   zhaojiahuan
* @DATE     2006-7-17
*/

typedef struct
Nand_phy_info{
    unsigned long  chip_id;//chip id
    unsigned short  page_size; //page size
    unsigned short  page_per_blk; //page of one blok
    unsigned short  blk_num;//total block number
    unsigned short  group_blk_num;//the same concept as die, according to nand's struture
    unsigned short  plane_blk_num;   
    unsigned char   spare_size;//spare�����С�ĵ�λ��������255 Byte
    unsigned char   col_cycle;//column address cycle
    unsigned char   lst_col_mask;//last column  addrress cycle mask bit
    unsigned char   row_cycle;//row address cycle
    unsigned char   delay_cnt;//Rb delay, unit is 1024 asic clock, default value corresponds to 84MHz
    unsigned char   custom_nd;//nand type flag, used to detect the original invilid block
        //currently there are 7 types, more types might be added when new nand come out
        //˵����������ǰһ����page��,��һ����page�е�λ��, �����Щλ�ò�Ϊ0xFF���block�ǳ�������
        //NAND_TYPE_SAMSUNG:        0x1 СҳSLC([0,1],[517]),   ��ҳSLC([0,1],[2048]),          MLC([127], [2048/4096])
        //NAND_TYPE_HYNIX:          0x2 СҳSLC([0,1],[517]),   ��ҳSLC([0,1],[2048]),          MLC([125,127], [2048/4096])
        //NAND_TYPE_TOSHIBA:        0x3 СҳSLC([0,1],[0,512]), ��ҳSLC([0,1],[0,2048]),        MLC([127], [0,2048/4096])
        //NAND_TYPE_TOSHIBA_EXT:    0x4 СҳSLC(),              ��ҳSLC(),                      MLC([0,127/255], [0,2048/4096/8192])
        //NAND_TYPE_MICRON:         0x5 СҳSLC([0,1],[512]),   ��ҳSLC([0,1],[2048]),          MLC([0,1], [2048/4096])
        //NAND_TYPE_ST:             0x6 СҳSLC([0,1],[517]),   ��ҳSLC([0],[2048,2053]),       MLC([127], [0])
        //NAND_TYPE_MICRON_4K       0x7 СҳSLC(),              ��ҳSLC(),                      MLC([0], [4096 ~ 4096+218])
    unsigned long  flag;//character bits, ���4λ��ʾplane���ԣ����λ��ʾ�Ƿ���Ҫblock��˳��дpage
    //bit31��ʾ�Ƿ���copyback��1��ʾ��copyback
    //bit30��ʾ�Ƿ�ֻ��һ��plane��1��ʾֻ��һ��plane
    //bit29��ʾ�Ƿ�ǰ��plane��1��ʾ��ǰ��plane
    //bit28��ʾ�Ƿ���żplane��1��ʾ����żplane

    //����bit��Ϊ�˽��page��block��ַ�����������ӵĿ���bit:
    //bit11��ʾblock number per die�Ƿ���Ҫ���Ϲ�������Toshiba TH58NVG6D2ETA20��2048 block/die(ʵ����2084 block/die)
         //Ϊ�˶�����һ��die��block����Ҫ����Ϊ4096 block/die���ײ�����
    //bit10��ʾpage number�Ƿ���Ҫ���Ϲ�������TLC��192page/block��Ϊ�˶�����һ��block����Ҫ����Ϊ256page/block������
    
    //bit8~9��ʾspare�����С�ĸ�λ����λ��256 Bytes����spare_size��Ϊunsigned char�������Ա�ʾ����nand��400����ֽڵ�spare��С
    //bit4-7��ʾECC���ͣ�0Ϊ4 bit/512B��1Ϊ8 bit/512B��2Ϊ12 bit/512B��3Ϊ16 bit/512B��4Ϊ24 bit/1024B��5Ϊ32 bit/1024B
    //bit0��ʾ��ͬһ��block���Ƿ���Ҫ˳��дpage��1��ʾ��Ҫ��˳��д������nandΪMLC
    //ע��: ���(bit29��bit28)Ϊ'11'�����ʾ��chip����4��plane��������żҲ��ǰ��plane

    unsigned long  cmd_len;//nandflash command length
    unsigned long  data_len;//nandflash data length
    unsigned char   des_str[32];//descriptor string
}T_NAND_PHY_INFO;

#define ERROR_CHIP_ID   0xFFFFFFFF

/*@}*/
#ifdef __cplusplus
}
#endif

#endif
