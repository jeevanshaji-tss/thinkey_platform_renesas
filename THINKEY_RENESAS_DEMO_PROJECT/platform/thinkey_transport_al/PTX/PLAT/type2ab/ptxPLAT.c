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
    File        : ptxPLAT.c

    Description :
*/

/*
 * ####################################################################################################################
 * INCLUDES
 * ####################################################################################################################
 */

#include "ptxPLAT.h"
#include "ptxPLAT_EXT.h"
#include <string.h>

#include "ptxPLAT_INT.h"
#include <stdio.h>
#include "thinkey_debug.h"
#include <unistd.h>
#include <stdbool.h>

/*
 * ####################################################################################################################
 * DEFINES / TYPES
 * ####################################################################################################################
 */
#define PTX_PLAT_MAXNUMOF_TXBUFF    (0x04u)
#define PTX_PLAT_MAX_PAYLOAD_LEN    (1024u)

/*
 * ####################################################################################################################
 * INTERNAL FUNCTIONS
 * ####################################################################################################################
 */

ptxPlat_t platform;


/*
 * ####################################################################################################################
 * API FUNCTIONS
 * ####################################################################################################################
 */

ptxStatus_t ptxPLAT_AllocAndInit(ptxPlat_t **plat, ptxPlat_ConfigPars_t *initParams)
{
    ptxStatus_t status = ptxStatus_Success;

    if ((NULL != plat) && (NULL != initParams))
    {
        (void)memset(&platform, 0, sizeof(ptxPlat_t));

        platform.CompId = ptxStatus_Comp_PLAT;
        platform.IRQDisabledCnt = 0;

        /* Init SPI. */
        ptxPLAT_SpiConfigPars_t spi_pars;   
        spi_pars.IntfSpeed = initParams->baudRate;//5000000u;
        //THINKEY_DEBUG_INFO("Frequency: %lu", spi_pars.IntfSpeed);
        status = ptxPLAT_SPI_GetInitialized(&platform.Spi, &spi_pars);
        if (ptxStatus_Success == status)
        {
            *plat = &platform;
        }
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
    }
    return status;
}

ptxStatus_t ptxPLAT_Deinit(ptxPlat_t *plat)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(plat, ptxStatus_Comp_PLAT))
    {
        (void) ptxPLAT_SPI_Deinit(plat->Spi);
        /* Clear Platform Context. */
        (void)memset(plat, 0, sizeof(ptxPlat_t));
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxPLAT_TRx(struct ptxPlat *plat, uint8_t *txBuf[], size_t txLen[], size_t numTxBuffers, uint8_t *rxBuf[], size_t *rxLen[], size_t numRxBuffers)
{
    ptxStatus_t status = ptxStatus_Success;
    const size_t numBuffers_max = PTX_PLAT_MAXNUMOF_TXBUFF;
    size_t num_buffers_tx = numTxBuffers;


    if (PTX_COMP_CHECK(plat, ptxStatus_Comp_PLAT) && (num_buffers_tx < numBuffers_max) && (numRxBuffers <= numBuffers_max))
    {
        status = ptxPLAT_SPI_TRx(plat->Spi, txBuf, txLen, numTxBuffers, rxBuf, rxLen, numRxBuffers);
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxPLAT_StartWaitForRx(struct ptxPlat *plat, pptxPlat_RxCallBack_t irqCb, void *ctxIrqCb)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(plat, ptxStatus_Comp_PLAT) && (NULL != irqCb) && (NULL != ctxIrqCb))
    {
        status = ptxPLAT_SPI_StartWaitForRx(plat->Spi, irqCb, ctxIrqCb);
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
    }
    return status;
}

ptxStatus_t ptxPLAT_StopWaitForRx(struct ptxPlat *plat)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(plat, ptxStatus_Comp_PLAT))
    {
        status = ptxPLAT_SPI_StopWaitForRx(plat->Spi);
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
    }
    return status;
}

