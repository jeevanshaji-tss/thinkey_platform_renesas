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
#include "ptxNativeTag_T5T.h"
#include "ptxPLAT.h"
#include <string.h>

/*
 * ####################################################################################################################
 * DEFINES / TYPES
 * ####################################################################################################################
 */
/*
 * \brief T5T Native Tag Component
 */
typedef enum ptxNativeTag_T5TFrameType
{
    T5T_FrameType_Standard,
    T5T_FrameType_Special,

} ptxNativeTag_T5TFrameType;

/*
 * \brief T5T General Defines
 */
#define PTX_T5T_READ_SINGLE_BLOCK_REQ_CODE              (uint8_t)0x20
#define PTX_T5T_WRITE_SINGLE_BLOCK_REQ_CODE             (uint8_t)0x21
#define PTX_T5T_LOCK_SINGLE_BLOCK_REQ_CODE              (uint8_t)0x22
#define PTX_T5T_READ_MULTIPLE_BLOCK_REQ_CODE            (uint8_t)0x23
#define PTX_T5T_EXT_READ_SINGLE_BLOCK_REQ_CODE          (uint8_t)0x30
#define PTX_T5T_EXT_WRITE_SINGLE_BLOCK_REQ_CODE         (uint8_t)0x31
#define PTX_T5T_EXT_LOCK_SINGLE_BLOCK_REQ_CODE          (uint8_t)0x32
#define PTX_T5T_EXT_READ_MULTIPLE_BLOCK_REQ_CODE        (uint8_t)0x33
#define PTX_T5T_SELECT_REQ_CODE                         (uint8_t)0x25
#define PTX_T5T_SLPV_REQ_CODE                           (uint8_t)0x02

/*
 * \brief T5T Internal Defines
 */
#define PTX_T5T_REQ_FLAG_OPTION_BIT_SET_MASK            (uint8_t)0x40
#define PTX_T5T_REQ_FLAG_AMS_SET_MASK                   (uint8_t)0x20
#define PTX_T5T_REQ_FLAG_HIGHT_DATA_RATE_SET_MASK       (uint8_t)0x02
#define PTX_T5T_REQ_FLAG_SELECT_FLAG_SET_MASK           (uint8_t)0x10

#define PTX_T5T_SELECTED_TRUE                            (uint8_t)0x01
#define PTX_T5T_SELECTED_FALSE                           (uint8_t)0x00

#define PTX_T5T_REQ_FLAG_TEMPLATE_MASK                  PTX_T5T_REQ_FLAG_HIGHT_DATA_RATE_SET_MASK

#define PTX_T5T_FDT_V_EOF                               (uint32_t)20

/*
 * ####################################################################################################################
 * INTERNAL FUNCTIONS / HELPERS
 * ####################################################################################################################
 */
static ptxStatus_t ptxNativeTag_T5TSetCommandHeader(ptxNativeTag_T5T_t *t5tComp, uint8_t commandCode, uint8_t optionFlag, uint32_t *bytesWritten);
static ptxStatus_t ptxNativeTag_T5TTransceive(ptxNativeTag_T5T_t *t5tComp, ptxNativeTag_T5TFrameType frameType, uint32_t txLen, uint8_t *rx, uint32_t *rxLen, uint32_t msTimeout);
static ptxStatus_t ptxNativeTag_T5THandleRfReset (ptxNativeTag_T5T_t *t5tComp);

/*
 * ####################################################################################################################
 * API FUNCTIONS
 * ####################################################################################################################
 */
