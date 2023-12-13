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
    File        : ptxNativeTag_T4T.c

    Description : Native Tag API for NFC Forum Tag Type 4 (IOT READER - Extension)
*/


/*
 * ####################################################################################################################
 * INCLUDES
 * ####################################################################################################################
 */
#include "ptx_IOT_READER.h"
#include "ptxNDEF_T4TOP.h"
#include "ptxNativeTag_T4T.h"
#include <string.h>

/*
 * ####################################################################################################################
 * DEFINES / TYPES
 * ####################################################################################################################
 */
#define PTX_T4T_CLASS_BYTE_NON_SECURE_MESSAGING					(uint8_t)0x00
#define PTX_T4T_INSTRUCTION_CODE_SELECT							(uint8_t)0xA4
#define PTX_T4T_INSTRUCTION_CODE_READ_BINARY					(uint8_t)0xB0
#define PTX_T4T_INSTRUCTION_CODE_READ_BINARY_ODO				(uint8_t)0xB1
#define PTX_T4T_INSTRUCTION_CODE_UPDATE_BINARY					(uint8_t)0xD6
#define PTX_T4T_INSTRUCTION_CODE_UPDATE_BINARY_ODO				(uint8_t)0xD7

#define PTX_T4T_SUCCESS_FLAG_SW1                                (uint8_t)0x90
#define PTX_T4T_SUCCESS_FLAG_SW2                                (uint8_t)0x00

#define PTX_T4T_STATUS_WORD_LENGTH                              (uint8_t)0x02
#define PTX_T4T_SW1_ERROR_WRONG_P1P2                            (uint8_t)0x6A
#define PTX_T4T_SW2_ERROR_FILE_OR_APP_NOT_FOUND                 (uint8_t)0x82

#define PTX_T4T_SELECT_FAIL_FLAG_SW1							(uint8_t)0x6A
#define PTX_T4T_SELECT_FAIL_FLAG_SW2							(uint8_t)0x82
#define PTX_T4T_READ_FAIL_FLAG_SW1_SIMPLE						(uint8_t)0x67
#define PTX_T4T_READ_FAIL_FLAG_SW2_SIMPLE						(uint8_t)0x00
#define PTX_T4T_READ_FAIL_FLAG_SW1_EXT							(uint8_t)0x6C

#define PTX_T4T_BER_TLV_T_FIELD									(uint8_t)0x54

#define PTX_T4T_NDEF_LENGTH										(uint8_t)0x08
#define PTX_T4T_ENDEF_LENGTH									(uint8_t)0x0A

/* defines for NDEF app select */
#define P1_NDEF_APP_SELECT										(uint8_t)0x04
#define P2_NDEF_APP_SELECT										(uint8_t)0x00
#define LC_FIELD_NDEF_APP_SELECT								(uint8_t)0x07
#define LE_FIELD_NDEF_APP_SELECT								(uint8_t)0x00

/* defines for CC file select */
#define P1_CC_SELECT											(uint8_t)0x00
#define P2_CC_SELECT											(uint8_t)0x0C
#define LC_FIELD_CC_SELECT										(uint8_t)0x02
#define LE_FIELD_CC_SELECT										(uint8_t)0x00

/* defines for NDEF file select */
#define P1_NDEF_FILE_SELECT										(uint8_t)0x00
#define P2_NDEF_FILE_SELECT										(uint8_t)0x0C
#define LC_FIELD_NDEF_FILE_SELECT								(uint8_t)0x02
#define LE_FIELD_NDEF_FILE_SELECT								(uint8_t)0x00

#define PTX_T4T_MAX_BYTES_SEND                                  (uint8_t)0xFD
#define PTX_T4T_MAX_BYTES_RECEIVE                               (uint8_t)0xFD
#define PTX_T4T_MAX_NLEN                                        (uint16_t)0x7FFD
#define PTX_T4T_MAX_ENLEN                                       (uint32_t)0xFFFFFFFA


/*
 * ####################################################################################################################
 * INTERNAL FUNCTIONS / HELPERS
 * ####################################################################################################################
 */
static ptxStatus_t ptxNDEF_T4TOpSelect (ptxNDEF_T4TOP_t *t4tOpComp, uint8_t paramByte1, uint8_t paramByte2, uint8_t *data, uint8_t nbrDataBytes, uint8_t expectedResponseLen, uint8_t *rx, uint32_t *rxLen, uint32_t msTimeout);
static ptxStatus_t ptxNDEF_T4TOpReadBinary (ptxNDEF_T4TOP_t *t4tOpComp, uint32_t offset, uint8_t nbrExpectedResponseBytes, uint8_t *rx, uint32_t *rxLen, uint32_t msTimeout);
static ptxStatus_t ptxNDEF_T4TOpUpdateBinary (ptxNDEF_T4TOP_t *t4tOpComp, uint32_t offset, uint8_t *dataField, uint8_t nbrDatabytes, uint8_t *rx, uint32_t *rxLen, uint32_t msTimeout);
static ptxStatus_t ptxNDEF_T4TOpGetCapabilityContainerInfo (ptxNDEF_T4TOP_t *t4tOpComp);
static ptxStatus_t ptxNDEF_T4TOpUpdateLength (ptxNDEF_T4TOP_t *t4tOpComp, uint32_t newLen);
/*
 * ####################################################################################################################
 * API FUNCTIONS
 * ####################################################################################################################
 */
