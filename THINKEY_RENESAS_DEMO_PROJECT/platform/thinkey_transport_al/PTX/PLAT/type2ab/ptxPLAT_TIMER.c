/** \file
   ---------------------------------------------------------------
   Copyright (C) 2021. Panthronics AG - All Rights Reserved.

   This material may not be reproduced, displayed, modified or
   distributed without the express prior written permission of the
   Panthronics AG.

   PLEASE REFER TO OUR DISCLAIMER NOTICE IN THE PROJECT ROOT.
   ---------------------------------------------------------------

    Project     : PTX100x
    Module      : PLAT
    File        : ptxPLAT_TIMER.c

    Description :
*/


/*
 * ####################################################################################################################
 * INCLUDES
 * ####################################################################################################################
 */

#include "ptxPLAT_TIMER.h"

#include "ptxPLAT_EXT.h"
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"


#include "thinkey_osal.h"

//void Thinkey_Timer_Callback(THINKey_HANDLE hHandle);
/*
 * ####################################################################################################################
 * DEFINES / TYPES
 * ####################################################################################################################
 */

/**
 * Timer Context.
 */
ptxPlatTimer_t timer_ctx;

/*
 * ####################################################################################################################
 * API FUNCTIONS
 * ####################################################################################################################
 */

void Thinkey_Timer_Callback(THINKey_HANDLE hHandle) {
    if(timer_ctx.ISRCallBack) {
        timer_ctx.ISRCallBack(timer_ctx.ISRCxt);
    }
    timer_ctx.IsElapsed = 1;
}



ptxStatus_t ptxPLAT_TIMER_GetInitializedTimer(ptxPlatTimer_t **timer)
{
    ptxStatus_t status = ptxStatus_Success;

    if (NULL != timer)
    {
        /**
         * Initialization of the TIMER Context
         */
    	memset(&timer_ctx, 0, sizeof(ptxPlatTimer_t));

        timer_ctx.TimerInstance = THINKey_OSAL_hCreateOneShotTimer(
                Thinkey_Timer_Callback, NULL);

        if(NULL == timer_ctx.TimerInstance) {
            status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InternalError);
        }
        *timer = &timer_ctx;
    }
    else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxPLAT_TIMER_Start(ptxPlatTimer_t *timer, uint32_t ms, uint8_t isBlock, pptxPlat_TimerCallBack_t fnISRCb, void *ISRCxt)
{
    ptxStatus_t status = ptxStatus_Success;

    if ((NULL != timer) && (ms > 0) && ((0 == isBlock) || (1 == isBlock)))
    {
        /** Clear IsElpased state */
        timer->IsElapsed = 0;
		timer->ISRCallBack = fnISRCb;
		timer->ISRCxt = ISRCxt;
		timer->TimerState = Timer_InUse;
	
        THINKey_OSAL_eStartTimer(timer->TimerInstance, ms);
        if(1 == isBlock)
		{
			/* Block until timer elapses */
			while(!timer->IsElapsed) {}
        } else {
        	//printf("TImer started\r\n");
        }
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
    }
    return status;
}

ptxStatus_t ptxPLAT_TIMER_IsElapsed(ptxPlatTimer_t *timer, uint8_t *isElapsed)
{
    ptxStatus_t status = ptxStatus_Success;

    if ( (NULL != timer) && (NULL != isElapsed))
    {
        *isElapsed = timer->IsElapsed;

        if (0 != timer->IsElapsed)
        {
            ptxPLAT_TIMER_Stop(timer);
        }
    }
    else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
    }
    return status;
}

ptxStatus_t ptxPLAT_TIMER_Stop(ptxPlatTimer_t *timer)
{
    ptxStatus_t status = ptxStatus_Success;

    if (NULL != timer)
    {
        THINKey_OSAL_eStopTimer(timer->TimerInstance);
		timer->IsElapsed = 0;
		timer->TimerState = Timer_Free;
    }
    else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
    }
    return status;
}

ptxStatus_t ptxPLAT_TIMER_Deinit(ptxPlatTimer_t *timer)
{
    ptxStatus_t status = ptxStatus_Success;

    if (NULL != timer)
    {
        THINKey_OSAL_eDestroyTimer(timer->TimerInstance);
		memset(&timer_ctx, 0, sizeof(ptxPlatTimer_t));
    }
    else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
    }
    return status;
}


ptxStatus_t ptxPLAT_TIMER_Sleep(ptxPlatTimer_t *timer, uint32_t sleep_ms) {
    ptxStatus_t status = ptxStatus_Success;

    if (NULL != timer)
	{
        nrf_delay_ms(sleep_ms);
	}
	else
	{
		status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
	}
	return status;
}

/*
 * ####################################################################################################################
 * INTERNAL FUNCTIONS
 * ####################################################################################################################
 */
#if 0
void Thinkey_SysTick_Handler(void) {
	ticks++;
	if(msExpiryTick) {
		if(ticks > msExpiryTick) {
			if(timer_ctx.ISRCallBack) {
				timer_ctx.ISRCallBack(timer_ctx.ISRCxt);
			}
			msExpiryTick = 0;
			timer_ctx.IsElapsed = 1;
		}

	}
}
#endif

