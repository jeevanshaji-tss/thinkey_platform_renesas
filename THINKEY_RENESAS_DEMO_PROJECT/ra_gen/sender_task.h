/* generated thread header file - do not edit */
#ifndef SENDER_TASK_H_
#define SENDER_TASK_H_
#include "bsp_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "hal_data.h"
#ifdef __cplusplus
                extern "C" void sender_task_entry(void * pvParameters);
                #else
extern void sender_task_entry(void *pvParameters);
#endif
FSP_HEADER
FSP_FOOTER
#endif /* SENDER_TASK_H_ */
