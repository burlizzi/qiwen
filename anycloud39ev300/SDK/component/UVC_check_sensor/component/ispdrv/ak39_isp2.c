#include <stdlib.h>
#include "ak39_isp2.h"
#include "ak39_isp2_reg.h"
#include "ak39_isp2_3a.h"

#define DMA_CONFIG_REG_SIZE			3424
#define DMA_STAT_DATA_SIZE			4288
#define STATIC_DEAD_PIXELS_SIZE		(1024*4)

/* each register block bytes */
#define REG_BLK1_SIZE				236
#define REG_BLK2_SIZE				208
#define REG_BLK3_SIZE				168
#define REG_BLK4_SIZE				140
#define REG_BLK5_SIZE				132

/* each memory block bytes */
#define MEM_BLK1_SIZE				512
#define MEM_BLK2_SIZE				512
#define MEM_BLK3_SIZE				172
#define MEM_BLK4_SIZE				512
#define MEM_BLK5_SIZE				576
#define MEM_BLK6_SIZE				256

//#define ISP_DEBUG
#ifdef ISP_DEBUG
#define isp_dbg(stuff...)       printk("ISP: " stuff)
#else
#define isp_dbg(stuff...)   do{}while(0)
#endif 

//#undef REG32 //add by xc
#define REG32(_reg_)  (*(volatile unsigned long *)(_reg_))

AK_ISP_STRUCT *isp=NULL;

//static int printk_flag = 0;
//static int ccm_printk_flag = 0;

void isp2_print_reg_table(void) 
{
	unsigned int i = 0;
	unsigned int num = 0;
	unsigned int cmd;
	 
	cmd = 0;
	cmd =  REG32(isp->base+0x00);
	printk("reg0x00 = %x\n",cmd);

	cmd = 0;
	cmd =  REG32(isp->base+0x04);
	printk("reg0x04 = %x\n",cmd);
	 
	cmd = 0;
	cmd =  REG32(isp->base+0x08);
	printk("reg0x08 = %x\n",cmd);
	 
	cmd = 0;
	cmd =  REG32(isp->base+0x0c);
	printk("reg0x0c = %x\n",cmd);
	 
	cmd = 0;
	cmd =  REG32(isp->base+0x10);
	printk("reg0x10 = %x\n",cmd);
	 
	cmd = 0;
	cmd =  REG32(isp->base+0x14);
	printk("reg0x14 = %x\n",cmd);
	 
	cmd = 0;
	cmd =  REG32(isp->base+0x18);
	printk("reg0x18 = %x\n",cmd);
	 
	cmd = 0;
	cmd =  REG32(isp->base+0x1c);
	printk("reg0x1c = %x\n",cmd);
	 
	cmd = 0;
	printk("\n\n\n");
	 
	
	/* register block1, 236 bytes */
	num = (REG_BLK1_SIZE / 4);
	REG32(isp->base+REG_BLOCK_1)=0x01;
	for(i=0; i<num; i++)
	{
		cmd = REG32(isp->base+REG_BLOCK_1);
		printk("REGBLK1 reg0x%X[%03d] = %.8x:%.8x\n",
			REG_BLOCK_1, i, cmd, isp->reg_blkaddr1[i]);
	}
	printk("\n\n\n");

	/* register block2, 208 bytes */
	num = (REG_BLK2_SIZE / 4);
	REG32(isp->base+REG_BLOCK_2)=0x01;
	for(i=0; i<num; i++)
	{
		cmd =REG32(isp->base+REG_BLOCK_2);
		printk("REGBLK2 reg0x%X[%03d] = %.8x:%.8x\n",
			REG_BLOCK_2, i, cmd, isp->reg_blkaddr2[i]);
	}
	printk("\n\n\n");
	
	/* memory block1, 512 bytes */
	num = (MEM_BLK1_SIZE / 4);
	REG32(isp->base+MEM_BLOCK_1)=0x01;
	for(i=0; i<num; i++)
	{
		cmd = REG32(isp->base+MEM_BLOCK_1);
		printk("MEMBLK1 reg0x%X[%03d] = %.8x:%.8x\n",
			MEM_BLOCK_1, i, cmd, isp->cfg_mem_blkaddr1[i]);
	} 
	printk("\n\n\n");
	
	/* memory block2, 512 bytes */
	num = (MEM_BLK2_SIZE / 4);
	REG32(isp->base+MEM_BLOCK_2) =0x01; 
	for(i=0; i<num; i++)
	{
		cmd = REG32(isp->base+MEM_BLOCK_2);
		printk("MEMBLK2 reg0x%X[%03d] = %.8x:%.8x\n",
			MEM_BLOCK_2, i, cmd, isp->cfg_mem_blkaddr2[i]);
	}
	printk("\n\n\n");

	/* memory block3, 172 bytes */
	num = (MEM_BLK3_SIZE / 4);
	REG32(isp->base+MEM_BLOCK_3) =0x01;
	for(i=0; i<num; i++)
	{
		cmd = REG32(isp->base+MEM_BLOCK_3);
		printk("MEMBLK3 reg0x%X[%03d] = %.8x:%.8x\n",
			MEM_BLOCK_3, i, cmd, isp->cfg_mem_blkaddr3[i]);
	}
	printk( "\n\n\n");
	 
	/* memory block4, 512 bytes */
	num = (MEM_BLK4_SIZE / 4);
	REG32(isp->base+MEM_BLOCK_4) =0x01;
	for(i=0; i<num; i++)
	{
		cmd = REG32(isp->base+MEM_BLOCK_4);
		printk( "MEMBLK4 reg0x%X[%03d] = %.8x:%.8x\n",
			MEM_BLOCK_4, i, cmd, isp->cfg_mem_blkaddr4[i]);
	}
	printk("\n\n\n");

	/* memory block5, 576 bytes */
	num = (MEM_BLK5_SIZE / 4);
	REG32(isp->base+MEM_BLOCK_5) =0x01;
	for(i=0; i<num; i++)
	{
		cmd = REG32(isp->base+MEM_BLOCK_5);
		printk( "MEMBLK5 reg0x%X[%03d] = %.8x:%.8x\n",
			MEM_BLOCK_5, i, cmd, isp->cfg_mem_blkaddr5[i]);
	}
	printk("\n\n\n");

	/* memory block6, 256 bytes */
	num = (MEM_BLK6_SIZE / 4);
	REG32(isp->base+MEM_BLOCK_6) =0x01;
	for(i=0; i<num; i++)
	{
		cmd = REG32(isp->base+MEM_BLOCK_6);
		printk( "MEMBLK6 reg0x%X[%03d] = %.8x:%.8x\n",
			MEM_BLOCK_6, i, cmd, isp->cfg_mem_blkaddr6[i]);
	}
	printk("\n\n\n");

	/* register block3, 168 bytes */
	num = (REG_BLK3_SIZE / 4);
	REG32(isp->base+REG_BLOCK_3) =0x01;
	for(i=0; i<num; i++)
	{
		cmd = REG32(isp->base+REG_BLOCK_3);
		printk("REGBLK3 reg0x%X[%03d] = %.8x:%.8x\n",
			REG_BLOCK_3, i, cmd, isp->reg_blkaddr3[i]);
	}
	printk("\n\n\n");

	/* register  block4, 140 bytes */
	num = (REG_BLK4_SIZE / 4);
	REG32(isp->base+REG_BLOCK_4) = 0x01;
	for(i=0; i<num; i++)
	{
		cmd =REG32(isp->base+REG_BLOCK_4);
		printk("REGBLK4 reg0x%X[%03d] = %.8x:%.8x\n",
			REG_BLOCK_4, i, cmd, isp->reg_blkaddr4[i]);
	}
	printk("\n\n\n");

	/* register  block5, 132 bytes */
	num = (REG_BLK5_SIZE / 4);
	REG32(isp->base+REG_BLOCK_5) = 0x01;
	for(i=0; i<num; i++)
	{
		cmd = REG32(isp->base+REG_BLOCK_5);
		printk( "REGBLK5 reg0x%X[%03d] = %.8x:%.8x\n",
			REG_BLOCK_5, i, cmd, isp->reg_blkaddr5[i]);
	}
	printk("\n\n\n");    
}

int ak_isp_vi_set_input_size(int width, int height)
{
    unsigned int err_check;

    if (!isp)
        return -1;

    err_check = REG32(isp->base + 0x4);
    CLEAR_BITS(err_check, 0, 28);
    err_check |= (width<<EXP_HOR)|(height<<EXP_VERT);
    REG32(isp->base + CSI_ERR_CHECK_1-ISP_BASE_ADDR) = err_check;
    return 0;
}

int ak_isp_vi_set_sub128(int sub)
{
	unsigned int cmd;

	cmd = REG32(isp->base + CSI_ERR_CHECK_1);
	CLEAR_BITS(cmd, 30, 1);
	cmd |= sub ? (1<<30):0;
	REG32(isp->base + CSI_ERR_CHECK_1) = cmd;

	return 0;
}

int ak_isp_vo_check_irq_status (void)              //??��??D??����??
{
    unsigned int irq_status;
    if(!isp)
    	return 0;
    
    irq_status = REG32(isp->base + 0x10);

    return irq_status;
}

int ak_isp_vo_clear_irq_status(int bit)              //??3y?D??���䨬?
{
    if (!isp)
        return -1;
    
    REG32(isp->base + 0x10) |= bit;//0xbfff;
    return 0;
}

int ak_isp_vo_enable_irq_status(int bit)              //��1?��?D??���䨬?
{
	if (!isp)
        	return -1;
	REG32(isp->base + 0x14) = bit;//0x01;//0xffff;
	return 0;
}

int ak_isp_vo_set_cfg_reg(int regaddr, int value, int bitmask)
{
    int bits;

    switch(regaddr)
    {
    case 0x0:
        bitmask &=0x3e0000ff;
        break;
    case 0x04:
    case 0x08:
    case 0x0c:
        break;
    default:
        return 0;
    }
    
    bits = REG32(isp->base + regaddr);
    bits &= ~bitmask;
    bits |= value&bitmask;
    REG32(isp->base + regaddr) = bits;

    return 1;
}

AK_ISP_PCLK_POLAR ak_isp_get_pclk_polar(void)
{
	if (!isp)
		return POLAR_ERR;

	if (isp->misc_para.pclk_pol) {
		return POLAR_FALLING;
	}

	return POLAR_RISING;
}

int _isp_set_misc_attr(void)
{
	int tmp;
    unsigned int cmd = 0;
    isp_dbg("%s enter.\n", __func__);
    
    cmd = REG32(isp->base + GEN_CTRL_ADDR);
    CLEAR_BITS(cmd, 4, 4);
    CLEAR_BITS(cmd, 25, 5);
    CLEAR_BITS(cmd, 0, 2);
    cmd |= (LOW_BITS(isp->misc_para.cfa_mode, 2) << RAW_SEQ)
    	|  (LOW_BITS(isp->misc_para.hsyn_pol, 1) << HERF_POL)
    	|  (LOW_BITS(isp->misc_para.vsync_pol, 1) << VSYNC_POL)
    	|  (LOW_BITS(isp->misc_para.test_pattern_en, 1) << TEST_PATTERN_EN)
    	|  (LOW_BITS(isp->misc_para.test_pattern_cfg, 4) << TEST_PATTERN)
    	|  (LOW_BITS(isp->misc_para.inputdataw, 2) << DATA_WIDTH);

   tmp = isp->misc_para.inputdataw & 0x3;
   if (tmp == 0)
	   cmd |= 0x1 << DATA_WIDTH; //10bits input;
   else if (tmp==1)
	   cmd |= 0x2 << DATA_WIDTH; //8bits input;
   else if (tmp==2)
	   cmd |= 0x0 << DATA_WIDTH; //12bits input;

    REG32(isp->base + GEN_CTRL_ADDR) = cmd;

    cmd = REG32(isp->base + CSI_ERR_CHECK_2);
    CLEAR_BITS(cmd, 16, 16);
    cmd |= (LOW_BITS(isp->misc_para.one_line_cycle, 16) << ONE_LINE_CYCLE);
    REG32(isp->base + CSI_ERR_CHECK_2) = cmd;

    cmd = REG32(isp->base + CSI_HANDLE_ADDR);
    CLEAR_BITS(cmd, 31, 1);
    CLEAR_BITS(cmd, 8, 16);
    CLEAR_BITS(cmd, 2, 6);
    cmd |= (LOW_BITS(isp->misc_para.frame_start_delay_en, 1) << FRAME_START_DELAY_EN)
    	|  (LOW_BITS(isp->misc_para.hblank_cycle, 16) << HBLANK_RAW_CYCLE)
    	|  (LOW_BITS(isp->misc_para.frame_start_delay_num, 6) << FRAME_START_DELAY_NUM);
    REG32(isp->base + CSI_HANDLE_ADDR) = cmd;

	cmd = 0;
	cmd = isp->reg_blkaddr5[32];
	CLEAR_BITS(cmd, HFILP_EN, 2);
	cmd |= ((LOW_BITS(isp->misc_para.mirror_en, 1) << HFILP_EN)
		|  (LOW_BITS(isp->misc_para.flip_en, 1) << VFILP_EN));
    isp->reg_blkaddr5[32] = cmd;

    return 0;
}

int ak_isp_vo_set_misc_attr(AK_ISP_MISC_ATTR *p_misc)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(&(isp->misc_para),p_misc,sizeof(AK_ISP_MISC_ATTR));
    
    return 0;
}

int ak_isp_vo_get_misc_attr(AK_ISP_MISC_ATTR *p_misc)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(p_misc,&(isp->misc_para),sizeof(AK_ISP_MISC_ATTR));

    return 0;
}

int ak_isp_set_flip_mirror(int flip_en, int mirror_en)
{
	unsigned int cmd = 0;

	isp_dbg("%s enter.\n", __func__);

	cmd = isp->reg_blkaddr5[32];
	CLEAR_BITS(cmd, HFILP_EN, 2);
	cmd |= ((flip_en ? 1:0) << VFILP_EN)|
		((mirror_en ? 1:0) << HFILP_EN);
	isp->reg_blkaddr5[32] = cmd;
	//printk("cmd:0x%x ,flip_en:%d, mirror_en:%d, enter.\n", cmd, flip_en, mirror_en);

	isp->misc_para.flip_en = flip_en;
	isp->misc_para.mirror_en = mirror_en;
	return 0;
}

int ak_isp_vo_update_regtable( enum  reg_updata_mode  mode)
{
	if (isp->cb.cb_cache_invalid)
		isp->cb.cb_cache_invalid();

    switch(mode)
    {
    case MODE_LEAST:
         REG32(isp->base + 0x1c) = 0x1<<2 ;
         break;

	/* һ���ڳ��������仯ʱ, ���ø�λΪ1, ����ISP�������ʺϱ仯��ĳ��� */
    case MODE_MOST:
         REG32(isp->base + 0x1c) = 0x1<<1;
         break;
	/*
	���¼���������ø�λΪ1:MODE_ALL
	1, ISP��ʼ��
	2, ���Ĵ�����д��ȷ
	*/
    case MODE_ALL:
         REG32(isp->base + 0x1c) = 0x1<<0 ;
         break;
         
    default:
        printk("error updata reg table mode\n");
        break;      
   }
   
   return 0;    
}

int ak_isp_vo_update_setting(void)
{
	if(!isp)
		return 0;
	
	isp->reg_update_flag = MODE_ALL;
	return 0;
}

int ak_isp_vo_update_setting_most(void)
{
	if(!isp)
		return 0;
	
	isp->reg_update_flag = MODE_MOST;
	return 0;
}

int  _isp_enable_buffer(void)
{
	unsigned long cmd = 0;

	cmd = REG32(isp->base + 0x00);
	if(isp->enable_buffer_list&0x1)
	{
		SET_BITS(cmd, 15, 1);
		SET_BITS(cmd, 21, 1);
	}

	if(isp->enable_buffer_list&0x2)
	{
		SET_BITS(cmd, 16, 1);
		SET_BITS(cmd, 22, 1);
	}

	if(isp->enable_buffer_list&0x4)
	{
		SET_BITS(cmd, 17, 1);
		SET_BITS(cmd, 23, 1);
	}

	if(isp->enable_buffer_list&0x8)
	{
		SET_BITS(cmd, 18, 1);
		SET_BITS(cmd, 24, 1);
	}

	REG32(isp->base + 0x00) = cmd;
	
	return 0;
}

int  ak_isp_vo_enable_buffer(enum buffer_id  id)
{    
	if(!isp)
		return 0;
	
    switch(id)
    {
    case BUFFER_ONE:
        isp->enable_buffer_list |=0x1;
        break;
    case BUFFER_TWO:
        isp->enable_buffer_list |=0x2;
        break;
    case BUFFER_THREE:
        isp->enable_buffer_list |=0x4;
        break;
    case BUFFER_FOUR:
        isp->enable_buffer_list |=0x8;
        break;
    default:
        break;
    }
    isp_dbg("%s :id=%d, list=0x%x\n", __func__, id, isp->enable_buffer_list);

    return 0;
}

int  ak_isp_vo_disable_buffer(enum buffer_id  id)
{
    unsigned long cmd = 0;

    if(!isp)
	return 0;
    
    isp_dbg("%s enter:id=%d, reg0=0x%x\n", __func__, id, REG32(isp->base + 0x00));
    
    switch(id){
    case BUFFER_ONE:
        cmd = REG32(isp->base + 0x00);
        CLEAR_BITS(cmd, 15, 1);
        CLEAR_BITS(cmd, 21, 1);
        REG32(isp->base + 0x00) = cmd;
        CLEAR_BITS(isp->enable_buffer_list, 0, 1);
        break;
    case BUFFER_TWO:
        cmd = REG32(isp->base + 0x00);
        CLEAR_BITS(cmd, 16, 1);
        CLEAR_BITS(cmd, 22, 1);
        REG32(isp->base + 0x00) = cmd;
        CLEAR_BITS(isp->enable_buffer_list, 1, 1);
        break;
    case BUFFER_THREE:
        cmd = REG32(isp->base + 0x00);
        CLEAR_BITS(cmd, 17, 1);
        CLEAR_BITS(cmd, 23, 1);
        REG32(isp->base + 0x00) = cmd;
        CLEAR_BITS(isp->enable_buffer_list, 2, 1);
        break;
    case BUFFER_FOUR:
        cmd = REG32(isp->base + 0x00);
        CLEAR_BITS(cmd, 18, 1);
        CLEAR_BITS(cmd, 24, 1);
        REG32(isp->base + 0x00) = cmd;
        CLEAR_BITS(isp->enable_buffer_list, 3, 1);
        break;
    default:
        break;
    }
    
    return 0;
}

int ak_isp_vo_set_buffer_addr(enum buffer_id  id,unsigned long yaddr_main_chan_addr, unsigned long yaddr_sub_chan_addr)//����??buffer��??��
{
	isp_dbg("%s:id=%d, main_adr=%x, sub_addr=%x\n", __func__, id, yaddr_main_chan_addr,  yaddr_sub_chan_addr);
        
	switch(id)
	{
    case BUFFER_ONE:
        //main chan
        isp->reg_blkaddr5[25] = yaddr_main_chan_addr;   //y1
        isp->reg_blkaddr5[26] = yaddr_main_chan_addr+isp->ch1_height*isp->ch1_width; //u1 
        isp->reg_blkaddr5[27] = yaddr_main_chan_addr+isp->ch1_height*isp->ch1_width*5/4 ;  //v1 
        //sub chan
        isp->reg_blkaddr5[28] = yaddr_sub_chan_addr;   //sub_y1
        isp->reg_blkaddr5[29] = yaddr_sub_chan_addr+isp->ch2_height*isp->ch2_width; //sub_u1 
        isp->reg_blkaddr5[30] = yaddr_sub_chan_addr+isp->ch2_height*isp->ch2_width*5/4 ;  //sub_v1 
        break;
    case BUFFER_TWO:
        isp->reg_blkaddr1[31] = yaddr_main_chan_addr;   //y2
        isp->reg_blkaddr1[34] = yaddr_main_chan_addr+isp->ch1_height*isp->ch1_width; //u2 
        isp->reg_blkaddr1[37] = yaddr_main_chan_addr+isp->ch1_height*isp->ch1_width*5/4 ;  //v2  

        isp->reg_blkaddr1[40] = yaddr_sub_chan_addr;   //sy2
        isp->reg_blkaddr1[43] = yaddr_sub_chan_addr+isp->ch2_height*isp->ch2_width; //su2
        isp->reg_blkaddr1[46] = yaddr_sub_chan_addr+isp->ch2_height*isp->ch2_width*5/4 ;  //sv2
        break;
    case BUFFER_THREE:
        isp->reg_blkaddr1[32] = yaddr_main_chan_addr;   //y3
        isp->reg_blkaddr1[35] = yaddr_main_chan_addr+isp->ch1_height*isp->ch1_width; //u3 
        isp->reg_blkaddr1[38] = yaddr_main_chan_addr+isp->ch1_height*isp->ch1_width*5/4 ;  //v3 

        isp->reg_blkaddr1[41] = yaddr_sub_chan_addr;   //sy3
        isp->reg_blkaddr1[44] = yaddr_sub_chan_addr+isp->ch2_height*isp->ch2_width; //su3 
        isp->reg_blkaddr1[47] = yaddr_sub_chan_addr+isp->ch2_height*isp->ch2_width*5/4 ;  //sv3
        break;
    case BUFFER_FOUR:
        isp->reg_blkaddr1[33] = yaddr_main_chan_addr;   //y4
        isp->reg_blkaddr1[36] = yaddr_main_chan_addr+isp->ch1_height*isp->ch1_width; //u4 
        isp->reg_blkaddr1[39] = yaddr_main_chan_addr+isp->ch1_height*isp->ch1_width*5/4 ;  //v4

        isp->reg_blkaddr1[42] = yaddr_sub_chan_addr;   //sy4
        isp->reg_blkaddr1[45] = yaddr_sub_chan_addr+isp->ch2_height*isp->ch2_width; //su4
        isp->reg_blkaddr1[48] = yaddr_sub_chan_addr+isp->ch2_height*isp->ch2_width*5/4 ;  //sv4
        break;
    default:
        break;
    }

  return 0;
}

int _isp_vo_set_stat_addr(enum buffer_id  id,unsigned long  stat_phy_addr, void *stat_virtual_addr)
{
	switch(id)
	{
	case BUFFER_ONE:
		isp->reg_blkaddr5[31] = stat_phy_addr;    //y1
		isp->stat_addr[0] = (T_U32*)stat_virtual_addr;
		break;
	case BUFFER_TWO:
		isp->reg_blkaddr1[49] = stat_phy_addr;   //y2
		isp->stat_addr[1] = (T_U32*)stat_virtual_addr;
		break;
	case BUFFER_THREE:
		isp->reg_blkaddr1[50] = stat_phy_addr;   //y3
		isp->stat_addr[2] = (T_U32*)stat_virtual_addr;
		break;
	case BUFFER_FOUR:
		isp->reg_blkaddr1[51] = stat_phy_addr;   //y4
		isp->stat_addr[3] = (T_U32*)stat_virtual_addr;
		break;
	default:
		break;
    }
    
    return 0;
}

