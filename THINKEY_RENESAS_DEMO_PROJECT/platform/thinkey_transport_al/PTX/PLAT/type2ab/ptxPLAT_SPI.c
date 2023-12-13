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
    File        : ptxPLAT_SPI.c

    Description : basic SPI implementation on Renesas RA4M2 platform.
         1) Pins: SPI0. SPI_CK = P4_12, MOSI = P4_11, MISO = P4_10, CS = P4_13
         2) Spi instance is automatically generated during build. Spi Open assigns this instance to the pointer.
         3) Callback function has to be defined - it is used to report the actual status of the transfer.

 */

/*
 * ####################################################################################################################
 * INCLUDES
 * ####################################################################################################################
 */
#include "ptxPLAT_SPI.h"
#include "r_spi.h"
#include "r_spi_cfg.h"
#include "ptxPLAT_EXT.h"
#include <string.h>
#include "hal_data.h"
#include <common_utils.h>


//#include "sdk_config.h"
//#include <nrfx_spi.h>
//#include <nrf_drv_spi.h>
//#include <nrf_gpio.h>

#include <stdio.h>
#include "thinkey_debug.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
//#include "nrf_delay.h"
//#include "nrf_drv_gpiote.h"

/*
 * ####################################################################################################################
 * DEFINES / TYPES
 * ####################################################################################################################
 */

#define PRINT_DATA 0

#define THINKEY_SPI_MOSI               NRF_GPIO_PIN_MAP(0, 24) 
#define THINKEY_SPI_MISO               NRF_GPIO_PIN_MAP(1, 9) 
#define THINKEY_SPI_SCLK               NRF_GPIO_PIN_MAP(0, 13)
#define THINKEY_SPI_SS                 NRF_GPIO_PIN_MAP(0, 27)
#define THINKEY_GPIO_INTR_PIN          NRF_GPIO_PIN_MAP (0, 2)

/*
 * ####################################################################################################################
 * STATIC VARIABLES
 * ####################################################################################################################
 */

 
#define SPI_INSTANCE  1
//nrf_drv_spi_t spi_t = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);

/**
 * Instance of spi context.
 */
ptxPLAT_Spi_t spi_ctx;

void printbuf(uint8_t *buf, unsigned int len) {
	//printf("\r\n size: %d\r\n", len);
	for(int i=0; i < (int)len; i++)
		//printf("0x%x ", buf[i]);
	    SEGGER_RTT_printf(0, "0x%x", buf[i]);
	//printf("\r\n\n");
	SEGGER_RTT_printf(0,"\r\n\n");
}

//void ptx_irq_handler(nrf_drv_gpiote_pin_t irqPin, nrf_gpiote_polarity_t irq_action)
//{
//
//}

/*
 * ####################################################################################################################
 * API FUNCTIONS
 * ####################################################################################################################
 */
BaseType_t xHigherPriorityTaskWoken = pdFALSE;
static volatile spi_event_t g_master_event_flag;
static SemaphoreHandle_t bin_sem;

void spi_callback(spi_callback_args_t * p_args)
{
    if (SPI_EVENT_TRANSFER_COMPLETE == p_args->event)
    {
        g_master_event_flag = SPI_EVENT_TRANSFER_COMPLETE;
        xSemaphoreGiveFromISR(bin_sem,&xHigherPriorityTaskWoken);
        //SEGGER_RTT_printf(0,"sem val after give %d",(int)uxSemaphoreGetCount( bin_sem ));

    }
    else
    {
        g_master_event_flag = SPI_EVENT_TRANSFER_ABORTED;
    }
}
 
