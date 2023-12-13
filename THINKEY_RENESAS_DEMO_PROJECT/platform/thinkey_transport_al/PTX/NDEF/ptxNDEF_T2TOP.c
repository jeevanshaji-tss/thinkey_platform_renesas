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
    File        : ptxNativeTag_T2T.c

    Description : Native Tag API for NFC Forum Tag Type 2 (IOT READER - Extension)
*/


/*
 * ####################################################################################################################
 * INCLUDES
 * ####################################################################################################################
 */
#include "ptx_IOT_READER.h"
#include "ptxNDEF_T2TOP.h"
#include "ptxNativeTag_T2T.h"
#include <string.h>

/*
 * ####################################################################################################################
 * DEFINES / TYPES
 * ####################################################################################################################
 */
#define PTX_T2TOP_STATIC_LOCK_BLOCK_NUMBER                  (uint8_t)0x02
#define PTX_T2TOP_CC_BLOCK_NUMBER                           (uint8_t)0x03
#define PTX_T2TOP_TLV_AREA_BEGIN_BLOCK                      (uint16_t)0x0004

#define PTX_T2TOP_CC_OFFSET_MAGIC_NUMBER                    (uint8_t)0x00
#define PTX_T2TOP_CC_OFFSET_VERSION_INFO                    (uint8_t)0x01
#define PTX_T2TOP_CC_OFFSET_TAG_SIZE                        (uint8_t)0x02
#define PTX_T2TOP_CC_OFFSET_ACCESS_COND                     (uint8_t)0x03

#define PTX_T2TOP_MASK_8                                    (uint8_t)0x08
#define PTX_T2TOP_MASK_4                                    (uint8_t)0x04
#define PTX_T2TOP_MASK_1                                    (uint8_t)0x01

#define PTX_T2TOP_CC_MAGIC_NUMBER                           (uint8_t)0xE1

#define PTX_T2TOP_NULL_TLV_T                                (uint8_t)0x00
#define PTX_T2TOP_LOCK_CONTROL_TLV_T                        (uint8_t)0x01
#define PTX_T2TOP_MEMORY_CONTROL_TLV_T                      (uint8_t)0x02
#define PTX_T2TOP_NDEF_TLV_T                                (uint8_t)0x03
#define PTX_T2TOP_TERMINATOR_TLV_T                          (uint8_t)0xFE

#define PTX_T2TOP_LOCK_CONTROL_TLV_LENGTH                   (uint8_t)0x03
#define PTX_T2TOP_MEMORY_CONTROL_TLV_LENGTH                 (uint8_t)0x03
#define PTX_T2TOP_TERMINATOR_TLV_LENGTH                     (uint8_t)0x01

#define PTX_T2TOP_BLOCK_SIZE                                (uint8_t)0x04
#define PTX_T2TOP_ACK_CODE                                  (uint8_t)0x0A
#define PTX_T2TOP_MIN_SIZE_FOR_DLA                          (uint8_t)0x30

#define PTX_T2TOP_INTERNAL_BUFLENGTH                        (uint8_t)64

/*
 * ####################################################################################################################
 * INTERNAL FUNCTIONS / HELPERS
 * ####################################################################################################################
 */
static ptxStatus_t ptxNDEF_T2TGetCapabilityContainerInfo (ptxNDEF_T2TOP_t *t2tOpComp);
static ptxStatus_t ptxNDEF_T2TOpReadBlocks  (ptxNDEF_T2TOP_t *t2tOpComp, uint8_t blockNumber, uint8_t *rx, uint32_t *rxLen);
static ptxStatus_t ptxNDEF_T2TOpWriteBlock  (ptxNDEF_T2TOP_t *t2tOpComp, uint8_t blockNumber, uint8_t *blockData, uint8_t blockDataLen, uint8_t *rx, uint32_t *rxLen, uint32_t msTimeout);
static ptxStatus_t ptxNDEF_T2TOpSectorSelect(ptxNDEF_T2TOP_t *t2tOpComp, uint8_t secNr, uint8_t *rx, uint32_t *rxLen, uint32_t msTimeout);
static ptxStatus_t ptxNDEF_T2TTLVCrawler    (ptxNDEF_T2TOP_t *t2tOpComp, uint16_t currentBlock);
static ptxStatus_t ptxNDEF_T2TUpdateLength  (ptxNDEF_T2TOP_t *t2tOpComp, uint16_t msgLen, uint8_t lengthSize);
static ptxStatus_t ptxNDEF_T2TDLAProcessor  (ptxNDEF_T2TOP_t *t2tOpComp);
static ptxStatus_t ptxNDEF_T2TRAProcessor   (ptxNDEF_T2TOP_t *t2tOpComp);
static ptxStatus_t ptxNDEF_T2TAreaChecker   (ptxNDEF_T2TOP_t *t2tOpComp, uint16_t currentByteAddress, uint16_t *bytesToProcess, uint16_t *preserveBytes);

/*
 * ####################################################################################################################
 * API FUNCTIONS
 * ####################################################################################################################
 */
