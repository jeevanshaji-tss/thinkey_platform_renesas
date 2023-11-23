/** \file
   ---------------------------------------------------------------
   Copyright (C) 2021. Panthronics AG - All Rights Reserved.

   This material may not be reproduced, displayed, modified or
   distributed without the express prior written permission of the
   Panthronics AG.

   PLEASE REFER TO OUR DISCLAIMER NOTICE IN THE PROJECT ROOT.
   ---------------------------------------------------------------

    Project     : PTX100R
    Module      : NATIVE TAG API
    File        : ptxNativeTag_T4T.h

    Description : Native Tag API for NFC Forum Tag Type 4 (IOT READER - Extension)
*/

/**
 * \addtogroup grp_ptx_api_native_tag_T4T Native Tag T4T API
 *
 * @{
 */

#ifndef APIS_PTX_NATIVE_TAG_T4T_H_
#define APIS_PTX_NATIVE_TAG_T4T_H_

/*
 * ####################################################################################################################
 * INCLUDES
 * ####################################################################################################################
 */

#include <stdint.h>
#include "ptxStatus.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ####################################################################################################################
 * DEFINES / TYPES
 * ####################################################################################################################
 */

/**
 * \brief T4T Tag specific definitions
 */
#define PTX_T4T_MIN_TX_BUFFER_SIZE   (uint32_t)256

/*
 * ####################################################################################################################
 * TYPES
 * ####################################################################################################################
 */

/**
 * \brief T4T Native Tag Initialization Parameters
 */
typedef struct ptxNativeTag_T4T_InitParams
{
    ptxIoTRd_t          *IotRd;             /**< Allocated instance of IoT-Reader component (see ptxIoTRd_Allocate_Stack()) */
    uint8_t             *TxBuffer;          /**< Internal Tx-Buffer provided by upper layer */
    uint32_t             TxBufferSize;      /**< Size of TX Buffer */

} ptxNativeTag_T4T_InitParams_t;

/**
 * \brief T4T CAPDU fields struct
 */
typedef struct ptxNativeTag_T4T_CommandAPDUFields
{
    uint8_t             data_length_field[3];               /**< Lc field */
    uint8_t             nbr_data_length_bytes;              /**< length of Lc field */
    uint16_t            nbr_data_bytes;                     /**< length of data field */
    uint8_t             expected_length_field[3];           /**< Le field */
    uint8_t             nbr_expected_length_bytes;          /**< length of Le field */
    uint8_t             ber_tlv_data_field[5];              /**< ber-tlv container */
} ptxNativeTag_T4T_CommandAPDUFields_t;

/**
 * \brief T4T Native Tag Component
 */
typedef struct ptxNativeTag_T4T
{
    /* Components */
    ptxStatus_Comps_t                       CompId;         /**< Component Id */
    ptxIoTRd_t                              *IotRd;         /**< IOT Reader provided by upper layer */
    uint8_t                                 *TxBuffer;      /**< Internal Tx-Buffer provided by upper layer (may be shared with other components) */
    ptxNativeTag_T4T_CommandAPDUFields_t    Fields;         /**< fields used in command ADPU */

} ptxNativeTag_T4T_t;

/*
 * ####################################################################################################################
 * API FUNCTIONS
 * ####################################################################################################################
 */

