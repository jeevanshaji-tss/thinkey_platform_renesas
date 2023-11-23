#include "hal_data.h"
#include "bsp_pin_cfg.h"
#include "r_ioport.h"
#include "thinkey_nal.h"
#include "thinkey_debug.h"
#include "thinkey_osal.h"

#include "ptx_IOT_READER.h"
#include "ptxPLAT.h"
#include "thinkey_platform_types.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"


#define THINKEY_MAX_NFC_KEYS           2


void R_BSP_WarmStart(bsp_warm_start_event_t event);

typedef struct {
    TKey_UINT32     uiKeyId;
} sNfcKeyHandleType;

typedef enum {
    E_PTX_STATE_WAIT_FOR_ACTIVATION,
    E_PTX_STATE_DATA_EXCHANGE,
    E_PTX_STATE_DEACTIVATE_READER,
    E_PTX_STATE_SYSTEM_ERROR,
} TKey_DiscoveryStates;


typedef struct {
    TKey_Handle       hNfcTpHandle;
    TKey_BOOL         bDiscovering;
    sNfcCallbackType* psNfcCallbacks;
    sNfcKeyHandleType asKeyHandle[THINKEY_MAX_NFC_KEYS];

    ptxIoTRd_t        sIotRd;

    TKey_DiscoveryStates eDiscoveryState;
} TKey_NalHandleType;

//TKey_NalHandleType sNalHandle;
//static TKey_NalHandleType* tkey_AllocateNalHandle() {
//
//    return &sNalHandle;
//}


static volatile spi_event_t g_master_event_flag;
/*******************************************************************************************************************//**

 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is

 * called by main() when no RTOS is used.
 **********************************************************************************************************************/


void hal_entry(void) {
    /* TODO: add your own code here */

//    TKey_NalHandleType* psNalHandle;
//    ptxStatus_t st = ptxStatus_Success;
//
//    psNalHandle = tkey_AllocateNalHandle();
//
//    ptxIoTRd_InitPars_t initParams;
//    ptxIoTRd_TempSense_Params_t tempSens;
//    ptxIoTRd_ComInterface_Params_t comIntf;
//
//    (void)memset(&initParams, 0, sizeof(ptxIoTRd_InitPars_t));
//    (void)memset(&tempSens, 0, sizeof(ptxIoTRd_TempSense_Params_t));
//    (void)memset(&comIntf, 0, sizeof(ptxIoTRd_ComInterface_Params_t));
//
//    comIntf.Speed = 1;
//    comIntf.FlowControl = 0;
//
//    tempSens.Calibrate = 1;
//    tempSens.Tambient = 25;
//    tempSens.Tshutdown = 100;
//
//
//    initParams.TemperatureSensor = &tempSens;
//    initParams.ComInterface = &comIntf;
//
//
//    st = ptxIoTRd_Init(&psNalHandle->sIotRd, &initParams);
//
//
//
//
//    if (ptxStatus_Success == st) {
//              //  THINKEY_DEBUG_INFO("System Initialization ... OK\n");
//
//                /* Print available revisions */
//      } else {
//               // printf("\n System Initialization failed: %d",st);
//
//            }


}


/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event) {
    if (BSP_WARM_START_POST_C == event) {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);
    }
}
