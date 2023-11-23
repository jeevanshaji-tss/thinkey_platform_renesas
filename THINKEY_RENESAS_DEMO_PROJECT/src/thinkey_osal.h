/*
 * thinkey_osal.h
 *
 *  Created on: Sep 21, 2021
 *      Author: sanjeev
 */

#ifndef THINKEY_OSAL_H
#define THINKEY_OSAL_H

#include "thinkey_platform_types.h"
#include "FreeRTOSConfig.h"

#define THINKEY_OSAL_ZERO 0
#define THINKEY_OSAL_FOREVER 0xFFFFFFFF
#define THINKEY_OSAL_GET_MAX_TASK_PRIORITY configMAX_PRIORITIES

typedef THINKey_VOID (*THINKey_pfnTaskFunction)(THINKey_VOID*);
typedef THINKey_VOID (*THINKey_pfnTimerCallback)(THINKey_HANDLE);


/* Parameters:
 * Task Name
 * Task Function
 * Task Function parameters
 * Stack Depth in Words(not bytes)
 * Task Priority
 * Task Identifier
 */
THINKey_eStatusType
THINKey_OSAL_eCreateTask
(THINKey_CONST_STRING strTaskName,
THINKey_pfnTaskFunction,
THINKey_VOID* pvTaskParams,
THINKey_UINT32 uiTaskPriority,
THINKey_UINT32 uiTaskStackSize,
THINKey_UINT32* puiTaskID);


/* Parameters:
 * Number of Entries in the Queue
 * Size of each entry in the Queue
 */
THINKey_HANDLE
THINKey_OSAL_hCreateQueue
(THINKey_UINT32 uiNumQElements,
THINKey_UINT32 uiQElementSize);

THINKey_eStatusType
THINKey_OSAL_eQueueReceive
(THINKey_HANDLE,
THINKey_VOID* pvMessage);

THINKey_eStatusType	THINKey_OSAL_eTimedQueueReceive
(THINKey_HANDLE hQHandle,THINKey_VOID* pvMessage, TKey_UINT32 uiTimeout);

THINKey_eStatusType
THINKey_OSAL_eQueueSend
(THINKey_HANDLE,
THINKey_VOID* pvMessage);

THINKey_eStatusType
THINKey_OSAL_eQueueSendToFromISR
(THINKey_HANDLE hQHandle, THINKey_VOID* pvMessage,
THINKey_HANDLE hHigherPriorityTaskWoken);

THINKey_VOID
THINKey_OSAL_vOSStart
(THINKey_VOID);

THINKey_eStatusType THINKey_OSAL_eMemCpy
(THINKey_BYTE* pbTarget, const THINKey_BYTE* pbSource,
		THINKey_UINT32 uiSize);

THINKey_BOOL THINKey_OSAL_eMemCmp
(const THINKey_BYTE* pbMem1, const THINKey_BYTE* pbMem2,
		THINKey_UINT32 uiSize);

THINKey_HANDLE THINKey_OSAL_hCreatePeriodicTimer
(THINKey_pfnTimerCallback pfnTimerCallback,
		THINKey_HANDLE hCallerHandle);

THINKey_HANDLE THINKey_OSAL_hCreateOneShotTimer
(THINKey_pfnTimerCallback pfnTimerCallback,
		THINKey_HANDLE hCallerHandle);

THINKey_eStatusType THINKey_OSAL_eStartTimer
(THINKey_HANDLE hTimerHandle,
		THINKey_UINT32 uiTimeInMilliSeconds);

THINKey_eStatusType THINKey_OSAL_eStopTimer
(THINKey_HANDLE hTimerHandle);

THINKey_eStatusType THINKey_OSAL_eDestroyTimer
(THINKey_HANDLE hTimerHandle);

TKey_VOID THINKey_OSAL_Delay(TKey_UINT32 uiDelayMs);


#endif /* SOURCE_OS_BSP_THINKEY_OSAL_H_ */