int ak_isp_vo_get_using_frame_buf_id(void)
{
	int cmd, cnt,i;
	
	if(!isp)
		return 0;

	cmd = REG32(isp->base + 0x00);
	cmd = (cmd>>15)&0x0f;
	cnt = 0;
	for(i=0; i<4; i++)
	{
		if(cmd&0x1)
			cnt++;
		cmd = cmd>>1;
	}
	
	if(cnt>1)
	{		
		if(isp->ae_drop_flag == 1)
		{
			//�ع��쳣����һ֡Ҳ��Ҫ����������WDR��Ӱ��
			isp->ae_drop_flag = 0;
			return isp->current_buffer|0x80;	//add flag, inform uplayer drop this frame
		}
		else if(isp->ae_drop_count>0)
		{
			unsigned long *stat_addr;
			unsigned long pixel_total = 0;
			int i;
			int curr_lumi;
			
			stat_addr = isp->stat_addr[isp->current_buffer];
       		for(i=0;i<256;i++)
			{
    			pixel_total += stat_addr[i];
			}

			curr_lumi = stat_addr[256]/pixel_total;
			if(abs(isp->ae_last_lumi-curr_lumi)*5<isp->ae_last_lumi)
			{
				isp->ae_last_lumi = curr_lumi;
				return isp->current_buffer;
			}
			else
			{
				isp->ae_drop_flag = 1;		//�ع��쳣����һ֡Ҳ��Ҫ����������WDR��Ӱ��
				return isp->current_buffer|0x80;	//add flag, inform uplayer drop this frame
			}
		}
		else
			return isp->current_buffer;
	}
	else
		return -1;
}

int ak_isp_vo_get_using_frame_buf_hwid(void)
{
	int frame_id;
	if(!isp)
		return 0;

	frame_id = REG32(isp->base + (STATUS_REG_ADDR-ISP_BASE_ADDR));
	frame_id = (frame_id>>MAIN_FRAME_BUF_ID)&0x03;
	isp->current_buffer = frame_id;

	return frame_id;
}

int ak_isp_is_continuous(void)
{
    enum isp_working_mode video_mode;
    
    if(!isp)
        return 0;

    video_mode = isp->cur_mode;
    if(video_mode == ISP_YUV_VIDEO_OUT
        ||video_mode == ISP_RGB_VIDEO_OUT)
        return 1;
    else
        return 0;
}

int _isp_set_pp_frame_ctrl(int ch1_frame_ctrl, int ch2_frame_ctrl)
{
	unsigned int cmd;
	
	cmd = 0;
	cmd = isp->reg_blkaddr4[34];
	CLEAR_BITS(cmd, 0, 16);
	cmd |= ((LOW_BITS(ch1_frame_ctrl, 8) << PP_FRAME1_CTRL)
		|  (LOW_BITS(ch2_frame_ctrl, 8) << PP_FRAME2_CTRL));
	isp->reg_blkaddr4[34] = cmd;
	
	return 0;
}


AK_ISP_AE_INFO isp_sensor_ae_info = {
	.exp_time = 0,
};
void  ak_isp_ae_info(AK_ISP_AE_INFO *isp_ae_info)
{		

	AK_ISP_AE_INFO *isp_ae_info_tmp = isp_ae_info;
	//if(isp->ae_para.a_gain_min < isp_ae_info_tmp->a_gain &&  isp_ae_info_tmp->a_gain < isp->ae_para.a_gain_max)
		isp_sensor_ae_info.a_gain      = isp_ae_info_tmp->a_gain;

	//if(isp->ae_para.d_gain_min < isp_ae_info_tmp->d_gain &&  isp_ae_info_tmp->d_gain < isp->ae_para.d_gain_max)	
		isp_sensor_ae_info.d_gain      = isp_ae_info_tmp->d_gain;

	//if(isp->ae_para.exp_time_min <= isp_ae_info_tmp->exp_time &&  isp_ae_info_tmp->exp_time <= isp->ae_para.exp_time_max)
		isp_sensor_ae_info.exp_time    = isp_ae_info_tmp->exp_time;

	//if(isp->ae_para.isp_d_gain_min < isp_ae_info_tmp->isp_d_gain &&  isp_ae_info_tmp->isp_d_gain < isp->ae_para.isp_d_gain_max)
		isp_sensor_ae_info.isp_d_gain  = isp_ae_info_tmp->isp_d_gain;

#if 0
	if (!isp->sensor_cb.sensor_init_exp_func)
		isp->sensor_cb.sensor_init_exp_func(isp_ae_info_tmp->exp_time,
			isp_ae_info_tmp->a_gain, isp_ae_info_tmp->d_gain);
#endif
}

static int isp2_reg_init()
{
	AK_ISP_WB_GAIN wb_gain;
	unsigned long cmd;

	cmd = 0;
	cmd = isp->reg_blkaddr5[32];
	CLEAR_BITS(cmd, SDB3_UPLOAD_EN, 1);
	CLEAR_BITS(cmd, SDB4_UPLOAD_EN, 1);
	CLEAR_BITS(cmd, SDB5_UPLOAD_EN, 1);

	SET_BITS(cmd, SDB3_UPLOAD_EN, 1);
	SET_BITS(cmd, SDB4_UPLOAD_EN, 1);
	SET_BITS(cmd, SDB5_UPLOAD_EN, 1);
    isp->reg_blkaddr5[32] = cmd;

	//general & timing setting
	_isp_set_misc_attr();
	ak_isp_vo_enable_irq_status(0x1);
	isp->reg_blkaddr2[51] = 0x40;   //RGB2YUV and Contrast Para
	_isp_set_pp_frame_ctrl(0xff,0xff);

	_isp_vo_set_stat_addr(0, isp->stat_handle, isp->stat_area);
	_isp_vo_set_stat_addr(1, isp->stat_handle, isp->stat_area);
	_isp_vo_set_stat_addr(2, isp->stat_handle, isp->stat_area);
	_isp_vo_set_stat_addr(3, isp->stat_handle, isp->stat_area);
    	
	//ae run info
	if(0 != isp_sensor_ae_info.exp_time )
	{
	    isp->ae_run_info_para.current_exp_time = isp_sensor_ae_info.exp_time;
		_cmos_updata_exp_time(isp->ae_run_info_para.current_exp_time);
		isp->ae_run_info_para.current_a_gain = isp_sensor_ae_info.a_gain  ;
		 _cmos_updata_a_gain(isp->ae_run_info_para.current_a_gain);
		isp->ae_run_info_para.current_d_gain = isp_sensor_ae_info.d_gain  ;
		_cmos_updata_d_gain(isp->ae_run_info_para.current_d_gain);
		isp->ae_run_info_para.current_isp_d_gain = isp_sensor_ae_info.isp_d_gain;

	}
	else
	{	   
    	isp->ae_run_info_para.current_exp_time = isp->ae_para.exp_time_max;
		_cmos_updata_exp_time(isp->ae_run_info_para.current_exp_time);
		isp->ae_run_info_para.current_a_gain = isp->ae_para.a_gain_min;
		_cmos_updata_a_gain(isp->ae_run_info_para.current_a_gain);
		isp->ae_run_info_para.current_d_gain = isp->ae_para.d_gain_min;
		_cmos_updata_d_gain(isp->ae_run_info_para.current_d_gain);
		isp->ae_run_info_para.current_isp_d_gain = isp->ae_para.isp_d_gain_min;
	}
	//set isp d gain

	/* FIXME */
	_isp_para_change_with_envi(ENVI_OUTDOOR); 
	_isp_ccm_change_with_envi(ISP2_COLORTEMP_MODE_D65);
	_set_ccm_attr(&(isp->ccm_para.ccm[ISP2_COLORTEMP_MODE_D65]));
	_isp_hue_change_with_envi(ISP2_COLORTEMP_MODE_D65);
	_set_hue_attr(&(isp->hue_para.hue[ISP2_COLORTEMP_MODE_D65]));
		
	if(WB_OPS_TYPE_MANU==isp->wb_type_para.wb_type )
	{
	       wb_gain.r_gain = isp->mwb_para.r_gain;
	       wb_gain.g_gain = isp->mwb_para.g_gain;
	       wb_gain.b_gain = isp->mwb_para.b_gain;
	       wb_gain.r_offset = isp->mwb_para.r_offset;
	       wb_gain.g_offset = isp->mwb_para.g_offset;
	       wb_gain.b_offset = isp->mwb_para.b_offset;
	       ak_isp_updata_wb_gain(&wb_gain);
	}
	else
	{
		//isp->awb_algo.target_r_gain = isp->awb_algo.current_r_gain = wb_gain.r_gain = 255;
		//isp->awb_algo.target_g_gain = isp->awb_algo.current_g_gain = wb_gain.g_gain = 255;
		//isp->awb_algo.target_b_gain = isp->awb_algo.current_b_gain = wb_gain.b_gain = 255;
		isp->awb_stat_info_para.current_colortemp_index=ISP2_COLORTEMP_MODE_D65;
		isp->awb_algo.current_r_gain = isp->awb_stat_info_para.r_gain =  isp->awb_algo.target_r_gain = isp->mwb_para.r_gain;
		isp->awb_algo.current_g_gain = isp->awb_stat_info_para.g_gain = isp->awb_algo.target_g_gain = isp->mwb_para.g_gain;
		isp->awb_algo.current_b_gain = isp->awb_stat_info_para.b_gain = isp->awb_algo.target_b_gain = isp->mwb_para.b_gain;
		
		wb_gain.r_gain = isp->mwb_para.r_gain;
	    wb_gain.g_gain = isp->mwb_para.g_gain;
	    wb_gain.b_gain = isp->mwb_para.b_gain;

		wb_gain.r_offset = 0;
		wb_gain.g_offset = 0;
		wb_gain.b_offset = 0;
		ak_isp_updata_wb_gain(&wb_gain);
		_set_awb_attr(&(isp->awb_para));
	}

	return 0;
}

int ak_isp_vi_start_capturing(void)
{
    unsigned long peri_status, cmd;
    isp_dbg("%s enter.\n", __func__);

    isp->current_buffer = -1;
    _isp_enable_buffer();
    cmd = REG32(isp->base + 0x00);
	cmd = (cmd>>15)&0x0f;
	isp->next_buffer= 0;
	while(((1<<isp->next_buffer)&cmd)==0)
	{
		isp->next_buffer = (isp->next_buffer+1)%4;
	}

    //set up DMA table address
    REG32(isp->base+DMA_REG_ADDR) = isp->handle;
	cmd = REG32(isp->base+DMA_SDPC_ADDR);
	CLEAR_BITS(cmd, 0, 31);
	cmd |= LOW_BITS(isp->sdpc_handle, 30);
	REG32(isp->base+DMA_SDPC_ADDR) = cmd;

    isp2_reg_init();
    ak_isp_vo_update_regtable(MODE_ALL);
    //isp2_print_reg_table();
    
    /* start capturing bit */
    peri_status = REG32(isp->base + GEN_CTRL_ADDR); //add by xc
    SET_BITS(peri_status, 20, 1);
    REG32(isp->base + GEN_CTRL_ADDR) = peri_status;

    return 0;
}

int ak_isp_vi_stop_capturing(void)
{
    unsigned long peri_status;

    if (!isp)
        return -1;
    
    isp_dbg("%s enter.\n", __func__);
	int maxdelay = 200;
	while ((REG32(isp->base + 0x10) & (1 << 14))) {
		if (maxdelay > 0) {
			maxdelay--;
			isp->cb.cb_msleep(1);
		} else {
			isp->cb.cb_printk("[%s] reg10 b14 not clear\n", __func__);
			break;
		}
	}
	
    peri_status = REG32(isp->base + GEN_CTRL_ADDR); //add by xc
    CLEAR_BITS(peri_status, 20, 1);
    //peri_status |= (1 << 19); //������λ�������ڴ����ң�ϵͳ���в�����
    REG32(isp->base + GEN_CTRL_ADDR) = peri_status;

    return 0;   
}

/**
 * @brief: set wdr other param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_wdr_ex: wdr other param
 */
int _isp_set_wdr_ex_attr(AK_ISP_WDR_EX_ATTR *p_wdr_ex)
{
	unsigned long cmd = 0;
    isp_dbg("%s enter.\n", __func__);
    memcpy(&(isp->wdr_ex_para),p_wdr_ex, sizeof(AK_ISP_WDR_EX_ATTR));

	//yuv_reg_4
	cmd = 0;
	cmd = isp->reg_blkaddr3[3];
	CLEAR_BITS(cmd, 8, 24);
	cmd |= (LOW_BITS(p_wdr_ex->hdr_reverseW_g, 9) << HDR_REVERSEW_G) 
		|  (LOW_BITS(p_wdr_ex->hdr_reverseW_shift, 3) << HDR_REVERSEW_SHIFT)
		|  (LOW_BITS(p_wdr_ex->hdr_reverseH_g, 9) << HDR_REVERSEH_G)
		|  (LOW_BITS(p_wdr_ex->hdr_reverseH_shift, 3) << HDR_REVERSEH_SHIFT);
	isp->reg_blkaddr3[3] = cmd;
	 
	//yuv_reg_5
	cmd = 0;
	cmd = isp->reg_blkaddr3[4];
	CLEAR_BITS(cmd, 0, 12);
	cmd |= (LOW_BITS(p_wdr_ex->hdr_weight_g, 9) << HDR_WEIGHT_G) 
		|  (LOW_BITS(p_wdr_ex->hdr_weight_shift, 3) << HDR_WEIGHT_SHIFT);
	isp->reg_blkaddr3[4] = cmd;

	//yuv_reg_6
	cmd = 0;
	cmd = isp->reg_blkaddr3[5];
	CLEAR_BITS(cmd, HDR_WEIGHT_K, 4);
	cmd |= (LOW_BITS(p_wdr_ex->hdr_weight_k, 4) << HDR_WEIGHT_K);
	isp->reg_blkaddr3[5] = cmd;
	
	ak_isp_vo_update_setting();
	
    return 0;
}

int ak_isp_vi_set_crop(int sx, int sy, int width, int height)
{
    unsigned long cmd = 0;
    AK_ISP_WDR_EX_ATTR wdr_ex;
    int tnr_revW, tnr_revH;
    int i, g;
    
    isp_dbg("%s enter. (%d,%d,%d,%d)\n", __func__,  sx, sy, width,  height);

    //cut_reg_1
    cmd = (LOW_BITS(sx, 13) << CUT_STR_XPOS) 
    	| (LOW_BITS(sy, 13) << CUT_STR_YPOS);
    //*pReg++ = cmd;
    isp->reg_blkaddr1[0] = cmd;

    //cut_reg_2
    cmd = 0;
    cmd = isp->reg_blkaddr1[1];
    CLEAR_BITS(cmd, 0, 28);
    cmd = (LOW_BITS(width, 13) << CUT_WIN_WIDTH)
    	| (LOW_BITS(height, 13) << CUT_WIN_HEIGHT);
    //(IspC->sub_sample << DOWNSAMPLE);
    //*pReg++ = cmd;
    isp->reg_blkaddr1[1] = cmd;

    isp->cut_width = width;
    isp->cut_height= height;

    wdr_ex.hdr_blkW= width/16;
    wdr_ex.hdr_blkH = height/8;
    for(i=0; i<8; i++)
    {
		g = (1<<(i+1+8))/wdr_ex.hdr_blkW;
		if(g>511)
			break;
    }
    wdr_ex.hdr_reverseW_shift = i;
    wdr_ex.hdr_reverseW_g = (1<<(i+8))/wdr_ex.hdr_blkW;
    for(i=0; i<8; i++)
    {
		g = (1<<(i+1+8))/wdr_ex.hdr_blkH;
		if(g>511)
			break;
    }
    wdr_ex.hdr_reverseH_shift = i;
    wdr_ex.hdr_reverseH_g = (1<<(i+8))/wdr_ex.hdr_blkH;

    for(i=0; i<8; i++)
    {
		g = (wdr_ex.hdr_blkW*wdr_ex.hdr_blkH)>>i;
		if(g<256)
			break;
    }
    wdr_ex.hdr_weight_shift= i;
    wdr_ex.hdr_weight_g= 65536/((wdr_ex.hdr_blkW*wdr_ex.hdr_blkH)>>i);
    wdr_ex.hdr_weight_k = 8;
    _isp_set_wdr_ex_attr(&wdr_ex);

	//set tnr motion stat max & revW & revH    
    tnr_revW = 4096/(width/32);
    tnr_revH = 4096/(height/16);
    cmd = 0;
    cmd = (0xffff) 
    	| (LOW_BITS(tnr_revW, 8) << TNR_REVW)
    	| (LOW_BITS(tnr_revH, 8) << TNR_REVH);
    isp->reg_blkaddr3[21] = cmd;

    return 0;
}

int ak_isp_vi_apply_mode(enum isp_working_mode mode)
{
    unsigned long peri_status;
    
    isp_dbg("%s enter. %d\n", __func__, mode);

    isp->cur_mode = mode;
    switch(isp->cur_mode) 
    {
    case ISP_RGB_VIDEO_OUT:
        peri_status = REG32(isp->base + GEN_CTRL_ADDR); //add by xc
	    CLEAR_BITS(peri_status, 2, 2);
        CLEAR_BITS(peri_status, 19, 1);
        SET_BITS(peri_status, 13, 1);
        CLEAR_BITS(peri_status, 9, 1);
        CLEAR_BITS(peri_status, 8, 1);
        REG32(isp->base + GEN_CTRL_ADDR) = peri_status;
        break;
        
    case ISP_RGB_OUT:
        peri_status = REG32(isp->base + GEN_CTRL_ADDR);
        CLEAR_BITS(peri_status, 2, 2);
        CLEAR_BITS(peri_status, 19, 1);
        CLEAR_BITS(peri_status, 13, 1);
        CLEAR_BITS(peri_status, 9, 1);
        CLEAR_BITS(peri_status, 8, 1);
        REG32(isp->base + GEN_CTRL_ADDR) = peri_status;
        break;
        
    case ISP_YUV_OUT:
        peri_status = REG32(isp->base + GEN_CTRL_ADDR);
        CLEAR_BITS(peri_status, 2, 2);
    	SET_BITS(peri_status, 2, 1);
        CLEAR_BITS(peri_status, 19, 1);
        CLEAR_BITS(peri_status, 13, 1);
        SET_BITS(peri_status, 9, 1);
        CLEAR_BITS(peri_status, 8, 1);
        REG32(isp->base + GEN_CTRL_ADDR) = peri_status;
		ak_isp_vi_set_sub128(1);
        break;
        
    case ISP_YUV_VIDEO_OUT:
        peri_status = REG32(isp->base + GEN_CTRL_ADDR);
        CLEAR_BITS(peri_status, 2, 2);
    	SET_BITS(peri_status, 2, 1);
        CLEAR_BITS(peri_status, 19, 1);
        SET_BITS(peri_status, 13, 1);
        SET_BITS(peri_status, 9, 1);
        CLEAR_BITS(peri_status, 8, 1);
        REG32(isp->base + GEN_CTRL_ADDR) = peri_status;
		ak_isp_vi_set_sub128(1);
        break;
    
    case ISP_JPEG_MODE:
        peri_status = REG32(isp->base + GEN_CTRL_ADDR);
        CLEAR_BITS(peri_status, 2, 2);
    	SET_BITS(peri_status, 3, 1);
       	CLEAR_BITS(peri_status, 19, 1);
        CLEAR_BITS(peri_status, 13, 1);
        SET_BITS(peri_status, 8, 1);
        REG32(isp->base + GEN_CTRL_ADDR) = peri_status;
        break;  
        
	default:
        //REG32(isp->base + GEN_CTRL_ADDR) |= (3 << 30);
        printk("error isp work mode\n");
    }

    return 0;
}

int ak_isp_vo_set_main_channel_scale(int width, int height)
{
    unsigned int cmd = 0;
    unsigned int sample;
    
    isp_dbg("%s enter.(%d,%d,%d,%d,%d)\n", __func__, width, height, isp->sub_sample, isp->cut_width, isp->cut_height);
    sample = 0x1 << (T_U8)isp->sub_sample;

    isp->ch1_width = width;
    isp->ch1_height = height;
    //int iWidth = ((isp->cut_width/2)/sample + (((isp->cut_width/2)%sample)?1:0))*2;
    int iHeight = ((isp->cut_height/2)/sample + (((isp->cut_height/2)%sample)?1:0))*2;

    if((isp->ch1_width-1)>2*(width-1))
    {
        isp->chl1_xrate  = 65536/isp->ch1_width;
    }
    else
    {
        isp->chl1_xrate  = 65536/(isp->ch1_width -1);
    }
    if((isp->ch1_height-1)>2*(iHeight-1))
    {
        isp->chl1_yrate  = 65536/isp->ch1_height;
    }
    else
    {
        isp->chl1_yrate  = 65536/(isp->ch1_height -1);
    }

    //pp_reg_31
    cmd = 0;
    cmd = (LOW_BITS(isp->chl1_xrate, 12) << PP_WIDTH1_PARA) 
    	| (LOW_BITS(isp->chl1_yrate, 12) << PP_HEIGHT1_PARA);
    //*pReg++ = cmd;
    isp->reg_blkaddr4[30] = cmd;;

    //pp_reg_32
    cmd = 0;
    cmd = (LOW_BITS(isp->ch1_width, 12) << PP_DST1_WIDTH) 
    	| (LOW_BITS(isp->ch1_height, 12) << pp_DST1_HEIGHT);
    //*pReg++ = cmd;
    isp->reg_blkaddr4[31] = cmd;

    return 0;   
}

int ak_isp_vo_get_main_channel_scale(int *width, int *height)
{
    isp_dbg("%s enter.\n", __func__);  
    *width = isp->ch1_width;
    *height = isp->ch1_height;
    
    return 0;   
}

int ak_isp_vo_set_sub_channel_scale(int width, int height)
{
    unsigned int cmd = 0;
    
    isp_dbg("%s enter.\n", __func__);
    isp->ch2_width = width;
    isp->ch2_height = height;
	//unsigned int sample = 0x1 << (T_U8)isp->sub_sample;
    //int iWidth = ((isp->cut_width/2)/sample + (((isp->cut_width/2)%sample)?1:0))*2;
    //int iHeight = ((isp->cut_height/2)/sample + (((isp->cut_height/2)%sample)?1:0))*2;

    isp->chl2_yrate  = (65536+isp->ch2_height /2)/(isp->ch2_height -1);
    isp->chl2_xrate = 65536/(isp->ch2_width-1);

    //pp_reg_33
    cmd = 0;
    cmd = (LOW_BITS(isp->chl2_xrate, 12) << PP_WIDTH2_PARA) 
    	| (LOW_BITS(isp->chl2_yrate, 12) << PP_HEIGHT2_PARA);
    //*pReg++ = cmd;
    isp->reg_blkaddr4[32] = cmd;
    
    //pp_reg_34
    cmd = 0;
    cmd = (LOW_BITS(isp->ch2_width, 12) << PP_DST2_WIDTH) 
    	| (LOW_BITS(isp->ch2_height, 12) << pp_DST2_HEIGHT);
    isp->reg_blkaddr4[33] = cmd;

    return 0;
}

int ak_isp_vo_get_sub_channel_scale(int *width, int *height)
{
    isp_dbg("%s enter.\n", __func__);
     *width = isp->ch2_width;
    *height = isp->ch2_height;

    return 0;
}

/**
 * @brief: set blc param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_blc:  blc param
 */
/************************************************************************************************/
int ak_isp_vp_set_blc_attr(AK_ISP_BLC_ATTR *p_blc)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(&(isp->blc_para),p_blc,sizeof(AK_ISP_BLC_ATTR));
    printk("isp_>blc_mode=%d",isp->blc_para.blc_mode);
	//ak_isp_vo_update_setting();
	isp->linkage_para_update_flag =1;

    return 0;
}

/**
 * @brief: get blc param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_blc:  blc param
 */

int ak_isp_vp_get_blc_attr(AK_ISP_BLC_ATTR *p_blc)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(p_blc,&(isp->blc_para),sizeof(AK_ISP_BLC_ATTR));

   	return 0;
}

/**
 * @brief: set lsc param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_lsc:  lsc param
 */
