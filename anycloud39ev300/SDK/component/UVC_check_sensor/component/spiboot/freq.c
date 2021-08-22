/**
 * @FILENAME: freq.c
 * @BRIEF freq driver file
 * Copyright (C) 2007 Anyka (GuangZhou) Software Technology Co., Ltd.
 * @AUTHOR liao_zhijun
 * @DATE 2010-05-24
 * @VERSION 1.0
 * @REF
 */
#include "anyka_cpu.h"
#include "anyka_types.h"
#include "l2.h"
#include "freq.h"

/*
    ASPEN CPU clock test:
    ��ִ����������ָ��Ϊ��(��ִ��100�״Σ�T ��ʾִ��ʱ��):
          ADD      r0,r0,#1
        CMP      r0,r1
        BCC      0x300064d0
    ��������ָ�������������ݾ�����ڼĴ����У������ִ��
    ʱ��ֻ���漰�����ڴ�ȡָ�Ĳ��������·�������������?
1.�ڴ�ICACHE ��DCAHE�������
    ��ʹ�����ݺ�ָ��CACHEʱ��CPU��������ָ��Ĳ���ֻ���漰
    ���Ĵ����ͻ���Ĳ������ٶ����
        CPU clock=60MHZʱ��T=8.4s
        CPU clock=100MHZʱ��T=5.0s    
    CPU �ٶȾ���Ƶ���ʱ��Ϊ:        
        CPU clock=60MHZʱ��T=4.2s
        CPU clock=100MHZʱ��T=2.5s

2.�ڹر�ICACHE ��DCACHE�������
    ��ʱCPU��Ҫ���ڴ���ȡָ���⽫�����Ľ���CPU��ִ���ٶ�
        CPU clock=60MHZ�� T = 45.3s
    ���⣬�ڹر�ICACHE����DCACHE ������£�
        CPU clock=60MHZ�� T = 44s

    �����������ݿ��Կ�����CPU��ִ������ָ��ʱ�Ĵ󲿷�ʱ��
    ��������ȡָ��

3.����
    ��Сϵͳ�У��ر�LCD������²ⶨ
    CPU clock=60MHZ�� I = 33mA
    CPU clock=200MHZ�� I = 45mA

4. ���㷽��
    �����ض���ָ�����У���ִ�����ĵ�ʱ�估ռ�õ�CPU clock��
    ���㷽��Ϊ:

    cnt:ִ��ָ��Ĵ���
    n:   ָ������
    clk: CPUʱ������
    ÿ��ARMָ��ռ������CPUʱ������

    ָ���ִ��ʱ��T = n*cnt*2/clk

    ÿ��ָ��ռ�õ�CPUʱ������:
        t = T*clk/(n * cnt)    
*/

/**
 * @brief    get current asic frequency.
 *
 * @author    liaozhijun
 * @date     2010-04-06
 * @return    T_U32 the frequency of asic running
 */
T_U32 get_asic_freq(T_VOID)
{
	T_U32 vclk_sel;
	T_U32 core_pll_freq;
	T_U32 vclk_freq;
	T_U32 m, n, od;
	T_U32 pll_cfg_val;


	vclk_sel =  ((inl(ASIC_CLOCK_DIV_REG) >> 17) & 0x7);
	if(vclk_sel == 0)
	{
		vclk_sel += 1;
	}

	pll_cfg_val = inl(ASIC_CLOCK_DIV_REG) ;
	m = (pll_cfg_val & 0xfe);
	n = ((pll_cfg_val & 0xf00)>>8);
	od = ((pll_cfg_val & 0x3000)>>12); 

	core_pll_freq = (m * 12 * 1000000) / (n *  (1<<od));

	vclk_freq = (core_pll_freq / (1<<vclk_sel));

	return vclk_freq;  //asic clock = vclk
}

