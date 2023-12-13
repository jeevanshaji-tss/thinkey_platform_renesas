/** \file
   ---------------------------------------------------------------
   Copyright (C) 2021. Panthronics AG - All Rights Reserved.

   This material may not be reproduced, displayed, modified or
   distributed without the express prior written permission of the
   Panthronics AG.

   PLEASE REFER TO OUR DISCLAIMER NOTICE IN THE PROJECT ROOT.
   ---------------------------------------------------------------

    Project     : PTX100R
    Module      : NDEF T4T OPERATION API
    File        : ptxNDEF_T4TOP.h

    Description : Tag Type 4 NDEF Operation API (IOT READER - Extension)
*/

/**
 * \addtogroup grp_ptx_api_T4T_op Tag Type T4T Operation API
 *
 * @{
 */

#ifndef APIS_PTX_NDEF_OP_T4T_H_
#define APIS_PTX_NDEF_OP_T4T_H_

/*
 * ####################################################################################################################
 * INCLUDES
 * ####################################################################################################################
 */

#include <stdint.h>
#include "ptxNativeTag_T4T.h"
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
#if defined (_WIN32) || defined (_WIN64) || defined (ming) || defined (__MINGW32__) || defined (__GNUWIN32__)
    #define PTX_API_LIBFUNC __declspec(dllexport) /* */
#else

    #if defined (__GNUC__)                                                  /* GCC */
        #define PTX_API_LIBFUNC __attribute__ ((visibility("default")))      /* requires  -fvisibility=hidden  as compiler option */
    #elif defined (_linux) || defined (__linux)                             /* Linux with some other compiler */
        #error Missing Platform Specification                               /* To be added */
    #else
        #error Missing Platform Specification
    #endif
#endif

/**
 * \brief T4T Tag specific default timeout definition
 */
#define PTX_T4T_DEFAULT_TIMEOUT_MS      (uint32_t)50000

/**
 * \brief T4T Tag specific data length definition
 */
#define PDATALEN                        (uint8_t)0x40

/*
 * ####################################################################################################################
 * TYPES
 * ####################################################################################################################
 */

/**
 * \brief T4T NDEF OP Initialization Parameters
 */
typedef struct ptxNDEF_T4TOP_InitParams
{
    ptxIoTRd_t                      *IotRd;        /**< Allocated instance of IoT-Reader component (see ptxIoTRd_Allocate_Stack()) */
    uint8_t                         *TxBuffer;     /**< Internal Tx-Buffer */
    uint32_t                        TxBufferSize;  /**< Size of Internal TX-Buffer */
    uint8_t                         *RxBuffer;     /**< Internal Rx-Buffer */
    uint32_t                        RxBufferSize;  /**< Size of Internal Rx-Buffer */

} ptxNDEF_T4TOP_InitParams_t;

/**
 * \brief T4T NDEF OP Capability Container Parameters
 */
typedef struct ptxNDEF_T4TOP_CC
{
    uint8_t                         CCLen[2];               /**< length of the CC file, 2 bytes */
    uint8_t                         MappingVersion;         /**< mapping version info, either 20h or 30h */
    uint16_t                        MLeDigit;               /**< digit containing maximum RAPDU size */
    uint16_t                        MLcDigit;               /**< digit containing maximum CAPDU size */
    uint8_t                         NDEFTLV[10];            /**< stores NDEF TLV, 8 bytes for mapping version 20h, 10 bytes for 30h, checked accordingly */
    uint8_t                         NDEFFileIdentifier[2];  /**< NDEF file identifier, 2 bytes */
    uint16_t                        NDEFFileSize;           /**< size of the NDEF file in bytes, 2 bytes */
    uint8_t                         NDEFAccessRead;         /**< access condition for read */
    uint8_t                         NDEFAccessWrite;        /**< access condition for write */

} ptxNDEF_T4TOP_CC_t;

/**
 * \brief T4T NDEF file contents
 */
typedef struct ptxNDEF_T4TOP_NLEN
{
    uint8_t                         NLEN[4];                /**< NLEN or ENLEN field */
    uint8_t                         NbrNLENBytes;           /**< number of relevant (E)NLEN bytes, either 2 or 4 */
    uint32_t                        DigitNLEN;              /**< digit representation of (E)NLEN */

} ptxNDEF_T4TOP_NLEN_t;

