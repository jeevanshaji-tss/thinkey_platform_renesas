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
    File        : ptxPLAT_INT.h

    Description :
*/

/**
 * \addtogroup grp_ptx_api_plat_int PTX NSC Platform API Internal
 *
 * @{
 */

#ifndef COMPS_PLAT_PTXPLAT_INT_H_
#define COMPS_PLAT_PTXPLAT_INT_H_

#include "ptxPLAT_TIMER.h"
#include "ptxStatus.h"
#include "ptxPLAT.h"

#if defined (UART_INTF)
    #include "ptxPLAT_UART.h"
#else
    #include "ptxPLAT_SPI.h"
#endif

#if defined (POLLER_BOARD)
    #include "ptxPLAT_POLLER_BOARD.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Platform-specific configuration parameters.
 */
typedef struct ptxPlat
{
        ptxStatus_Comps_t           CompId;             /**< Component Id. */

#if defined (UART_INTF)
        ptxPlatUart_t               *Uart;              /**< Pointer to UART Context. Platform dependent. */
#else
        ptxPLAT_Spi_t               *Spi;               /**< Pointer to SPI Context. Platform dependent. */
#endif
        pptxPlat_RxCallBack_t       RxCb;               /**< Callback function */
        void                        *CtxRxCb;           /**< Callback Context */

        uint16_t                    IRQDisabledCnt;     /**< IRQ Enable/Disable Protection */

}ptxPlat_t;

#ifdef __cplusplus
}
#endif

#endif /* Guard */

/** @} */

