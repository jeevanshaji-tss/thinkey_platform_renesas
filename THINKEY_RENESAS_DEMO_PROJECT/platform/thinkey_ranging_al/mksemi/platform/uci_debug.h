
/**
 * @file    uci_debug.h
 * @brief   For UCI Host debugging, dont worry about it
 * @author  Mauna Kea Semiconductor
 * @copyright   Copyright (c) 2019-2022 Mauna Kea Semiconductor Holdings.
 *              All rights reserved.
 * @date    2022-7-14
 * @ingroup uci_platfrom
 * @{
 */

#ifndef _UCI_DEBUG_H_
#define _UCI_DEBUG_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

//#define DEBUG_UCI

#ifdef DEBUG_UCI
#define UCI_TRACE0(msg)                     printf("%s "msg"\r\n", __func__)
#define UCI_TRACE1(msg, v1)                 printf("%s "msg"\r\n", __func__, v1)
#define UCI_TRACE2(msg, v1, v2)             printf("%s "msg"\r\n", __func__, v1, v2)
#define UCI_TRACE3(msg, v1, v2, v3)         printf("%s "msg"\r\n", __func__, v1, v2, v3)
#define UCI_TRACE4(msg, v1, v2, v3, v4)     printf("%s "msg"\r\n", __func__, v1, v2, v3, v4)
#define UCI_TRACE5(msg, v1, v2, v3, v4, v5) printf("%s "msg"\r\n", __func__, v1, v2, v3, v4, v5)
#else
#define UCI_TRACE0(msg)
#define UCI_TRACE1(msg, v1)
#define UCI_TRACE2(msg, v1, v2)
#define UCI_TRACE3(msg, v1, v2, v3)
#define UCI_TRACE4(msg, v1, v2, v3, v4)
#define UCI_TRACE5(msg, v1, v2, v3, v4, v5)
#endif

#ifdef __cplusplus
}
#endif

#endif // _UCI_DEBUG_H_

/** @} */
