/*! ----------------------------------------------------------------------------
 * @file    dw_pdoa_node_common.c
 * @brief   Defines Commom functionalities of Node Application
 *
 * @author  Decawave 
 *
 * @attention
 *
 * Copyright 2018 (c) Decawave Ltd, Dublin, Ireland.
 *
 * All rights reserved.
 *
 */


/* Includes ------------------------------------------------------------------*/

#include "app_error.h"
#include "app_util.h"
#include "app_usbd_cdc_acm.h"

#include "nrf_drv_twi.h"
#include "nrf_drv_gpiote.h"
#include "nrf_drv_clock.h"

#include "platform_port.h"

#include "nrf_drv_wdt.h"
#if (defined(CUSTOM_BOARD_INC) && (CUSTOM_BOARD_INC == type2ab_evb))
#include "nrf_drv_power.h"
#endif
#include "thinkey_debug.h"

void wdt_init(void);

void deca_irq_handler(nrf_drv_gpiote_pin_t irqPin, nrf_gpiote_polarity_t irq_action)
{
    process_deca_irq();
}

nrf_drv_wdt_channel_id m_channel_id;

uint32_t wdt_reset_cnt = 0;

/**
 * @brief WDT events handler.
 */
void wdt_event_handler(void)
{
    //NOTE: The max amount of time we can spend in WDT interrupt is two cycles of 32768[Hz] clock - after that, reset occurs
    wdt_reset_cnt = wdt_reset_cnt + 1;
}

/* @fn  peripherals_init
 *
 * @param[in] void
 * */
void peripherals_init(void)
{
    ret_code_t ret;
    ret_code_t err_code;

   /* With this change, Reset After Power Cycle is not required */
    nrf_gpio_cfg_input(UART_0_RX_PIN, NRF_GPIO_PIN_PULLUP);

#if 0
    err_code = nrf_drv_clock_init();
    THINKEY_DEBUG_INFO("err_code:%d", err_code);
    APP_ERROR_CHECK(err_code);
#endif
    nrf_drv_clock_lfclk_request(NULL);

#if (defined(CUSTOM_BOARD_INC) && (CUSTOM_BOARD_INC == type2ab_evb))
#if 0
    ret = nrf_drv_power_init(NULL);
    THINKEY_DEBUG_INFO("nrf_drv_power_init :%d", ret);
    APP_ERROR_CHECK(ret);
#endif
#if (defined(EXTERNAL_32MHZ_CRYSTAL) && (EXTERNAL_32MHZ_CRYSTAL == 1))
    nrf_drv_clock_hfclk_request(NULL);
#endif
#endif

#ifndef ENABLE_USB_PRINT
    ret = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(ret);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    NRF_LOG_INFO("\n\rDeca Test Example......");
    NRF_LOG_FLUSH();
#endif
#if 0
    rtc_init();
#endif

    init_timer();

    /*WDT Initilization*/
    /* For digital key we are not using Watch Dog timer. Not initializing it */
    //wdt_init();
}

void dw_irq_init(void)
{
    ret_code_t err_code;

    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
    in_config.pull = NRF_GPIO_PIN_PULLDOWN; 

    err_code = nrf_drv_gpiote_in_init(DW3000_IRQ_Pin, &in_config, deca_irq_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(DW3000_IRQ_Pin, false);
}

void wdt_init(void)
{
    ret_code_t err_code;

    nrf_drv_wdt_config_t config = NRF_DRV_WDT_DEAFULT_CONFIG;
        
     /* WDT Timer is configured for 60Secs*/
    config.reload_value = 60000;
    err_code = nrf_drv_wdt_init(&config, wdt_event_handler);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_wdt_channel_alloc(&m_channel_id);
    APP_ERROR_CHECK(err_code);
    nrf_drv_wdt_enable();
}


// To Test Low power mode - Set configUSE_IDLE_HOOK as '1' in FreeRTOSConfig.h
void vApplicationIdleHook( void )
{
    __WFI();
}