ptxStatus_t ptxNDEF_T4TOpOpen (ptxNDEF_T4TOP_t *t4tOpComp, ptxNDEF_T4TOP_InitParams_t *initParams)
{
    ptxStatus_t status = ptxStatus_Success;
    ptxNativeTag_T4T_InitParams_t T4T_init_params;

    if ((NULL != t4tOpComp) && (NULL != initParams))
    {
        if ((NULL != initParams->IotRd) &&
            (NULL != initParams->TxBuffer) &&
            (0 != initParams->TxBufferSize) &&
            (NULL != initParams->RxBuffer) &&
            (0 != initParams->RxBufferSize))
        {
            /* clear component */
            (void)memset(t4tOpComp, 0, sizeof(ptxNDEF_T4TOP_t));

            /* set members */
            (void)ptxIoTRd_Get_Card_Registry (initParams->IotRd, &t4tOpComp->CardRegistry);

            t4tOpComp->RxBuffer = initParams->RxBuffer;
            t4tOpComp->RxBufferSize = initParams->RxBufferSize;
            t4tOpComp->LifeCycle = TagLC_NoNDEFTag;

            /* initialize lower layer component */
            (void)memset(&T4T_init_params, 0, sizeof(ptxNativeTag_T4T_InitParams_t));
            T4T_init_params.IotRd = initParams->IotRd;
            T4T_init_params.TxBuffer = initParams->TxBuffer;
            T4T_init_params.TxBufferSize = initParams->TxBufferSize;

            status = ptxNativeTag_T4TOpen(&t4tOpComp->NativeTagT4T, &T4T_init_params);

            /* set Component-ID at the end to prevent futher calls in case of an error */
            if (ptxStatus_Success == status)
            {
                t4tOpComp->CompId = ptxStatus_Comp_T4TOP;
            }

        } else
        {
            status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_InvalidParameter);
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNDEF_T4TOpFormatTag (ptxNDEF_T4TOP_t *t4tOpComp)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(t4tOpComp, ptxStatus_Comp_T4TOP))
    {
        status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_NotImplemented);

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNDEF_T4TOpCheckMessage (ptxNDEF_T4TOP_t *t4tOpComp)
{
    ptxStatus_t status = ptxStatus_Success;
    uint32_t rx_len;
    uint8_t nbr_nlen_bytes;
    uint32_t nlen_offset = 0x00;
    uint8_t DATA_FIELD_NDEF_APP_SELECT[] = {0xD2,0x76,0x00,0x00,0x85,0x01,0x01}; /* AID for 2.0 and 3.0 */

    if (PTX_COMP_CHECK(t4tOpComp, ptxStatus_Comp_T4TOP))
    {
        /* select NDEF application */
        t4tOpComp->NativeTagT4T.Fields.nbr_data_length_bytes = 1u;
        t4tOpComp->NativeTagT4T.Fields.nbr_expected_length_bytes = 1u;
        status = ptxNDEF_T4TOpSelect(t4tOpComp,P1_NDEF_APP_SELECT,P2_NDEF_APP_SELECT,
                                    &DATA_FIELD_NDEF_APP_SELECT[0],
                                    LC_FIELD_NDEF_APP_SELECT,LE_FIELD_NDEF_APP_SELECT,
                                    &t4tOpComp->RxBuffer[0],&rx_len,PTX_T4T_DEFAULT_TIMEOUT_MS);

        if (ptxStatus_Success == status)
        {
            /* get CC info */
            status = ptxNDEF_T4TOpGetCapabilityContainerInfo(t4tOpComp);

            if (ptxStatus_Success == status)
            {
                t4tOpComp->LifeCycle = TagLC_Initialized;
                /* select NDEF file */
                t4tOpComp->NativeTagT4T.Fields.nbr_data_length_bytes = 1u;
                t4tOpComp->NativeTagT4T.Fields.nbr_expected_length_bytes = 0;

                status = ptxNDEF_T4TOpSelect(t4tOpComp,P1_NDEF_FILE_SELECT,P2_NDEF_FILE_SELECT,
                                            &t4tOpComp->CCParams.NDEFFileIdentifier[0],
                                            LC_FIELD_NDEF_FILE_SELECT,LE_FIELD_NDEF_FILE_SELECT,
                                            &t4tOpComp->RxBuffer[0],&rx_len,PTX_T4T_DEFAULT_TIMEOUT_MS);

                if (ptxStatus_Success == status)
                {
                    switch (t4tOpComp->CCParams.MappingVersion)
                    {
                        case 0x20:
                            nbr_nlen_bytes = 2;

                            status = ptxNDEF_T4TOpReadBinary(t4tOpComp,nlen_offset,nbr_nlen_bytes,
                                                            &t4tOpComp->RxBuffer[0],&rx_len,PTX_T4T_DEFAULT_TIMEOUT_MS);

                            if ((ptxStatus_Success == status) && (nbr_nlen_bytes == rx_len))
                            {
                                (void)memcpy(&t4tOpComp->NLEN.NLEN[0],&t4tOpComp->RxBuffer[0],(uint32_t)nbr_nlen_bytes);
                                t4tOpComp->NLEN.NbrNLENBytes = nbr_nlen_bytes;
                            }

                            break;
                        case 0x30:
                            nbr_nlen_bytes = 4;

                            status = ptxNDEF_T4TOpReadBinary(t4tOpComp,nlen_offset,nbr_nlen_bytes,
                                                            &t4tOpComp->RxBuffer[0],&rx_len,PTX_T4T_DEFAULT_TIMEOUT_MS);

                            if ((ptxStatus_Success == status) && (nbr_nlen_bytes == rx_len))
                            {
                                (void)memcpy(&t4tOpComp->NLEN.NLEN[0],&t4tOpComp->RxBuffer[0],(uint32_t)nbr_nlen_bytes);
                                t4tOpComp->NLEN.NbrNLENBytes = nbr_nlen_bytes;
                            }

                            break;
                        default:
                            status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_InvalidParameter);
                            break;
                    }

                    if (ptxStatus_Success == status)
                    {
                        switch (t4tOpComp->CCParams.MappingVersion)
                        {
                            case 0x20:
                                t4tOpComp->NLEN.DigitNLEN = (uint16_t)(t4tOpComp->NLEN.NLEN[1] | (t4tOpComp->NLEN.NLEN[0] << 8u));
                                break;
                            case 0x30:
                                t4tOpComp->NLEN.DigitNLEN = (uint32_t)(t4tOpComp->NLEN.NLEN[3] | (t4tOpComp->NLEN.NLEN[2] << 8u) | (t4tOpComp->NLEN.NLEN[1]) << 16u | (t4tOpComp->NLEN.NLEN[0]) << 24u);
                                break;
                            default:
                                status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_InvalidParameter);
                                break;
                        }
                        if (0 != t4tOpComp->NLEN.DigitNLEN)
                        {
                            switch (t4tOpComp->CCParams.NDEFAccessWrite)
                            {
                                case 0x00:
                                    t4tOpComp->LifeCycle = TagLC_ReadWrite;
                                    break;

                                default:
                                    t4tOpComp->LifeCycle = TagLC_ReadOnly;
                                    break;
                            }
                        }
                    }
                }
            } else
            {
                t4tOpComp->LifeCycle = TagLC_NoNDEFTag;
            }
        }
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNDEF_T4TOpReadMessage (ptxNDEF_T4TOP_t *t4tOpComp, uint8_t *msgBuffer, uint32_t *msgLen)
{
    ptxStatus_t status = ptxStatus_Success;
    uint32_t msg_buffer_size;
    uint32_t rx_len;
    uint8_t nbr_nlen_bytes;
    uint16_t bytes_to_read;
    uint32_t bytes_read = 0;

    if (PTX_COMP_CHECK(t4tOpComp, ptxStatus_Comp_T4TOP) && (NULL != msgBuffer) && (NULL != msgLen))
    {
        msg_buffer_size = *msgLen;
        nbr_nlen_bytes = t4tOpComp->NLEN.NbrNLENBytes;

        if (0 != msg_buffer_size)
        {
            /* check tag LC */
            switch(t4tOpComp->LifeCycle)
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
                /* read NDEF file after (E)NLEN bytes, SELECT was done in check */

                if ((uint32_t)(t4tOpComp->CCParams.MLeDigit-PTX_T4T_STATUS_WORD_LENGTH) > t4tOpComp->NLEN.DigitNLEN)
                {
                    /* read message in one go */

                    status = ptxNDEF_T4TOpReadBinary(t4tOpComp,(uint32_t)nbr_nlen_bytes,
                                                    (uint8_t)t4tOpComp->NLEN.DigitNLEN,
                                                    &t4tOpComp->RxBuffer[0],&rx_len,PTX_T4T_DEFAULT_TIMEOUT_MS);
                    bytes_read = (uint32_t)(bytes_read + rx_len);

                    if ((ptxStatus_Success == status) && (t4tOpComp->NLEN.DigitNLEN == bytes_read))
                    {
                        (void)memcpy(&msgBuffer[0],&t4tOpComp->RxBuffer[0],bytes_read);
                        msg_buffer_size = t4tOpComp->NLEN.DigitNLEN;
                    } else
                    {
                        msg_buffer_size = 0;
                    }
                } else
                {
                    /* read message in parts */
                    while ((t4tOpComp->NLEN.DigitNLEN - (uint16_t)(t4tOpComp->CCParams.MLeDigit-PTX_T4T_STATUS_WORD_LENGTH)) > bytes_read)
                    {
                        bytes_to_read = t4tOpComp->CCParams.MLeDigit;

                        status = ptxNDEF_T4TOpReadBinary(t4tOpComp,(uint32_t)(nbr_nlen_bytes+bytes_read),
                                                        (uint8_t)t4tOpComp->CCParams.MLeDigit,
                                                        &t4tOpComp->RxBuffer[0],&rx_len,PTX_T4T_DEFAULT_TIMEOUT_MS);
                        if (ptxStatus_Success == status)
                        {
                            (void)memcpy(&msgBuffer[bytes_read],&t4tOpComp->RxBuffer[0],bytes_to_read);
                            bytes_read = (uint32_t)(bytes_read + bytes_to_read);
                        }
                    }
                    /* last read step handled separately */
                    bytes_to_read = (uint16_t)(t4tOpComp->NLEN.DigitNLEN - bytes_read);

                    status = ptxNDEF_T4TOpReadBinary(t4tOpComp,(uint32_t)(nbr_nlen_bytes+bytes_read),
                                                    (uint8_t)bytes_to_read,
                                                    &t4tOpComp->RxBuffer[0],&rx_len,PTX_T4T_DEFAULT_TIMEOUT_MS);
                    if (ptxStatus_Success == status)
                    {
                        (void)memcpy(&msgBuffer[bytes_read],&t4tOpComp->RxBuffer[0],bytes_to_read);
                        bytes_read = (uint32_t)(bytes_read + bytes_to_read);
                    }

                    if ((ptxStatus_Success == status) && (t4tOpComp->NLEN.DigitNLEN == bytes_read))
                    {
                        msg_buffer_size = t4tOpComp->NLEN.DigitNLEN;
                    } else
                    {
                        msg_buffer_size = 0;
                    }
                }
                *msgLen = msg_buffer_size;
            }
        } else
        {
            status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_InvalidParameter);
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNDEF_T4TOpWriteMessage (ptxNDEF_T4TOP_t *t4tOpComp, uint8_t *msgBuffer, uint32_t msgLen)
{
    ptxStatus_t status = ptxStatus_Success;
    uint32_t msg_len;
    uint32_t bytes_written = 0;
    uint8_t bytes_to_write;
    uint16_t ndef_file_size;
    uint8_t nbr_nlen_bytes;
    uint8_t *msg_buffer;
    uint8_t EMPTY_NDEF_MESSAGE[3] = {0xD0, 0x00, 0x00};
    uint8_t empty_record_len = 0x03;
    uint8_t c_adpu_size;
    uint32_t rx_len;

    if (PTX_COMP_CHECK(t4tOpComp, ptxStatus_Comp_T4TOP))
    {
        msg_len = msgLen;
        msg_buffer = msgBuffer;
        ndef_file_size = (uint16_t)t4tOpComp->CCParams.NDEFFileSize;
        nbr_nlen_bytes = t4tOpComp->NLEN.NbrNLENBytes;
        c_adpu_size = 5u; /* class, inst, p1, p2, Le */

        /* check tag LC */
        switch (t4tOpComp->LifeCycle)
        {
            case TagLC_Initialized:
            case TagLC_ReadWrite:
                /* OK */
                break;

            default:
                status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_InvalidState);
                break;
        }

        if (0 != msg_len)
        {
            if (ptxStatus_Success == status)
            {
                if (ndef_file_size >= msg_len)
                {
                    if ((uint16_t)(t4tOpComp->CCParams.MLcDigit - c_adpu_size) >= msg_len) /* MLcDigit is by standard 0xFFFF at max */
                    {
                        /* reset NLEN before write */
                        status = ptxNDEF_T4TOpUpdateLength(t4tOpComp, 0);

                        if (ptxStatus_Success == status)
                        {
                            /* write message in one go */
                            status = ptxNDEF_T4TOpUpdateBinary(t4tOpComp,(uint32_t)nbr_nlen_bytes,&msg_buffer[0],(uint8_t)msg_len,
                                                              &t4tOpComp->RxBuffer[0],&rx_len,PTX_T4T_DEFAULT_TIMEOUT_MS);

                            if (ptxStatus_Success == status)
                            {
                                bytes_written = (uint32_t)(bytes_written + msg_len);
                            }
                        }
                    } else
                    {
                        /* reset NLEN before write */
                        status = ptxNDEF_T4TOpUpdateLength(t4tOpComp, 0);

                        if (ptxStatus_Success == status)
                        {
                            /* write message in parts */
                            while ((uint16_t)(msg_len - (t4tOpComp->CCParams.MLcDigit) - c_adpu_size) > bytes_written)
                            {
                                bytes_to_write = (uint8_t)t4tOpComp->CCParams.MLcDigit;

                                status = ptxNDEF_T4TOpUpdateBinary(t4tOpComp,(uint32_t)(nbr_nlen_bytes+bytes_written),&msg_buffer[bytes_written],bytes_to_write,
                                                                  &t4tOpComp->RxBuffer[0],&rx_len,PTX_T4T_DEFAULT_TIMEOUT_MS);

                                if (ptxStatus_Success == status)
                                {
                                    bytes_written = (uint32_t)(bytes_written + bytes_to_write);
                                }
                            }
                            /* last read step handled separately */
                            bytes_to_write = (uint8_t)(msg_len - bytes_written);

                            status = ptxNDEF_T4TOpUpdateBinary(t4tOpComp,(uint32_t)(nbr_nlen_bytes+bytes_written),&msg_buffer[bytes_written],bytes_to_write,
                                                              &t4tOpComp->RxBuffer[0],&rx_len,PTX_T4T_DEFAULT_TIMEOUT_MS);

                            if (ptxStatus_Success == status)
                            {
                                bytes_written = (uint32_t)(bytes_written + bytes_to_write);
                            }
                        }
                    }

                    /* message written correctly? */
                    if ((ptxStatus_Success == status) && (msgLen == bytes_written))
                    {
                        /* OK, update (E)NLEN */
                        status = ptxNDEF_T4TOpUpdateLength(t4tOpComp, msg_len);
                    }
                } else
                {
                    status = PTX_STATUS(ptxStatus_Comp_T5TOP, ptxStatus_InsufficientResources);
                }

            }
        } else
        {
            /* reset NLEN before write */
            status = ptxNDEF_T4TOpUpdateLength(t4tOpComp, 0);

            if (ptxStatus_Success == status)
            {
                /* set empty message */
                msg_buffer = &EMPTY_NDEF_MESSAGE[0];
                status = ptxNDEF_T4TOpUpdateBinary(t4tOpComp,(uint32_t)nbr_nlen_bytes,&msg_buffer[0],empty_record_len,
                                                  &t4tOpComp->RxBuffer[0],&rx_len,PTX_T4T_DEFAULT_TIMEOUT_MS);
                if (ptxStatus_Success == status)
                {
                    /* OK, update (E)NLEN */
                    status = ptxNDEF_T4TOpUpdateLength(t4tOpComp, empty_record_len);
                    if (ptxStatus_Success == status)
                    {
                        t4tOpComp->LifeCycle = TagLC_Initialized;
                    }
                }
            }
        }
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_InvalidParameter);
    }

    (void)msgBuffer;
    (void)msgLen;

    return status;
}

