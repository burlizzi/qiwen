/*
 * @(#)nandflash.h
 * @date 2009/06/18
 * @version 1.0
 * @author: aijun.
 * @Leader:xuchuang
 * Copyright 2015 Anyka corporation, Inc. All rights reserved.
 * ANYKA PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */


#ifndef __NANDFLASH_H__
#define __NANDFLASH_H__

#include "anyka_types.h"

/* Corresponding to ChipCharacterBits */
#define NANDFLASH_MAPPING_HORIZONTAL    0x10000000	// Horizontal-mapping mode

//nand������
#define NANDFLASH_RANDOMISER        17
#define NANDFLASH_COPYBACK          16
#define NANDFLASH_MULTI_COPYBACK    15
#define NANDFLASH_MULTI_READ        14
#define NANDFLASH_MULTI_ERASE       13
#define NANDFLASH_MULTI_WRITE       12
#define NANDFLASH_CACHE_WRITE        1
#define NANDFLASH_CACHE_READ         2

//define the error code of nandflash
typedef enum tag_SNandErrorCode
{
    NF_SUCCESS        =    ((unsigned short)1),
    NF_FAIL           =    ((unsigned short)0),
    NF_WEAK_DANGER    =    ((unsigned short)-1),    			//successfully read, but a little dangerous.
    NF_STRONG_DANGER    =      ((unsigned short)-2),			//successfully read, but very dangerous, forbidden to use again.
}E_NANDERRORCODE;

typedef struct SNandflash  T_NANDFLASH;
typedef struct SNandflash *T_PNANDFLASH;


typedef E_NANDERRORCODE (*fNand_WriteSector)(T_PNANDFLASH nand, unsigned long chip,
                              unsigned long block, unsigned long page, const unsigned char data[], unsigned char* oob,unsigned long oob_len);
                              
typedef E_NANDERRORCODE (*fNand_ReadSector)(T_PNANDFLASH nand, unsigned long chip,
                              unsigned long block, unsigned long page, unsigned char data[], unsigned char* oob,unsigned long oob_len);

                              
typedef E_NANDERRORCODE (*fNand_WriteFlag)(T_PNANDFLASH nand, unsigned long chip,
                              unsigned long block, unsigned long page, unsigned char* oob,unsigned long oob_len);
                              
typedef E_NANDERRORCODE (*fNand_ReadFlag)(T_PNANDFLASH nand, unsigned long chip,
                              unsigned long block, unsigned long page, unsigned char* oob,unsigned long oob_len);
                              
typedef E_NANDERRORCODE (*fNand_CopyBack)(T_PNANDFLASH nand, unsigned long chip,
                              unsigned long SourceBlock, unsigned long DestBlock, unsigned long page);
                              
typedef E_NANDERRORCODE (*fNand_MultiCopyBack)(T_PNANDFLASH nand, unsigned long chip,
                              unsigned long PlaneNum, unsigned long SourceBlock, unsigned long DestBlock, unsigned long page);
                              
typedef E_NANDERRORCODE (*fNand_MultiReadSector)(T_PNANDFLASH nand, unsigned long chip,
                              unsigned long PlaneNum, unsigned long block, unsigned long page,unsigned char data[], unsigned char* SpareTbl,unsigned long oob_len);	//SpareTbl���ǰ���MutiPlaneNum��T_MTDOOB�����ָ��
                              
typedef E_NANDERRORCODE (*fNand_MultiWriteSector)(T_PNANDFLASH nand, unsigned long chip,
                              unsigned long PlaneNum, unsigned long block, unsigned long page,const unsigned char data[], unsigned char* SpareTbl,unsigned long oob_len);	//SpareTbl���ǰ���MutiPlaneNum��T_MTDOOB�����ָ��
                              
typedef E_NANDERRORCODE (*fNand_EraseBlock)(T_PNANDFLASH nand, unsigned long chip, unsigned long block);

typedef E_NANDERRORCODE (*fNand_MultiEraseBlock)(T_PNANDFLASH nand, unsigned long chip, unsigned long planeNum, unsigned long block);

typedef bool (*fNand_IsBadBlock)(T_PNANDFLASH nand, unsigned long chip, unsigned long block);

