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
    File        : ptxNativeTag_T3T.c

    Description : Native Tag API for NFC Forum Tag Type 3 (IOT READER - Extension)
*/


/*
 * ####################################################################################################################
 * INCLUDES
 * ####################################################################################################################
 */
#include "ptx_IOT_READER.h"
#include "ptxNDEF_T3TOP.h"
#include "ptxNativeTag_T3T.h"
#include <string.h>

/*
 * ####################################################################################################################
 * DEFINES / TYPES
 * ####################################################################################################################
 */
#define PTX_T3TOP_READ_ATTRIBUTE_INFORMATION_BLOCK_SC           (uint16_t)0x000B
#define PTX_T3TOP_WRITE_ATTRIBUTE_INFORMATION_BLOCK_SC          (uint16_t)0x0009
#define PTX_T3TOP_ATTRIBUTE_INFORMATION_BYTES                   (uint8_t)0x0E
#define PTX_T3TOP_ATTRIBUTE_INFORMATION_BYTES_CHECKSUM          (uint8_t)0x10
#define PTX_T3TOP_SENSF_REQ_SC_NDEF                             (uint16_t)0x12FC
#define PTX_T3TOP_READ_NDEF_SC                                  (uint16_t)0x000B
#define PTX_T3TOP_WRITE_NDEF_SC                                 (uint16_t)0x0009
#define PTX_T3TOP_BLOCK_SIZE                                    (uint8_t)0x10
#define PTX_T3TOP_WRITE_FLAG_ON                                 (uint8_t)0x0F
#define PTX_T3TOP_WRITE_FLAG_OFF                                (uint8_t)0x00
#define PTX_T3TOP_NOS_NDEF                                      (uint8_t)0x01
#define PTX_T3TOP_NOB_NDEF                                      (uint8_t)0x01
#define PTX_T3TOP_CHECK_RSP_OFFSET_DATA                         (uint8_t)0x0D
#define PTX_T3TOP_CHECK_RSP_OFFSET_STATUSFLAG1                  (uint8_t)0x0A
#define PTX_T3TOP_CHECK_RSP_OFFSET_STATUSFLAG2                  (uint8_t)0x0B
#define PTX_T3TOP_CHECK_RSP_OFFSET_NUMBER_BYTES                 (uint8_t)0x0C
#define PTX_T3TOP_STATUSFLAG1_OK                                (uint8_t)0x00
#define PTX_T3TOP_STATUSFLAG2_OK                                (uint8_t)0x00
#define PTX_T3TOP_UPDATE_RSP_OFFSET_STATUSFLAG1                 (uint8_t)0x09
#define PTX_T3TOP_UPDATE_RSP_OFFSET_STATUSFLAG2                 (uint8_t)0x0A
#define PTX_T3TOP_UPDATE_RSP_LENGTH                             (uint8_t)0x0B
#define PTX_T3TOP_UPDATE_RSP_STATUSFLAG2_ACCESS_DENIED          (uint8_t)0x70
#define PTX_T3TOP_UPDATE_RSP_STATUSFLAG2_MAX_NUM_WRITES         (uint8_t)0x71
#define PTX_T3TOP_READ_ONLY_FLAG                                (uint8_t)0x00

/*
 * ####################################################################################################################
 * INTERNAL FUNCTIONS / HELPERS
 * ####################################################################################################################
 */
static ptxStatus_t ptxNDEF_T3TOpCheck  (ptxNDEF_T3TOP_t *t3tComp, uint8_t *NFCID2, uint8_t NoS, uint8_t *serviceCodeList, uint8_t serviceCodeListLen, uint8_t NoB, uint8_t *blockList, uint8_t blockListLen, uint8_t *rx, uint32_t *rxLen, uint32_t msTimeout);
static ptxStatus_t ptxNDEF_T3TOpUpdate (ptxNDEF_T3TOP_t *t3tComp, uint8_t *NFCID2, uint8_t NoS, uint8_t *serviceCodeList, uint8_t serviceCodeListLen, uint8_t NoB, uint8_t *blockList, uint8_t blockListLen, uint8_t *blockData, uint8_t blockDataLen, uint8_t *rx, uint32_t *rxLen, uint32_t msTimeout);
static ptxStatus_t ptxNDEF_T3TOpGetAttributeInformationBlock (ptxNDEF_T3TOP_t *t3tOpComp);
static ptxStatus_t ptxNDEF_T3TOpSetAttributeInformationBlock (ptxNDEF_T3TOP_t *t3tOpComp);
static ptxStatus_t ptxNDEF_T3TOpCalculateChecksum (ptxNDEF_T3TOP_t *t3tOpComp, uint16_t *checksum);

