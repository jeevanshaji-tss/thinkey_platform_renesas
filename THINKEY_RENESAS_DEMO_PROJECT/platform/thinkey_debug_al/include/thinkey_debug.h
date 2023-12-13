/*
 * \file thinkey_debug.h
 *
 * \brief Header file for debug functions
 *
 * Copyright (C) 2021-2022, ThinkSeed Systems Private Limited.
 * All Rights Reserved.
 */
#ifndef THINKEY_DEBUG_H
#define THINKEY_DEBUG_H

#include "uart_debug.h"
#include "thinkey_platform_types.h"

#define THINKEY_DEBUG_ERROR task_print_error
#define THINKEY_DEBUG_WARNING task_print_warning
#define THINKEY_DEBUG_INFO task_print_info
#define THINKEY_DEBUG_PRINT_STATUS_MESSAGE(...) do{TKey_Debug_Tab_App_Send_Status_Message(__VA_ARGS__);}while(0)
/**
 * \brief Initialisation method for debug prints 
 */ 
THINKey_eStatusType THINKey_DEBUGInit(THINKey_VOID);
void TKey_Debug_Tab_App_Send_Status_Message(char* stringPtr, ...);

#endif /* THINKEY_DEBUG_H */
