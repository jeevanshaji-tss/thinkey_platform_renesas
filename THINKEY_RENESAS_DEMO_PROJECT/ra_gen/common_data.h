/* generated common header file - do not edit */
#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "r_ioport.h"
#include "bsp_pin_cfg.h"
FSP_HEADER
#define IOPORT_CFG_NAME g_bsp_pin_cfg

/* IOPORT Instance */
extern const ioport_instance_t g_ioport;

/* IOPORT control structure. */
extern ioport_instance_ctrl_t g_ioport_ctrl;
extern QueueHandle_t g_queue;
extern SemaphoreHandle_t g_binary_semaphore;
void g_common_init(void);
FSP_FOOTER
#endif /* COMMON_DATA_H_ */
