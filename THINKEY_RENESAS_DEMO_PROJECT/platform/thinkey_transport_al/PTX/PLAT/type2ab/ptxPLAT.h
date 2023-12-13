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
    File        : ptxPLAT.h

    Description :
*/

/**
 * \addtogroup grp_ptx_api_plat PTX NSC Platform API
 *
 * @{
 */

#ifndef COMPS_PLAT_PTXPLAT_H_
#define COMPS_PLAT_PTXPLAT_H_


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
 * DEFINES / TYPES
 * ####################################################################################################################
 */

/**
 * Forward declaration of Plat
 */
struct ptxPlat;

/**
 * Forward declaration of Timer
 */
struct ptxPlatTimer;

/**
 * \brief Callback function for IRQ triggered (asynchronously).
 *
 */
typedef void (*pptxPlat_RxCallBack_t) (void *ctx);

/**
 * \brief Callback function for Timer Interrupt.
 *
 */
typedef void (*pptxPlat_TimerCallBack_t) (void *ctx);

/**
 * \brief Platform-specific parameter initializers.
 */
typedef struct ptxPlat_ConfigPars
{
    uint32_t    baudRate;         /**< Baud rate */
    uint8_t     flowControl;      /**< Flow control */
}ptxPlat_ConfigPars_t;


/*
 * ####################################################################################################################
 * API FUNCTIONS (Mandatory Implementation)
 * ####################################################################################################################
 */

/**
 * \brief Allocate and initialize the PLAT context.
 *
 * \note This function is in charge of provide back through /ref plat an initialize Platform Context.
 * Allocation of software resources and configuration of HW resources for the specific CPU,
 * shall be performed by this function.
 *
 * \note This function shall be successfully executed before any other call to the functions in this module.
 *
 * \param[in,out]       plat             Pointer to pointer where the allocated and initialized platform context is going to be provided.
 * \param[in]           initParams       Initializers for platform context parameters, e.g. com interface speed and flow control.
 *
 * \return Status, indicating whether the operation was successful. See \ref ptxStatus_t.
 */
ptxStatus_t ptxPLAT_AllocAndInit(struct ptxPlat **plat, struct ptxPlat_ConfigPars *initParams);


/**
 * \brief De-initialize the PLAT context.
 *
 * This function deallocates and de-init the resources used by the Platform Component that
 * were previously initialized by /ref ptxPLAT_AllocAndInit.
 *
 * \param[in]      plat        Pointer to an initialized component structure.
 *
 * \return Status, indicating whether the operation was successful. See \ref ptxStatus_t.
 */
ptxStatus_t ptxPLAT_Deinit(struct ptxPlat *plat);


/**
 * \brief Send and receive content of multiple buffers through the physical interface to the PTX100R.
 *
 * \note The function transmits first the transmission buffers and then receive the buffers for receiving.
 *
 * Sends the content of multiple buffers. In an implementation-specific way, the function concatenates sending of content of
 * multiple buffers. The implementation is hardware-specific, but the interface is such that the
 * underlying code can be tweaked for maximum performance (e.g. Gather-DMA on some embedded systems, of just
 * multi-send on others). \n
 * The receive function returns the bytes received during I/O. Some interfaces have additional requirements
 * like having to keep CS asserted or to apply repeated a start condition. The Exchange function allows for
 * requirements like these.
 *
 * \param[in]      plat             Pointer to an initialized component structure.
 * \param[in]      txBuf            Array of pointers to buffers with data to write.
 * \param[in]      txLen            Array of Number of bytes in the respective transmit buffer.
 * \param[in]      numTxBuffers     Number of buffers whose data to transmit.
 * \param[in,out]  rxBuf            Array of pointers to buffers where data is going to be received.
 * \param[in,out]  rxLen            Array of length of /rxBuf. As input parameter capacity of the buffers and as output actual number of bytes read out.
 * \param[in]      numRxBuffers     Number of buffers with data to be received.
 *
 *
 * \note If txBuf or txLen are NULL, then only receive operation is performed (if rx buffer and length are not NULL).
 *       For transmitting: individual buffers may be NULL or the length of individual buffers may be zero. It is required however
 *       that the total number of bytes to transfer is greater than zero and within the range the \b length \b field
 *       of the NSC-specific frame can transfer.
 *
 * \return Status, indicating whether the operation was successful. See \ref ptxStatus_t.
 *
 */
