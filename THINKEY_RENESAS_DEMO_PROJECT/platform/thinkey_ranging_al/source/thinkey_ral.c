/*
 * thinkey_ral.c
 *
 *  Created on: Nov 12, 2021
 *      Author: Adarsh
 */
#include "thinkey_osal.h"
#include "thinkey_debug.h"
#include "thinkey_ral.h"



THINKey_pfnRangingCB gpfnRangingCB;
TKey_HANDLE ghRangingSSHandle;


THINKey_BOOL bRangingInitDone = false;

THINKey_eStatusType THINKey_eRangingInit()
{
    THINKey_UINT32 uiStatus = E_THINKEY_FAILURE;
//    THINKEY_DEBUG_INFO("THINKey_eRangingInit Entered\r\n");
//    if(!bRangingInitDone)
//    {
//        init_crc16();
//        peripherals_init();
//        port_init_dw_chip();
//        dw_irq_init();
//        memset(&app,0,sizeof(app));
//        memset(&gDiagPrintFStr, 0, sizeof(gDiagPrintFStr));
//        load_bssConfig();                 /**< load the RAM Configuration parameters from NVM block */
//        app.pConfig = get_pbssConfig();
//        if (app.pConfig->s.uartEn)
//        {
//            deca_uart_init();
//        }
//        /* TODO: LED blicking. Can be removed. Added for initial debugging */
//        for(int i=0; i<6; i++)
//        {
//            bsp_board_led_invert(BSP_BOARD_LED_0);
//            bsp_board_led_invert(BSP_BOARD_LED_1);
//            nrf_delay_ms(250);
//        }
//        /*TODO: Do we need this sleep? */
//        nrf_delay_ms(1000);        /**< small pause to startup */
//        reset_DW3000(); //this will reset DW device
//        if (dwt_probe((struct dwt_probe_s *)&dw3000_probe_interf))
//        {
//            error_handler(1, _ERR_INIT);
//        }
//        /* This initialization is added to avoid crashing the board when calling APIs that writes inside local data
//        like setxtaltrim */
//        if (dwt_initialise(DWT_DW_INIT) != DWT_SUCCESS)
//        {
//            error_handler(1, _ERR_INIT);
//        }
//        /* initialize inter-task communication mail queue for Node :
//        *
//        * The RxTask need to send the rxPckt to the CalcTask.
//        *
//        * TODO: rxPcktPool_q_id should be a part of NodeInfo, but
//        * FreeRTOS cannot free resources efficiently on task deletion.
//        *
//        * Current code has an implementation where NodeInfo is statically defined
//        * and rxPcktPool_q is a part of FreeRtos Heap.
//        *
//        * Note, the debug accumulator & diagnostics readings are a part of
//        * mail queue. Every rx_mail_t has a size of ~6kB.
//        *
//        * */
//        osMailQDef(rxPcktPool_q, RX_MAIL_QUEUE_SIZE, rx_mail_t);
//        app.rxPcktPool_q_id = osMailCreate(osMailQ(rxPcktPool_q), NULL);
//
//        if(!app.rxPcktPool_q_id)
//        {
//            error_handler(1, _ERR_Cannot_Alloc_Mail);
//        }
//        bRangingInitDone = true;
    //}

//    uiStatus = E_THINKEY_SUCCESS;

    return uiStatus;
}

TKey_StatusType_t THINKey_RAL_setUrsk(TKey_BYTE *psUrsk) {
//    printf("THINKey_setUrsk\r\n");
    return E_TKEY_SUCCESS;
}

THINKey_eStatusType THINKey_eRangingStart(THINKey_HANDLE hKeyHandle, THINKey_pfnRangingCB pfnRangingCB)
{
    THINKey_eStatusType eStatus = E_THINKEY_SUCCESS;
//    gpfnRangingCB = pfnRangingCB;
//    ghRangingSSHandle = hKeyHandle;
//    THINKEY_DEBUG_INFO("Enetered THINKey_eRangingStart");
//    node_helper(NULL);
    return eStatus;
}

