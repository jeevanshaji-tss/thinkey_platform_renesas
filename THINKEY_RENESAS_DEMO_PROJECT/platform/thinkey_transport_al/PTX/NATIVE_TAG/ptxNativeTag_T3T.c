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
#include "ptxNativeTag_T3T.h"
#include <string.h>

/*
 * ####################################################################################################################
 * DEFINES / TYPES
 * ####################################################################################################################
 */
#define PTX_T3T_SENSF_REQ_CODE                          (uint8_t)0x00
#define PTX_T3T_SENSF_REQ_SC_NDEF                       (uint16_t)0x12FC
#define PTX_T3T_CHECK_CODE                              (uint8_t)0x06
#define PTX_T3T_UPDATE_CODE                             (uint8_t)0x08
#define PTX_T3T_NFCID2_LEN                              (uint8_t)0x08
#define PTX_T3T_NDEF_BLOCKS_PER_OP                      (uint8_t)0x01

/*
 * ####################################################################################################################
 * INTERNAL FUNCTIONS / HELPERS
 * ####################################################################################################################
 */
static ptxStatus_t ptxNativeTag_T3TTransceive (ptxNativeTag_T3T_t *t3tComp, uint32_t txLen, uint8_t *rx, uint32_t *rxLen, uint32_t msTimeout);
static ptxStatus_t ptxNativeTag_T3TSetCommandHeader (ptxNativeTag_T3T_t *t3tComp, uint8_t commandCode, uint8_t *NFCID2, uint8_t NoS, uint8_t *serviceCodeList, uint8_t serviceCodeListLen, uint8_t NoB, uint8_t *blockList, uint8_t blockListLen, uint8_t *bytesWritten);

/*
 * ####################################################################################################################
 * API FUNCTIONS
 * ####################################################################################################################
 */
