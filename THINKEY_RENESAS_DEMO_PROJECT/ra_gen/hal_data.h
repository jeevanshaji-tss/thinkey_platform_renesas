/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_sci_uart.h"
#include "r_uart_api.h"
#include "r_flash_hp.h"
#include "r_flash_api.h"
#include "r_spi.h"
#include "r_gpt.h"
#include "r_timer_api.h"
FSP_HEADER
/** UART on SCI Instance. */
extern const uart_instance_t g_uart1;

/** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
extern sci_uart_instance_ctrl_t g_uart1_ctrl;
extern const uart_cfg_t g_uart1_cfg;
extern const sci_uart_extended_cfg_t g_uart1_cfg_extend;

#ifndef uwb_uart_callback
void uwb_uart_callback(uart_callback_args_t *p_args);
#endif
/* Flash on Flash HP Instance */
extern const flash_instance_t g_flash0;

/** Access the Flash HP instance using these structures when calling API functions directly (::p_api is not used). */
extern flash_hp_instance_ctrl_t g_flash0_ctrl;
extern const flash_cfg_t g_flash0_cfg;

#ifndef bgo_callback
void bgo_callback(flash_callback_args_t *p_args);
#endif
/** SPI on SPI Instance. */
extern const spi_instance_t g_spi0;

/** Access the SPI instance using these structures when calling API functions directly (::p_api is not used). */
extern spi_instance_ctrl_t g_spi0_ctrl;
extern const spi_cfg_t g_spi0_cfg;

/** Callback used by SPI Instance. */
#ifndef spi_callback
void spi_callback(spi_callback_args_t *p_args);
#endif

#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
#define g_spi0_P_TRANSFER_TX (NULL)
#else
    #define g_spi0_P_TRANSFER_TX (&RA_NOT_DEFINED)
#endif
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
#define g_spi0_P_TRANSFER_RX (NULL)
#else
    #define g_spi0_P_TRANSFER_RX (&RA_NOT_DEFINED)
#endif
#undef RA_NOT_DEFINED
/** Timer on GPT Instance. */
extern const timer_instance_t g_periodic_timer_sem;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t g_periodic_timer_sem_ctrl;
extern const timer_cfg_t g_periodic_timer_sem_cfg;

#ifndef periodic_timer_semphr_cb
void periodic_timer_semphr_cb(timer_callback_args_t *p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t g_periodic_timer_msgq;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t g_periodic_timer_msgq_ctrl;
extern const timer_cfg_t g_periodic_timer_msgq_cfg;

#ifndef periodic_timer_msgq_cb
void periodic_timer_msgq_cb(timer_callback_args_t *p_args);
#endif
void hal_entry(void);
void g_hal_init(void);
FSP_FOOTER
#endif /* HAL_DATA_H_ */