ptxStatus_t ptxNDEF_T4TOpLockTag (ptxNDEF_T4TOP_t *t4tOpComp)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(t4tOpComp, ptxStatus_Comp_T4TOP))
    {
        status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_Success); /* there is no procedure to lock the tag via the NDEF API */

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNDEF_T4TOpClose (ptxNDEF_T4TOP_t *t4tOpComp)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(t4tOpComp, ptxStatus_Comp_T4TOP))
    {
        status = ptxNativeTag_T4TClose(&t4tOpComp->NativeTagT4T);
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

/*
 * ####################################################################################################################
 * INTERNAL FUNCTIONS / CALLBACK(s)
 * ####################################################################################################################
 */
static ptxStatus_t ptxNDEF_T4TOpSelect (ptxNDEF_T4TOP_t *t4tOpComp,
                                        uint8_t paramByte1,
                                        uint8_t paramByte2,
                                        uint8_t *data,
                                        uint8_t nbrDataBytes,
                                        uint8_t expectedResponseLen,
                                        uint8_t *rx,
                                        uint32_t *rxLen,
                                        uint32_t msTimeout)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(t4tOpComp, ptxStatus_Comp_T4TOP) && (NULL != data) && (NULL != rx) && (NULL != rxLen))
    {
        *rxLen = (uint32_t)t4tOpComp->RxBufferSize;

        status = ptxNativeTag_T4TSelect(&t4tOpComp->NativeTagT4T,paramByte1,paramByte2,&data[0],nbrDataBytes,expectedResponseLen,rx,rxLen,msTimeout);

        if (ptxStatus_Success == status)
        {
            if (0 != *rxLen)
            {
                if (PTX_T4T_STATUS_WORD_LENGTH <= *rxLen)
                {
                    if ((PTX_T4T_SUCCESS_FLAG_SW1 == rx[*rxLen-2]) && (PTX_T4T_SUCCESS_FLAG_SW2 == rx[*rxLen-1]))
                    {
                        /* successful select */
                        *rxLen = *rxLen - PTX_T4T_STATUS_WORD_LENGTH;
                    } else
                    {
                        status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_NscRfError);
                        if ((PTX_T4T_SW1_ERROR_WRONG_P1P2 == rx[*rxLen-2]) && (PTX_T4T_SW2_ERROR_FILE_OR_APP_NOT_FOUND == rx[*rxLen-1]))
                        {
                            status = PTX_STATUS(ptxStatus_Comp_T4TOP,ptxStatus_NotFound);
                        }
                    }
                } else
                {
                    status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_NscRfError);
                }
            } else
            {
                status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_NscRfError);
            }
        } else
        {
            status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_NscRfError);
        }
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

