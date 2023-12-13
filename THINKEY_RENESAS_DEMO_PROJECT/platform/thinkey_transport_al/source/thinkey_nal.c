
#include "thinkey_nal.h"
#include "thinkey_debug.h"
#include "thinkey_osal.h"
#include "thinkey_platform_types.h"

#include "ptx_IOT_READER.h"
#include "ptxPLAT.h"


//#define TAB_UI_DEMO

#ifdef TAB_UI_DEMO
#include "thinkey_tab_app.h"
#endif
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
//#include "nrf_spi.h"

#define THINKEY_NFC_DETECT_TASK_NAME   "NFC Detect Task"
#define THINKEY_DETECT_TASK_PRIORITY   3
#define THINKEY_DETECT_TASK_STACK_SIZE 2048
#define THINKEY_MAX_NFC_KEYS           2

#define MAX_NFC_DATA_SIZE 256

#define PTX_IOTRD_NO_CARD_SLEEP_TIME       5u
#define PTX_IOTRD_DATA_EXCHANGE_SLEEP_TIME 1000u
#define PTX_IOTRD_DATA_EXCHANGE_TIMEOUT    5000
#define APP_INTERFACE_SPEED_1000000        (1000000UL)

typedef enum {
    E_PTX_STATE_WAIT_FOR_ACTIVATION,
    E_PTX_STATE_DATA_EXCHANGE,
    E_PTX_STATE_DEACTIVATE_READER,
    E_PTX_STATE_SYSTEM_ERROR,
} TKey_DiscoveryStates;

typedef struct {
    TKey_UINT32     uiKeyId;
} sNfcKeyHandleType;

typedef struct {
    TKey_Handle       hNfcTpHandle;
    TKey_BOOL         bDiscovering;
    sNfcCallbackType* psNfcCallbacks;
    sNfcKeyHandleType asKeyHandle[THINKEY_MAX_NFC_KEYS];

    ptxIoTRd_t        sIotRd;

    TKey_DiscoveryStates eDiscoveryState;
} TKey_NalHandleType;

TKey_NalHandleType sNalHandle;


TKey_VOID tkey_NotifyDisconnection(TKey_NalHandleType* psNalHandle);
TKey_VOID tkey_Nfc_DetectTask(TKey_VOID* pvTaskParam);
static void ptxIoTRdInt_Print_Revision_Info(ptxIoTRd_t *iotRd);
static TKey_VOID tkey_DiscoveryHandler(TKey_NalHandleType* psNalHandle,
        uint8_t discover_status, ptxIoTRd_CardRegistry_t* card_registry);

/*void printbuf(uint8_t *buf, unsigned int len) {
    printf("\r\n size: %d\r\n", len);
    for(int i=0; i < len; i++)
        printf("0x%x ", buf[i]);
    printf("\r\n");
}*/


static TKey_NalHandleType* tkey_AllocateNalHandle() {

    return &sNalHandle;
}