int ak_isp_vp_set_lsc_attr(AK_ISP_LSC_ATTR *p_lsc)
{
	T_U32 cmd;
    isp_dbg("%s enter.\n", __func__);
    
    memcpy(&(isp->lsc_para),p_lsc,sizeof(AK_ISP_LSC_ATTR));
    ////lenc_reg_1
    cmd = 0;
    cmd = (LOW_BITS(p_lsc->lsc_r_coef.coef_b[0], 8) << LENS_COEF0_BB_R)
    	| (LOW_BITS(p_lsc->lsc_gr_coef.coef_b[0], 8) << LENS_COEF0_BB_GR)
    	| (LOW_BITS(p_lsc->lsc_gb_coef.coef_b[0], 8) << LENS_COEF0_BB_GB)
    	| (LOW_BITS(p_lsc->lsc_b_coef.coef_b[0], 8) << LENS_COEF0_BB_B);
    isp->reg_blkaddr1[2] = cmd;

    //lenc_reg_2
    cmd = 0;
    cmd = (LOW_BITS(p_lsc->lsc_r_coef.coef_b[1], 8) << LENS_COEF1_BB_R)
    	| (LOW_BITS(p_lsc->lsc_gr_coef.coef_b[1], 8) << LENS_COEF1_BB_GR)
        | (LOW_BITS(p_lsc->lsc_gb_coef.coef_b[1], 8) << LENS_COEF1_BB_GB)
        | (LOW_BITS(p_lsc->lsc_b_coef.coef_b[1], 8) << LENS_COEF1_BB_B);
    isp->reg_blkaddr1[3] = cmd;

    //lenc_reg_3
    cmd = 0;
    cmd = (LOW_BITS(p_lsc->lsc_r_coef.coef_b[2], 8) << LENS_COEF2_BB_R)
    	| (LOW_BITS(p_lsc->lsc_gr_coef.coef_b[2], 8) << LENS_COEF2_BB_GR) 
    	| (LOW_BITS(p_lsc->lsc_gb_coef.coef_b[2], 8) << LENS_COEF2_BB_GB) 
    	| (LOW_BITS(p_lsc->lsc_b_coef.coef_b[2], 8) << LENS_COEF2_BB_B);
    
    isp->reg_blkaddr1[4] = cmd;

    //lenc_reg_4
    cmd = 0;
    cmd = (LOW_BITS(p_lsc->lsc_r_coef.coef_b[3], 8) << LENS_COEF3_BB_R)
    	| (LOW_BITS(p_lsc->lsc_gr_coef.coef_b[3], 8) << LENS_COEF3_BB_GR)
    	| (LOW_BITS(p_lsc->lsc_gb_coef.coef_b[3], 8) << LENS_COEF3_BB_GB)
    	| (LOW_BITS(p_lsc->lsc_b_coef.coef_b[3], 8) << LENS_COEF3_BB_B);
   	isp->reg_blkaddr1[5] = cmd;

   	//lenc_reg_5
    cmd = 0;
    cmd = (LOW_BITS(p_lsc->lsc_r_coef.coef_b[4], 8) << LENS_COEF4_BB_R)
    	| (LOW_BITS(p_lsc->lsc_gr_coef.coef_b[4], 8) << LENS_COEF4_BB_GR)
    	| (LOW_BITS(p_lsc->lsc_gb_coef.coef_b[4], 8) << LENS_COEF4_BB_GB)
    	| (LOW_BITS(p_lsc->lsc_b_coef.coef_b[4], 8) << LENS_COEF4_BB_B);
   	isp->reg_blkaddr1[6] = cmd;

   	//lenc_reg_6
    cmd = 0;
    cmd = (LOW_BITS(p_lsc->lsc_r_coef.coef_b[5], 8) << LENS_COEF5_BB_R)
    	| (LOW_BITS(p_lsc->lsc_gr_coef.coef_b[5], 8) << LENS_COEF5_BB_GR)
        | (LOW_BITS(p_lsc->lsc_gb_coef.coef_b[5], 8) << LENS_COEF5_BB_GB)
        | (LOW_BITS(p_lsc->lsc_b_coef.coef_b[5], 8) << LENS_COEF5_BB_B);
    isp->reg_blkaddr1[7] = cmd;

    //lenc_reg_7
    cmd = 0;
    cmd = (LOW_BITS(p_lsc->lsc_r_coef.coef_b[6], 8) << LENS_COEF6_BB_R)
    	| (LOW_BITS(p_lsc->lsc_gr_coef.coef_b[6], 8) << LENS_COEF6_BB_GR)
    	| (LOW_BITS(p_lsc->lsc_gb_coef.coef_b[6], 8) << LENS_COEF6_BB_GB)
    	| (LOW_BITS(p_lsc->lsc_b_coef.coef_b[6], 8) << LENS_COEF6_BB_B);
    isp->reg_blkaddr1[8] = cmd;

    //lenc_reg_8
    cmd = 0;
    cmd = (LOW_BITS(p_lsc->lsc_r_coef.coef_b[7], 8) << LENS_COEF7_BB_R)
    	| (LOW_BITS(p_lsc->lsc_gr_coef.coef_b[7], 8) << LENS_COEF7_BB_GR)
    	| (LOW_BITS(p_lsc->lsc_gb_coef.coef_b[7], 8) << LENS_COEF7_BB_GB)
    	| (LOW_BITS(p_lsc->lsc_b_coef.coef_b[7], 8) << LENS_COEF7_BB_B);
    isp->reg_blkaddr1[9] = cmd;

    //lenc_reg_9
    cmd = 0;
    cmd = (LOW_BITS(p_lsc->lsc_r_coef.coef_b[8], 8) << LENS_COEF8_BB_R)
    	| (LOW_BITS(p_lsc->lsc_gr_coef.coef_b[8], 8) << LENS_COEF8_BB_GR)
    	| (LOW_BITS(p_lsc->lsc_gb_coef.coef_b[8], 8) << LENS_COEF8_BB_GB)
    	| (LOW_BITS(p_lsc->lsc_b_coef.coef_b[8], 8) << LENS_COEF8_BB_B);
    isp->reg_blkaddr1[10] =cmd;

    //lenc_reg_10
    cmd = 0;
    cmd = (LOW_BITS(p_lsc->lsc_r_coef.coef_b[9], 8) << LENS_COEF9_BB_R)
    	| (LOW_BITS(p_lsc->lsc_gr_coef.coef_b[9], 8) << LENS_COEF9_BB_GR)
    	| (LOW_BITS(p_lsc->lsc_gb_coef.coef_b[9], 8) << LENS_COEF9_BB_GB)
    	| (LOW_BITS(p_lsc->lsc_b_coef.coef_b[9], 8) << LENS_COEF9_BB_B);
    isp->reg_blkaddr1[11] =cmd;

    //lenc_reg_11
    cmd = 0;
    cmd = (LOW_BITS(p_lsc->lsc_r_coef.coef_c[0], 10) << LENS_COEF0_CC_R)
    	| (LOW_BITS(p_lsc->lsc_gr_coef.coef_c[0], 10) << LENS_COEF0_CC_GR)
    	| (LOW_BITS(p_lsc->lsc_gb_coef.coef_c[0], 10) << LENS_COEF0_CC_GB);
    isp->reg_blkaddr1[12] =cmd;

    //lenc_reg_12
    cmd = 0;
    cmd = (LOW_BITS(p_lsc->lsc_b_coef.coef_c[0], 10) << LENS_COEF0_CC_B)
    	| (LOW_BITS(p_lsc->lsc_r_coef.coef_c[1], 10)<< LENS_COEF1_CC_R)
    	| (LOW_BITS(p_lsc->lsc_gr_coef.coef_c[1], 10)<< LENS_COEF1_CC_GR);
    isp->reg_blkaddr1[13] =cmd;

    //lenc_reg_13
    cmd = 0;
    cmd = (LOW_BITS(p_lsc->lsc_gb_coef.coef_c[1], 10) << LENS_COEF1_CC_GB)
    	| (LOW_BITS(p_lsc->lsc_b_coef.coef_c[1], 10) << LENS_COEF1_CC_B)
    	| (LOW_BITS(p_lsc->lsc_r_coef.coef_c[2], 10)<< LENS_COEF2_CC_R);
    isp->reg_blkaddr1[14] =cmd;
    

    //lenc_reg_14
    cmd = 0;
    cmd = (LOW_BITS(p_lsc->lsc_gr_coef.coef_c[2], 10)<< LENS_COEF2_CC_GR)
    	| (LOW_BITS(p_lsc->lsc_gb_coef.coef_c[2], 10)<< LENS_COEF2_CC_GB)
    	| (LOW_BITS(p_lsc->lsc_b_coef.coef_c[2], 10) << LENS_COEF2_CC_B);
    isp->reg_blkaddr1[15] =cmd;

    //lenc_reg_15
    cmd = 0;
    cmd = (LOW_BITS(p_lsc->lsc_r_coef.coef_c[3], 10) << LENS_COEF3_CC_R)
    	| (LOW_BITS(p_lsc->lsc_gr_coef.coef_c[3], 10)<< LENS_COEF3_CC_GR)
    	| (LOW_BITS(p_lsc->lsc_gb_coef.coef_c[3], 10)<< LENS_COEF3_CC_GB);
    isp->reg_blkaddr1[16] =cmd;

    //lenc_reg_16
    cmd = 0;
    cmd = (LOW_BITS(p_lsc->lsc_b_coef.coef_c[3], 10) << LENS_COEF3_CC_B)
    	| (LOW_BITS(p_lsc->lsc_r_coef.coef_c[4], 10)<< LENS_COEF4_CC_R)
    	| (LOW_BITS(p_lsc->lsc_gr_coef.coef_c[4], 10)<< LENS_COEF4_CC_GR);
    isp->reg_blkaddr1[17] =cmd;

    //lenc_reg_17
    cmd = 0;
    cmd = (LOW_BITS(p_lsc->lsc_gb_coef.coef_c[4] , 10)<< LENS_COEF4_CC_GB)
    	| (LOW_BITS(p_lsc->lsc_b_coef.coef_c[4], 10) << LENS_COEF4_CC_B)
    	| (LOW_BITS(p_lsc->lsc_r_coef.coef_c[5], 10) << LENS_COEF5_CC_R);
    isp->reg_blkaddr1[18] =cmd;

    //lenc_reg_18
    cmd = 0;
    cmd = (LOW_BITS(p_lsc->lsc_gr_coef.coef_c[5], 10)<< LENS_COEF5_CC_GR)
    	| (LOW_BITS(p_lsc->lsc_gb_coef.coef_c[5], 10)<< LENS_COEF5_CC_GB)
    	| (LOW_BITS(p_lsc->lsc_b_coef.coef_c[5], 10)<< LENS_COEF5_CC_B);
    isp->reg_blkaddr1[19] =cmd;

    //lenc_reg_19
    cmd = 0;
    cmd = (LOW_BITS(p_lsc->lsc_r_coef.coef_c[6], 10)<< LENS_COEF6_CC_R)
    	| (LOW_BITS(p_lsc->lsc_gr_coef.coef_c[6], 10)<< LENS_COEF6_CC_GR)
    	| (LOW_BITS(p_lsc->lsc_gb_coef.coef_c[6], 10) << LENS_COEF6_CC_GB);
    isp->reg_blkaddr1[20] =cmd;

    //lenc_reg_20
    cmd = 0;
    cmd = (LOW_BITS(p_lsc->lsc_b_coef.coef_c[6], 10)<< LENS_COEF6_CC_B)
    	| (LOW_BITS(p_lsc->lsc_r_coef.coef_c[7], 10)<< LENS_COEF7_CC_R)
    	| (LOW_BITS(p_lsc->lsc_gr_coef.coef_c[7], 10)<< LENS_COEF7_CC_GR);
    isp->reg_blkaddr1[21] =cmd;

    //lenc_reg_21
    cmd = 0;
    cmd = (LOW_BITS(p_lsc->lsc_gb_coef.coef_c[7], 10) << LENS_COEF7_CC_GB)
    	| (LOW_BITS(p_lsc->lsc_b_coef.coef_c[7], 10)<< LENS_COEF7_CC_B)
    	| (LOW_BITS(p_lsc->lsc_r_coef.coef_c[8], 10)<< LENS_COEF8_CC_R);
    isp->reg_blkaddr1[22] =cmd;

    //lenc_reg_22
    cmd = 0;
    cmd = (LOW_BITS(p_lsc->lsc_gr_coef.coef_c[8], 10) << LENS_COEF8_CC_GR)
    	| (LOW_BITS(p_lsc->lsc_gb_coef.coef_c[8], 10) << LENS_COEF8_CC_GB)
    	| (LOW_BITS(p_lsc->lsc_b_coef.coef_c[8], 10)<< LENS_COEF8_CC_B);
    isp->reg_blkaddr1[23] =cmd;

    //lenc_reg_23
    cmd = 0;
    cmd = (LOW_BITS(p_lsc->lsc_r_coef.coef_c[9], 10)<< LENS_COEF9_CC_R)
    	| (LOW_BITS(p_lsc->lsc_gr_coef.coef_c[9], 10)<< LENS_COEF9_CC_GR)
    	| (LOW_BITS(p_lsc->lsc_gb_coef.coef_c[9], 10)<< LENS_COEF9_CC_GB);
    isp->reg_blkaddr1[24] =cmd;

    //lenc_reg_24
    cmd = 0;
    cmd = (LOW_BITS(p_lsc->lsc_b_coef.coef_c[9], 10)<< LENS_COEF9_CC_B)
    	| (LOW_BITS(p_lsc->range[0], 10) << LENS_RANGE0_RR)
    	| (LOW_BITS(p_lsc->range[1], 10) << LENS_RANGE1_RR);
    isp->reg_blkaddr1[25] =cmd;

    //lenc_reg_25
    cmd = 0;
    cmd = (LOW_BITS(p_lsc->range[2], 10)  << LENS_RANGE2_RR)
    	| (LOW_BITS(p_lsc->range[3], 10) << LENS_RANGE3_RR)
    	| (LOW_BITS(p_lsc->range[4], 10) << LENS_RANGE4_RR);
    isp->reg_blkaddr1[26] =cmd;
    

    //lenc_reg_26
    cmd = 0;
    cmd = (LOW_BITS(p_lsc->range[5], 10) << LENS_RANGE5_RR)
    	| (LOW_BITS(p_lsc->range[6], 10) << LENS_RANGE6_RR)
    	| (LOW_BITS(p_lsc->range[7], 10) << LENS_RANGE7_RR);
    isp->reg_blkaddr1[27] =cmd;

    //lenc_reg_27
    cmd = 0;
    cmd = (LOW_BITS(p_lsc->range[8], 10) << LENS_RANGE8_RR)
    	| (LOW_BITS(p_lsc->range[9], 10) << LENS_RANGE9_RR);
    isp->reg_blkaddr1[28] =cmd;

    //lenc_reg_28
    cmd = 0;
    cmd = (LOW_BITS(p_lsc->xref, 12) << LENS_XREF)
    	| (LOW_BITS(p_lsc->yref, 12) << LENS_YREF);
    isp->reg_blkaddr1[29] =cmd;
    

    //lenc_reg_29
    cmd = 0;
    cmd = (LOW_BITS((p_lsc->xref*p_lsc->xref + p_lsc->yref*p_lsc->yref), 23) << LENS_1STSQR)
    	| (LOW_BITS(p_lsc->lsc_shift, 4) << LENS_LSC_SHIFT);
    isp->reg_blkaddr1[30] =cmd;

   	//enable
    cmd = 0;
    cmd = isp->reg_blkaddr5[32];
    CLEAR_BITS(cmd , LEN_ADJUST_EN, 1);
    cmd |= (LOW_BITS(p_lsc->enable, 1) << LEN_ADJUST_EN);
    isp->reg_blkaddr5[32] = cmd;
    
	ak_isp_vo_update_setting();
                    
    return 0;
}

/**
 * @brief: get lsc param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_lsc:  lsc param
 */
 int ak_isp_vp_get_lsc_attr( AK_ISP_LSC_ATTR *p_lsc)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(p_lsc,&(isp->lsc_para),sizeof(AK_ISP_LSC_ATTR));

    return 0;
}

/**
 * @brief: set rgb gamma param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_rgb_gamma:  rgb gamma param
 */
int ak_isp_vp_set_rgb_gamma_attr(AK_ISP_RGB_GAMMA_ATTR *p_rgb_gamma)
{
    int i=0,j=0;
    unsigned long cmd = 0;
    isp_dbg("%s enter.\n", __func__);
    memcpy(&(isp->rgb_gamma_para),p_rgb_gamma,sizeof(AK_ISP_RGB_GAMMA_ATTR));

    for (i=1; i<129; i++)
    {
        cmd = 0;
        cmd = (p_rgb_gamma->r_gamma[i]) |
              (p_rgb_gamma->g_gamma[i] << 10) |
              (p_rgb_gamma->b_gamma[i] << 20);
        j = i-1;
        isp->cfg_mem_blkaddr2[j] = cmd;
        j++;
    }

	ak_isp_vo_update_setting();
	
    return 0;
}

/**
 * @brief: get rgb gamma param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_rgb_gamma:  rgb gamma param
 */
int ak_isp_vp_get_rgb_gamma_attr(AK_ISP_RGB_GAMMA_ATTR *p_rgb_gamma)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(p_rgb_gamma,&(isp->rgb_gamma_para),sizeof(AK_ISP_RGB_GAMMA_ATTR));

    return 0;
}

/**
 * @brief: set y gamma param
 * @author: lz
 * @date: 2016-9-9
 * @param [in] *p_y_gamma:  y gamma param
 */
int ak_isp_vp_set_y_gamma_attr(AK_ISP_Y_GAMMA_ATTR *p_y_gamma)
{
	int i=0,j=0;
	unsigned long cmd = 0;
	isp_dbg("%s enter.\n", __func__);
	memcpy(&(isp->y_gamma_para),p_y_gamma,sizeof(AK_ISP_Y_GAMMA_ATTR));

	for (i=1; i<127; i+=3)
	{
		cmd = 0;
		cmd = (p_y_gamma->ygamma[i]) |
			  (p_y_gamma->ygamma[i+1] << 10) |
			  (p_y_gamma->ygamma[i+2] << 20);
		isp->cfg_mem_blkaddr3[j] = cmd;
		j++;
	}

	cmd = 0;
	cmd = (p_y_gamma->ygamma[127]) |
		  (p_y_gamma->ygamma[128] << 10);
	isp->cfg_mem_blkaddr3[42] = cmd;

	//yuv_reg_1
	cmd = 0;
	cmd = isp->reg_blkaddr3[0];
    CLEAR_BITS(cmd, 0, 30);
	cmd |= (LOW_BITS(p_y_gamma->ygamma_cnoise_yth1, 10) << YGAMMA_CNOISE_TH1)
		|  (LOW_BITS(p_y_gamma->ygamma_cnoise_yth2, 10) << YGAMMA_CNOISE_TH2)
		|  (LOW_BITS(p_y_gamma->ygamma_cnoise_gain, 10) << YGAMMA_CNOISE_GAIN);
	isp->reg_blkaddr3[0] = cmd;

	//yuv_reg_2
	cmd = 0;
	cmd = isp->reg_blkaddr3[1];
	CLEAR_BITS(cmd, 0, 13);
	cmd |= (LOW_BITS(p_y_gamma->ygamma_cnoise_slop, 8) << YGAMMA_CNOISE_SLOP)
		|  (LOW_BITS(p_y_gamma->ygamma_uv_adjust_level, 5) << YGAMMA_UV_ADJUST_LEVEL);
	isp->reg_blkaddr3[1] = cmd;

	cmd = 0;											
	cmd = isp->reg_blkaddr5[32];
	CLEAR_BITS(cmd, YGAMMA_UV_ADJUST_EN, 1);
	cmd |= (LOW_BITS(p_y_gamma->ygamma_uv_adjust_enable, 1) << YGAMMA_UV_ADJUST_EN);
	isp->reg_blkaddr5[32] = cmd;

	ak_isp_vo_update_setting();
	
	return 0;
}

/**
 * @brief: get y gamma param
 * @author: lz
 * @date: 2016-9-9
 * @param [out] *p_y_gamma:  y gamma param
 */
int ak_isp_vp_get_y_gamma_attr(AK_ISP_Y_GAMMA_ATTR *p_y_gamma)
{
	isp_dbg("%s enter.\n", __func__);
	memcpy(p_y_gamma,&(isp->y_gamma_para),sizeof(AK_ISP_Y_GAMMA_ATTR));

	return 0;
}

/**
 * @brief: set raw gamma param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_raw_lut:  raw gamma param
 */
int ak_isp_vp_set_raw_lut_attr(AK_ISP_RAW_LUT_ATTR *p_raw_lut)
{
    int i = 0,j=0;
    unsigned long cmd = 0;
    isp_dbg("%s enter.\n", __func__);
    memcpy(&(isp->raw_lut_para),p_raw_lut,sizeof(AK_ISP_RAW_LUT_ATTR));

    for (i=1; i<129; i++)
    {
        cmd = 0;
        cmd = (p_raw_lut->raw_r[i]) |
            (p_raw_lut->raw_g[i] << 10) |
            (p_raw_lut->raw_b[i] << 20);
        j = i-1;
        isp->cfg_mem_blkaddr1[j] = cmd;
        j++;
    }

	ak_isp_vo_update_setting();
	
    return 0;
}

/**
 * @brief: get raw gamma param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_raw_lut:  raw gamma param
 */
int ak_isp_vp_get_raw_lut_attr(AK_ISP_RAW_LUT_ATTR *p_raw_lut)
{
    isp_dbg("%s enter.\n", __func__); 
    memcpy(p_raw_lut,&(isp->raw_lut_para),sizeof(AK_ISP_RAW_LUT_ATTR));

    return 0;
}

/**
 * @brief: set envi dpc param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_dpc:  dpc param
 */
int _set_dpc_attr(AK_ISP_DDPC* p_dpc)
{
    unsigned long cmd = 0;  
    isp_dbg("%s enter.\n", __func__);
    
    //raw_reg_20
    cmd = 0;
	cmd = isp->reg_blkaddr2[19];
	CLEAR_BITS(cmd, 0, 10);
	CLEAR_BITS(cmd, 16, 2);
    cmd |= (LOW_BITS(p_dpc->ddpc_th, 10) << DPC_TH)
    	|  (LOW_BITS(p_dpc->black_dpc_enable, 1) << BLACK_DPC_EN)
    	|  (LOW_BITS(p_dpc->white_dpc_enable, 1) << WHITE_DPC_EN);
    isp->reg_blkaddr2[19] = cmd;
    
	//reg_block5  enable_reg
    cmd = 0;
    cmd = isp->reg_blkaddr5[32];
    CLEAR_BITS(cmd, DDPC_EN, 1);
    cmd |= (LOW_BITS(p_dpc->ddpc_enable, 1) << DDPC_EN);
    isp->reg_blkaddr5[32] = cmd;

	ak_isp_vo_update_setting();

    return 0;
} 

/**
 * @brief: set dpc param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_dpc:  dpc param
 */
int ak_isp_vp_set_dpc_attr(AK_ISP_DDPC_ATTR *p_dpc)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(&(isp->dpc_para), p_dpc,sizeof(AK_ISP_DDPC_ATTR));
    isp->linkage_para_update_flag =  1;

    return 0;
}

/**
 * @brief: get dpc param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_dpc:  dpc param
 */
int ak_isp_vp_get_dpc_attr(AK_ISP_DDPC_ATTR *p_dpc)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(p_dpc,&(isp->dpc_para),sizeof(AK_ISP_DDPC_ATTR));

    return 0;
}

/**
 * @brief: set sdpc param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_sdpc:  sdpc param
 */
int ak_isp_vp_set_sdpc_attr(AK_ISP_SDPC_ATTR *p_sdpc)
{
    int sdpc_cfg;
    isp_dbg("%s enter.\n", __func__);
    memcpy(&(isp->sdpc_para),p_sdpc,sizeof(AK_ISP_SDPC_ATTR));
    memcpy(isp->sdpc_area, isp->sdpc_para.sdpc_table, 1024*4);

	sdpc_cfg = REG32(isp->base + DMA_SDPC_ADDR);
    if(isp->sdpc_para.sdpc_enable)
		SET_BITS(sdpc_cfg, 31, 1);
    else
        CLEAR_BITS(sdpc_cfg, 31, 1);

    REG32(isp->base + DMA_SDPC_ADDR) = sdpc_cfg;

	ak_isp_vo_update_setting();
        return 0;
}