ptxStatus_t ptxPLAT_SPI_GetInitialized(ptxPLAT_Spi_t **spi, ptxPLAT_SpiConfigPars_t *spiPars)
{
    ptxStatus_t status = ptxStatus_Success;
    //ret_code_t err_code;
    fsp_err_t err_code;   //= FSP_SUCCESS;
   // THINKEY_DEBUG_INFO("ptxPLAT_SPI_GetInitialized entered %d", APP_IRQ_PRIORITY_LOW);

    if ((NULL != spi) && (NULL != spiPars))
    {
       // ret_code_t result = NRF_SUCCESS;
        fsp_err_t result = FSP_SUCCESS;
        /**
         * Initialization of the SPI Context
         */
        memset(&spi_ctx, 0, sizeof(ptxPLAT_Spi_t));

        do {
           // nrfx_gpiote_out_config_t sChipSelectPinConfig = GPIOTE_CONFIG_OUT_SIMPLE(1);
           // nrf_drv_gpiote_out_init(THINKEY_SPI_SS, &sChipSelectPinConfig);
            // pin SPI_SS configured on P800


//            nrf_drv_spi_config_t p_config =
//            {   .bit_order = NRF_SPI_BIT_ORDER_MSB_FIRST,
//                .frequency =  spiPars->IntfSpeed,
//                .miso_pin = THINKEY_SPI_MISO,
//                .mosi_pin = THINKEY_SPI_MOSI,
//                .mode = NRF_SPI_MODE_0,
//                .sck_pin =  THINKEY_SPI_SCLK,
//                .ss_pin =   NRF_DRV_SPI_PIN_NOT_USED,
//                .irq_priority = APP_IRQ_PRIORITY_LOW,
//            };


           // result  = nrf_drv_spi_init(&spi_t, &p_config, NULL , NULL);

            bin_sem = xSemaphoreCreateBinary();

            result = R_SPI_Open(&g_spi0_ctrl, &g_spi0_cfg);


             SEGGER_RTT_printf(0,"sem init %d \n",uxSemaphoreGetCount( bin_sem ));
            //THINKEY_DEBUG_INFO("SPI Init %d", result);
            if(result != FSP_SUCCESS) {
                THINKEY_DEBUG_INFO("SPI Init failed %lu", result);
                status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InternalError);
                break;
            }

            //nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
            //in_config.pull = NRF_GPIO_PIN_PULLDOWN;
            // my comment : create a gpio pin and configure it as low by default.....

            //err_code = nrf_drv_gpiote_in_init(THINKEY_GPIO_INTR_PIN, &in_config, ptx_irq_handler);
           // THINKEY_DEBUG_INFO("nrf_drv_gpiote_in_init" );

           // nrf_drv_gpiote_in_event_enable(THINKEY_GPIO_INTR_PIN, false);

            // here the GPIO_INTR_PIN is configured at p8004
            spi_ctx.pSPI_instance = g_spi0_ctrl;
            *spi = &spi_ctx;
        } while(0);
    }

    else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);   
    }

    return status; 
}

ptxStatus_t ptxPLAT_SPI_Deinit(ptxPLAT_Spi_t *spi)   
{
    ptxStatus_t status = ptxStatus_Success;

    if (NULL != spi)
    {
        /* TODO:
        /**
         * De Init tasks shall be performed here:
         *  close spi port - power down the peripheral
         *  de-init external irq gpio module
         */

        /** Clean spi context structure. */
        //memset(&spi_ctx, 0, sizeof(ptxPLAT_Spi_t));
    }else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
    }
    return status;
}