TKey_Handle tkey_NAL_NfcStackInit(TKey_Handle hNfcTpHandle,
        sNfcCallbackType *psCallbacks, TKey_Transport_t eTransport) {
    TKey_NalHandleType* psNalHandle;
    TKey_Handle hNalRetHandle = TKey_NULL;

    ptxStatus_t st = ptxStatus_Success;
    uint8_t start_temperature_sensor_calibration = 1;

    psNalHandle = tkey_AllocateNalHandle();
    psNalHandle->hNfcTpHandle = hNfcTpHandle;
    psNalHandle->psNfcCallbacks = psCallbacks;

    do {
        ptxIoTRd_InitPars_t initParams;
        ptxIoTRd_TempSense_Params_t tempSens;
        ptxIoTRd_ComInterface_Params_t comIntf;

        (void)memset(&initParams, 0, sizeof(ptxIoTRd_InitPars_t));
        (void)memset(&tempSens, 0, sizeof(ptxIoTRd_TempSense_Params_t));
        (void)memset(&comIntf, 0, sizeof(ptxIoTRd_ComInterface_Params_t));

        /* Define communication interface settings: speed and flow control. Flow control is relevant only for Uart. */
       // comIntf.Speed = NRF_SPI_FREQ_1M;
        comIntf.Speed = 1;
        comIntf.FlowControl = 0;

        if(1u == start_temperature_sensor_calibration)
        {
            /* Calibration required. Set ambient temperature and expected shutdown temperature threshold. */
            tempSens.Calibrate = 1;
            tempSens.Tambient = 25;
            tempSens.Tshutdown = 100;

            /* Calibration will take place now. Doesn´t need to be done anymore if ptxPOS_Init is successful. */
            start_temperature_sensor_calibration = 0;
        } else
        {
            /* Calibration not needed, it has already been done. Provide compensated threshold temperature word. */
            tempSens.Calibrate = 0;
            tempSens.Tambient = 25;
            tempSens.Tshutdown = 223u;
        }

        /* Initial parameters for temperature sensor are ready. */
        initParams.TemperatureSensor = &tempSens;
        initParams.ComInterface = &comIntf;

        /* Initiate IoT-Reader System. */
        st = ptxIoTRd_Init(&psNalHandle->sIotRd, &initParams);
        if (ptxStatus_Success == st) {
            THINKEY_DEBUG_INFO("System Initialization ... OK\n");

            /* Print available revisions */
            ptxIoTRdInt_Print_Revision_Info(&psNalHandle->sIotRd);
        } else {
            printf("\n System Initialization failed: %d",st);
            break; 
        }
        
        psNalHandle->bDiscovering = 0;

        hNalRetHandle = psNalHandle;
    } while(TKey_EXIT);

    return hNalRetHandle;
}

TKey_Status_t tkey_NAL_NfcStackDeInit(TKey_Handle hNalHandle, TKey_Transport_t eTransport) {
    TKey_Status_t eRetStatus = E_TKEY_FAILURE;
    TKey_NalHandleType* psNalHandle = (TKey_NalHandleType*)hNalHandle;
    ptxStatus_t st = ptxStatus_Success;
    /* Deinitialize ptx */
    THINKEY_DEBUG_INFO("Calling ptx Denit API\n");
    st = ptxIoTRd_Deinit(&psNalHandle->sIotRd);
    if(ptxStatus_Success != st) {
        THINKEY_DEBUG_INFO(" Deinitalization failed\n");
    } else {
        eRetStatus = E_TKEY_SUCCESS;
        THINKEY_DEBUG_INFO(" Deinitalization Success\n");
    }
    return eRetStatus;
}

TKey_StatusType	tkey_NAL_StartDiscovery(TKey_Handle hNalHandle) {
    TKey_NalHandleType* psNalHandle = (TKey_NalHandleType*)hNalHandle;
    TKey_UINT32 uiTaskID;
    TKey_StatusType eRetStatus = E_TKEY_FAILURE;

    ptxIoTRd_DiscConfig_t rf_disc_config;
    ptxStatus_t st = ptxStatus_Success;

    (void)memset(&rf_disc_config, 0, sizeof(ptxIoTRd_DiscConfig_t));

    do {
        /* Start discovery */
        rf_disc_config.PollTypeA    = 1u;
        rf_disc_config.IdleTime     = 100u;
        /* Do not start discovery if already ongoing */
        if(psNalHandle->bDiscovering) {
           break; 
        }

        st = ptxIoTRd_Initiate_Discovery (&psNalHandle->sIotRd, &rf_disc_config);
        if(ptxStatus_Success != st) {
            THINKEY_DEBUG_ERROR("Start Discovery failed");
            break;
        } else {
            eRetStatus = E_TKEY_SUCCESS;
            psNalHandle->bDiscovering = 1;
        }

        /* Start thread to listen to detect card */
        eRetStatus = THINKey_OSAL_eCreateTask(THINKEY_NFC_DETECT_TASK_NAME, &tkey_Nfc_DetectTask,
                hNalHandle, THINKEY_DETECT_TASK_PRIORITY, THINKEY_DETECT_TASK_STACK_SIZE,
                &uiTaskID);
        /* Store task ID or task handle in NalHandle */
    } while(TKey_EXIT);

    return eRetStatus;
}

