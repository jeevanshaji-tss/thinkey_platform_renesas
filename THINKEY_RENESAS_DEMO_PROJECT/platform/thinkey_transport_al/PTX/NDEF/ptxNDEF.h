/** \file
   ---------------------------------------------------------------
   Copyright (C) 2021. Panthronics AG - All Rights Reserved.

   This material may not be reproduced, displayed, modified or
   distributed without the express prior written permission of the
   Panthronics AG.

   PLEASE REFER TO OUR DISCLAIMER NOTICE IN THE PROJECT ROOT.
   ---------------------------------------------------------------

    Project     : PTX100R
    Module      : Generic NDEF OPERATION API
    File        : ptxNDEF.h

    Description : Generic NDEF Operation API (IOT READER - Extension)
*/

/**
 * \addtogroup grp_ptx_api_ndef_op Generic NDEF Operation API
 *
 * @{
 */

#ifndef APIS_PTX_NDEF_H_
#define APIS_PTX_NDEF_H_

/*
 * ####################################################################################################################
 * INCLUDES
 * ####################################################################################################################
 */

#include <stdint.h>
#include "ptx_IOT_READER.h"
#include "ptxNDEF_T2TOP.h"
#include "ptxNDEF_T3TOP.h"
#include "ptxNDEF_T4TOP.h"
#include "ptxNDEF_T5TOP.h"
#include "ptxStatus.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ####################################################################################################################
 * DEFINES / TYPES
 * ####################################################################################################################
 */

/*
 * ####################################################################################################################
 * TYPES
 * ####################################################################################################################
 */
struct ptxIoTRd_CardRegistry;

/**
 * \brief NDEF OP Initialization Parameters
 */
typedef struct ptxNDEF_InitParams
{
    ptxIoTRd_t                      *IotRd;             /**< IOT Reader provided by upper layer */
    uint8_t                         *TxBuffer;          /**< Internal Tx-Buffer provided by upper layer */
    uint32_t                        TxBufferSize;       /**< Size of Internal TX-Buffer */
    uint8_t                         *RxBuffer;          /**< Internal Rx-Buffer provided by upper layer */
    uint32_t                        RxBufferSize;       /**< Size of Internal Rx-Buffer */

} ptxNDEF_InitParams_t;

/**
 * \brief Generic NDEF OP Component
 */
typedef struct ptxNDEF
{
    /* Components */
    ptxStatus_Comps_t               CompId;                /**< Component Id */

    struct ptxIoTRd_CardRegistry    *CardRegistry;          /**< Reference to Internal Card Registry */
    ptxNDEF_T2TOP_t                 T2TOP;                  /**< T2T Operation Component */
    ptxNDEF_T3TOP_t                 T3TOP;                  /**< T3T Operation Component */
    ptxNDEF_T4TOP_t                 T4TOP;                  /**< T4T Operation Component */
    ptxNDEF_T5TOP_t                 T5TOP;                  /**< T5T Operation Component */

} ptxNDEF_t;

/*
 * ####################################################################################################################
 * API FUNCTIONS
 * ####################################################################################################################
 */

/**
 * \brief Initialize / Open the NDEF OP Component.
 *
 * \param[in]   ndefComp            Pointer to an existing instance of the T5T-OP component.
 * \param[in]   initParams          Pointer to initialization parameters.
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNDEF_Open (ptxNDEF_t *ndefComp, ptxNDEF_InitParams_t *initParams);

/**
 * \brief Formats a given Tag to INITIALIZED state.
 *
 * \param[in]   ndefComp           Pointer to an existing instance of the T5T-OP component.
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNDEF_FormatTag (ptxNDEF_t *ndefComp);

/**
 * \brief Checks if a NDEF-message is present on the given Tag (or not).
 *
 * \param[in]   ndefComp           Pointer to an existing instance of the T5T-OP component.
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNDEF_CheckMessage (ptxNDEF_t *ndefComp);

/**
 * \brief Reads a NDEF-message from a given Tag.
 *
 * \param[in]     ndefComp            Pointer to an existing instance of the T5T-OP component.
 * \param[in]     msgBuffer           Pointer to buffer holding the read NDEF-message.
 * \param[in,out] msgLen              Size of the buffer (in), Length of the read NDEF-message (out).
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNDEF_ReadMessage (ptxNDEF_t *ndefComp, uint8_t *msgBuffer, uint32_t *msgLen);

/**
 * \brief Writes a NDEF-message onto a given Tag.
 *
 * \param[in] ndefComp            Pointer to an existing instance of the T5T-OP component.
 * \param[in] msgBuffer           Pointer to buffer holding the NDEF-message to write.
 * \param[in] msgLen              Size of NDEF-message.
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNDEF_WriteMessage (ptxNDEF_t *ndefComp, uint8_t *msgBuffer, uint32_t msgLen);

/**
 * \brief Puts a Tag into READ-ONLY state (Attention: This is a irreversible Operation!).
 *
 * \param[in]   ndefComp           Pointer to an existing instance of the T5T-OP component.
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNDEF_LockTag (ptxNDEF_t *ndefComp);

/**
 * \brief Unitialize / Close the NDEF OP Component
 *
 * \param[in]   ndefComp           Pointer to an existing instance of the T5T-OP component.
 *
 * \return Status, indicating whether the operation was successful. See \ref ptxStatus_t.
 */
ptxStatus_t ptxNDEF_Close (ptxNDEF_t *ndefComp);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* Guard */