ptxStatus_t ptxPLAT_SPI_TRx(ptxPLAT_Spi_t *spi, uint8_t *txBuf[], size_t txLen[], size_t numTxBuffers, uint8_t *rxBuf[], size_t *rxLen[], size_t numRxBuffers)
{
    ptxStatus_t status = ptxStatus_Success;
    uint32_t num_bytes_master = RESET_VALUE;

    uint8_t tempWriteBuf[256];
    uint8_t tempReadBuf[256];
    if ((NULL != spi) && (NULL != txBuf) && (NULL != txLen))
    {
        //nrfx_err_t  result = NRF_SUCCESS;
        fsp_err_t result = FSP_SUCCESS;

        int i = 0;

        unsigned int tempTxLen = 0;
        unsigned int tempRxLen = 0;
    #if 1
        memset(tempWriteBuf, 0x00, 256);
        memset(tempReadBuf, 0xFF, 256);
    #endif
       // nrfx_gpiote_out_clear(THINKEY_SPI_SS);


       // R_BSP_PinWrite( SPI_SS  , BSP_IO_LEVEL_LOW );
        result = R_IOPORT_PinWrite(&g_ioport_ctrl, SPI_SS, BSP_IO_LEVEL_LOW);


        for(i = 0; i < (int)numTxBuffers; i++) {
            memcpy(&tempWriteBuf[tempTxLen], txBuf[i], txLen[i]);
            tempTxLen += txLen[i];
        }

        for(i = 0; i < (int )numRxBuffers; i++){
            if((NULL != rxBuf) && (NULL != rxLen)) {
                tempRxLen += *rxLen[i];
            }
        }

        if((NULL != rxBuf) && (NULL != rxLen)) {
            /* This is to write-read */

           // result  = nrf_drv_spi_transfer(&spi_t, tempWriteBuf, tempTxLen, tempReadBuf, tempRxLen + tempTxLen);
            result = R_SPI_WriteRead(&g_spi0_ctrl, tempWriteBuf, tempReadBuf, tempRxLen + tempTxLen, SPI_BIT_WIDTH_8_BITS);

            xSemaphoreTake(bin_sem,portMAX_DELAY);
           // SEGGER_RTT_printf(0,"sem val after take %d",(int)uxSemaphoreGetCount( bin_sem ));
          //  result = R_IOPORT_PinWrite(&g_ioport_ctrl, SPI_SS, BSP_IO_LEVEL_HIGH);

            if(result == FSP_SUCCESS) {
                int len = tempTxLen;
               // THINKEY_DEBUG_INFO("SPI Write-Read Success");
    #if PRINT_DATA
                //THINKEY_DEBUG_INFO("Write data:");
                SEGGER_RTT_printf(0,"write data\n");
                printbuf(tempWriteBuf, tempTxLen);
                //THINKEY_DEBUG_INFO("Read data:");
                SEGGER_RTT_printf(0,"Read data\n");
                printbuf(&tempReadBuf[len], tempRxLen);
    #endif
                for(i = 0; i < (int)numRxBuffers; i++) {
                    memcpy(rxBuf[i], &tempReadBuf[len], *rxLen[i]);
                    len += *rxLen[i];
                    if(len > tempRxLen) {break;}
                }
            }
        } else {
           // result = R_IOPORT_PinWrite(&g_ioport_ctrl, SPI_SS, BSP_IO_LEVEL_LOW);
            for(i = 0; i < (int)numTxBuffers; i++){
    #if PRINT_DATA
                //THINKEY_DEBUG_INFO("Going to write");
                SEGGER_RTT_printf(0,"Going to write data\n");
                printbuf(txBuf[i], txLen[i]);
    #endif
               // result  = nrf_drv_spi_transfer(&spi_t, txBuf[i], txLen[i], NULL, 0);

                result = R_SPI_Write(&g_spi0_ctrl, txBuf[i], txLen[i], SPI_BIT_WIDTH_8_BITS);
               // SEGGER_RTT_printf(0,"sem val before take %d",(int)uxSemaphoreGetCount( bin_sem ));
                xSemaphoreTake(bin_sem,portMAX_DELAY);
               // SEGGER_RTT_printf(0,"sem val after take %d",(int)uxSemaphoreGetCount( bin_sem ));

                if(result == FSP_SUCCESS) {
                    //THINKEY_DEBUG_INFO("SPI write success");
                } else {
                    //THINKEY_DEBUG_ERROR("Write Failed!!");
                }
            }

        }

        //nrf_drv_gpiote_out_set(THINKEY_SPI_SS);
       // R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);



        result = R_IOPORT_PinWrite(&g_ioport_ctrl, SPI_SS, BSP_IO_LEVEL_HIGH);



    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxPLAT_SPI_StartWaitForRx(ptxPLAT_Spi_t *spi, pptxPlat_RxCallBack_t irqCb, void *ctxIrqCb)
{
    ptxStatus_t status = ptxStatus_Success;
    uint32_t read_val = 1u;

    if ((NULL != spi) && (NULL != irqCb) && (NULL != ctxIrqCb))
    {
        /** This function shall ensure that IRQ is not high before starting the wait for the event. */

        /** First register the callback function and the context, for future use */
        spi->RxCb = irqCb;
        spi->CtxRxCb = ctxIrqCb;
       // THINKEY_DEBUG_INFO("Waiting fro RX");
        //read_val = nrf_drv_gpiote_in_is_set(THINKEY_GPIO_INTR_PIN);
        // read_val = R_BSP_PinRead (GPIO_INTR_PIN);
         read_val = R_BSP_PinRead (INTRQ);

        if(1u == read_val) {
            /* Data present call callback */
            spi->RxCb(spi->CtxRxCb);
        }
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxPLAT_SPI_StopWaitForRx(ptxPLAT_Spi_t *spi)
{
    ptxStatus_t status = ptxStatus_Success;

    if (NULL != spi)
    {
        /** Stop the wait for the trigger on IRQ (Not blocking) */
        spi->RxCb = NULL;
        spi->CtxRxCb = NULL;
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
    }
    return status;
}

ptxStatus_t ptxPLAT_SPI_TriggerRx(ptxPLAT_Spi_t *spi)
{
    ptxStatus_t status = ptxStatus_Success;
    uint32_t read_val = 0u;

    if (NULL != spi)
    {
        //read_val = nrf_drv_gpiote_in_is_set(THINKEY_GPIO_INTR_PIN);
      //  read_val = R_BSP_PinRead(GPIO_INTR_PIN);
        read_val = R_BSP_PinRead (INTRQ);
       // SEGGER_RTT_printf(0, "\nValue of intr read val %d\n", read_val);

        if(1u == read_val){
           // THINKEY_DEBUG_INFO("There is something to read!");
            spi->RxCb(spi->CtxRxCb);
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
    }
    return status;
}

bool ptxPLAT_GPIO_IsRxPending(ptxPLAT_Spi_t *spi) {
    uint32_t read_val = 0u;

    if (NULL != spi)
    {
        //read_val = nrf_drv_gpiote_in_is_set(THINKEY_GPIO_INTR_PIN);
      // read_val = R_BSP_PinRead (GPIO_INTR_PIN);
        read_val = R_BSP_PinRead (INTRQ);

    }
    return read_val;
}

#if 0
void ptxPLAT_SPI_TransferCallback(spi_callback_args_t *p_args)
{
    if((NULL != p_args) && (PTX_PLAT_SPI_CHANNEL == p_args->channel))
    {
        switch(p_args->event)
        {
            case SPI_EVENT_TRANSFER_COMPLETE:
                ptxPLAT_SPI_SetTRxState(&spi_ctx, 1u);
                break;
            default:
                /** Transfer error. */
                ptxPLAT_SPI_SetTRxState(&spi_ctx, 0xFFu);
                break;
        }
    } else
    {
        /** Transfer error. */
        ptxPLAT_SPI_SetTRxState(&spi_ctx, 0xFFu);
    }
}


/*
 * ####################################################################################################################
 * LOCAL FUNCTIONS
 * ####################################################################################################################
 */
static ptxStatus_t ptxPLAT_SPI_SetTRxState(ptxPLAT_Spi_t *spi, uint8_t state)
{
    ptxStatus_t status = ptxStatus_Success;

    if(NULL != spi)
    {
        spi->TransferState = state;
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
    }

    return status;
}

static ptxStatus_t ptxPLAT_SPI_PortClose(ptxPLAT_SpiPort_t *spiPort)
{
    ptxStatus_t status = ptxStatus_Success;

    if(NULL != spiPort)
    {
        /**
         * Close spi port: power down the peripheral.
         */
        ptxPLAT_SpiInstance_t *spi_instance = (ptxPLAT_SpiInstance_t *)spiPort->SpiInstance;
        fsp_err_t r_status = R_SCI_SPI_Close(spi_instance->p_ctrl);
        if(FSP_SUCCESS != r_status)
        {
            status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InternalError);
        }
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
    }

    return status;
}

static ptxStatus_t ptxPLAT_SPI_PortOpen(ptxPLAT_SpiPort_t **spiPort, ptxPLAT_SpiConfigPars_t *spiPars)
{
    ptxStatus_t status = ptxStatus_Success;

    if((NULL != spiPars) && (NULL != spiPort))
    {
        /**
         * It is assumed that the driver (interface api + data structures) for spi port has already been generated.
         * Here we only set the speed.
         * Default configuration structure has to be wrapped and modified to change speed.
         */

        ptxPLAT_SpiInstance_t *spi_instance = (ptxPLAT_SpiInstance_t *)available_spi_port.SpiInstance;

        sci_spi_extended_cfg_t * spi_extended_cfg = (sci_spi_extended_cfg_t *) spi_instance->p_cfg->p_extend;
        R_SCI_SPI_CalculateBitrate(spiPars->IntfSpeed, &spi_extended_cfg->clk_div, false);

        fsp_err_t r_status = R_SCI_SPI_Open(spi_instance->p_ctrl, spi_instance->p_cfg);

        if(FSP_SUCCESS == r_status)
        {
            *spiPort = (ptxPLAT_SpiPort_t *)&available_spi_port;
        } else
        {
            status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InternalError);
        }
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
    }

    return status;
}

static ptxStatus_t ptxPLAT_SPI_TxAux(ptxPLAT_Spi_t *spi, uint8_t *txBuff, size_t *len)
{
    /**
     * All input arguments shall be checked by the caller. Here, it is expected all are valid i.e. not NULL and
     * allocated enough memory for Rx operation.
     *
     * What about setting a safeguard timer before the wait-for-transfer-end loop?
     */
    ptxStatus_t status = ptxStatus_Success;

    ptxPLAT_SpiInstance_t *spi_instance = (ptxPLAT_SpiInstance_t *)spi->SpiPortUsed->SpiInstance;
    uint32_t length = (uint32_t)*len;

    ptxPLAT_SPI_SetTRxState(spi, 0);

    fsp_err_t _st = R_SCI_SPI_Write(spi_instance->p_ctrl, txBuff, length, SPI_BIT_WIDTH_8_BITS);
    if(FSP_SUCCESS != _st)
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InterfaceError);
    }

    if(ptxStatus_Success == status)
    {
        /** Wait until transfer notification is received via callback. */
        while(0 == spi->TransferState)
        {

        }

        if(1u != spi->TransferState)
        {
            status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InterfaceError);
        }
    }

    return status;
}

