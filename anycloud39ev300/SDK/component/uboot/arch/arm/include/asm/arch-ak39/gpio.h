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


/**@brief total number
 */
#define GPIO_NUMBER                     79

/**@brief invalidate gpio
 */
#define INVALID_GPIO                    0xfe

/** @name gpio output level define
 *  define gpio output level
 */
 /*@{*/
#define GPIO_LEVEL_LOW                  0
#define GPIO_LEVEL_HIGH                 1
/* @} */

/** @name gpio dir define
 *  define gpio dir
 */
 /*@{*/
#define GPIO_DIR_INPUT                  0
#define GPIO_DIR_OUTPUT                 1
/* @} */

/** @name gpio interrupt control define
 *  define gpio interrupt enable/disable
 */
 /*@{*/
#define GPIO_INTERRUPT_DISABLE          0
#define GPIO_INTERRUPT_ENABLE           1
/* @} */

/** @name gpio interrupt active level
 *  define gpio interrupt active level
 */
 /*@{*/
#define GPIO_INTERRUPT_LOW_ACTIVE       0
#define GPIO_INTERRUPT_HIGH_ACTIVE      1   
/* @} */

/** @name gpio interrupt type define
 *  define gpio interrupt type
 */
 /*@{*/
#define GPIO_LEVEL_INTERRUPT            0
#define GPIO_EDGE_INTERRUPT             1
/* @} */


typedef enum
{
    PULLUP = 0,
    PULLDOWN,
    PULLUPDOWN,
    UNDEFINED
}T_GPIO_TYPE;


/**
 * @brief share pins
 * 
 */
typedef enum
{
    ePIN_AS_MMCSD = 0,             ///< share pin as MDAT1, 8 lines
    ePIN_AS_I2S,                ///< share pin as I2S bit[24]:0
    ePIN_AS_PWM0,               ///< share pin as PWM0   
    ePIN_AS_PWM1,               ///< share pin as PWM1
    ePIN_AS_PWM2,               ///< share pin as PWM2
    ePIN_AS_PWM3,               ///< share pin as PWM3
    ePIN_AS_PWM4,               ///< share pin as PWM4
	
    ePIN_AS_SDIO,               ///< share pin as SDIO
    ePIN_AS_UART1,              ///< share pin as UART1
    ePIN_AS_UART2,              ///< share pin as UART2
    ePIN_AS_CAMERA,             ///< share pin as CAMERA
    ePIN_AS_SPI0,               ///< share pin as SPI1 bit[25]:0
    ePIN_AS_SPI1,               ///< share pin as SPI2  bit[26]:1
    ePIN_AS_JTAG,               ///< share pin as JTAG
    ePIN_AS_TWI,                ///< share pin as I2C
    ePIN_AS_MAC,                ///< share pin as Ethernet MAC
    ePIN_AS_OPCLK,

    ePIN_AS_DUMMY

}E_GPIO_PIN_SHARE_CONFIG;

/**
 * @brief gpio_attr
 * 
 */

typedef enum
{
    GPIO_ATTR_IE = 1,   ///<input enable
    GPIO_ATTR_PE,       ///<pullup/pulldown enable
    GPIO_ATTR_SL,       ///<slew rate
    GPIO_ATTR_DS,       ///<drive strength
    GPIO_ATTR_PS        ///<pullup/pulldown selection
}T_GPIO_PIN_ATTR;



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



#endif

