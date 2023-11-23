

/*
 * \file thinkey_debug.c
 *
 * \brief file for debug print implementation
 *
 * Copyright (C) 2021-2022, ThinkSeed Systems Private Limited.
 * All Rights Reserved.
 */

#include "uart_debug.h"
#include "thinkey_platform_types.h"
#include "thinkey_tab_app.h"
#include "thinkey_osal.h"
#include "thinkey_debug.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define TAB_APP_MSG_SIZE  64
#define TAB_APP_MSG_BUFFER_COUNT 5

static TKey_UINT32 guiMsgBufferIndex = 0;
TKey_CHAR gcMsgBuffer[TAB_APP_MSG_BUFFER_COUNT][TAB_APP_MSG_SIZE];

/* Initialisation method for debug prints */
THINKey_eStatusType THINKey_DEBUGInit(THINKey_VOID)
{
    //task_debug_init();
    return E_THINKEY_SUCCESS;
}

TKey_VOID TKey_Debug_Tab_App_Send_Status_Message(TKey_CHAR* pcStringPtr, ...)
{
    sTabAppMessageType sTabAppMessage;
    sTabAppMessage.eEvent = E_TAB_APP_SEND_STATUS_MSG;
    THINKey_eStatusType eResult;
    
    
    va_list args;
    va_start(args, pcStringPtr);
    memset(gcMsgBuffer[guiMsgBufferIndex], 0, TAB_APP_MSG_SIZE);
    vsnprintf(gcMsgBuffer[guiMsgBufferIndex], TAB_APP_MSG_SIZE, pcStringPtr, args);
    va_end(args);
    sTabAppMessage.aiData[0] = (TKey_INT32)&gcMsgBuffer[guiMsgBufferIndex];
    THINKey_sTabAppParamType *psTabTaskParams = hGetTabAppHandle();
    eResult = THINKey_OSAL_eQueueSend
                      (psTabTaskParams->vTabAppQueue, &sTabAppMessage);
    if(E_THINKEY_SUCCESS == eResult)
    {
        THINKEY_DEBUG_INFO ("print on tab successful\r\n");
    } else {
        THINKEY_DEBUG_INFO ("print on tab unsuccessful\r\n");
     }
    guiMsgBufferIndex ++;
    if(TAB_APP_MSG_BUFFER_COUNT == guiMsgBufferIndex) {
        guiMsgBufferIndex = 0;
    }
    return;
}
