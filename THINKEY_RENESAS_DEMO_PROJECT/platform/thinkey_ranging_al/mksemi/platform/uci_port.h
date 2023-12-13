
/**
 * @defgroup uci_platfrom Platform
 * @brief   Platform-related interfaces that users need to provide to ensure the correct operation of UCI Host
 * @{
 * @ingroup uci_host
 * @{
 * @file    uci_port.h
 * @brief   UCI Host relies on helper functions or interfaces provided by specific platforms
 * @author  Mauna Kea Semiconductor
 * @copyright   Copyright (c) 2019-2022 Mauna Kea Semiconductor Holdings.
 *              All rights reserved.
 * @date    2022-7-14
 */

#ifndef _UCI_PORT_H_
#define _UCI_PORT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#include "user_config.h"


/****************************************************************************//**
 *
 *                                 UCI RX BUFFER TYPEDEF
 *
 *******************************************************************************/

typedef struct {
    uint8_t		buf[256];
    bool        occupied;
} uci_rx_buf_t;

/****************************************************************************//**
 *
 *                                 MACRO function
 *
 *******************************************************************************/

#define gpio_read(x)				cyhal_gpio_read(x)
#define gpio_write(x,y)			R_IOPORT_PinWrite(&g_ioport_ctrl, x, y);//cyhal_gpio_write(x,y)


#define	HAL_Delay(x)				delay_ms(x)

/****************************************************************************//**
 *
 *                                 function prototypes
 *
 *******************************************************************************/
void handle_error(void);
uint32_t hal_get_tick(void);
void hal_init(void);
void uwb_reset(void);
void delay_ms(uint32_t ms);
void delay_us(uint32_t us);
void uci_reset_rx_buf(void);
int uci_get_free_rx_buf(void);

/**
 * @brief   UCI Host requests to get scheduled
 * @note    The timing of calling this API  \n
 *          - when user called UCI API and there are UCI Message frames pending to be send \n
 *          - when user requested to destroy any UWBS intance via @ref uci_destroy_uwbs \n
 *          - data from hardware driver pending to be processed    \n
 * @note    User should call @ref uci_host_loop as soon as possible
 * @return  void
 */
void uci_sched_me(void);

/**
 * @brief   Initialize bus that used to send and receive UCI Message frames
 * @param[in]   bus_id bus id
 * @note    UCI Host only calls this API when bus instance in created
 * @return  void
 */
void uci_intf_open(uint8_t bus_id);

/**
 * @brief   Send one UCI Message frame to specific UWBS instance over specific bus
 * @param[in]   bus_id bus id
 * @param[in]   uwbs_id uwbs id
 * @param[in]   buf data pending to be send
 * @param[in]   len number of bytes to be send
 * @note    The UCI host will keep the content pointed to by @p buf valid until a response is obtained from UWBS or a timeout occurs
 * @return  send result
 *  @retval 0 success
 *  @retval !0 failure
 */
int32_t uci_send(uint8_t bus_id, uint8_t uwbs_id, const uint8_t *buf, uint16_t len);

/**
 * @brief   Recovery or restore platform internal flags/variables when time-out happens
 * @param[in]   bus_id bus id
 * @param[in]   uwbs_id uwbs id
 * @param[in]   buf data failed to be send
 * @param[in]   len number of bytes the @p buf points to
 * @note    This API is called when the UCI Host cannot get response in time from a specific WBS instance   \n
 *          It means that giving the platform a chance to do things like clean-up when time-out happens
 * @return  void
*/
void uci_recov_from_timeout(uint8_t bus_id, uint8_t uwbs_id, uint8_t *buf, uint16_t len);

/**
 * @brief   Release the buffer that has been processed
 * @param[in]   bus_id bus id
 * @param[in]   uwbs_id uwbs id
 * @param[in]   ptr buffer pending to be released
 * @note    Until this function gets called, the buffer content provided to @ref uci_process_recv_pkt cannot be corrupted
 * @return  void
 */
void uci_buf_release(uint8_t bus_id, uint8_t uwbs_id, uint8_t *ptr);

/**
 * @brief   Malloc memory
 * @param[in]   len number of bytes to be malloc
 * @return  malloc result
 *  @retval NULL failure
 *  @retval !NULL success
 */
void *uci_malloc(uint16_t len);

/**
 * @brief   Free memory
 * @param[in]   ptr memory pending to be free
 * @return  void
 */
void uci_free(void* ptr);

/**
 * @brief   Enter critical section
 * @return  value that releated to specified platform
 */
uint32_t uci_enter_critical(void);

/**
 * @brief   Exit from critical section
 * @param[in]   key value returned from @ref uci_enter_critical
 * @return  void
 */
void uci_exit_critical(uint32_t key);

/**
 * @brief   Get current time in ms
 * @return  current time
 */
uint32_t uci_get_tick(void);

/**
 * @brief   Submit data to be processed to UCI Host
 * @param[in]   bus_id bus id
 * @param[in]   uwbs_id uwbs id
 * @param[in]   buf data pending to be processed
 * @param[in]   len number of bytes to be processed
 * @note    User should call this API while hardware bus received data from UBWS    \n
 *          The content pointed to by @p buf should remain unchanged until @ref uci_buf_release gets called,    \n
 *          since UCI Host will NOT do malloc to save data from hardware bus    \n
 *          Parameters @p bus_id and @p uwbs_id must be the same as when the UWBS instance was created through @ref uci_create_uwbs \n
 * @note    You don't need to implement this API but call it when needed
 * @return  submit result
 *  @retval
 *          -
 *          - @ref UEC_UWBS_NOT_FOUND UWBS instance not found
 *          - @ref UEC_BUF_RECS_OVERFLOW The buf_recs assigned for this UWBS is too small
 *          - @ref UEC_SUCCESS success
 */
extern int32_t uci_process_recv_pkt(uint8_t bus_id, uint8_t uwbs_id, uint8_t *buf, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif // _UCI_PORT_H_

/** @} */
/** @} */