ptxStatus_t ptxPLAT_TRx(struct ptxPlat *plat, uint8_t *txBuf[], size_t txLen[], size_t numTxBuffers, uint8_t *rxBuf[], size_t *rxLen[], size_t numRxBuffers);


/**
 * \brief Wait for any kind of interrupt to be triggered on the CPU. (Blocking)
 *
 * This function is expected to be used is expected to be used by upper layers for waiting for any asynchronous event to happen
 *
 * \return Status, indicating whether the operation was successful. See \ref ptxStatus_t.
 */
ptxStatus_t ptxPLAT_WaitForInterrupt(struct ptxPlat *plat);


/**
 * \brief Start waiting for IRQ to be triggered.(Not-blocking)
 *
 * This function registers a CallBack function to be called by the platform (e.g. typically from ISR) when the IRQ
 * has been triggered by the PTX100R.
 *
 * \param[in]           plat        Pointer to an initialized component structure.
 * \param[in]           irqCb       Callback function to be called from HW (PLatform) when the IRQ is triggered by PTX100R.
 * \param[in]           ctxIrqCb    Context to be used as first argument when invoking the callback.
 *
 * \return Status, indicating whether the operation was successful. See \ref ptxStatus_t.
 */
ptxStatus_t ptxPLAT_StartWaitForRx(struct ptxPlat *plat, pptxPlat_RxCallBack_t irqCb, void *ctxIrqCb);


/**
 * \brief Stop the waiting for IRQ
 *
 * This function stops the asynchronous wait for IRQ triggered by /ref ptxPLAT_StartWaitForIRQ.
 *
 * \param[in]           plat        Pointer to an initialized component structure.
 *
 * \return Status, indicating whether the operation was successful. See \ref ptxStatus_t.
 */
ptxStatus_t ptxPLAT_StopWaitForRx(struct ptxPlat *plat);


/**
 * \brief Sleep software execution
 *
 * This function returns when the time provided by /ref sleep_ms has elapsed.
 *
 * \param[in]           plat        Pointer to an initialized component structure.
 * \param[in]           sleep_ms    Number of milliseconds to sleep.
 *
 * \note It si expected that this function provides less error than 5% in the range of 1 - 1000 ms /ref sleep_ms
 *
 * \return Status, indicating whether the operation was successful. See \ref ptxStatus_t.
 */
ptxStatus_t ptxPLAT_Sleep(struct ptxPlat *plat, uint32_t sleep_ms);


/**
 * \brief Disable Interrupts
 *
 * This is used to disable IRQs from the underlying HW-platform.
 *
 * \param[in]           plat        Pointer to an initialized component structure.
 */
void ptxPLAT_DisableInterrupts (struct ptxPlat *plat);


/**
 * \brief Enable Interrupts
 *
 * This is used to (re-)enable IRQs from the underlying HW-platform.
 *
 * \param[in]           plat        Pointer to an initialized component structure.
 */
void ptxPLAT_EnableInterrupts ( struct ptxPlat *plat );

/**
 * \brief Resets PTX100R
 *
 * This is used to reset the PTX100R-HW via the SEN-pin (optional, if connected)
 *
 * \param[in]           plat        Pointer to an initialized component structure.
 *
 * \return Status, indicating whether the operation was successful. See \ref ptxStatus_t.
 */
ptxStatus_t ptxPLAT_ResetChip ( struct ptxPlat *plat );

/*
 * ####################################################################################################################
 * API FUNCTIONS (Timer)
 * ####################################################################################################################
 */


/**
 * \brief Get an initialized timer.
 *
 * \note This function is in charge of provide an available (e.g.not in use) Timer, and initialize it.
 *
 * \note This function shall be successfully executed before any other call to the functions in this module.
 *
 * \param[in]        plat              Pointer to an initialized component structure.
 * \param[out]       timer             Pointer to pointer where the allocated and initialized timer context is going to be provided.
 *
 * \return Status, indicating whether the operation was successful. See \ref ptxStatus_t.
 */
ptxStatus_t ptxPLAT_GetInitializedTimer(struct ptxPlat *plat, struct ptxPlatTimer **timer);