TKey_StatusType_t THINKey_RangingStop() {
    TKey_StatusType_t eStatus = E_THINKEY_SUCCESS;
//    THINKEY_DEBUG_INFO("Enetered THINKey_RangingStop");
//    node_terminate();
    return eStatus;
}

/* TODO: is_update_uwb_power and is_update_uwb_pdoaoff copied from Murata twr demo for comiplation. May remove afterwards */
/**
 *
 * Byte[3]     Byte[2]     Byte[1]     Byte[0]
 * Reserved    Reserved    PDoAoffset  TXpower/PGDLY/PGCNT
 *
 *
 * vlaue:
 *  1 : Updated by CMD
 *  0 : Not Updated yet
 * */
//bool is_update_uwb_power(void)
//{
//    bool flag = false;
//    /* TODO: Always retrurn false. May remove this function itself later */
//    //flag = (uint8_t)app.pConfig->s.update_uwb_config_flag;
//    return (bool)flag;
//}
//
//bool is_update_uwb_pdoaoff(void)
//{
//    bool flag = false;
//    /* TODO: Always retrurn false. May remove this function itself later */
//    //flag = (uint8_t)(app.pConfig->s.update_uwb_config_flag >> 8);
//    return (bool)flag;
//}
//
///*Function prototype taken from Murata twr demo. This function will be callled when the range data is available */
//void send_to_pc_twr(result_t *pRes)
//{
//    TKey_UINT32 uiDistance= (int)pRes->dist_cm;
//
//    THINKEY_DEBUG_INFO("send_to_pc_twr:dist_cm : %d", uiDistance);
//    gpfnRangingCB(ghRangingSSHandle, NULL, uiDistance, 0);
//}
//
///* Function prototype Take from Murata twr demo */
///* @brief send acc & diagnostics information
// *           these are blocking operations
// *
// * */
//void send_to_pc_diag_acc(rx_mail_t *pRxMailPckt)
//{
//    (TKey_VOID)pRxMailPckt;
//}
//
///* Function prototype Take from Murata twr demo */
///*
// * @brief function to report to PC a new tag was discovered
// *
// * 'JSxxxx{"NewTag":
// *             <string>//address64, string
// *        }'
// *
// * */
//void signal_to_pc_new_tag_discovered(uint64_t addr64)
//{
//    THINKEY_DEBUG_INFO("signal_to_pc_new_tag_discovered: %08lX%08lX", (uint32_t)(addr64>>32), (uint32_t)addr64);
//    param_block_t *pbss = get_pbssConfig();
//    pbss->s.sfConfig.tag_pollTxFinalTx_us = 2900;
//
//
//    tag_addr_slot_t    *tag;
//    uint64_t        *pAddr64;
//    uint16_t        size = getDList_size();
//
//    /* below can be removed if the d2k command, i.e. "add all discovered tags to known list automatically", is not used */
//    uint16_t    tag_mFast = 1;              /**< Used to pass a "moving" refresh rate to all tags in the "d2k" command */
//    uint16_t    tag_mSlow = 2;              /**< Used to pass a "stationary" refresh rate to all tags in the "d2k" command */
//    uint16_t    tag_Mode = 1;               /**< Used to pass a common "mode" parameter to all tags in the "d2k" command */
//
//    pAddr64 = getDList();
//
//    /* TODO: Do we need this while loop, as we are expecting only one Tag. Just kept for now to keep the code same as reference */
//    while( (*pAddr64 != 0) && (size>0) )
//    {
//
//        tag = add_tag_to_knownTagList( *pAddr64, (uint16_t)(*pAddr64), tag_mFast, tag_mSlow, tag_Mode);
//
//        if (!tag)
//        {
//            THINKEY_DEBUG_ERROR("Failed to add UWB tag!");
//            break;
//        }
//        else
//        {
//            THINKEY_DEBUG_ERROR("Succefully added tag");
//            tag->reqUpdatePending = 1;    //enable update of Tag's configuration on its next Poll
//        }
//
//        size--;
//        pAddr64++;
//    }
//    initDList();    //clear Discovered Tag List
//}
