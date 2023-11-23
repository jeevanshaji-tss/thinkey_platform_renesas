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
    File        : ptxNativeTag_T5T.c

    Description : Native Tag API for NFC Forum Tag Type 5 (IOT READER - Extension)
*/


/*
 * ####################################################################################################################
 * INCLUDES
 * ####################################################################################################################
 */
#include "ptx_IOT_READER.h"
#include "ptxNDEF_T5TOP.h"
#include "ptxNativeTag_T5T.h"
#include <string.h>

/*
 * ####################################################################################################################
 * DEFINES / TYPES
 * ####################################################################################################################
 */
#define PTX_T5TOP_CC_BLOCK_NUMBER                           (uint16_t)0x0000

#define PTX_T5TOP_CC_OFFSET_MAGIC_NUMBER                    (uint8_t)0x00
#define PTX_T5TOP_CC_OFFSET_VERSION_ACCESS                  (uint8_t)0x01
#define PTX_T5TOP_CC_OFFSET_MLEN_BASIC                      (uint8_t)0x02
#define PTX_T5TOP_CC_OFFSET_MLEN_EXTENDED                   (uint8_t)0x06
#define PTX_T5TOP_CC_OFFSET_ADDITIONAL_FEATURE              (uint8_t)0x03

#define PTX_T5TOP_CC_ADD_INFO_SPECIAL_FRAME_MASK            (uint8_t)0x10
#define PTX_T5TOP_CC_ADD_INFO_LOCK_BLOCK_MASK               (uint8_t)0x08
#define PTX_T5TOP_CC_ADD_INFO_MBREAD_MASK                   (uint8_t)0x01

#define PTX_T5TOP_CC_MAGIC_NUMBER_E1                        (uint8_t)0xE1
#define PTX_T5TOP_CC_MAGIC_NUMBER_E2                        (uint8_t)0xE2
#define PTX_T5TOP_CC_VERSION_MASK                           (uint8_t)0xF0
#define PTX_T5TOP_CC_READ_ACCESS_MASK                       (uint8_t)0x0C
#define PTX_T5TOP_CC_WRITE_ACCESS_MASK                      (uint8_t)0x03

#define PTX_T5TOP_OFFSET_RES_FLAG                           (uint8_t)0x00
#define PTX_T5TOP_OFFSET_RES_DATA                           (uint8_t)0x01   /**< Fixed at offset 1 as "Block Security Status"-byte is never requested */

#define PTX_T5TOP_RES_FLAG_ERROR_MASK                       (uint8_t)0x01

#define PTX_T5TOP_NDEF_MESSAGE_TLV_T                        (uint8_t)0x03
#define PTX_T5TOP_TERMINATOR_TLV_T                          (uint8_t)0xFE

#define PTX_T5TOP_2ND_READ_OP_NOT_REQUIRED                  (uint8_t)0x00
#define PTX_T5TOP_2ND_READ_OP_GET_FULL_LENGTH               (uint8_t)0x01
#define PTX_T5TOP_2ND_READ_OP_3BL_GET_PART_LENGTH           (uint8_t)0x02
#define PTX_T5TOP_2ND_READ_OP_3BL_GET_FULL_LENGTH           (uint8_t)0x03

#define PTX_T5TOP_INTERNAL_BUFLENGTH                        (uint8_t)64

/*
 * ####################################################################################################################
 * INTERNAL FUNCTIONS / HELPERS
 * ####################################################################################################################
 */
static ptxStatus_t ptxNDEF_T5TGetCapabilityContainerInfo (ptxNDEF_T5TOP_t *t5tOpComp);
static ptxStatus_t ptxNDEF_T5TOpReadBlock (ptxNDEF_T5TOP_t *t5tOpComp, uint16_t blockNumber, uint8_t *rx, uint32_t *rxLen);
static ptxStatus_t ptxNDEF_T5TOpWriteBlock (ptxNDEF_T5TOP_t *t5tOpComp, uint16_t blockNumber, uint8_t *blockData, uint8_t blockDataLen, uint8_t *rx, uint32_t *rxLen);
static ptxStatus_t ptxNDEF_T5TOpLockBlock (ptxNDEF_T5TOP_t *t5tOpComp, uint16_t blockNumber, uint8_t *rx, uint32_t *rxLen);
static ptxStatus_t ptxNDEF_T5TOpUpdateLength (ptxNDEF_T5TOP_t *t5tOpComp, uint16_t messageLen, uint8_t lengthSize);

/*
 * ####################################################################################################################
 * API FUNCTIONS
 * ####################################################################################################################
 */
