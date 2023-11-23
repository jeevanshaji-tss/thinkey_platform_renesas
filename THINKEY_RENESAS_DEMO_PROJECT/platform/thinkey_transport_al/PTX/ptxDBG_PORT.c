/** \file
   ---------------------------------------------------------------
   Copyright (C) 2021. Panthronics AG - All Rights Reserved.

   This material may not be reproduced, displayed, modified or
   distributed without the express prior written permission of the
   Panthronics AG.

   PLEASE REFER TO OUR DISCLAIMER NOTICE IN THE PROJECT ROOT.
   ---------------------------------------------------------------

    Project     : PTX100x
    Module      : DEBUG_PORT
    File        : ptxPDBG_PORT.c

    Description : implementation of debug port on uart.
*/

/*
 * ####################################################################################################################
 * INCLUDES
 * ####################################################################################################################
 */
#include "ptxDBG_PORT.h"
#include "ptxPLAT_EXT.h"
#include <string.h>
#include <stdio.h>

#define DBGPORT_STATUS_SUCCESS      0
#define DBGPORT_STATUS_ERROR        1U


/*
 * ####################################################################################################################
 * API FUNCTIONS
 * ####################################################################################################################
 */
uint16_t ptxDBGPORT_Open(void)
{
    uint16_t status = DBGPORT_STATUS_SUCCESS;

    return status;
}


uint16_t ptxDBGPORT_Close(void)
{
    uint16_t status = DBGPORT_STATUS_SUCCESS;

    return status;
}

uint16_t ptxDBGPORT_Write(char *message)
{
    uint16_t status = DBGPORT_STATUS_SUCCESS;

    if(NULL != message)
    {
        printf("%s", message);
    }

    return status;
}

uint16_t ptxDBGPORT_Write_Buffer(uint8_t *buffer, uint32_t bufferLen)
{
    uint16_t status = DBGPORT_STATUS_SUCCESS;

    if((NULL != buffer) && (0 != bufferLen))
    {
    	for(int i=0; i < bufferLen; i++)
    	{
    		printf("%.2x", buffer[i]);
    	}
    }

    return status;
}

void ptxDBGPORT_Reset_FIFO_Level(void)
{
	(void)0;
}

uint32_t ptxDBGPORT_Get_Available_Bytes(void)
{
    uint32_t available_bytes = 0;

    return available_bytes;
}

uint16_t ptxDBGPORT_Read_Buffer_(uint8_t *buffer, uint32_t bufferOffset)
{
    uint16_t status = DBGPORT_STATUS_ERROR;

    (void)buffer;
    (void)bufferOffset;
    return status;
}

#ifdef  DBGPORT_HOSTRQ_HANDLER_REGISTERED
uint16_t ptxDBGPORT_RegisterCB(void *cb)
{
    uint16_t status = DBGPORT_STATUS_ERROR;

    if(NULL != cb)
    {
        dbgport_ctx.RqHandler = cb;
        status = DBGPORT_STATUS_SUCCESS;
    }

    return status;
}
#endif
