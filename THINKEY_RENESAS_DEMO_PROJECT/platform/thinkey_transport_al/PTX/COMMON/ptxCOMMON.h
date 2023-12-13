/** \file
   ---------------------------------------------------------------
   Copyright (C) 2021. Panthronics AG - All Rights Reserved.

   This material may not be reproduced, displayed, modified or
   distributed without the express prior written permission of the
   Panthronics AG.

   PLEASE REFER TO OUR DISCLAIMER NOTICE IN THE PROJECT ROOT.
   ---------------------------------------------------------------

    Project     : PTX100R
    Module      : COMMON API
    File        : ptxCOMMON.h

    Description : Common API for common functions.
*/

/**
 * \addtogroup grp_ptx_api_common Common API
 *
 * @{
 */

#ifndef APIS_PTX_COMMON_H_
#define APIS_PTX_COMMON_H_

/*
 * ####################################################################################################################
 * INCLUDES
 * ####################################################################################################################
 */

#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "ptxStatus.h"

#ifdef __cplusplus
extern "C" {
#endif
/*
 * ####################################################################################################################
 * DEFINES
 * ####################################################################################################################
 */

/**
 * Command line input parameters for main.
 */
#define MAX_NUMBER_OF_IN_PARAMETERS     5

/**
 * Define Tx- and Rx-buffer size for the data exchanged via RF.
 * Tx- and Rx-buffers data is sent/received with API IoTRd_Data_Exchange.
 */
#define TX_BUFFER_SIZE                          (266u)                          /**< TX-Buffer. Length */
#define RX_BUFFER_SIZE                          (266u)                          /**< RX-Buffer. Length */
#define NDEF_BUFFER_SIZE                        (256u)                          /**< NDEF-Buffer. Length */


/**
 * Maximum number of characters to be printed per line.
 */
#define LINE_LENGTH                             (80u)                           /**< Maximum number of characters to be printed per line. */

/*
 * ####################################################################################################################
 * TYPES
 * ####################################################################################################################
 */

/**
 * Command line parameters.
 */
typedef struct ptxCmdLineParameters
{
    uint8_t interfaceType;
    char *deviceName;
    char *nvmFilePath;
    uint8_t useNDEF;
    uint8_t tResetDelayMSPending;
    uint32_t tResetDelayMS;
} ptxCmdLineParameters_t;


/**
 * Possible values for interfaceType input parameter, used for stack initialization.
 */
typedef enum ptx_InterfaceType
{
    ptx_IntfType_None=0,
    ptx_IntfType_Uart,
    ptx_IntfType_Spi,
    ptx_IntfType_I2c,
    ptx_IntfType_TotalNumber,
    ptx_IntfType_Undefined
}ptx_InterfaceType_t;

/**
 * \brief Structure containing stack initialization parameters directly set by the user.
 *
 * Parameter description is given in detail within the description of Pos_Init_Stack API function.
 *
 */
typedef struct ptx_InitStack_Params
{
    uint8_t         interfaceType;
    char            *deviceName;
    uint32_t        deviceSpeed;
    uint32_t        deviceGpioNum;
    char            *deviceFsPath;
}ptx_InitStack_Params_t;


/*
 * ####################################################################################################################
 * API FUNCTIONS
 * ####################################################################################################################
 */

/*
 * Target-platform specific PrintF Helper-function
 */
void ptxCommon_PrintF(const char *format, ...);


/*
 * Helper function to print buffer data in hex format, starting from the offset and length defined by input parameters.
 * Add new line after printing the data if addNewLine not 0.
 */
void ptxCommon_Print_Buffer(uint8_t *buffer, uint32_t bufferOffset, uint32_t bufferLength, uint8_t addNewLine, uint8_t printASCII);

/*
 * Helper Function to print Application status values with error-codes
 */
void ptxCommon_PrintStatusMessage(const char *message, ptxStatus_t st);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* Guard */
