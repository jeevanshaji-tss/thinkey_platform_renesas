
/**
 * @file    user_config.h
 * @brief   Configurations of UWB CCC Example
 * @author  Mauna Kea Semiconductor
 * @copyright   Copyright (c) 2019-2022 Mauna Kea Semiconductor Holdings.
 *              All rights reserved.
 * @{
 */
#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_

/****************************************************************************//**
 *
 *                  UCI INTERFACE define
 *
 *******************************************************************************/
#define UCI_INTF_UART 	1
#define UCI_INTF_SPI 	2

/*
 * UCI INTERFACE PORT define
 */
#define UCI_INTF_PORT	UCI_INTF_UART	//UCI_INTF_SPI

#if UCI_INTF_PORT == UCI_INTF_UART
	#define UCI_INTF_UART	1
	#undef UCI_INTF_SPI
#elif	UCI_INTF_PORT == UCI_INTF_SPI
	#define UCI_INTF_SPI	2
	#undef UCI_INTF_UART
#else
    #error UCI Interface error
#endif

/****************************************************************************//**
 *
 *                  GPIO PIN define
 *
 *******************************************************************************/
/* GPIO REPONSER ID pins define */
//#define GPIO_RESPONER_ID_PIN_0		P12_4
//#define GPIO_RESPONER_ID_PIN_1		P12_5

/* GPIO ranging role pin define */
//#define GPIO_RANGING_ROLE			P12_6

/* GPIO to control MK8000 reset pin */
//#define GPIO_MK8000_RESET_PIN		P9_3

/****************************************************************************//**
 *
 *                  UCI SDIO define
 *
 *******************************************************************************/
#define mSPI_MOSI               	(P12_0)
#define mSPI_MISO               	(P12_1)
#define mSPI_SCLK               	(P12_2)
#define mSPI_SS                 	(P12_3)

#define SPI_HOST2SLAVE_PIN			P13_0
#define SPI_SLAVE2HOST_PIN			P13_1

#define SPI_FREQ_HZ                (256000L)

/****************************************************************************//**
 *
 *                  UCI UART PIN define
 *
 *******************************************************************************/
#define UCI_UART_TX_PIN				P9_1
#define UCI_UART_RX_PIN				P9_0

/****************************************************************************//**
 *
 *                   UCI RX BUFFER MACRO define
 *
 *******************************************************************************/
#define MAX_UCI_RX_BUF_NUM 			30

/****************************************************************************//**
 *
 *         			UWB Application define
 *
 *******************************************************************************/
#define UCI_UWBS_ID_BASE    	0x01
#define SESSION_ID      		0x12345678
#define MAX_UWBS        		5

/****************************************************************************//**
 *
 *         			Loop Test define
 *
 *******************************************************************************/
#define LOOP_TEST	0

#endif // _USER_CONFIG_H_

/** @} */
