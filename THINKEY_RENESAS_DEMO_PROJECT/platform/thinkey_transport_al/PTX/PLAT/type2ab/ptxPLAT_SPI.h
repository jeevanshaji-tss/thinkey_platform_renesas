/** \file
   ---------------------------------------------------------------
   Copyright (C) 2021. Panthronics AG - All Rights Reserved.

   This material may not be reproduced, displayed, modified or
   distributed without the express prior written permission of the
   Panthronics AG.

   PLEASE REFER TO OUR DISCLAIMER NOTICE IN THE PROJECT ROOT.
   ---------------------------------------------------------------

    Project     : PTX100x
    Module      : PLAT
    File        : ptxPLAT_SPI.h

    Description :
*/

#ifndef COMPS_PLAT_PTXPLAT_SPI_H_
#define COMPS_PLAT_PTXPLAT_SPI_H_


/*
 * ####################################################################################################################
 * INCLUDES
 * ####################################################################################################################
 */
#include "ptxStatus.h"
#include <stddef.h>
#include<stdbool.h>
#include "ptxPLAT.h"
#include "hal_data.h"
//#include "cyhal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ####################################################################################################################
 * DEFINES / TYPES
 * ####################################################################################################################
 */


/**
 * SPI port context.
 */
typedef struct ptxPLAT_SpiPort
{
    void const              *SpiInstance;       /**< Pointer to generated SPI interface instance. */
}ptxPLAT_SpiPort_t;

/**
 * Platform-specific SPI configuration parameters.
 */
typedef struct ptxPLAT_SpiConfigPars
{
    uint32_t                IntfSpeed;          /**< Clock speed in Hz to be configured. */
}ptxPLAT_SpiConfigPars_t;

/**
 * Platform-specific. SPI Main structure.
 */



typedef struct ptxPLAT_Spi
{
    uint32_t                IntfSpeed;      /**< Clock speed in Hz to be configured. */
    spi_instance_ctrl_t           pSPI_instance;  /* NRF spi context */
    pptxPlat_RxCallBack_t   RxCb;           /**< Rx Callback function. */
    void                    *CtxRxCb;       /**< Rx Callback Context. */
    volatile uint8_t        TransferState;  /**< Current state of the ongoing transfer. */
}ptxPLAT_Spi_t;

/*
 * ####################################################################################################################
 * API FUNCTIONS
 * ####################################################################################################################
 */


/**
 * \brief Get an initialized SPI Context.
 *
 * \note This function shall be successfully executed before any other call to the functions in this module.
 *       It initializes SPI hardware wise.
 *
 * \param[out]       spi              Pointer to pointer where the allocated and initialized SPI context is going to be provided.
 * \param[in]        spiPars          Configuration for SPI.
 *
 * \return Status, indicating whether the operation was successful. See \ref ptxStatus_t.
 */
ptxStatus_t ptxPLAT_SPI_GetInitialized(ptxPLAT_Spi_t **spi, ptxPLAT_SpiConfigPars_t *spiPars);


/**
 * \brief De initialize the SPI.
 *
 * \note This function shall be called once that the caller has finished with SPI.
 *
 * \param[in]       spi             Pointer to an initialized SPI context.
 *
 * \return Status, indicating whether the operation was successful. See \ref ptxStatus_t.
 */
ptxStatus_t ptxPLAT_SPI_Deinit(ptxPLAT_Spi_t *spi);


/**
 * \brief SPI transmit and receive function.
 *
 * \note Wrapper function for /ref ptxPLAT_TRx. See it for detailed description
 *
 * \param[in]       spi             Pointer to an initialized SPI context.
 * \param[in]       txBuf           Array of buffers to transmit.
 * \param[in]       txLen           Array of lengths of buffers to transmit.
 * \param[in]       numTxBuffers    Number of buffers to transmit.
 * \param[out]      rxBuf           Array of buffers to receive.
 * \param[in,out]   rxLen           Array of lengths of buffers to receive.
 * \param[in]       numTxBuffers    Number of buffers to receive.
 *
 * \return Status, indicating whether the operation was successful. See \ref ptxStatus_t.
 */
ptxStatus_t ptxPLAT_SPI_TRx(ptxPLAT_Spi_t *spi, uint8_t *txBuf[], size_t txLen[], size_t numTxBuffers, uint8_t *rxBuf[], size_t *rxLen[], size_t numRxBuffers);


/**
 * \brief Start waiting for IRQ to be triggered.(Not-blocking)
 *
 * This function registers a CallBack function to be called by the platform (e.g. typically from ISR) when the IRQ
 * has been triggered by the PTX100R.
 *
 * \param[in]           spi         Pointer to an initialized SPI context.
 * \param[in]           irqCb       Callback function to be called from HW (PLatform) when the IRQ is triggered by PTX100R.
 * \param[in]           ctxIrqCb    Context to be used as first argument when invoking the callback.
 *
 * \return Status, indicating whether the operation was successful. See \ref ptxStatus_t.
 */
ptxStatus_t ptxPLAT_SPI_StartWaitForRx(ptxPLAT_Spi_t *spi, pptxPlat_RxCallBack_t irqCb, void *ctxIrqCb);


/**
 * \brief Stop the waiting for IRQ
 *
 * This function stops the asynchronous wait for IRQ triggered by /ref ptxPLAT_StartWaitForIRQ.
 *
 * \param[in]           spi         Pointer to an initialized SPI context.
 *
 * \return Status, indicating whether the operation was successful. See \ref ptxStatus_t.
 */
ptxStatus_t ptxPLAT_SPI_StopWaitForRx(ptxPLAT_Spi_t *spi);

ptxStatus_t ptxPLAT_SPI_TriggerRx(ptxPLAT_Spi_t *spi);

bool ptxPLAT_GPIO_IsRxPending(ptxPLAT_Spi_t *spi);

#ifdef __cplusplus
}
#endif

#endif /* Guard */

