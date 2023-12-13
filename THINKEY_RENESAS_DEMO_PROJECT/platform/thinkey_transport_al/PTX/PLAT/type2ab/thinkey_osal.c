

#include "thinkey_osal.h"
#include "thinkey_debug.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "thinkey_platform_types.h"

static THINKey_DEBUG_TAG TAG = "OSAL";
#define PERIODIC_TIMER_NAME "Periodic Timer"
#define ONE_SHOT_TIMER_NAME "One Shot Timer"

/* OSAL Implementations */
THINKey_eStatusType
THINKey_OSAL_eCreateTask
(THINKey_CONST_STRING strTaskName, THINKey_pfnTaskFunction pfnTaskFunction,
THINKey_VOID* pvTaskParams, THINKey_UINT32 uiTaskPriority,
THINKey_UINT32 uiStackSize,THINKey_UINT32* puiTaskID)
{
	THINKey_eStatusType eStatus = E_THINKEY_FAILURE;
	BaseType_t uiResult;

	uiResult = xTaskCreate(pfnTaskFunction,
	                      strTaskName,
						  uiStackSize,
						  pvTaskParams,
	                      uiTaskPriority, (TaskHandle_t *)puiTaskID);

	if(uiResult == pdPASS)
		eStatus = E_THINKEY_SUCCESS;

	return eStatus;
}

THINKey_HANDLE THINKey_OSAL_hCreateQueue
(THINKey_UINT32 uiNumQElements, THINKey_UINT32 uiQElementSize)
{
	THINKey_HANDLE hQHandle = THINKey_NULL;

	hQHandle = (THINKey_HANDLE)	xQueueCreate((size_t)uiNumQElements,
			(size_t) uiQElementSize);

	return hQHandle;
}


THINKey_eStatusType	THINKey_OSAL_eQueueReceive
(THINKey_HANDLE hQHandle,THINKey_VOID* pvMessage)
{
	THINKey_eStatusType eStatus = E_THINKEY_FAILURE;

	BaseType_t uiResult;

	/* Wait forever if no items in the queue.*/
	uiResult = xQueueReceive((QueueHandle_t)hQHandle,
			(void*) pvMessage, portMAX_DELAY);

	if(uiResult == pdTRUE)
			eStatus = E_THINKEY_SUCCESS;

	return eStatus;
}

THINKey_eStatusType	THINKey_OSAL_eTimedQueueReceive
(THINKey_HANDLE hQHandle,THINKey_VOID* pvMessage, TKey_UINT32 uiTimeout)
{
	THINKey_eStatusType eStatus = E_THINKEY_FAILURE;

	BaseType_t uiResult;
	uiTimeout = uiTimeout / portTICK_PERIOD_MS;

	/* Wait forever if no items in the queue.*/
	uiResult = xQueueReceive((QueueHandle_t)hQHandle,
			(void*) pvMessage, uiTimeout);

	if(uiResult == pdTRUE)
			eStatus = E_THINKEY_SUCCESS;

	return eStatus;
}

THINKey_eStatusType THINKey_OSAL_eQueueSend
(THINKey_HANDLE hQHandle, THINKey_VOID* pvMessage)
{
	THINKey_eStatusType eStatus = E_THINKEY_FAILURE;

	BaseType_t uiResult;

	/* Do not wait if no space is available in the queue.*/
	uiResult = xQueueSend(hQHandle, (const void*) pvMessage,
			THINKEY_OSAL_ZERO);

	if(uiResult == pdTRUE)
		eStatus = E_THINKEY_SUCCESS;

	return eStatus;
}

/* Abstraction Function to Send to queue from ISR */
THINKey_eStatusType THINKey_OSAL_eQueueSendToFromISR(
        THINKey_HANDLE hQHandle, THINKey_VOID* pvMessage,
        THINKey_HANDLE hHigherPriorityTaskWoken)
{
	THINKey_eStatusType eStatus = E_THINKEY_SUCCESS;

        xQueueSendToFrontFromISR(hQHandle, pvMessage,
	            (BaseType_t*)hHigherPriorityTaskWoken);
        return eStatus;
}

THINKey_eStatusType THINKey_OSAL_eMemCpy
(THINKey_BYTE* pbTarget, const THINKey_BYTE* pbSource, THINKey_UINT32 uiSize)
{
	TKey_UINT32 uiIndex;
	/* This can be later replaced by MemCpy utility */

	if((pbTarget != NULL) && (pbSource != NULL))
	{
		for(uiIndex = 0; uiIndex < uiSize; uiIndex++)
		{
			pbTarget[uiIndex]= pbSource[uiIndex];
		}
	}

	return E_THINKEY_SUCCESS;
}

