/** \file
   ---------------------------------------------------------------
   Copyright (C) 2021. Panthronics AG - All Rights Reserved.

   This material may not be reproduced, displayed, modified or
   distributed without the express prior written permission of the
   Panthronics AG.

   PLEASE REFER TO OUR DISCLAIMER NOTICE IN THE PROJECT ROOT.
   ---------------------------------------------------------------

    Project     : PTX100R
    Module      : NDEF T2T OPERATION API
    File        : ptxNDEF_T2TOP.h

    Description : Tag Type 2 NDEF Operation API (IOT READER - Extension)
*/

/**
 * \addtogroup grp_ptx_api_T2T_op Tag Type T2T Operation API
 *
 * @{
 */

#ifndef APIS_PTX_NDEF_OP_T2T_H_
#define APIS_PTX_NDEF_OP_T2T_H_

/*
 * ####################################################################################################################
 * INCLUDES
 * ####################################################################################################################
 */

#include <stdint.h>
#include "ptxNativeTag_T2T.h"
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
 * \brief T2T Tag specific default timeout definition
 */
#define PTX_T2T_DEFAULT_TIMEOUT_MS          (uint32_t)100 
/**
 * \brief T2T Tag specific default supported version definition
 */
#define PTX_T2T_SUPPORTED_VERSION           (uint8_t)0x11
/**
 * \brief T2T Tag specific default max number of lock controls definition
 */
#define PTX_T2TOP_MAX_NUMBER_LOCK_CONTROL   (uint8_t)16
/**
 * \brief T2T Tag specific default max number of memory controls definition
 */
#define PTX_T2TOP_MAX_NUMBER_MEMORY_CONTROL (uint8_t)16

/*
 * ####################################################################################################################
 * TYPES
 * ####################################################################################################################
 */

/**
 * \brief T2T NDEF OP Initialization Parameters
 */
typedef struct ptxNDEF_T2TOP_InitParams
{
    ptxIoTRd_t                      *IotRd;                         /**< IOT Reader provided by upper layer */
    uint8_t                         *TxBuffer;                      /**< Internal Tx-Buffer provided by upper layer */
    uint32_t                        TxBufferSize;                   /**< Size of Internal TX-Buffer */
    uint8_t                         *RxBuffer;                      /**< Internal Rx-Buffer provided by upper layer */
    uint32_t                        RxBufferSize;                   /**< Size of Internal Rx-Buffer */

} ptxNDEF_T2TOP_InitParams_t;

/**
 * \brief T2T NDEF OP Capability Container Parameters
 */
typedef struct ptxNDEF_T2TOP_CC
{
    uint8_t                         MagicNumber;                    /**< Magic Number */
    uint8_t                         Version;                        /**< Version */
    uint16_t                        MLEN;                           /**< Length */
    uint8_t                         Access;                         /**< Access contains read and write access */

    uint8_t                         ReadAccess;                     /**< read permission */
    uint8_t                         WriteAccess;                    /**< write permission */
    uint8_t                         VersionMajor;                   /**< Major Version */
    uint8_t                         VersionMinor;                   /**< Minor Version */
    uint16_t                        Size;                           /**< Size in bytes */
    uint16_t                        NumberOfBlocks;                 /**< NUmber of Blocks each containing 4 bytes */

} ptxNDEF_T2TOP_CC_t;

/**
 * \brief T2T NDEF TLV position
 */
typedef struct ptxNDEF_T2TOP_NDEF_TLV
{
    uint16_t                        ByteAddress;                    /**< Byte address of T byte */
    uint8_t                         NDEFTLVFound;                   /**< NDEF TLV found check, only first accepted */
    uint16_t                        Length;                         /**< Length in byte */
    uint16_t                        AvailableLength;                /**< Available length in byte, not including other TLVs and DLAs/RAs */

} ptxNDEF_T2TOP_NDEF_TLV_t;

/**
 * \brief T2T Terminator TLV position
 */
typedef struct ptxNDEF_T2TOP_TERMINATOR_TLV
{
    uint16_t                        ByteAddress;                    /**< Byte address of T byte */
    uint8_t                         TerminatorTLVFound;             /**< Terminator TLV found check, only one needed */

} ptxNDEF_T2TOP_TERMINATOR_TLV_t;

/**
 * \brief T2T Lock Control TLV positions and counter
 */
typedef struct ptxNDEF_T2TOP_LOCK_CONTROL_TLV
{
    uint16_t                        ByteAddress[PTX_T2TOP_MAX_NUMBER_LOCK_CONTROL];                 /**< Byte addresses of T byte */
    uint8_t                         NumberOfTLVs;                                                   /**< Number of TLVs */
    uint16_t                        LockArea[PTX_T2TOP_MAX_NUMBER_LOCK_CONTROL * 2];                /**< begin and end byte address of areas [begin1,end1,begin2,end2,...] */
    uint8_t                         NbrReservedBits[PTX_T2TOP_MAX_NUMBER_LOCK_CONTROL];             /**< Reserved Bits for not completely lock-bit-filled bytes */
    uint8_t                         BytesLockedPerLockBit[PTX_T2TOP_MAX_NUMBER_LOCK_CONTROL];       /**< Bytes Locked per Lock Bit */

} ptxNDEF_T2TOP_LOCK_CONTROL_TLV_t;

/**
 * \brief T2T Memory Control TLV positions and counter
 */