/**
 * \brief T4T NDEF OP Component
 */
typedef struct ptxNDEF_T4TOP
{
    /* Components */
    ptxStatus_Comps_t               CompId;                /**< Component Id */

    ptxNativeTag_T4T_t              NativeTagT4T;          /**< T4T Native Tag Component */
    struct ptxIoTRd_CardRegistry    *CardRegistry;         /**< Reference to Internal Card Registry */
    ptxNDEF_TagLifeCycle_t          LifeCycle;             /**< Tag Life-Cycle */
    uint8_t                         *RxBuffer;             /**< Internal Rx-Buffer provided by upper layer (may be shared with other components) */
    uint32_t                        RxBufferSize;          /**< Internal Rx-Buffer size */

    /* Tag Type specific members */
    ptxNDEF_T4TOP_CC_t              CCParams;              /**< T4T specific CC parameters */

    ptxNDEF_T4TOP_NLEN_t            NLEN;                  /**< T4T NDEF file contents */

    /* General Info and / or Greedy Collecting related Parameters */

} ptxNDEF_T4TOP_t;

/*
 * ####################################################################################################################
 * API FUNCTIONS
 * ####################################################################################################################
 */

/**
 * \brief Initialize / Open the T4T OP Component.
 *
 * \param[in]   t4tOpComp           Pointer to an existing instance of the T4T-OP component.
 * \param[in]   initParams          Pointer to initialization parameters.
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNDEF_T4TOpOpen (ptxNDEF_T4TOP_t *t4tOpComp, ptxNDEF_T4TOP_InitParams_t *initParams);

/**
 * \brief Formats a Type 4 Tag to INITIALIZED state.
 *
 * \param[in]   t4tOpComp           Pointer to an existing instance of the T4T-OP component.
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNDEF_T4TOpFormatTag (ptxNDEF_T4TOP_t *t4tOpComp);

/**
 * \brief Checks if a NDEF-message is present on the given Tag (or not).
 *
 * \param[in]   t4tOpComp           Pointer to an existing instance of the T4T-OP component.
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNDEF_T4TOpCheckMessage (ptxNDEF_T4TOP_t *t4tOpComp);

/**
 * \brief Reads a NDEF-message from a given Tag.
 *
 * \param[in]     t4tOpComp           Pointer to an existing instance of the T4T-OP component.
 * \param[in]     msgBuffer           Pointer to buffer holding the read NDEF-message.
 * \param[in,out] msgLen              Size of the buffer (in), Length of the read NDEF-message (out).
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNDEF_T4TOpReadMessage (ptxNDEF_T4TOP_t *t4tOpComp, uint8_t *msgBuffer, uint32_t *msgLen);

/**
 * \brief Writes a NDEF-message onto a given Tag.
 *
 * \param[in] t4tOpComp           Pointer to an existing instance of the T4T-OP component.
 * \param[in] msgBuffer           Pointer to buffer holding the NDEF-message to write (NULL -> empty NDEF-message is written).
 * \param[in] msgLen              Size of NDEF-message (0 -> empty NDEF-message is written).
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNDEF_T4TOpWriteMessage (ptxNDEF_T4TOP_t *t4tOpComp, uint8_t *msgBuffer, uint32_t msgLen);

/**
 * \brief Puts a Tag into READ-ONLY state (Attention: This is a irreversible Operation!).
 *
 * \param[in]   t4tOpComp           Pointer to an existing instance of the T4T-OP component.
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNDEF_T4TOpLockTag (ptxNDEF_T4TOP_t *t4tOpComp);

/**
 * \brief Unitialize / Close the T4T OP Component
 *
 * \param[in]   t4tOpComp           Pointer to an existing instance of the T4T-OP component.
 *
 * \return Status, indicating whether the operation was successful. See \ref ptxStatus_t.
 */
ptxStatus_t ptxNDEF_T4TOpClose (ptxNDEF_T4TOP_t *t4tOpComp);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* Guard */

