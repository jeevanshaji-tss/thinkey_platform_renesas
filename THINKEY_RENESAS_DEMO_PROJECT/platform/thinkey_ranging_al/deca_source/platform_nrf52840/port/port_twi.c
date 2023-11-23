/*
 * Copyright 2021, Murata/Sychip Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Murata/Sychip Corporation,
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Murata/Sychip Corporation.
 */

/******************************************************
 *                    Include files
 ******************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "app_error.h"
#include "port_twi.h"
#include "platform_port.h"
/******************************************************
 *                      Macros
 ******************************************************/
#define TWI_INSTANCE_ID 1
/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *           External Variable Declarations
 ******************************************************/

/******************************************************
 *           External Function Declarations
 ******************************************************/

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *                 Static Variables
 ******************************************************/
static volatile bool s_no_ack    = false;
static volatile bool s_xfer_done = false;
static twi_handler_t s_twi_handler =
{
    .instance_id = TWI_INSTANCE_ID,
    .sub_addr = 0,
    .initialized = false,
    .open = false
};
/******************************************************
 *              Static Function Declarations
 ******************************************************/

/******************************************************
 *              Static Function Definitions
 ******************************************************/
static void twi_evt_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{
    switch (p_event->type)
    {
        case NRF_DRV_TWI_EVT_DONE:
            if ((p_event->xfer_desc.type == NRF_DRV_TWI_XFER_TX)
             || (p_event->xfer_desc.type == NRF_DRV_TWI_XFER_RX))
             {
                 s_xfer_done = true;
             }

        case NRF_DRV_TWI_EVT_ADDRESS_NACK:
        case NRF_DRV_TWI_EVT_DATA_NACK:
            s_no_ack = true;
            break;

        default:
            break;
    }
}
/******************************************************
 *               Function Definitions
 ******************************************************/
twi_handler_t *port_twi_init(void)
{
    ret_code_t twi_err_code;
    const nrf_drv_twi_config_t twi_config =
    {
        .scl                = LIS2DW12_IIC_SCL,
        .sda                = LIS2DW12_IIC_SDA,
        .frequency          = NRF_TWI_FREQ_400K,
        .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
        .clear_bus_init     = false,
        .hold_bus_uninit    = false
    };

    if (!s_twi_handler.initialized)
    {
        nrf_drv_twi_t tmp_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);
        memcpy(&s_twi_handler.twi, &tmp_twi, sizeof(tmp_twi));

        s_twi_handler.initialized = true;
    }

    twi_err_code = nrf_drv_twi_init(&s_twi_handler.twi, &twi_config, twi_evt_handler, &s_twi_handler);
    APP_ERROR_CHECK(twi_err_code);

    nrf_drv_twi_enable(&s_twi_handler.twi);
    s_twi_handler.open = true;

    return &s_twi_handler;
}

void port_twi_uninit(twi_handler_t *handle)
{
    nrf_drv_twi_disable(&handle->twi);
    nrf_drv_twi_uninit(&handle->twi);
    s_twi_handler.open = false;
    s_twi_handler.initialized = false;
}

void port_twi_set_address(twi_handler_t *handle, uint8_t addr)
{
    handle->sub_addr = addr;
}

int32_t port_twi_write(twi_handler_t *handle, uint8_t reg, uint8_t *buffer, uint16_t length)
{
    ret_code_t twi_err_code;

    ASSERT(handle->open);
    ASSERT(handle->sub_addr);

    uint8_t buf[length + 1];
    buf[0] = reg;
    memcpy(&buf[1], buffer, length);

    s_no_ack    = false;
    s_xfer_done = false;
    twi_err_code = nrf_drv_twi_tx(&handle->twi, handle->sub_addr, buf, sizeof(buf), false);
    if (twi_err_code == NRF_SUCCESS)
    {
        while(!s_xfer_done && !s_no_ack);
    }

    return (((twi_err_code == NRF_SUCCESS) && (s_xfer_done == true)) ? 0 : -1);
}

int32_t port_twi_read(twi_handler_t *handle, uint8_t reg, uint8_t *buffer, uint16_t length)
{
    int32_t    ret_val      = 0;
    ret_code_t twi_err_code = NRF_SUCCESS;

    ASSERT(handle->open);
    ASSERT(handle->sub_addr);

    s_no_ack    = false;
    s_xfer_done = false;
    twi_err_code = nrf_drv_twi_tx(&handle->twi, handle->sub_addr, &reg, 1, false);
    if (twi_err_code == NRF_SUCCESS)
    {
        while(!s_xfer_done && !s_no_ack);
    }

    if ((twi_err_code != NRF_SUCCESS) || (s_xfer_done != true))
    {
        ret_val = -1;
        goto lab_end;
    }

    s_no_ack    = false;
    s_xfer_done = false;
    twi_err_code = nrf_drv_twi_rx(&handle->twi, handle->sub_addr, buffer, length);
    if (twi_err_code == NRF_SUCCESS)
    {
        while(!s_xfer_done && !s_no_ack);
    }

    if ((twi_err_code != NRF_SUCCESS) || (s_xfer_done != true))
    {
        ret_val = -1;
        goto lab_end;
    }

lab_end:
    return ret_val;
}