ptxStatus_t ptxNDEF_T2TOpOpen (ptxNDEF_T2TOP_t *t2tOpComp, ptxNDEF_T2TOP_InitParams_t *initParams)
{
    ptxStatus_t status = ptxStatus_Success;
    ptxNativeTag_T2T_InitParams_t t2t_init_params;

    if ((NULL != t2tOpComp) && (NULL != initParams))
    {
        if ((NULL != initParams->IotRd) &&
            (NULL != initParams->TxBuffer) &&
            (0 != initParams->TxBufferSize) &&
            (NULL != initParams->RxBuffer) &&
            (0 != initParams->RxBufferSize))
        {
            /* clear component */
            (void)memset(t2tOpComp, 0, sizeof(ptxNDEF_T2TOP_t));

            /* set members */
            (void)ptxIoTRd_Get_Card_Registry (initParams->IotRd, &t2tOpComp->CardRegistry);

            t2tOpComp->RxBuffer = initParams->RxBuffer;
            t2tOpComp->RxBufferSize = initParams->RxBufferSize;
            t2tOpComp->LifeCycle = TagLC_NoNDEFTag;

            /* initialize lower layer component */
            (void)memset(&t2t_init_params, 0, sizeof(ptxNativeTag_T2T_InitParams_t));
            t2t_init_params.IotRd = initParams->IotRd;
            t2t_init_params.TxBuffer = initParams->TxBuffer;
            t2t_init_params.TxBufferSize = initParams->TxBufferSize;

            status = ptxNativeTag_T2TOpen(&t2tOpComp->NativeTagT2T, &t2t_init_params);

            /* set Component-ID at the end to prevent futher calls in case of an error */
            if (ptxStatus_Success == status)
            {
                t2tOpComp->CompId = ptxStatus_Comp_T2TOP;
            }

        } else
        {
            status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_InvalidParameter);
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNDEF_T2TOpFormatTag (ptxNDEF_T2TOP_t *t2tOpComp)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(t2tOpComp, ptxStatus_Comp_T2TOP))
    {
        status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_NotImplemented);

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNDEF_T2TOpCheckMessage (ptxNDEF_T2TOP_t *t2tOpComp)
{
    ptxStatus_t status = ptxStatus_Success;
    uint16_t current_block = (uint16_t)PTX_T2TOP_TLV_AREA_BEGIN_BLOCK; //BlAdd
    uint8_t current_block_number; //BlNo
    uint8_t current_sector = 0;
    uint32_t rx_len;
    uint16_t non_ndef_bytes = 0;

    /* reset TLV data */
    (void)memset(&t2tOpComp->TLVs, 0, sizeof(ptxNDEF_T2TOP_TLV_t));

    if (PTX_COMP_CHECK(t2tOpComp, ptxStatus_Comp_T2TOP))
    {
        if (ptxStatus_Success == status)
        {
            status = ptxNDEF_T2TGetCapabilityContainerInfo(t2tOpComp);
        }
        if (ptxStatus_Success == status)
        {
            switch (t2tOpComp->CCParams.MagicNumber)
            {
            case PTX_T2TOP_CC_MAGIC_NUMBER:
                t2tOpComp->LifeCycle = TagLC_Initialized;
                break;
            default:
                t2tOpComp->LifeCycle = TagLC_NoNDEFTag;
                status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_ProtocolError);
                break;
            }
            if (ptxStatus_Success == status)
            {
                switch (t2tOpComp->CCParams.ReadAccess)
                {
                case (uint8_t)0x00:
                    break;

                default:
                    status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_AccessDenied);
                }
            }
            if (ptxStatus_Success == status)
            {
                switch (t2tOpComp->CCParams.WriteAccess)
                {
                case (uint8_t)0x00:
                    t2tOpComp->LifeCycle = TagLC_ReadWrite;
                    break;
                case (uint8_t)0xFF:
                    t2tOpComp->LifeCycle = TagLC_ReadOnly;
                    break;

                default:
                    break;
                }
            }

            /* read the whole tag and look for TLVs */

            /* if only one sector (less than or equal to 256 blocks) is present */
            if ((ptxStatus_Success == status) && ((t2tOpComp->CCParams.NumberOfBlocks) <= 0xFF))
            {
                while ((current_block < t2tOpComp->CCParams.NumberOfBlocks) && (ptxStatus_Success == status) && (0 == t2tOpComp->TLVs.TerminatorTLV.TerminatorTLVFound))
                {
                    status = ptxNDEF_T2TOpReadBlocks(t2tOpComp, (uint8_t)current_block, &t2tOpComp->RxBuffer[0], &rx_len);
                    (void)memcpy(&t2tOpComp->WorkBuffer[0], &t2tOpComp->RxBuffer[0], (uint32_t)16);
                    if (ptxStatus_Success == status)
                    {
                        status = ptxNDEF_T2TTLVCrawler(t2tOpComp, current_block);
                    }
                    current_block = (uint16_t)(current_block + 4);
                }
            }

            /* if more than one sectors (more than 256 blocks) are present */
            if ((ptxStatus_Success == status) && ((t2tOpComp->CCParams.NumberOfBlocks) > 0xFF))
            {
                while ((current_block < t2tOpComp->CCParams.NumberOfBlocks) && (ptxStatus_Success == status) && (0 == t2tOpComp->TLVs.TerminatorTLV.TerminatorTLVFound))
                {
                    current_block_number = (uint8_t)(current_block % 256);
                    current_sector = (uint8_t)(current_block / 256);

                    if (t2tOpComp->SectorParams.CurrentSector != current_sector)
                    {
                        status = ptxNDEF_T2TOpSectorSelect(t2tOpComp, current_sector, &t2tOpComp->RxBuffer[0], &rx_len, PTX_T2T_DEFAULT_TIMEOUT_MS);
                    }

                    if (ptxStatus_Success == status)
                    {
                        status = ptxNDEF_T2TOpReadBlocks(t2tOpComp, current_block_number, &t2tOpComp->RxBuffer[0], &rx_len);
                        (void)memcpy(&t2tOpComp->WorkBuffer[0], &t2tOpComp->RxBuffer[0], (uint32_t)16);
                        if (ptxStatus_Success == status)
                        {
                            status = ptxNDEF_T2TTLVCrawler(t2tOpComp, current_block);
                        }
                        current_block = (uint16_t)(current_block + 4);
                    }
                }
            }
            /* find NDEF available space */
            if (ptxStatus_Success == status)
            {
                status = ptxNDEF_T2TDLAProcessor(t2tOpComp);
                if (ptxStatus_Success == status)
                {
                    non_ndef_bytes = (uint16_t)(non_ndef_bytes + (t2tOpComp->TLVs.LockControlTLVs.NumberOfTLVs * (PTX_T2TOP_LOCK_CONTROL_TLV_LENGTH + 2))); /* X (fixed) value bytes, 1 T and 1 L byte per TLV */
                    for (uint8_t i = 0; i < (t2tOpComp->TLVs.LockControlTLVs.NumberOfTLVs * 2); i=(uint8_t)(i+2))
                    {
                        non_ndef_bytes = (uint16_t)(non_ndef_bytes + (t2tOpComp->TLVs.LockControlTLVs.LockArea[i+1] - t2tOpComp->TLVs.LockControlTLVs.LockArea[i]));
                    }

                    status = ptxNDEF_T2TRAProcessor(t2tOpComp);
                    if (ptxStatus_Success == status)
                    {
                        non_ndef_bytes = (uint16_t)(non_ndef_bytes + (t2tOpComp->TLVs.MemoryControlTLVs.NumberOfTLVs * (PTX_T2TOP_MEMORY_CONTROL_TLV_LENGTH + 2))); // X (fixed) value bytes, 1 T and 1 L byte per TLV
                        for (uint8_t i = 0; i < (t2tOpComp->TLVs.MemoryControlTLVs.NumberOfTLVs * 2); i=(uint8_t)(i+2))
                        {
                            non_ndef_bytes = (uint16_t)(non_ndef_bytes + (t2tOpComp->TLVs.MemoryControlTLVs.RsvdArea[i+1] - t2tOpComp->TLVs.MemoryControlTLVs.RsvdArea[i]));
                        }

                        if (0 != t2tOpComp->TLVs.TerminatorTLV.TerminatorTLVFound)
                        {
                            non_ndef_bytes++;
                        }

                        t2tOpComp->TLVs.NDEFTLV.AvailableLength = (uint16_t)(t2tOpComp->CCParams.Size - non_ndef_bytes);
                    }
                }
            }
        } else
        {
            /* nothing to do, return error status */
        }
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNDEF_T2TOpReadMessage (ptxNDEF_T2TOP_t *t2tOpComp, uint8_t *msgBuffer, uint32_t *msgLen)
{
    ptxStatus_t status = ptxStatus_Success;
    uint8_t current_block;
    uint16_t current_block_address;
    uint16_t current_byte_address;
    uint16_t previous_sector = 0;
    uint16_t current_sector = 0;
    uint8_t block_offset;
    uint16_t bytes_to_read;
    uint16_t bytes_read = 0;
    uint16_t delete_offset;
    uint32_t rx_len;
    uint8_t *msg_buffer;
    uint32_t msg_buffer_size;
    uint16_t msg_length = 0;
    uint8_t ndef_bytes_to_jump;
    uint16_t preserve_bytes[16]; /* reset every operation, read max of 16 */
    uint8_t end_offset = 0;
    uint8_t ndef_tlv_offset;


    if (PTX_COMP_CHECK(t2tOpComp, ptxStatus_Comp_T2TOP) && (NULL != msgBuffer) && (NULL != msgLen))
    {
        msg_buffer_size = *msgLen;
        if (0 != msg_buffer_size)
        {
            msg_buffer = msgBuffer;

            switch (t2tOpComp->LifeCycle)
            {
            case TagLC_Initialized:
            case TagLC_ReadWrite:
            case TagLC_ReadOnly:
                break;

            default:
                status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_InvalidState);

            }
            if (ptxStatus_Success == status)
            {
                current_byte_address = t2tOpComp->TLVs.NDEFTLV.ByteAddress;
                current_block_address = (current_byte_address / 4);
                current_block = (uint8_t)(current_block_address % 256);
                ndef_tlv_offset = (current_block % 4);

                /* check sector */
                if (current_block != current_block_address)
                {
                    current_sector = (current_block_address / 256);
                    status = ptxNDEF_T2TOpSectorSelect(t2tOpComp, (uint8_t)current_sector, &t2tOpComp->RxBuffer[0], &rx_len, PTX_T2T_DEFAULT_TIMEOUT_MS);
                }

                /* read L part of TLV and go to first V-byte */
                if (ptxStatus_Success == status)
                {
                    status = ptxNDEF_T2TOpReadBlocks(t2tOpComp, current_block, &t2tOpComp->RxBuffer[0], &rx_len);
                    (void)memcpy(&t2tOpComp->WorkBuffer[0], &t2tOpComp->RxBuffer[1+ndef_tlv_offset], (uint32_t)3);
                    if (0xFF == t2tOpComp->WorkBuffer[0])
                    {
                        msg_length = (uint16_t)(((uint16_t)t2tOpComp->WorkBuffer[1] << PTX_T2TOP_MASK_8) | t2tOpComp->WorkBuffer[2]);
                        ndef_bytes_to_jump = 4;
                    } else
                    {
                        msg_length = t2tOpComp->WorkBuffer[0];
                        ndef_bytes_to_jump = 2;
                    }
                    current_byte_address = (uint16_t)(current_byte_address + ndef_bytes_to_jump); //position of first NDEF value byte
                }

                /* check if msgBuffer has enough space for the message */
                if (msg_buffer_size < msg_length)
                {
                    status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_InsufficientResources);
                }
                while ((msg_length > bytes_read) && (ptxStatus_Success == status))
                {
                    block_offset = (current_byte_address % 4);
                    bytes_to_read = (uint16_t)((PTX_T2TOP_BLOCK_SIZE * 4) - block_offset);

                    current_block_address = (current_byte_address / 4);
                    current_block = (uint8_t)(current_block_address % 256);
                    /* check sector */
                    if (previous_sector != (current_block_address / 256))
                    {
                        current_sector = (current_block_address / 256);
                        status = ptxNDEF_T2TOpSectorSelect(t2tOpComp, (uint8_t)current_sector, &t2tOpComp->RxBuffer[0], &rx_len, PTX_T2T_DEFAULT_TIMEOUT_MS);
                    }

                    if (msg_length < (bytes_read + ((PTX_T2TOP_BLOCK_SIZE * 4) - block_offset)))
                    {
                        bytes_to_read = (uint16_t)(msg_length - bytes_read); // leftover bytes
                        end_offset = (uint8_t)((PTX_T2TOP_BLOCK_SIZE * 4)-bytes_to_read-block_offset);
                    }
                    status = ptxNDEF_T2TOpReadBlocks(t2tOpComp, current_block, &t2tOpComp->RxBuffer[0], &rx_len);

                    if (ptxStatus_Success == status)
                    {
                        (void)memcpy(&msg_buffer[bytes_read], &t2tOpComp->RxBuffer[block_offset], (uint32_t)bytes_to_read);
                        status = ptxNDEF_T2TAreaChecker(t2tOpComp, current_byte_address, &bytes_to_read, &preserve_bytes[0]);

                        if (ptxStatus_Success == status)
                        {
                            bytes_read = (uint16_t)(bytes_read + bytes_to_read);

                            if (msg_length < bytes_read)
                            {
                                bytes_read = (uint16_t)(bytes_read-end_offset);
                            } else
                            {
                                end_offset = 0;
                            }

                            for (uint8_t i = 0; i < ((PTX_T2TOP_BLOCK_SIZE * 4)-bytes_to_read-end_offset); i++)
                            {
                                //index of msgBuffer position to be deleted
                                delete_offset = (uint16_t)(preserve_bytes[i] - current_byte_address - i); // i is the number of already deleted msgBuffer parts, needs to be subtracted

                                /* shift msgBuffer to the left */
                                for (uint16_t j = delete_offset; j <= msg_length; j++)
                                {
                                    msg_buffer[j] = msg_buffer[j+1];
                                }
                            }
                            previous_sector = current_sector;
                            current_byte_address = (uint16_t)(current_byte_address + (PTX_T2TOP_BLOCK_SIZE * 4) - block_offset - end_offset);
                            block_offset = 0;
                            (void)memset(&preserve_bytes[0],0,sizeof(preserve_bytes));
                        }
                    }
                }
                if ((ptxStatus_Success == status) && (msg_length == bytes_read))
                {
                    *msgLen = msg_length;
                } else
                {
                    *msgLen = 0;
                }
            }
        } else
        {
            status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_InvalidParameter);
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNDEF_T2TOpWriteMessage (ptxNDEF_T2TOP_t *t2tOpComp, uint8_t *msgBuffer, uint32_t msgLen)
{
    ptxStatus_t status = ptxStatus_Success;

    uint8_t length_size;
    uint8_t ndef_tlv_header_size;
    const uint8_t max_ndef_byte_offset = 3U;
    uint8_t first_ndef_byte_offset = 0;
    uint16_t msg_len;
    uint8_t *msg_Buffer;
    uint8_t emptyMsg = 0;
    uint8_t EMPTY_NDEF_MSG_T2T[] = {0xD8, 0x00, 0x00, 0x00};
    uint8_t current_block;
    uint8_t previous_sector = 0;
    uint8_t current_sector = 0;
    uint16_t current_byte_address = 0;
    uint16_t current_block_address;
    uint32_t rx_len;
    uint16_t bytes_written = 0;
    uint16_t bytes_to_write;
    uint16_t bytes_to_write_checker;
    uint16_t preserve_bytes[16]; // reset every operation, write max of 4 but uses same preserve buffer
    uint8_t end_offset = 0;



    if (PTX_COMP_CHECK(t2tOpComp, ptxStatus_Comp_T2TOP))
    {
        if (0 != msgLen)
        {
            switch (t2tOpComp->LifeCycle)
            {
            case TagLC_Initialized:
            case TagLC_ReadWrite:
                break;

            case TagLC_ReadOnly:
                status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_AccessDenied);
                break;

            default:
                status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_InvalidState);
                break;
            }
        }
        if (ptxStatus_Success == status)
        {
            if ((0 != msgLen) && (NULL != msgBuffer))
            {
                msg_Buffer = msgBuffer;
                msg_len = (uint16_t)msgLen;
            } else
            {
                msg_Buffer = &EMPTY_NDEF_MSG_T2T[0];
                msg_len = sizeof(EMPTY_NDEF_MSG_T2T);
                emptyMsg = 1;

            }

            length_size = (msg_len <= ((uint32_t)0xFE)) ? (uint8_t)0x01 : (uint8_t)0x03;
            ndef_tlv_header_size = (uint8_t)(length_size + 1);

            if (msgLen <= (uint32_t)t2tOpComp->TLVs.NDEFTLV.AvailableLength)
            {
                /* reset L-field */
                status = ptxNDEF_T2TUpdateLength(t2tOpComp, 0, length_size);

                if (ptxStatus_Success == status)
                {
                    current_byte_address = t2tOpComp->TLVs.NDEFTLV.ByteAddress;
                    current_byte_address = (uint16_t)(current_byte_address + ndef_tlv_header_size);
                    current_block_address = (current_byte_address / 4);
                    current_block = (uint8_t)(current_block_address % 256);

                    /* check sector */
                    if (current_block != current_block_address)
                    {
                        current_sector = (uint8_t)(current_block_address / 256);
                        status = ptxNDEF_T2TOpSectorSelect(t2tOpComp, current_sector, &t2tOpComp->RxBuffer[0], &rx_len, PTX_T2T_DEFAULT_TIMEOUT_MS);
                    }

                    if (ptxStatus_Success == status)
                    {
                        previous_sector = current_sector;
                        first_ndef_byte_offset = current_byte_address % 4;
                        bytes_to_write = (uint16_t)(PTX_T2TOP_BLOCK_SIZE - first_ndef_byte_offset);

                        status = ptxNDEF_T2TOpReadBlocks(t2tOpComp, current_block, &t2tOpComp->RxBuffer[0], &rx_len);
                    }


                    if (ptxStatus_Success == status)
                    {
                        (void)memcpy(&t2tOpComp->WorkBuffer[0],&t2tOpComp->RxBuffer[0],(uint32_t)PTX_T2TOP_BLOCK_SIZE);
                        (void)memcpy(&t2tOpComp->WorkBuffer[first_ndef_byte_offset],&msg_Buffer[0],(uint32_t)bytes_to_write);

                        status = ptxNDEF_T2TAreaChecker(t2tOpComp, current_byte_address, &bytes_to_write, &preserve_bytes[0]);

                        if (ptxStatus_Success == status)
                        {
                            for (uint8_t i = 0; i < (PTX_T2TOP_BLOCK_SIZE-bytes_to_write-first_ndef_byte_offset); i++)
                            {
                                switch (preserve_bytes[i] - current_byte_address)
                                {
                                    case 0:
                                    {
                                        for (uint8_t j = first_ndef_byte_offset; j<max_ndef_byte_offset; j++)
                                        {
                                            t2tOpComp->WorkBuffer[j+1] = t2tOpComp->WorkBuffer[j];
                                        }
                                        (void)memcpy(&t2tOpComp->WorkBuffer[first_ndef_byte_offset],&t2tOpComp->RxBuffer[first_ndef_byte_offset],(uint32_t)1);
                                        break;
                                    }
                                    case 1:
                                    {
                                        for (uint8_t j = (uint8_t)(first_ndef_byte_offset+1U); j<max_ndef_byte_offset; j++)
                                        {
                                            t2tOpComp->WorkBuffer[j+1] = t2tOpComp->WorkBuffer[j];
                                        }

                                        uint16_t tmp_adr = (uint16_t)(first_ndef_byte_offset+1U);
                                        if((uint16_t)(tmp_adr+1U) <= PTX_T2TOP_INTERNAL_BUFLENGTH)
                                        {
                                            (void)memcpy(&t2tOpComp->WorkBuffer[tmp_adr],&t2tOpComp->RxBuffer[tmp_adr], 1U);
                                        }
                                        break;
                                    }
                                    case 2:
                                    {
                                        for (uint8_t j = (uint8_t)(first_ndef_byte_offset+2U); j<max_ndef_byte_offset; j++)
                                        {
                                            t2tOpComp->WorkBuffer[j+1] = t2tOpComp->WorkBuffer[j];
                                        }

                                        uint16_t tmp_adr = (uint16_t)(first_ndef_byte_offset+2U);
                                        if((uint16_t)(tmp_adr+1U) <= PTX_T2TOP_INTERNAL_BUFLENGTH)
                                        {
                                            (void)memcpy(&t2tOpComp->WorkBuffer[tmp_adr],&t2tOpComp->RxBuffer[tmp_adr], 1U);
                                        }
                                        break;
                                    }
                                    case 3:
                                    {
                                        uint16_t tmp_adr = (uint16_t)(first_ndef_byte_offset+3U);
                                        if((uint16_t)(tmp_adr+1U) <= PTX_T2TOP_INTERNAL_BUFLENGTH)
                                        {
                                            (void)memcpy(&t2tOpComp->WorkBuffer[tmp_adr],&t2tOpComp->RxBuffer[tmp_adr], 1U);
                                        }
                                        break;
                                    }
                                    default:
                                    {
                                        break;
                                    }
                                }
                            }

                            if (ptxStatus_Success == status)
                            {
                                rx_len = t2tOpComp->RxBufferSize;
                                status = ptxNDEF_T2TOpWriteBlock(t2tOpComp, current_block, &t2tOpComp->WorkBuffer[0], PTX_T2TOP_BLOCK_SIZE, &t2tOpComp->RxBuffer[0], &rx_len, PTX_T2T_DEFAULT_TIMEOUT_MS);
                                if (ptxStatus_Success == status)
                                {
                                    bytes_written = (uint16_t)(bytes_written + bytes_to_write);
                                    (void)memset(&preserve_bytes[0],0,PTX_T2TOP_BLOCK_SIZE);
                                    current_byte_address = (uint16_t)((current_block_address + 1) * 4);
                                }
                            }
                        }
                    }
                }
            } else
            {
                status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_InsufficientResources);
            }

            while ((ptxStatus_Success == status) && (msg_len > bytes_written))
            {
                bytes_to_write = PTX_T2TOP_BLOCK_SIZE;
                bytes_to_write_checker = bytes_to_write;
                if (msg_len < (bytes_written + PTX_T2TOP_BLOCK_SIZE))
                {
                    bytes_to_write = (uint16_t)(msg_len - bytes_written);
                    bytes_to_write_checker = bytes_to_write;
                    end_offset = (uint8_t)(PTX_T2TOP_BLOCK_SIZE-bytes_to_write);
                }

                current_block_address = (uint16_t)(current_byte_address / 4);
                current_block = (uint8_t)(current_block_address % 256);


                if (previous_sector != (current_block_address / 256))
                {
                    current_sector = (uint8_t)(current_block_address / 256);
                    status = ptxNDEF_T2TOpSectorSelect(t2tOpComp, current_sector, &t2tOpComp->RxBuffer[0], &rx_len, PTX_T2T_DEFAULT_TIMEOUT_MS);
                }

                if (ptxStatus_Success == status)
                {
                    status = ptxNDEF_T2TAreaChecker(t2tOpComp, current_byte_address, &bytes_to_write, &preserve_bytes[0]);

                    if (ptxStatus_Success == status)
                    {
                        rx_len = t2tOpComp->RxBufferSize;
                        status = ptxNDEF_T2TOpReadBlocks(t2tOpComp, current_block, &t2tOpComp->RxBuffer[0], &rx_len);
                    }
                    if ((bytes_to_write_checker != bytes_to_write) && (ptxStatus_Success == status))
                    {
                        (void)memcpy(&t2tOpComp->WorkBuffer[0],&msg_Buffer[bytes_written],PTX_T2TOP_BLOCK_SIZE);
                        for (uint8_t i = 0; i < (PTX_T2TOP_BLOCK_SIZE - bytes_to_write - end_offset); i++)
                        {
                            switch(preserve_bytes[i] - current_byte_address)
                            {
                                case 0:
                                {
                                    for (uint8_t j = 0; j<max_ndef_byte_offset; j++)
                                    {
                                        t2tOpComp->WorkBuffer[j+1] = t2tOpComp->WorkBuffer[j];
                                    }
                                    (void)memcpy(&t2tOpComp->WorkBuffer[0], &t2tOpComp->RxBuffer[0],(uint32_t)1);
                                    break;
                                }
                                case 1:
                                {
                                    for (uint8_t j = 1; j<max_ndef_byte_offset; j++)
                                        {
                                            t2tOpComp->WorkBuffer[j+1] = t2tOpComp->WorkBuffer[j];
                                        }
                                    (void)memcpy(&t2tOpComp->WorkBuffer[1], &t2tOpComp->RxBuffer[1],(uint32_t)1);
                                    break;
                                }
                                case 2:
                                {
                                    for (uint8_t j = 2; j<max_ndef_byte_offset; j++)
                                    {
                                        t2tOpComp->WorkBuffer[j+1] = t2tOpComp->WorkBuffer[j];
                                    }
                                    (void)memcpy(&t2tOpComp->WorkBuffer[2], &t2tOpComp->RxBuffer[2],(uint32_t)1);
                                    break;
                                }
                                case 3:
                                {
                                    (void)memcpy(&t2tOpComp->WorkBuffer[3], &t2tOpComp->RxBuffer[3],(uint32_t)1);
                                    break;
                                }
                                default:
                                {
                                    break;
                                }
                            }
                        }
                        rx_len = t2tOpComp->RxBufferSize;
                        status = ptxNDEF_T2TOpWriteBlock(t2tOpComp, current_block, &t2tOpComp->WorkBuffer[0], PTX_T2TOP_BLOCK_SIZE, &t2tOpComp->RxBuffer[0], &rx_len, PTX_T2T_DEFAULT_TIMEOUT_MS);

                        (void)memset(&preserve_bytes[0],0,PTX_T2TOP_BLOCK_SIZE);
                        bytes_written = (uint16_t)(bytes_written + bytes_to_write);
                        current_byte_address = (uint16_t)(current_byte_address + (PTX_T2TOP_BLOCK_SIZE - end_offset));
                    } else
                    {
                        (void)memcpy(&t2tOpComp->WorkBuffer[0],&msg_Buffer[bytes_written],PTX_T2TOP_BLOCK_SIZE);

                        rx_len = t2tOpComp->RxBufferSize;
                        status = ptxNDEF_T2TOpWriteBlock(t2tOpComp, current_block, &t2tOpComp->WorkBuffer[0], PTX_T2TOP_BLOCK_SIZE, &t2tOpComp->RxBuffer[0], &rx_len, PTX_T2T_DEFAULT_TIMEOUT_MS);

                        bytes_written = (uint16_t)(bytes_written + bytes_to_write);
                        current_byte_address = (uint16_t)(current_byte_address + (PTX_T2TOP_BLOCK_SIZE - end_offset));
                    }
                }
            }
            if (ptxStatus_Success == status)
            {
                status = ptxNDEF_T2TUpdateLength(t2tOpComp, msg_len, length_size);
            }
        }
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_InvalidParameter);
    }

    if ((t2tOpComp->TLVs.NDEFTLV.Length == bytes_written) && (ptxStatus_Success == status))
    {
        msgLen = (uint32_t)t2tOpComp->TLVs.NDEFTLV.Length;

        if (0 == emptyMsg)
        {
            t2tOpComp->LifeCycle = TagLC_ReadWrite;
        } else
        {
            t2tOpComp->LifeCycle = TagLC_Initialized;
            status = ptxNDEF_T2TUpdateLength(t2tOpComp, 0, length_size);
        }

    } else
    {
        msgLen = 0;
    }

    return status;
}