static ptxStatus_t ptxNDEF_T4TOpReadBinary (ptxNDEF_T4TOP_t *t4tOpComp,
                                            uint32_t offset,
                                            uint8_t nbrExpectedResponseBytes,
                                            uint8_t *rx,
                                            uint32_t *rxLen,
                                            uint32_t msTimeout)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(t4tOpComp, ptxStatus_Comp_T4TOP) && (NULL != rx) && (NULL != rxLen))
    {
        *rxLen = (uint32_t)t4tOpComp->RxBufferSize;

        switch (t4tOpComp->CCParams.MappingVersion)
        {
            case 0x20:
                status = ptxNativeTag_T4TReadBinary(&t4tOpComp->NativeTagT4T,(uint16_t)offset,nbrExpectedResponseBytes,rx,rxLen,msTimeout);
                break;
            case 0x30:
                status = ptxNativeTag_T4TReadBinaryODO(&t4tOpComp->NativeTagT4T,offset,nbrExpectedResponseBytes,rx,rxLen,msTimeout);
                break;

            default:
                status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_InvalidParameter);
                break;
        }

        if (ptxStatus_Success == status)
        {
            if (0 != *rxLen)
            {
                if (PTX_T4T_STATUS_WORD_LENGTH <= *rxLen)
                {
                    if ((PTX_T4T_SUCCESS_FLAG_SW1 == rx[*rxLen-2]) && (PTX_T4T_SUCCESS_FLAG_SW2 == rx[*rxLen-1]))
                    {
                        /* successful read */
                        *rxLen = *rxLen - PTX_T4T_STATUS_WORD_LENGTH;
                    } else
                    {
                        status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_NscRfError);
                        if ((PTX_T4T_READ_FAIL_FLAG_SW1_SIMPLE == rx[*rxLen-2]) && (PTX_T4T_READ_FAIL_FLAG_SW2_SIMPLE == rx[*rxLen-1]))
                        {
                            status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_InvalidParameter); /* wrong length, no further information */
                        }

                        if (PTX_T4T_READ_FAIL_FLAG_SW1_EXT == rx[*rxLen-2])
                        {
                            status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_InvalidParameter); /* wrong length, t4tOpComp->RxBuffer[rxLen-1] has nbr of available data bytes */
                        }
                    }
                } else
                {
                    status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_NscRfError);
                }
            }
        } else
        {
            status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_NscRfError);
        }
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

