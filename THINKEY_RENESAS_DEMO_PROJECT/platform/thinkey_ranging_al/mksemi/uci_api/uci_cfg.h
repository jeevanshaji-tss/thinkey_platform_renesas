
/**
 * @file    uci_cfg.h
 * @brief   Configurations of UCI Host Library
 * @author  Mauna Kea Semiconductor
 * @copyright   Copyright (c) 2019-2022 Mauna Kea Semiconductor Holdings.
 *              All rights reserved.
 * @date    2022-7-14
 * @ingroup uci_api
 * @{
 */

#ifndef _UCI_CFG_H_
#define _UCI_CFG_H_

#define UCI_RESP_TIMEOUT_MS     50      /*!< Default response timeout */
#define UCI_MAX_CONTROLEES      7       /*!< Maximum number of controlees, affects @ref uci_app_cfg_t */

#define UCI_MAX_STACK_BUF       256     /*!< The buffer size used internally by the APIs, in bytes */
#define UCI_FLUSH_IF_TIMEOUT    false   /*!< How the command caches are handled after response timeout occurs */

#endif // _UCI_CFG_H_

/** @} */
