/** \file
   ---------------------------------------------------------------
   Copyright (C) 2021. Panthronics AG - All Rights Reserved.

   This material may not be reproduced, displayed, modified or
   distributed without the express prior written permission of the
   Panthronics AG.

   PLEASE REFER TO OUR DISCLAIMER NOTICE IN THE PROJECT ROOT.
   ---------------------------------------------------------------

    Project     : PTX100R
    Module      : COMMON API
    File        : ptxCOMMON.c

    Description : Common API for common functions.
*/


/*
 * ####################################################################################################################
 * INCLUDES
 * ####################################################################################################################
 */

#include "ptxCOMMON.h"

/*
 * ####################################################################################################################
 * DEFINES / TYPES
 * ####################################################################################################################
 */
/*
 * Comment / Uncomment this #define to enable output via printf
 */
#define ENABLE_PRINTF_OPTION

#ifdef ENABLE_PRINTF_OPTION
    #include "ptxDBG_PORT.h"
    #include <stdarg.h>
    #include <stdio.h>
#endif

/*
 * ####################################################################################################################
 * API FUNCTIONS
 * ####################################################################################################################
 */

void ptxCommon_PrintF(const char *format, ...)
{
#ifdef ENABLE_PRINTF_OPTION
    va_list argptr;
    va_start(argptr, format);

    const size_t max_len = 256u;
    char buffer[max_len];
    buffer[max_len-1] = '\0';

    (void)vsnprintf(buffer, max_len, format, argptr);

    (void)ptxDBGPORT_Write(buffer);

    va_end(argptr);
#else
    (void)format;
#endif
}

void ptxCommon_Print_Buffer(uint8_t *buffer, uint32_t bufferOffset, uint32_t bufferLength, uint8_t addNewLine, uint8_t printASCII)
{
    uint32_t i;
    uint32_t lineIdx = 0;
    uint8_t character_to_print;

    if (NULL != buffer)
    {
        if (0 != bufferLength)
        {
            for (i = 0; (i < bufferLength) && (i < (uint32_t)TX_BUFFER_SIZE); i++)
            {
                if ((i > 0) && ((i % (LINE_LENGTH - 5) == 0)))
                {
                    lineIdx++;
                    ptxCommon_PrintF("\n     ");
                }

                if (0 == printASCII)
                {
                    ptxCommon_PrintF("%02X", (uint8_t)buffer[i + bufferOffset]);
                } else
                {
                    character_to_print = (uint8_t)buffer[i + bufferOffset];
                    /* avoid unintentional interpretation of ascii-commands */
                    if (character_to_print < 0x20)
                    {
                        ptxCommon_PrintF(".");
                    } else
                    {
                        ptxCommon_PrintF("%c", character_to_print);
                    }
                }
            }

            if (0 != addNewLine)
            {
                ptxCommon_PrintF("\n");
            }
        }
    }
}

void ptxCommon_PrintStatusMessage(const char *message, ptxStatus_t st)
{
    if (NULL != message)
    {
        if (ptxStatus_Success == st)
        {
            ptxCommon_PrintF("%s ... OK\n", message);
        } else
        {
            ptxCommon_PrintF("%s ... ERROR (Status-Code = %04X)\n", message, st);
        }
    }
}