/**
 * \brief Start timer for some milliseconds.
 *
 * \note This function shall be able to be used on two different ways:
 *
 *        1./ It blocks and waits until the time elapses.
 *        2./ It does not block, It register a callback that will be called asyncrhously when the time elapses.
 *
 * \param[in]       plat              Pointer to an initialized component structure.
 * \param[in]       timer             Pointer to an initialized timer context.
 * \param[in]       ms                Milliseconds to wait.
 * \param[in]       isBlock           Set to 0 if the function is blocking. Set to 1 if it blocks.
 * \param[in]       fnISRCb           Callback function that may be used in case of not-blocking call. NULL to be provided if not-used.
 * \param[in]       ISRCxt            Context to be used by /ref fnISRCb. NULL to be provided if not-used.
 *
 * \return Status, indicating whether the operation was successful. See \ref ptxStatus_t.
 */
ptxStatus_t ptxPLAT_TimerStart(struct ptxPlat *plat, struct ptxPlatTimer *timer, uint32_t ms, uint8_t isBlock, pptxPlat_TimerCallBack_t fnISRCb, void *ISRCxt);


/**
 * \brief Get Elapse state of the timer.
 *
 * \note This function shall be called once that the timer has been started
 *
 * \param[in]       plat              Pointer to an initialized component structure.
 * \param[in]       timer             Pointer to an initialized timer context.
 * \param[out]      isElapsed         Pointer where the elapse state is going to be written.(e.g. 1 if timer is elapsed, 0 if timer is not elapsed)
 *
 * \return Status, indicating whether the operation was successful. See \ref ptxStatus_t.
 */
ptxStatus_t ptxPLAT_TimerIsElapsed(struct ptxPlat *plat, struct ptxPlatTimer *timer, uint8_t *isElapsed);


/**
 * \brief De initialize the timer.
 *
 * \note This function shall be called once that the timer has been used.
 *       It shall :
 *          1./ Stop the timer in case that is still in-use.
 *          2./ Release the timer to be available by the system.
 *
 * \param[in]       plat              Pointer to an initialized component structure.
 * \param[in]       timer             Pointer to an initialized timer context.
 *
 * \return Status, indicating whether the operation was successful. See \ref ptxStatus_t.
 */
ptxStatus_t ptxPLAT_TimerDeinit(struct ptxPlat *plat, struct ptxPlatTimer *timer);


/**
 * \brief It checks if Rx Operation is pending from PTX100X
 *
 * \param[in]       plat              Pointer to an initialized component structure.
 * \param[out]      isRxPending       Pointer to write if rx operation is pending. ( 1 if rx operation is pending, 0 if not )
 *
 * \return Status, indicating whether the operation was successful. See \ref ptxStatus_t.
 */
ptxStatus_t ptxPLAT_IsRxPending (struct ptxPlat *plat , uint8_t *isRxPending);


/**
 * \brief It triggers Rx Operation from PTX100X (If rx operation pending)
 *
 * \param[in]       plat              Pointer to an initialized component structure.
 *
 * \return Status, indicating whether the operation was successful. See \ref ptxStatus_t.
 */
ptxStatus_t ptxPLAT_TriggerRx(struct ptxPlat *plat);

/*
 * ####################################################################################################################
 * API FUNCTIONS (Communication interface)
 * ####################################################################################################################
 */

/**
 * \brief Set communication interface speed.
 *
 * \note Used only for Uart interface to update com interface speed after NSC init command.
 *
 * \param[in]       plat              Pointer to an initialized component structure.
 * \param[in]       baudrate          Expected baudrate, in bps.
 *
 * \return Status, indicating whether the operation was successful. See \ref ptxStatus_t.
 */
ptxStatus_t ptxPLAT_SetIntfSpeed(struct ptxPlat *plat, uint32_t baudrate);


/**
 * \brief Check if plat interface reception is taking place at the moment.
 *
 * \note The function is called in message reception loop after WFI -after CPU exits Sleep mode.
 * This is relevant for Uart interface since reception is interrupt driven (every byte received in ISR)
 * and race condition might occur while checking if message has been received. If there is Rx activity
 * on Uart, WFI doesn´t get called anymore in that loop.
 * For other interfaces this function call always returns 0 and CPU enters sleep mode in every iteration.
 *
 * \param[in]       plat              Pointer to an initialized component structure.
 *
 * \return Rx status: 0 - no rx activity, 1 - rx activity ongoing.
 */
uint8_t ptxPLAT_CheckRxActive(struct ptxPlat *plat);


#ifdef __cplusplus
}
#endif

#endif /* Guard */

/** @} */