static ptxStatus_t ptxNDEF_T4TOpUpdateBinary (ptxNDEF_T4TOP_t *t4tOpComp,
                                              uint32_t offset,
                                              uint8_t *dataField,
                                              uint8_t nbrDatabytes,
                                              uint8_t *rx,
                                              uint32_t *rxLen,
                                              uint32_t msTimeout)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(t4tOpComp, ptxStatus_Comp_T4TOP) && (NULL != dataField) && (NULL != rx) && (NULL != rxLen))
    {
        *rxLen = (uint32_t)t4tOpComp->RxBufferSize;

        switch (t4tOpComp->CCParams.MappingVersion)
        {
            case 0x20:
                status = ptxNativeTag_T4TUpdateBinary(&t4tOpComp->NativeTagT4T,(uint16_t)offset,dataField,nbrDatabytes,rx,rxLen,msTimeout);
                break;
            case 0x30:
                status = ptxNativeTag_T4TUpdateBinaryODO(&t4tOpComp->NativeTagT4T,offset,dataField,nbrDatabytes,rx,rxLen,msTimeout);
                break;

            default:
                status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_InvalidParameter);
                break;
        }

        if (ptxStatus_Success == status)
        {
            if (0 != *rxLen)
            {
                if (PTX_T4T_STATUS_WORD_LENGTH <= *rxLen)
                {
                    if ((PTX_T4T_SUCCESS_FLAG_SW1 == rx[*rxLen-2]) && (PTX_T4T_SUCCESS_FLAG_SW2 == rx[*rxLen-1]))
                    {
                        /* successful update */
                        *rxLen = *rxLen - PTX_T4T_STATUS_WORD_LENGTH;
                    } else
                    {
                        status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_NscRfError);
                    }
                } else
                {
                    status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_NscRfError);
                }
            } else
            {
                status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_NscRfError);
            }
        } else
        {
            status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_NscRfError);
        }
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