ptxStatus_t ptxNDEF_T2TOpLockTag (ptxNDEF_T2TOP_t *t2tOpComp)
{
    ptxStatus_t status = ptxStatus_Success;

    uint16_t NbrLockBits;
    uint16_t NbrLockBytes;
    uint16_t LockedBytes = 0;
    uint16_t current_byte_address;
    uint16_t current_block_address;
    uint8_t current_block;
    uint8_t current_sector = 0;
    uint8_t bytes_to_write;
    uint8_t first_lock_byte_offset;
    uint8_t last_lock_byte_offset;
    uint8_t incomplete_lock_byte;
    uint16_t last_lock_byte_address;
    uint8_t nbr_reserved_bits = 0;
    uint32_t rx_len;
    uint16_t first_dla_address;
    uint16_t last_dla_address;

    if (PTX_COMP_CHECK(t2tOpComp, ptxStatus_Comp_T2TOP))
    {
        if (PTX_T2TOP_MIN_SIZE_FOR_DLA < t2tOpComp->CCParams.Size)
        {
            if (0 == t2tOpComp->TLVs.LockControlTLVs.NumberOfTLVs)
            {
                /* use default setting for dynamic lock bits */
                //BytesLockedPerLockBit = 8;
                NbrLockBits = (uint16_t)((t2tOpComp->CCParams.Size - PTX_T2TOP_MIN_SIZE_FOR_DLA) / 8);
                if (0 != ((t2tOpComp->CCParams.Size - PTX_T2TOP_MIN_SIZE_FOR_DLA) % 8))
                {
                    NbrLockBits = (uint16_t)(NbrLockBits + ((t2tOpComp->CCParams.Size - PTX_T2TOP_MIN_SIZE_FOR_DLA) % 8));
                }
                NbrLockBytes = (uint16_t)(NbrLockBits / 8);
                if (0 != (NbrLockBits % 8))
                {
                    NbrLockBytes++;
                    nbr_reserved_bits = (uint8_t)(8-(NbrLockBits % 8));
                }
                bytes_to_write = (uint8_t)NbrLockBytes;

                /* go to first byte after TLV area */
                current_byte_address = (uint16_t)(t2tOpComp->CCParams.Size + (PTX_T2TOP_TLV_AREA_BEGIN_BLOCK * PTX_T2TOP_BLOCK_SIZE));

                first_lock_byte_offset = (current_byte_address % 4);
                last_lock_byte_address = (uint16_t)(current_byte_address + NbrLockBytes - 1);
                last_lock_byte_offset  = (last_lock_byte_address % 4);

                while ((NbrLockBytes > LockedBytes) && (ptxStatus_Success == status) && ((bytes_to_write - PTX_T2TOP_BLOCK_SIZE) > 0))
                {
                    current_block_address = (current_byte_address / 4);
                    current_block = (uint8_t)(current_block_address % 256);
                    /* check sector */
                    if (current_sector !=  (current_block_address / 256))
                    {
                        current_sector = (uint8_t)(current_block_address / 256);
                        status = ptxNDEF_T2TOpSectorSelect(t2tOpComp, current_sector, &t2tOpComp->RxBuffer[0], &rx_len, PTX_T2T_DEFAULT_TIMEOUT_MS);
                    }

                    if (ptxStatus_Success == status)
                    {
                        /* read first block of DLA (following 3 ignored) */
                        status = ptxNDEF_T2TOpReadBlocks(t2tOpComp, current_block, &t2tOpComp->RxBuffer[0], &rx_len);

                        if (ptxStatus_Success == status)
                        {
                            /* check for leading non-DLA bytes and put them into the work buffer to be written, then fill remaining bytes with 0xFF */
                            if (0 != first_lock_byte_offset)
                            {
                                (void)memcpy(&t2tOpComp->WorkBuffer[0],&t2tOpComp->RxBuffer[0],(uint32_t)first_lock_byte_offset);
                            }
                            (void)memset(&t2tOpComp->WorkBuffer[first_lock_byte_offset],(uint8_t)0xFF,(uint32_t)(PTX_T2TOP_BLOCK_SIZE - first_lock_byte_offset));

                            /* write the leading non-DLA blocks and the lock bits set to 1 */
                            rx_len = t2tOpComp->RxBufferSize;
                            status = ptxNDEF_T2TOpWriteBlock(t2tOpComp, current_block, &t2tOpComp->WorkBuffer[0], PTX_T2TOP_BLOCK_SIZE, &t2tOpComp->RxBuffer[0], &rx_len, PTX_T2T_DEFAULT_TIMEOUT_MS);
                        }
                    }
                    bytes_to_write = (uint8_t)(bytes_to_write - (PTX_T2TOP_BLOCK_SIZE - first_lock_byte_offset));
                    LockedBytes = (uint16_t)(LockedBytes + (PTX_T2TOP_BLOCK_SIZE - first_lock_byte_offset));
                    current_byte_address = (uint16_t)(current_byte_address + (PTX_T2TOP_BLOCK_SIZE - first_lock_byte_offset));
                    first_lock_byte_offset = 0;
                }

                /* last block handled separately due to possible offset and reserved bits */
                current_block_address = (current_byte_address / 4);
                current_block = (uint8_t)(current_block_address % 256);

                /* check sector */
                if ((current_block != current_block_address) && (current_sector !=  (current_block_address / 256)))
                {
                    current_sector = (uint8_t)(current_block_address / 256);
                    status = ptxNDEF_T2TOpSectorSelect(t2tOpComp, current_sector, &t2tOpComp->RxBuffer[0], &rx_len, PTX_T2T_DEFAULT_TIMEOUT_MS);
                }

                /* read last block (3 following blocks are ignored) */
                status = ptxNDEF_T2TOpReadBlocks(t2tOpComp, current_block, &t2tOpComp->RxBuffer[0], &rx_len);

                if (ptxStatus_Success == status)
                {
                    /* fill the DLA bytes with 0xFF and put trailing non-DLA bytes into work buffer */
                    (void)memcpy(&t2tOpComp->WorkBuffer[0],&t2tOpComp->RxBuffer[0],(uint32_t)PTX_T2TOP_BLOCK_SIZE);
                    (void)memset(&t2tOpComp->WorkBuffer[first_lock_byte_offset],0xFF,(uint32_t)((last_lock_byte_offset + 1) - first_lock_byte_offset));

                    /* handle reserved bits if last DLA byte is not completely filled with lock bits */
                    if (0 != nbr_reserved_bits)
                    {
                        /* DLA bytes are filled lsb to msb, push x 0s into the byte for reserved bits */
                        incomplete_lock_byte = (uint8_t)((uint8_t)0xFF >> nbr_reserved_bits);
                        (void)memcpy(&t2tOpComp->WorkBuffer[last_lock_byte_offset],&incomplete_lock_byte,(uint32_t)1);
                    }
                    rx_len = t2tOpComp->RxBufferSize;
                    status = ptxNDEF_T2TOpWriteBlock(t2tOpComp, current_block, &t2tOpComp->WorkBuffer[0], PTX_T2TOP_BLOCK_SIZE, &t2tOpComp->RxBuffer[0], &rx_len, PTX_T2T_DEFAULT_TIMEOUT_MS);
                    if (ptxStatus_Success == status)
                    {
                        LockedBytes = (uint16_t)(LockedBytes + ((last_lock_byte_offset + 1) - first_lock_byte_offset));
                    }
                }
                if ((LockedBytes == NbrLockBytes) && (ptxStatus_Success == status))
                {
                    /* everything ok, all lock bits set */
                } else
                {
                    status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_AssertionError);
                }
            } else
            {
                /* use lock control TLVs */

                /* iterate over all DLAs */
                for (uint8_t LockTLV = 0; LockTLV < t2tOpComp->TLVs.LockControlTLVs.NumberOfTLVs; LockTLV=(uint8_t)(LockTLV+2))
                {
                    first_dla_address = t2tOpComp->TLVs.LockControlTLVs.LockArea[LockTLV];
                    last_dla_address  = t2tOpComp->TLVs.LockControlTLVs.LockArea[LockTLV+1];
                    nbr_reserved_bits = t2tOpComp->TLVs.LockControlTLVs.NbrReservedBits[LockTLV/2];

                    current_byte_address = first_dla_address;
                    first_lock_byte_offset = (current_byte_address % 4);
                    last_lock_byte_offset = (last_dla_address % 4);


                    NbrLockBytes = (uint16_t)(last_dla_address - first_dla_address);
                    NbrLockBits = (uint16_t)(NbrLockBytes * 8 - nbr_reserved_bits);

                    bytes_to_write = (uint8_t)NbrLockBytes;

                    while ((NbrLockBytes > LockedBytes) && (ptxStatus_Success == status) && ((bytes_to_write - PTX_T2TOP_BLOCK_SIZE) > 0))
                    {
                        current_block_address = (current_byte_address / 4);
                        current_block = (uint8_t)(current_block_address % 256);
                        /* check sector */
                        if (current_sector !=  (current_block_address / 256))
                        {
                            current_sector = (uint8_t)(current_block_address / 256);
                            status = ptxNDEF_T2TOpSectorSelect(t2tOpComp, current_sector, &t2tOpComp->RxBuffer[0], &rx_len, PTX_T2T_DEFAULT_TIMEOUT_MS);
                        }

                        if (ptxStatus_Success == status)
                        {
                            /* read first block of DLA (following 3 ignored) */
                            status = ptxNDEF_T2TOpReadBlocks(t2tOpComp, current_block, &t2tOpComp->RxBuffer[0], &rx_len);

                            if (ptxStatus_Success == status)
                            {
                                /* check for leading non-DLA bytes and put them into the work buffer to be written, then fill remaining bytes with 0xFF */
                                if (0 != first_lock_byte_offset)
                                {
                                    (void)memcpy(&t2tOpComp->WorkBuffer[0],&t2tOpComp->RxBuffer[0],(uint32_t)first_lock_byte_offset);
                                }
                                (void)memset(&t2tOpComp->WorkBuffer[first_lock_byte_offset],(uint8_t)0xFF,(uint32_t)(PTX_T2TOP_BLOCK_SIZE - first_lock_byte_offset));

                                /* write the leading non-DLA blocks and the lock bits set to 1 */
                                rx_len = t2tOpComp->RxBufferSize;
                                status = ptxNDEF_T2TOpWriteBlock(t2tOpComp, current_block, &t2tOpComp->WorkBuffer[0], PTX_T2TOP_BLOCK_SIZE, &t2tOpComp->RxBuffer[0], &rx_len, PTX_T2T_DEFAULT_TIMEOUT_MS);
                            }
                        }
                        bytes_to_write = (uint8_t)(bytes_to_write - (PTX_T2TOP_BLOCK_SIZE - first_lock_byte_offset));
                        LockedBytes = (uint16_t)(LockedBytes + (PTX_T2TOP_BLOCK_SIZE - first_lock_byte_offset));
                        current_byte_address = (uint16_t)(current_byte_address + (PTX_T2TOP_BLOCK_SIZE - first_lock_byte_offset));
                        first_lock_byte_offset = 0;
                    }

                    /* last block handled separately due to possible offset and reserved bits */
                    current_block_address = (current_byte_address / 4);
                    current_block = (uint8_t)(current_block_address % 256);

                    /* check sector */
                    if ((current_block != current_block_address) && (current_sector !=  (current_block_address / 256)))
                    {
                        current_sector = (uint8_t)(current_block_address / 256);
                        status = ptxNDEF_T2TOpSectorSelect(t2tOpComp, current_sector, &t2tOpComp->RxBuffer[0], &rx_len, PTX_T2T_DEFAULT_TIMEOUT_MS);
                    }

                    /* read last block (3 following blocks are ignored) */
                    status = ptxNDEF_T2TOpReadBlocks(t2tOpComp, current_block, &t2tOpComp->RxBuffer[0], &rx_len);

                    if (ptxStatus_Success == status)
                    {
                        /* fill the DLA bytes with 0xFF and put trailing non-DLA bytes into work buffer */
                        (void)memcpy(&t2tOpComp->WorkBuffer[0],&t2tOpComp->RxBuffer[0],(uint32_t)PTX_T2TOP_BLOCK_SIZE);
                        (void)memset(&t2tOpComp->WorkBuffer[first_lock_byte_offset],0xFF,(uint32_t)(last_lock_byte_offset + 1) - first_lock_byte_offset);

                        /* handle reserved bits if last DLA byte is not completely filled with lock bits */
                        if (0 != nbr_reserved_bits)
                        {
                            /* DLA bytes are filled lsb to msb, push x 0s into the byte for reserved bits */
                            incomplete_lock_byte = (uint8_t)((uint8_t)0xFF >> nbr_reserved_bits);
                            (void)memcpy(&t2tOpComp->WorkBuffer[last_lock_byte_offset],&incomplete_lock_byte,(uint32_t)1);
                        }
                        rx_len = t2tOpComp->RxBufferSize;
                        status = ptxNDEF_T2TOpWriteBlock(t2tOpComp, current_block, &t2tOpComp->WorkBuffer[0], PTX_T2TOP_BLOCK_SIZE, &t2tOpComp->RxBuffer[0], &rx_len, PTX_T2T_DEFAULT_TIMEOUT_MS);
                        if (ptxStatus_Success == status)
                        {
                            LockedBytes = (uint16_t)(LockedBytes + ((last_lock_byte_offset + 1) - first_lock_byte_offset));
                        }
                    }
                    if ((LockedBytes == NbrLockBytes) && (ptxStatus_Success == status))
                    {
                        /* everything ok, all lock bits set */
                    } else
                    {
                        status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_AssertionError);
                    }
                }
            }
        }

        /* set static lock area lock bits to 1 */
        if (0 != current_sector)
        {
            status = ptxNDEF_T2TOpSectorSelect(t2tOpComp, 0, &t2tOpComp->RxBuffer[0], &rx_len, PTX_T2T_DEFAULT_TIMEOUT_MS);
        }

        /* set CC access condition to read-only since CC will be locked by static lock bytes */
        if (ptxStatus_Success == status)
        {
            t2tOpComp->WorkBuffer[0] = t2tOpComp->CCParams.MagicNumber;
            t2tOpComp->WorkBuffer[1] = t2tOpComp->CCParams.Version;
            t2tOpComp->WorkBuffer[2] = (uint8_t)t2tOpComp->CCParams.MLEN;

            /* set CC access condition to read only */
            t2tOpComp->WorkBuffer[3] = (uint8_t)((uint8_t)0xFF >> PTX_T2TOP_MASK_4);

            rx_len = t2tOpComp->RxBufferSize;
            status = ptxNDEF_T2TOpWriteBlock(t2tOpComp, PTX_T2TOP_CC_BLOCK_NUMBER, &t2tOpComp->WorkBuffer[0], PTX_T2TOP_BLOCK_SIZE, &t2tOpComp->RxBuffer[0], &rx_len, PTX_T2T_DEFAULT_TIMEOUT_MS);
            if (ptxStatus_Success == status)
            {
                t2tOpComp->LifeCycle = TagLC_ReadOnly;
            }
        }

        /* set static lock bytes */
        if (ptxStatus_Success == status)
        {
            rx_len = t2tOpComp->RxBufferSize;
            status = ptxNDEF_T2TOpReadBlocks(t2tOpComp, PTX_T2TOP_STATIC_LOCK_BLOCK_NUMBER, &t2tOpComp->RxBuffer[0], &rx_len);

            if (ptxStatus_Success == status)
            {
                /* copy internal byte 8 and 9 to work buffer */
                (void)memcpy(&t2tOpComp->WorkBuffer[0], &t2tOpComp->RxBuffer[0],(uint32_t)2);

                /* set buffer component for static lock bytes */
                t2tOpComp->WorkBuffer[2] = 0xFF;
                t2tOpComp->WorkBuffer[3] = 0xFF;

                status = ptxNDEF_T2TOpWriteBlock(t2tOpComp, PTX_T2TOP_STATIC_LOCK_BLOCK_NUMBER, &t2tOpComp->WorkBuffer[0], PTX_T2TOP_BLOCK_SIZE, &t2tOpComp->RxBuffer[0], &rx_len, PTX_T2T_DEFAULT_TIMEOUT_MS);
            }
        }
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNDEF_T2TOpClose (ptxNDEF_T2TOP_t *t2tOpComp)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(t2tOpComp, ptxStatus_Comp_T2TOP))
    {
        status = ptxNativeTag_T2TClose(&t2tOpComp->NativeTagT2T);
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

/*
 * ####################################################################################################################
 * INTERNAL FUNCTIONS / CALLBACK(s)
 * ####################################################################################################################
 */
static ptxStatus_t ptxNDEF_T2TGetCapabilityContainerInfo (ptxNDEF_T2TOP_t *t2tOpComp)
{
    ptxStatus_t status = ptxStatus_Success;
    uint32_t rx_len;
    if (PTX_COMP_CHECK(t2tOpComp, ptxStatus_Comp_T2TOP))
    {

        /* reset current CC-parameters */
        (void)memset(&t2tOpComp->CCParams, 0, sizeof(ptxNDEF_T2TOP_CC_t));
        (void)memset(&t2tOpComp->SectorParams, 0, sizeof(ptxNDEF_T2TOP_Sector_t));

        /* read CC-block and 3 following */
        status = ptxNDEF_T2TOpReadBlocks(t2tOpComp, PTX_T2TOP_CC_BLOCK_NUMBER, &t2tOpComp->RxBuffer[0], &rx_len);
    }

    if (ptxStatus_Success == status)
    {
        (void)memcpy(&t2tOpComp->WorkBuffer[0], &t2tOpComp->RxBuffer[0],PTX_T2TOP_BLOCK_SIZE);

        t2tOpComp->CCParams.MagicNumber = t2tOpComp->WorkBuffer[PTX_T2TOP_CC_OFFSET_MAGIC_NUMBER];
        t2tOpComp->CCParams.Version = t2tOpComp->WorkBuffer[PTX_T2TOP_CC_OFFSET_VERSION_INFO];
        t2tOpComp->CCParams.MLEN = t2tOpComp->WorkBuffer[PTX_T2TOP_CC_OFFSET_TAG_SIZE];
        t2tOpComp->CCParams.Access = t2tOpComp->WorkBuffer[PTX_T2TOP_CC_OFFSET_ACCESS_COND];

        t2tOpComp->CCParams.ReadAccess = (t2tOpComp->CCParams.Access >> PTX_T2TOP_MASK_4);
        t2tOpComp->CCParams.WriteAccess = (t2tOpComp->CCParams.Access & ((PTX_T2TOP_MASK_1<<PTX_T2TOP_MASK_4)-PTX_T2TOP_MASK_1));

        t2tOpComp->CCParams.VersionMajor = (t2tOpComp->CCParams.Version >> PTX_T2TOP_MASK_4);
        t2tOpComp->CCParams.VersionMinor = (t2tOpComp->CCParams.Access & ((PTX_T2TOP_MASK_1<<PTX_T2TOP_MASK_4)-PTX_T2TOP_MASK_1));

        t2tOpComp->CCParams.Size = (uint16_t)((uint16_t)t2tOpComp->CCParams.MLEN * (uint16_t)8);
        t2tOpComp->CCParams.NumberOfBlocks = (uint16_t)((uint16_t)t2tOpComp->CCParams.Size / (uint16_t)4);

        t2tOpComp->SectorParams.NumberOfSectors = (uint8_t)((t2tOpComp->CCParams.NumberOfBlocks / 256) + 1);

    } else
    {
        /* nothing to do, return error status */
    }


    return status;
}


static ptxStatus_t ptxNDEF_T2TOpReadBlocks (ptxNDEF_T2TOP_t *t2tOpComp, uint8_t blockNumber, uint8_t *rx, uint32_t *rxLen)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(t2tOpComp, ptxStatus_Comp_T2TOP) && (NULL != rx) && (NULL != rxLen))
    {
        *rxLen = (uint32_t)t2tOpComp->RxBufferSize;
        status = ptxNativeTag_T2TRead(&t2tOpComp->NativeTagT2T, blockNumber, rx, rxLen, PTX_T2T_DEFAULT_TIMEOUT_MS);

        if (ptxStatus_Success == status)
        {
            if (0 != *rxLen)
            {
                if (16 != *rxLen)
                {
                    status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_ProtocolError);
                }
            }
        } else
        {
            status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_NscRfError);
        }
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