static ptxStatus_t ptxPLAT_SPI_RxAux(ptxPLAT_Spi_t *spi, uint8_t *rxBuff, size_t *len)
{
    /**
     * All input arguments shall be checked by the caller. Here, it is expected all are valid i.e. not NULL and
     * allocated enough memory for Rx operation.
     *
     * What about setting a safeguard timer before the wait-for-transfer-end loop?
     */
    ptxStatus_t status = ptxStatus_Success;

    ptxPLAT_SpiInstance_t *spi_instance = (ptxPLAT_SpiInstance_t *)spi->SpiPortUsed->SpiInstance;
    uint32_t length = (uint32_t)*len;

    ptxPLAT_SPI_SetTRxState(spi, 0);

    fsp_err_t r_status = R_SCI_SPI_Read(spi_instance->p_ctrl, rxBuff, length, SPI_BIT_WIDTH_8_BITS);
    if(FSP_SUCCESS != r_status)
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InterfaceError);
    }

    if(ptxStatus_Success == status)
    {
        /** Wait until transfer notification is received via callback. */
        while (0 == spi->TransferState)
        {

        }

        if (1u != spi->TransferState)
        {
            status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InterfaceError);
        }
    }

    return status;
}

static ptxStatus_t ptxPLAT_SPI_SetChipSelect(ptxPLAT_SpiPort_t *spiPort, uint8_t newState)
{
    ptxStatus_t status = ptxStatus_Success;

    if((NULL != spiPort) && ((PTX_PLAT_SPI_CS_LOW == newState) || (PTX_PLAT_SPI_CS_HIGH == newState)))
    {
        fsp_err_t r_status = R_IOPORT_PinWrite(spiPort->Nss.PortInstance->p_ctrl, spiPort->Nss.PinNumber, newState);
        if(FSP_SUCCESS != r_status)
        {
            status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InterfaceError);
        }
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
    }

    return status;
}