/**
 * \brief Initialize / Open the T4T Native Tag Component.
 *
 * \param[in]   t4tComp             Pointer to an existing instance of the T4T component.
 * \param[in]   initParams          Pointer to initialization parameters.
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNativeTag_T4TOpen (ptxNativeTag_T4T_t *t4tComp, ptxNativeTag_T4T_InitParams_t *initParams);

/**
 * \brief Select command for Native Tag T4T
 * \param[in]   t4tComp                 Pointer to an existing instance of the T4T component.
 * \param[in]   paramByte1              P1 of command APDU
 * \param[in]   paramByte2              P2 of command APDU
 * \param[in]   data                    Pointer to data field
 * \param[in]   nbrDataBytes            Number of data bytes
 * \param[in]   expectedResponseLen     Number of response bytes expected
 * \param[in]   rx                      Pointer to read buffer
 * \param[in,out]   rxLen               Size of Rx-buffer (in), Length of the received data (out).
 * \param[in]   msTimeout               Timeout value
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNativeTag_T4TSelect (ptxNativeTag_T4T_t *t4tComp,
                                                        uint8_t paramByte1,
                                                        uint8_t paramByte2,
                                                        uint8_t *data,
                                                        uint8_t nbrDataBytes,
                                                        uint8_t expectedResponseLen,
                                                        uint8_t *rx,
                                                        uint32_t *rxLen,
                                                        uint32_t msTimeout);
/**
 * \brief Read Binary command for Native Tag T4T
 * \param[in]   t4tComp                 Pointer to an existing instance of the T4T component.
 * \param[in]   offset                  Data offset
 * \param[in]   nbrExpectedBytes        Number of expected bytes
 * \param[in]   rx                      Pointer to read buffer
 * \param[in,out]   rxLen               Size of Rx-buffer (in), Length of the received data (out).
 * \param[in]   msTimeout               Timeout value
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNativeTag_T4TReadBinary (ptxNativeTag_T4T_t *t4tComp,
                                                        uint16_t offset,
                                                        uint8_t nbrExpectedBytes,
                                                        uint8_t *rx,
                                                        uint32_t *rxLen,
                                                        uint32_t msTimeout);

/**
 * \brief Read Binary command for Native Tag T4T
 * \param[in]   t4tComp                 Pointer to an existing instance of the T4T component.
 * \param[in]   offset                  Data offset
 * \param[in]   nbrExpectedBytes        Number of expected bytes
 * \param[in]   rx                      Pointer to read buffer
 * \param[in,out]   rxLen               Size of Rx-buffer (in), Length of the received data (out).
 * \param[in]   msTimeout               Timeout value
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNativeTag_T4TReadBinaryODO (ptxNativeTag_T4T_t *t4tComp,
                                                        uint32_t offset,
                                                        uint8_t nbrExpectedBytes,
                                                        uint8_t *rx,
                                                        uint32_t *rxLen,
                                                        uint32_t msTimeout);

/**
 * \brief Update Binary command for Native Tag T4T
 * \param[in]   t4tComp                 Pointer to an existing instance of the T4T component.
 * \param[in]   offset                  Data offset
 * \param[in]   data                    Pointer to the data field
 * \param[in]   nbrDataBytes            Number of data bytes
 * \param[in]   rx                      Pointer to read buffer
 * \param[in,out]   rxLen               Size of Rx-buffer (in), Length of the received data (out).
 * \param[in]   msTimeout               Timeout value
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNativeTag_T4TUpdateBinary (ptxNativeTag_T4T_t *t4tComp,
                                                        uint16_t offset,
                                                        uint8_t *data,
                                                        uint8_t nbrDataBytes,
                                                        uint8_t *rx,
                                                        uint32_t *rxLen,
                                                        uint32_t msTimeout);

/**
 * \brief Update Binary command for Native Tag T4T
 * \param[in]   t4tComp                 Pointer to an existing instance of the T4T component.
 * \param[in]   offset                  Data offset
 * \param[in]   data                    Pointer to the data field
 * \param[in]   nbrDataBytes            Number of data bytes
 * \param[in]   rx                      Pointer to read buffer
 * \param[in,out]   rxLen               Size of Rx-buffer (in), Length of the received data (out).
 * \param[in]   msTimeout               Timeout value
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNativeTag_T4TUpdateBinaryODO (ptxNativeTag_T4T_t *t4tComp,
                                                        uint32_t offset,
                                                        uint8_t *data,
                                                        uint8_t nbrDataBytes,
                                                        uint8_t *rx,
                                                        uint32_t *rxLen,
                                                        uint32_t msTimeout);

/**
 * \brief Uninitialize / Close the T4T Native Tag Component.
 *
 * \param[in]   t4tComp             Pointer to an existing instance of the T4T component.
 *
 * \return Status, indicating whether the operation was successful. See \ref ptxStatus_t.
 */
ptxStatus_t ptxNativeTag_T4TClose (ptxNativeTag_T4T_t *t4tComp);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* Guard */