static ptxStatus_t ptxNDEF_T2TOpWriteBlock(ptxNDEF_T2TOP_t *t2tOpComp,
                                           uint8_t blockNumber,
                                           uint8_t *blockData,
                                           uint8_t blockDataLen,
                                           uint8_t *rx,
                                           uint32_t *rxLen,
                                           uint32_t msTimeout)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(t2tOpComp, ptxStatus_Comp_T2TOP) && (NULL != blockData) && (NULL != rx) && (NULL != rxLen))
    {
        *rxLen = t2tOpComp->RxBufferSize;
        status = ptxNativeTag_T2TWrite(&t2tOpComp->NativeTagT2T, blockNumber, blockData, blockDataLen, rx, rxLen, msTimeout);

        if(ptxStatus_Success == status)
        {
            if (0 != *rxLen)
            {
                if (PTX_T2TOP_ACK_CODE != rx[0])
                {
                    status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_ProtocolError);
                }
            } else
            {
                status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_NscRfError);
            }
        }
    }
    return status;
}

static ptxStatus_t ptxNDEF_T2TOpSectorSelect(ptxNDEF_T2TOP_t *t2tOpComp,
                                             uint8_t secNr,
                                             uint8_t *rx,
                                             uint32_t *rxLen,
                                             uint32_t msTimeout)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(t2tOpComp, ptxStatus_Comp_T2TOP) && (NULL != rx) && (NULL != rxLen))
    {
        *rxLen = t2tOpComp->RxBufferSize;
        status = ptxNativeTag_T2TSectorSelect(&t2tOpComp->NativeTagT2T, secNr, rx, rxLen, msTimeout);
        if(ptxStatus_Success == status)
        {
            if (0 != *rxLen)
            {
                if (PTX_T2TOP_ACK_CODE != rx[0])
                {
                    status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_ProtocolError);
                }
            } else
            {
                status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_NscRfError);
            }
        }
        if (ptxStatus_Success == status)
        {
            t2tOpComp->SectorParams.CurrentSector++;
        }
    }
    return status;
}