/**
 * @brief: get sdpc param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_sdpc:  sdpc param
 */
int ak_isp_vp_get_sdpc_attr(AK_ISP_SDPC_ATTR *p_sdpc)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(p_sdpc,&(isp->sdpc_para),sizeof(AK_ISP_SDPC_ATTR));

    return 0;
}

/**
 * @brief: set raw noise remove param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *nr1:  raw noise remove  param
 */
int _set_nr1_attr(AK_ISP_NR1 *nr1)
{
    unsigned long cmd = 0;
    isp_dbg("%s enter.\n", __func__);

    //raw_reg_21
    cmd = 0;
    cmd = (LOW_BITS(nr1->nr1_weight_rtbl[0], 10) << NR_WEIGHT_TBL_R0)
    	| (LOW_BITS(nr1->nr1_weight_rtbl[1], 10) << NR_WEIGHT_TBL_R1)
    	| (LOW_BITS(nr1->nr1_weight_rtbl[2], 10) << NR_WEIGHT_TBL_R2);
    //*pReg++ = cmd;
    isp->reg_blkaddr2[20] = cmd;

    //raw_reg_22
    cmd = 0;
    cmd = (LOW_BITS(nr1->nr1_weight_rtbl[3], 10) << NR_WEIGHT_TBL_R3)
    	| (LOW_BITS(nr1->nr1_weight_rtbl[4], 10) << NR_WEIGHT_TBL_R4)
    	| (LOW_BITS(nr1->nr1_weight_rtbl[5], 10) << NR_WEIGHT_TBL_R5);
    isp->reg_blkaddr2[21] = cmd;
    
    //raw_reg_23
    cmd = 0;
    cmd = (LOW_BITS(nr1->nr1_weight_rtbl[6], 10) << NR_WEIGHT_TBL_R6)
    	| (LOW_BITS(nr1->nr1_weight_rtbl[7], 10) << NR_WEIGHT_TBL_R7) 
    	| (LOW_BITS(nr1->nr1_weight_rtbl[8], 10) << NR_WEIGHT_TBL_R8);
    isp->reg_blkaddr2[22] = cmd;

    //raw_reg_24
    cmd = 0;
    cmd = (LOW_BITS(nr1->nr1_weight_rtbl[9], 10) << NR_WEIGHT_TBL_R9)
    	| (LOW_BITS(nr1->nr1_weight_rtbl[10], 10) << NR_WEIGHT_TBL_R10)
    	| (LOW_BITS(nr1->nr1_weight_rtbl[11], 10) << NR_WEIGHT_TBL_R11);
    isp->reg_blkaddr2[23] = cmd;
    //raw_reg_25
    cmd = 0;
    cmd = (LOW_BITS(nr1->nr1_weight_rtbl[12], 10) << NR_WEIGHT_TBL_R12)
    	| (LOW_BITS(nr1->nr1_weight_rtbl[13], 10) << NR_WEIGHT_TBL_R13)
    	| (LOW_BITS(nr1->nr1_weight_rtbl[14], 10) << NR_WEIGHT_TBL_R14);
    isp->reg_blkaddr2[24] = cmd;

    //raw_reg_26
    cmd = 0;
    cmd = (LOW_BITS(nr1->nr1_weight_rtbl[15], 10) << NR_WEIGHT_TBL_R15)
    	| (LOW_BITS(nr1->nr1_weight_rtbl[16], 10) << NR_WEIGHT_TBL_R16)
    	| (LOW_BITS(nr1->nr1_weight_gtbl[0], 10) << NR_WEIGHT_TBL_G0);
    isp->reg_blkaddr2[25] = cmd;

    //raw_reg_27
    cmd = 0;
    cmd = (LOW_BITS(nr1->nr1_weight_gtbl[1], 10) << NR_WEIGHT_TBL_G1)
    	| (LOW_BITS(nr1->nr1_weight_gtbl[2], 10) << NR_WEIGHT_TBL_G2)
    	| (LOW_BITS(nr1->nr1_weight_gtbl[3], 10) << NR_WEIGHT_TBL_G3);
    isp->reg_blkaddr2[26] = cmd;

    //raw_reg_28
    cmd = 0;
    cmd = (LOW_BITS(nr1->nr1_weight_gtbl[4], 10) << NR_WEIGHT_TBL_G4)
    	| (LOW_BITS(nr1->nr1_weight_gtbl[5], 10) << NR_WEIGHT_TBL_G5)
    	| (LOW_BITS(nr1->nr1_weight_gtbl[6], 10) << NR_WEIGHT_TBL_G6);
    isp->reg_blkaddr2[27] = cmd;

    //raw_reg_29
    cmd = 0;
    cmd = (LOW_BITS(nr1->nr1_weight_gtbl[7], 10) << NR_WEIGHT_TBL_G7)
    	| (LOW_BITS(nr1->nr1_weight_gtbl[8], 10) << NR_WEIGHT_TBL_G8)
    	| (LOW_BITS(nr1->nr1_weight_gtbl[9], 10) << NR_WEIGHT_TBL_G9);
    isp->reg_blkaddr2[28] = cmd;

    //raw_reg_30
    cmd = 0;
    cmd = (LOW_BITS(nr1->nr1_weight_gtbl[10], 10) << NR_WEIGHT_TBL_G10)
    	| (LOW_BITS(nr1->nr1_weight_gtbl[11], 10) << NR_WEIGHT_TBL_G11)
    	| (LOW_BITS(nr1->nr1_weight_gtbl[12], 10) << NR_WEIGHT_TBL_G12);
    isp->reg_blkaddr2[29] = cmd;

    //raw_reg_31
    cmd = 0;
    cmd = (LOW_BITS(nr1->nr1_weight_gtbl[13], 10) << NR_WEIGHT_TBL_G13)
    	| (LOW_BITS(nr1->nr1_weight_gtbl[14], 10) << NR_WEIGHT_TBL_G14)
    	| (LOW_BITS(nr1->nr1_weight_gtbl[15], 10) << NR_WEIGHT_TBL_G15);
    isp->reg_blkaddr2[30] = cmd;

    //raw_reg_32
    cmd = 0;
    cmd = (LOW_BITS(nr1->nr1_weight_gtbl[16], 10) << NR_WEIGHT_TBL_G16)
    	| (LOW_BITS(nr1->nr1_weight_btbl[0], 10) << NR_WEIGHT_TBL_B0)
    	| (LOW_BITS(nr1->nr1_weight_btbl[1], 10) << NR_WEIGHT_TBL_B1);
    isp->reg_blkaddr2[31] = cmd;

    //raw_reg_33
    cmd = 0;
    cmd = (LOW_BITS(nr1->nr1_weight_btbl[2], 10) << NR_WEIGHT_TBL_B2)
    	| (LOW_BITS(nr1->nr1_weight_btbl[3], 10) << NR_WEIGHT_TBL_B3)
    	| (LOW_BITS(nr1->nr1_weight_btbl[4], 10) << NR_WEIGHT_TBL_B4);
    isp->reg_blkaddr2[32] = cmd;

    //raw_reg_34
    cmd = 0;
    cmd = (LOW_BITS(nr1->nr1_weight_btbl[5], 10) << NR_WEIGHT_TBL_B5)
    	| (LOW_BITS(nr1->nr1_weight_btbl[6], 10) << NR_WEIGHT_TBL_B6)
    	| (LOW_BITS(nr1->nr1_weight_btbl[7], 10) << NR_WEIGHT_TBL_B7);
    isp->reg_blkaddr2[33] = cmd;

    //raw_reg_35
    cmd = 0;
    cmd = (LOW_BITS(nr1->nr1_weight_btbl[8], 10) << NR_WEIGHT_TBL_B8)
    	| (LOW_BITS(nr1->nr1_weight_btbl[9], 10) << NR_WEIGHT_TBL_B9)
    	| (LOW_BITS(nr1->nr1_weight_btbl[10], 10) << NR_WEIGHT_TBL_B10);
    isp->reg_blkaddr2[34] = cmd;
    
    //raw_reg_36
    cmd = 0;
    cmd = (LOW_BITS(nr1->nr1_weight_btbl[11], 10) << NR_WEIGHT_TBL_B11)
    	| (LOW_BITS(nr1->nr1_weight_btbl[12], 10) << NR_WEIGHT_TBL_B12)
    	| (LOW_BITS(nr1->nr1_weight_btbl[13], 10) << NR_WEIGHT_TBL_B13);
    isp->reg_blkaddr2[35] = cmd;

    //raw_reg_37
    cmd = 0;
    cmd = (LOW_BITS(nr1->nr1_weight_btbl[14], 10) << NR_WEIGHT_TBL_B14)
    	| (LOW_BITS(nr1->nr1_weight_btbl[15], 10) << NR_WEIGHT_TBL_B15)
    	| (LOW_BITS(nr1->nr1_weight_btbl[16], 10) << NR_WEIGHT_TBL_B16);
    isp->reg_blkaddr2[36] = cmd;

    //raw_reg_38
    cmd = 0;
    cmd = (LOW_BITS(nr1->nr1_lc_lut[0], 10) << NR_LC_LUT_0)
    	| (LOW_BITS(nr1->nr1_lc_lut[1], 10) << NR_LC_LUT_1)
    	| (LOW_BITS(nr1->nr1_lc_lut[2], 10) << NR_LC_LUT_2);
    isp->reg_blkaddr2[37] = cmd;

    //raw_reg_39
    cmd = 0;
    cmd = (LOW_BITS(nr1->nr1_lc_lut[3], 10) << NR_LC_LUT_3)
    	| (LOW_BITS(nr1->nr1_lc_lut[4], 10) << NR_LC_LUT_4)
    	| (LOW_BITS(nr1->nr1_lc_lut[5], 10) << NR_LC_LUT_5);
    isp->reg_blkaddr2[38] = cmd;

    //raw_reg_40
    cmd = 0;
    cmd = (LOW_BITS(nr1->nr1_lc_lut[6], 10) << NR_LC_LUT_6)
	    | (LOW_BITS(nr1->nr1_lc_lut[7], 10) << NR_LC_LUT_7)
	    | (LOW_BITS(nr1->nr1_lc_lut[8], 10) << NR_LC_LUT_8);
    isp->reg_blkaddr2[39] = cmd;
    //raw_reg_41
    cmd = 0;
    cmd = (LOW_BITS(nr1->nr1_lc_lut[9], 10) << NR_LC_LUT_9)
    	| (LOW_BITS(nr1->nr1_lc_lut[10], 10) << NR_LC_LUT_10)
    	| (LOW_BITS(nr1->nr1_lc_lut[11], 10) << NR_LC_LUT_11);
    isp->reg_blkaddr2[40] = cmd;

    //raw_reg_42
    cmd = 0;
    cmd = (LOW_BITS(nr1->nr1_lc_lut[12], 10) << NR_LC_LUT_12)
    	| (LOW_BITS(nr1->nr1_lc_lut[13], 10) << NR_LC_LUT_13)
    	| (LOW_BITS(nr1->nr1_lc_lut[14], 10) << NR_LC_LUT_14);
    isp->reg_blkaddr2[41] = cmd;

    //raw_reg_43
    cmd = 0;
    cmd = isp->reg_blkaddr2[42];
    CLEAR_BITS(cmd, 0, 24);
    cmd |= (LOW_BITS(nr1->nr1_lc_lut[15], 10) << NR_LC_LUT_15)
    	|  (LOW_BITS(nr1->nr1_lc_lut[16], 10) << NR_LC_LUT_16)
    	|  (LOW_BITS(nr1->nr1_k, 4) << NR_K);
        //(nr1->gb_en_th << GB_EN_TH);
    isp->reg_blkaddr2[42] = cmd;
		
    //enable
    cmd = 0;                                            //reg5
    cmd = isp->reg_blkaddr5[32];
    CLEAR_BITS(cmd, NR_EN, 1);
    cmd |= (LOW_BITS(nr1->nr1_enable, 1) << NR_EN);
    isp->reg_blkaddr5[32] = cmd;

	ak_isp_vo_update_setting();

    return 0;
}

/**
 * @brief: set raw noise remove param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *nr1:  raw noise remove  param
 */
int ak_isp_vp_set_nr1_attr(AK_ISP_NR1_ATTR *p_nr1)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(&(isp->nr1_para),p_nr1,sizeof(AK_ISP_NR1_ATTR));
	//ak_isp_vo_update_setting();
	isp->linkage_para_update_flag =1;

    return 0;
}


/**
 * @brief: get raw noise remove param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *nr1:  raw noise remove  param
 */
int ak_isp_vp_get_nr1_attr(AK_ISP_NR1_ATTR *p_nr1)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(p_nr1,&(isp->nr1_para),sizeof(AK_ISP_NR1_ATTR));

    return 0;
}

/**
 * @brief: set green balance param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *gb:  green balance param
 */
int _set_gb_attr( AK_ISP_GB *gb)
{
    unsigned long cmd = 0;

    //raw_reg_43    
    cmd = 0;
    cmd = isp->reg_blkaddr2[42];
    CLEAR_BITS(cmd, GB_EN_TH, 8);
    cmd |= (LOW_BITS(gb->gb_en_th, 8) << GB_EN_TH);
    isp->reg_blkaddr2[42] = cmd;

    //raw_reg_44
     cmd = 0;
    cmd = isp->reg_blkaddr2[43];
    CLEAR_BITS(cmd, 0, 14);
    cmd |= (LOW_BITS(gb->gb_threshold, 10) << GB_TH)
    	|  (LOW_BITS(gb->gb_kstep, 4) << GB_KSTEP);
    isp->reg_blkaddr2[43] = cmd;
        
   	//enable
    cmd = 0;
    cmd = isp->reg_blkaddr5[32];
    CLEAR_BITS(cmd, GB_EN, 1);
    cmd |= (LOW_BITS(gb->gb_enable, 1) << GB_EN);
    isp->reg_blkaddr5[32] = cmd;

	ak_isp_vo_update_setting();

    return 0;
}

/**
 * @brief: set green balance param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_gb:  green balance param
 */
int ak_isp_vp_set_gb_attr( AK_ISP_GB_ATTR *p_gb)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(&(isp->gb_para),p_gb,sizeof(AK_ISP_GB_ATTR));
	//ak_isp_vo_update_setting();
	isp->linkage_para_update_flag =1;

    return 0;
}

/**
 * @brief: get green balance param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_gb:  green balance param
 */
int ak_isp_vp_get_gb_attr(AK_ISP_GB_ATTR *p_gb)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(p_gb,&(isp->gb_para),sizeof(AK_ISP_GB_ATTR));

    return 0;
}

/**
 * @brief: set demosaic param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_demo:  demosaic param
 */
int ak_isp_vp_set_demo_attr(AK_ISP_DEMO_ATTR *p_demo)
{
    unsigned long cmd;
    
    isp_dbg("%s enter.\n", __func__);
    memcpy(&(isp->demo_para),p_demo,sizeof(AK_ISP_DEMO_ATTR));

    //raw_reg_44   regblk2
    cmd = 0;
    cmd = LOW_BITS(isp->reg_blkaddr2[43], 14);
    cmd |= (LOW_BITS(p_demo->dm_rg_thre, 10) << DM_RG_TH) 
    	|  (LOW_BITS(p_demo->dm_rg_gain, 8) << DM_RG_GAIN);
    isp->reg_blkaddr2[43]= cmd;

    //raw_reg_45   regblk2
    cmd = 0;
    cmd = (LOW_BITS(p_demo->dm_hf_th1, 10) << DM_HF_TH1)
    	| (LOW_BITS(p_demo->dm_hf_th2, 10) << DM_HF_TH2)
    	| (LOW_BITS(p_demo->dm_bg_thre, 10) << DM_BG_TH);
    isp->reg_blkaddr2[44] = cmd;

    //raw_reg_46  regblk2
    cmd = 0;
    cmd = (LOW_BITS(p_demo->dm_bg_gain, 8) << DM_BG_GAIN)
    	| (LOW_BITS(p_demo->dm_HV_th, 8) << DM_HV_TH)
    	| (LOW_BITS(p_demo->dm_gr_gain, 8) << DM_GR_GAIN)
    	| (LOW_BITS(p_demo->dm_gb_gain, 8) << DM_GB_GAIN);
    isp->reg_blkaddr2[45] = cmd;
    
	ak_isp_vo_update_setting();

    return 0;
}

/**
 * @brief: get demosaic param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_demo:  demosaic param
 */
int ak_isp_vp_get_demo_attr(AK_ISP_DEMO_ATTR *p_demo)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(p_demo,&(isp->demo_para),sizeof(AK_ISP_DEMO_ATTR));

    return 0;
}

/**
 * @brief: set color correct param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *cc:  color correct param
 */
int _set_ccm_attr(AK_ISP_CCM *cc)   
{
    unsigned long cmd;

    isp_dbg("%s enter.\n", __func__);
    memcpy(&(isp->ccm_info.ccm_current), cc, sizeof(AK_ISP_CCM));

	//raw_reg_47
	cmd = 0;
    cmd = isp->reg_blkaddr2[46];
	CLEAR_BITS(cmd, 0, 23);
	cmd |= (LOW_BITS(cc->cc_cnoise_yth, 8) << CC_CC_CNOISE_TH)
		|  (LOW_BITS(cc->cc_cnoise_gain, 8) << CC_CC_CNOISE_GAIN)
		|  (LOW_BITS(cc->cc_cnoise_slop, 7) << CC_CC_CNOISE_SLOP);
	isp->reg_blkaddr2[46] = cmd;
	
    //raw_reg_48
    cmd = 0;
    cmd = isp->reg_blkaddr2[47];
    CLEAR_BITS(cmd, 12, 20);
    cmd |= (LOW_BITS(cc->ccm[0][0], 12) << CC_CCM_RR)
    	|  (LOW_BITS(cc->ccm[0][1], 8) << CC_CCM_RG_L8);
    isp->reg_blkaddr2[47] = cmd;

    //raw_reg_49
    cmd = 0;
    cmd = (LOW_BITS((cc->ccm[0][1] >> 8), 4) << CC_CCM_RG_H4)
    	| (LOW_BITS(cc->ccm[0][2], 12) << CC_CCM_RB)
    	| (LOW_BITS(cc->ccm[1][0], 12) << CC_CCM_GR)
    	| (LOW_BITS(cc->ccm[1][1], 4) << CC_CCM_GG_L4);
    isp->reg_blkaddr2[48] = cmd;

    //raw_reg_50
    cmd = 0;
    cmd = (LOW_BITS((cc->ccm[1][1] >> 4), 8) << CC_CCM_GG_H8)
    	| (LOW_BITS(cc->ccm[1][2], 12) << CC_CCM_GB)
    	| (LOW_BITS(cc->ccm[2][0], 12) << CC_CCM_BR);
    isp->reg_blkaddr2[49] = cmd;

    //raw_reg_51
    cmd = 0;
    cmd = (LOW_BITS(cc->ccm[2][1], 12) << CC_CCM_BG)
    	| (LOW_BITS(cc->ccm[2][2], 12) << CC_CCM_BB);
    isp->reg_blkaddr2[50] = cmd;

	//enable   
    cmd = 0;                                            //reg5
    cmd = isp->reg_blkaddr5[32];
    CLEAR_BITS(cmd, CC_EN, 1);
    cmd |= (LOW_BITS(cc->cc_enable, 1) << CC_EN);
    isp->reg_blkaddr5[32] = cmd;

	ak_isp_vo_update_setting();

    return 0;   
}

/**
 * @brief: set color correct param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_ccm:  color correct param
 */
int ak_isp_vp_set_ccm_attr(AK_ISP_CCM_ATTR *p_ccm)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(&(isp->ccm_para),p_ccm,sizeof(AK_ISP_CCM_ATTR));
	//ak_isp_vo_update_setting();
	isp->linkage_ccm_update_flag =1;

    return 0;
}

/**
 * @brief: set color correct param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_ccm:  color correct param
 */
int ak_isp_vp_get_ccm_attr(AK_ISP_CCM_ATTR *p_ccm)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(p_ccm,&(isp->ccm_para),sizeof(AK_ISP_CCM_ATTR));

    return 0;
}

/**
 * @brief: set wdr  param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_wdr: wdr param
 */
int _set_wdr_attr(AK_ISP_WDR*p_wdr)
{
    int i = 0,j=0,k=1;
    unsigned long cmd = 0;
    int hdr_cnoise_suppress_slop;
    isp_dbg("%s enter.\n", __func__);

	if(p_wdr->wdr_enable)
	{
		if(p_wdr->hdr_cnoise_suppress_yth2 == p_wdr->hdr_cnoise_suppress_yth1)
			hdr_cnoise_suppress_slop = 128;
		else
			hdr_cnoise_suppress_slop = 1024/(p_wdr->hdr_cnoise_suppress_yth2-p_wdr->hdr_cnoise_suppress_yth1);
		
		for (i=1; i<129; i+=2)
		{			
		    cmd = 0;
		    cmd = (p_wdr->area_tb1[k]) |
		        (p_wdr->area_tb2[k] << 10) |
		        (p_wdr->area_tb3[k] << 20);
			
		    k++;
		    j = i-1;
		    isp->cfg_mem_blkaddr4[j] = cmd;
		}

		k = 1;
		for (i=2; i<129; i+=2)
		{
			cmd = 0;
			cmd = (p_wdr->area_tb4[k]) |
				  (p_wdr->area_tb5[k] << 10) |
				  (p_wdr->area_tb6[k] << 20);

			k++;
			j = i-1;
			isp->cfg_mem_blkaddr4[j] = cmd;
		}
		
	    //yuv_reg_3
		cmd = 0;
		cmd |= (LOW_BITS(p_wdr->wdr_th1, 8) << HDR_TH1) 
			|  (LOW_BITS(p_wdr->wdr_th2, 8) << HDR_TH2) 
			|  (LOW_BITS(p_wdr->wdr_th3, 8) << HDR_TH3) 
			|  (LOW_BITS(p_wdr->wdr_th4, 8) << HDR_TH4);
		isp->reg_blkaddr3[2] = cmd;
		 
		//yuv_reg_4
		cmd = 0;
		cmd = isp->reg_blkaddr3[3];
		CLEAR_BITS(cmd, 0, 8);
		cmd |= (LOW_BITS(p_wdr->wdr_th5, 8) << HDR_TH5);
		isp->reg_blkaddr3[3] = cmd;
		 
		//yuv_reg_5
		cmd = 0;
		cmd = isp->reg_blkaddr3[4];
		CLEAR_BITS(cmd, 12, 18);
		cmd |= (LOW_BITS(hdr_cnoise_suppress_slop, 8) << HDR_CNOISE_SUPPRESS_SLOP)
			|  (LOW_BITS(p_wdr->hdr_cnoise_suppress_yth1, 10) << HDR_CNOISE_SUPPRESS_YTH1);
		isp->reg_blkaddr3[4]= cmd;
		 
		//yuv_reg_6
		cmd = 0;
		cmd = isp->reg_blkaddr3[5];
		CLEAR_BITS(cmd, 0, 25);
		cmd |= (LOW_BITS(p_wdr->hdr_cnoise_suppress_yth2, 10) << HDR_CNOISE_SUPPRESS_YTH2) 
			|  (LOW_BITS(p_wdr->hdr_cnoise_suppress_gain, 10) << HDR_CNOISE_SUPPRESS_GAIN) 
			|  (LOW_BITS(p_wdr->hdr_uv_adjust_level, 5) << HDR_UV_ADJUST_LEVEL);
		isp->reg_blkaddr3[5] = cmd;
	}
	
	//enable
	cmd = 0; 
	cmd = isp->reg_blkaddr5[32];
	CLEAR_BITS(cmd, HDR_UV_ADJUST_EN, 2);
	cmd |= ((LOW_BITS(p_wdr->hdr_uv_adjust_enable, 1) << HDR_UV_ADJUST_EN)
		|  (LOW_BITS(p_wdr->wdr_enable, 1) << HDR_EN));  
	isp->reg_blkaddr5[32] = cmd;

	ak_isp_vo_update_setting();

    return 0; 
}