static ptxStatus_t ptxNDEF_T4TOpGetCapabilityContainerInfo (ptxNDEF_T4TOP_t *t4tOpComp)
{
    ptxStatus_t status = ptxStatus_Success;
    uint32_t rx_len;

    uint8_t cc_le_field = 0x02;
    uint8_t cc_index = 0;
    uint8_t data_index = 0;
    uint8_t data_field[2];

    uint16_t DATA_FIELD_CC_SELECT = 0xE103;

    if (PTX_COMP_CHECK(t4tOpComp, ptxStatus_Comp_T4TOP))
    {
        data_field[0] = (uint8_t)(DATA_FIELD_CC_SELECT >> 8u);
        data_field[1] = (uint8_t)(DATA_FIELD_CC_SELECT & 0xFF);

        /* select CC file (function is to be called AFTER NDEF application select) */
        t4tOpComp->NativeTagT4T.Fields.nbr_data_length_bytes = 1u;
        t4tOpComp->NativeTagT4T.Fields.nbr_expected_length_bytes = 0;
        status = ptxNDEF_T4TOpSelect(t4tOpComp,P1_CC_SELECT,P2_CC_SELECT,
                                    data_field,LC_FIELD_CC_SELECT,LE_FIELD_CC_SELECT,
                                    &t4tOpComp->RxBuffer[0],&rx_len,PTX_T4T_DEFAULT_TIMEOUT_MS);

        if (ptxStatus_Success == status)
        {
            /* read CCLen bytes of CC file */
            t4tOpComp->CCParams.MappingVersion = 0x20; /* preset for CC collection */

            status = ptxNDEF_T4TOpReadBinary(t4tOpComp,(uint32_t)cc_index,cc_le_field,
                                            &t4tOpComp->RxBuffer[0],&rx_len,PTX_T4T_DEFAULT_TIMEOUT_MS);
            cc_index = (uint8_t)(cc_index + cc_le_field);

            if (ptxStatus_Success == status)
            {
                (void)memcpy(&t4tOpComp->CCParams.CCLen[0],&t4tOpComp->RxBuffer[0],(uint32_t)2);

                /* CCLen <= 0xFF, read in one go (proprietary ignored) */
                cc_le_field = t4tOpComp->CCParams.CCLen[1];

                /* read rest of the CC file */
                status = ptxNDEF_T4TOpReadBinary(t4tOpComp,(uint32_t)cc_index,(uint8_t)(cc_le_field-cc_index),
                                                &t4tOpComp->RxBuffer[0],&rx_len,PTX_T4T_DEFAULT_TIMEOUT_MS);


                if (ptxStatus_Success == status)
                {
                    t4tOpComp->CCParams.MappingVersion = t4tOpComp->RxBuffer[data_index];
                    data_index++;
                    t4tOpComp->CCParams.MLeDigit = (uint16_t)(((uint16_t)t4tOpComp->RxBuffer[data_index] << 8) | t4tOpComp->RxBuffer[data_index+1]);
                    data_index = (uint8_t)(data_index + 2);
                    if (PTX_T4T_MAX_BYTES_RECEIVE < t4tOpComp->CCParams.MLeDigit)
                    {
                        t4tOpComp->CCParams.MLeDigit = PTX_T4T_MAX_BYTES_RECEIVE;
                    }
                    t4tOpComp->CCParams.MLcDigit = (uint16_t)(((uint16_t)t4tOpComp->RxBuffer[data_index] << 8) | t4tOpComp->RxBuffer[data_index+1]);
                    data_index = (uint8_t)(data_index + 2);
                    if (PTX_T4T_MAX_BYTES_SEND < t4tOpComp->CCParams.MLcDigit)
                    {
                        t4tOpComp->CCParams.MLcDigit = PTX_T4T_MAX_BYTES_SEND;
                    }
                    if ((t4tOpComp->CCParams.MLeDigit == 0) || (t4tOpComp->CCParams.MLcDigit == 0))
                    {
                        status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_InvalidParameter);
                    }

                    switch (t4tOpComp->CCParams.MappingVersion)
                    {
                        case 0x20:
                            /* NDEF control TLV */
                            (void)memcpy(&t4tOpComp->CCParams.NDEFTLV[0],&t4tOpComp->RxBuffer[data_index],(uint8_t)PTX_T4T_NDEF_LENGTH);
                            data_index = (uint8_t)(data_index + PTX_T4T_NDEF_LENGTH);
                            (void)memcpy(&t4tOpComp->CCParams.NDEFFileIdentifier[0],&t4tOpComp->CCParams.NDEFTLV[2],(uint32_t)2);
                            t4tOpComp->CCParams.NDEFFileSize = (uint16_t)(((uint16_t)t4tOpComp->CCParams.NDEFTLV[4] << 8u) | ((uint16_t)t4tOpComp->CCParams.NDEFTLV[5]));
                            t4tOpComp->CCParams.NDEFAccessRead = t4tOpComp->CCParams.NDEFTLV[6];
                            t4tOpComp->CCParams.NDEFAccessWrite = t4tOpComp->CCParams.NDEFTLV[7];
                            break;
                        case 0x30:
                            /* ENDEF control TLV */
                            (void)memcpy(&t4tOpComp->CCParams.NDEFTLV[0],&t4tOpComp->RxBuffer[data_index],(uint8_t)PTX_T4T_ENDEF_LENGTH);
                            data_index = (uint8_t)(data_index + PTX_T4T_NDEF_LENGTH);
                            (void)memcpy(&t4tOpComp->CCParams.NDEFFileIdentifier[0],&t4tOpComp->CCParams.NDEFTLV[2],(uint32_t)2);
                            t4tOpComp->CCParams.NDEFFileSize = (uint16_t)(((uint16_t)t4tOpComp->CCParams.NDEFTLV[4] << 24u) | ((uint16_t)t4tOpComp->CCParams.NDEFTLV[5] << 16u) | ((uint16_t)t4tOpComp->CCParams.NDEFTLV[6] << 8u) | ((uint16_t)t4tOpComp->CCParams.NDEFTLV[7]));
                            t4tOpComp->CCParams.NDEFAccessRead = t4tOpComp->CCParams.NDEFTLV[8];
                            t4tOpComp->CCParams.NDEFAccessWrite = t4tOpComp->CCParams.NDEFTLV[9];
                            break;
                        default:
                            status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_InvalidParameter);
                            break;
                    }
                }
            }
        }
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