/* update L of NDEF TLV */
static ptxStatus_t ptxNDEF_T2TUpdateLength(ptxNDEF_T2TOP_t *t2tOpComp, uint16_t msgLen, uint8_t lengthSize)
{
    ptxStatus_t status = ptxStatus_Success;
    uint32_t rx_len;
    uint16_t length_byte_address;
    uint16_t block_address_ndef_len;
    uint8_t block_number_ndef_len;
    uint16_t new_len = msgLen;
    uint8_t lengthBytes[3];


    if (PTX_COMP_CHECK(t2tOpComp, ptxStatus_Comp_T2TOP))
    {
        switch (t2tOpComp->LifeCycle)
        {
        case TagLC_Initialized:
        case TagLC_ReadWrite:
            break;

        case TagLC_ReadOnly:
            status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_AccessDenied);
            break;

        default:
            status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_InvalidState);
            break;
        }


        /* L is 1 byte */
        if ((ptxStatus_Success == status) && (0x01 == lengthSize))
        {
            length_byte_address = (uint16_t)(t2tOpComp->TLVs.NDEFTLV.ByteAddress + 1);

            block_address_ndef_len = (length_byte_address / 4);
            block_number_ndef_len = (uint8_t)(block_address_ndef_len % 256);

            status = ptxNDEF_T2TOpReadBlocks(t2tOpComp, block_number_ndef_len, &t2tOpComp->RxBuffer[0], &rx_len);
            if (ptxStatus_Success == status)
            {
                (void)memcpy(&t2tOpComp->WorkBuffer[0], &t2tOpComp->RxBuffer[0], PTX_T2TOP_BLOCK_SIZE);
                t2tOpComp->WorkBuffer[(length_byte_address % 4)] = (uint8_t)new_len;

                rx_len = t2tOpComp->RxBufferSize;
                status = ptxNDEF_T2TOpWriteBlock(t2tOpComp, block_number_ndef_len, &t2tOpComp->WorkBuffer[0], PTX_T2TOP_BLOCK_SIZE, &t2tOpComp->RxBuffer[0], &rx_len, PTX_T2T_DEFAULT_TIMEOUT_MS);
            }
            if (ptxStatus_Success == status)
            {
                t2tOpComp->TLVs.NDEFTLV.Length = new_len;

                if (0 == new_len)
                {
                    t2tOpComp->LifeCycle = TagLC_Initialized;
                } else
                {
                    t2tOpComp->LifeCycle = TagLC_ReadWrite;
                }
            }
        }

        /* L is 3 bytes */
        if ((ptxStatus_Success == status) && (0x03 == lengthSize))
        {
            length_byte_address = (uint16_t)(t2tOpComp->TLVs.NDEFTLV.ByteAddress + 1);

            block_address_ndef_len = (length_byte_address / 4);
            block_number_ndef_len = (uint8_t)(block_address_ndef_len % 256);

            lengthBytes[0] = 0xFF;
            lengthBytes[1] = (uint8_t)((new_len & 0xFF00) >> PTX_T2TOP_MASK_8);
            lengthBytes[2] = (uint8_t)(new_len & 0x00FF);

            status = ptxNDEF_T2TOpReadBlocks(t2tOpComp, block_number_ndef_len, &t2tOpComp->RxBuffer[0], &rx_len);
            (void)memcpy(&t2tOpComp->WorkBuffer[0], &t2tOpComp->RxBuffer[0], PTX_T2TOP_BLOCK_SIZE * 2);

            if (2 < (length_byte_address % 4))
            {
                (void)memcpy(&t2tOpComp->WorkBuffer[(length_byte_address % 4)], &lengthBytes[0], sizeof(lengthBytes));
                rx_len = t2tOpComp->RxBufferSize;
                status = ptxNDEF_T2TOpWriteBlock(t2tOpComp, block_number_ndef_len, &t2tOpComp->WorkBuffer[0], PTX_T2TOP_BLOCK_SIZE, &t2tOpComp->RxBuffer[0], &rx_len, PTX_T2T_DEFAULT_TIMEOUT_MS);
            } else
            {
                (void)memcpy(&t2tOpComp->WorkBuffer[(length_byte_address % 4)], &lengthBytes[0], (uint32_t)((uint32_t)sizeof(lengthBytes) - (uint32_t)(PTX_T2TOP_BLOCK_SIZE - (length_byte_address % 4))));
                rx_len = t2tOpComp->RxBufferSize;
                status = ptxNDEF_T2TOpWriteBlock(t2tOpComp, block_number_ndef_len, &t2tOpComp->WorkBuffer[0], PTX_T2TOP_BLOCK_SIZE, &t2tOpComp->RxBuffer[0], &rx_len, PTX_T2T_DEFAULT_TIMEOUT_MS);
                //check sector length
                if (ptxStatus_Success == status)
                {
                    if (255 > block_number_ndef_len)
                    {
                        /* second write is in new sector */
                        status = ptxNDEF_T2TOpSectorSelect(t2tOpComp, (uint8_t)(t2tOpComp->SectorParams.CurrentSector + 1), &t2tOpComp->RxBuffer[0], &rx_len, PTX_T2T_DEFAULT_TIMEOUT_MS);
                        if (ptxStatus_Success == status)
                        {
                            /* write to block 0 of next sector */
                            status = ptxNDEF_T2TOpWriteBlock(t2tOpComp, 0, &t2tOpComp->WorkBuffer[PTX_T2TOP_BLOCK_SIZE], PTX_T2TOP_BLOCK_SIZE, &t2tOpComp->RxBuffer[0], &rx_len, PTX_T2T_DEFAULT_TIMEOUT_MS);
                        }
                    } else
                    {
                        /* second write is in same sector */
                        status = ptxNDEF_T2TOpWriteBlock(t2tOpComp, (uint8_t)(block_number_ndef_len + 1), &t2tOpComp->WorkBuffer[PTX_T2TOP_BLOCK_SIZE], PTX_T2TOP_BLOCK_SIZE, &t2tOpComp->RxBuffer[0], &rx_len, PTX_T2T_DEFAULT_TIMEOUT_MS);
                    }
                }
            }

            if (ptxStatus_Success == status)
            {
                t2tOpComp->TLVs.NDEFTLV.Length = new_len;
                t2tOpComp->LifeCycle = TagLC_ReadWrite;
            }
        }
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_InvalidParameter);
    }
    return status;
}

