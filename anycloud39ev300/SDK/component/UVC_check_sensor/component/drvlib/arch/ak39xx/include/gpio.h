/**
 * @file gpio.h
 * @brief gpio function header file
 *
 * This file define gpio macros and APIs: intial, set gpio, get gpio. etc.
 * Copyright (C) 2006 Anyka (GuangZhou) Software Technology Co., Ltd.
 * @author Miaobaoli
 * @date 2005-07-24
 * @version 1.0
 *
 * @note:
 * 1. ����mmiϵͳ���Ѷ����˵�gpio������Ҫɾ����ش��룬ֻ�轫�䶨��ΪINVALID_GPIO
 
 * 2. �����Ҫ�õ���չio��ֻ��Ҫ��GPIO_MULTIPLE_USE�꣬�����ö�Ӧ��gpio
 *    GPIO_EXPAND_OUT1��GPIO_EXPAND_OUT2�����ֻ��һ����չio,���Խ�GPIO_EXPAND_OUT2
 *	  ��ΪINVALID_GPIO����
 * 
 * 3. ���ڲ�ͬ��Ӳ�������Ժ���������ú���Ӧ�궨��
 *
 */

#ifndef __GPIO_H__
#define __GPIO_H__

#include "drv_gpio.h"

typedef struct
{
    E_GPIO_PIN_SHARE_CONFIG func_module;
    unsigned long reg1_bit_mask;
    unsigned long reg1_bit_value;
    unsigned long reg2_bit_mask;
    unsigned long reg2_bit_value;
    unsigned long reg3_bit_mask;
    unsigned long reg3_bit_value;
    unsigned long reg4_bit_mask;
    unsigned long reg4_bit_value;
}
T_SHARE_CFG_FUNC_MODULE;

/**
 * @brief special share pins
 * 
 */
typedef enum
{
    ePIN_AS_PWM1_S0 = 0,               ///< share pin as PWM1 IO4
    ePIN_AS_PWM1_S1,               ///< share pin as PWM1 IO50

    ePIN_AS_PWM3_S0,               ///< share pin as PWM3 IO23
    ePIN_AS_PWM3_S1,               ///< share pin as PWM3 IO57

    ePIN_AS_I2S_S0,                ///< share pin as I2S bit[24]:0
    ePIN_AS_I2S_S1,                ///< share pin as I2S bit[24]:1
    ePIN_AS_SPI0_S0,               ///< share pin as SPI1 bit[25]:0
    ePIN_AS_SPI0_S1,               ///< share pin as SPI1 bit[25]:1
    ePIN_AS_SPI1_S0,               ///< share pin as SPI2  bit[26]:0
    ePIN_AS_SPI1_S1,               ///< share pin as SPI2  bit[26]:1
    ePIN_AS_SDIO_S0,               ///< share pin as SDIO bit[27]:0
    ePIN_AS_SDIO_S1               ///< share pin as SDIO bit[27]:1
}E_SPECIAL_GPIO_PIN_SHARE_CONFIG;
typedef struct
{
	E_SPECIAL_GPIO_PIN_SHARE_CONFIG func_module;
    unsigned long reg1_bit_mask;
    unsigned long reg1_bit_value;
    unsigned long reg2_bit_mask;
    unsigned long reg2_bit_value;
    unsigned long reg3_bit_mask;
    unsigned long reg3_bit_value;
    unsigned long reg4_bit_mask;
    unsigned long reg4_bit_value;
}
T_SPECIAL_SHARE_CFG_FUNC_MODULE;
typedef struct
{
    unsigned char gpio_start;
    unsigned char gpio_end;
    unsigned char rig_num;       ///���ƼĴ���ʱ����Χ0-3
    unsigned long bit_start_mask;     ///��Ե�ַ�׵�ַ��mask
}
T_SHARE_CFG_FUNC_PULL;

typedef struct
{
    unsigned char gpio_start;     ///IO������ʼ��
    unsigned char gpio_end;      ///IO�����β��
    unsigned char rig_num;       ///���ƼĴ���ʱ����Χ0-3
    unsigned long bit_start_mask;     ///��Ե�ַ�׵�ַ��mask
    unsigned char bit_num;          ///������IO�ڵ�����Ĵ���λ��,1or2
    unsigned long valu;          ///��Ӧ��bit�뱻�õ�ֵ
}
T_SHARE_CFG_GPIO;

unsigned long gpio_pin_check(unsigned long pin);
/**
 * @brief get gpio share pin as uart
 * @author  liao_zhijun
 * @date 2010-07-28
 * @param uart_id [in]  uart id
 * @param clk_pin [in]  clk pin
 * @param data_pin [in]  data pin
 * @return bool
 * @retval true get successfully
 * @retval false fail to get
 */
bool gpio_get_uart_pin(T_UART_ID uart_id, unsigned long* clk_pin, unsigned long* data_pin);

#endif //#ifndef __GPIO_H__

