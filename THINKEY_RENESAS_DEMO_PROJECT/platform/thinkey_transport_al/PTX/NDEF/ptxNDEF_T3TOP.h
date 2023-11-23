/** \file
   ---------------------------------------------------------------
   Copyright (C) 2021. Panthronics AG - All Rights Reserved.

   This material may not be reproduced, displayed, modified or
   distributed without the express prior written permission of the
   Panthronics AG.

   PLEASE REFER TO OUR DISCLAIMER NOTICE IN THE PROJECT ROOT.
   ---------------------------------------------------------------

    Project     : PTX100R
    Module      : NDEF T3T OPERATION API
    File        : ptxNDEF_T3TOP.h

    Description : Tag Type 3 NDEF Operation API (IOT READER - Extension)
*/

/**
 * \addtogroup grp_ptx_api_T3T_op Tag Type T3T Operation API
 *
 * @{
 */

#ifndef APIS_PTX_NDEF_OP_T3T_H_
#define APIS_PTX_NDEF_OP_T3T_H_

/*
 * ####################################################################################################################
 * INCLUDES
 * ####################################################################################################################
 */

#include <stdint.h>
#include "ptxNativeTag_T3T.h"
#include "ptxNDEF_Defines.h"
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
 * \brief T3T Tag specific default timeout definition
 */
#define PTX_T3T_DEFAULT_TIMEOUT_MS   (uint32_t)500
/**
 * \brief T3T Tag specific supported timeout definition
 */
#define PTX_T3T_SUPPORTED_VERSION    (uint8_t)0x10

/*
 * ####################################################################################################################
 * TYPES
 * ####################################################################################################################
 */

/**
 * \brief T3T NDEF OP Initialization Parameters
 */
typedef struct ptxNDEF_T3TOP_InitParams
{
    ptxIoTRd_t                      *IotRd;                         /**< IOT Reader provided by upper layer */
    uint8_t                         *TxBuffer;                      /**< Internal Tx-Buffer provided by upper layer */
    uint32_t                        TxBufferSize;                   /**< Size of Internal TX-Buffer */
    uint8_t                         *RxBuffer;                      /**< Internal Rx-Buffer provided by upper layer */
    uint32_t                        RxBufferSize;                   /**< Size of Internal Rx-Buffer */
    uint8_t                         *NFCID2;                        /**< NFCID2 */
    uint8_t                         NFCID2Len;                      /**< Size of NFCID2 */

} ptxNDEF_T3TOP_InitParams_t;

/**
 * \brief T3T NDEF OP Capability Container Parameters
 */
typedef struct ptxNDEF_T3TOP_CC
{
    uint8_t                         AttributeInformationBlock[16];  /**< contains full attribute information block */
    uint8_t                         Version;                        /**< version information, containing 4 bit major and 4 bit minor */
    uint8_t                         MajorVersion;                   /**< Version Information, major */
    uint8_t                         MinorVersion;                   /**< Version Information, minor */
    uint8_t                         Nbr;                            /**< maximum number of blocks that can be read with one check command */
    uint8_t                         Nbw;                            /**< maximum number of blocks that can be written with one update command */
    uint16_t                        NmaxB;                          /**< maximum number of blocks available for the NDEF message */
    uint8_t                         RFU[4];                         /**< to be ignored and not changed */
    uint8_t                         WriteFlag;                      /**< indicator whether a previous NDEF operation was finished */
    uint8_t                         RWFlag;                         /**< access condition whether the tag can be updated or not */
    uint32_t                        Ln;                             /**< size of actual stored NDEF data bytes */
    uint16_t                        Nbc;                            /**< number of NDEF data blocks */
    uint16_t                        Checksum;                       /**< helps check whether the attributes are correct */

} ptxNDEF_T3TOP_CC_t;

/**
 * \brief T3T NDEF OP Component
 */
typedef struct ptxNDEF_T3TOP
{
    /* Components */
    ptxStatus_Comps_t               CompId;                /**< Component Id */

    ptxNativeTag_T3T_t              NativeTagT3T;          /**< T3T Native Tag Component */
    struct ptxIoTRd_CardRegistry    *CardRegistry;         /**< Reference to Internal Card Registry */
    ptxNDEF_TagLifeCycle_t          LifeCycle;             /**< Tag Life-Cycle */
    uint8_t                         *RxBuffer;             /**< Internal Rx-Buffer provided by upper layer (may be shared with other components) */
    uint32_t                        RxBufferSize;          /**< Internal Rx-Buffer size */
    uint8_t                         WorkBuffer[64];        /**< Internal Work-Buffer */

    /* Tag Type specific members */
    ptxNDEF_T3TOP_CC_t              CCParams;               /**< T3T specific CC parameters */

    /* General Info and / or Greedy Collectin related Paraemters */

} ptxNDEF_T3TOP_t;

/*
 * ####################################################################################################################
 * API FUNCTIONS
 * ####################################################################################################################
 */

/**
 * \brief Initialize / Open the T3T OP Component.
 *
 * \param[in]   t3tOpComp           Pointer to an existing instance of the T3T-OP component.
 * \param[in]   initParams          Pointer to initialization parameters.
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNDEF_T3TOpOpen (ptxNDEF_T3TOP_t *t3tOpComp, ptxNDEF_T3TOP_InitParams_t *initParams);

/**
 * \brief Formats a Type 3 Tag to INITIALIZED state.
 *
 * \param[in]   t3tOpComp           Pointer to an existing instance of the T3T-OP component.
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNDEF_T3TOpFormatTag (ptxNDEF_T3TOP_t *t3tOpComp);

/**
 * \brief Checks if a NDEF-message is present on the given Tag (or not).
 *
 * \param[in]   t3tOpComp           Pointer to an existing instance of the T3T-OP component.
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNDEF_T3TOpCheckMessage (ptxNDEF_T3TOP_t *t3tOpComp);

/**
 * \brief Reads a NDEF-message from a given Tag.
 *
 * \param[in]     t3tOpComp           Pointer to an existing instance of the T3T-OP component.
 * \param[in]     msgBuffer           Pointer to buffer holding the read NDEF-message.
 * \param[in,out] msgLen              Size of the buffer (in), Length of the read NDEF-message (out).
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNDEF_T3TOpReadMessage (ptxNDEF_T3TOP_t *t3tOpComp, uint8_t *msgBuffer, uint32_t *msgLen);

/**
 * \brief Writes a NDEF-message onto a given Tag.
 *
 * \param[in] t3tOpComp           Pointer to an existing instance of the T3T-OP component.
 * \param[in] msgBuffer           Pointer to buffer holding the NDEF-message to write (NULL -> empty NDEF-message is written).
 * \param[in] msgLen              Size of NDEF-message (0 -> empty NDEF-message is written).
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNDEF_T3TOpWriteMessage (ptxNDEF_T3TOP_t *t3tOpComp, uint8_t *msgBuffer, uint32_t msgLen);

/**
 * \brief Puts a Tag into READ-ONLY state (Attention: This is a irreversible Operation!).
 *
 * \param[in]   t3tOpComp           Pointer to an existing instance of the T3T-OP component.
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNDEF_T3TOpLockTag (ptxNDEF_T3TOP_t *t3tOpComp);

/**
 * \brief Unitialize / Close the T3T OP Component
 *
 * \param[in]   t3tOpComp           Pointer to an existing instance of the T3T-OP component.
 *
 * \return Status, indicating whether the operation was successful. See \ref ptxStatus_t.
 */
ptxStatus_t ptxNDEF_T3TOpClose (ptxNDEF_T3TOP_t *t3tOpComp);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* Guard */