/* look for TLVs after CheckMessage has read the memory */
static ptxStatus_t ptxNDEF_T2TTLVCrawler (ptxNDEF_T2TOP_t *t2tOpComp, uint16_t currentBlock)
{
    ptxStatus_t status = ptxStatus_Success;
    uint16_t length_byte_combined;
    uint32_t rx_len;

    if (PTX_COMP_CHECK(t2tOpComp, ptxStatus_Comp_T2TOP))
    {
        uint16_t byte_address = (uint16_t)(currentBlock * 4);
        uint16_t i=0;

        do
        {
            switch (t2tOpComp->WorkBuffer[i])
            {
                case PTX_T2TOP_NULL_TLV_T:
                    break;
                case PTX_T2TOP_LOCK_CONTROL_TLV_T:
                    if ((1 == t2tOpComp->TLVs.NDEFTLV.NDEFTLVFound) && (t2tOpComp->TLVs.NDEFTLV.ByteAddress < byte_address + i) && ((t2tOpComp->TLVs.NDEFTLV.ByteAddress + t2tOpComp->TLVs.NDEFTLV.Length) >= byte_address + i))
                    {
                        /* skip this, as it is not a TLV but a part of the NDEF message */
                    } else
                    {
                        t2tOpComp->TLVs.LockControlTLVs.ByteAddress[t2tOpComp->TLVs.LockControlTLVs.NumberOfTLVs] = (uint8_t)(byte_address + i);
                        t2tOpComp->TLVs.LockControlTLVs.NumberOfTLVs++;
                        i++; // skip the length part of the TLV
                    }
                    break;
                case PTX_T2TOP_MEMORY_CONTROL_TLV_T:
                    if ((1 == t2tOpComp->TLVs.NDEFTLV.NDEFTLVFound) && (t2tOpComp->TLVs.NDEFTLV.ByteAddress < byte_address + i) && ((t2tOpComp->TLVs.NDEFTLV.ByteAddress + t2tOpComp->TLVs.NDEFTLV.Length) >= byte_address + i))
                    {
                        /* skip this, as it is not a TLV but a part of the NDEF message */
                    } else
                    {
                        t2tOpComp->TLVs.MemoryControlTLVs.ByteAddress[t2tOpComp->TLVs.MemoryControlTLVs.NumberOfTLVs] = (uint8_t)(byte_address + i);
                        t2tOpComp->TLVs.MemoryControlTLVs.NumberOfTLVs++;
                        i++; // skip the length part of the TLV
                    }
                    break;
                case PTX_T2TOP_NDEF_TLV_T:
                    if (0 == t2tOpComp->TLVs.NDEFTLV.NDEFTLVFound)
                    {
                        t2tOpComp->TLVs.NDEFTLV.ByteAddress = (uint16_t)(byte_address + i);
                        t2tOpComp->TLVs.NDEFTLV.NDEFTLVFound = 1;

                        /* check if the length byte/s is/are out of bounds of current work buffer content */
                        if ((PTX_T2TOP_BLOCK_SIZE * 4 - 4) >= i)
                        {
                            /* length byte(s) in buffer */

                            i++; // skip to length part of the TLV
                            if (0xFF == t2tOpComp->WorkBuffer[i])
                            { /* 3 byte L */
                                length_byte_combined = (uint16_t)((((uint16_t)t2tOpComp->WorkBuffer[i+1] << PTX_T2TOP_MASK_8) | t2tOpComp->WorkBuffer[i+2]));
                                t2tOpComp->TLVs.NDEFTLV.Length = (uint16_t)(t2tOpComp->TLVs.NDEFTLV.Length + length_byte_combined);
                            } else
                            { /* 1 byte L */
                                t2tOpComp->TLVs.NDEFTLV.Length = (uint16_t)(t2tOpComp->TLVs.NDEFTLV.Length + t2tOpComp->WorkBuffer[i]);
                            }
                        } else
                        {
                            /* length byte(s) out of bounds, check sector and add another block */
                            if (((currentBlock + 4)/256) != (currentBlock/256))
                            {
                                status = ptxNDEF_T2TOpSectorSelect(t2tOpComp, (uint8_t)((currentBlock + 3)/256), &t2tOpComp->RxBuffer[0], &rx_len, PTX_T2T_DEFAULT_TIMEOUT_MS);
                            }
                            if (ptxStatus_Success == status)
                            {
                                status = ptxNDEF_T2TOpReadBlocks(t2tOpComp, (uint8_t)(currentBlock + 4), &t2tOpComp->RxBuffer[0], &rx_len);
                                if (ptxStatus_Success == status)
                                {
                                    /* add the fifth block to the work buffer */
                                    (void)memcpy(&t2tOpComp->WorkBuffer[PTX_T2TOP_BLOCK_SIZE * 4], &t2tOpComp->RxBuffer[0], (uint32_t)(PTX_T2TOP_BLOCK_SIZE));

                                    /* proceed with length byte(s) as above */
                                    i++;
                                    if (0xFF == t2tOpComp->WorkBuffer[i])
                                    { /* 3 byte L */
                                        length_byte_combined = (uint16_t)(((uint16_t)t2tOpComp->WorkBuffer[i+1] << PTX_T2TOP_MASK_8) | t2tOpComp->WorkBuffer[i+2]);
                                        t2tOpComp->TLVs.NDEFTLV.Length = (uint16_t)(t2tOpComp->TLVs.NDEFTLV.Length + length_byte_combined);
                                    } else
                                    { /* 1 byte L */
                                        t2tOpComp->TLVs.NDEFTLV.Length = (uint16_t)(t2tOpComp->TLVs.NDEFTLV.Length + t2tOpComp->WorkBuffer[i]);
                                    }
                                }
                            }
                        }
                    }
                    break;
                case PTX_T2TOP_TERMINATOR_TLV_T:
                    if ((1 == t2tOpComp->TLVs.NDEFTLV.NDEFTLVFound) && (t2tOpComp->TLVs.NDEFTLV.ByteAddress < byte_address + i) && ((t2tOpComp->TLVs.NDEFTLV.ByteAddress + t2tOpComp->TLVs.NDEFTLV.Length) >= byte_address + i))
                    {
                        /* skip this, as it is not a TLV but a part of the NDEF message */
                    } else
                    {
                        t2tOpComp->TLVs.TerminatorTLV.ByteAddress = (uint16_t)(byte_address + i);
                        t2tOpComp->TLVs.TerminatorTLV.TerminatorTLVFound = 1;
                        i = 16; //exit the for loop


                    }
                    break;

                default:
                    break;
            }

            i++;
        } while(i<(PTX_T2TOP_BLOCK_SIZE * 4 - 1));
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_InvalidParameter);
    }
    return status;
}