/**
 * @brief: set wdr  param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_wdr: wdr param
 */
int ak_isp_vp_set_wdr_attr(AK_ISP_WDR_ATTR *p_wdr)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(&(isp->wdr_para), p_wdr,sizeof(AK_ISP_WDR_ATTR));
    isp->linkage_para_update_flag = 1;

    return 0;
}

/**
 * @brief: get wdr  param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_wdr: wdr param
 */
int ak_isp_vp_get_wdr_attr(AK_ISP_WDR_ATTR *p_wdr)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(p_wdr,&(isp->wdr_para), sizeof(AK_ISP_WDR_ATTR));

    return 0;
}

/**
 * @brief: set wdr other param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_wdr_ex: wdr other param
 */
int ak_isp_vp_set_wdr_ex_attr(AK_ISP_WDR_EX_ATTR *p_wdr_ex)
{
    isp_dbg("%s enter.\n", __func__);
    //code was moved to ak_isp_vi_set_crop, auto caculate with cut_width/cut_height

    return 0;
}

/**
 * @brief: get wdr other param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_wdr_ex: wdr other param
 */
int ak_isp_vp_get_wdr_ex_attr(AK_ISP_WDR_EX_ATTR *p_wdr_ex)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(p_wdr_ex,&(isp->wdr_ex_para), sizeof(AK_ISP_WDR_EX_ATTR));

    return 0;
}

/**
 * @brief: set yuv noise remove param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *nr2: noise remove param
 */
int _set_nr2_attr(AK_ISP_NR2 *nr2)
{
	unsigned long cmd;
    isp_dbg("%s enter.\n", __func__);

    //yuv_reg_23
    cmd = 0;
    cmd = (LOW_BITS(nr2->nr2_weight_tbl[1], 10) << YNR_WEIGHT_TBL_1) 
    	| (LOW_BITS(nr2->nr2_weight_tbl[2], 10) << YNR_WEIGHT_TBL_2) 
    	| (LOW_BITS(nr2->nr2_weight_tbl[3], 10) << YNR_WEIGHT_TBL_3);
    isp->reg_blkaddr3[22] =cmd;
    
    //yuv_reg_24
    cmd = 0;
    cmd = (LOW_BITS(nr2->nr2_weight_tbl[4], 10) << YNR_WEIGHT_TBL_4) 
    	| (LOW_BITS(nr2->nr2_weight_tbl[5], 10) << YNR_WEIGHT_TBL_5) 
    	| (LOW_BITS(nr2->nr2_weight_tbl[6], 10) << YNR_WEIGHT_TBL_6);
     isp->reg_blkaddr3[23] =cmd;

    //yuv_reg_25
    cmd = 0;
    cmd = (LOW_BITS(nr2->nr2_weight_tbl[7], 10) << YNR_WEIGHT_TBL_7) 
    	| (LOW_BITS(nr2->nr2_weight_tbl[8], 10) << YNR_WEIGHT_TBL_8) 
    	| (LOW_BITS(nr2->nr2_weight_tbl[9], 10) << YNR_WEIGHT_TBL_9);
	isp->reg_blkaddr3[24] =cmd;

    //yuv_reg_26
    cmd = 0;
    cmd = (LOW_BITS(nr2->nr2_weight_tbl[10], 10) << YNR_WEIGHT_TBL_10) 
    	| (LOW_BITS(nr2->nr2_weight_tbl[11], 10) << YNR_WEIGHT_TBL_11) 
    	| (LOW_BITS(nr2->nr2_weight_tbl[12], 10) << YNR_WEIGHT_TBL_12);
    isp->reg_blkaddr3[25] =cmd;

    //yuv_reg_27
    cmd = 0;
    cmd = (LOW_BITS(nr2->nr2_weight_tbl[13], 10) << YNR_WEIGHT_TBL_13) 
    	| (LOW_BITS(nr2->nr2_weight_tbl[14], 10) << YNR_WEIGHT_TBL_14) 
    	| (LOW_BITS(nr2->nr2_weight_tbl[15], 10) << YNR_WEIGHT_TBL_15);
    isp->reg_blkaddr3[26] =cmd;

    //yuv_reg_28
    cmd = 0;
	cmd = isp->reg_blkaddr3[27];
	CLEAR_BITS(cmd, 0, 24);
	CLEAR_BITS(cmd, 30, 2);
    cmd |= (LOW_BITS(nr2->nr2_weight_tbl[16], 10) << YNR_WEIGHT_TBL_16) 
    	|  (LOW_BITS(nr2->y_dpc_th, 10) << YNR_Y_DPC_TH)
    	|  (LOW_BITS(nr2->nr2_k, 4) << YNR_K)
    	|  (LOW_BITS(nr2->y_black_dpc_enable, 1) << YNR_Y_BLACK_DPC_EN)
    	|  (LOW_BITS(nr2->y_white_dpc_enable, 1) << YNR_Y_WHITE_DPC_EN);
    isp->reg_blkaddr3[27] =cmd;

	
    //enable
    cmd = 0;	//reg5
    cmd = isp->reg_blkaddr5[32];
    CLEAR_BITS(cmd, Y_DPC_EN, 2);
    cmd |= (LOW_BITS(nr2->y_dpc_enable, 1) << Y_DPC_EN)
    	|  (LOW_BITS(nr2->nr2_enable, 1) << YNR_EN);
    isp->reg_blkaddr5[32] = cmd;

	ak_isp_vo_update_setting();

    return 0;
}

/**
 * @brief: set yuv noise remove param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_nr2: noise remove param
 */
int ak_isp_vp_set_nr2_attr(AK_ISP_NR2_ATTR *p_nr2)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(&(isp->nr2_para),p_nr2,sizeof(AK_ISP_NR2_ATTR));
	//ak_isp_vo_update_setting();
	isp->linkage_para_update_flag =1;

    return 0;
}

/**
 * @brief: get yuv noise remove param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_nr2: noise remove param
 */
int ak_isp_vp_get_nr2_attr(AK_ISP_NR2_ATTR *p_nr2)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(p_nr2,&(isp->nr2_para),sizeof(AK_ISP_NR2_ATTR));

    return 0;
}

/**
 * @brief: set 3d noise remove param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *_3d_nr: 3d noise remove param
 */
int _set_3d_nr_attr(AK_ISP_3D_NR  *_3d_nr)
{
	unsigned long cmd;
	int t_y_k1, t_y_k2,t_y_ac_th,t_uv_ac_th;
	int t_y_diffth_slop = 0;
	int t_uv_diffth_slop = 0;
	AK_ISP_3D_NR *_3d_nr_still;
	AK_ISP_3D_NR_STAT_INFO *tnr_stat;
	
	isp_dbg("%s enter.\n", __func__);

	_3d_nr_still = &isp->_3d_nr_para.linkage_3d_nr[8];
	tnr_stat = &isp->_3d_nr_stat_para;

	//yuv_reg_9
	cmd = 0;
	cmd = isp->reg_blkaddr3[8];
	CLEAR_BITS(cmd, 16, 3);
	cmd |= (LOW_BITS(_3d_nr->uv_min_enable, 1) << TNR_UV_MIN_EN)
		|  (LOW_BITS(_3d_nr->tnr_refFrame_format, 2) << TNR_COMPRESS_FORMAT);	
	isp->reg_blkaddr3[8] = cmd;
	
	//yuv_reg_10
	cmd = 0;
	cmd = (LOW_BITS(_3d_nr->ynr_weight_tbl[1], 10) << TNR_YNR_WEIGHT_TBL_1)
		| (LOW_BITS(_3d_nr->ynr_weight_tbl[2], 10) << TNR_YNR_WEIGHT_TBL_2)
		| (LOW_BITS(_3d_nr->ynr_weight_tbl[3], 10) << TNR_YNR_WEIGHT_TBL_3);
	isp->reg_blkaddr3[9] = cmd;

	//yuv_reg_11
	cmd = 0;
	cmd = (LOW_BITS(_3d_nr->ynr_weight_tbl[4], 10) << TNR_YNR_WEIGHT_TBL_4)
		| (LOW_BITS(_3d_nr->ynr_weight_tbl[5], 10) << TNR_YNR_WEIGHT_TBL_5)
		| (LOW_BITS(_3d_nr->ynr_weight_tbl[6], 10) << TNR_YNR_WEIGHT_TBL_6);
	isp->reg_blkaddr3[10] = cmd;

	//yuv_reg_12
	cmd = 0;
	cmd = (LOW_BITS(_3d_nr->ynr_weight_tbl[7], 10) << TNR_YNR_WEIGHT_TBL_7)
		| (LOW_BITS(_3d_nr->ynr_weight_tbl[8], 10) << TNR_YNR_WEIGHT_TBL_8)
		| (LOW_BITS(_3d_nr->ynr_weight_tbl[9], 10) << TNR_YNR_WEIGHT_TBL_9);
	isp->reg_blkaddr3[11] = cmd;

	//yuv_reg_13
	cmd = 0;
	cmd = (LOW_BITS(_3d_nr->ynr_weight_tbl[10], 10) << TNR_YNR_WEIGHT_TBL_10)
		| (LOW_BITS(_3d_nr->ynr_weight_tbl[11], 10) << TNR_YNR_WEIGHT_TBL_11)
		| (LOW_BITS(_3d_nr->ynr_weight_tbl[12], 10) << TNR_YNR_WEIGHT_TBL_12);
	isp->reg_blkaddr3[12] = cmd;

	//yuv_reg_14
	cmd = 0;
	cmd = (LOW_BITS(_3d_nr->ynr_weight_tbl[13], 10) << TNR_YNR_WEIGHT_TBL_13)
		| (LOW_BITS(_3d_nr->ynr_weight_tbl[14], 10) << TNR_YNR_WEIGHT_TBL_14)
		| (LOW_BITS(_3d_nr->ynr_weight_tbl[15], 10) << TNR_YNR_WEIGHT_TBL_15);
	isp->reg_blkaddr3[13] = cmd;

	//yuv_reg_15
	cmd = 0;
	cmd = (LOW_BITS(_3d_nr->ynr_weight_tbl[16], 10) << TNR_YNR_WEIGHT_TBL_16)
		| (LOW_BITS(_3d_nr->ynr_diff_shift, 1) << TNR_YNR_DIFF_SHIFT)
		| (LOW_BITS(_3d_nr->ynr_k, 4)  << TNR_YNR_K)
		| (LOW_BITS(_3d_nr->ylp_k, 4) << TNR_YLP_K)
		| (LOW_BITS(_3d_nr->uvnr_k, 4) << TNR_UVNR_K)
		| (LOW_BITS(_3d_nr->uvlp_k, 4) << TNR_UVLP_K);	   
	isp->reg_blkaddr3[14] = cmd;


	if(_3d_nr->t_y_mf_th2 == _3d_nr->t_y_mf_th1)
		_3d_nr->t_y_mf_th1 = _3d_nr->t_y_mf_th2 - 1;
	//yuv_reg_16
	t_y_diffth_slop =  (((_3d_nr->t_y_diffth_k1 - _3d_nr->t_y_diffth_k2 )*512)
		/ (_3d_nr->t_y_mf_th2 - _3d_nr->t_y_mf_th1) +1)/2;
	cmd = 0;
	cmd = (LOW_BITS(_3d_nr->t_y_th1, 8) << TNR_T_Y_TH1)
		| (LOW_BITS(_3d_nr->t_y_diffth_k1, 8) << TNR_T_Y_DIFFTH_K1)
		| (LOW_BITS(_3d_nr->t_y_diffth_k2, 8) << TNR_T_Y_DIFFTH_K2)
		| (LOW_BITS(t_y_diffth_slop, 8) << TNR_T_Y_DIFFTH_SLOP);
	isp->reg_blkaddr3[15] = cmd;

	//yuv_reg_17
	t_y_k1 = _3d_nr->t_y_k1;
	if(_3d_nr->t_y_k1<_3d_nr_still->t_y_k1)
		t_y_k1 = (_3d_nr->t_y_k1 *tnr_stat->MD_level+_3d_nr_still->t_y_k1*(16-tnr_stat->MD_level ))/16;
	t_y_k2 = _3d_nr->t_y_k2;
	if(_3d_nr->t_y_k2<_3d_nr_still->t_y_k2)
		t_y_k2 = (_3d_nr->t_y_k2 *tnr_stat->MD_level+_3d_nr_still->t_y_k2*(16-tnr_stat->MD_level ))/16;
	t_y_ac_th = (_3d_nr->t_y_ac_th *tnr_stat->MD_level+_3d_nr_still->t_y_ac_th*(16-tnr_stat->MD_level ))/16;
	cmd = 0;
	cmd = (LOW_BITS(t_y_k1, 7) << TNR_T_Y_K1)
		| (LOW_BITS(t_y_k2, 7) << TNR_T_Y_K2)
		| (LOW_BITS(_3d_nr->t_y_kslop, 7) << TNR_T_Y_KSLOP)
		| (LOW_BITS(t_y_ac_th, 10) << TNR_T_Y_AC_TH);
	isp->reg_blkaddr3[16] = cmd;

	//yuv_reg_18
	cmd = 0;
	cmd = (LOW_BITS(_3d_nr->t_y_minstep, 5) << TNR_T_Y_MINSTEP)
		| (LOW_BITS(_3d_nr->t_y_mf_th1, 13) << TNR_T_Y_MF_TH1)
		| (LOW_BITS(_3d_nr->t_y_mf_th2, 13) << TNR_T_Y_MF_TH2);
	isp->reg_blkaddr3[17] = cmd;

	//yuv_reg_19
	t_uv_ac_th = (_3d_nr->t_uv_ac_th *tnr_stat->MD_level+_3d_nr_still->t_uv_ac_th*(16-tnr_stat->MD_level ))/16;
	cmd = 0;
	cmd = (LOW_BITS(_3d_nr->t_y_mc_k, 5) << TNR_T_Y_MC_K)
		| (LOW_BITS(_3d_nr->t_uv_k, 7) << TNR_T_UV_K)
		| (LOW_BITS(_3d_nr->t_uv_minstep, 5) << TNR_T_UV_MINSTEP)
		| (LOW_BITS(_3d_nr->t_uv_mc_k, 5) << TNR_T_UV_MC_K)
		| (LOW_BITS(t_uv_ac_th, 10) << TNR_T_UV_AC_TH);
	isp->reg_blkaddr3[18] = cmd;
	
	//yuv_reg_20
	cmd = 0;
	cmd = (LOW_BITS(_3d_nr->t_uv_mf_th1, 13) << TNR_T_UV_MF_TH1)
		| (LOW_BITS(_3d_nr->t_uv_mf_th2, 13) << TNR_T_UV_MF_TH2);
	isp->reg_blkaddr3[19] = cmd;


	if(_3d_nr->t_uv_mf_th2 == _3d_nr->t_uv_mf_th1)
			_3d_nr->t_uv_mf_th1 = _3d_nr->t_uv_mf_th2 - 1;

	//yuv_reg_21
	t_uv_diffth_slop =  (((_3d_nr->t_uv_diffth_k1 - _3d_nr->t_uv_diffth_k2 ) * 512)
		/ (_3d_nr->t_uv_mf_th2 - _3d_nr->t_uv_mf_th1)+1)/2;
	cmd = 0;
	cmd = (LOW_BITS(_3d_nr->t_uv_diffth_k1, 8) << TNR_T_UV_DIFFTH_K1)
		| (LOW_BITS(_3d_nr->t_uv_diffth_k2, 8) << TNR_T_UV_DIFFTH_K2)
		| (LOW_BITS(t_uv_diffth_slop, 8) << TNR_T_UV_DIFFTH_SLOP);
	isp->reg_blkaddr3[20] = cmd;

	//endble
	cmd = 0;
    cmd = isp->reg_blkaddr5[32];
    CLEAR_BITS(cmd, TNR_Y_EN, 6);
    cmd |=  (LOW_BITS(_3d_nr->tnr_y_enable, 1) << TNR_Y_EN)
    	|   (LOW_BITS(_3d_nr->updata_ref_y, 1) << TNR_UPDATA_REF_Y)
    	|   (LOW_BITS(_3d_nr->tnr_uv_enable, 1) << TNR_UV_EN)
    	|   (LOW_BITS(_3d_nr->updata_ref_uv, 1) << TNR_UPDATA_REF_UV)
    	|   (LOW_BITS(_3d_nr->y_2dnr_enable, 1) << TNR_Y_2DNR_EN)
    	|   (LOW_BITS(_3d_nr->uv_2dnr_enable, 1) <<TNR_UV_2DNR_EN);
    
	isp->reg_blkaddr5[32] = cmd;
	
	ak_isp_vo_update_setting();
	
    return 0;	
}

/**
 * @brief: set 3d noise remove param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_3d_nr: 3d noise remove param
 */
int ak_isp_vp_set_3d_nr_attr(AK_ISP_3D_NR_ATTR *p_3d_nr)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(&(isp->_3d_nr_para),p_3d_nr,sizeof(AK_ISP_3D_NR_ATTR));
	//ak_isp_vo_update_setting();
	isp->linkage_para_update_flag =1;

    return 0;
}

/**
 * @brief: get 3d noise remove param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_3d_nr: 3d noise remove param
 */
int ak_isp_vp_get_3d_nr_attr(AK_ISP_3D_NR_ATTR *p_3d_nr)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(p_3d_nr,&(isp->_3d_nr_para),sizeof(AK_ISP_3D_NR_ATTR));

    return 0;
}

/**
 * @brief: get 3d noise remove statics param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_3d_nr_stat_info: 3d noise remove statics param
 */
int ak_isp_vp_get_3d_nr_stat_info(AK_ISP_3D_NR_STAT_INFO * p_3d_nr_stat_info)
{
	isp_dbg("%s enter.\n", __func__);
	memcpy(p_3d_nr_stat_info,&(isp->_3d_nr_stat_para), sizeof(AK_ISP_3D_NR_STAT_INFO));

	return 0;
}

/**
 * @brief: set 3d noise remove reference param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_ref: 3d noise remove referenc param
 */
int ak_isp_vp_set_3d_nr_ref_addr(AK_ISP_3D_NR_REF_ATTR *p_ref)
{
    unsigned long cmd = 0;
    
    isp_dbg("%s enter.\n", __func__);
    memcpy(&(isp->_3d_nr_ref_para),p_ref,sizeof(AK_ISP_3D_NR_REF_ATTR));

    //addr_reg_23   53
    cmd = 0;
    cmd = (LOW_BITS(p_ref->yaddr_3d, 30) << TNR_YADDR);
    isp->reg_blkaddr1[53] = cmd;

    //addr_reg_24   54
    cmd = 0;
    cmd = (LOW_BITS(p_ref->ysize_3d, 30) << TNR_YSIZE);
    isp->reg_blkaddr1[54] = cmd;

    //addr_reg_25    55
    cmd = 0;
    cmd = (LOW_BITS(p_ref->uaddr_3d, 30) << TNR_UADDR);
    isp->reg_blkaddr1[55] = cmd;
    
    //addr_reg_26    56
    cmd = 0;
    cmd = (LOW_BITS(p_ref->usize_3d, 30) << TNR_USIZE);
    isp->reg_blkaddr1[56] = cmd;

    //addr_reg_27    57
    cmd = 0;
    cmd = (LOW_BITS(p_ref->vaddr_3d, 30) << TNR_VADDR);
    isp->reg_blkaddr1[57] = cmd;
    
    //addr_reg_28    58
    cmd = 0;
    cmd = (LOW_BITS(p_ref->vsize_3d, 30) << TNR_VSIZE);
    isp->reg_blkaddr1[58] = cmd;

	ak_isp_vo_update_setting();

    return 0;
}

/**
 * @brief: get 3d noise remove reference param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_ref: 3d noise remove referenc param
 */
int ak_isp_vp_get_3d_nr_ref_addr(AK_ISP_3D_NR_REF_ATTR *p_ref)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(p_ref,&(isp->_3d_nr_ref_para),sizeof(AK_ISP_3D_NR_REF_ATTR));

    return 0;
}

/**
 * @brief: set sharp param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *sharp: sharp param
 */
