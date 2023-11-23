/** \file
   ---------------------------------------------------------------
   Copyright (C) 2021. Panthronics AG - All Rights Reserved.

   This material may not be reproduced, displayed, modified or
   distributed without the express prior written permission of the
   Panthronics AG.

   PLEASE REFER TO OUR DISCLAIMER NOTICE IN THE PROJECT ROOT.
   ---------------------------------------------------------------

    Project     : PTX100x
    Module      : PTX100x Felica-DTE
    File        : ptxFeliCa_DTE.c

    Description : Collection of functions for FeliCa compliance Tests.
*/

/*
 * ####################################################################################################################
 * INCLUDES
 * ####################################################################################################################
 */
#include "ptxFeliCa_DTE.h"
#ifdef PTX_PRODUCT_TYPE_IOT_READER
    #include "ptx_IOT_READER.h"
#else
   #include "ptxPOS.h"
#endif

#include <string.h>


/*
 * ####################################################################################################################
 * DEFINES / TYPES
 * ####################################################################################################################
 */
#define PTX_FELICA_DTE_TECH     (uint8_t)2

/*
 * ####################################################################################################################
 * INTERNAL FUNCTIONS / HELPERS
 * ####################################################################################################################
 */
static ptxStatus_t ptxFeliCa_DTE_RunPerformanceTest(ptxFeliCa_DTE_t *feliCaDTEComp, ptxFeliCa_DTE_TestParams_t *testParams);
static ptxStatus_t ptxFeliCa_DTE_Data_Exchange(ptxFeliCa_DTE_t *feliCaDTEComp, uint8_t *tx, uint32_t txLength, uint8_t *rx, uint32_t *rxLength);
static ptxStatus_t ptxFeliCa_DTE_HandleCarrier (ptxFeliCa_DTE_t *feliCaDTEComp, uint8_t switchOn);
static ptxStatus_t ptxFeliCa_DTE_SetParams (ptxFeliCa_DTE_t *feliCaDTEComp, uint8_t rfTech, uint8_t rfBitrate, uint32_t timeout);

/*
 * ####################################################################################################################
 * API FUNCTIONS
 * ####################################################################################################################
 */