typedef struct ptxNDEF_T2TOP_MEMORY_CONTROL_TLV
{
    uint16_t                        ByteAddress[PTX_T2TOP_MAX_NUMBER_MEMORY_CONTROL];               /**< Byte addresses of T byte */
    uint8_t                         NumberOfTLVs;                                                   /**< Number of TLVs */
    uint16_t                        RsvdArea[PTX_T2TOP_MAX_NUMBER_MEMORY_CONTROL * 2];              /**< begin and end byte address of areas [begin1,end1,begin2,end2,...] */

} ptxNDEF_T2TOP_MEMORY_CONTROL_TLV_t;

/**
 * \brief T2T NDEF OP TLV byte adresses and counters
 */
typedef struct ptxNDEF_T2TOP_TLV
{
    ptxNDEF_T2TOP_TERMINATOR_TLV_t                      TerminatorTLV;                              /**< Terminator TLV */
    ptxNDEF_T2TOP_NDEF_TLV_t                            NDEFTLV;                                    /**< NDEF TLV */
    ptxNDEF_T2TOP_LOCK_CONTROL_TLV_t                    LockControlTLVs;                            /**< Lock control TLV */
    ptxNDEF_T2TOP_MEMORY_CONTROL_TLV_t                  MemoryControlTLVs;                          /**< Memory control TLV */

} ptxNDEF_T2TOP_TLV_t;

/**
 * \brief T2T NDEF OP sector information
 */ 
typedef struct ptxNDEF_T2TOP_Sector
{
    uint8_t                                             CurrentSector;                              /**< Current sector */
    uint8_t                                             NumberOfSectors;                            /**< Number of Sectors */

} ptxNDEF_T2TOP_Sector_t;


/**
 * \brief T2T NDEF OP Component
 */
typedef struct ptxNDEF_T2TOP
{
    /* Components */
    ptxStatus_Comps_t               CompId;                /**< Component Id */

    ptxNativeTag_T2T_t              NativeTagT2T;          /**< T2T Native Tag Component */
    struct ptxIoTRd_CardRegistry    *CardRegistry;         /**< Reference to Internal Card Registry */
    ptxNDEF_TagLifeCycle_t          LifeCycle;             /**< Tag Life-Cycle */
    uint8_t                         *RxBuffer;             /**< Internal Rx-Buffer provided by upper layer (may be shared with other components) */
    uint32_t                        RxBufferSize;          /**< Internal Rx-Buffer size */
    uint8_t                         WorkBuffer[64];        /**< Internal Work-Buffer */

    /* Tag Type specific members */
    ptxNDEF_T2TOP_CC_t              CCParams;              /**< T2T CC Parameters */
    ptxNDEF_T2TOP_TLV_t             TLVs;                  /**< T2T TLVs */
    ptxNDEF_T2TOP_Sector_t          SectorParams;          /**< T2T Sector Parameters */


    /* General Info and / or Greedy Collecting related Parameters */

} ptxNDEF_T2TOP_t;

/*
 * ####################################################################################################################
 * API FUNCTIONS
 * ####################################################################################################################
 */

/**
 * \brief Initialize / Open the T2T OP Component.
 *
 * \param[in]   t2tOpComp           Pointer to an existing instance of the T2T-OP component.
 * \param[in]   initParams          Pointer to initialization parameters.
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNDEF_T2TOpOpen (ptxNDEF_T2TOP_t *t2tOpComp, ptxNDEF_T2TOP_InitParams_t *initParams);

/**
 * \brief Formats a Type 2 Tag to INITIALIZED state.
 *
 * \param[in]   t2tOpComp           Pointer to an existing instance of the T2T-OP component.
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNDEF_T2TOpFormatTag (ptxNDEF_T2TOP_t *t2tOpComp);

/**
 * \brief Checks if a NDEF-message is present on the given Tag (or not).
 *
 * \param[in]   t2tOpComp           Pointer to an existing instance of the T2T-OP component.
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNDEF_T2TOpCheckMessage (ptxNDEF_T2TOP_t *t2tOpComp);

/**
 * \brief Reads a NDEF-message from a given Tag.
 *
 * \param[in]     t2tOpComp           Pointer to an existing instance of the T2T-OP component.
 * \param[in]     msgBuffer           Pointer to buffer holding the read NDEF-message.
 * \param[in,out] msgLen              Size of the buffer (in), Length of the read NDEF-message (out).
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNDEF_T2TOpReadMessage (ptxNDEF_T2TOP_t *t2tOpComp, uint8_t *msgBuffer, uint32_t *msgLen);

/**
 * \brief Writes a NDEF-message onto a given Tag.
 *
 * \param[in] t2tOpComp           Pointer to an existing instance of the T2T-OP component.
 * \param[in] msgBuffer           Pointer to buffer holding the NDEF-message to write (NULL -> empty NDEF-message is written).
 * \param[in] msgLen              Size of NDEF-message (0 -> empty NDEF-message is written).
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNDEF_T2TOpWriteMessage (ptxNDEF_T2TOP_t *t2tOpComp, uint8_t *msgBuffer, uint32_t msgLen);

/**
 * \brief Puts a Tag into READ-ONLY state (Attention: This is a irreversible Operation!).
 *
 * \param[in]   t2tOpComp           Pointer to an existing instance of the T2T-OP component.
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxNDEF_T2TOpLockTag (ptxNDEF_T2TOP_t *t2tOpComp);

/**
 * \brief Unitialize / Close the T2T OP Component
 *
 * \param[in]   t2tOpComp           Pointer to an existing instance of the T2T-OP component.
 *
 * \return Status, indicating whether the operation was successful. See \ref ptxStatus_t.
 */
ptxStatus_t ptxNDEF_T2TOpClose (ptxNDEF_T2TOP_t *t2tOpComp);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* Guard */