int _set_sharp_attr(AK_ISP_SHARP *sharp)
{
	unsigned long cmd;
	int i ,j;
	isp_dbg("%s enter.\n", __func__);

    //yuv_reg_30
    cmd = 0 ; 
    cmd  = isp->reg_blkaddr3[29];
    CLEAR_BITS(cmd, 6, 10);
    cmd |= (LOW_BITS(sharp->mf_hpf_k, 7) << YUV_SHARPEN_MF_HPF_K) 
    	|  (LOW_BITS(sharp->sharp_method, 2) << YUV_SHARPEN_METHOD) ;
    isp->reg_blkaddr3[29] = cmd;

    //yuv_reg_32
    cmd = 0;
    cmd = (LOW_BITS(sharp->mf_hpf_shift, 4) << YUV_SHARPEN_MF_HPF_SHIFT) 
    	| (LOW_BITS(sharp->hf_hpf_k, 7) << YUV_SHARPEN_HF_HPF_K) 
    	| (LOW_BITS(sharp->hf_hpf_shift, 4) << YUV_SHARPEN_HF_HPF_SHIFT) 
    	| (LOW_BITS(sharp->sharp_skin_gain_th, 8) << YUV_SHARPEN_SKIN_GAIN_TH) 
    	| (LOW_BITS(sharp->sharp_skin_gain_weaken, 2) << YUV_SHARPEN_SKIN_GAIN_WEEKEN);
    isp->reg_blkaddr3[31] = cmd;
    
    //MF
    j = 0;
    for (i=0; i<256; i+=32)
    {
        cmd = 0;
        cmd = (sharp->MF_HPF_LUT[i] & 0x1FF) |
            ((sharp->MF_HPF_LUT[i+1] & 0x1FF) << 9) |
            ((sharp->MF_HPF_LUT[i+2] & 0x1FF) << 18) |
            ((sharp->MF_HPF_LUT[i+3] & 0x1F) << 27);
        //*pReg++ = cmd;
        isp->cfg_mem_blkaddr5[j] = cmd;
        j++;

        cmd = 0;
        cmd = ((sharp->MF_HPF_LUT[i+3] & 0x1E0) >> 5) |
            ((sharp->MF_HPF_LUT[i+4] & 0x1FF) << 4) |
            ((sharp->MF_HPF_LUT[i+5] & 0x1FF) << 13) |
            ((sharp->MF_HPF_LUT[i+6] & 0x1FF) << 22) |
            ((sharp->MF_HPF_LUT[i+7] & 0x1) << 31);
        //*pReg++ = cmd;
        isp->cfg_mem_blkaddr5[j] = cmd;
        j++;
        
        cmd = 0;
        cmd = ((sharp->MF_HPF_LUT[i+7] & 0x1FE) >> 1) |
            ((sharp->MF_HPF_LUT[i+8] & 0x1FF) << 8) |
            ((sharp->MF_HPF_LUT[i+9] & 0x1FF) << 17) |
            ((sharp->MF_HPF_LUT[i+10] & 0x3F) << 26);
        //*pReg++ = cmd;
        isp->cfg_mem_blkaddr5[j] = cmd;
         j++;
         
        cmd = 0;
        cmd = ((sharp->MF_HPF_LUT[i+10] & 0x1C0) >> 6) |
            ((sharp->MF_HPF_LUT[i+11] & 0x1FF) << 3) |
            ((sharp->MF_HPF_LUT[i+12] & 0x1FF) << 12) |
            ((sharp->MF_HPF_LUT[i+13] & 0x1FF) << 21) |
            ((sharp->MF_HPF_LUT[i+14] & 0x3) << 30);
        //*pReg++ = cmd;
        isp->cfg_mem_blkaddr5[j] = cmd;
        j++;

         
        cmd = 0;
        cmd = ((sharp->MF_HPF_LUT[i+14] & 0x1FC) >> 2) |
            ((sharp->MF_HPF_LUT[i+15] & 0x1FF) << 7) |
            ((sharp->MF_HPF_LUT[i+16] & 0x1FF) << 16) |
            ((sharp->MF_HPF_LUT[i+17] & 0x7F) << 25);
        //*pReg++ = cmd;
        isp->cfg_mem_blkaddr5[j] = cmd;
        j++;

        cmd = 0;
        cmd = ((sharp->MF_HPF_LUT[i+17] & 0x180) >> 7) |
            ((sharp->MF_HPF_LUT[i+18] & 0x1FF) << 2) |
            ((sharp->MF_HPF_LUT[i+19] & 0x1FF) << 11) |
            ((sharp->MF_HPF_LUT[i+20] & 0x1FF) << 20) |
            ((sharp->MF_HPF_LUT[i+21] & 0x7) << 29);
        //*pReg++ = cmd;
        isp->cfg_mem_blkaddr5[j] = cmd;
        j++;

        cmd = 0;
        cmd = ((sharp->MF_HPF_LUT[i+21] & 0x1F8) >> 3) |
            ((sharp->MF_HPF_LUT[i+22] & 0x1FF) << 6) |
            ((sharp->MF_HPF_LUT[i+23] & 0x1FF) << 15) |
            ((sharp->MF_HPF_LUT[i+24] & 0xFF) << 24);
        //*pReg++ = cmd;
        isp->cfg_mem_blkaddr5[j] = cmd;
        j++;

        cmd = 0;
        cmd = ((sharp->MF_HPF_LUT[i+24] & 0x100) >> 8) |
            ((sharp->MF_HPF_LUT[i+25] & 0x1FF) << 1) |
            ((sharp->MF_HPF_LUT[i+26] & 0x1FF) << 10) |
            ((sharp->MF_HPF_LUT[i+27] & 0x1FF) << 19) |
            ((sharp->MF_HPF_LUT[i+28] & 0xF) << 28);
        //*pReg++ = cmd;
        isp->cfg_mem_blkaddr5[j] = cmd;
        j++;

        cmd = 0;
        cmd = ((sharp->MF_HPF_LUT[i+28] & 0x1F0) >> 4) |
            ((sharp->MF_HPF_LUT[i+29] & 0x1FF) << 5) |
            ((sharp->MF_HPF_LUT[i+30] & 0x1FF) << 14) |
            ((sharp->MF_HPF_LUT[i+31] & 0x1FF) << 23);
        //*pReg++ = cmd;
        isp->cfg_mem_blkaddr5[j] = cmd;
        j++;
        
    }

    //HF
    for (i=0; i<256; i+=32)
    {
        cmd = 0;
        cmd = (sharp->HF_HPF_LUT[i] & 0x1FF) |
            ((sharp->HF_HPF_LUT[i+1] & 0x1FF) << 9) |
            ((sharp->HF_HPF_LUT[i+2] & 0x1FF) << 18) |
            ((sharp->HF_HPF_LUT[i+3] & 0x1F) << 27);
        //*pReg++ = cmd;
        isp->cfg_mem_blkaddr5[j] = cmd;
        j++;
        
        cmd = 0;
        cmd = ((sharp->HF_HPF_LUT[i+3] & 0x1E0) >> 5) |
            ((sharp->HF_HPF_LUT[i+4] & 0x1FF) << 4) |
            ((sharp->HF_HPF_LUT[i+5] & 0x1FF) << 13) |
            ((sharp->HF_HPF_LUT[i+6] & 0x1FF) << 22) |
            ((sharp->HF_HPF_LUT[i+7] & 0x1) << 31);
        //*pReg++ = cmd;
        isp->cfg_mem_blkaddr5[j] = cmd;
        j++;
        
        cmd = 0;
        cmd = ((sharp->HF_HPF_LUT[i+7] & 0x1FE) >> 1) |
            ((sharp->HF_HPF_LUT[i+8] & 0x1FF) << 8) |
            ((sharp->HF_HPF_LUT[i+9] & 0x1FF) << 17) |
            ((sharp->HF_HPF_LUT[i+10] & 0x3F) << 26);
        //*pReg++ = cmd;
        isp->cfg_mem_blkaddr5[j] = cmd;
        j++;
        
        cmd = 0;
        cmd = ((sharp->HF_HPF_LUT[i+10] & 0x1C0) >> 6) |
            ((sharp->HF_HPF_LUT[i+11] & 0x1FF) << 3) |
            ((sharp->HF_HPF_LUT[i+12] & 0x1FF) << 12) |
            ((sharp->HF_HPF_LUT[i+13] & 0x1FF) << 21) |
            ((sharp->HF_HPF_LUT[i+14] & 0x3) << 30);
        //*pReg++ = cmd;
        isp->cfg_mem_blkaddr5[j] = cmd;
        j++;
        
        cmd = 0;
        cmd = ((sharp->HF_HPF_LUT[i+14] & 0x1FC) >> 2) |
            ((sharp->HF_HPF_LUT[i+15] & 0x1FF) << 7) |
            ((sharp->HF_HPF_LUT[i+16] & 0x1FF) << 16) |
            ((sharp->HF_HPF_LUT[i+17] & 0x7F) << 25);
        //*pReg++ = cmd;
        isp->cfg_mem_blkaddr5[j] = cmd;
        j++;
        
        cmd = 0;
        cmd = ((sharp->HF_HPF_LUT[i+17] & 0x180) >> 7) |
            ((sharp->HF_HPF_LUT[i+18] & 0x1FF) << 2) |
            ((sharp->HF_HPF_LUT[i+19] & 0x1FF) << 11) |
            ((sharp->HF_HPF_LUT[i+20] & 0x1FF) << 20) |
            ((sharp->HF_HPF_LUT[i+21] & 0x7) << 29);
        //*pReg++ = cmd;
        isp->cfg_mem_blkaddr5[j] = cmd;
        j++;
        
        cmd = 0;
        cmd = ((sharp->HF_HPF_LUT[i+21] & 0x1F8) >> 3) |
            ((sharp->HF_HPF_LUT[i+22] & 0x1FF) << 6) |
            ((sharp->HF_HPF_LUT[i+23] & 0x1FF) << 15) |
            ((sharp->HF_HPF_LUT[i+24] & 0xFF) << 24);
        //*pReg++ = cmd;
        isp->cfg_mem_blkaddr5[j] = cmd;
        j++;
        
        cmd = 0;
        cmd = ((sharp->HF_HPF_LUT[i+24] & 0x100) >> 8) |
            ((sharp->HF_HPF_LUT[i+25] & 0x1FF) << 1) |
            ((sharp->HF_HPF_LUT[i+26] & 0x1FF) << 10) |
            ((sharp->HF_HPF_LUT[i+27] & 0x1FF) << 19) |
            ((sharp->HF_HPF_LUT[i+28] & 0xF) << 28);
        //*pReg++ = cmd;
        isp->cfg_mem_blkaddr5[j] = cmd;
        j++;
        
        cmd = 0;
        cmd = ((sharp->HF_HPF_LUT[i+28] & 0x1F0) >> 4) |
            ((sharp->HF_HPF_LUT[i+29] & 0x1FF) << 5) |
            ((sharp->HF_HPF_LUT[i+30] & 0x1FF) << 14) |
            ((sharp->HF_HPF_LUT[i+31] & 0x1FF) << 23);
        //*pReg++ = cmd;
        isp->cfg_mem_blkaddr5[j] = cmd;
        j++;
    }
    
    //enable
    cmd = 0;                                            //reg5
    cmd = isp->reg_blkaddr5[32];
    CLEAR_BITS(cmd, YUV_SHARPEN_SKIN_DETECT, 2);
    cmd |= ((LOW_BITS(sharp->sharp_skin_detect_enable, 1) << YUV_SHARPEN_SKIN_DETECT)
    	|  (LOW_BITS(sharp->ysharp_enable, 1) << YUV_SHARPEN_EN));
    isp->reg_blkaddr5[32] = cmd;

	ak_isp_vo_update_setting();

    return 0;
}

/**
 * @brief: set sharp param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_sharp: sharp param
 */
int ak_isp_vp_set_sharp_attr(AK_ISP_SHARP_ATTR *p_sharp)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(&(isp->sharp_para),p_sharp,sizeof(AK_ISP_SHARP_ATTR));
	//ak_isp_vo_update_setting();
	isp->linkage_para_update_flag =1;
	
    return 0;
}

/**
 * @brief: get sharp param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_sharp: sharp param
 */
int ak_isp_vp_get_sharp_attr(AK_ISP_SHARP_ATTR* p_sharp)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(p_sharp,&(isp->sharp_para),sizeof(AK_ISP_SHARP_ATTR));
    
    return 0;
}

/**
 * @brief: set sharp other param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_sharp_ex: sharp other param
 */
int ak_isp_vp_set_sharp_ex_attr(AK_ISP_SHARP_EX_ATTR *p_sharp_ex)
{
    unsigned long cmd;
    isp_dbg("%s enter.\n", __func__);
    memcpy(&(isp->sharp_ex_para),p_sharp_ex, sizeof(AK_ISP_SHARP_EX_ATTR));

    //yuv_reg_28
    cmd = 0;
	cmd = isp->reg_blkaddr3[27];
	CLEAR_BITS(cmd, 24, 6);
	cmd |= (LOW_BITS(p_sharp_ex->mf_HPF[0], 6) << YUV_SHARPEN_M10);
	isp->reg_blkaddr3[27] = cmd;
	
	//yuv_reg_29
	cmd = 0; 
	cmd = (LOW_BITS(p_sharp_ex->mf_HPF[1], 4) << YUV_SHARPEN_M11) 
		| (LOW_BITS(p_sharp_ex->mf_HPF[2], 4) << YUV_SHARPEN_M12) 
		| (LOW_BITS(p_sharp_ex->mf_HPF[3], 4) << YUV_SHARPEN_M13) 
		| (LOW_BITS(p_sharp_ex->mf_HPF[4], 4) << YUV_SHARPEN_M14) 
		| (LOW_BITS(p_sharp_ex->mf_HPF[5], 4) << YUV_SHARPEN_M15) 
		| (LOW_BITS(p_sharp_ex->hf_HPF[0], 6) << YUV_SHARPEN_M20)
		| (LOW_BITS(p_sharp_ex->hf_HPF[1], 4) << YUV_SHARPEN_M21);
	isp->reg_blkaddr3[28]= cmd;

    //yuv_reg_30
    cmd = 0 ;
    cmd = isp->reg_blkaddr3[29];
    CLEAR_BITS(cmd, 0, 4);
    CLEAR_BITS(cmd, 16, 16);
    cmd |= (LOW_BITS(p_sharp_ex->hf_HPF[2], 4) << YUV_SHARPEN_M22)
    	|  (LOW_BITS(p_sharp_ex->sharp_skin_max_th, 8) << YUV_SHARPEN_SKIN_MAX_TH) 
    	|  (LOW_BITS(p_sharp_ex->sharp_skin_min_th, 8) << YUV_SHARPEN_SKIN_MIN_TH);
    isp->reg_blkaddr3[29]= cmd;
    
    //yuv_reg_31
    cmd = 0;
    cmd = (LOW_BITS(p_sharp_ex->sharp_skin_v_max_th, 8) << YUV_SHARPEN_SKIN_V_MAX_TH) 
    	| (LOW_BITS(p_sharp_ex->sharp_skin_v_min_th, 8) << YUV_SHARPEN_SKIN_V_MIN_TH) 
    	| (LOW_BITS(p_sharp_ex->sharp_skin_y_max_th, 8) << YUV_SHARPEN_SKIN_Y_MAX_TH) 
    	| (LOW_BITS(p_sharp_ex->sharp_skin_y_min_th, 8) << YUV_SHARPEN_SKIN_Y_MIN_TH);
    isp->reg_blkaddr3[30]= cmd;

	ak_isp_vo_update_setting();
	
    return 0;
}

/**
 * @brief: get sharp other param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_sharp_ex: sharp other param
 */
int ak_isp_vp_get_sharp_ex_attr(AK_ISP_SHARP_EX_ATTR* p_sharp_ex)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(p_sharp_ex,&(isp->sharp_ex_para),sizeof(AK_ISP_SHARP_EX_ATTR)); 

    return 0;
}

/**
 * @brief: set false color param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *fcs: false color param
 */
int _set_fcs_attr(AK_ISP_FCS *fcs)
{
    unsigned long cmd;
    isp_dbg("%s enter.\n", __func__);

    //yuv_reg_40
    cmd = 0;
    cmd = (LOW_BITS(fcs->fcs_th, 8) << FCS_TH) 
    	| (LOW_BITS(fcs->fcs_gain_slop, 6) << FCS_SLOP) 
    	| (LOW_BITS(fcs->fcs_uv_nr_th, 10) << FCS_UV_NR_TH);
    //*pReg++ = cmd;
    isp->reg_blkaddr3[39]=  cmd;

    cmd = 0;                                            //reg5
    cmd = isp->reg_blkaddr5[32];
    CLEAR_BITS(cmd, FCS_EN, 2);
    cmd |= ((LOW_BITS(fcs->fcs_enable, 1) << FCS_EN)
    	|  (LOW_BITS(fcs->fcs_uv_nr_enable, 1) <<FCS_UV_NR_EN));
    isp->reg_blkaddr5[32] = cmd;

	ak_isp_vo_update_setting();
	
    return 0;
}

/**
 * @brief: set false color param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *fcs: false color param
 */
int ak_isp_vp_set_fcs_attr(AK_ISP_FCS_ATTR *p_fcs)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(&(isp->fcs_para),p_fcs,sizeof(AK_ISP_FCS_ATTR));
	//ak_isp_vo_update_setting();
	isp->linkage_para_update_flag =1;

    return 0;
}

/**
 * @brief: get false color param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *fcs: false color param
 */
int ak_isp_vp_get_fcs_attr(AK_ISP_FCS_ATTR *p_fcs)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(p_fcs,&(isp->fcs_para),sizeof(AK_ISP_FCS_ATTR));

    return 0;
}

/**
 * @brief: set hue param 
 * @author: lz
 * @date: 2016-9-19
 * @param [in] *hue: hue param 
 */
int _set_hue_attr( AK_ISP_HUE *hue)
{
	int i=0;
	unsigned long cmd = 0;
	isp_dbg("%s enter.\n", __func__);
		
	for (i=0; i<64; i++)
	{
		cmd = 0;
		cmd = (hue->hue_lut_a[i] & 0xff )      |
			  ((hue->hue_lut_b[i]& 0xff )<< 8) |
			  (hue->hue_lut_s[i] << 16);
		
		isp->cfg_mem_blkaddr6[i] = cmd;
	}

	//enable
	cmd = 0;
    cmd = isp->reg_blkaddr5[32];
    CLEAR_BITS(cmd, FCS_HUE_SAT_EN, 1);
    cmd |= (LOW_BITS(hue->hue_sat_en, 1) << FCS_HUE_SAT_EN);     
    isp->reg_blkaddr5[32] = cmd;
	
	ak_isp_vo_update_setting();

	return 0;
}

/**
 * @brief: set hue param
 * @author: lz
 * @date: 2016-9-12
 * @param [in] *p_hue:hue param
 */
int ak_isp_vp_set_hue_attr(AK_ISP_HUE_ATTR *p_hue)
{
	isp_dbg("%s enter.\n", __func__);
    memcpy(&(isp->hue_para),p_hue,sizeof(AK_ISP_HUE_ATTR));
	//ak_isp_vo_update_setting();
	isp->linkage_hue_update_flag =1;

    return 0;
}

/**
 * @brief: gethue param
 * @author: lz
 * @date: 2016-9-12
 * @param [in] *p_hue:hue param
 */
int ak_isp_vp_get_hue_attr(AK_ISP_HUE_ATTR *p_hue)
{
	isp_dbg("%s enter.\n", __func__);
    memcpy(p_hue,&(isp->hue_para),sizeof(AK_ISP_HUE_ATTR));

    return 0;
}

/**
 * @brief: set satruration param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *sat: satruration param
 */
int _set_saturation_attr(AK_ISP_SATURATION *sat)
{
    unsigned long cmd;
	int SE_scale_slop1 = 0;
	int SE_scale_slop2 = 0;
	
	if(sat->SE_th2 == sat->SE_th1)
		SE_scale_slop1 = 128;
	else
		SE_scale_slop1 = 1024/(sat->SE_th2-sat->SE_th1);
	if(sat->SE_th4 == sat->SE_th3)
	  SE_scale_slop2 = 128;
	else
		SE_scale_slop2 = 1024/(sat->SE_th4-sat->SE_th3);
	
    isp_dbg("%s enter.\n", __func__);

    //yuv_reg_7
    cmd = 0;
    cmd = (LOW_BITS(sat->SE_th1, 10) << SE_YTH1) 
    	| (LOW_BITS(sat->SE_th2, 10) << SE_YTH2) 
    	| (LOW_BITS(sat->SE_th3, 10) << SE_YTH3) 
    	| (LOW_BITS(sat->SE_th4, 2) << SE_YTH4_L2);
    //*pReg++ = cmd;
    isp->reg_blkaddr3[6] = cmd;

    //yuv_reg_8
    cmd = 0;
    cmd =(LOW_BITS((sat->SE_th4 >> 2), 8) << SE_YTH4_H_8) 
    	| (LOW_BITS(sat->SE_scale1, 8) << SE_SCALE1) 
    	| (LOW_BITS(sat->SE_scale2, 8) << SE_SCALE2) 
    	| (LOW_BITS(sat->SE_scale3, 8) << SE_SCALE3);
    isp->reg_blkaddr3[7] = cmd;

	//yuv_reg_9
    cmd = 0;
	cmd = isp->reg_blkaddr3[8];
    CLEAR_BITS(cmd, 0, 16);
    cmd |=((LOW_BITS(SE_scale_slop1, 8) << SE_SCALE_SLOP_1)
    	| (LOW_BITS(SE_scale_slop2, 8) << SE_SCALE_SLOP_2));
    isp->reg_blkaddr3[8] = cmd;
	
    //enable
    cmd = 0;                                            //reg5
    cmd = isp->reg_blkaddr5[32];
    CLEAR_BITS(cmd, SE_EN, 1);
    cmd |= (LOW_BITS(sat->SE_enable, 1) << SE_EN);
    isp->reg_blkaddr5[32] = cmd;

	ak_isp_vo_update_setting();
	
    return 0;
}

/**
 * @brief: set satruration param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_sat: satruration param
 */
int ak_isp_vp_set_saturation_attr(AK_ISP_SATURATION_ATTR *p_sat)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(&(isp->saturation_para),p_sat,sizeof(AK_ISP_SATURATION_ATTR));
	//ak_isp_vo_update_setting();
	isp->linkage_para_update_flag =1;

    return 0;
}

/**
 * @brief: get satruration param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_sat: satruration param
 */
int ak_isp_vp_get_saturation_attr(AK_ISP_SATURATION_ATTR *p_sat)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(p_sat,&(isp->saturation_para),sizeof(AK_ISP_SATURATION_ATTR));

    return 0;
}

int _set_contrast_attr(AK_ISP_CONTRAST  *p_contrast)
{
    unsigned long cmd ;
    isp_dbg("%s enter.\n", __func__);
    memcpy(&(isp->contrast_setting),p_contrast,sizeof(AK_ISP_CONTRAST));

    //raw_reg_52
    cmd = 0;
    cmd = isp->reg_blkaddr2[51];
    CLEAR_BITS(cmd, 0, 17);
    cmd |= ((LOW_BITS(p_contrast->y_contrast, 8) << RGB2YUV_Y_CONSTRAST) 
    	|  (LOW_BITS(p_contrast->y_shift, 9) << RGB2YUV_Y_SHIFT));
        //(IspC->rgb2yuv_para.yuv_formula << RGB2YUV_FORMULA);
    isp->reg_blkaddr2[51] = cmd;

	ak_isp_vo_update_setting();

    return 0;
}

int ak_isp_vp_set_contrast_attr(AK_ISP_CONTRAST_ATTR  *p_contrast)
{
	isp_dbg("%s enter.\n", __func__);
	memcpy(&(isp->contrast_para),p_contrast,sizeof(AK_ISP_CONTRAST_ATTR));
	isp->linkage_para_update_flag = 1;

	return 0;
}

int ak_isp_vp_get_contrast_attr(AK_ISP_CONTRAST_ATTR  *p_contrast)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(p_contrast,&(isp->contrast_para),sizeof(AK_ISP_CONTRAST_ATTR));

    return 0;
}

int ak_isp_vpp_set_main_chan_mask_area_attr( AK_ISP_MAIN_CHAN_MASK_AREA_ATTR *p_mask)
{
    unsigned int cmd = 0;
    isp_dbg("%s enter.\n", __func__);
    memcpy(&(isp->main_chan_mask_area_para),p_mask,sizeof(AK_ISP_MAIN_CHAN_MASK_AREA_ATTR));
    
    //pp_reg_1
    cmd = 0;
    cmd = (LOW_BITS(isp->main_chan_mask_area_para.mask_area[0].start_xpos, 11) << PP_MASK1_S_XPOS)
    	| (LOW_BITS(isp->main_chan_mask_area_para.mask_area[0].end_xpos, 11) << PP_MASK1_E_XPOS)
    	| (LOW_BITS(isp->main_chan_mask_area_para.mask_area[0].start_ypos, 10) << PP_MASK1_S_YPOS);
    ///*pReg++ = cmd;
    isp->reg_blkaddr4[0]=cmd;

    //pp_reg_2
    cmd = isp->reg_blkaddr4[1];
    CLEAR_BITS(cmd, 0, 10);
    cmd |= (LOW_BITS(isp->main_chan_mask_area_para.mask_area[0].end_ypos, 10) << PP_MASK1_E_YPOS);
    isp->reg_blkaddr4[1]=cmd;

    //enable
    cmd = 0;
    cmd = isp->reg_blkaddr4[10];
    CLEAR_BITS(cmd, PP_MASK1_EN, 1);
    cmd |= (LOW_BITS(isp->main_chan_mask_area_para.mask_area[0].enable, 1) << PP_MASK1_EN);
    isp->reg_blkaddr4[10] = cmd;

    //pp_reg_2
    cmd = 0;
    cmd =isp->reg_blkaddr4[1];
    CLEAR_BITS(cmd, PP_MASK2_S_XPOS, 22);
    cmd |= ((LOW_BITS(isp->main_chan_mask_area_para.mask_area[1].start_xpos, 11) << PP_MASK2_S_XPOS) 
    	|  (LOW_BITS(isp->main_chan_mask_area_para.mask_area[1].end_xpos, 11) << PP_MASK2_E_XPOS));
    //*pReg++ = cmd;
    isp->reg_blkaddr4[1] = cmd;

    //pp_reg_3
    cmd = 0;
    cmd = isp->reg_blkaddr4[2];
    CLEAR_BITS(cmd, 0, 20);
    cmd |= (LOW_BITS(isp->main_chan_mask_area_para.mask_area[1].start_ypos, 10) << PP_MASK2_S_YPOS)
    	|  (LOW_BITS(isp->main_chan_mask_area_para.mask_area[1].end_ypos, 10) << PP_MASK2_E_YPOS);
    //*pReg++ = cmd;
    isp->reg_blkaddr4[2] = cmd;

    //enable
    cmd = 0;
    cmd = isp->reg_blkaddr4[10];
    CLEAR_BITS(cmd, PP_MASK2_EN, 1);
    cmd |= (LOW_BITS(isp->main_chan_mask_area_para.mask_area[1].enable, 1) << PP_MASK2_EN);
    isp->reg_blkaddr4[10] = cmd;

	//pp_reg_3
    cmd = 0;
    cmd = isp->reg_blkaddr4[2];
    CLEAR_BITS(cmd, PP_MASK3_S_XPOS, 11);
    cmd |= (LOW_BITS(isp->main_chan_mask_area_para.mask_area[2].start_xpos, 11) << PP_MASK3_S_XPOS);
    //*pReg++ = cmd;
    isp->reg_blkaddr4[2]=cmd;

    //pp_reg_4
    cmd = 0;
    cmd = (LOW_BITS(isp->main_chan_mask_area_para.mask_area[2].end_xpos, 11) << PP_MASK3_E_XPOS)
    	| (LOW_BITS(isp->main_chan_mask_area_para.mask_area[2].start_ypos, 10) << PP_MASK3_S_YPOS) 
    	| (LOW_BITS(isp->main_chan_mask_area_para.mask_area[2].end_ypos, 10) << PP_MASK3_E_YPOS);
    //*pReg++ = cmd;
    isp->reg_blkaddr4[3] = cmd;

    //enable
    cmd = 0;
    cmd = isp->reg_blkaddr4[10];
    CLEAR_BITS(cmd, PP_MASK3_EN, 1);
    cmd |= (LOW_BITS(isp->main_chan_mask_area_para.mask_area[2].enable, 1) << PP_MASK3_EN);
    isp->reg_blkaddr4[10] = cmd;
    
    //pp_reg_5
    cmd = 0;
    cmd = (LOW_BITS(isp->main_chan_mask_area_para.mask_area[3].start_xpos, 11) << PP_MASK4_S_XPOS)
    	| (LOW_BITS(isp->main_chan_mask_area_para.mask_area[3].end_xpos, 11) << PP_MASK4_E_XPOS)
    	| (LOW_BITS(isp->main_chan_mask_area_para.mask_area[3].start_ypos, 10) << PP_MASK4_S_YPOS);
    //*pReg++ = cmd;
    isp->reg_blkaddr4[4] = cmd;

    //pp_reg_6
    cmd = 0;
    cmd = isp->reg_blkaddr4[5];
    CLEAR_BITS(cmd, 0, 10);
    cmd |= (LOW_BITS(isp->main_chan_mask_area_para.mask_area[3].end_ypos, 10) << PP_MASK4_E_YPOS);
    //*pReg++ = cmd;
    isp->reg_blkaddr4[5] = cmd;

    //enable
    cmd = 0;
    cmd = isp->reg_blkaddr4[10];
    CLEAR_BITS(cmd, PP_MASK4_EN, 1);
    cmd |= (LOW_BITS(isp->main_chan_mask_area_para.mask_area[3].enable, 1) << PP_MASK4_EN);
    isp->reg_blkaddr4[10] = cmd;

	ak_isp_vo_update_setting();

    return 0;
}