TKey_StatusType tkey_NAL_StopDiscovery(TKey_Handle hNalHandle) {
    TKey_StatusType eRetStatus = E_TKEY_FAILURE;
    TKey_NalHandleType* psNalHandle = (TKey_NalHandleType*)hNalHandle;

    psNalHandle->bDiscovering = 0;

    //edited :

    ptxStatus_t st = ptxStatus_Success;

    st = ptxIoTRd_Reader_Deactivation(&psNalHandle->sIotRd,
                                      PTX_IOTRD_RF_DEACTIVATION_TYPE_IDLE);
                        THINKEY_DEBUG_INFO("Deactivating Reader %d", st);
                        if (ptxStatus_Success == st)
                        {
                            THINKEY_DEBUG_INFO("Deactivation success");
                        }
                        THINKEY_DEBUG_INFO("Waiting for discovered Cards ...\n");
                        psNalHandle->eDiscoveryState = E_PTX_STATE_WAIT_FOR_ACTIVATION;

     // edited end :
    /* TODO: Delete task */

    return eRetStatus;
}

TKey_StatusType tkey_NAL_SendData(TKey_Handle hNalHandle,
        TKey_Handle hKeyHandle, TKey_BYTE* pucDataBuffer,
        TKey_UINT32 uiLength) {
    TKey_StatusType eRetStatus = E_TKEY_FAILURE;
    TKey_NalHandleType* psNalHandle = (TKey_NalHandleType*)hNalHandle;
    TKey_BYTE acReadBuffer[MAX_NFC_DATA_SIZE];
    TKey_UINT32 uiRxLen = MAX_NFC_DATA_SIZE;
    ptxStatus_t st = ptxStatus_Success;

    THINKEY_DEBUG_INFO("NAL Send Data called");
    printbuf(pucDataBuffer, uiLength);

    if(TKey_NULL != psNalHandle && TKey_NULL != pucDataBuffer) {

        st = ptxIoTRd_Data_Exchange(&psNalHandle->sIotRd, pucDataBuffer, uiLength,
                &acReadBuffer[0], &uiRxLen, PTX_IOTRD_DATA_EXCHANGE_TIMEOUT);
        if(st == ptxStatus_Success) {

            eRetStatus = E_TKEY_SUCCESS;
            if((uiRxLen > 0) &&
                    (NULL != psNalHandle->psNfcCallbacks->TKey_pfnNfcDataReceived)) {
                psNalHandle->psNfcCallbacks->TKey_pfnNfcDataReceived(
                        hKeyHandle, psNalHandle, acReadBuffer, uiRxLen);
                printbuf(acReadBuffer, uiRxLen);
            }
        }
    }

    return eRetStatus;
}


TKey_Status_t tkey_NAL_DeactivateDevice(TKey_Handle_t hNalHandle,
        TKey_Handle_t hKeyHandle) {

    TKey_StatusType eRetStatus = E_TKEY_FAILURE;

    TKey_NalHandleType* psNalHandle = (TKey_NalHandleType*)hNalHandle;
    ptxStatus_t st = ptxStatus_Success;

    st = ptxIoTRd_Reader_Deactivation(&psNalHandle->sIotRd,
            PTX_IOTRD_RF_DEACTIVATION_TYPE_NO_RF_RESET);
    THINKEY_DEBUG_INFO("Deactivating Reader %d", st);
    if(ptxStatus_Success == st) {
        THINKEY_DEBUG_INFO("Deactivation success");
        eRetStatus = E_TKEY_SUCCESS;
    }
    return eRetStatus;
}

/*TODO: This handles connection disconnection serially (only 1 for now). 
 * Need to handle multiple connection and disconnection */
