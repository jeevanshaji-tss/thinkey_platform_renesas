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
    File        : ptxNSC_Hal.h

    Description :
*/

/**
 * \addtogroup grp_ptx_api_nsc_hal PTX NSC Stack HAL
 *
 * @{
 */

#ifndef COMPS_NSC_PTXNSC_HAL_H_
#define COMPS_NSC_PTXNSC_HAL_H_

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


#define PTX_HAL_WRITE_TIMEOUT                           (500u)          /**< Hal write timeout in ms. */
//nscCtx->RspTimeout  = (uint32_t)(20 * PTX_HAL_READ_TIMEOUT); /* 20 Times timeout at Hal level */
#define PTX_HAL_READ_TIMEOUT                            (500u)          /**< Hal read timeout in ms. */
#define PTX_NSC_RESPONSE_TIMEOUT                        (uint32_t)(20 * PTX_HAL_READ_TIMEOUT) /**< Hal response timeout in ms. */


#define PTX_NSC_HAL_ADDRESS_LENGTH                      (0x02u)         /**< Hal Address length. */

#define PTX_NSC_HAL_BUFFER_LENGTH_RX_MAX                (256u)          /**< HAL Maximum RX buffer length. */
#define PTX_NSC_HAL_DUMMY_VALUE                         (0xFFu)         /**< HAL Dummy Value. */
#define PTX_NSC_LENGTH_MAX                              (255u)          /**< Maximum size of NSC_CMD. 256 (Maximun on Uart - 1 Opcode for Write buffer operation)*/

#define PTX_NSC_HAL_PAYLOAD_LENGTH_WI_MAX               (252u)          /**< Maximum payload that can be taken by Write Instruction on Uart (used as well for other interfaces.)*/
#define PTX_NSC_HAL_WRITE_INSTRUCTION_MASK              (0x04u)         /**< HAL write instruction mask */
#define PTX_NSC_HAL_WRITE_RANDOM_ADDRESS_MASK           (0x05u)         /**< HAL write Random address mask */
#define PTX_NSC_HAL_READ_BUFFER_MASK                    (0x03u)         /**< HAL read buffer mask */
#define PTX_NSC_HAL_WRITE_BUFFER_MASK                   (0x07u)         /**< HAL write buffer mask */
#define PTX_NSC_HAL_READ_RANDOM_ADDRESS_MASK            (0x01u)         /**< HAL read random address mask */
#define PTX_NSC_HAL_READ_CONTINUOUS_ADDRESS_MASK        (0x02u)         /**< HAL read continuous address mask */
#define PTX_NSC_HAL_WRITE_CONTINUOUS_ADDRESS_MASK       (0x06u)         /**< HAL write continuous address mask */
#define PTX_NSC_HAL_BUFFER_ADDRESS_MASK                 (0x1Fu)         /**< HAL buffer address mask */

/**
 * \brief Forward declaration of NSC Component
 */
struct ptxNSC;

/**
 * \brief ID of buffers handled by Hardware
 */
typedef enum ptxNscHal_BufferId
{
    NscWriteBuffer_0,
    NscWriteBuffer_1,
    NscWriteBuffer_2,
    NscWriteBuffer_3,
    NscWriteBuffer_4,
    NscWriteBuffer_5,
    NscWriteBuffer_6,
    NscWriteBuffer_7,
    NscWriteBuffer_8,
    NscWriteBuffer_9,
    NscWriteBuffer_10,
    NscWriteBuffer_11,
    NscWriteBuffer_12,
    NscWriteBuffer_13,
    NscWriteBuffer_14,
    NscWriteBuffer_15,
    NscWriteBuffer_Max
} ptxNscHal_BufferId_t;

/*
 * ####################################################################################################################
 * FUNCTIONS
 * ####################################################################################################################
 */

/**
 * \brief Internal command to exchange NSC-commands using buffers.
 *
 * \param[in]   nscCtx           Pointer to an initialized instance of the NSC.
 * \param[in]   bufferId         Buffer identifier.
 * \param[in]   txBuf            Pointer to an array of buffers.
 * \param[in]   txLen            Pointer to an array containing length-information for each buffer.
 * \param[in]   numBuffers       Number of buffers.
 *
 * \return Status, indicating whether the operation was successful.See \ref ptxStatus_t.
 */
ptxStatus_t ptxNSC_HAL_WriteBuffer(struct ptxNSC *nscCtx, ptxNscHal_BufferId_t bufferId, uint8_t *txBuf[], size_t txLen[], size_t numBuffers);

/**
 * \brief Internal command to write certain SFRs.
 *
 * \param[in]   nscCtx           Pointer to an initialized instance of the NSC.
 * \param[in]   address          Address of SFR.
 * \param[in]   value            Value to write.
 *
 * \return Status, indicating whether the operation was successful.See \ref ptxStatus_t.
 */
ptxStatus_t ptxNSC_HAL_Wra(struct ptxNSC *nscCtx, uint16_t address, uint8_t value);

/**
 * \brief Internal command to write certain SFRs (operating-mode dependent).
 *
 * \param[in]   nscCtx           Pointer to an initialized instance of the NSC.
 * \param[in]   address          Address of SFR.
 * \param[in]   value            Value to write.
 *
 * \return Status, indicating whether the operation was successful.See \ref ptxStatus_t.
 */
ptxStatus_t ptxNSC_HAL_Wra_NoWait(struct ptxNSC *nscCtx, uint16_t address, uint8_t value);

/**
 * \brief Internal command to write certain SFRs (operating-mode dependent).
 *
 * \param[in]   nscCtx           Pointer to an initialized instance of the NSC.
 * \param[in]   address          Address of SFR.
 * \param[in]   value            Value to write.
 *
 * \return Status, indicating whether the operation was successful.See \ref ptxStatus_t.
 */
ptxStatus_t ptxNSC_HAL_Wra_NoCheck(struct ptxNSC *nscCtx, uint16_t address, uint8_t value);

/**
 * \brief Internal command to read certain SFRs.
 *
 * \param[in]   nscCtx           Pointer to an initialized instance of the NSC.
 * \param[in]   address          Address of SFR.
 * \param[out]  value            Value read.
 *
 * \return Status, indicating whether the operation was successful.See \ref ptxStatus_t.
 */
ptxStatus_t ptxNSC_HAL_Rra(struct ptxNSC *nscCtx, uint16_t address, uint8_t *value);

/**
 * \brief Internal command to write NSC-instructions.
 *
 * \param[in]   nscCtx           Pointer to an initialized instance of the NSC.
 * \param[in]   address          Instruction address.
 * \param[in]   pPayload         Pointer to buffer containing instructions.
 * \param[in]   txLen            Number of instructions.
 *
 * \return Status, indicating whether the operation was successful.See \ref ptxStatus_t.
 */
ptxStatus_t ptxNSC_HAL_WriteInstruction(struct ptxNSC *nscCtx, uint16_t address, uint8_t *pPayload, size_t txLen );

#ifdef __cplusplus
}
#endif

#endif /* Guard */

/** @} */