ptxStatus_t ptxNDEF_T5TOpOpen (ptxNDEF_T5TOP_t *t5tOpComp, ptxNDEF_T5TOP_InitParams_t *initParams)
{
    ptxStatus_t status = ptxStatus_Success;
    ptxNativeTag_T5T_InitParams_t t5t_init_params;

    if ((NULL != t5tOpComp) && (NULL != initParams))
    {
        if ((NULL != initParams->IotRd) &&
            (NULL != initParams->TxBuffer) &&
            (0 != initParams->TxBufferSize) &&
            (NULL != initParams->RxBuffer) &&
            (0 != initParams->RxBufferSize))
        {
            /* clear component */
            (void)memset(t5tOpComp, 0, sizeof(ptxNDEF_T5TOP_t));

            /* set members */
            (void)ptxIoTRd_Get_Card_Registry (initParams->IotRd, &t5tOpComp->CardRegistry);

            t5tOpComp->RxBuffer = initParams->RxBuffer;
            t5tOpComp->RxBufferSize = initParams->RxBufferSize;
            t5tOpComp->LifeCycle = TagLC_NoNDEFTag;

            /* initialize lower layer component */
            (void)memset(&t5t_init_params, 0, sizeof(ptxNativeTag_T5T_InitParams_t));
            t5t_init_params.IotRd = initParams->IotRd;
            t5t_init_params.TxBuffer = initParams->TxBuffer;
            t5t_init_params.TxBufferSize = initParams->TxBufferSize;
            t5t_init_params.UID = initParams->UID;
            t5t_init_params.UIDLen = initParams->UIDLen;

            status = ptxNativeTag_T5TOpen(&t5tOpComp->NativeTagT5T, &t5t_init_params);

            /* set Component-ID at the end to prevent futher calls in case of an error */
            if (ptxStatus_Success == status)
            {
                t5tOpComp->CompId = ptxStatus_Comp_T5TOP;
            }

        } else
        {
            status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_InvalidParameter);
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNDEF_T5TOpFormatTag (ptxNDEF_T5TOP_t *t5tOpComp)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(t5tOpComp, ptxStatus_Comp_T5TOP))
    {
        /* set UID */
        /*
        status = ptxNativeTag_T5TSetUID(&t5tOpComp->NativeTagT5T,
                                        &t5tOpComp->CardRegistry->ActiveCard->TechParams.CardVParams.UID[0],
                                        PTX_T5T_UID_SIZE);
         */

        status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_NotImplemented);

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNDEF_T5TOpCheckMessage (ptxNDEF_T5TOP_t *t5tOpComp)
{
    ptxStatus_t status = ptxStatus_Success;
    uint16_t current_block_nr = 0;
    uint8_t quit_loop = 0;
    uint8_t ndef_tlv_found = 0;
    uint8_t terminator_tlv_found = 0;
    uint16_t total_read_index = 0;
    uint8_t init_block_read_offset = 0;
    uint32_t rx_len;
    uint8_t i;
    uint8_t second_read_operation = PTX_T5TOP_2ND_READ_OP_NOT_REQUIRED;

    if (PTX_COMP_CHECK(t5tOpComp, ptxStatus_Comp_T5TOP))
    {
        /* set UID */
        status = ptxNativeTag_T5TSetUID(&t5tOpComp->NativeTagT5T,
                                        &t5tOpComp->CardRegistry->ActiveCard->TechParams.CardVParams.UID[0],
                                        PTX_T5T_UID_SIZE);

        if (ptxStatus_Success == status)
        {
            /* read CC-parameters */
            status = ptxNDEF_T5TGetCapabilityContainerInfo (t5tOpComp);
        }

        if (ptxStatus_Success == status)
        {
            /* reset Greedy-collection */
            t5tOpComp->NDEF_TLV_POS_BN = 0;
            t5tOpComp->NDEF_TLV_POS_BY = 0;
            t5tOpComp->NDEF_TLV_LENGTH = 0;

            /* determine starting block number and overal read index */
            switch (t5tOpComp->BlockSize)
            {
                case (uint8_t)4:
                    current_block_nr = ((uint8_t)4 == t5tOpComp->CCParams.Size) ? (uint16_t)1 : (uint16_t)2;
                    init_block_read_offset = 0;
                    break;

                case (uint8_t)8:
                    current_block_nr = ((uint8_t)4 == t5tOpComp->CCParams.Size) ? (uint16_t)0 : (uint16_t)1;
                    init_block_read_offset = (0 == current_block_nr) ? (uint8_t)(t5tOpComp->BlockSize - t5tOpComp->CCParams.Size) : 0;
                    break;

                default:
                    current_block_nr = 0;
                    init_block_read_offset = (uint8_t)(t5tOpComp->BlockSize - t5tOpComp->CCParams.Size);
                    break;
            }
        }

        /* search for the NDEF-TLV */
        while ((total_read_index < t5tOpComp->CCParams.MLEN) && (ptxStatus_Success == status) && (0 == quit_loop))
        {
            /* read block */
            status = ptxNDEF_T5TOpReadBlock(t5tOpComp, current_block_nr, &t5tOpComp->RxBuffer[0], &rx_len);

            if ((ptxStatus_Success == status) && (rx_len >= (uint32_t)((1 + t5tOpComp->BlockSize))))
            {
                for (i = (uint8_t)(PTX_T5TOP_OFFSET_RES_DATA + init_block_read_offset); i < (uint8_t)rx_len; i++)
                {
                    /* T-field of NDEF-TLV found ? */
                    if (PTX_T5TOP_NDEF_MESSAGE_TLV_T == t5tOpComp->RxBuffer[i])
                    {
                        t5tOpComp->NDEF_TLV_POS_BN = current_block_nr;
                        t5tOpComp->NDEF_TLV_POS_BY = (uint8_t)(i - PTX_T5TOP_OFFSET_RES_DATA);

                        /* try to read L-field (Attention: Might consist of 1 or 3 bytes; can be split over next block ...) */
                        if ((uint8_t)(i + 1) < (uint8_t)rx_len)
                        {
                            /* 1- or 3-byte L-field ?*/
                            if ((uint8_t)0xFE >= t5tOpComp->RxBuffer[i + 1])
                            {
                                /* 1-byte L-field */
                                t5tOpComp->NDEF_TLV_LENGTH = (uint16_t)t5tOpComp->RxBuffer[i + 1];

                            } else
                            {
                                /* can complete L-field be read (2 additional bytes) ? */
                                if ((uint8_t)(i + 3) < (uint8_t)rx_len)
                                {
                                    t5tOpComp->NDEF_TLV_LENGTH = (uint16_t)((t5tOpComp->RxBuffer[i + 2] << 8) | (t5tOpComp->RxBuffer[i + 3]));

                                /* can at least 1st part of L-field be read (1 additional byte) ? */
                                } else if ((uint8_t)(i + 2) < (uint8_t)rx_len)
                                {
                                    t5tOpComp->NDEF_TLV_LENGTH = (uint16_t)((t5tOpComp->RxBuffer[i + 2] << 8));
                                    second_read_operation = (uint8_t)2;

                                } else
                                {
                                    second_read_operation = (uint8_t)3;
                                }
                            }
                        } else
                        {
                            second_read_operation = (uint8_t)1;
                        }

                        /*
                         * NDEF_TLV_POS_BN and NDEF_TLV_POS_BY can not be 0 because this would mean TLV is 1st byte of CC.
                         * This has already been checked upfront during the call to ptxNDEF_T5TGetCapabilityContainerInfo().
                         * NDEF_TLV_LENGTH might be 0 (=> empty message i.e. INITIALIZED-state).
                         */
                        if ((0 != t5tOpComp->NDEF_TLV_POS_BN) ||
                            (0 != t5tOpComp->NDEF_TLV_POS_BY) ||
                            (0 != t5tOpComp->NDEF_TLV_LENGTH))
                        {
                            ndef_tlv_found = (uint8_t)1;
                        }
                    }

                    /* T-field of TERMINATOR-TLV found ? */
                    if (PTX_T5TOP_TERMINATOR_TLV_T == t5tOpComp->RxBuffer[i])
                    {
                        /* Terminator-TLV determines the end of the reading-procedure */
                        terminator_tlv_found = (uint8_t)1;
                    }

                    total_read_index++;

                    quit_loop = (uint8_t)(quit_loop | ndef_tlv_found | terminator_tlv_found | second_read_operation);

                    if (0 != quit_loop)
                    {
                        /* quit for-loop */
                        break;
                    }
                }

                /* continue with next block if no TLV was found so far */
                if ((0 == ndef_tlv_found) && (0 == terminator_tlv_found))
                {
                    current_block_nr++;
                }

                /* reset initial block offset because after 1st read, every further block gets read starting from offset 0 */
                init_block_read_offset = 0;
            }
        }

        /* get L-field from next block (if necessary) */
        if (PTX_T5TOP_2ND_READ_OP_NOT_REQUIRED != second_read_operation)
        {
            current_block_nr++;

            /* read block */
            status = ptxNDEF_T5TOpReadBlock(t5tOpComp, current_block_nr, &t5tOpComp->RxBuffer[0], &rx_len);

            if (ptxStatus_Success == status)
            {
                switch (second_read_operation)
                {
                    case PTX_T5TOP_2ND_READ_OP_GET_FULL_LENGTH:
                        /* 1- or 3-byte L-field ?*/
                        if ((uint8_t)0xFE >= t5tOpComp->RxBuffer[PTX_T5TOP_OFFSET_RES_DATA + 0])
                        {
                            /* 1-byte L-field */
                            t5tOpComp->NDEF_TLV_LENGTH = (uint16_t)t5tOpComp->RxBuffer[PTX_T5TOP_OFFSET_RES_DATA + 0];

                        } else
                        {
                            t5tOpComp->NDEF_TLV_LENGTH = (uint16_t)((t5tOpComp->RxBuffer[PTX_T5TOP_OFFSET_RES_DATA + 0] << 8) | \
                                                                    (t5tOpComp->RxBuffer[PTX_T5TOP_OFFSET_RES_DATA + 1]));
                        }
                        break;

                    case PTX_T5TOP_2ND_READ_OP_3BL_GET_PART_LENGTH:
                        t5tOpComp->NDEF_TLV_LENGTH = (uint16_t)(t5tOpComp->NDEF_TLV_LENGTH |
                                                     (uint16_t)((t5tOpComp->RxBuffer[PTX_T5TOP_OFFSET_RES_DATA + 0])));
                        break;

                    case PTX_T5TOP_2ND_READ_OP_3BL_GET_FULL_LENGTH:
                        t5tOpComp->NDEF_TLV_LENGTH = (uint16_t)((t5tOpComp->RxBuffer[PTX_T5TOP_OFFSET_RES_DATA + 0] << 8) | \
                                                                (t5tOpComp->RxBuffer[PTX_T5TOP_OFFSET_RES_DATA + 1]));
                        break;

                    default:
                        status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_InternalError);
                        break;
                }
            }
        }

        /* update Life-Cycle of Tag */
        if (ptxStatus_Success == status)
        {
            if (0 != ndef_tlv_found)
            {
                if (0 != t5tOpComp->CCParams.WriteAccess)
                {
                    t5tOpComp->LifeCycle = (0 != t5tOpComp->NDEF_TLV_LENGTH) ? TagLC_ReadWrite : TagLC_Initialized;

                } else
                {
                    t5tOpComp->LifeCycle = TagLC_ReadOnly;
                }

            } else
            {
                status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_ResourceNotFound);
            }
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNDEF_T5TOpReadMessage (ptxNDEF_T5TOP_t *t5tOpComp, uint8_t *msgBuffer, uint32_t *msgLen)
{
    ptxStatus_t status = ptxStatus_Success;
    uint16_t current_block_nr;
    uint16_t bytes_read = 0;
    uint32_t msg_buffer_size;
    uint8_t ndef_tlv_header_size;
    uint8_t init_block_read_offset;
    uint32_t rx_len;
    uint16_t bytes_received;
    uint16_t bytes_to_copy;

    if (PTX_COMP_CHECK(t5tOpComp, ptxStatus_Comp_T5TOP) && (NULL != msgBuffer) && (NULL != msgLen))
    {
        msg_buffer_size = *msgLen;

        if (0 != msg_buffer_size)
        {
            switch (t5tOpComp->LifeCycle)
            {
                case TagLC_Initialized:
                case TagLC_ReadWrite:
                case TagLC_ReadOnly:
                    /* OK */
                    break;

                default:
                    status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_InvalidState);
                    break;
            }

            if (ptxStatus_Success == status)
            {
                /* set UID */
                status = ptxNativeTag_T5TSetUID(&t5tOpComp->NativeTagT5T,
                                                &t5tOpComp->CardRegistry->ActiveCard->TechParams.CardVParams.UID[0],
                                                PTX_T5T_UID_SIZE);
            }

            if (ptxStatus_Success == status)
            {
                /*
                 * determine block-adress to start reading (NDEF_TLV_POS_BN defines the block where the NDEF-TLV starts,
                 * but not the actual NDEF-message, which could be stored in the next block;
                 */
                current_block_nr = t5tOpComp->NDEF_TLV_POS_BN;
                ndef_tlv_header_size = (t5tOpComp->NDEF_TLV_LENGTH <= (uint16_t)0xFE) ? (uint8_t)2 : (uint8_t)4;

                if ((uint8_t)((t5tOpComp->NDEF_TLV_POS_BY + ndef_tlv_header_size)) < t5tOpComp->BlockSize)
                {
                    init_block_read_offset = (uint8_t)(t5tOpComp->NDEF_TLV_POS_BY + ndef_tlv_header_size);

                } else
                {
                    init_block_read_offset = (uint8_t)((t5tOpComp->NDEF_TLV_POS_BY + ndef_tlv_header_size) % t5tOpComp->BlockSize);
                    current_block_nr++;
                }

                while ((bytes_read != t5tOpComp->NDEF_TLV_LENGTH) && (bytes_read < (uint16_t)(t5tOpComp->CCParams.MLEN - ndef_tlv_header_size)) && (ptxStatus_Success == status))
                {
                    /* read block */
                    status = ptxNDEF_T5TOpReadBlock(t5tOpComp, current_block_nr, &t5tOpComp->RxBuffer[0], &rx_len);

                    if ((ptxStatus_Success == status) && (rx_len >= init_block_read_offset))
                    {
                        /* skip status-flag at offset 0 */
                        rx_len--;

                        bytes_received = (uint16_t)(rx_len - init_block_read_offset);

                        if ((t5tOpComp->NDEF_TLV_LENGTH - bytes_read) > bytes_received)
                        {
                            bytes_to_copy = bytes_received;

                        } else
                        {
                            bytes_to_copy = (uint16_t)(t5tOpComp->NDEF_TLV_LENGTH - bytes_read);
                        }

                        /* does received data still fit into the output-buffer ? */
                        if ((uint32_t)(bytes_to_copy) <= msg_buffer_size)
                        {
                            /* copy NDEF-message content to output buffer */
                            (void)memcpy(&msgBuffer[bytes_read], &t5tOpComp->RxBuffer[PTX_T5TOP_OFFSET_RES_DATA + init_block_read_offset], bytes_to_copy);
                            bytes_read = (uint16_t)(bytes_read + bytes_to_copy);

                        } else
                        {
                            status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_InsufficientResources);
                        }
                    }

                    init_block_read_offset = 0;
                    current_block_nr++;
                }

                /* everything correctly processed ? */
                if ((ptxStatus_Success == status) && (bytes_read == t5tOpComp->NDEF_TLV_LENGTH))
                {
                    *msgLen = (uint32_t)(t5tOpComp->NDEF_TLV_LENGTH);
                } else
                {
                    *msgLen = 0;
                }
            }

        } else
        {
            status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_InvalidParameter);
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNDEF_T5TOpWriteMessage (ptxNDEF_T5TOP_t *t5tOpComp, uint8_t *msgBuffer, uint32_t msgLen)
{
    ptxStatus_t status = ptxStatus_Success;
    uint8_t length_size;
    uint16_t bytes_written = 0;
    uint16_t current_block_nr = 0;
    uint32_t rx_len;
    uint8_t ndef_tlv_header_size;
    uint8_t init_block_write_offset = 0;
    uint8_t bytes_to_copy;
    uint16_t msg_len;
    uint8_t *msg_buffer;
    uint8_t EMPTY_NDEF_MESSAGE[] = {0xD0, 0x00, 0x00};

    if (PTX_COMP_CHECK(t5tOpComp, ptxStatus_Comp_T5TOP))
    {
        switch (t5tOpComp->LifeCycle)
        {
            case TagLC_Initialized:
            case TagLC_ReadWrite:
                /* OK */
                break;

            default:
                status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_InvalidState);
                break;
        }

        if (ptxStatus_Success == status)
        {
            /* set UID */
            status = ptxNativeTag_T5TSetUID(&t5tOpComp->NativeTagT5T,
                                            &t5tOpComp->CardRegistry->ActiveCard->TechParams.CardVParams.UID[0],
                                            PTX_T5T_UID_SIZE);
        }

        if (ptxStatus_Success == status)
        {
            /* determine NDEF-message to write */
            if ((NULL != msgBuffer) && (0 != msgLen))
            {
                msg_buffer = msgBuffer;
                msg_len = (uint16_t)msgLen;

            } else
            {
                msg_buffer = &EMPTY_NDEF_MESSAGE[0];
                msg_len = (uint16_t)sizeof(EMPTY_NDEF_MESSAGE);
            }

            length_size = (msg_len <= ((uint32_t)0xFE)) ? (uint8_t)0x01 : (uint8_t)0x03;
            ndef_tlv_header_size = (uint8_t)(length_size + 1);

            /* does NDEF-message fit at all onto the Tag ? */
            if (msgLen < (uint32_t)(t5tOpComp->CCParams.MLEN - (ndef_tlv_header_size + ((t5tOpComp->NDEF_TLV_POS_BN * t5tOpComp->BlockSize) + t5tOpComp->NDEF_TLV_POS_BY))))
            {
                /* reset L-field */
                status = ptxNDEF_T5TOpUpdateLength (t5tOpComp, 0, length_size);

                if (ptxStatus_Success == status)
                {
                    /*
                     * determine block-adress to start writing (NDEF_TLV_POS_BN defines the block where the NDEF-TLV starts,
                     * but not the actual NDEF-message, which could be stored in the next block;
                     */
                    current_block_nr = t5tOpComp->NDEF_TLV_POS_BN;
                    if ((uint8_t)((t5tOpComp->NDEF_TLV_POS_BY + ndef_tlv_header_size)) < t5tOpComp->BlockSize)
                    {
                        init_block_write_offset = (uint8_t)(t5tOpComp->NDEF_TLV_POS_BY + ndef_tlv_header_size);

                    } else
                    {
                        init_block_write_offset = (uint8_t)((t5tOpComp->NDEF_TLV_POS_BY + ndef_tlv_header_size) % t5tOpComp->BlockSize);
                        current_block_nr++;
                    }

                    /* if the offet != 0, we have to read the current block first */
                    if (0 != init_block_write_offset)
                    {
                        /* read block */
                        status = ptxNDEF_T5TOpReadBlock(t5tOpComp, current_block_nr, &t5tOpComp->RxBuffer[0], &rx_len);

                        if ((ptxStatus_Success == status) && (rx_len > 1))
                        {
                            (void)memcpy(&t5tOpComp->WorkBuffer[0], &t5tOpComp->RxBuffer[PTX_T5TOP_OFFSET_RES_DATA], (uint32_t)(rx_len - 1));
                        }
                    }
                }

            } else
            {
                status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_InsufficientResources);
            }

            while ((bytes_written != msg_len) && (bytes_written < (uint16_t)(t5tOpComp->CCParams.MLEN - ndef_tlv_header_size)) && (ptxStatus_Success == status))
            {
                /* prepare NDEF-data for next write-cycle */
                if (((uint32_t)(msg_len - bytes_written - init_block_write_offset) > (uint32_t)(t5tOpComp->BlockSize)))
                {
                    bytes_to_copy = (uint8_t)(t5tOpComp->BlockSize - init_block_write_offset);

                } else
                {
                    bytes_to_copy = (uint8_t)(msg_len - bytes_written - init_block_write_offset);

                    /* last (or first and only) block to be written completely or only partly ? */
                    if ((((uint32_t)(bytes_to_copy + bytes_written) == msgLen)) && (bytes_to_copy != t5tOpComp->BlockSize))
                    {
                        status = ptxNDEF_T5TOpReadBlock(t5tOpComp, current_block_nr, &t5tOpComp->RxBuffer[0], &rx_len);

                        if ((ptxStatus_Success == status) && (rx_len-1 >= init_block_write_offset))
                        {
                            (void)memcpy(&t5tOpComp->WorkBuffer[init_block_write_offset],
                                         &t5tOpComp->RxBuffer[PTX_T5TOP_OFFSET_RES_DATA],
                                         (uint32_t)(rx_len - 1 - init_block_write_offset));
                        }
                    }
                }

                if (ptxStatus_Success == status)
                {
                    uint16_t temp_var = (uint16_t)(bytes_to_copy + init_block_write_offset);
                    if(temp_var <= (uint16_t)PTX_T5TOP_INTERNAL_BUFLENGTH)
                    {
                        (void)memcpy(&t5tOpComp->WorkBuffer[init_block_write_offset], &msg_buffer[bytes_written], bytes_to_copy);
                    } else
                    {
                        status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_InternalError);
                    }
                }

                if (ptxStatus_Success == status)
                {
                    /* enough space left to store the (optional) TERMINATOR-TLV ? */
                    if ((((uint32_t)(bytes_to_copy + bytes_written) == msgLen)) && (bytes_to_copy < t5tOpComp->BlockSize))
                    {
                        t5tOpComp->WorkBuffer[bytes_to_copy] = PTX_T5TOP_TERMINATOR_TLV_T;
                    }

                    /* write data to Tag */
                    status = ptxNDEF_T5TOpWriteBlock(t5tOpComp, current_block_nr, &t5tOpComp->WorkBuffer[0], t5tOpComp->BlockSize, &t5tOpComp->RxBuffer[0], &rx_len);

                    if (ptxStatus_Success == status)
                    {
                        bytes_written = (uint16_t)(bytes_written + bytes_to_copy);
                        init_block_write_offset = 0;
                        current_block_nr++;
                    }
                }
            }

            if (ptxStatus_Success == status)
            {
                /* set intended size of L-field */
                status = ptxNDEF_T5TOpUpdateLength (t5tOpComp, msg_len, length_size);
            }
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNDEF_T5TOpLockTag (ptxNDEF_T5TOP_t *t5tOpComp)
{
    ptxStatus_t status = ptxStatus_Success;
    uint16_t current_block_nr = 0;
    uint32_t rx_len;
    uint16_t nr_blocks_to_lock;

    if (PTX_COMP_CHECK(t5tOpComp, ptxStatus_Comp_T5TOP))
    {
        switch (t5tOpComp->LifeCycle)
        {
            case TagLC_ReadWrite:
                /* transition to READ-ONLY i.e. locked-state can only be done if the Tag supports the Lock-commands */
                if (0 == t5tOpComp->CCParams.LockBlockSupported)
                {
                    status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_NotPermitted);
                }
                break;

            default:
                status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_InvalidState);
                break;
        }

        if (ptxStatus_Success == status)
        {
            /* set UID */
            status = ptxNativeTag_T5TSetUID(&t5tOpComp->NativeTagT5T,
                                            &t5tOpComp->CardRegistry->ActiveCard->TechParams.CardVParams.UID[0],
                                            PTX_T5T_UID_SIZE);
        }

        if (ptxStatus_Success == status)
        {
            /* clear write-access by reading block 0 (CC) and update the "Version & Access-Condition"-field at offset 2  */
            status = ptxNDEF_T5TOpReadBlock(t5tOpComp, 0, &t5tOpComp->RxBuffer[0], &rx_len);

            if (ptxStatus_Success == status)
            {
                /* skip status-flag at offset 0 */
                rx_len--;

                (void)memcpy(&t5tOpComp->WorkBuffer[0], &t5tOpComp->RxBuffer[PTX_T5TOP_OFFSET_RES_DATA], (uint32_t)(rx_len));

                /* clear write-access bits */
                t5tOpComp->WorkBuffer[PTX_T5TOP_CC_OFFSET_VERSION_ACCESS] = (uint8_t)(t5tOpComp->WorkBuffer[PTX_T5TOP_CC_OFFSET_VERSION_ACCESS] & ~PTX_T5TOP_CC_WRITE_ACCESS_MASK);

                /* update block 0 */
                status = ptxNDEF_T5TOpWriteBlock(t5tOpComp, 0, &t5tOpComp->WorkBuffer[0], t5tOpComp->BlockSize, &t5tOpComp->RxBuffer[0], &rx_len);
            }

            if (ptxStatus_Success == status)
            {
                /* pyhsically lock every available block (Attention: THIS IS NOT REVERSIBLE!) */
                nr_blocks_to_lock = (uint16_t)(t5tOpComp->CCParams.MLEN / t5tOpComp->BlockSize);

                for (current_block_nr = 0; current_block_nr < nr_blocks_to_lock; current_block_nr++)
                {
                    /* lock block-by-block */
                    status = ptxNDEF_T5TOpLockBlock (t5tOpComp, current_block_nr, &t5tOpComp->RxBuffer[0], &rx_len);

                    if (ptxStatus_Success != status)
                    {
                        break;
                    }
                }
            }
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNDEF_T5TOpClose (ptxNDEF_T5TOP_t *t5tOpComp)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(t5tOpComp, ptxStatus_Comp_T5TOP))
    {
        status = ptxNativeTag_T5TClose(&t5tOpComp->NativeTagT5T);

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

/*
 * ####################################################################################################################
 * INTERNAL FUNCTIONS / CALLBACK(s)
 * ####################################################################################################################
 */
static ptxStatus_t ptxNDEF_T5TGetCapabilityContainerInfo (ptxNDEF_T5TOP_t *t5tOpComp)
{
    ptxStatus_t status = ptxStatus_Success;
    uint32_t rx_len;

    if (PTX_COMP_CHECK(t5tOpComp, ptxStatus_Comp_T5TOP))
    {
        /* reset current CC-parameters */
        (void)memset(&t5tOpComp->CCParams, 0, sizeof(ptxNDEF_T5TOP_CC_t));

        /* read CC-block */
        status = ptxNDEF_T5TOpReadBlock(t5tOpComp, PTX_T5TOP_CC_BLOCK_NUMBER, &t5tOpComp->RxBuffer[0], &rx_len);

        if (ptxStatus_Success == status)
        {
            t5tOpComp->BlockSize = (uint8_t)(rx_len - 1);

            switch (t5tOpComp->BlockSize)
            {
                case (uint8_t)4:
                case (uint8_t)8:
                case (uint8_t)16:
                case (uint8_t)32:
                    /* everything OK */
                    break;

                default:
                    t5tOpComp->LifeCycle = TagLC_NoNDEFTag;
                    status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_ProtocolError);
                    break;
            }

            if (ptxStatus_Success == status)
            {
                /*
                 * Note: The CC consists either of
                 *       - 4 bytes (MLEN-parameter at offset 3 != 0) or
                 *       - 8 bytes (MLEN-parameter at offset 3 == 0).
                 *
                 *       If the block-size is set to 4 but the CC consists of 8 byte, a 2nd read-operation is required.
                 */

                /* copy first 4 bytes of CC as they're valid in any case */
                (void)memcpy(&t5tOpComp->WorkBuffer[0], &t5tOpComp->RxBuffer[PTX_T5TOP_OFFSET_RES_DATA], (uint32_t)4);
                t5tOpComp->CCParams.Size = (uint8_t)4;

                /* 2nd read-operation required ? */
                if (0 == t5tOpComp->WorkBuffer[PTX_T5TOP_CC_OFFSET_MLEN_BASIC])
                {
                    t5tOpComp->CCParams.Size = (uint8_t)8;

                    if ((uint8_t)4 == t5tOpComp->BlockSize)
                    {
                        status = ptxNDEF_T5TOpReadBlock(t5tOpComp, (uint16_t)(PTX_T5TOP_CC_BLOCK_NUMBER + 1), &t5tOpComp->RxBuffer[0], &rx_len);

                        if (ptxStatus_Success == status)
                        {
                            /* copy 2nd half of CC from already available data */
                            (void)memcpy(&t5tOpComp->WorkBuffer[4], &t5tOpComp->RxBuffer[PTX_T5TOP_OFFSET_RES_DATA], (uint32_t)4);
                        }
                    } else
                    {
                        /* copy 2nd half of CC from already available data */
                        (void)memcpy(&t5tOpComp->WorkBuffer[4], &t5tOpComp->RxBuffer[PTX_T5TOP_OFFSET_RES_DATA + 4], (uint32_t)4);
                    }
                }
            }

            /* parse CC-data */
            if (ptxStatus_Success == status)
            {
                t5tOpComp->CCParams.MagicNumber = t5tOpComp->WorkBuffer[PTX_T5TOP_CC_OFFSET_MAGIC_NUMBER];

                /* NDEF-Tag at all ?*/
                switch (t5tOpComp->CCParams.MagicNumber)
                {
                    case PTX_T5TOP_CC_MAGIC_NUMBER_E1:
                        /* OK - nothing to do */
                        break;

                    case PTX_T5TOP_CC_MAGIC_NUMBER_E2:
                        /* OK - EXT_*-commands are required for block-addresses above 0xFF */
                        t5tOpComp->CCParams.ExtCommandTypeRequired = (uint8_t)0x01;
                        break;

                    default:
                        /* not ready for NDEF - update Life-Cycle and cancel operation */
                        t5tOpComp->LifeCycle = TagLC_NoNDEFTag;
                        status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_ProtocolError);
                        break;
                }

                /* check supported mapping version and set Read-/Write-Access information */
                if (ptxStatus_Success == status)
                {
                    t5tOpComp->CCParams.Version = (uint8_t)((t5tOpComp->WorkBuffer[PTX_T5TOP_CC_OFFSET_VERSION_ACCESS] & PTX_T5TOP_CC_VERSION_MASK) >> 2);

                    if (t5tOpComp->CCParams.Version <= PTX_T5T_SUPPORTED_VERSION)
                    {
                        if (0 == (t5tOpComp->WorkBuffer[PTX_T5TOP_CC_OFFSET_VERSION_ACCESS] & PTX_T5TOP_CC_READ_ACCESS_MASK))
                        {
                            t5tOpComp->CCParams.ReadAccess = (uint8_t)0x01;
                        }

                        if (0 == (t5tOpComp->WorkBuffer[PTX_T5TOP_CC_OFFSET_VERSION_ACCESS] & PTX_T5TOP_CC_WRITE_ACCESS_MASK))
                        {
                            t5tOpComp->CCParams.WriteAccess = (uint8_t)0x01;
                        }

                    } else
                    {
                        status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_ProtocolError);
                    }
                }

                /* set memory-length (MLEN) and feature flags */
                if (ptxStatus_Success == status)
                {
                    if (0 != t5tOpComp->WorkBuffer[PTX_T5TOP_CC_OFFSET_MLEN_BASIC])
                    {
                        t5tOpComp->CCParams.MLEN = (uint16_t)(t5tOpComp->WorkBuffer[PTX_T5TOP_CC_OFFSET_MLEN_BASIC] * 8);

                    } else
                    {
                        t5tOpComp->CCParams.MLEN = (uint16_t)(((t5tOpComp->WorkBuffer[PTX_T5TOP_CC_OFFSET_MLEN_EXTENDED + 0] << 8) | \
                                                                t5tOpComp->WorkBuffer[PTX_T5TOP_CC_OFFSET_MLEN_EXTENDED + 1]) * 8);
                    }

                    if (0 != (t5tOpComp->WorkBuffer[PTX_T5TOP_CC_OFFSET_ADDITIONAL_FEATURE] & PTX_T5TOP_CC_ADD_INFO_SPECIAL_FRAME_MASK))
                    {
                        t5tOpComp->CCParams.SpecialFrameRequired = (uint8_t)0x01;
                    }

                    if (0 != (t5tOpComp->WorkBuffer[PTX_T5TOP_CC_OFFSET_ADDITIONAL_FEATURE] & PTX_T5TOP_CC_ADD_INFO_LOCK_BLOCK_MASK))
                    {
                        t5tOpComp->CCParams.LockBlockSupported = (uint8_t)0x01;
                    }

                    if (0 != (t5tOpComp->WorkBuffer[PTX_T5TOP_CC_OFFSET_ADDITIONAL_FEATURE] & PTX_T5TOP_CC_ADD_INFO_MBREAD_MASK))
                    {
                        t5tOpComp->CCParams.MultiBlockReadSupported = (uint8_t)0x01;
                    }
                }
            }
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

static ptxStatus_t ptxNDEF_T5TOpReadBlock (ptxNDEF_T5TOP_t *t5tOpComp, uint16_t blockNumber, uint8_t *rx, uint32_t *rxLen)
{
    ptxStatus_t status = ptxStatus_Success;

    /* Aside from actual Tag-component, remaining parameters are checked at lower-layers */
    if (PTX_COMP_CHECK(t5tOpComp, ptxStatus_Comp_T5TOP) && (NULL != rxLen))
    {
        *rxLen = (uint32_t)t5tOpComp->RxBufferSize;

        /* send standard- or Extended-Command depending on Block-Number */
        if (0 == t5tOpComp->CCParams.ExtCommandTypeRequired)
        {
            status = ptxNativeTag_T5TReadSingleBlock (&t5tOpComp->NativeTagT5T,
                                                      0,
                                                      (uint8_t)blockNumber,
                                                      rx,
                                                      rxLen,
                                                      PTX_T5T_DEFAULT_TIMEOUT_MS);

        } else
        {
            status = ptxNativeTag_T5TExtReadSingleBlock (&t5tOpComp->NativeTagT5T,
                                                         0,
                                                         blockNumber,
                                                         rx,
                                                         rxLen,
                                                         PTX_T5T_DEFAULT_TIMEOUT_MS);
        }

        /* check response (if applicable); complete packet incl. RES_FLAG return to calling function */
        if (ptxStatus_Success == status)
        {
            if (0 != *rxLen)
            {
                if (0 != (rx[PTX_T5TOP_OFFSET_RES_FLAG] & PTX_T5TOP_RES_FLAG_ERROR_MASK))
                {
                    status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_ProtocolError);
                }
            } else
            {
                status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_NscRfError);
            }
        }
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

static ptxStatus_t ptxNDEF_T5TOpWriteBlock (ptxNDEF_T5TOP_t *t5tOpComp, uint16_t blockNumber, uint8_t *blockData, uint8_t blockDataLen, uint8_t *rx, uint32_t *rxLen)
{
    ptxStatus_t status = ptxStatus_Success;

    /* Aside from actual Tag-component, remaining parameters are checked at lower-layers */
    if (PTX_COMP_CHECK(t5tOpComp, ptxStatus_Comp_T5TOP))
    {
        *rxLen = (uint32_t)t5tOpComp->RxBufferSize;

        /* send standard- or Extended-Command depending on Block-Number */
        if (0 == t5tOpComp->CCParams.ExtCommandTypeRequired)
        {
            status = ptxNativeTag_T5TWriteSingleBlock (&t5tOpComp->NativeTagT5T,
                                                       t5tOpComp->CCParams.SpecialFrameRequired,
                                                       (uint8_t)blockNumber,
                                                       blockData,
                                                       blockDataLen,
                                                       rx,
                                                       rxLen,
                                                       PTX_T5T_DEFAULT_TIMEOUT_MS);

        } else
        {
            status = ptxNativeTag_T5TExtWriteSingleBlock (&t5tOpComp->NativeTagT5T,
                                                          t5tOpComp->CCParams.SpecialFrameRequired,
                                                          blockNumber,
                                                          blockData,
                                                          blockDataLen,
                                                          rx,
                                                          rxLen,
                                                          PTX_T5T_DEFAULT_TIMEOUT_MS);
        }

        /* check response (if applicable); complete packet incl. RES_FLAG return to calling function */
        if (ptxStatus_Success == status)
        {
            if (0 != *rxLen)
            {
                if (0 != (rx[PTX_T5TOP_OFFSET_RES_FLAG] & PTX_T5TOP_RES_FLAG_ERROR_MASK))
                {
                    status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_ProtocolError);
                }
            } else
            {
                status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_NscRfError);
            }
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

static ptxStatus_t ptxNDEF_T5TOpLockBlock (ptxNDEF_T5TOP_t *t5tOpComp, uint16_t blockNumber, uint8_t *rx, uint32_t *rxLen)
{
    ptxStatus_t status = ptxStatus_Success;

    /* Aside from actual Tag-component, remaining parameters are checked at lower-layers */
    if (PTX_COMP_CHECK(t5tOpComp, ptxStatus_Comp_T5TOP))
    {
        /* send standard- or Extended-Command depending on Block-Number */
        if (0 == t5tOpComp->CCParams.ExtCommandTypeRequired)
        {
            status = ptxNativeTag_T5TLockSingleBlock (&t5tOpComp->NativeTagT5T,
                                                       t5tOpComp->CCParams.SpecialFrameRequired,
                                                       (uint8_t)blockNumber,
                                                       rx,
                                                       rxLen,
                                                       PTX_T5T_DEFAULT_TIMEOUT_MS);

        } else
        {
            status = ptxNativeTag_T5TExtLockSingleBlock (&t5tOpComp->NativeTagT5T,
                                                          t5tOpComp->CCParams.SpecialFrameRequired,
                                                          blockNumber,
                                                          rx,
                                                          rxLen,
                                                          PTX_T5T_DEFAULT_TIMEOUT_MS);
        }

        /* check response (if applicable); complete packet incl. RES_FLAG return to calling function */
        if (ptxStatus_Success == status)
        {
            if (0 != *rxLen)
            {
                if (0 != (rx[PTX_T5TOP_OFFSET_RES_FLAG] & PTX_T5TOP_RES_FLAG_ERROR_MASK))
                {
                    status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_ProtocolError);
                }
            } else
            {
                status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_NscRfError);
            }
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

static ptxStatus_t ptxNDEF_T5TOpUpdateLength (ptxNDEF_T5TOP_t *t5tOpComp, uint16_t messageLen, uint8_t lengthSize)
{
    ptxStatus_t status = ptxStatus_Success;
    uint16_t current_block_nr;
    uint32_t rx_len;
    uint8_t i;
    uint8_t nr_blocks_to_process;
    uint8_t bytes_processed = 0;

    /* Aside from actual Tag-component, remaining parameters are checked at lower-layers */
    if (PTX_COMP_CHECK(t5tOpComp, ptxStatus_Comp_T5TOP) && (((uint8_t)1 == lengthSize) || ((uint8_t)3 == lengthSize)))
    {
        switch (t5tOpComp->LifeCycle)
        {
            case TagLC_Initialized:
            case TagLC_ReadWrite:
                /* OK */
                break;

            default:
                status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_InvalidState);
                break;
        }

        if (ptxStatus_Success == status)
        {
            /* does L-field completely fit into current block ? */
            if ((uint8_t)(t5tOpComp->NDEF_TLV_POS_BY + lengthSize) < t5tOpComp->BlockSize)
            {
                current_block_nr = t5tOpComp->NDEF_TLV_POS_BN;
                nr_blocks_to_process = (uint8_t)1;

            } else
            {
                /* does the L-field start at the next block i.e. is the T-field byte the last one of the current block ? */
                if (t5tOpComp->NDEF_TLV_POS_BY == (uint8_t)(t5tOpComp->BlockSize - 1))
                {
                    current_block_nr = (uint8_t)(t5tOpComp->NDEF_TLV_POS_BN + 1);
                    nr_blocks_to_process = (uint8_t)1;

                } else
                {
                    /* L-field is split between current and next block */
                    current_block_nr = t5tOpComp->NDEF_TLV_POS_BN;
                    nr_blocks_to_process = (uint8_t)2;
                }
            }

            /* read n-blocks to update the length field at once */
            bytes_processed = 0;
            for (i = 0; i < nr_blocks_to_process; i++)
            {
                status = ptxNDEF_T5TOpReadBlock(t5tOpComp, current_block_nr, &t5tOpComp->RxBuffer[0], &rx_len);

                if (ptxStatus_Success == status)
                {
                    /* skip status-flag at offset 0 */
                    rx_len--;

                    (void)memcpy(&t5tOpComp->WorkBuffer[bytes_processed], &t5tOpComp->RxBuffer[PTX_T5TOP_OFFSET_RES_DATA], rx_len);
                    bytes_processed = (uint8_t)(bytes_processed + rx_len);
                }
            }

            /* update L-field */
            if  ((uint8_t)1 == lengthSize)
            {
                t5tOpComp->WorkBuffer[(t5tOpComp->NDEF_TLV_POS_BY + 1) % t5tOpComp->BlockSize] = (uint8_t)messageLen;

            } else
            {
                t5tOpComp->WorkBuffer[(t5tOpComp->NDEF_TLV_POS_BY + 1) % t5tOpComp->BlockSize] = (uint8_t)0xFF;
                t5tOpComp->WorkBuffer[(t5tOpComp->NDEF_TLV_POS_BY + 2) % t5tOpComp->BlockSize] = (uint8_t)((messageLen & 0xFF00) >> 8);
                t5tOpComp->WorkBuffer[(t5tOpComp->NDEF_TLV_POS_BY + 3) % t5tOpComp->BlockSize] = (uint8_t)((messageLen & 0x00FF) >> 0);
            }

            /* write L-field */
            bytes_processed = 0;
            for (i = 0; i < nr_blocks_to_process; i++)
            {
                status = ptxNDEF_T5TOpWriteBlock(t5tOpComp, current_block_nr, &t5tOpComp->WorkBuffer[bytes_processed], t5tOpComp->BlockSize, &t5tOpComp->RxBuffer[0], &rx_len);

                if (ptxStatus_Success == status)
                {
                    bytes_processed = (uint8_t)(bytes_processed + t5tOpComp->BlockSize);
                    current_block_nr++;
                } else
                {
                    /* error - skip further write-operations */
                    break;
                }
            }
        }

        /* if everything went OK, update the TLV-length of the message */
        if (ptxStatus_Success == status)
        {
            t5tOpComp->NDEF_TLV_LENGTH = messageLen;
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