typedef bool (*fNand_SetBadBlock)(T_PNANDFLASH nand, unsigned long chip, unsigned long block);

typedef unsigned long  (*fNand_Fake2Real)(T_PNANDFLASH nand, unsigned long plane,
                                  unsigned long FakePhyAddr, unsigned long *chip);


//ExNFTL�ӿں���                        
typedef E_NANDERRORCODE (*fNand_ExReadFlag)(T_PNANDFLASH nand, unsigned long chip,
                              unsigned long block, unsigned long page, unsigned char* oob,unsigned long oob_len);
                              
typedef E_NANDERRORCODE (*fNand_ExReadSector)(T_PNANDFLASH nand, unsigned long chip,
                              unsigned long plane_num, unsigned long block, unsigned long page,unsigned char data[], unsigned char* spare_tbl,unsigned long oob_len, unsigned long page_num);//SpareTbl���ǰ���MutiPlaneNum��T_MTDOOB�����ָ��
                              
typedef E_NANDERRORCODE (*fNand_ExWriteSector)(T_PNANDFLASH nand, unsigned long chip,
                              unsigned long plane_num, unsigned long block, unsigned long page,const unsigned char data[], unsigned char* spare_tbl,unsigned long oob_len, unsigned long page_num);    //SpareTbl���ǰ���MutiPlaneNum��T_MTDOOB�����ָ��
   
typedef E_NANDERRORCODE (*fNand_ExEraseBlock)(T_PNANDFLASH nand, unsigned long chip, unsigned long plane_num, unsigned long block);

typedef bool (*fNand_ExIsBadBlock)(T_PNANDFLASH nand, unsigned long chip, unsigned long block);

typedef bool (*fNand_ExSetBadBlock)(T_PNANDFLASH nand, unsigned long chip, unsigned long block);

struct SNandflash			//���ʽṹ��˵��
{
     unsigned long   NandType;
    /* character bits, ���4λ��ʾplane���ԣ����λ��ʾ�Ƿ���Ҫblock��˳��дpage
       bit31��ʾ�Ƿ���copyback��1��ʾ��copyback
       bit30��ʾ�Ƿ�ֻ��һ��plane��1��ʾֻ��һ��plane
       bit29��ʾ�Ƿ�ǰ��plane��1��ʾ��ǰ��plane
       bit28��ʾ�Ƿ���żplane��1��ʾ����żplane
       bit0��ʾ��ͬһ��block���Ƿ���Ҫ˳��дpage��1��ʾ��Ҫ��˳��д������nandΪMLC
    ע��: ���(bit29��bit28)Ϊ'11'�����ʾ��chip����4��plane��������żҲ��ǰ��plane */
    unsigned long   ChipCharacterBits;
    unsigned long   PlaneCnt;
    unsigned long   PlanePerChip;
    unsigned long   BlockPerPlane;
    unsigned long   PagePerBlock;
    unsigned long   SectorPerPage;
    unsigned long   BytesPerSector;
    fNand_WriteSector   WriteSector;
    fNand_ReadSector    ReadSector;
    fNand_WriteFlag     WriteFlag;
    fNand_ReadFlag      ReadFlag;
    fNand_EraseBlock    EraseBlock;
    fNand_CopyBack      CopyBack;
    fNand_IsBadBlock    IsBadBlock;  
    fNand_SetBadBlock    SetBadBlock;
    fNand_Fake2Real     Fake2Real;	// the func of phy addr converting
    fNand_MultiReadSector    MultiRead;
    fNand_MultiWriteSector    MultiWrite;
    fNand_MultiCopyBack        MultiCopyBack;
    fNand_MultiEraseBlock     MultiEraseBlock;
    unsigned long     BufStart[1];
	
    //ExNFTL�ӿں���
    fNand_ExReadFlag      ExReadFlag;
    fNand_ExIsBadBlock    ExIsBadBlock;  
    fNand_ExSetBadBlock   ExSetBadBlock;
    fNand_ExReadSector    ExRead;
    fNand_ExWriteSector   ExWrite;
    fNand_ExEraseBlock    ExEraseBlock;
};
#endif

