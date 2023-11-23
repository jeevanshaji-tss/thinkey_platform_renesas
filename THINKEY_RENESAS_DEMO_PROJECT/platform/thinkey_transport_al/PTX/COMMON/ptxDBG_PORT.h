/** \file
   ---------------------------------------------------------------
   Copyright (C) 2021. Panthronics AG - All Rights Reserved.

   This material may not be reproduced, displayed, modified or
   distributed without the express prior written permission of the
   Panthronics AG.

   PLEASE REFER TO OUR DISCLAIMER NOTICE IN THE PROJECT ROOT.
   ---------------------------------------------------------------

    Project     : PTX100x
    Module      : DEBUG_PORT
    File        : ptxDBG_PORT.h

    Description : definitions for debug port
*/

#ifndef STACK_COMPS_DEBUG_PORT_PTXDBG_PORT_H_
#define STACK_COMPS_DEBUG_PORT_PTXDBG_PORT_H_


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

/** If application wants to handle host requests, a callback will be registered. */
//#define DBGPORT_HOSTRQ_HANDLER_REGISTERED


#define PTX_DBGPORT_DEFAULT_UART_BAUDRATE   (115200u)
#define PTX_DBGPORT_MAX_UART_BAUDRATE_ERROR (3000)
#define PTX_DBGPORT_MAX_TX_BUFF_SIZE        (256U)

typedef void (*ptxHostRequestHandler)(void *, uint32_t);

/*
 * ####################################################################################################################
 * API FUNCTIONS
 * ####################################################################################################################
 */

/**
 * \brief Opens / Initializes the debug port.
 *
 * \return Status, indicating whether the operation was successful.
 */
uint16_t ptxDBGPORT_Open(void);

/**
 * \brief Closes / Uninitializes the debug port.
 *
 * \return Status, indicating whether the operation was successful.
 */
uint16_t ptxDBGPORT_Close(void);

/**
 * \brief Write message on debug port.
 *
 * \note The function internally checks if the port has been initialized and it calls local init function if needed.
 *
 * \param[in]       message         Data to be sent to the host.
 *
 * \return Status, indicating whether the operation was successful.
 */
uint16_t ptxDBGPORT_Write(char *message);

/**
 * \brief Write buffer content on debug port.
 *
 * \note The function internally checks if the port has been initialized and it calls local init function if needed.
 *
 * \param[in]       buffer         Data to be sent to the host.
 * \param[in]       bufferLen      Length of data.
 *
 * \return Status, indicating whether the operation was successful.
 */
uint16_t ptxDBGPORT_Write_Buffer(uint8_t *buffer, uint32_t bufferLen);

/**
 * \brief Returns the number of available bytes in the buffer.
 *
 * \return Number of available bytes.
 */
uint32_t ptxDBGPORT_Get_Available_Bytes(void);

/**
 * \brief Reads buffer content.
 *
 * \param[in]       buffer         Destination buffer.
 * \param[in]       bufferOffset   Destination buffer offset.
 *
 * \return Status, indicating whether the operation was successful.
 */
uint16_t ptxDBGPORT_Read_Buffer_(uint8_t *buffer, uint32_t bufferOffset);

/**
 * \brief Resets the (internal) buffer states.
 *
 */
void ptxDBGPORT_Reset_FIFO_Level(void);

#ifdef  DBGPORT_HOSTRQ_HANDLER_REGISTERED
/**
 * \brief Register host request reception callback.
 *
 * \note Typically, if the application wants to receive commands/data over debug interface, it should register a function
 *       to parse received data.
 *
 * \param[in]       cb              Callback function.
 *
 * \return Status, indicating whether the operation was successful.
 */
uint16_t ptxDBGPORT_RegisterCB(void *cb);
#endif


#ifdef __cplusplus
}
#endif

#endif /* Guard */

