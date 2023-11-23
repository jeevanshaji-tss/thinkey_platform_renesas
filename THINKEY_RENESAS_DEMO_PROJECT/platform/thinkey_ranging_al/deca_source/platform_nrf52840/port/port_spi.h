/*
 * Copyright 2021, Murata/Sychip Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Murata/Sychip Corporation,
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Murata/Sychip Corporation.
 */

#ifndef __PORT_SPI_H__
#define __PORT_SPI_H__


#include <stdint.h>
#include "nrfx_spim.h"


#ifdef __cplusplus
extern "C"
{
#endif /* end of __cplusplus */


/**
 * SPI port write function.
 *
 * Write data to the device via the SPI interface.
 *
 * @note None.
 *
 * @param[in] handle SPI handle point.
 * @param[in] reg    The address of the operation.
 * @param[in] buffer The pointer to the data buffer to be written.
 * @param[in] length The length of the data.
 * @return 0: success, -1: failed.
 */
int32_t port_spi_write(void *handle, uint8_t reg, uint8_t *buffer, uint16_t length);

/**
 * SPI port read function.
 *
 * Read data from the device via the SPI interface.
 *
 * @note None.
 *
 * @param[in]  handle SPI handle point.
 * @param[in]  reg    The address of the operation.
 * @param[out] buffer The pointer to the data buffer to be readed.
 * @param[in]  length The length of the data.
 * @return 0: success, -1: failed.
 */
int32_t port_spi_read(void *handle, uint8_t reg, uint8_t *buffer, uint16_t length);

/**
 * SPI port init function.
 *
 * Init SPI port.
 *
 * @note None.
 *
 * @return SPI handle point.
 */
void *port_spi_init(void);

/**
 * SPI port init function.
 *
 * Init SPI port.
 *
 * @note None.
 *
 * @param[in] handle SPI handle point.
 */
void port_spi_uninit(void *handle);


#ifdef __cplusplus
} /* extern "C" */
#endif  /* end of __cplusplus */


#endif /* end of __PORT_SPI_H__ */