ptxStatus_t ptxNativeTag_T3TOpen (ptxNativeTag_T3T_t *t3tComp, ptxNativeTag_T3T_InitParams_t *initParams)
{
    ptxStatus_t status = ptxStatus_Success;

    if ((NULL != t3tComp) && (NULL != initParams))
    {
        if ((NULL != initParams->IotRd) && (NULL != initParams->TxBuffer) && (PTX_T3T_MIN_TX_BUFFER_SIZE <= initParams->TxBufferSize))
        {
            /* clear component */
            (void)memset(t3tComp, 0, sizeof(ptxNativeTag_T3T_t));

            /* set members */
            t3tComp->CompId = ptxStatus_Comp_NativeTag_T3T;
            t3tComp->IotRd = initParams->IotRd;
            t3tComp->TxBuffer = initParams->TxBuffer;
            if (0 != initParams->NFCID2Len)
            {
                if ((PTX_T3T_NFCID2_LEN == initParams->NFCID2Len) && (NULL != initParams->NFCID2))
                {
                    t3tComp->NFCID2 = initParams->NFCID2;
                }
                else
                {
                    /* ignore, set NFCID2 later */
                }
            } else
            {
                t3tComp->NFCID2 = NULL;
            }

        } else
        {
            status = PTX_STATUS(ptxStatus_Comp_NativeTag_T3T, ptxStatus_InvalidParameter);
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NativeTag_T3T, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNativeTag_T3TSetTagParams (ptxNativeTag_T3T_t *t3tComp,
                                                          uint8_t *NFCID2,
                                                          uint8_t NFCID2Len,
                                                          uint8_t MRTICheck,
                                                          uint8_t MRTIUpdate)
{
    ptxStatus_t status = ptxStatus_Success;
    uint8_t mask_a = 0x07;
    uint8_t mask_b = 0x38;
    uint8_t mask_e = 0xC0;
    uint8_t nbr_check_blocks = PTX_T3T_NDEF_BLOCKS_PER_OP;
    uint8_t nbr_update_blocks = PTX_T3T_NDEF_BLOCKS_PER_OP;
    uint32_t timeout_check;
    uint32_t timeout_update;
    uint16_t t_t3t = 1; /* assumed as 1 since it is usually below that */
    uint16_t last_comp_check = 1;
    uint16_t last_comp_update = 1;

    if (PTX_COMP_CHECK(t3tComp, ptxStatus_Comp_NativeTag_T3T))
    {
        /* set NFCID2 */
        if (0 != NFCID2Len)
        {
            if ((PTX_T3T_NFCID2_LEN == NFCID2Len) && (NULL != NFCID2))
            {
                t3tComp->NFCID2 = NFCID2;
                t3tComp->NFCID2Len = NFCID2Len;
            }
            else
            {
                status = PTX_STATUS(ptxStatus_Comp_NativeTag_T3T, ptxStatus_InvalidParameter);
            }
        } else
        {
            t3tComp->NFCID2 = NULL;
        }

        /* 4^n */
        for (uint8_t i = 0; i < ((MRTICheck & mask_e)>>6u); i++)
        {
            last_comp_check = (uint16_t)(last_comp_check * 4u);
        }
        for (uint8_t i = 0; i < ((MRTIUpdate & mask_e)>>6u); i++)
        {
            last_comp_update = (uint16_t)(last_comp_update * 4u);
        }

        /* set timeout value */
        timeout_check = t_t3t * ((((MRTICheck & mask_a)+1u) + nbr_check_blocks*(((MRTICheck & mask_b)>>3u)+1u)) * last_comp_check);
        timeout_update = t_t3t * ((((MRTIUpdate & mask_a)+1u) + nbr_update_blocks*(((MRTIUpdate & mask_b)>>3u)+1u)) * last_comp_update);

        timeout_check += 20;        /* added to avoid floating point */
        timeout_update += 20;

        t3tComp->TagTimeoutCheck = timeout_check;
        t3tComp->TagTimeoutUpdate = timeout_update;

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NativeTag_T3T, ptxStatus_InvalidParameter);
    }

    return status;

}

ptxStatus_t ptxNativeTag_T3TSENSF_REQ (ptxNativeTag_T3T_t *t3tComp,
                                                       uint16_t sc,
                                                       uint8_t rc,
                                                       uint8_t tsn,
                                                       uint8_t *rx,
                                                       uint32_t *rxLen,
                                                       uint32_t msTimeout)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(t3tComp, ptxStatus_Comp_NativeTag_T3T) && (0 != t3tComp->NFCID2Len) && (NULL != rx) && (NULL != rxLen))
    {
        status = ptxIoTRd_T3T_SENSFRequest(t3tComp->IotRd, sc, rc, tsn, rx, rxLen, msTimeout);

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NativeTag_T3T, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNativeTag_T3TCheck (ptxNativeTag_T3T_t *t3tComp,
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
    uint8_t tx_index = 0;

    if (PTX_COMP_CHECK(t3tComp, ptxStatus_Comp_NativeTag_T3T) && (0 != t3tComp->NFCID2Len) && (NULL != NFCID2) && (NULL != serviceCodeList) && (NULL != blockList) && (NULL != rx) && (NULL != rxLen))
    {
        status = ptxNativeTag_T3TSetCommandHeader(t3tComp,PTX_T3T_CHECK_CODE,NFCID2,NoS,serviceCodeList,serviceCodeListLen,NoB,blockList,blockListLen,&tx_index);

        if (ptxStatus_Success == status)
        {
            /* no need to add the block data field, send data */
            status = ptxNativeTag_T3TTransceive(t3tComp,tx_index,rx,rxLen,msTimeout);
        }
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NativeTag_T3T, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNativeTag_T3TUpdate (ptxNativeTag_T3T_t *t3tComp,
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
    uint8_t tx_index = 0;

    if (PTX_COMP_CHECK(t3tComp, ptxStatus_Comp_NativeTag_T3T) && (0 != t3tComp->NFCID2Len) && (NULL != NFCID2) && (NULL != serviceCodeList) && (NULL != blockList) && (NULL != rx) && (NULL != rxLen) && (NULL != blockData))
    {
        status = ptxNativeTag_T3TSetCommandHeader(t3tComp,PTX_T3T_UPDATE_CODE,NFCID2,NoS,serviceCodeList,serviceCodeListLen,NoB,blockList,blockListLen,&tx_index);

        if (ptxStatus_Success == status)
        {
            /* add the block data to the tx buffer */
            (void)memcpy(&t3tComp->TxBuffer[tx_index],&blockData[0],(uint32_t)blockDataLen);
            tx_index = (uint8_t)(tx_index + blockDataLen);

            /* send data */
            status = ptxNativeTag_T3TTransceive(t3tComp,tx_index,rx,rxLen,msTimeout);
        }
    }

    return status;
}

ptxStatus_t ptxNativeTag_T3TClose (ptxNativeTag_T3T_t *t3tComp)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(t3tComp, ptxStatus_Comp_NativeTag_T3T))
    {

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NativeTag_T3T, ptxStatus_InvalidParameter);
    }

    return status;
}

/*
 * ####################################################################################################################
 * INTERNAL FUNCTIONS / CALLBACK(s)
 * ####################################################################################################################
 */
static ptxStatus_t ptxNativeTag_T3TTransceive (ptxNativeTag_T3T_t *t3tComp,
                                        uint32_t txLen,
                                        uint8_t *rx,
                                        uint32_t *rxLen,
                                        uint32_t msTimeout)
{
    ptxStatus_t status = ptxStatus_Success;
    uint32_t timeout_value;
    uint32_t rx_len;

    if (PTX_COMP_CHECK(t3tComp, ptxStatus_Comp_NativeTag_T3T) && (0 != t3tComp->NFCID2Len) && (NULL != rx) && (NULL != rxLen))
    {
        timeout_value = msTimeout;
        rx_len = *rxLen;

        status = ptxIoTRd_Data_Exchange (t3tComp->IotRd, &t3tComp->TxBuffer[0], txLen, rx, &rx_len, timeout_value);

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
                    status = PTX_STATUS(ptxStatus_Comp_NativeTag_T3T, ptxStatus_NscRfError);
                }
            } else
            {
                status = PTX_STATUS(ptxStatus_Comp_NativeTag_T3T, ptxStatus_NscRfError);
            }
        }
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NativeTag_T3T, ptxStatus_InvalidParameter);
    }

    return status;
}

