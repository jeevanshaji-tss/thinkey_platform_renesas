/** \file
   ---------------------------------------------------------------
   Copyright (C) 2021. Panthronics AG - All Rights Reserved.

   This material may not be reproduced, displayed, modified or
   distributed without the express prior written permission of the
   Panthronics AG.

   PLEASE REFER TO OUR DISCLAIMER NOTICE IN THE PROJECT ROOT.
   ---------------------------------------------------------------

    Project     : PTX100R
    Module      : Generic NDEF OPERATION API
    File        : ptxNDEF.c

    Description : Generic NDEF Operation API (IOT READER - Extension)
*/

/*
 * ####################################################################################################################
 * INCLUDES
 * ####################################################################################################################
 */
#include "ptxNDEF.h"
#include "ptx_IOT_READER.h"
#include <string.h>

/*
 * ####################################################################################################################
 * DEFINES / TYPES
 * ####################################################################################################################
 */

/*
 * ####################################################################################################################
 * INTERNAL FUNCTIONS / HELPERS
 * ####################################################################################################################
 */

/*
 * ####################################################################################################################
 * API FUNCTIONS
 * ####################################################################################################################
 */
ptxStatus_t ptxNDEF_Open (ptxNDEF_t *ndefComp, ptxNDEF_InitParams_t *initParams)
{
    ptxStatus_t status = ptxStatus_Success;
    ptxNDEF_T2TOP_InitParams_t t2t_init_params;
    ptxNDEF_T3TOP_InitParams_t t3t_init_params;
    ptxNDEF_T4TOP_InitParams_t t4t_init_params;
    ptxNDEF_T5TOP_InitParams_t t5t_init_params;

    if ((NULL != ndefComp) && (NULL != initParams))
    {
        if ((NULL != initParams->IotRd) &&
            (NULL != initParams->TxBuffer) &&
            (0 != initParams->TxBufferSize) &&
            (NULL != initParams->RxBuffer) &&
            (0 != initParams->RxBufferSize))
        {
            /* clear component */
            (void)memset(ndefComp, 0, sizeof(ptxNDEF_t));

            /* set members */
            (void)ptxIoTRd_Get_Card_Registry (initParams->IotRd, &ndefComp->CardRegistry);

            if (ptxStatus_Success == status)
            {
                /* initialize lower layer component(s) */
                (void)memset(&t2t_init_params, 0, sizeof(ptxNDEF_T2TOP_InitParams_t));

                t2t_init_params.IotRd = initParams->IotRd;
                t2t_init_params.TxBuffer = initParams->TxBuffer;
                t2t_init_params.TxBufferSize = initParams->TxBufferSize;
                t2t_init_params.RxBuffer = initParams->RxBuffer;
                t2t_init_params.RxBufferSize = initParams->RxBufferSize;

                status = ptxNDEF_T2TOpOpen(&ndefComp->T2TOP, &t2t_init_params);
            }

            if (ptxStatus_Success == status)
            {
                /* initialize lower layer component(s) */
                (void)memset(&t3t_init_params, 0, sizeof(ptxNDEF_T3TOP_InitParams_t));

                t3t_init_params.IotRd = initParams->IotRd;
                t3t_init_params.TxBuffer = initParams->TxBuffer;
                t3t_init_params.TxBufferSize = initParams->TxBufferSize;
                t3t_init_params.RxBuffer = initParams->RxBuffer;
                t3t_init_params.RxBufferSize = initParams->RxBufferSize;

                if (ndefComp->CardRegistry->ActiveCardProtType == Prot_T3T)
                {
                    t3t_init_params.NFCID2 = &ndefComp->CardRegistry->ActiveCard->TechParams.CardFParams.SENSF_RES[0];
                    t3t_init_params.NFCID2Len = ndefComp->CardRegistry->ActiveCard->TechParams.CardFParams.SENSF_RES_LEN;
                }

                status = ptxNDEF_T3TOpOpen(&ndefComp->T3TOP, &t3t_init_params);
            }

            if (ptxStatus_Success == status)
            {
                /* initialize lower layer component(s) */
                (void)memset(&t4t_init_params, 0, sizeof(ptxNDEF_T4TOP_InitParams_t));

                t4t_init_params.IotRd = initParams->IotRd;
                t4t_init_params.TxBuffer = initParams->TxBuffer;
                t4t_init_params.TxBufferSize = initParams->TxBufferSize;
                t4t_init_params.RxBuffer = initParams->RxBuffer;
                t4t_init_params.RxBufferSize = initParams->RxBufferSize;

                status = ptxNDEF_T4TOpOpen(&ndefComp->T4TOP, &t4t_init_params);
            }

            if (ptxStatus_Success == status)
            {
                /* initialize lower layer component(s) */
                (void)memset(&t5t_init_params, 0, sizeof(ptxNDEF_T5TOP_InitParams_t));

                t5t_init_params.IotRd = initParams->IotRd;
                t5t_init_params.TxBuffer = initParams->TxBuffer;
                t5t_init_params.TxBufferSize = initParams->TxBufferSize;
                t5t_init_params.RxBuffer = initParams->RxBuffer;
                t5t_init_params.RxBufferSize = initParams->RxBufferSize;

                if (ndefComp->CardRegistry->ActiveCardProtType == Prot_T5T)
                {
                    t5t_init_params.UID = &ndefComp->CardRegistry->ActiveCard->TechParams.CardVParams.UID[0];
                    t5t_init_params.UIDLen = PTX_T5T_UID_SIZE;
                }

                status = ptxNDEF_T5TOpOpen(&ndefComp->T5TOP, &t5t_init_params);
            }

            /* set Component-ID at the end to prevent further calls in case of an error */
            if (ptxStatus_Success == status)
            {
                ndefComp->CompId = ptxStatus_Comp_NDEF;
            }

        } else
        {
            status = PTX_STATUS(ptxStatus_Comp_NDEF, ptxStatus_InvalidParameter);
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NDEF, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNDEF_FormatTag (ptxNDEF_t *ndefComp)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(ndefComp, ptxStatus_Comp_NDEF))
    {
        switch (ndefComp->CardRegistry->ActiveCardProtType)
        {
            case Prot_T2T:
                status = ptxNDEF_T2TOpFormatTag(&ndefComp->T2TOP);
                break;

            case Prot_T3T:
                status = ptxNDEF_T3TOpFormatTag(&ndefComp->T3TOP);
                break;

            case Prot_ISODEP:
                status = ptxNDEF_T4TOpFormatTag(&ndefComp->T4TOP);
                break;

            case Prot_T5T:
                status = ptxNDEF_T5TOpFormatTag(&ndefComp->T5TOP);
                break;

            default:
                status = PTX_STATUS(ptxStatus_Comp_NDEF, ptxStatus_NotImplemented);
                break;
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NDEF, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNDEF_CheckMessage (ptxNDEF_t *ndefComp)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(ndefComp, ptxStatus_Comp_NDEF))
    {
        switch (ndefComp->CardRegistry->ActiveCardProtType)
        {
            case Prot_T2T:
                status = ptxNDEF_T2TOpCheckMessage(&ndefComp->T2TOP);
                break;

            case Prot_T3T:
                status = ptxNDEF_T3TOpCheckMessage(&ndefComp->T3TOP);
                break;

            case Prot_ISODEP:
                status = ptxNDEF_T4TOpCheckMessage(&ndefComp->T4TOP);
                break;

            case Prot_T5T:
                status = ptxNDEF_T5TOpCheckMessage(&ndefComp->T5TOP);
                break;

            default:
                status = PTX_STATUS(ptxStatus_Comp_NDEF, ptxStatus_NotImplemented);
                break;
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NDEF, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNDEF_ReadMessage (ptxNDEF_t *ndefComp, uint8_t *msgBuffer, uint32_t *msgLen)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(ndefComp, ptxStatus_Comp_NDEF) && (NULL != msgBuffer) && (NULL != msgLen))
    {
        if (0 != *msgLen)
        {
            switch (ndefComp->CardRegistry->ActiveCardProtType)
            {
                case Prot_T2T:
                    status = ptxNDEF_T2TOpReadMessage(&ndefComp->T2TOP, msgBuffer, msgLen);
                    break;

                case Prot_T3T:
                    status = ptxNDEF_T3TOpReadMessage(&ndefComp->T3TOP, msgBuffer, msgLen);
                    break;

                case Prot_ISODEP:
                    status = ptxNDEF_T4TOpReadMessage(&ndefComp->T4TOP, msgBuffer, msgLen);
                    break;

                case Prot_T5T:
                    status = ptxNDEF_T5TOpReadMessage(&ndefComp->T5TOP, msgBuffer, msgLen);
                    break;

                default:
                    status = PTX_STATUS(ptxStatus_Comp_NDEF, ptxStatus_InvalidState);
                    break;
            }
        } else
        {
            status = PTX_STATUS(ptxStatus_Comp_NDEF, ptxStatus_NotImplemented);
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NDEF, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNDEF_WriteMessage (ptxNDEF_t *ndefComp, uint8_t *msgBuffer, uint32_t msgLen)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(ndefComp, ptxStatus_Comp_NDEF) && (NULL != msgBuffer) && (0 != msgLen))
    {
        switch (ndefComp->CardRegistry->ActiveCardProtType)
        {
            case Prot_T2T:
                status = ptxNDEF_T2TOpWriteMessage(&ndefComp->T2TOP, msgBuffer, msgLen);
                break;

            case Prot_T3T:
                status = ptxNDEF_T3TOpWriteMessage(&ndefComp->T3TOP, msgBuffer, msgLen);
                break;

            case Prot_ISODEP:
                status = ptxNDEF_T4TOpWriteMessage(&ndefComp->T4TOP, msgBuffer, msgLen);
                break;

            case Prot_T5T:
                status = ptxNDEF_T5TOpWriteMessage(&ndefComp->T5TOP, msgBuffer, msgLen);
                break;

            default:
                status = PTX_STATUS(ptxStatus_Comp_NDEF, ptxStatus_NotImplemented);
                break;
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NDEF, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNDEF_LockTag (ptxNDEF_t *ndefComp)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(ndefComp, ptxStatus_Comp_NDEF))
    {
        switch (ndefComp->CardRegistry->ActiveCardProtType)
        {
            case Prot_T2T:
                status = ptxNDEF_T2TOpLockTag(&ndefComp->T2TOP);
                break;

            case Prot_T3T:
                status = ptxNDEF_T3TOpLockTag(&ndefComp->T3TOP);
                break;

            case Prot_ISODEP:
                status = ptxNDEF_T4TOpLockTag(&ndefComp->T4TOP);
                break;

            case Prot_T5T:
                status = ptxNDEF_T5TOpLockTag(&ndefComp->T5TOP);
                break;

            default:
                status = PTX_STATUS(ptxStatus_Comp_NDEF, ptxStatus_NotImplemented);
                break;
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NDEF, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxNDEF_Close (ptxNDEF_t *ndefComp)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(ndefComp, ptxStatus_Comp_NDEF))
    {
        (void)ptxNDEF_T2TOpClose(&ndefComp->T2TOP);
        (void)ptxNDEF_T3TOpClose(&ndefComp->T3TOP);
        (void)ptxNDEF_T4TOpClose(&ndefComp->T4TOP);
        (void)ptxNDEF_T5TOpClose(&ndefComp->T5TOP);

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_NDEF, ptxStatus_InvalidParameter);
    }

    return status;
}

/*
 * ####################################################################################################################
 * INTERNAL FUNCTIONS / CALLBACK(s)
 * ####################################################################################################################
 */