static ptxStatus_t ptxNDEF_T4TOpUpdateLength (ptxNDEF_T4TOP_t *t4tOpComp, uint32_t newLen)
{
    ptxStatus_t status = ptxStatus_Success;
    uint8_t lc_field;
    uint8_t length_data_field[4];
    uint8_t nlen_offset = 0;
    uint32_t rx_len;

    if (PTX_COMP_CHECK(t4tOpComp, ptxStatus_Comp_T4TOP))
    {
        switch (t4tOpComp->CCParams.MappingVersion)
        {
            case 0x20:
                if (PTX_T4T_MAX_NLEN < newLen)
                {
                    status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_InvalidParameter);
                } else
                {
                    length_data_field[0] = (uint8_t)((uint16_t)newLen >> 8u);
                    length_data_field[1] = (uint8_t)((uint16_t)newLen & 0xFF);

                    lc_field = 2u;

                    status = ptxNDEF_T4TOpUpdateBinary(t4tOpComp,nlen_offset,length_data_field,lc_field,&t4tOpComp->RxBuffer[0],&rx_len,PTX_T4T_DEFAULT_TIMEOUT_MS);
                }
                break;
            case 0x30:
                if (PTX_T4T_MAX_ENLEN < newLen)
                {
                    status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_InvalidParameter);
                } else
                {
                    length_data_field[3] = (uint8_t)(newLen & 0x000000FF);
                    length_data_field[2] = (uint8_t)(newLen & 0x0000FF00) >> 8u;
                    length_data_field[1] = (uint8_t)(newLen & 0x00FF0000) >> 16u;
                    length_data_field[0] = (uint8_t)(newLen & 0xFF000000) >> 24u;

                    lc_field = 4;

                    status = ptxNDEF_T4TOpUpdateBinary(t4tOpComp,nlen_offset,length_data_field,lc_field,&t4tOpComp->RxBuffer[0],&rx_len,PTX_T4T_DEFAULT_TIMEOUT_MS);
                }
                break;

            default:
                status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_InvalidParameter);
                break;
        }
        if (ptxStatus_Success == status)
        {
            t4tOpComp->NLEN.DigitNLEN = newLen;
            t4tOpComp->NLEN.NbrNLENBytes = lc_field;
            (void)memcpy(&t4tOpComp->NLEN.NLEN[0], &length_data_field[0], (uint32_t)lc_field);

            if (newLen != 0)
            {
                t4tOpComp->LifeCycle = TagLC_ReadWrite;
            } else
            {
                t4tOpComp->LifeCycle = TagLC_Initialized;
            }
        }
    }
    else
    {
        status = PTX_STATUS(ptxStatus_Comp_T4TOP, ptxStatus_InvalidParameter);
    }

    return status;
}