TKey_VOID tkey_Nfc_DetectTask(TKey_VOID* pvTaskParam) {

    TKey_NalHandleType* psNalHandle = (TKey_NalHandleType*)pvTaskParam;
    ptxStatus_t st = ptxStatus_Success;
    TKey_BOOL bExitLoop = TKey_FALSE;    
    uint8_t system_state = PTX_SYSTEM_STATUS_OK;
    uint8_t last_rf_error = PTX_RF_ERROR_NTF_CODE_NO_ERROR;
    uint8_t discover_status = RF_DISCOVER_STATUS_NO_CARD;
    ptxIoTRd_CardRegistry_t *card_registry = NULL;

    psNalHandle->eDiscoveryState = E_PTX_STATE_WAIT_FOR_ACTIVATION;

    THINKEY_DEBUG_INFO("NFC detect task");
    /* get reference to the internal card registry */
    (void)ptxIoTRd_Get_Card_Registry (&psNalHandle->sIotRd, &card_registry);

    if (NULL == card_registry)
    {
        bExitLoop = TKey_TRUE;
    }

    while(psNalHandle->bDiscovering && (TKey_FALSE == bExitLoop)) {

        /* check regularly for critical system errors */
        st = ptxIoTRd_Get_Status_Info (&psNalHandle->sIotRd,
                StatusType_System, &system_state);

        if (PTX_SYSTEM_STATUS_OK != system_state)
        {
            /* Handle system-error */
            psNalHandle->eDiscoveryState = E_PTX_STATE_SYSTEM_ERROR;
        } else
        {
            if (ptxStatus_Success != st)
            {
                THINKEY_DEBUG_ERROR("\r\nSystem error! Deactivating\n");
                psNalHandle->eDiscoveryState = E_PTX_STATE_DEACTIVATE_READER;
            }
        }

        /* check optionally if PA current-limiter got activated */
        (void)ptxIoTRd_Get_Status_Info (&psNalHandle->sIotRd, StatusType_LastRFError, &last_rf_error);
        if (PTX_RF_ERROR_NTF_CODE_WARNING_PA_OVERCURRENT_LIMIT == last_rf_error)
        {
            THINKEY_DEBUG_INFO("Warning - PA Overcurrent Limiter activated!\n");
        }
        switch(psNalHandle->eDiscoveryState) {
            case E_PTX_STATE_WAIT_FOR_ACTIVATION:
                {
                    /* check the state of the ongoing discovery */
                    st = ptxIoTRd_Get_Status_Info (&psNalHandle->sIotRd,
                            StatusType_Discover, &discover_status);
                    if(ptxStatus_Success != st)
                    {
                        psNalHandle->eDiscoveryState = E_PTX_STATE_DEACTIVATE_READER;
                    } else
                    {
                        tkey_DiscoveryHandler(psNalHandle,
                                discover_status, card_registry);
                        if(discover_status == RF_DISCOVER_STATUS_CARD_ACTIVE) {
                            psNalHandle->eDiscoveryState = E_PTX_STATE_DATA_EXCHANGE;
                        }
                    }
                    break;
                }
            case E_PTX_STATE_DATA_EXCHANGE:
                {
                    ptxPLAT_Sleep(psNalHandle->sIotRd.Plat,
                            PTX_IOTRD_DATA_EXCHANGE_SLEEP_TIME);

                    st = ptxIoTRd_RF_PresenceCheck(&psNalHandle->sIotRd, PresCheck_B);
                    THINKEY_DEBUG_INFO("Card status %d\n", st);
                    if(ptxStatus_Success != st)
                    {
                        THINKEY_DEBUG_INFO("Card Disconnected!!!\n");
                        tkey_NotifyDisconnection(psNalHandle);
                        THINKEY_DEBUG_INFO("Callback returned!!!\n");
                        psNalHandle->eDiscoveryState = E_PTX_STATE_DEACTIVATE_READER;
                    }
                    break;
                }
            case E_PTX_STATE_DEACTIVATE_READER:
                {
                    st = ptxIoTRd_Reader_Deactivation(&psNalHandle->sIotRd,
                            PTX_IOTRD_RF_DEACTIVATION_TYPE_DISCOVER);
                    THINKEY_DEBUG_INFO("Deactivating Reader %d", st);
                    if (ptxStatus_Success == st)
                    {
                        THINKEY_DEBUG_INFO("Deactivation success");
                    }
                    THINKEY_DEBUG_INFO("Waiting for discovered Cards ...\n");
                    psNalHandle->eDiscoveryState = E_PTX_STATE_WAIT_FOR_ACTIVATION;
                    break;
                }
            case E_PTX_STATE_SYSTEM_ERROR:
                {
                    bExitLoop = TKey_TRUE;
                    THINKEY_DEBUG_INFO("E_PTX_STATE_SYSTEM_ERROR \n");
                    /* reset the system and quit*/
                    (void)ptxIoTRd_SWReset(&psNalHandle->sIotRd);
                    break;
                }
            default:
                {
                    break;
                }
        }
        /* If this delay was not present, always this task was running. And BLE functionality
           was getting affected */
        vTaskDelay(1);
    }

    THINKEY_DEBUG_INFO("Exiting Card detection task\n");

    vTaskDelete( NULL );    
    /*TODO: Exit task gracefully */

    return;
}

