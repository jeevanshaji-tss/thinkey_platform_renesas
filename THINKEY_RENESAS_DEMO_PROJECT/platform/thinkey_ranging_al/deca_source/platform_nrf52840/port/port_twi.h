/*
 * Copyright 2021, Murata/Sychip Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Murata/Sychip Corporation,
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Murata/Sychip Corporation.
 */

#ifndef __PORT_TWI_H__
#define __PORT_TWI_H__


#include <stdint.h>
#include <stdbool.h>
#include "nrf_drv_twi.h"


#ifdef __cplusplus
extern "C" {
#endif /* end of __cplusplus */


/**
 * TWI handler struct.
 *
 * The variables to be used in TWI.
 */
typedef struct _twi_handler
{
    nrf_drv_twi_t twi;   /**< TWI handle. */
    uint8_t instance_id; /**< TWI instance ID. */
    uint8_t sub_addr;    /**< TWI address. */
    bool initialized;    /**< True: Already initialized, False: Not yet initialized. */
    bool open;           /**< True: Already opened, False: Not yet opened. */
} twi_handler_t;


/**
 * TWI port init function.
 *
 * Init TWI port.
 *
 * @note None.
 *
 * @return TWI handle.
 */
twi_handler_t *port_twi_init(void);

/**
 * TWI port uninit function.
 *
 * Uninit TWI port.
 *
 * @note None.
 *
 * @param[in] handle TWI handle point.
 */
void port_twi_uninit(twi_handler_t *handle);

/**
 * Set TWI address function.
 *
 * Set the address of the device.
 *
 * @note None.
 *
 * @param[in] handle TWI handle point.
 * @param[in] addr   The address of the device.
 */
void port_twi_set_address(twi_handler_t *handle, uint8_t addr);

/**
 * TWI port write function.
 *
 * Write data to the device via the TWI interface.
 *
 * @note None.
 *
 * @param[in] handle TWI handle point.
 * @param[in] reg    The address of the operation.
 * @param[in] buffer The pointer to the data buffer to be written.
 * @param[in] length The length of the data.
 * @return 0: success, -1: failed.
 */
int32_t port_twi_write(twi_handler_t *handle, uint8_t reg, uint8_t *buffer, uint16_t length);

/**
 * TWI port read function.
 *
 * Read data from the device via the TWI interface.
 *
 * @note None.
 *
 * @param[in]  handle TWI handle point.
 * @param[in]  reg    The address of the operation.
 * @param[out] buffer The pointer to the data buffer to be readed.
 * @param[in]  length The length of the data.
 * @return 0: success, -1: failed.
 */
int32_t port_twi_read(twi_handler_t *handle, uint8_t reg, uint8_t *buffer, uint16_t length);


#ifdef __cplusplus
}
#endif /* end of __cplusplus */


#endif /* end of __PORT_TWI_H__ */
