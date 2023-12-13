/*
 * File: thinkey_bspal.c
 * 
 * Brief: This file contains the BSP abstraction functions.
 *
 * Copyright (C) 2021-2022, ThinkSeed Systems Private Limited.
 * All Rights Reserved.
 */

#include "thinkey_bspal.h"

//#include <boards.h>
//#include <sdk_config.h>
#include <stdio.h>
#include <stdlib.h>
//#include "nrf_drv_gpiote.h"

/* This function initializes platform BSP. */
THINKey_eStatusType THINKey_eBSPInit(void)
{
    THINKey_eStatusType eStatus = E_THINKEY_FAILURE;

    return eStatus;
}

/* This function sets ble interrupt. */
THINKey_VOID THINKey_BSPAL_eSetBleInterrupt(THINKey_HANDLE hBleIrqConfig)
{
    (void)hBleIrqConfig;

}

/* This is to use Freertos malloc, calloc, free instead of std implementation.
 * --wrap=<std_function_name> has to be given in linker option for this to take affect */
//void *__wrap_calloc(size_t num, size_t size)
//{
//    void *ptr = pvPortMalloc(num * size);
//    memset(ptr, 0x00, num * size);
//    return ptr;
//}
//
//void *__wrap_malloc(size_t size)
//{
//    void *ptr = pvPortMalloc(size);
//    return ptr;
//}
//
//void __wrap_free(void *ptr)
//{
//    vPortFree(ptr);
//    return;
//}