/*
 * ####################################################################################################################
 * API FUNCTIONS
 * ####################################################################################################################
 */
ptxStatus_t ptxNDEF_T3TOpOpen (ptxNDEF_T3TOP_t *t3tOpComp, ptxNDEF_T3TOP_InitParams_t *initParams)
{
    ptxStatus_t status = ptxStatus_Success;
    ptxNativeTag_T3T_InitParams_t T3T_init_params;

    if ((NULL != t3tOpComp) && (NULL != initParams))
    {
        if ((NULL != initParams->IotRd) &&
            (NULL != initParams->TxBuffer) &&
            (0 != initParams->TxBufferSize) &&
            (NULL != initParams->RxBuffer) &&
            (0 != initParams->RxBufferSize))
        {
            /* clear component */
            (void)memset(t3tOpComp, 0, sizeof(ptxNDEF_T3TOP_t));

            /* set members */
            (void)ptxIoTRd_Get_Card_Registry (initParams->IotRd, &t3tOpComp->CardRegistry);

            t3tOpComp->RxBuffer = initParams->RxBuffer;
            t3tOpComp->RxBufferSize = initParams->RxBufferSize;
            t3tOpComp->LifeCycle = TagLC_NoNDEFTag;

            /* initialize lower layer component */
            (void)memset(&T3T_init_params, 0, sizeof(ptxNativeTag_T3T_InitParams_t));
            T3T_init_params.IotRd = initParams->IotRd;
            T3T_init_params.TxBuffer = initParams->TxBuffer;
            T3T_init_params.TxBufferSize = initParams->TxBufferSize;
            T3T_init_params.NFCID2 = initParams->NFCID2;
            T3T_init_params.NFCID2Len = initParams->NFCID2Len;

            status = ptxNativeTag_T3TOpen(&t3tOpComp->NativeTagT3T, &T3T_init_params);

            /* set Component-ID at the end to prevent futher calls in case of an error */
            if (ptxStatus_Success == status)
            {
                t3tOpComp->CompId = ptxStatus_Comp_T3TOP;
            }

        } else
        {
            status = PTX_STATUS(ptxStatus_Comp_T3TOP, ptxStatus_InvalidParameter);
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T3TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNDEF_T3TOpFormatTag (ptxNDEF_T3TOP_t *t3tOpComp)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(t3tOpComp, ptxStatus_Comp_T3TOP))
    {
        status = PTX_STATUS(ptxStatus_Comp_T3TOP, ptxStatus_NotImplemented);

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T3TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNDEF_T3TOpCheckMessage (ptxNDEF_T3TOP_t *t3tOpComp)
{
    ptxStatus_t status = ptxStatus_Success;
    uint8_t nfcid2[8];
    uint8_t mrt_value_check;
    uint8_t mrt_value_update;

    if (PTX_COMP_CHECK(t3tOpComp, ptxStatus_Comp_T3TOP))
    {
        /* set NFCID2 */
        (void)memcpy(&nfcid2[0],&t3tOpComp->CardRegistry->ActiveCard->TechParams.CardFParams.SENSF_RES[2],(uint32_t)PTX_T3T_NFCID2_SIZE);
        mrt_value_check = t3tOpComp->CardRegistry->ActiveCard->TechParams.CardFParams.SENSF_RES[15];
        mrt_value_update = t3tOpComp->CardRegistry->ActiveCard->TechParams.CardFParams.SENSF_RES[16];

        status =  ptxNativeTag_T3TSetTagParams(&t3tOpComp->NativeTagT3T,
                                               &nfcid2[0],
                                               PTX_T3T_NFCID2_SIZE,
                                               mrt_value_check,
                                               mrt_value_update);


        if (ptxStatus_Success == status)
        {
            /* read attribute information block (equivalent to CC params) */
            status = ptxNDEF_T3TOpGetAttributeInformationBlock(t3tOpComp);
        }

        if (ptxStatus_Success == status)
        {
            if (PTX_T3TOP_READ_ONLY_FLAG != t3tOpComp->CCParams.RWFlag)
            {
                t3tOpComp->LifeCycle = (0 != t3tOpComp->CCParams.Ln) ? TagLC_ReadWrite : TagLC_Initialized;

            } else
            {
                t3tOpComp->LifeCycle = TagLC_ReadOnly;
            }
        }
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T3TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNDEF_T3TOpReadMessage (ptxNDEF_T3TOP_t *t3tOpComp, uint8_t *msgBuffer, uint32_t *msgLen)
{
    ptxStatus_t status = ptxStatus_Success;
    uint32_t msg_buffer_size;
    uint8_t nfcid2[8];
    uint8_t bytes_read = 0;
    uint8_t service_code_list[2];
    uint8_t block_list[2];
    uint8_t current_block = 1;
    uint16_t block_list_entry;
    uint32_t rx_len;
    uint8_t mrt_value_check;
    uint8_t mrt_value_update;

    if (PTX_COMP_CHECK(t3tOpComp, ptxStatus_Comp_T3TOP) && (NULL != msgBuffer) && (NULL != msgLen))
    {
        msg_buffer_size = *msgLen;
        service_code_list[0] = (PTX_T3TOP_READ_NDEF_SC & 0xFF);
        service_code_list[1] = (PTX_T3TOP_READ_NDEF_SC >> 8u);

        if (0 != msg_buffer_size)
        {
            switch (t3tOpComp->LifeCycle)
            {
                case TagLC_Initialized:
                case TagLC_ReadWrite:
                case TagLC_ReadOnly:
                    /* OK */
                    break;

                default:
                    status = PTX_STATUS(ptxStatus_Comp_T3TOP, ptxStatus_InvalidState);
                    break;
            }

            /* check if previous writing procedures have finished */
            if (PTX_T3TOP_WRITE_FLAG_ON == t3tOpComp->CCParams.WriteFlag)
            {
                status = PTX_STATUS(ptxStatus_Comp_T3TOP, ptxStatus_T3T_WriteFlagSet);
            }

            if (ptxStatus_Success == status)
            {
                /* set NFCID2 */
                (void)memcpy(&nfcid2[0],&t3tOpComp->CardRegistry->ActiveCard->TechParams.CardFParams.SENSF_RES[2],(uint32_t)PTX_T3T_NFCID2_SIZE);
                mrt_value_check = t3tOpComp->CardRegistry->ActiveCard->TechParams.CardFParams.SENSF_RES[15];
                mrt_value_update = t3tOpComp->CardRegistry->ActiveCard->TechParams.CardFParams.SENSF_RES[16];


                status =  ptxNativeTag_T3TSetTagParams(&t3tOpComp->NativeTagT3T,
                                                       &nfcid2[0],
                                                       PTX_T3T_NFCID2_SIZE,
                                                       mrt_value_check,
                                                       mrt_value_update);


                if (ptxStatus_Success == status)
                {
                    /* read blocks */
                    while ((t3tOpComp->CCParams.Nbc > (current_block-1)) && (ptxStatus_Success == status))
                    {
                        block_list_entry = (uint16_t)((uint16_t)current_block | (uint16_t)0x8000);
                        block_list[0] = (uint8_t)(block_list_entry >> 8u);
                        block_list[1] = (uint8_t)(block_list_entry & 0xFF);
                        rx_len = t3tOpComp->RxBufferSize;

                        status = ptxNDEF_T3TOpCheck(t3tOpComp,t3tOpComp->NativeTagT3T.NFCID2,PTX_T3TOP_NOS_NDEF,service_code_list,sizeof(service_code_list),PTX_T3TOP_NOB_NDEF,block_list,sizeof(block_list),t3tOpComp->RxBuffer,&rx_len,t3tOpComp->NativeTagT3T.TagTimeoutCheck);

                        if (ptxStatus_Success == status)
                        {
                            /* in case of last read operation only copy NDEF bytes to message buffer */
                            if (t3tOpComp->CCParams.Ln < (uint8_t)(bytes_read + PTX_T3TOP_BLOCK_SIZE))
                            {
                                (void)memcpy(&msgBuffer[bytes_read],&t3tOpComp->RxBuffer[0],t3tOpComp->CCParams.Ln - bytes_read); /* block data starts at byte 12 of check response, length byte already truncated */
                                bytes_read = (uint8_t)(bytes_read + t3tOpComp->CCParams.Ln - bytes_read);
                            } else
                            {
                                (void)memcpy(&msgBuffer[bytes_read],&t3tOpComp->RxBuffer[0],PTX_T3TOP_BLOCK_SIZE);
                                bytes_read = (uint8_t)(bytes_read + PTX_T3TOP_BLOCK_SIZE);
                            }
                            current_block++;
                        }
                    }

                    if (ptxStatus_Success == status)
                    {
                        *msgLen = bytes_read;
                    }
                }
            }
        } else
        {
            status = PTX_STATUS(ptxStatus_Comp_T3TOP, ptxStatus_InvalidParameter);
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T3TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNDEF_T3TOpWriteMessage (ptxNDEF_T3TOP_t *t3tOpComp, uint8_t *msgBuffer, uint32_t msgLen)
{
    ptxStatus_t status = ptxStatus_Success;
    uint8_t nfcid2[8];
    uint8_t set_write_flag = 0;
    uint8_t bytes_written = 0;
    uint16_t msg_len;
    uint8_t nbr_padding_bytes = 0;
    uint8_t nbr_data_blocks;
    uint8_t block_data[PTX_T3TOP_BLOCK_SIZE];
    uint8_t EMPTY_NDEF_MESSAGE[] = {0x0D, 0x00, 0x00};
    uint8_t *msg_buffer;
    uint8_t service_code_list[2];
    uint8_t block_list[2];
    uint16_t block_list_entry;
    uint8_t current_block = 1;
    uint32_t rx_len;
    uint8_t mrt_value_check;
    uint8_t mrt_value_update;

    if (PTX_COMP_CHECK(t3tOpComp, ptxStatus_Comp_T3TOP))
    {
        switch (t3tOpComp->LifeCycle)
        {
            case TagLC_Initialized:
            case TagLC_ReadWrite:
                /* OK */
                break;

            default:
                status = PTX_STATUS(ptxStatus_Comp_T3TOP, ptxStatus_InvalidState);
                break;
        }

        if (ptxStatus_Success == status)
        {
            /* set NFCID2 */
            (void)memcpy(&nfcid2[0],&t3tOpComp->CardRegistry->ActiveCard->TechParams.CardFParams.SENSF_RES[2],(uint32_t)PTX_T3T_NFCID2_SIZE);
            mrt_value_check = t3tOpComp->CardRegistry->ActiveCard->TechParams.CardFParams.SENSF_RES[15];
            mrt_value_update = t3tOpComp->CardRegistry->ActiveCard->TechParams.CardFParams.SENSF_RES[16];

            status =  ptxNativeTag_T3TSetTagParams(&t3tOpComp->NativeTagT3T,
                                                   &nfcid2[0],
                                                   PTX_T3T_NFCID2_SIZE,
                                                   mrt_value_check,
                                                   mrt_value_update);
        }

        /* check for other write procedure in progress */
        if (PTX_T3TOP_WRITE_FLAG_ON == t3tOpComp->CCParams.WriteFlag)
        {
            status = PTX_STATUS(ptxStatus_Comp_T3TOP, ptxStatus_InvalidState);
        }

        if (ptxStatus_Success == status)
        {
            /* check for empty message */
            if ((0 != msgLen) && (NULL != msgBuffer))
            {
                msg_buffer = msgBuffer;
                msg_len = (uint16_t)msgLen;
                if (PTX_T3TOP_BLOCK_SIZE < msg_len)
                {
                    set_write_flag = 1;
                }
            } else
            {
                msg_buffer = &EMPTY_NDEF_MESSAGE[0];
                msg_len = sizeof(EMPTY_NDEF_MESSAGE);
            }

            nbr_data_blocks = (uint8_t)(msg_len / PTX_T3TOP_BLOCK_SIZE);
            if (0 != (msg_len % PTX_T3TOP_BLOCK_SIZE))
            {
                nbr_data_blocks++;
            }

            if (t3tOpComp->CCParams.NmaxB < nbr_data_blocks)
            {
                status = PTX_STATUS(ptxStatus_Comp_T3TOP, ptxStatus_InsufficientResources);
            }

            /* check if write flag needs to be set to ON */
            if (1 == set_write_flag)
            {
                t3tOpComp->CCParams.WriteFlag = PTX_T3TOP_WRITE_FLAG_ON;
                status = ptxNDEF_T3TOpSetAttributeInformationBlock(t3tOpComp);
            }

            while ((msg_len > bytes_written) && (ptxStatus_Success == status))
            {
                /* find out if padding is needed and fill data block */
                if ((PTX_T3TOP_BLOCK_SIZE > (msg_len - bytes_written)) && (ptxStatus_Success == status))
                {
                    nbr_padding_bytes = (uint8_t)(PTX_T3TOP_BLOCK_SIZE - (msg_len - bytes_written));
                }
                (void)memcpy(&block_data[0],&msg_buffer[bytes_written],(uint32_t)PTX_T3TOP_BLOCK_SIZE - nbr_padding_bytes);
                if (0 != nbr_padding_bytes)
                {
                    (void)memset(&block_data[PTX_T3TOP_BLOCK_SIZE-nbr_padding_bytes],0x00,nbr_padding_bytes);
                }

                if (ptxStatus_Success == status)
                {
                    service_code_list[0] = (PTX_T3TOP_WRITE_NDEF_SC & 0xFF);
                    service_code_list[1] = (PTX_T3TOP_WRITE_NDEF_SC >> 8u);

                    block_list_entry = (uint16_t)((uint16_t)current_block | (uint16_t)0x8000);
                    block_list[0] = (uint8_t)(block_list_entry >> 8u);
                    block_list[1] = (uint8_t)(block_list_entry & 0xFF);
                    rx_len = t3tOpComp->RxBufferSize;

                    status = ptxNDEF_T3TOpUpdate(t3tOpComp,t3tOpComp->NativeTagT3T.NFCID2,PTX_T3TOP_NOS_NDEF,service_code_list,sizeof(service_code_list),PTX_T3TOP_NOB_NDEF,block_list,sizeof(block_list),block_data,PTX_T3TOP_BLOCK_SIZE,t3tOpComp->RxBuffer,&rx_len,t3tOpComp->NativeTagT3T.TagTimeoutUpdate);

                    if (ptxStatus_Success == status)
                    {
                        bytes_written = (uint8_t)(bytes_written + (PTX_T3TOP_BLOCK_SIZE - nbr_padding_bytes));
                        current_block++;
                        (void)memset(&block_data[0],0x00,PTX_T3TOP_BLOCK_SIZE);
                    }
                }
            }

            if (ptxStatus_Success == status)
            {
                /* write is done, check if write flag needs to be set to OFF and write Ln field */
                if (1 == set_write_flag)
                {
                    t3tOpComp->CCParams.WriteFlag = PTX_T3TOP_WRITE_FLAG_OFF;
                }

                /* update message length infos */
                t3tOpComp->CCParams.Ln = msgLen;
                t3tOpComp->CCParams.Nbc = (t3tOpComp->CCParams.Ln % 16);
                if (0 != (t3tOpComp->CCParams.Ln % 16))
                {
                    t3tOpComp->CCParams.Nbc++;
                }
                status = ptxNDEF_T3TOpSetAttributeInformationBlock(t3tOpComp);

                if (ptxStatus_Success == status)
                {
                    switch (msgLen)
                    {
                        case 0:
                            t3tOpComp->LifeCycle = TagLC_Initialized;
                            break;
                        default:
                            t3tOpComp->LifeCycle = TagLC_ReadWrite;
                            break;
                    }
                }
            } else
            {
                /* write might have failed, check if write flag needs to be set to OFF */
                if (1 == set_write_flag)
                {
                    t3tOpComp->CCParams.WriteFlag = PTX_T3TOP_WRITE_FLAG_OFF;
                }
            }
        }
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T3TOP, ptxStatus_InvalidParameter);
    }

    (void)msgBuffer;
    (void)msgLen;

    return status;
}

ptxStatus_t ptxNDEF_T3TOpLockTag (ptxNDEF_T3TOP_t *t3tOpComp)
{
    ptxStatus_t status = ptxStatus_Success;

    uint8_t nfcid2[8];
    uint8_t mrt_value_check;
    uint8_t mrt_value_update;

    if (PTX_COMP_CHECK(t3tOpComp, ptxStatus_Comp_T3TOP))
    {
        if (0 == t3tOpComp->CCParams.Ln)
        {
            status = PTX_STATUS(ptxStatus_Comp_T3TOP, ptxStatus_InvalidState);
        }

        if (ptxStatus_Success == status)
        {
            /* set NFCID2 */
            (void)memcpy(&nfcid2[0],&t3tOpComp->CardRegistry->ActiveCard->TechParams.CardFParams.SENSF_RES[2],(uint32_t)PTX_T3T_NFCID2_SIZE);
            mrt_value_check = t3tOpComp->CardRegistry->ActiveCard->TechParams.CardFParams.SENSF_RES[15];
            mrt_value_update = t3tOpComp->CardRegistry->ActiveCard->TechParams.CardFParams.SENSF_RES[16];

            status =  ptxNativeTag_T3TSetTagParams(&t3tOpComp->NativeTagT3T,
                                                   &nfcid2[0],
                                                   PTX_T3T_NFCID2_SIZE,
                                                   mrt_value_check,
                                                   mrt_value_update);
        }

        if (ptxStatus_Success == status)
        {
            t3tOpComp->CCParams.RWFlag = PTX_T3TOP_READ_ONLY_FLAG;

            status = ptxNDEF_T3TOpSetAttributeInformationBlock(t3tOpComp);

            if (ptxStatus_Success == status)
            {
                t3tOpComp->LifeCycle = TagLC_ReadOnly;
            }
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T3TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNDEF_T3TOpClose (ptxNDEF_T3TOP_t *t3tOpComp)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(t3tOpComp, ptxStatus_Comp_T3TOP))
    {
        status = ptxNativeTag_T3TClose(&t3tOpComp->NativeTagT3T);
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T3TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

/*
 * ####################################################################################################################
 * INTERNAL FUNCTIONS / CALLBACK(s)
 * ####################################################################################################################
 */
static ptxStatus_t ptxNDEF_T3TOpCheck (ptxNDEF_T3TOP_t *t3tOpComp,
                                uint8_t *NFCID2,
                                uint8_t NoS,
                                uint8_t *serviceCodeList,
                                uint8_t serviceCodeListLen,
                                uint8_t NoB,
                                uint8_t *blockList,
                                uint8_t blockListLen,
                                uint8_t *rx,
                                uint32_t *rxLen,
                                uint32_t msTimeout)
{
    ptxStatus_t status = ptxStatus_Success;
    uint8_t num_bytes_res;

    if (PTX_COMP_CHECK(t3tOpComp, ptxStatus_Comp_T3TOP) && (NULL != NFCID2) && (NULL != serviceCodeList) && (NULL != blockList) && (NULL != rx) && (NULL != rxLen))
    {
        *rxLen = (uint32_t)t3tOpComp->RxBufferSize;

        /* send native tag command */
        status = ptxNativeTag_T3TCheck(&t3tOpComp->NativeTagT3T,NFCID2,NoS,serviceCodeList,serviceCodeListLen,NoB,blockList,blockListLen,rx,rxLen,msTimeout);

        if (ptxStatus_Success == status)
        {
            /* handle response */
            if ((0 != *rxLen) && (PTX_T3TOP_CHECK_RSP_OFFSET_DATA <= *rxLen))
            {
                /* OK, check response */
                if ((PTX_T3TOP_STATUSFLAG1_OK != rx[PTX_T3TOP_CHECK_RSP_OFFSET_STATUSFLAG1]) && (PTX_T3TOP_STATUSFLAG2_OK != rx[PTX_T3TOP_CHECK_RSP_OFFSET_STATUSFLAG2]))
                {
                    /* error, no block data received */
                    status = PTX_STATUS(ptxStatus_Comp_T3TOP, ptxStatus_ACKError);
                } else
                {
                    /* OK, result stays in RxBuffer */
                    num_bytes_res = rx[PTX_T3TOP_CHECK_RSP_OFFSET_NUMBER_BYTES];

                    for (uint8_t i = 0; i<num_bytes_res; i++)
                    {
                        t3tOpComp->RxBuffer[i] = t3tOpComp->RxBuffer[PTX_T3TOP_CHECK_RSP_OFFSET_NUMBER_BYTES+i];
                    }
                    *rxLen = num_bytes_res;
                }
            } else
            {
                status = PTX_STATUS(ptxStatus_Comp_T3TOP, ptxStatus_NscRfError);
            }
        }
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T3TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

static ptxStatus_t ptxNDEF_T3TOpUpdate (ptxNDEF_T3TOP_t *t3tOpComp,
                                 uint8_t *NFCID2,
                                 uint8_t NoS,
                                 uint8_t *serviceCodeList,
                                 uint8_t serviceCodeListLen,
                                 uint8_t NoB,
                                 uint8_t *blockList,
                                 uint8_t blockListLen,
                                 uint8_t *blockData,
                                 uint8_t blockDataLen,
                                 uint8_t *rx,
                                 uint32_t *rxLen,
                                 uint32_t msTimeout)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(t3tOpComp, ptxStatus_Comp_T3TOP) && (NULL != NFCID2) && (NULL != serviceCodeList) && (NULL != blockList) && (NULL != rx) && (NULL != rxLen) && (NULL != blockData))
    {
        *rxLen = (uint32_t)t3tOpComp->RxBufferSize;

        /* send native tag command */
        status = ptxNativeTag_T3TUpdate(&t3tOpComp->NativeTagT3T,NFCID2,NoS,serviceCodeList,serviceCodeListLen,NoB,blockList,blockListLen,blockData,blockDataLen,rx,rxLen,msTimeout);

        if (ptxStatus_Success == status)
        {
            /* handle response */
            if (0 != *rxLen)
            {
                /* OK, check response */
                if ((PTX_T3TOP_UPDATE_RSP_LENGTH <= *rxLen) && (PTX_T3TOP_STATUSFLAG1_OK != rx[PTX_T3TOP_UPDATE_RSP_OFFSET_STATUSFLAG1]) && (PTX_T3TOP_STATUSFLAG2_OK != rx[PTX_T3TOP_UPDATE_RSP_OFFSET_STATUSFLAG2]))
                {
                    /* error */
                    switch (rx[PTX_T3TOP_UPDATE_RSP_OFFSET_STATUSFLAG2])
                    {
                        case PTX_T3TOP_UPDATE_RSP_STATUSFLAG2_ACCESS_DENIED:
                            /* cannot write to memory */
                            status = PTX_STATUS(ptxStatus_Comp_T3TOP,ptxStatus_AccessDenied);
                            break;
                        case PTX_T3TOP_UPDATE_RSP_STATUSFLAG2_MAX_NUM_WRITES:
                            /* operation successful, but exceeded the max number of writes to memory */
                            break;
                        default:
                            status = PTX_STATUS(ptxStatus_Comp_T3TOP, ptxStatus_NscRfError);
                            break;
                    }

                } else
                {
                    /* OK */
                }
            } else
            {
                status = PTX_STATUS(ptxStatus_Comp_T3TOP, ptxStatus_NscRfError);
            }
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T3TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

static ptxStatus_t ptxNDEF_T3TOpCalculateChecksum (ptxNDEF_T3TOP_t *t3tOpComp, uint16_t *checksum)
{
    ptxStatus_t status = ptxStatus_Success;
    if (PTX_COMP_CHECK(t3tOpComp, ptxStatus_Comp_T3TOP) && (NULL != checksum))
    {
        t3tOpComp->CCParams.Checksum = (uint16_t)(((uint16_t)t3tOpComp->CCParams.AttributeInformationBlock[14] << 8u) | (uint16_t)t3tOpComp->CCParams.AttributeInformationBlock[15]);

        for (uint8_t i = 0; i<PTX_T3TOP_ATTRIBUTE_INFORMATION_BYTES; i++)
        {
            *checksum = (uint16_t)(*checksum + t3tOpComp->CCParams.AttributeInformationBlock[i]);
        }
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T3TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

static ptxStatus_t ptxNDEF_T3TOpGetAttributeInformationBlock (ptxNDEF_T3TOP_t *t3tOpComp)
{
    ptxStatus_t status = ptxStatus_Success;
    uint8_t service_code_list[2];
    uint8_t block_list[2];
    uint32_t rx_len;
    uint16_t checksum = 0;

    if (PTX_COMP_CHECK(t3tOpComp, ptxStatus_Comp_T3TOP))
    {
        service_code_list[0] = (PTX_T3TOP_READ_ATTRIBUTE_INFORMATION_BLOCK_SC & 0xFF);
        service_code_list[1] = (PTX_T3TOP_READ_ATTRIBUTE_INFORMATION_BLOCK_SC >> 8u);
        block_list[0] = 0x80;
        block_list[1] = 0x00;
        rx_len = t3tOpComp->RxBufferSize;

        /* read the attribute information block */
        status = ptxNDEF_T3TOpCheck(t3tOpComp,t3tOpComp->NativeTagT3T.NFCID2,0x01,service_code_list,0x02,0x01,block_list,0x02,t3tOpComp->RxBuffer,&rx_len,t3tOpComp->NativeTagT3T.TagTimeoutCheck);
        if ((ptxStatus_Success == status) && (PTX_T3TOP_ATTRIBUTE_INFORMATION_BYTES_CHECKSUM == rx_len))
        {
            (void)memcpy(&t3tOpComp->CCParams.AttributeInformationBlock[0],&t3tOpComp->RxBuffer[0],(uint32_t)rx_len);

            status = ptxNDEF_T3TOpCalculateChecksum(t3tOpComp, &checksum);

            if (t3tOpComp->CCParams.Checksum == checksum)
            {
                /* fill the components */
                t3tOpComp->CCParams.Version = t3tOpComp->CCParams.AttributeInformationBlock[0];
                if (PTX_T3T_SUPPORTED_VERSION != t3tOpComp->CCParams.Version)
                {
                    status = PTX_STATUS(ptxStatus_Comp_T3TOP, ptxStatus_InvalidParameter);
                }
                t3tOpComp->CCParams.MajorVersion = (t3tOpComp->CCParams.Version >> 4u);
                t3tOpComp->CCParams.MinorVersion = (t3tOpComp->CCParams.Version & 0xFF);

                t3tOpComp->CCParams.Nbr = t3tOpComp->CCParams.AttributeInformationBlock[1];
                t3tOpComp->CCParams.Nbw = t3tOpComp->CCParams.AttributeInformationBlock[2];

                t3tOpComp->CCParams.NmaxB = (uint16_t)(((uint16_t)t3tOpComp->CCParams.AttributeInformationBlock[3] << 8u) | (uint16_t)t3tOpComp->CCParams.AttributeInformationBlock[4]); /* byte 3 upper, byte 4 lower */

                (void)memcpy(&t3tOpComp->CCParams.RFU[0],&t3tOpComp->CCParams.AttributeInformationBlock[5],(uint32_t)4);

                t3tOpComp->CCParams.WriteFlag = t3tOpComp->CCParams.AttributeInformationBlock[9];
                t3tOpComp->CCParams.RWFlag = t3tOpComp->CCParams.AttributeInformationBlock[10];

                t3tOpComp->CCParams.Ln  = (((uint32_t)t3tOpComp->CCParams.AttributeInformationBlock[11] << 16u) | ((uint32_t)t3tOpComp->CCParams.AttributeInformationBlock[12] << 8u) | (uint32_t)t3tOpComp->CCParams.AttributeInformationBlock[13]);
                if (0 == t3tOpComp->CCParams.Ln % 16)
                {
                    t3tOpComp->CCParams.Nbc = (uint16_t)(t3tOpComp->CCParams.Ln / 16);
                } else
                {
                    t3tOpComp->CCParams.Nbc = (uint16_t)((t3tOpComp->CCParams.Ln / 16) + 1);
                }
            } else
            {
                status = PTX_STATUS(ptxStatus_Comp_T3TOP, ptxStatus_InvalidParameter);
            }
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T3TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

static ptxStatus_t ptxNDEF_T3TOpSetAttributeInformationBlock (ptxNDEF_T3TOP_t *t3tOpComp)
{
    /* translate the attributes back into bytes and write the block */
    ptxStatus_t status = ptxStatus_Success;
    uint8_t attribute_information[16];
    uint8_t service_code_list[2];
    uint8_t block_list[2];
    uint32_t rx_len;
    uint16_t checksum = 0;

    if (PTX_COMP_CHECK(t3tOpComp, ptxStatus_Comp_T3TOP))
    {
        /* get the components into a data block (already updated outside the function) */
        attribute_information[0]  = t3tOpComp->CCParams.Version;
        attribute_information[1]  = t3tOpComp->CCParams.Nbr;
        attribute_information[2]  = t3tOpComp->CCParams.Nbw;

        attribute_information[3]  = (uint8_t)(t3tOpComp->CCParams.NmaxB >> 8u);
        attribute_information[4]  = (uint8_t)(t3tOpComp->CCParams.NmaxB & 0xFF);

        (void)memcpy(&attribute_information[5],&t3tOpComp->CCParams.RFU[0],(uint32_t)4);

        attribute_information[9]  = t3tOpComp->CCParams.WriteFlag;
        attribute_information[10] = t3tOpComp->CCParams.RWFlag;

        attribute_information[11] = (uint8_t)(t3tOpComp->CCParams.Ln >> 16u);
        attribute_information[12] = (uint8_t)((t3tOpComp->CCParams.Ln >> 8u) & 0xFF);
        attribute_information[13] = (uint8_t)(t3tOpComp->CCParams.Ln & 0xFF);

        (void)memcpy(&t3tOpComp->CCParams.AttributeInformationBlock[0],&attribute_information[0],(uint32_t)14);
        status = ptxNDEF_T3TOpCalculateChecksum(t3tOpComp, &checksum);

        attribute_information[14] = (uint8_t)(checksum >> 8u);
        attribute_information[15] = (uint8_t)(checksum & 0xFF);

        /* write attribute block */
        service_code_list[0] = (PTX_T3TOP_WRITE_ATTRIBUTE_INFORMATION_BLOCK_SC & 0xFF);
        service_code_list[1] = (PTX_T3TOP_WRITE_ATTRIBUTE_INFORMATION_BLOCK_SC >> 8u);
        block_list[0] = 0x80;
        block_list[1] = 0x00;

        rx_len = t3tOpComp->RxBufferSize;
        status = ptxNDEF_T3TOpUpdate(t3tOpComp,t3tOpComp->NativeTagT3T.NFCID2,PTX_T3TOP_NOS_NDEF,service_code_list,sizeof(service_code_list),PTX_T3TOP_NOS_NDEF,block_list,sizeof(block_list),attribute_information,sizeof(attribute_information),t3tOpComp->RxBuffer,&rx_len,t3tOpComp->NativeTagT3T.TagTimeoutUpdate);

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T3TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

