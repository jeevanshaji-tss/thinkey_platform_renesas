/** \file
   ---------------------------------------------------------------
   Copyright (C) 2021. Panthronics AG - All Rights Reserved.

   This material may not be reproduced, displayed, modified or
   distributed without the express prior written permission of the
   Panthronics AG.

   PLEASE REFER TO OUR DISCLAIMER NOTICE IN THE PROJECT ROOT.
   ---------------------------------------------------------------

    Project     : PTX100x
    Module      : NSC System Features
    File        : ptxNSC_System.c

    Description :
*/

#include "ptxNSC_System.h"
#include "ptxNSC.h"
#include <string.h>


/*
 * ####################################################################################################################
 * FUNCTIONS API.
 * ####################################################################################################################
 */

ptxStatus_t ptxNSC_System_SetConfig (ptxNSC_t *nscCtx, ptxNSC_System_t *sysParams)
{
    ptxStatus_t ret = ptxStatus_Success;

    if (PTX_COMP_CHECK(nscCtx, ptxStatus_Comp_NSC) && (NULL != sysParams))
    {
        (void)memcpy(nscCtx->SysParams, sysParams, sizeof(ptxNSC_System_t));

    } else
    {
        ret = PTX_STATUS(ptxStatus_Comp_NSC, ptxStatus_InvalidParameter);
    }

    ret = ptxNSC_CheckSystemState(nscCtx, ret);

    return ret;
}

ptxStatus_t ptxNSC_System_GetConfig (ptxNSC_t *nscCtx, ptxNSC_System_t *sysParams)
{
    ptxStatus_t ret = ptxStatus_Success;

    if (PTX_COMP_CHECK(nscCtx, ptxStatus_Comp_NSC) && (NULL != sysParams))
    {
        (void)memcpy(sysParams, nscCtx->SysParams, sizeof(ptxNSC_System_t));

    } else
    {
        ret = PTX_STATUS(ptxStatus_Comp_NSC, ptxStatus_InvalidParameter);
    }

    return ret;
}

ptxStatus_t ptxNSC_System_ApplyDefaults (ptxNSC_t *nscCtx)
{
    ptxStatus_t ret = ptxStatus_Success;

    if (PTX_COMP_CHECK(nscCtx, ptxStatus_Comp_NSC))
    {
        nscCtx->SysParams->Version = SYSTEM_DEFAULT_VERSION;
        nscCtx->SysParams->PowerAmpOverCurrThreshold = SYSTEM_DEFAULT_PA_OVERCURRENT_TH;
        nscCtx->SysParams->PowerAmpTempThreshold     = SYSTEM_DEFAULT_PA_TEMPERATURE_TH;
        nscCtx->SysParams->ConClkSource              = SYSTEM_DEFAULT_CON_CLK_SOURCE;
        nscCtx->SysParams->ConVarLBS                 = SYSTEM_DEFAULT_CON_VAR_LBS;
        (void)memcpy(&nscCtx->SysParams->conHost[0],   &SYSTEM_DEFAULT_CON_NHOST[0],    sizeof(SYSTEM_DEFAULT_CON_NHOST));
        (void)memcpy(&nscCtx->SysParams->conHostCE[0], &SYSTEM_DEFAULT_CON_NHOST_CE[0], sizeof(SYSTEM_DEFAULT_CON_NHOST_CE));

    } else
    {
        ret = PTX_STATUS(ptxStatus_Comp_NSC, ptxStatus_InvalidParameter);
    }

    return ret;
}