/* looks through LockControlTLVs and saves the DLAs */
static ptxStatus_t ptxNDEF_T2TDLAProcessor (ptxNDEF_T2TOP_t *t2tOpComp)
{
    ptxStatus_t status = ptxStatus_Success;

    uint16_t lock_byte_address;
    uint32_t rx_len;

    uint8_t DLAPosition;
    /* contains: */
    uint8_t NbrMajorOffsets;
    uint8_t NbrMinorOffsets;

    uint16_t DLA_NbrLockBits;
    uint8_t DLA_NbrBytes;

    uint8_t DLAControl;
    /* contains: */
    uint8_t BLPLB_Index;
    uint8_t MOS_DLA;

    uint16_t DLA_FirstByteAddress;
    uint16_t MajorOffset_Size_DLA;


    if (PTX_COMP_CHECK(t2tOpComp, ptxStatus_Comp_T2TOP))
    {
        for (uint8_t LockTLV = 0; LockTLV < t2tOpComp->TLVs.LockControlTLVs.NumberOfTLVs; LockTLV++)
        {
            lock_byte_address = t2tOpComp->TLVs.LockControlTLVs.ByteAddress[LockTLV];
            status = ptxNDEF_T2TOpReadBlocks(t2tOpComp, (uint8_t)(lock_byte_address / 4), &t2tOpComp->RxBuffer[0], &rx_len);
            if (ptxStatus_Success == status)
            {
                (void)memcpy(&t2tOpComp->WorkBuffer[0], &t2tOpComp->RxBuffer[0], PTX_T2TOP_BLOCK_SIZE * 2);
                DLAPosition = t2tOpComp->WorkBuffer[(lock_byte_address % 4) + 2];
                DLA_NbrLockBits = (uint16_t)t2tOpComp->WorkBuffer[(lock_byte_address % 4) + 3];
                if (0 == DLA_NbrLockBits)
                {
                    DLA_NbrLockBits = 256;
                }
                DLAControl = t2tOpComp->WorkBuffer[(lock_byte_address % 4) + 4];

                NbrMajorOffsets = (DLAPosition >> PTX_T2TOP_MASK_4);
                NbrMinorOffsets = (DLAPosition & ((PTX_T2TOP_MASK_1<<PTX_T2TOP_MASK_4)-PTX_T2TOP_MASK_1));

                DLA_NbrBytes = (uint8_t)(DLA_NbrLockBits / 8);
                if (0 != (DLA_NbrLockBits % 8))
                {
                    DLA_NbrBytes++;
                }

                 BLPLB_Index= (DLAControl >> PTX_T2TOP_MASK_4);
                 MOS_DLA = (DLAControl & ((PTX_T2TOP_MASK_1<<PTX_T2TOP_MASK_4)-PTX_T2TOP_MASK_1));

                t2tOpComp->TLVs.LockControlTLVs.BytesLockedPerLockBit[LockTLV] = (uint8_t)(1 << BLPLB_Index);

                MajorOffset_Size_DLA = (uint16_t)(1 << MOS_DLA);
                DLA_FirstByteAddress = (uint16_t)((NbrMajorOffsets * MajorOffset_Size_DLA) + NbrMinorOffsets);

                t2tOpComp->TLVs.LockControlTLVs.NbrReservedBits[LockTLV] = (uint8_t)(8-(DLA_NbrLockBits % 8));
                if (8 == (8-(DLA_NbrLockBits % 8)))
                {
                    t2tOpComp->TLVs.LockControlTLVs.NbrReservedBits[LockTLV] = 0;
                }
                t2tOpComp->TLVs.LockControlTLVs.LockArea[LockTLV] = DLA_FirstByteAddress;
                t2tOpComp->TLVs.LockControlTLVs.LockArea[LockTLV + 1] = (uint8_t)(DLA_FirstByteAddress + DLA_NbrBytes);
            }
        }
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T2TOP, ptxStatus_InvalidParameter);
    }
    return status;
}