int ak_isp_vpp_get_main_chan_mask_area_attr(AK_ISP_MAIN_CHAN_MASK_AREA_ATTR *p_mask)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(p_mask,&(isp->main_chan_mask_area_para),sizeof(AK_ISP_MAIN_CHAN_MASK_AREA_ATTR));

    return 0;
}

int ak_isp_vpp_set_sub_chan_mask_area_attr(AK_ISP_SUB_CHAN_MASK_AREA_ATTR *p_mask)
{
    unsigned int cmd = 0;
    isp_dbg("%s enter.\n", __func__);
    memcpy(&(isp->sub_chan_mask_area_para),p_mask,sizeof(AK_ISP_SUB_CHAN_MASK_AREA_ATTR));

    //pp_reg_6
    cmd = 0;
    cmd = isp->reg_blkaddr4[5];
    CLEAR_BITS(cmd, 10, 22);
    cmd |= (LOW_BITS(isp->sub_chan_mask_area_para.mask_area[0].start_xpos, 11) << PP_MASK5_S_XPOS)
    	|  (LOW_BITS(isp->sub_chan_mask_area_para.mask_area[0].end_xpos, 11) << PP_MASK5_E_XPOS);
    //*pReg++ = cmd;
    isp->reg_blkaddr4[5] = cmd;

    //pp_reg_7
    cmd = 0;
    cmd = isp->reg_blkaddr4[6];
    CLEAR_BITS(cmd, 0, 20);
    cmd |= (LOW_BITS(isp->sub_chan_mask_area_para.mask_area[0].start_ypos, 10) << PP_MASK5_S_YPOS)
    	|  (LOW_BITS(isp->sub_chan_mask_area_para.mask_area[0].end_ypos, 10) << PP_MASK5_E_YPOS);
    isp->reg_blkaddr4[6] = cmd;

    //enable
    cmd = 0;
    cmd = isp->reg_blkaddr4[10];
    CLEAR_BITS(cmd, PP_MASK5_EN, 1);
    cmd |= (LOW_BITS(isp->sub_chan_mask_area_para.mask_area[0].enable, 1) << PP_MASK5_EN);
    isp->reg_blkaddr4[10] = cmd;
    
    //pp_reg_7
    cmd = 0;
	cmd = isp->reg_blkaddr4[6];
    CLEAR_BITS(cmd, 20, 11);
    cmd |= (LOW_BITS(isp->sub_chan_mask_area_para.mask_area[1].start_xpos, 11) << PP_MASK6_S_XPOS);
    //*pReg++ = cmd;
    isp->reg_blkaddr4[6] = cmd;

    //pp_reg_8
    cmd = 0;
    cmd = (LOW_BITS(isp->sub_chan_mask_area_para.mask_area[1].end_xpos, 11) << PP_MASK6_E_XPOS)
    	| (LOW_BITS(isp->sub_chan_mask_area_para.mask_area[1].start_ypos, 10) << PP_MASK6_S_YPOS)
    	| (LOW_BITS(isp->sub_chan_mask_area_para.mask_area[1].end_ypos, 10) << PP_MASK6_E_YPOS);
    //*pReg++ = cmd;
    isp->reg_blkaddr4[7] = cmd;
    
    //enable
    cmd = 0;
    cmd = isp->reg_blkaddr4[10];
    CLEAR_BITS(cmd, PP_MASK6_EN, 1);
    cmd |= (LOW_BITS(isp->sub_chan_mask_area_para.mask_area[1].enable, 1) << PP_MASK6_EN);
    isp->reg_blkaddr4[10] = cmd;

    //pp_reg_9
    cmd = 0;
    cmd = (LOW_BITS(isp->sub_chan_mask_area_para.mask_area[2].start_xpos, 11) << PP_MASK7_S_XPOS) 
    	| (LOW_BITS(isp->sub_chan_mask_area_para.mask_area[2].end_xpos, 11) << PP_MASK7_E_XPOS)
    	| (LOW_BITS(isp->sub_chan_mask_area_para.mask_area[2].start_ypos, 10) << PP_MASK7_S_YPOS);
    //*pReg++ = cmd;
    isp->reg_blkaddr4[8]=cmd;
    
    //pp_reg_10
    cmd = 0;
    cmd = isp->reg_blkaddr4[9];
    CLEAR_BITS(cmd, 0, 10);
    cmd |= (LOW_BITS(isp->sub_chan_mask_area_para.mask_area[2].end_ypos, 10) << PP_MASK7_E_YPOS);
    //*pReg++ = cmd;
    isp->reg_blkaddr4[9] = cmd;
    
    cmd = 0;
    cmd = isp->reg_blkaddr4[10];
    CLEAR_BITS(cmd, PP_MASK7_EN, 1);
    cmd |= (LOW_BITS(isp->sub_chan_mask_area_para.mask_area[2].enable, 1) << PP_MASK7_EN);
    isp->reg_blkaddr4[10] = cmd;

    //pp_reg_10
        cmd = 0;
    cmd = isp->reg_blkaddr4[9];
    CLEAR_BITS(cmd, 10, 22);
    cmd |= (LOW_BITS(isp->sub_chan_mask_area_para.mask_area[3].start_xpos, 11) << PP_MASK8_S_XPOS)
        |  (LOW_BITS(isp->sub_chan_mask_area_para.mask_area[3].end_xpos, 11) << PP_MASK8_E_XPOS);
    //*pReg++ = cmd;
    isp->reg_blkaddr4[9] = cmd;

    //pp_reg_11
    cmd = 0;
    cmd= isp->reg_blkaddr4[10];
    CLEAR_BITS(cmd, 0, 20);
    cmd |= (LOW_BITS(isp->sub_chan_mask_area_para.mask_area[3].start_ypos, 10) << PP_MASK8_S_YPOS)
    	|  (LOW_BITS(isp->sub_chan_mask_area_para.mask_area[3].end_ypos, 10) << PP_MASK8_E_YPOS);
    //*pReg++ = cmd;
    isp->reg_blkaddr4[10] = cmd;
    
    cmd = 0;
    cmd = isp->reg_blkaddr4[10];
    CLEAR_BITS(cmd, PP_MASK8_EN, 1);
    cmd |= (LOW_BITS(isp->sub_chan_mask_area_para.mask_area[3].enable, 1) << PP_MASK8_EN);
    isp->reg_blkaddr4[10] = cmd;    

	ak_isp_vo_update_setting();

    return 0;
}

int ak_isp_vpp_get_sub_chan_mask_area_attr(AK_ISP_SUB_CHAN_MASK_AREA_ATTR *p_mask)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(p_mask,&(isp->sub_chan_mask_area_para),sizeof(AK_ISP_SUB_CHAN_MASK_AREA_ATTR));

    return 0;
}

int ak_isp_vpp_set_mask_area_attr( AK_ISP_MASK_AREA_ATTR *p_mask, MASK_NUM num)
{
    unsigned long cmd = 0;

    memcpy(&(isp->mask_area_para),p_mask,sizeof(AK_ISP_MASK_AREA_ATTR));
    switch(num)
    {
    case MAIN_CHAN_ONE:
        //pp_reg_1
        cmd = 0;
        cmd = (LOW_BITS(isp->mask_area_para.mask_area[0].start_xpos, 11) << PP_MASK1_S_XPOS)
        	| (LOW_BITS(isp->mask_area_para.mask_area[0].end_xpos, 11) << PP_MASK1_E_XPOS)
        	| (LOW_BITS(isp->mask_area_para.mask_area[0].start_ypos, 10) << PP_MASK1_S_YPOS);
        ///*pReg++ = cmd;
        isp->reg_blkaddr4[0] = cmd;

        //pp_reg_2
        cmd = isp->reg_blkaddr4[1];
        CLEAR_BITS(cmd, 0, 10);
        cmd |= (LOW_BITS(isp->mask_area_para.mask_area[0].end_ypos, 10) << PP_MASK1_E_YPOS);
        isp->reg_blkaddr4[1]=cmd;

        //enable
        cmd = 0;
        cmd = isp->reg_blkaddr4[10];
        CLEAR_BITS(cmd, PP_MASK1_EN, 1);
        cmd |= (LOW_BITS(isp->mask_area_para.mask_area[0].enable, 1) << PP_MASK1_EN);
        isp->reg_blkaddr4[10] = cmd;
        break;
    case MAIN_CHAN_TWO: 
        //pp_reg_2
        cmd = 0;
        cmd =isp->reg_blkaddr4[1];
        CLEAR_BITS(cmd, 10, 22);
        cmd |= (LOW_BITS(isp->mask_area_para.mask_area[1].start_xpos, 11) << PP_MASK2_S_XPOS)
        	|  (LOW_BITS(isp->mask_area_para.mask_area[1].end_xpos, 11) << PP_MASK2_E_XPOS);
        //*pReg++ = cmd;
        isp->reg_blkaddr4[1] = cmd;

        //pp_reg_3
        cmd = 0;
        cmd = isp->reg_blkaddr4[2];
        CLEAR_BITS(cmd, 0, 20);
        cmd |= (LOW_BITS(isp->mask_area_para.mask_area[1].start_ypos, 10) << PP_MASK2_S_YPOS)
        	|  (LOW_BITS(isp->mask_area_para.mask_area[1].end_ypos, 10) << PP_MASK2_E_YPOS);
        //*pReg++ = cmd;
        isp->reg_blkaddr4[2] = cmd;

        //enable
        cmd = 0;
        cmd = isp->reg_blkaddr4[10];
        CLEAR_BITS(cmd, PP_MASK2_EN, 1);
        cmd |= (LOW_BITS(isp->mask_area_para.mask_area[1].enable, 1) << PP_MASK2_EN);
        isp->reg_blkaddr4[10] = cmd;
        break;
    case MAIN_CHAN_THREE:
        //pp_reg_3
        cmd = 0;
        cmd = isp->reg_blkaddr4[2];
        CLEAR_BITS(cmd, PP_MASK3_S_XPOS, 11);
        cmd |= (LOW_BITS(isp->mask_area_para.mask_area[2].start_xpos, 11) << PP_MASK3_S_XPOS);
        //*pReg++ = cmd;
        isp->reg_blkaddr4[2]=cmd;
         
        //pp_reg_4
        cmd = 0;
        cmd = (LOW_BITS(isp->mask_area_para.mask_area[2].end_xpos, 11) << PP_MASK3_E_XPOS)
        	| (LOW_BITS(isp->mask_area_para.mask_area[2].start_ypos, 10) << PP_MASK3_S_YPOS)
        	| (LOW_BITS(isp->mask_area_para.mask_area[2].end_ypos, 10) << PP_MASK3_E_YPOS);
        //*pReg++ = cmd;
        isp->reg_blkaddr4[3] = cmd;
        //enable
        cmd = 0;
        cmd = isp->reg_blkaddr4[10];
        CLEAR_BITS(cmd, PP_MASK3_EN, 1);
        cmd |= (LOW_BITS(isp->mask_area_para.mask_area[2].enable, 1) << PP_MASK3_EN);
        isp->reg_blkaddr4[10] = cmd;
        break;
    case MAIN_CHAN_FOURE:
        //pp_reg_5
        cmd = 0;
        cmd = (LOW_BITS(isp->mask_area_para.mask_area[3].start_xpos, 11) << PP_MASK4_S_XPOS)
        	| (LOW_BITS(isp->mask_area_para.mask_area[3].end_xpos, 11) << PP_MASK4_E_XPOS)
        	| (LOW_BITS(isp->mask_area_para.mask_area[3].start_ypos, 10) << PP_MASK4_S_YPOS);
        //*pReg++ = cmd;
                 isp->reg_blkaddr4[4] = cmd;

        //pp_reg_6
        cmd = 0;
        cmd = isp->reg_blkaddr4[5];
        CLEAR_BITS(cmd, 0, 10);
        cmd |= (LOW_BITS(isp->mask_area_para.mask_area[3].end_ypos, 10) << PP_MASK4_E_YPOS);
        //*pReg++ = cmd;
        isp->reg_blkaddr4[5] = cmd;

        //enable
        cmd = 0;
        cmd = isp->reg_blkaddr4[10];
        CLEAR_BITS(cmd, PP_MASK4_EN, 1);
        cmd |= (LOW_BITS(isp->mask_area_para.mask_area[3].enable, 1) << PP_MASK4_EN);
        isp->reg_blkaddr4[10] = cmd;
        break;
    case SUB_CHAN_ONE:
        //pp_reg_6
        cmd = 0;
        cmd = isp->reg_blkaddr4[5];
        CLEAR_BITS(cmd, 10, 22);
        cmd |= (LOW_BITS(isp->mask_area_para.mask_area[4].start_xpos, 11) << PP_MASK5_S_XPOS)
        	|  (LOW_BITS(isp->mask_area_para.mask_area[4].end_xpos, 11) << PP_MASK5_E_XPOS);
        //*pReg++ = cmd;
        isp->reg_blkaddr4[5] = cmd;

        //pp_reg_7
        cmd = 0;
        cmd = isp->reg_blkaddr4[6];
        CLEAR_BITS(cmd, 0, 20);
        cmd |= (LOW_BITS(isp->mask_area_para.mask_area[4].start_ypos, 10) << PP_MASK5_S_YPOS)
        	|  (LOW_BITS(isp->mask_area_para.mask_area[4].end_ypos, 10) << PP_MASK5_E_YPOS);
        isp->reg_blkaddr4[6] = cmd;

        //enable
        cmd = 0;
        cmd = isp->reg_blkaddr4[10];
        CLEAR_BITS(cmd, PP_MASK5_EN, 1);
        cmd |= (LOW_BITS(isp->mask_area_para.mask_area[4].enable, 1) << PP_MASK5_EN);
        isp->reg_blkaddr4[10] = cmd;
        break;
    case SUB_CHAN_TWO:
        //pp_reg_7
        cmd = 0;
        CLEAR_BITS(cmd, 20, 11);
        cmd |= (LOW_BITS(isp->mask_area_para.mask_area[5].start_xpos, 11) << PP_MASK6_S_XPOS);
        isp->reg_blkaddr4[6] = cmd;

        //pp_reg_8
        cmd = 0;
        cmd = (LOW_BITS(isp->mask_area_para.mask_area[5].end_xpos, 11) << PP_MASK6_E_XPOS)
        	| (LOW_BITS(isp->mask_area_para.mask_area[5].start_ypos, 10) << PP_MASK6_S_YPOS)
        	| (LOW_BITS(isp->mask_area_para.mask_area[5].end_ypos, 10) << PP_MASK6_E_YPOS);
        isp->reg_blkaddr4[7] = cmd;
        
        //enable
        cmd = 0;
        cmd = isp->reg_blkaddr4[10];
        CLEAR_BITS(cmd, PP_MASK6_EN, 1);
        cmd |= (LOW_BITS(isp->mask_area_para.mask_area[5].enable, 1) << PP_MASK6_EN);
        isp->reg_blkaddr4[10] = cmd;
        break;
    case SUB_CHAN_THREE:
        //pp_reg_9
        cmd = 0;
        cmd = (LOW_BITS(isp->mask_area_para.mask_area[6].start_xpos, 11) << PP_MASK7_S_XPOS)
        	| (LOW_BITS(isp->mask_area_para.mask_area[6].end_xpos, 11) << PP_MASK7_E_XPOS)
        	| (LOW_BITS(isp->mask_area_para.mask_area[6].start_ypos, 10) << PP_MASK7_S_YPOS);
        //*pReg++ = cmd;
        isp->reg_blkaddr4[8]=cmd;
        
        //pp_reg_10
        cmd = 0;
        cmd = isp->reg_blkaddr4[9];
        CLEAR_BITS(cmd, 0, 10);
        cmd |= (LOW_BITS(isp->mask_area_para.mask_area[6].end_ypos, 10) << PP_MASK7_E_YPOS);
        //*pReg++ = cmd;
        isp->reg_blkaddr4[9] = cmd;
        
        cmd = 0;
        cmd = isp->reg_blkaddr4[10];
        CLEAR_BITS(cmd, PP_MASK7_EN, 1);
        cmd |= (LOW_BITS(isp->mask_area_para.mask_area[6].enable, 1) << PP_MASK7_EN);
        isp->reg_blkaddr4[10] = cmd;
        break;
    case SUB_CHAN_FOURE:
        //pp_reg_10
        cmd = 0;
        cmd = isp->reg_blkaddr4[9];
        CLEAR_BITS(cmd, 10, 22);
        cmd |= (LOW_BITS(isp->mask_area_para.mask_area[7].start_xpos, 11) << PP_MASK8_S_XPOS)
        	|  (LOW_BITS(isp->mask_area_para.mask_area[7].end_xpos, 11) << PP_MASK8_E_XPOS);
        //*pReg++ = cmd;
        isp->reg_blkaddr4[9]=cmd;;
    
        //pp_reg_11
        cmd = 0;
        cmd= isp->reg_blkaddr4[10];
        CLEAR_BITS(cmd, 0, 20);
        cmd |= (LOW_BITS(isp->mask_area_para.mask_area[7].start_ypos, 10) << PP_MASK8_S_YPOS)
        	|  (LOW_BITS(isp->mask_area_para.mask_area[7].end_ypos, 10) << PP_MASK8_E_YPOS);
        //*pReg++ = cmd;
        isp->reg_blkaddr4[10] =cmd;
        
        cmd = 0;
        cmd = isp->reg_blkaddr4[10];
        CLEAR_BITS(cmd, PP_MASK8_EN, 1);
        cmd |= (LOW_BITS(isp->mask_area_para.mask_area[7].enable, 1) << PP_MASK8_EN);
        isp->reg_blkaddr4[10] = cmd;
        break;
    default:
        break;
    }

	ak_isp_vo_update_setting();
	
    return 0;
}

int ak_isp_vpp_get_mask_area_attr(AK_ISP_MASK_AREA_ATTR *p_mask,  MASK_NUM num)
{
   isp_dbg("%s enter.\n", __func__);
   memcpy(&(isp->mask_area_para),p_mask,sizeof(AK_ISP_MASK_AREA_ATTR));

   return 0;
}

int ak_isp_vpp_set_mask_color( AK_ISP_MASK_COLOR_ATTR *p_mask)
{
    unsigned long cmd = 0;
    
    isp_dbg("%s enter.\n", __func__);
    memcpy(&(isp->mask_color_para),p_mask,sizeof(AK_ISP_MASK_COLOR_ATTR));

    //p_mask.y_mk_color<<
    //pp_reg_24
    cmd = 0;
    cmd = (p_mask->v_mk_color << 0)
    	| (p_mask->u_mk_color << 8)
    	| (p_mask->y_mk_color << 16)
    	| (LOW_BITS(p_mask->mk_alpha, 4) << PP_MASK_ALPHA)
        | (LOW_BITS(p_mask->color_type, 2) << PP_MASK_FORMAT);
    isp->reg_blkaddr4[23] = cmd;

	ak_isp_vo_update_setting();

    return 0;
}
int ak_isp_vpp_get_mask_color(AK_ISP_MASK_COLOR_ATTR *p_mask)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(p_mask,&(isp->mask_color_para),sizeof(AK_ISP_MASK_COLOR_ATTR));

   	return 0;
}

int ak_isp_vpp_set_osd_color_table_attr(AK_ISP_OSD_COLOR_TABLE_ATTR *p_isp_color_table)
{
	int i;
	int v_index;
	unsigned long cmd;
    isp_dbg("%s enter.\n", __func__);

	if (!isp)
		return -1;

	//isp->cb.cb_printk("%s %d\n",__func__, __LINE__);
	for (i = 0; i < 16; i++)
	{
		v_index = 11 + 3 * (i / 4);

		/* set V */
		cmd = isp->reg_blkaddr4[v_index];
		CLEAR_BITS(cmd, (8 * (i % 4)), 8);
		cmd |= (LOW_BITS(*(p_isp_color_table->color_table + i), 8) << (8 * (i % 4)));
    	isp->reg_blkaddr4[v_index] = cmd;

		/* set U */
		cmd = isp->reg_blkaddr4[v_index + 1];
		CLEAR_BITS(cmd, (8 * (i % 4)), 8);
		cmd |= (LOW_BITS((*(p_isp_color_table->color_table + i) >> 8), 8) << (8 * (i % 4)));
    	isp->reg_blkaddr4[v_index + 1] = cmd;

		/* set Y */
		cmd = isp->reg_blkaddr4[v_index + 2];
		CLEAR_BITS(cmd, (8 * (i % 4)), 8);
		cmd |= (LOW_BITS((*(p_isp_color_table->color_table + i) >> 16), 8) << (8 * (i % 4)));
    	isp->reg_blkaddr4[v_index + 2] = cmd;
	}

	ak_isp_vo_update_setting_most();

	return 0;
}

int ak_isp_vpp_mainchn_osdmem_useok(void)
{
	return isp->main_osd_update_flag;
}

int ak_isp_vpp_subchn_osdmem_useok(void)
{
	return isp->sub_osd_update_flag;
}

int ak_isp_vpp_set_main_channel_osd_mem_attr(AK_ISP_OSD_MEM_ATTR *p_mem)
{
	T_U32  bytes_osd_bak = isp->bytes_main_osd; 
	
	isp->bytes_main_osd = p_mem->size;   
	isp->handle_main_osd = (T_U32)p_mem->dma_paddr;
	isp->area_main_osd = p_mem->dma_vaddr;
	if (!isp->area_main_osd) {
		isp->cb.cb_printk("Failed to alloc dma memory for main osd\n");			
	}
	if (bytes_osd_bak == isp->bytes_main_osd){
		return 0;
	}

	return 0;
}

