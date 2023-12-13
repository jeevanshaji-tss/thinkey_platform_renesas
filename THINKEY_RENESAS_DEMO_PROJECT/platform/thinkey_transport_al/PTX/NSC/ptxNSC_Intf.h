/** \file
   ---------------------------------------------------------------
   Copyright (C) 2021. Panthronics AG - All Rights Reserved.

   This material may not be reproduced, displayed, modified or
   distributed without the express prior written permission of the
   Panthronics AG.

   PLEASE REFER TO OUR DISCLAIMER NOTICE IN THE PROJECT ROOT.
   ---------------------------------------------------------------

    Project     : PTX100x
    Module      : NSC
    File        : ptxNSC_Intf.h

    Description :
*/

/**
 * \addtogroup grp_ptx_api_nsc_hal PTX NSC Stack HAL
 *
 * @{
 */

#ifndef COMPS_NSC_PTXNSC_INTF_H_
#define COMPS_NSC_PTXNSC_INTF_H_


/*
 * ####################################################################################################################
 * INCLUDES
 * ####################################################################################################################
 */
#include "ptxStatus.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 * ####################################################################################################################
 * DEFINES / TYPES / INTERNALS
 * ####################################################################################################################
 */

/**
 * Forward declaration of NSC Component
 */
struct ptxNSC;


/*
 * ####################################################################################################################
 * FUNCTIONS
 * ####################################################################################################################
 */

/**
 * \brief Asynchronous reception callback.
 *
 * This function is called from ISR. Typical usage e.g. in GPIO IRQ for SPI, or in Rx ISR for Uart.
 * It performs reading of PTX buffer to get NSC notification data.
 *
 * \param[in]   nscCtx           Pointer to NSC Context.
 *
 * \return No status returned.
 */
void ptxNSC_GetRx (struct ptxNSC *nscCtx);


#if defined (UART_INTF)
/**
 * \brief Get actual baudrate out of Uart config parameters used in NSC_INIT_CMD.
 *
 * This function is called in \ref ptxNSC_InitCmd in order to set Uart interface baudrate after successful
 * NSC_INIT_CMD/RSP.
 *
 * \param[in]   nscCtx           Pointer to NSC Context.
 * \param[out]  baudRate         Holder for returned baudrate value.
 * \param[in]   uartConfig       Array of 2 Uart config parameters, corresponding to a predefined baudrate.
 *
 * \return No status returned.
 */
void ptxNSC_GetUartBaudRateFromConfigParams(struct ptxNSC *nscCtx, uint32_t *baudRate, uint8_t *uartConfig);

/**
 * \brief Set Rx control parameters to initial values.
 *
 * This is typically called before starting Tx operation, so that asynchronous Rx operation after Tx could
 * start with expected state.
 *
 * \param[in]   nscCtx           Pointer to NSC Context.
 *
 * \return Status, indicating whether the operation was successful.See \ref ptxStatus_t.
 */
ptxStatus_t ptxNSC_SetCleanStateRx (struct ptxNSC *nscCtx);

/**
 * \brief Synchronize host and PTX communication interface speed.
 *
 * This is typically called before the call to reset NSC. There might be the case where the host uart interface
 * changes speed although the speed on NSC chip uart interface hasn´t changed. Use this API to set proper speed on
 * the host interface in order to continue with NSC initialization.
 *
 * \param[in]   nscCtx           Pointer to NSC Context.
 *
 * \return Status, indicating whether the operation was successful.See \ref ptxStatus_t.
 */
ptxStatus_t ptxNSC_UartComSync(struct ptxNSC *nscCtx);

/**
 * \brief Set default uart speed for the host.
 *
 * This is typically called after NSC reset has been performed.
 * After reset the host interface will operate on the default uart baudrate. However, there might happen that
 * after the call to \ref ptxNSC_UartComSync host uart speed is not the default one.
 * Call this API then to set the default uart speed.
 *
 * \param[in]   nscCtx           Pointer to NSC Context.
 *
 * \return Status, indicating whether the operation was successful.See \ref ptxStatus_t.
 */
ptxStatus_t ptxNSC_UartSetDefaultSpeed(struct ptxNSC *nscCtx);

#endif


#ifdef __cplusplus
}
#endif

#endif /* Guard */

/** @} */