static ptxStatus_t ptxNativeTag_T3TSetCommandHeader (ptxNativeTag_T3T_t *t3tComp,
                                              uint8_t commandCode,
                                              uint8_t *NFCID2,
                                              uint8_t NoS,
                                              uint8_t *serviceCodeList,
                                              uint8_t serviceCodeListLen,
                                              uint8_t NoB,
                                              uint8_t *blockList,
                                              uint8_t blockListLen,
                                              uint8_t *bytesWritten)
{
    ptxStatus_t status = ptxStatus_Success;
    uint8_t tx_index = 0;

    if (PTX_COMP_CHECK(t3tComp, ptxStatus_Comp_NativeTag_T3T) && (0 != t3tComp->NFCID2Len) && (NULL != NFCID2) && (NULL != serviceCodeList) && (NULL != blockList))
    {
         /* length byte prepended later */
        t3tComp->TxBuffer[tx_index] = commandCode;
        tx_index++;
        (void)memcpy(&t3tComp->TxBuffer[tx_index],&NFCID2[0],(uint32_t)PTX_T3T_NFCID2_LEN);
        tx_index = (uint8_t)(tx_index + PTX_T3T_NFCID2_LEN);
        t3tComp->TxBuffer[tx_index] = NoS;
        tx_index++;
        (void)memcpy(&t3tComp->TxBuffer[tx_index],&serviceCodeList[0],(uint32_t)serviceCodeListLen);
        tx_index = (uint8_t)(tx_index + serviceCodeListLen);
        t3tComp->TxBuffer[tx_index] = NoB;
        tx_index++;
        (void)memcpy(&t3tComp->TxBuffer[tx_index],&blockList[0],(uint32_t)blockListLen);
        tx_index = (uint8_t)(tx_index + blockListLen);

        *bytesWritten = tx_index;

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NativeTag_T3T, ptxStatus_InvalidParameter);
    }

    return status;
}