THINKey_BOOL THINKey_OSAL_eMemCmp
(const THINKey_BYTE* pbMem1, const THINKey_BYTE* pbMem2, THINKey_UINT32 uiSize)
{
	THINKey_BOOL boReturn = THINKey_TRUE;
	TKey_UINT32 uiIndex;

	if((pbMem1 != NULL) && (pbMem2 != NULL))
	{
		/* This can be later replaced by MemCmp utility */
		for(uiIndex = 0; uiIndex < uiSize; uiIndex++)
		{
			if(	pbMem1[uiIndex] != pbMem2[uiIndex] )
			{
				boReturn = THINKey_FALSE;
				break;
			}
		}
	}

	return boReturn;
}

THINKey_VOID THINKey_OSAL_vOSStart(THINKey_VOID)
{
	vTaskStartScheduler();
	return;
}

THINKey_HANDLE THINKey_OSAL_hCreatePeriodicTimer
(THINKey_pfnTimerCallback pfnTimerCallback, THINKey_HANDLE hCallerHandle)
{
	THINKey_HANDLE hTimerHandle;
	THINKey_VOID   *pvTimerId = NULL;
	(void)hCallerHandle;

	hTimerHandle = (THINKey_HANDLE) xTimerCreate(PERIODIC_TIMER_NAME,
			pdMS_TO_TICKS(500), pdTRUE, pvTimerId,
			(TimerCallbackFunction_t)pfnTimerCallback);

	return ((THINKey_HANDLE)hTimerHandle);
}

THINKey_HANDLE THINKey_OSAL_hCreateOneShotTimer
(THINKey_pfnTimerCallback pfnTimerCallback, THINKey_HANDLE hCallerHandle)
{
	THINKey_HANDLE hTimerHandle;
	THINKey_VOID   *pvTimerId = NULL;
		(void)hCallerHandle;

		hTimerHandle = (THINKey_HANDLE) xTimerCreate(PERIODIC_TIMER_NAME,
				pdMS_TO_TICKS(500), pdFALSE, pvTimerId,
				(TimerCallbackFunction_t)pfnTimerCallback);

	return ((THINKey_HANDLE)hTimerHandle);
}

THINKey_eStatusType THINKey_OSAL_eStartTimer
(THINKey_HANDLE hTimerHandle, THINKey_UINT32 uiTimeInMilliSeconds)
{
	THINKey_eStatusType eStatus = E_THINKEY_SUCCESS;
	BaseType_t uiResult;

	uiResult = xTimerChangePeriod(hTimerHandle, pdMS_TO_TICKS(uiTimeInMilliSeconds), pdMS_TO_TICKS(0));

	if(uiResult == pdPASS)
		eStatus = E_THINKEY_SUCCESS;

	return eStatus;
}

THINKey_eStatusType THINKey_OSAL_eStopTimer (THINKey_HANDLE hTimerHandle)
{
	THINKey_eStatusType eStatus = E_THINKEY_SUCCESS;
	BaseType_t uiResult;

	uiResult = xTimerStop(hTimerHandle, pdMS_TO_TICKS(0));

	if(uiResult == pdPASS)
		eStatus = E_THINKEY_SUCCESS;

	return eStatus;
}

THINKey_eStatusType THINKey_OSAL_eDestroyTimer (THINKey_HANDLE hTimerHandle)
{
	THINKey_eStatusType eStatus = E_THINKEY_SUCCESS;
	BaseType_t uiResult;

	uiResult = xTimerDelete(hTimerHandle, pdMS_TO_TICKS(0));

	if(uiResult == pdPASS)
		eStatus = E_THINKEY_SUCCESS;

	return eStatus;
}

TKey_VOID THINKey_OSAL_Delay(TKey_UINT32 uiDelayMs)
{
    const TickType_t xDelay = uiDelayMs / portTICK_PERIOD_MS;
	vTaskDelay(xDelay);
}

__attribute__((weak)) void  vApplicationStackOverflowHook(void) {
/* Print something so that we know */
	THINKEY_DEBUG_ERROR("\r\n %s Stack Overflow!!!", TAG);
}

__attribute__((weak)) void vApplicationMallocFailedHook(void) {
	THINKEY_DEBUG_ERROR("\r\n %s Malloc Failed!!", TAG);

}
