/* generated thread header file - do not edit */
#ifndef SEMAPHORE_TASK_H_
#define SEMAPHORE_TASK_H_
#include "bsp_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "hal_data.h"
#ifdef __cplusplus
                extern "C" void semaphore_task_entry(void * pvParameters);
                #else
extern void semaphore_task_entry(void *pvParameters);
#endif
FSP_HEADER
FSP_FOOTER
#endif /* SEMAPHORE_TASK_H_ */
