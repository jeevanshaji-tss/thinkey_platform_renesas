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
#include "port_spi.h"
#include "platform_port.h"
/******************************************************
 *                      Macros
 ******************************************************/
#define SPI_INSTANCE 2
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
static nrfx_spim_t s_spi = NRFX_SPIM_INSTANCE(SPI_INSTANCE);
static volatile bool s_spi_xfer_done = false;
/******************************************************
 *              Static Function Declarations
 ******************************************************/

/******************************************************
 *              Static Function Definitions
 ******************************************************/
static void s_close_spi(nrfx_spim_t const * const p_instance)
{
    NRF_SPIM_Type *p_spi = p_instance->p_reg;
    nrf_spim_disable(p_spi);
}

static void s_open_spi(nrfx_spim_t const * const p_instance)
{
    NRF_SPIM_Type *p_spi = p_instance->p_reg;
    nrf_spim_enable(p_spi);
}

static void s_spim_event_handler(nrfx_spim_evt_t const *p_event, void *p_context)
{
    s_spi_xfer_done = true;
}
/******************************************************
 *               Function Definitions
 ******************************************************/
void *port_spi_init(void)
{
    nrfx_spim_config_t spi_config = NRFX_SPIM_DEFAULT_CONFIG;

    spi_config.frequency      = NRF_SPIM_FREQ_4M;
    spi_config.ss_pin         = LIS2DW12_SPI_CS;
    spi_config.miso_pin       = LIS2DW12_SPI_MISO;
    spi_config.mosi_pin       = LIS2DW12_SPI_MOSI;
    spi_config.sck_pin        = LIS2DW12_SPI_SCLK;
    spi_config.ss_active_high = false;
    spi_config.irq_priority   =(APP_IRQ_PRIORITY_MID - 2);
    nrfx_spim_init(&s_spi, &spi_config, s_spim_event_handler, NULL);

    return &s_spi;
}

void port_spi_uninit(void *handle)
{
    nrfx_spim_uninit(handle);
}

int32_t port_spi_write(void *handle, uint8_t reg, uint8_t *buffer, uint16_t length)
{
    uint8_t m_length = 0;
    uint8_t m_tx_buf[length + 1];
    uint8_t m_rx_buf[length + 1];

    s_open_spi(handle);

    m_tx_buf[0] = reg;
    memcpy(&m_tx_buf[1], buffer, length);
    m_length = length + 1;

    s_spi_xfer_done = false;

    nrfx_spim_xfer_desc_t xfer_desc = NRFX_SPIM_XFER_TRX(m_tx_buf, m_length, m_rx_buf, m_length);

    if(nrfx_spim_xfer(handle, &xfer_desc, 0) != NRFX_SUCCESS)
    {
        return -1;
    }

    while (!s_spi_xfer_done);

    s_close_spi(handle);

    return 0;
}

int32_t port_spi_read(void *handle, uint8_t reg, uint8_t *buffer, uint16_t length)
{
    uint8_t m_length = 0;
    uint8_t m_tx_buf[length + 1];
    uint8_t m_rx_buf[length + 1];

    s_open_spi(handle);

    m_tx_buf[0] = reg | 0x80;
    memset(&m_tx_buf[1], 0, length);
    m_length = length + 1;

    s_spi_xfer_done = false;

    nrfx_spim_xfer_desc_t xfer_desc = NRFX_SPIM_XFER_TRX(m_tx_buf, m_length, m_rx_buf, m_length);

    if(nrfx_spim_xfer(handle, &xfer_desc, 0) != NRFX_SUCCESS)
    {
        return -1;
    }

    while (!s_spi_xfer_done);

    memcpy(buffer, &m_rx_buf[1], length);

    s_close_spi(handle);

    return 0;
}