TKey_VOID tkey_NotifyDisconnection(TKey_NalHandleType* psNalHandle) {

    TKey_Handle hKeyHandle = &psNalHandle->sIotRd;

    /* Device connected callback */
    if(psNalHandle->psNfcCallbacks->TKey_pfnNfcKeyRemoved) {
        THINKEY_DEBUG_INFO("calling disconnection callback");
        psNalHandle->psNfcCallbacks->TKey_pfnNfcKeyRemoved(hKeyHandle,
                psNalHandle->hNfcTpHandle);
    }
}

TKey_VOID tkey_DiscoveryHandler(TKey_NalHandleType* psNalHandle,
        uint8_t discover_status, ptxIoTRd_CardRegistry_t* card_registry) {

    ptxIoTRd_t* psIotRd = &psNalHandle->sIotRd;
    TKey_Handle hKeyHandle = TKey_NULL;

    if(NULL != psIotRd && NULL != card_registry) {
        switch(discover_status) {
            case RF_DISCOVER_STATUS_NO_CARD:
                {
                    ptxPLAT_Sleep(psIotRd->Plat, PTX_IOTRD_NO_CARD_SLEEP_TIME);
                    break;
                }
            case RF_DISCOVER_STATUS_CARD_ACTIVE:
                {
                    THINKEY_DEBUG_INFO("card detected!!!");
                    if((card_registry->ActiveCard->TechType == Tech_TypeA) &&
                            (card_registry->ActiveCardProtType == Prot_ISODEP)) {
                        TKey_UINT32 uiKeyId = 0;
                        uiKeyId = 
                            (TKey_UINT32)*card_registry->ActiveCard->TechParams.CardAParams.NFCID1;
                        psNalHandle->asKeyHandle[1].uiKeyId = uiKeyId;
                        hKeyHandle = &psNalHandle->sIotRd;
#ifdef TAB_UI_DEMO
                        /*TODO: This is for UI on Tab. The below code is purely
                          for demo prupose and shall be disabled for production
                         */
                        TKey_StatusType eResult = E_TKEY_FAILURE;
                        sTabAppMessageType sTabAppMessage;
                        sTabAppMessage.eEvent = E_TAB_NFC_KEY_DETECTED;
                        sTabAppMessage.aiData[0] = E_TAB_NFC_KEY_DETECTED;
                        THINKey_sTabAppParamType *psTabTaskParams = hGetTabAppHandle();
                        eResult = THINKey_OSAL_eQueueSend
                            (psTabTaskParams->vTabAppQueue, &sTabAppMessage);
                        if(E_TKEY_SUCCESS == eResult)
                        {
                            THINKEY_DEBUG_INFO ("Send to tab app success\r\n");
                        } else {
                            THINKEY_DEBUG_INFO ("Send to tab app Failed %x\r\n", psTabTaskParams->vTabAppQueue);
                        }
#endif
                        /* Device connected callback */
                        if(psNalHandle->psNfcCallbacks->TKey_pfnNfcKeyFound) {
                            THINKEY_DEBUG_INFO("calling callback");
                            psNalHandle->psNfcCallbacks->TKey_pfnNfcKeyFound(hKeyHandle, 
                                    uiKeyId, psNalHandle->hNfcTpHandle);
                        }

                    } else {
                        THINKEY_DEBUG_INFO(" Irrelevant card detected");
                    }

                    break;
                }
                /* TODO: Handle multi card detection */
            default:
                {
                    break;
                }
        }
    }
}