ptxStatus_t ptxPLAT_WaitForInterrupt(struct ptxPlat *plat)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(plat, ptxStatus_Comp_PLAT))
    {
        //THINKEY_DEBUG_INFO("Waiting for interrupt");

        /* Wait for Interrupts */
        __DSB();
        __WFI();
        __ISB();

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
    }

    return status;
}

uint8_t ptxPLAT_CheckRxActive(struct ptxPlat *plat)
{
    uint8_t rx_active = 0;
	//printf("\r\nchecking rx active\r\n");

    if (PTX_COMP_CHECK(plat, ptxStatus_Comp_PLAT))
    {

    }

    return rx_active;
}

ptxStatus_t ptxPLAT_IsRxPending (struct ptxPlat *plat , uint8_t *isRxPending)
{
    ptxStatus_t status = ptxStatus_Success;

    if ((NULL != plat) && (NULL != isRxPending))
    {
    	bool isPending = false;
        //THINKEY_DEBUG_INFO("check anything pending to read");
    	isPending = ptxPLAT_GPIO_IsRxPending(plat->Spi);
    	*isRxPending = isPending;
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxPLAT_TriggerRx(ptxPlat_t *plat)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(plat, ptxStatus_Comp_PLAT))
    {
    	status = ptxPLAT_SPI_TriggerRx(plat->Spi);
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
    }

    return status;
}


ptxStatus_t ptxPLAT_Sleep(ptxPlat_t *plat, uint32_t sleep_ms)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(plat, ptxStatus_Comp_PLAT))
    {
       // nrf_delay_ms(sleep_ms);
        const bsp_delay_units_t bsp_delay_units = BSP_DELAY_UNITS_MILLISECONDS;
        R_BSP_SoftwareDelay(sleep_ms,bsp_delay_units);
    }
    else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
    }

    (void)sleep_ms;
    return status;
}

void ptxPLAT_DisableInterrupts (ptxPlat_t *plat)
{
    if (NULL != plat)
    {
        /* Disable interrupts */
        //sd_nvic_critical_region_enter(&gIsNested);
        /* Increase number of disable interrupt function calls */
        plat->IRQDisabledCnt++;
    }
}

void ptxPLAT_EnableInterrupts (ptxPlat_t *plat)
{
    if (NULL != plat)
    {
        /* Decrease number of disable interrupt function calls */
        if (0 != plat->IRQDisabledCnt)
        {
            plat->IRQDisabledCnt--;
        }

        //sd_nvic_critical_region_exit(gIsNested);

        /* Check if we are ready to enable interrupts */
        if (0 == plat->IRQDisabledCnt)
        {
            /* Enable interrupts */

        }
    }
}


ptxStatus_t ptxPLAT_ResetChip ( struct ptxPlat *plat )
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(plat, ptxStatus_Comp_PLAT))
    {
    	//Set the status to not implemented
    	status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_NotImplemented);

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
    }

    return status;
}


ptxStatus_t ptxPLAT_GetInitializedTimer(struct ptxPlat *plat, struct ptxPlatTimer **timer)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(plat, ptxStatus_Comp_PLAT))
    {
        status = ptxPLAT_TIMER_GetInitializedTimer(timer);
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxPLAT_TimerStart(struct ptxPlat *plat, struct ptxPlatTimer *timer, uint32_t ms, uint8_t isBlock, pptxPlat_TimerCallBack_t fnISRCb, void *ISRCxt)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(plat, ptxStatus_Comp_PLAT))
    {
        status = ptxPLAT_TIMER_Start(timer, ms, isBlock, fnISRCb, ISRCxt);
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxPLAT_TimerIsElapsed(struct ptxPlat *plat, struct ptxPlatTimer *timer, uint8_t *isElapsed)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(plat, ptxStatus_Comp_PLAT))
    {
        status = ptxPLAT_TIMER_IsElapsed(timer, isElapsed);
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxPLAT_TimerDeinit(struct ptxPlat *plat, struct ptxPlatTimer *timer)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(plat, ptxStatus_Comp_PLAT))
    {
        status = ptxPLAT_TIMER_Deinit(timer);
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
    }


    return status;
}