int ak_isp_vpp_set_sub_channel_osd_mem_attr(AK_ISP_OSD_MEM_ATTR *p_mem)
{
	T_U32   bytes_osd_bak = isp->bytes_sub_osd; 
	
	isp->bytes_sub_osd = p_mem->size; 
	isp->handle_sub_osd = (T_U32)p_mem->dma_paddr;
	isp->area_sub_osd = p_mem->dma_vaddr;
	if (!isp->area_sub_osd) {
		isp->cb.cb_printk("Failed to alloc dma memory for sub osd\n");			
	}
	if (bytes_osd_bak == isp->bytes_sub_osd){
		return 0;
	}

	return 0;
}

int ak_isp_vpp_set_main_channel_osd_context_attr(AK_ISP_OSD_CONTEXT_ATTR *p_context)
{
	AK_ISP_OSD_CONTEXT_ATTR *p_osd_buffer;

	if (!isp)
		return -1;
	if( !isp->area_main_osd)
		return -1;

	/* copy all config */
	p_osd_buffer = &isp->main_osd_buffer;
	memcpy(p_osd_buffer, p_context, sizeof(AK_ISP_OSD_CONTEXT_ATTR));
	isp->main_osd_update_flag = 1;

	return 0;
}

static int ak_isp_vpp_set_main_channel_osd_context_attr_irq(void)
{
	int osd_len;
	unsigned long cmd;
	T_U8    *p_area_osd;
	T_U32   handle_osd;
	T_U32   bytes_osd;
	AK_ISP_OSD_CONTEXT_ATTR *p_context;

	if (!isp)
		return -1;

	if (!isp->main_osd_update_flag)
		return -1;

	//isp->cb.cb_printk("%s %d area_main_osd:%p, osd_context_addr:%p\n",__func__, __LINE__,isp->area_main_osd,p_context->osd_context_addr);
	p_context = &isp->main_osd_buffer;
	p_area_osd = isp->area_main_osd;
	handle_osd = isp->handle_main_osd;
	bytes_osd = isp->bytes_main_osd;
	osd_len = p_context->osd_width * p_context->osd_height / 2;
	osd_len = (osd_len < bytes_osd) ? osd_len:bytes_osd;
	if(p_context->enable)
		memcpy(p_area_osd, p_context->osd_context_addr, osd_len);
		
    isp->reg_blkaddr4[24] = LOW_BITS(handle_osd, 30);

    cmd = LOW_BITS(p_context->start_xpos, 13);
	cmd += (LOW_BITS(p_context->start_ypos, 13) << 13);
	isp->reg_blkaddr4[25] = cmd;

    cmd = LOW_BITS((p_context->start_xpos + p_context->osd_width - 1), 13);
	cmd += (LOW_BITS((p_context->start_ypos + p_context->osd_height - 1), 13) << 13);
	cmd += (LOW_BITS(p_context->alpha, 4) << 28);
	isp->reg_blkaddr4[26] = cmd;

	//isp->cb.cb_printk("%s %d osd width:%d, height:%d,start_xpos:%d,start_ypos:%d,alpha:%d,reg_blkaddr4[26]:0x%x\n",__func__, __LINE__,p_context->osd_width, p_context->osd_height,p_context->start_xpos, p_context->start_ypos,p_context->alpha,isp->reg_blkaddr4[26]);
	//isp->cb.cb_printk("%s %d osd width:%d, height:%d\n",__func__, __LINE__,p_context->osd_width, p_context->osd_height);

    cmd = isp->reg_blkaddr5[32];
    CLEAR_BITS(cmd, OSD1_EN, 1);
    cmd |= (LOW_BITS(p_context->enable, 1) << OSD1_EN);
    isp->reg_blkaddr5[32] = cmd;

	ak_isp_vo_update_setting_most();

	isp->main_osd_update_flag = 0;

	return 0;
}

int ak_isp_vpp_set_sub_channel_osd_context_attr(AK_ISP_OSD_CONTEXT_ATTR *p_context)
{
	AK_ISP_OSD_CONTEXT_ATTR *p_osd_buffer;

	if (!isp)
		return -1;
	if(!isp->area_sub_osd)
		return -1;
	
	p_osd_buffer = &isp->sub_osd_buffer;
	memcpy(p_osd_buffer, p_context, sizeof(AK_ISP_OSD_CONTEXT_ATTR));
	isp->sub_osd_update_flag = 1;

	return 0;
}

static int ak_isp_vpp_set_sub_channel_osd_context_attr_irq(void)
{
	int osd_len;
	unsigned long cmd;

	T_U8    *p_area_osd;
	T_U32   handle_osd;
	T_U32   bytes_osd;
	AK_ISP_OSD_CONTEXT_ATTR *p_context;

	if (!isp)
		return -1;

	if (!isp->sub_osd_update_flag)
		return -1;

	p_context = &isp->sub_osd_buffer;
	p_area_osd = isp->area_sub_osd;
	handle_osd = isp->handle_sub_osd;
	bytes_osd = isp->bytes_sub_osd;

	//isp->cb.cb_printk("%s %d\n",__func__, __LINE__);
	osd_len = p_context->osd_width * p_context->osd_height / 2;
	osd_len = (osd_len <= bytes_osd) ? osd_len:bytes_osd;
	if(p_context->enable)
		memcpy(p_area_osd, p_context->osd_context_addr, osd_len);

    isp->reg_blkaddr4[27] = LOW_BITS(handle_osd, 30);

    cmd = LOW_BITS(p_context->start_xpos, 13);
	cmd += (LOW_BITS(p_context->start_ypos, 13) << 13);
	isp->reg_blkaddr4[28] = cmd;

    cmd = LOW_BITS((p_context->start_xpos + p_context->osd_width - 1), 13);
	cmd += (LOW_BITS((p_context->start_ypos + p_context->osd_height - 1), 13) << 13);
	cmd += (LOW_BITS(p_context->alpha, 4) << 28);
	isp->reg_blkaddr4[29] = cmd;

    cmd = isp->reg_blkaddr5[32];
    CLEAR_BITS(cmd, OSD2_EN, 1);
    cmd |= (LOW_BITS(p_context->enable, 1) << OSD2_EN);
    isp->reg_blkaddr5[32] = cmd;

	//isp->cb.cb_printk("%s %d osd width:%d, height:%d,start_xpos:%d,start_ypos:%d,alpha:%d\n",__func__, __LINE__,p_context->osd_width, p_context->osd_height,p_context->start_xpos, p_context->start_ypos,p_context->alpha);
	//isp2_print_reg_table();
	ak_isp_vo_update_setting_most();

	isp->sub_osd_update_flag = 0;

	return 0;
}

static int ak_isp_osd_irq_update(void)
{
	if (!isp)
		return -1;

	ak_isp_vpp_set_main_channel_osd_context_attr_irq();
	ak_isp_vpp_set_sub_channel_osd_context_attr_irq();

	return 0;
}

int ak_isp_vp_set_rgb2yuv_attr(AK_ISP_RGB2YUV_ATTR*p_rgb2yuv_attr)
{
    unsigned long cmd = 0;
    
    isp_dbg("%s enter.\n", __func__);
    memcpy(&(isp->rgb2yuv_para),p_rgb2yuv_attr,sizeof(AK_ISP_RGB2YUV_ATTR));

    //raw_reg_52
    cmd = 0;
    cmd = isp->reg_blkaddr2[51];
    CLEAR_BITS(cmd, RGB2YUV_FORMULA, 1);
    cmd |= (LOW_BITS(p_rgb2yuv_attr->mode, 1) << RGB2YUV_FORMULA);
    isp->reg_blkaddr2[51] =cmd;

	ak_isp_vo_update_setting();

    return 0;
}

int ak_isp_vp_get_rgb2yuv_attr(AK_ISP_RGB2YUV_ATTR*p_rgb2yuv_attr)
{
    isp_dbg("%s enter.\n", __func__);
    memcpy(p_rgb2yuv_attr,&(isp->rgb2yuv_para),sizeof(AK_ISP_RGB2YUV_ATTR));

    return 0;
}

int ak_isp_vp_set_effect_attr(AK_ISP_EFFECT_ATTR *p_effect_attr)
{
    unsigned long cmd = 0;

    isp_dbg("%s enter.\n", __func__);
    memcpy(&(isp->effect_para),p_effect_attr,sizeof(AK_ISP_EFFECT_ATTR));

    //yuv_reg_41
    cmd = 0;
	cmd = isp->reg_blkaddr3[40];
	CLEAR_BITS(cmd, 0, 25);
    cmd |= ((LOW_BITS(p_effect_attr->y_a, 8) << ENHANCE_Y_A) 
    	|  (LOW_BITS(p_effect_attr->y_b, 8) << ENHANCE_Y_B) 
    	|  (LOW_BITS(p_effect_attr->uv_a, 9) << ENHANCE_UV_A));
    isp->reg_blkaddr3[40] = cmd;

	//yuv_reg_42
    cmd = 0;
	cmd = isp->reg_blkaddr3[41];
	CLEAR_BITS(cmd, 0, 9);
	cmd |= (LOW_BITS(p_effect_attr->uv_b, 9) << ENHANCE_UV_B);
	isp->reg_blkaddr3[41] = cmd;

    //dark margen enable
    cmd = 0;                                            //reg5
    cmd = isp->reg_blkaddr5[32];
    CLEAR_BITS(cmd, DARK_MARGIN_EN, 1);
    cmd |= (LOW_BITS(p_effect_attr->dark_margin_en, 1) << DARK_MARGIN_EN);
    isp->reg_blkaddr5[32] = cmd;

    ak_isp_vo_update_setting();

    return 0;
}

int ak_isp_vp_get_effect_attr(AK_ISP_EFFECT_ATTR *p_effect_attr)
{
    isp_dbg("%s enter.\n", __func__);    
    memcpy(p_effect_attr,&(isp->effect_para),sizeof(AK_ISP_EFFECT_ATTR));

    return 0;
}

int ak_isp_set_isp_capturing(int resume)
{
	int cnt;
	unsigned long peri_status;

	if (!isp)
		return -1;

	if (resume) {
		int maxdelay = 100;
		while ((REG32(isp->base + 0x10) & (1 << 14))) {
			if (maxdelay > 0) {
				maxdelay--;
				isp->cb.cb_msleep(1);
			} else {
				isp->cb.cb_printk("[%s:%d] reg10 b14 not clear\n", 
						__func__, __LINE__);
				break;
			}
		}
		ak_isp_vo_update_regtable(MODE_ALL);
		peri_status = REG32(isp->base + GEN_CTRL_ADDR);
		SET_BITS(peri_status, 13, 1);
		SET_BITS(peri_status, 20, 1);
		REG32(isp->base + GEN_CTRL_ADDR) = peri_status;
	} else {
		isp->pause_flag = 1;

		cnt = 100;
		while (cnt-- > 0) {
			if (isp->pause_flag == 0)
				break;
			isp->cb.cb_msleep(1); //
		}

		if (cnt == 0)
			printk("ERR: request irq set isp pause fail\n");

		if (REG32(isp->base + GEN_CTRL_ADDR) & (1 << 20)) {
			int maxdelay = 200;
			printk("ERR: isp still working\n");
			
			while ((REG32(isp->base + 0x10) & (1 << 14))) {
				if (maxdelay) {
					maxdelay--;
					isp->cb.cb_msleep(1);
				} else {
					isp->cb.cb_printk("[%s:%d] reg10 b14 not clear\n", 
							__func__, __LINE__);
					break;
				}
			}
			peri_status = REG32(isp->base + GEN_CTRL_ADDR);
			CLEAR_BITS(peri_status, 20, 1);
			CLEAR_BITS(peri_status, 13, 1);
			REG32(isp->base + GEN_CTRL_ADDR) = peri_status;
		}

		isp->cb.cb_msleep(100); //
		isp->pause_flag = 0;
	}

	return 0;
}

/***************************************************************************/
int  isp2_module_init(AK_ISP_FUNC_CB *cb, AK_ISP_SENSOR_CB *sensor_cb, void *reg_base)
{
    if (!cb || !cb->cb_malloc || !cb->cb_printk || !sensor_cb)
        return -1;
        
    isp = cb->cb_malloc(sizeof(AK_ISP_STRUCT));
    if (!isp) {
        cb->cb_printk("Failed to allocate memory for AK_ISP_STRUCT\n");
        return -1;
    }    
    
    cb->cb_memcpy(&(isp->cb), cb, sizeof(AK_ISP_FUNC_CB));
    cb->cb_memcpy(&(isp->sensor_cb), sensor_cb, sizeof(AK_ISP_SENSOR_CB));

    isp->base = reg_base;
    //register table total size is (32 * 4 * 6) = 768byte. here is more than real size.
    isp->bytes = STATIC_DEAD_PIXELS_SIZE 
    	+ DMA_CONFIG_REG_SIZE + DMA_STAT_DATA_SIZE;  
	/*
	cb->cb_dmamalloc������void *dma_alloc_coherent(struct device *dev,size_t size, dma_addr_t *handl,gfp_t gfp);
	o����DMA����������ַ��handle�������ߵ�ַ
	*/
    isp->area = cb->cb_dmamalloc(isp->bytes, &(isp->handle));
    if (!isp->area) {
        printk("Failed to allocate memory for register table\n");
        return -1;
    }
    isp->bytes_main_osd = 0;  
    isp->area_main_osd = NULL;
    isp->main_osd_buffer.osd_context_addr = NULL;
    isp->bytes_sub_osd = 0;
    isp->area_sub_osd = NULL;
    isp->sub_osd_buffer.osd_context_addr = NULL;
	isp->main_osd_update_flag = 0;
	isp->sub_osd_update_flag = 0;
	
    printk("isp_module_init: %s\n", ISP_DRV_LIB_VER);
	
    memset(isp->area, 0x0, isp->bytes);
    printk("isp2 module init: isp_struct size=%d, dma_area=0x%p, dma_bytes=%d, io_base=0x%x\n", 
            sizeof(AK_ISP_STRUCT), isp->area, isp->bytes, reg_base);

	/* REG_BLOCK_1 */
    isp->reg_blkaddr1     = (unsigned long *)isp->area;
    /* REG_BLOCK_2 */
    isp->reg_blkaddr2     = isp->reg_blkaddr1+(REG_BLK1_SIZE / 4);

    /* MEM_BLOCK_1 */
    isp->cfg_mem_blkaddr1 = isp->reg_blkaddr2 + (REG_BLK2_SIZE / 4);
    /* MEM_BLOCK_2 */
    isp->cfg_mem_blkaddr2 = isp->cfg_mem_blkaddr1 + (MEM_BLK1_SIZE / 4);
    /* MEM_BLOCK_4 */
    isp->cfg_mem_blkaddr4 = isp->cfg_mem_blkaddr2 +(MEM_BLK2_SIZE / 4);
    /* MEM_BLOCK_3 */
    isp->cfg_mem_blkaddr3 = isp->cfg_mem_blkaddr4 +(MEM_BLK4_SIZE / 4);
    /* MEM_BLOCK_5 */
    isp->cfg_mem_blkaddr5 = isp->cfg_mem_blkaddr3 +(MEM_BLK3_SIZE / 4);
    /* MEM_BLOCK_6 */
    isp->cfg_mem_blkaddr6 = isp->cfg_mem_blkaddr5 +(MEM_BLK5_SIZE / 4);

	/* REG_BLOCK_3 */
    isp->reg_blkaddr3 = isp->cfg_mem_blkaddr6 + (MEM_BLK6_SIZE / 4);
    /* REG_BLOCK_4 */
    isp->reg_blkaddr4 = isp->reg_blkaddr3 + (REG_BLK3_SIZE / 4);
    /* REG_BLOCK_5 */
    isp->reg_blkaddr5 = isp->reg_blkaddr4 + (REG_BLK4_SIZE / 4);

    isp->sdpc_area = (unsigned long *)(isp->area + DMA_CONFIG_REG_SIZE);
    isp->sdpc_handle = isp->handle + DMA_CONFIG_REG_SIZE;

    isp->stat_area = (isp->area + DMA_CONFIG_REG_SIZE 
    	+ STATIC_DEAD_PIXELS_SIZE);
    isp->stat_handle = (isp->handle + DMA_CONFIG_REG_SIZE 
    	+ STATIC_DEAD_PIXELS_SIZE);


   	//isp_init_awb_parms(isp);
   	//_cmos_cam_exp_time_init();  //comment by xc
    isp_dbg("Allocate %d bytes for register table\n", isp->bytes);

    return 0;
}

void isp2_module_fini(void)
{
	ak_isp_vi_stop_capturing();
	isp->cb.cb_msleep(500);
#ifdef AK_RTOS
	SET_BITS(REG32(0x08000000 + 0x1C), 19, 1);//disable isp clock
#else
	SET_BITS(REG32(0xF0008000 + 0x1C), 19, 1);//disable isp clock
#endif
    isp->cb.cb_dmafree(isp->area, isp->bytes, isp->handle);    
    isp->cb.cb_free(isp);
    isp = NULL;
}

/**
 * @brief: isp irq work
 * 
 * @author: xiepenghe
 * @date: 2016-5-06
 **/
int ak_isp_irq_work(void)
{	
	int cmd;
	unsigned long peri_status;
	
	if(!isp)
		return 0;

	if (isp->pause_flag == 1) {
		peri_status = REG32(isp->base + GEN_CTRL_ADDR);
		CLEAR_BITS(peri_status, 20, 1);
		CLEAR_BITS(peri_status, 13, 1);
		REG32(isp->base + GEN_CTRL_ADDR) = peri_status;
		isp->pause_flag = 0;
	}

	cmd = REG32(isp->base + 0x10)&0x01;
	if(!cmd)
		return 0;
	isp->frame_cnt++;
	
	//get last frame id
	isp->current_buffer = isp->next_buffer;

	_isp_enable_buffer();
	
	//read frame id
	cmd = REG32(isp->base + (STATUS_REG_ADDR-ISP_BASE_ADDR));
	cmd = (cmd>>MAIN_FRAME_BUF_ID)&0x03;
	if(isp->current_buffer == cmd)
	{
		cmd = REG32(isp->base + 0x00);
		cmd = (cmd>>15)&0x0f;
		isp->next_buffer= (isp->current_buffer+1)%4;
		while(((1<<isp->next_buffer)&cmd)==0)
		{
			isp->next_buffer = (isp->next_buffer+1)%4;
		}
	}
	else	//switch to next frame
		isp->next_buffer = cmd;
	
	//AWB handle

	//do 3d nr handle
	cmd = REG32(isp->base + (STATUS_REG_ADDR-ISP_BASE_ADDR));
	cmd &=  0x1<<TNR_DMA_OVERFLOW;
	if(cmd)
	{
		REG32(isp->base + (STATUS_REG_ADDR-ISP_BASE_ADDR)) = 0x1<<TNR_DMA_OVERFLOW;

		cmd = isp->reg_blkaddr5[32];
		CLEAR_BITS(cmd, TNR_Y_EN, 1);
		CLEAR_BITS(cmd, TNR_UV_EN, 1);
		isp->reg_blkaddr5[32] = cmd;	
		isp->_3d_nr_status=1;
	}

	ak_isp_osd_irq_update();
  	//isp2_print_reg_table();
	ak_isp_vo_update_regtable(isp->reg_update_flag);
	isp->reg_update_flag = MODE_LEAST;

	return 1;
}

#define MAX_SENSORS	32
static void *sensors_info[MAX_SENSORS] = {0};
int ak_isp_register_sensor(void *sensor_info)
{
	int i;
	void **infos = &sensors_info[0];

	for (i = 0; i < MAX_SENSORS; i++) {
		if (!infos[i]) {
			infos[i] = sensor_info;
			return 0;
		}
	}

	return -1;
}

void *ak_isp_get_sensor(int *index)
{
	int i;
	void **infos = &sensors_info[0];

	for (i = *index; i < MAX_SENSORS; i++) {
		if (infos[i]) {
			*index = i;
			return infos[i];
		}
	}

	return NULL;
}

void ak_isp_remove_all_sensors(void)
{
	int i;
	void **infos = &sensors_info[0];

	for (i = 0; i < MAX_SENSORS; i++)
		infos[i] = NULL;
}

int ak_isp_sensor_cb_init(AK_ISP_SENSOR_INIT_PARA *sensor_para)
{
	if ((isp == NULL) || (isp->sensor_cb.sensor_init_func == NULL))
		return -1;

	isp->sensor_cb.sensor_init_func(sensor_para);

//jk+
	isp->sensor_cb.sensor_updata_exp_time_func(isp_sensor_ae_info.exp_time);
	isp->sensor_cb.sensor_update_a_gain_func(isp_sensor_ae_info.a_gain);
	isp->sensor_cb.sensor_update_d_gain_func(isp_sensor_ae_info.d_gain);

	return 0;
}
static AK_ISP_3D_NR_ATTR  _3d_nr2;
int ak_isp_set_td(void)
{
	AK_ISP_3D_NR_ATTR  _3d_nr;

	if (ak_isp_vp_get_3d_nr_attr(&_3d_nr) != 0) {
		printk("get 3d nr attr fail\n");
		return -1;
	}
	memcpy(&_3d_nr2, &_3d_nr, sizeof(AK_ISP_3D_NR_ATTR));

	_3d_nr._3d_nr_mode = 0;
	_3d_nr.manual_3d_nr.tnr_y_enable = 0;
	_3d_nr.manual_3d_nr.tnr_uv_enable = 0;
	if (ak_isp_vp_set_3d_nr_attr(&_3d_nr) != 0) {
		printk("set 3d nr attr fail\n");
		return -1;
	}

	printk("set td ok\n");
	return 0;
}

int ak_isp_reload_td(void)
{
	AK_ISP_3D_NR_ATTR  _3d_nr;

	if (ak_isp_vp_get_3d_nr_attr(&_3d_nr) != 0) {
		printk("get 3d nr attr fail\n");
		return -1;
	}
	if (_3d_nr._3d_nr_mode) {
		printk("_3d_nr._3d_nr_mode: %d\n", _3d_nr._3d_nr_mode);
		return 0;
	}

	if (ak_isp_vp_set_3d_nr_attr(&_3d_nr2) != 0) {
		printk("set 3d nr attr fail2\n");
		return -1;
	}

	printk("reload td ok\n");
	return 0;
}

enum scene ak_isp_get_scene(void)
{
	int iso;
	enum scene s;

	iso = _get_iso();
	if (iso == 0)
		s = SCENE_OUTDOOR;
	else
		s = SCENE_INDOOR;

	return s;
}

int ak_isp_get_yuvaddr_and_mdinfo(enum buffer_id  id, void **yuv, void **mdinfo)
{
	isp_dbg("%s: id=%d\n", __func__, id);
	int sub_len = 0;
        
	/* get yuv tail addr */
	switch(id)
	{
    case BUFFER_ONE:
		sub_len = (isp->reg_blkaddr5[30] - isp->reg_blkaddr5[29]) * 2;
		*yuv = (void *)isp->reg_blkaddr5[29] + sub_len;
        break;
    case BUFFER_TWO:
		sub_len = (isp->reg_blkaddr1[46] - isp->reg_blkaddr1[43]) * 2;
		*yuv = (void *)isp->reg_blkaddr1[43] + sub_len;
        break;
    case BUFFER_THREE:
		sub_len = (isp->reg_blkaddr1[47] - isp->reg_blkaddr1[44]) * 2;
		*yuv = (void *)isp->reg_blkaddr1[44] + sub_len;
        break;
    case BUFFER_FOUR:
		sub_len = (isp->reg_blkaddr1[48] - isp->reg_blkaddr1[45]) * 2;
		*yuv = (void *)isp->reg_blkaddr1[45] + sub_len;
        break;
    default:
		*yuv = NULL;
        break;
    }

	/* get md info addr */
	*mdinfo = isp->_3d_nr_stat_para.MD_stat;

	return 0;
}