ptxStatus_t ptxFeliCa_DTE_Init (ptxFeliCa_DTE_t *feliCaDTEComp, ptxFeliCa_DTE_InitParams_t *initParams)
{
    ptxStatus_t status = ptxStatus_Success;

    if ((NULL != feliCaDTEComp) && (NULL != initParams))
    {
#ifdef PTX_PRODUCT_TYPE_IOT_READER
        if ((NULL != initParams->IoTRdComp))
#else
        if ((NULL != initParams->POSComp))
#endif
        {
            /* clear component */
            (void)memset(feliCaDTEComp, 0, sizeof(ptxFeliCa_DTE_t));

            /* set members */
#ifdef PTX_PRODUCT_TYPE_IOT_READER
            feliCaDTEComp->IoTRdComp = initParams->IoTRdComp;
#else
            feliCaDTEComp->POSComp = initParams->POSComp;
#endif

            /* assign Component ID */
            feliCaDTEComp->CompId = ptxStatus_Comp_FELICA_DTE;

        } else
        {
            status = PTX_STATUS(ptxStatus_Comp_FELICA_DTE, ptxStatus_InvalidParameter);
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_FELICA_DTE, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxFeliCa_DTE_Deinit (ptxFeliCa_DTE_t *feliCaDTEComp)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(feliCaDTEComp, ptxStatus_Comp_FELICA_DTE))
    {
        /* nothing to do here */

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_FELICA_DTE, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxFeliCa_DTE_EnableMode (ptxFeliCa_DTE_t *feliCaDTEComp, uint8_t enableMode, ptxFeliCa_DTE_TestParams_t *testParams)
{
    ptxStatus_t status = ptxStatus_Success;

    if (PTX_COMP_CHECK(feliCaDTEComp, ptxStatus_Comp_FELICA_DTE))
    {
        size_t rf_par_len = 0;
        ptxNSC_RfPar_t rf_par[10];

        (void)memset(rf_par, 0, rf_par_len * sizeof(ptxNSC_RfPar_t));

        if (0 != enableMode)
        {
            if (NULL != testParams)
            {
                status = ptxFeliCa_DTE_HandleCarrier(feliCaDTEComp, enableMode);

                if (ptxStatus_Success == status)
                {
                    switch (testParams->ID)
                    {
                        case FELICA_DTE_TestID_PerformanceTest:
                            status = ptxFeliCa_DTE_SetParams(feliCaDTEComp,
                                                             PTX_FELICA_DTE_TECH,
                                                             (uint8_t)testParams->Params.PerformanceTest.Bitrate,
                                                             testParams->Params.PerformanceTest.TimeOutMS);
                            break;

                        default:
                            status = PTX_STATUS(ptxStatus_Comp_FELICA_DTE, ptxStatus_InvalidParameter);
                            break;
                    }
                }

            } else
            {
                status = PTX_STATUS(ptxStatus_Comp_FELICA_DTE, ptxStatus_InvalidParameter);
            }

        } else
        {
            status = ptxFeliCa_DTE_SetParams(feliCaDTEComp, 0, 0, 0);

            if (ptxStatus_Success == status)
            {
                status = ptxFeliCa_DTE_HandleCarrier(feliCaDTEComp, enableMode);
            }
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_FELICA_DTE, ptxStatus_InvalidParameter);
    }

    return status;
}

ptxStatus_t ptxFeliCa_DTE_RunTest(ptxFeliCa_DTE_t *feliCaDTEComp, ptxFeliCa_DTE_TestParams_t *testParams)
{
    ptxStatus_t status = ptxStatus_Success;

    if ((PTX_COMP_CHECK(feliCaDTEComp, ptxStatus_Comp_FELICA_DTE)) && (NULL != testParams))
    {
        switch (testParams->ID)
        {
            case FELICA_DTE_TestID_PerformanceTest:
                status = ptxFeliCa_DTE_RunPerformanceTest(feliCaDTEComp, testParams);
                break;

            default:
                status = PTX_STATUS(ptxStatus_Comp_FELICA_DTE, ptxStatus_InvalidParameter);
                break;
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_FELICA_DTE, ptxStatus_InvalidParameter);
    }

    return status;
}

/*
 * ####################################################################################################################
 * INTERNAL FUNCTIONS / CALLBACK(s)
 * ####################################################################################################################
 */

static ptxStatus_t ptxFeliCa_DTE_RunPerformanceTest(ptxFeliCa_DTE_t *feliCaDTEComp, ptxFeliCa_DTE_TestParams_t *testParams)
{
    const uint32_t RX_BUFFER_LEN = 30;

    /* Note: Length-byte prepended by the chip */
    uint8_t SENSF_REQ[] = {
                            0x00,           // SENSF_REQ Command Code
                            0xFF, 0xFF,     // System Code
                            0x00,           // Request Code
                            0x00            // TSN
                          };

    uint8_t rx_buffer[30];
    uint32_t rx_buffer_size;

    ptxStatus_t status = ptxStatus_Success;

    ptxFeliCa_DTE_TestProgressParams_t progress_params;
    progress_params.ExitProcessing = 0;
    progress_params.NrTestsProcessed = 0;

    if ((PTX_COMP_CHECK(feliCaDTEComp, ptxStatus_Comp_FELICA_DTE)) && (NULL != testParams))
    {
        if ((testParams->Params.PerformanceTest.NrTests <= testParams->Params.PerformanceTest.ResultBufferSize) &&
            (NULL != (testParams->Params.PerformanceTest.ResultBuffer)))
        {
            for (uint8_t i = 0; i < testParams->Params.PerformanceTest.NrTests; i++)
            {
                (void)memset(&rx_buffer[0], 0, RX_BUFFER_LEN);
                rx_buffer_size = RX_BUFFER_LEN;
                status = ptxFeliCa_DTE_Data_Exchange(feliCaDTEComp,
                                                     &SENSF_REQ[0],
                                                     (uint32_t)sizeof(SENSF_REQ),
                                                     &rx_buffer[0],
                                                     &rx_buffer_size);

                progress_params.NrTestsProcessed++;

                if (ptxStatus_Success == status)
                {
                    /* Test OK */
                    testParams->Params.PerformanceTest.ResultBuffer[i] = 'O';

                } else if ((ptxStatus_NscRfError == PTX_GET_STATUS(status)) || (ptxStatus_TimeOut == PTX_GET_STATUS(status)))
                {
                    /* Test FAIL */
                    testParams->Params.PerformanceTest.ResultBuffer[i] = '.';

                    /* reset status */
                    status = ptxStatus_Success;
                }
                else
                {
                    /* cancel test */
                    break;
                }

                /* inform upper layer about progress and provide option to quit (both optional) */
                if (NULL != testParams->ProgressCB)
                {
                    testParams->ProgressCB(&progress_params);
                }

                if (0 != progress_params.ExitProcessing)
                {
                    /* cancel test */
                    break;
                }
            }
        } else
        {
            status = PTX_STATUS(ptxStatus_Comp_FELICA_DTE, ptxStatus_InvalidParameter);
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_FELICA_DTE, ptxStatus_InvalidParameter);
    }

    return status;
}

static ptxStatus_t ptxFeliCa_DTE_Data_Exchange(ptxFeliCa_DTE_t *feliCaDTEComp, uint8_t *tx, uint32_t txLength, uint8_t *rx, uint32_t *rxLength)
{
    ptxStatus_t status = ptxStatus_Success;

    /* use large enough value - must exceed test timeout value set within ptxFeliCa_DTE_EnableMode */
    uint32_t timeout_ms = (uint32_t)50000;

    if ((PTX_COMP_CHECK(feliCaDTEComp, ptxStatus_Comp_FELICA_DTE)) && (NULL != tx) && (0 != txLength) && (NULL != rxLength))
    {
#ifdef PTX_PRODUCT_TYPE_IOT_READER
        status = ptxIoTRd_Data_Exchange(feliCaDTEComp->IoTRdComp, &tx[0], txLength, &rx[0], rxLength, timeout_ms);
#else
        status = ptxPOS_Data_Exchange(feliCaDTEComp->POSComp, &tx[0], txLength, &rx[0], rxLength, timeout_ms);
#endif
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_FELICA_DTE, ptxStatus_InvalidParameter);
    }

    return status;
}

static ptxStatus_t ptxFeliCa_DTE_HandleCarrier (ptxFeliCa_DTE_t *feliCaDTEComp, uint8_t switchOn)
{
    ptxStatus_t status = ptxStatus_Success;

    if (NULL != feliCaDTEComp)
    {
        if (0 != switchOn)
        {
#ifdef PTX_PRODUCT_TYPE_IOT_READER
            ptxIoTRd_DiscConfig_t disc_cfg = {0};
            disc_cfg.ContinuousField = (uint8_t)1;

            status = ptxIoTRd_Initiate_Discovery (feliCaDTEComp->IoTRdComp, &disc_cfg);
#else
            ptxPOS_PollConfig_t poll_cfg = {0};
            poll_cfg.ContinuousField = (uint8_t)1;

            status = ptxPOS_Initiate_Polling (feliCaDTEComp->POSComp, &poll_cfg);
#endif
        } else
        {
#ifdef PTX_PRODUCT_TYPE_IOT_READER
            status = ptxIoTRd_Reader_Deactivation(feliCaDTEComp->IoTRdComp, PTX_IOTRD_RF_DEACTIVATION_TYPE_IDLE);
#else
            status = ptxPOS_Reader_Deactivation (feliCaDTEComp->POSComp, 0);
#endif
        }
    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_FELICA_DTE, ptxStatus_InvalidParameter);
    }

    return status;
}

static ptxStatus_t ptxFeliCa_DTE_SetParams (ptxFeliCa_DTE_t *feliCaDTEComp, uint8_t rfTech, uint8_t rfBitrate, uint32_t timeout)
{
    /*
     * Internal uses a resolution of 128/fc per tick ~ 9,44us; 1 ms is roughly 106 ticks (~1.00059ms)
     * Note: The resolution is limited to 24-bits i.e. the max. value is roughly 158 s.
     */
    const uint32_t BASE_TIMEOUT_1_MS = (uint32_t)106;

    ptxStatus_t status = ptxStatus_Success;

    if (NULL != feliCaDTEComp)
    {
        size_t rf_par_len = 0;
        ptxNSC_RfPar_t rf_par[10];

        (void)memset(rf_par, 0, rf_par_len * sizeof(ptxNSC_RfPar_t));

        /* RF-Technology (default) */
        rf_par[rf_par_len].ParmId = RfParameter_Rf_Tech;
        rf_par[rf_par_len].Parm.RfTech.RfTech = rfTech;
        rf_par_len++;

        /* FWT (default)*/
        uint32_t timeout_val = (uint32_t)((BASE_TIMEOUT_1_MS * timeout) & 0x00FFFFFF);
        rf_par[rf_par_len].ParmId = RfParameter_Fwt;
        rf_par[rf_par_len].Parm.Fwt.Fwt[0] = (uint8_t)((timeout_val >> 16) & (uint8_t)0xFF);
        rf_par[rf_par_len].Parm.Fwt.Fwt[1] = (uint8_t)((timeout_val >> 8)  & (uint8_t)0xFF);
        rf_par[rf_par_len].Parm.Fwt.Fwt[2] = (uint8_t)((timeout_val >> 0)  & (uint8_t)0xFF);
        rf_par_len++;

        /* TX-Bitrate */
        rf_par[rf_par_len].ParmId = RfParameter_Tx_Bit_Rate;
        rf_par[rf_par_len].Parm.TxBitRate.TxBitRate = rfBitrate;
        rf_par_len++;

        /* RX-Bitrate */
        rf_par[rf_par_len].ParmId = RfParameter_Rx_Bit_Rate;
        rf_par[rf_par_len].Parm.RxBitRate.RxBitRate = rfBitrate;
        rf_par_len++;

        /* TX-CRC */
        rf_par[rf_par_len].ParmId = RfParameter_Tx_CRC;
        rf_par[rf_par_len].Parm.TxCRC.TxCRC = (uint8_t)1;
        rf_par_len++;

        /* RX-CRC */
        rf_par[rf_par_len].ParmId = RfParameter_Rx_CRC;
        rf_par[rf_par_len].Parm.RxCRC.RxCRC = (uint8_t)1;
        rf_par_len++;

        /* RES_LIMIT */
        rf_par[rf_par_len].ParmId = RfParameter_Res_Limit;
        rf_par[rf_par_len].Parm.ResLimit.ResLimit = (uint8_t)1;
        rf_par_len++;

        if (ptxStatus_Success == status)
        {
#ifdef PTX_PRODUCT_TYPE_IOT_READER
                status = ptxNSC_RfSetParams(feliCaDTEComp->IoTRdComp->Nsc, rf_par, rf_par_len);
#else
                status = ptxNSC_RfSetParams(feliCaDTEComp->POSComp->Nsc, rf_par, rf_par_len);
#endif
        }

    } else
    {
        status = PTX_STATUS(ptxStatus_Comp_FELICA_DTE, ptxStatus_InvalidParameter);
    }

    return status;
}