/* looks through LockControlTLVs and saves the reserved areas */
static ptxStatus_t ptxNDEF_T2TRAProcessor(ptxNDEF_T2TOP_t *t2tOpComp)
{
    ptxStatus_t status = ptxStatus_Success;

    uint16_t mem_byte_address;
    uint32_t rx_len;

    uint8_t RAPosition;
    /* contains: */
    uint8_t NbrMajorOffsets;
    uint8_t NbrMinorOffsets;

    uint16_t Rsvd_Area_Size;

    uint8_t RAControl;
    /* contains: */
    uint8_t MOS_RA;

    uint16_t RA_FirstByteAddress;
    uint16_t MajorOffset_Size_RA;

    if (PTX_COMP_CHECK(t2tOpComp, ptxStatus_Comp_T2TOP))
    {
        for (uint8_t MemoryTLV = 0; MemoryTLV < t2tOpComp->TLVs.MemoryControlTLVs.NumberOfTLVs; MemoryTLV++)
        {
            mem_byte_address = t2tOpComp->TLVs.LockControlTLVs.ByteAddress[MemoryTLV];
            status = ptxNDEF_T2TOpReadBlocks(t2tOpComp, (uint8_t)(mem_byte_address / 4), &t2tOpComp->RxBuffer[0], &rx_len);
            if (ptxStatus_Success == status)
            {
                (void)memcpy(&t2tOpComp->WorkBuffer[0], &t2tOpComp->RxBuffer[0], PTX_T2TOP_BLOCK_SIZE * 2);
                RAPosition = t2tOpComp->WorkBuffer[(mem_byte_address % 4) + 2];
                Rsvd_Area_Size = (uint16_t)t2tOpComp->WorkBuffer[(mem_byte_address % 4) + 3];
                if (0 == Rsvd_Area_Size)
                {
                    Rsvd_Area_Size = 256;
                }
                RAControl = t2tOpComp->WorkBuffer[(mem_byte_address % 4) + 4];

                NbrMajorOffsets = (RAPosition >> PTX_T2TOP_MASK_4);
                NbrMinorOffsets = (RAPosition & ((PTX_T2TOP_MASK_1<<PTX_T2TOP_MASK_4)-PTX_T2TOP_MASK_1));

                MOS_RA = (RAControl & ((PTX_T2TOP_MASK_1<<PTX_T2TOP_MASK_4)-PTX_T2TOP_MASK_1));

                MajorOffset_Size_RA = (uint16_t)(1 << MOS_RA);
                RA_FirstByteAddress = (uint16_t)((NbrMajorOffsets * MajorOffset_Size_RA) + NbrMinorOffsets);

                t2tOpComp->TLVs.MemoryControlTLVs.RsvdArea[MemoryTLV] = RA_FirstByteAddress;
                t2tOpComp->TLVs.MemoryControlTLVs.RsvdArea[MemoryTLV + 1] = (uint8_t)(RA_FirstByteAddress + Rsvd_Area_Size);
            }
        }
    }

    return status;
}

/* check RAs and DLAs against current address to ignore, bytes to process is to be set before the operation to either T2T block length or T2T block length * 4 depending on operation read or write  */
static ptxStatus_t ptxNDEF_T2TAreaChecker (ptxNDEF_T2TOP_t *t2tOpComp, uint16_t currentByteAddress, uint16_t *bytesToProcess, uint16_t *preserveBytes)
{
    ptxStatus_t status = ptxStatus_Success;

    uint16_t bytes_to_process = *bytesToProcess;

    uint8_t preserve_counter = 0;

    uint16_t current_byte_address = currentByteAddress;
    uint16_t begin_byte = current_byte_address;
    uint16_t end_byte = (uint16_t)(begin_byte + bytes_to_process);

    if (PTX_COMP_CHECK(t2tOpComp, ptxStatus_Comp_T2TOP) && (NULL != bytesToProcess) && (NULL != preserveBytes))
    {
        /* reset preserve bytes container */
        (void)memset(&preserveBytes[0],0,(uint32_t)(PTX_T2TOP_BLOCK_SIZE * 4));

        for (uint16_t check_address = begin_byte; check_address < end_byte; check_address++)
        {
            /* check DLAs */
            for (uint8_t j = 0; j < (t2tOpComp->TLVs.LockControlTLVs.NumberOfTLVs * 2); j=(uint8_t)(j+2))
            {
                if ((t2tOpComp->TLVs.LockControlTLVs.LockArea[j] < check_address) && (t2tOpComp->TLVs.LockControlTLVs.LockArea[j+1] > check_address))
                {
                    preserveBytes[preserve_counter] = check_address;
                    preserve_counter++;
                    bytes_to_process--;
                }
            }

            /* check RAs */
            for (uint8_t j = 0; j < (t2tOpComp->TLVs.MemoryControlTLVs.NumberOfTLVs * 2); j=(uint8_t)(j+2))
            {
                if ((t2tOpComp->TLVs.MemoryControlTLVs.RsvdArea[j] < check_address) && (t2tOpComp->TLVs.MemoryControlTLVs.RsvdArea[j+1] > check_address))
                {
                    preserveBytes[preserve_counter] = check_address;
                    preserve_counter++;
                    bytes_to_process--;
                }
            }
        }
        *bytesToProcess = bytes_to_process;
    }
    return status;
}