static void ptxIoTRdInt_Print_Revision_Info(ptxIoTRd_t *iotRd)
{
    ptxStatus_t st = ptxStatus_Success;
    uint32_t rev_info;

    THINKEY_DEBUG_INFO ("\n");
    THINKEY_DEBUG_INFO ("Print Revision Information...\n\n");

    /* get C-Stack Revision */
    st = ptxIoTRd_Get_Revision_Info(iotRd, RevInfo_C_Stack, &rev_info);
    if (ptxStatus_Success == st)
    {
        THINKEY_DEBUG_INFO ("C-Stack Revision.......: 0x%04X\n", rev_info);
    }

    /* any local modifications ? */
    st = ptxIoTRd_Get_Revision_Info(iotRd, RevInfo_Local_Changes, &rev_info);
    if (ptxStatus_Success == st)
    {
        THINKEY_DEBUG_INFO ("Local Modifications....: 0x%04X\n", rev_info);
    }

    /* get NSC (DFY)-Code Revision */
    st = ptxIoTRd_Get_Revision_Info(iotRd, RevInfo_DFY_Code, &rev_info);
    if (ptxStatus_Success == st)
    {
        THINKEY_DEBUG_INFO ("NSC-Code Revision......: %04d\n", rev_info);
    }

    /* get NSC (DFY)-Toolchain Revision */
    st = ptxIoTRd_Get_Revision_Info(iotRd, RevInfo_DFY_Toolchain, &rev_info);
    if (ptxStatus_Success == st)
    {
        THINKEY_DEBUG_INFO ("NSC-Toolchain Revision.: %04d\n", rev_info);
    }

    /* get Chip-ID/-revision */
    st = ptxIoTRd_Get_Revision_Info(iotRd, RevInfo_ChipID, &rev_info);
    if (ptxStatus_Success == st)
    {
        THINKEY_DEBUG_INFO ("Chip-ID................: 0x%02X\n", rev_info);
    }

    /* get Product-ID/-revision */
    st = ptxIoTRd_Get_Revision_Info(iotRd, RevInfo_ProductID, &rev_info);
    if (ptxStatus_Success == st)
    {
        switch ((uint8_t)rev_info)
        {
            case PTX_HW_PRODUCT_ID_PTX100X:
                THINKEY_DEBUG_INFO ("Product-ID.............: 0x%02X (PTX100x)\n", rev_info);
                break;

            case PTX_HW_PRODUCT_ID_PTX105X:
                THINKEY_DEBUG_INFO ("Product-ID.............: 0x%02X (PTX105x)\n", rev_info);
                break;

            case PTX_HW_PRODUCT_ID_PTX130X:
                THINKEY_DEBUG_INFO ("Product-ID.............: 0x%02X (PTX130x)\n", rev_info);
                break;

            default:
                THINKEY_DEBUG_INFO ("Product-ID.............: Unknown\n");
                break;
        }
    }

    THINKEY_DEBUG_INFO ("\n");

    if (ptxStatus_Success == st)
    {
        THINKEY_DEBUG_ERROR ("Print Revision Information...OK\n");
    }
    else
    {
        THINKEY_DEBUG_INFO ("Print Revision Information...FAILED (Internal Error)\n");
    }
}
