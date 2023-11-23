/* generated common source file - do not edit */
#include "common_data.h"
ioport_instance_ctrl_t g_ioport_ctrl;
const ioport_instance_t g_ioport =
{ .p_api = &g_ioport_on_ioport, .p_ctrl = &g_ioport_ctrl, .p_cfg = &g_bsp_pin_cfg, };
QueueHandle_t g_queue;
#if 1
StaticQueue_t g_queue_memory;
uint8_t g_queue_queue_memory[15 * 4];
#endif
void rtos_startup_err_callback(void *p_instance, void *p_data);
SemaphoreHandle_t g_binary_semaphore;
#if 1
StaticSemaphore_t g_binary_semaphore_memory;
#endif
void rtos_startup_err_callback(void *p_instance, void *p_data);
void g_common_init(void)
{
    g_queue =
#if 1
            xQueueCreateStatic (
#else
                xQueueCreate(
                #endif
                                4,
                                15
#if 1
                                ,
                                &g_queue_queue_memory[0], &g_queue_memory
#endif
                                );
    if (NULL == g_queue)
    {
        rtos_startup_err_callback (g_queue, 0);
    }
    g_binary_semaphore =
#if 1
            xSemaphoreCreateBinaryStatic (&g_binary_semaphore_memory);
#else
                xSemaphoreCreateBinary();
                #endif
    if (NULL == g_binary_semaphore)
    {
        rtos_startup_err_callback (g_binary_semaphore, 0);
    }
}