ptxStatus_t ptxNativeTag_T5TOpen (ptxNativeTag_T5T_t *t5tComp, ptxNativeTag_T5T_InitParams_t *initParams)
{
    ptxStatus_t status = ptxStatus_Success;

    if ((NULL != t5tComp) && (NULL != initParams))
    {
        if ((NULL != initParams->IotRd) && (NULL != initParams->TxBuffer) && (PTX_T5T_MIN_TX_BUFFER_SIZE <= initParams->TxBufferSize))
        {
            /* clear component */
            (void)memset(t5tComp, 0, sizeof(ptxNativeTag_T5T_t));

            /* set members */
            t5tComp->IotRd = initParams->IotRd;
            t5tComp->TxBuffer = initParams->TxBuffer;

            if (0 != initParams->UIDLen)
            {
                if ((PTX_T5T_UID_SIZE == initParams->UIDLen) && (NULL != initParams->UID))
                {
                    t5tComp->UID = initParams->UID;
                }
                else
                {
                    status = PTX_STATUS(ptxStatus_Comp_NativeTag_T5T, ptxStatus_InvalidParameter);
                }
            } else
            {
                t5tComp->UID = NULL;
            }

            /* set Component-ID at the end to prevent futher calls in case of an error */
            if (ptxStatus_Success == status)
            {
                t5tComp->CompId = ptxStatus_Comp_NativeTag_T5T;
            }

        } else
        {
            status = PTX_STATUS(ptxStatus_Comp_NativeTag_T5T, ptxStatus_InvalidParameter);
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NativeTag_T5T, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNativeTag_T5TReadSingleBlock (ptxNativeTag_T5T_t *t5tComp,
                                             uint8_t optionFlag,
                                             uint8_t blockNr,
                                             uint8_t *rx,
                                             uint32_t *rxLen,
                                             uint32_t msTimeout)
{
    ptxStatus_t status = ptxStatus_Success;
    uint32_t tx_index = 0;

    if (PTX_COMP_CHECK(t5tComp, ptxStatus_Comp_NativeTag_T5T) && (NULL != rx) && (NULL != rxLen))
    {
        /* set T5T command-header */
        status = ptxNativeTag_T5TSetCommandHeader(t5tComp, PTX_T5T_READ_SINGLE_BLOCK_REQ_CODE, optionFlag, &tx_index);

        /* set command specific parameters */
        if (ptxStatus_Success == status)
        {
            t5tComp->TxBuffer[tx_index] = blockNr;
            tx_index++;
        }

        /* everything OK so far ? - send data */
        if (ptxStatus_Success == status)
        {
            status = ptxNativeTag_T5TTransceive(t5tComp, T5T_FrameType_Standard, tx_index, rx, rxLen, msTimeout);
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NativeTag_T5T, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNativeTag_T5TWriteSingleBlock (ptxNativeTag_T5T_t *t5tComp,
                                              uint8_t optionFlag,
                                              uint8_t blockNr,
                                              uint8_t *blockData,
                                              uint8_t blockDataLen,
                                              uint8_t *rx,
                                              uint32_t *rxLen,
                                              uint32_t msTimeout)
{
    ptxStatus_t status = ptxStatus_Success;
    uint32_t tx_index = 0;
    ptxNativeTag_T5TFrameType frame_type = (0 != optionFlag) ? T5T_FrameType_Special : T5T_FrameType_Standard;

    if (PTX_COMP_CHECK(t5tComp, ptxStatus_Comp_NativeTag_T5T) && (NULL != rx) && (NULL != rxLen) && (NULL != blockData) && (PTX_T5T_MIN_TX_BUFFER_SIZE >= blockDataLen))
    {
        /* set T5T command-header */
        status = ptxNativeTag_T5TSetCommandHeader(t5tComp, PTX_T5T_WRITE_SINGLE_BLOCK_REQ_CODE, optionFlag, &tx_index);

        /* set command specific parameters */
        if (ptxStatus_Success == status)
        {
            t5tComp->TxBuffer[tx_index] = blockNr;
            tx_index++;

            (void)memcpy(&t5tComp->TxBuffer[tx_index], blockData, blockDataLen);
            tx_index = (uint32_t)(tx_index + blockDataLen);
        }

        /* everything OK so far ? - send data */
        if (ptxStatus_Success == status)
        {
            status = ptxNativeTag_T5TTransceive(t5tComp, frame_type, tx_index, rx, rxLen, msTimeout);
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NativeTag_T5T, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNativeTag_T5TLockSingleBlock (ptxNativeTag_T5T_t *t5tComp,
                                             uint8_t optionFlag,
                                             uint8_t blockNr,
                                             uint8_t *rx,
                                             uint32_t *rxLen,
                                             uint32_t msTimeout)
{
    ptxStatus_t status = ptxStatus_Success;
    uint32_t tx_index = 0;
    ptxNativeTag_T5TFrameType frame_type = (0 != optionFlag) ? T5T_FrameType_Special : T5T_FrameType_Standard;

    if (PTX_COMP_CHECK(t5tComp, ptxStatus_Comp_NativeTag_T5T) && (NULL != rx) && (NULL != rxLen))
    {
        /* set T5T command-header */
        status = ptxNativeTag_T5TSetCommandHeader(t5tComp, PTX_T5T_LOCK_SINGLE_BLOCK_REQ_CODE, optionFlag, &tx_index);

        /* set command specific parameters */
        if (ptxStatus_Success == status)
        {
            t5tComp->TxBuffer[tx_index] = blockNr;
            tx_index++;
        }

        /* everything OK so far ? - send data */
        if (ptxStatus_Success == status)
        {
            status = ptxNativeTag_T5TTransceive(t5tComp, frame_type, tx_index, rx, rxLen, msTimeout);
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NativeTag_T5T, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNativeTag_T5TReadMultipleBlock (ptxNativeTag_T5T_t *t5tComp,
                                               uint8_t optionFlag,
                                               uint8_t blockNr,
                                               uint8_t nrBlocks,
                                               uint8_t *rx,
                                               uint32_t *rxLen,
                                               uint32_t msTimeout)
{
    ptxStatus_t status = ptxStatus_Success;
    uint32_t tx_index = 0;

    if (PTX_COMP_CHECK(t5tComp, ptxStatus_Comp_NativeTag_T5T) && (NULL != rx) && (NULL != rxLen))
    {
        /* set T5T command-header */
        status = ptxNativeTag_T5TSetCommandHeader(t5tComp, PTX_T5T_READ_MULTIPLE_BLOCK_REQ_CODE, optionFlag, &tx_index);

        /* set command specific parameters */
        if (ptxStatus_Success == status)
        {
            t5tComp->TxBuffer[tx_index] = blockNr;
            tx_index++;

            t5tComp->TxBuffer[tx_index] = nrBlocks;
            tx_index++;
        }

        /* everything OK so far ? - send data */
        if (ptxStatus_Success == status)
        {
            status = ptxNativeTag_T5TTransceive(t5tComp, T5T_FrameType_Standard, tx_index, rx, rxLen, msTimeout);
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NativeTag_T5T, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNativeTag_T5TExtReadSingleBlock (ptxNativeTag_T5T_t *t5tComp,
                                                uint8_t optionFlag,
                                                uint16_t blockNr,
                                                uint8_t *rx,
                                                uint32_t *rxLen,
                                                uint32_t msTimeout)
{
    ptxStatus_t status = ptxStatus_Success;
    uint32_t tx_index = 0;

    if (PTX_COMP_CHECK(t5tComp, ptxStatus_Comp_NativeTag_T5T) && (NULL != rx) && (NULL != rxLen))
    {
        /* set T5T command-header */
        status = ptxNativeTag_T5TSetCommandHeader(t5tComp, PTX_T5T_EXT_READ_SINGLE_BLOCK_REQ_CODE, optionFlag, &tx_index);

        /* set command specific parameters */
        if (ptxStatus_Success == status)
        {
            t5tComp->TxBuffer[tx_index] = (uint8_t)((blockNr & 0xFF00) >> 8);
            tx_index++;

            t5tComp->TxBuffer[tx_index] = (uint8_t)((blockNr & 0x00FF) >> 0);
            tx_index++;
        }

        /* everything OK so far ? - send data */
        if (ptxStatus_Success == status)
        {
            status = ptxNativeTag_T5TTransceive(t5tComp, T5T_FrameType_Standard, tx_index, rx, rxLen, msTimeout);
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NativeTag_T5T, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNativeTag_T5TExtWriteSingleBlock (ptxNativeTag_T5T_t *t5tComp,
                                                 uint8_t optionFlag,
                                                 uint16_t blockNr,
                                                 uint8_t *blockData,
                                                 uint8_t blockDataLen,
                                                 uint8_t *rx,
                                                 uint32_t *rxLen,
                                                 uint32_t msTimeout)
{
    ptxStatus_t status = ptxStatus_Success;
    uint32_t tx_index = 0;
    ptxNativeTag_T5TFrameType frame_type = (0 != optionFlag) ? T5T_FrameType_Special : T5T_FrameType_Standard;

    if (PTX_COMP_CHECK(t5tComp, ptxStatus_Comp_NativeTag_T5T) && (NULL != rx) && (NULL != rxLen) && (NULL != blockData) && (PTX_T5T_MIN_TX_BUFFER_SIZE >= blockDataLen))
    {
        /* set T5T command-header */
        status = ptxNativeTag_T5TSetCommandHeader(t5tComp, PTX_T5T_EXT_WRITE_SINGLE_BLOCK_REQ_CODE, optionFlag, &tx_index);

        /* set command specific parameters */
        if (ptxStatus_Success == status)
        {
            t5tComp->TxBuffer[tx_index] = (uint8_t)((blockNr & 0xFF00) >> 8);
            tx_index++;

            t5tComp->TxBuffer[tx_index] = (uint8_t)((blockNr & 0x00FF) >> 0);
            tx_index++;

            (void)memcpy(&t5tComp->TxBuffer[tx_index], blockData, blockDataLen);
            tx_index = (uint32_t)(tx_index + blockDataLen);
        }

        /* everything OK so far ? - send data */
        if (ptxStatus_Success == status)
        {
            status = ptxNativeTag_T5TTransceive(t5tComp, frame_type, tx_index, rx, rxLen, msTimeout);
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NativeTag_T5T, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNativeTag_T5TExtLockSingleBlock (ptxNativeTag_T5T_t *t5tComp,
                                                uint8_t optionFlag,
                                                uint16_t blockNr,
                                                uint8_t *rx,
                                                uint32_t *rxLen,
                                                uint32_t msTimeout)
{
    ptxStatus_t status = ptxStatus_Success;
    uint32_t tx_index = 0;
    ptxNativeTag_T5TFrameType frame_type = (0 != optionFlag) ? T5T_FrameType_Special : T5T_FrameType_Standard;

    if (PTX_COMP_CHECK(t5tComp, ptxStatus_Comp_NativeTag_T5T) && (NULL != rx) && (NULL != rxLen))
    {
        /* set T5T command-header */
        status = ptxNativeTag_T5TSetCommandHeader(t5tComp, PTX_T5T_EXT_LOCK_SINGLE_BLOCK_REQ_CODE, optionFlag, &tx_index);

        /* set command specific parameters */
        if (ptxStatus_Success == status)
        {
            t5tComp->TxBuffer[tx_index] = (uint8_t)((blockNr & 0xFF00) >> 8);
            tx_index++;

            t5tComp->TxBuffer[tx_index] = (uint8_t)((blockNr & 0x00FF) >> 0);
            tx_index++;
        }

        /* everything OK so far ? - send data */
        if (ptxStatus_Success == status)
        {
            status = ptxNativeTag_T5TTransceive(t5tComp, frame_type, tx_index, rx, rxLen, msTimeout);
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NativeTag_T5T, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNativeTag_T5TExtReadMultipleBlock (ptxNativeTag_T5T_t *t5tComp,
                                                  uint8_t optionFlag,
                                                  uint16_t blockNr,
                                                  uint16_t nrBlocks,
                                                  uint8_t *rx,
                                                  uint32_t *rxLen,
                                                  uint32_t msTimeout)
{
    ptxStatus_t status = ptxStatus_Success;
    uint32_t tx_index = 0;

    if (PTX_COMP_CHECK(t5tComp, ptxStatus_Comp_NativeTag_T5T) && (NULL != rx) && (NULL != rxLen))
    {
        /* set T5T command-header */
        status = ptxNativeTag_T5TSetCommandHeader(t5tComp, PTX_T5T_EXT_READ_MULTIPLE_BLOCK_REQ_CODE, optionFlag, &tx_index);

        /* set command specific parameters */
        if (ptxStatus_Success == status)
        {
            t5tComp->TxBuffer[tx_index] = (uint8_t)((blockNr & 0xFF00) >> 8);
            tx_index++;

            t5tComp->TxBuffer[tx_index] = (uint8_t)((blockNr & 0x00FF) >> 0);
            tx_index++;

            t5tComp->TxBuffer[tx_index] = (uint8_t)((nrBlocks & 0xFF00) >> 8);
            tx_index++;

            t5tComp->TxBuffer[tx_index] = (uint8_t)((nrBlocks & 0x00FF) >> 0);
            tx_index++;
        }

        /* everything OK so far ? - send data */
        if (ptxStatus_Success == status)
        {
            status = ptxNativeTag_T5TTransceive(t5tComp, T5T_FrameType_Standard, tx_index, rx, rxLen, msTimeout);
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NativeTag_T5T, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNativeTag_T5TSelect (ptxNativeTag_T5T_t *t5tComp,
                                    uint8_t optionFlag,
                                    uint8_t *uid,
                                    uint8_t uidLen,
                                    uint8_t *rx,
                                    uint32_t *rxLen,
                                    uint32_t msTimeout)
{
    ptxStatus_t status = ptxStatus_Success;
    uint32_t tx_index = 0;

    if (PTX_COMP_CHECK(t5tComp, ptxStatus_Comp_NativeTag_T5T) && (NULL != rx) && (NULL != rxLen)  && (NULL != uid) && (PTX_T5T_UID_SIZE == uidLen))
    {
        /* set T5T command-header */
        status = ptxNativeTag_T5TSetCommandHeader(t5tComp, PTX_T5T_SELECT_REQ_CODE, optionFlag, &tx_index);

        /* set command specific parameters */
        if (ptxStatus_Success == status)
        {
            (void)memcpy(&t5tComp->TxBuffer[tx_index], uid, uidLen);
            tx_index = (uint32_t)(tx_index + uidLen);
        }

        /* everything OK so far ? - send data */
        if (ptxStatus_Success == status)
        {
            status = ptxNativeTag_T5TTransceive(t5tComp, T5T_FrameType_Standard, tx_index, rx, rxLen, msTimeout);
        }

        /* check RES_FLAG */
        if (0x00 == rx[0])
        {
            t5tComp->isSelected = PTX_T5T_SELECTED_TRUE;
        } else
        {
            t5tComp->isSelected = PTX_T5T_SELECTED_FALSE;
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NativeTag_T5T, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNativeTag_T5TSleep (ptxNativeTag_T5T_t *t5tComp,
                                   uint8_t optionFlag,
                                   uint8_t *uid,
                                   uint8_t uidLen,
                                   uint8_t *rx,
                                   uint32_t *rxLen,
                                   uint32_t msTimeout)
{
    ptxStatus_t status = ptxStatus_Success;
    uint32_t tx_index = 0;

    if (PTX_COMP_CHECK(t5tComp, ptxStatus_Comp_NativeTag_T5T) && (NULL != rx) && (NULL != rxLen) && (NULL != uid) && (PTX_T5T_UID_SIZE == uidLen))
    {
        /* set T5T command-header */
        status = ptxNativeTag_T5TSetCommandHeader(t5tComp, PTX_T5T_SLPV_REQ_CODE, optionFlag, &tx_index);

        /* set command specific parameters */
        if (ptxStatus_Success == status)
        {
            (void)memcpy(&t5tComp->TxBuffer[tx_index], uid, uidLen);
            tx_index = (uint32_t)(tx_index + uidLen);
        }

        /* everything OK so far ? - send data */
        if (ptxStatus_Success == status)
        {
            status = ptxNativeTag_T5TTransceive(t5tComp, T5T_FrameType_Standard, tx_index, rx, rxLen, msTimeout);

            /* Attention: A successful SLEEP-command doesn't send a response - therefore overwrite the status for the caller */
            if (((ptxStatus_TimeOut == PTX_GET_STATUS(status)) && (0 == *rxLen)))
            {
                status = ptxStatus_Success;
            }
        }

        if ((ptxStatus_Success == status) && (0 == memcmp(&t5tComp->UID[0], &uid[0], PTX_T5T_UID_SIZE)))
        {
            t5tComp->isSelected = PTX_T5T_SELECTED_FALSE;
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NativeTag_T5T, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNativeTag_T5TSetUID (ptxNativeTag_T5T_t *t5tComp,
                                    uint8_t *uid,
                                    uint8_t uidLen)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(t5tComp, ptxStatus_Comp_NativeTag_T5T))
    {
        status = ptxNativeTag_T5THandleRfReset(t5tComp);
        if (ptxStatus_Success == status)
        {
            if (0 != uidLen)
            {
                /* check if new UID is being set if in SELECTED mode, if yes exit SELECTED mode */
                if ((NULL != t5tComp->UID) && (PTX_T5T_SELECTED_TRUE == t5tComp->isSelected))
                {
                    memcmp(&t5tComp->UID, uid, PTX_T5T_UID_SIZE) ? (t5tComp->isSelected = PTX_T5T_SELECTED_FALSE) : (t5tComp->isSelected = PTX_T5T_SELECTED_TRUE);
                }

	            if ((PTX_T5T_UID_SIZE == uidLen) && (NULL != uid))
	            {
	                t5tComp->UID = uid;
	            }
	            else
	            {
	                status = PTX_STATUS(ptxStatus_Comp_NativeTag_T5T, ptxStatus_InvalidParameter);
	            }
	        } else
	        {
	            t5tComp->UID = NULL;
	        }
		}
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NativeTag_T5T, ptxStatus_InvalidParameter);
    }

    return status;

}

ptxStatus_t ptxNativeTag_T5TClose (ptxNativeTag_T5T_t *t5tComp)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(t5tComp, ptxStatus_Comp_NativeTag_T5T))
    {
        /* nothing to do here */

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NativeTag_T5T, ptxStatus_InvalidParameter);
    }

    return status;
}

/*
 * ####################################################################################################################
 * INTERNAL FUNCTIONS / CALLBACK(s)
 * ####################################################################################################################
 */
static ptxStatus_t ptxNativeTag_T5TSetCommandHeader(ptxNativeTag_T5T_t *t5tComp, uint8_t commandCode, uint8_t optionFlag, uint32_t *bytesWritten)
{
    ptxStatus_t status = ptxStatus_Success;
    uint8_t req_flag = PTX_T5T_REQ_FLAG_TEMPLATE_MASK;
    uint32_t index = 0;

    if (PTX_COMP_CHECK(t5tComp, ptxStatus_Comp_NativeTag_T5T) && (NULL != bytesWritten))
    {
        status = ptxNativeTag_T5THandleRfReset(t5tComp);
        if (ptxStatus_Success == status)
        {
            /* set Option-Flag ? */
            if (0 != optionFlag)
            {
                req_flag = (uint8_t)(req_flag | PTX_T5T_REQ_FLAG_OPTION_BIT_SET_MASK);
            }

            /* use Adressed-Mode ? */
            if ((NULL != t5tComp->UID) || ((PTX_T5T_SELECT_REQ_CODE == commandCode) || (PTX_T5T_SLPV_REQ_CODE == commandCode)))
            {
                req_flag = (uint8_t)(req_flag | PTX_T5T_REQ_FLAG_AMS_SET_MASK);
            }

            /* SELECTED mode? */
            if ((PTX_T5T_SELECTED_FALSE != t5tComp->isSelected) && (PTX_T5T_SELECT_REQ_CODE != commandCode))
            {
                req_flag = (uint8_t)(req_flag | PTX_T5T_REQ_FLAG_SELECT_FLAG_SET_MASK);
                req_flag = (uint8_t)(req_flag & ~PTX_T5T_REQ_FLAG_AMS_SET_MASK);
            }

            /* set REQ_FLAG-byte */
            t5tComp->TxBuffer[index] = req_flag;
            index++;

            /* set command-code */
            t5tComp->TxBuffer[index] = commandCode;
            index++;

            switch (commandCode)
            {
                case PTX_T5T_SELECT_REQ_CODE:
                case PTX_T5T_SLPV_REQ_CODE:
                    /* don't insert UID here - done by caller because the UID is treated as payload in this case */
                    break;

                default:
                    /* set UID (optional) */
                    if ((0 != (req_flag & PTX_T5T_REQ_FLAG_AMS_SET_MASK)) && (NULL != t5tComp->UID))
                    {
                        (void)memcpy(&t5tComp->TxBuffer[index], &t5tComp->UID[0], PTX_T5T_UID_SIZE);
                        index = (uint32_t)(index + PTX_T5T_UID_SIZE);
                    }
                    break;
            }
            *bytesWritten = index;
        }
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NativeTag_T5T, ptxStatus_InvalidParameter);
    }

    return status;
}

static ptxStatus_t ptxNativeTag_T5TTransceive(ptxNativeTag_T5T_t *t5tComp, ptxNativeTag_T5TFrameType frameType, uint32_t txLen, uint8_t *rx, uint32_t *rxLen, uint32_t msTimeout)
{
    ptxStatus_t status = ptxStatus_Success;
    uint32_t timeout_value;
    uint32_t rx_len;

    /* Aside from actual Tag-component, remaining parameters are checked within Data-Exchange function */
    if ((PTX_COMP_CHECK(t5tComp, ptxStatus_Comp_NativeTag_T5T)) && (NULL != rxLen))
    {
        timeout_value = (T5T_FrameType_Special == frameType) ? 1 : msTimeout;

        /* send command */
        rx_len = *rxLen;
        status = ptxIoTRd_Data_Exchange (t5tComp->IotRd, &t5tComp->TxBuffer[0], txLen, rx, &rx_len, timeout_value);

        /* Special-Frame format is supposed to return a timeout-error after the first RF-exchange -> reset the status */
        if ((T5T_FrameType_Special == frameType) && (ptxStatus_TimeOut == PTX_GET_STATUS(status)))
        {
            status = ptxStatus_Success;
        }

        if (ptxStatus_Success == status)
        {
            /* special-frame format i.e. isolated EoF required ? */
            if (T5T_FrameType_Special == frameType)
            {
                (void)ptxPLAT_Sleep(t5tComp->IotRd->Plat, PTX_T5T_FDT_V_EOF);

                rx_len = *rxLen;
                status = ptxIoTRd_T5T_IsolatedEoF (t5tComp->IotRd, rx, &rx_len, msTimeout);
            }
        }

        /* handle reception part */
        if (ptxStatus_Success == status)
        {
            if (0 != rx_len)
            {
                /*
                 * Last byte of received data from chip indicates additional status / RF error flags.
                 * If everything was OK (value == 0), cut it from the received length, otherwise report an error to upper layer.
                 */
                if (0 == rx[rx_len - 1])
                {
                    *rxLen = (uint32_t)(rx_len - 1);

                } else
                {
                    *rxLen = 0;
                    status = PTX_STATUS(ptxStatus_Comp_NativeTag_T5T, ptxStatus_NscRfError);
                }
            } else
            {
                *rxLen = 0;
                status = PTX_STATUS(ptxStatus_Comp_NativeTag_T5T, ptxStatus_NscRfError);
            }
        } else
        {
            *rxLen = 0;
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NativeTag_T5T, ptxStatus_InvalidParameter);
    }

    return status;
}

static ptxStatus_t ptxNativeTag_T5THandleRfReset (ptxNativeTag_T5T_t *t5tComp)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(t5tComp, ptxStatus_Comp_NativeTag_T5T))
    {
        ptxIoTRd_t *iotRd_internal = t5tComp->IotRd;

        if (NULL != iotRd_internal)
        {
            if (0 != iotRd_internal->rfResetFlag)
            {
                t5tComp->isSelected = PTX_T5T_SELECTED_FALSE;
                iotRd_internal->rfResetFlag = 0;
            }
        }
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NativeTag_T5T, ptxStatus_InvalidParameter);
    }

    return status;
}