static ptxStatus_t ptxPLAT_SPI_TRx_Helper(ptxPLAT_Spi_t *spi, uint8_t *txBuf[], size_t txLen[], size_t numTxBuffers, uint8_t *rxBuf[], size_t *rxLen[], size_t numRxBuffers)
{
    ptxStatus_t status = ptxStatus_Success;

    /**
     * Tx operation is required always: to send and to receive anything on SPI. So, tx buffers have to be provided always.
     */

    if ((NULL != spi) && (NULL != spi->SpiPortUsed) && (NULL != txBuf) && (NULL != txLen))
    {
        uint8_t i =0;

        /** At this point the SPI transfer operation is triggered */
        ptxPLAT_SPI_SetChipSelect(spi->SpiPortUsed, PTX_PLAT_SPI_CS_LOW);

        /** Tx part of the overall transaction. */
        i = 0;
        while ((ptxStatus_Success == status) && (i < numTxBuffers))
        {
            if ((txBuf[i] != NULL) && (txLen[i]>0))
            {
                size_t trx_len = txLen[i];
                status = ptxPLAT_SPI_TxAux (spi, txBuf[i], &trx_len);
            } else
            {
                status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
            }

            i++;
        }

        if (ptxStatus_Success == status)
        {
            /** Let's see if there is something to read. */
            if ((NULL != rxBuf) && (NULL != rxLen))
            {
                i = 0;
                while((ptxStatus_Success == status) && (i < numRxBuffers))
                {
                    if ((rxBuf[i] != NULL) && (rxLen[i] != NULL) && (*rxLen[i] > 0))
                    {
                        status = ptxPLAT_SPI_RxAux(spi, rxBuf[i], rxLen[i]);
                    } else
                    {
                        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
                    }
                    i++;
                }
            }
        }

        /** In any case, at this point the SPI transfer operation is finished */
        ptxPLAT_SPI_SetChipSelect(spi->SpiPortUsed, PTX_PLAT_SPI_CS_HIGH);
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_PLAT, ptxStatus_InvalidParameter);
    }

    return status;
}
#endif
