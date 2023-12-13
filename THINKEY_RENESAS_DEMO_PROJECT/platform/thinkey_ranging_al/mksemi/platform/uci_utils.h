
/**
 * @file    uci_utils.h
 * @brief   Byte-Order conversion between UCI and platform  \n
 *          Please note that UCI encode integer in little-endian format
 * @author  Mauna Kea Semiconductor
 * @copyright   Copyright (c) 2019-2022 Mauna Kea Semiconductor Holdings.
 *              All rights reserved.
 * @date    2022-7-14
 * @ingroup uci_platfrom
 * @{
 */

#ifndef _UCI_UTILS_H_
#define _UCI_UTILS_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Decoding a uint16 value
 * @param[in]   ptr buffer where the encoded data is stored
 * @return  Decoded value of type uint16_t
 */
#define uint16_decode(ptr)      ((uint16_t)(*(ptr) | (*((ptr) + 1) << 8)))

/**
 * @brief   Decoding a uint32 value
 * @param[in]   ptr buffer where the encoded data is stored
 * @return  Decoded value of type uint32_t
 */
#define uint32_decode(ptr)      ((uint32_t)(*(ptr) | (*((ptr) + 1) << 8) | (*((ptr) + 2) << 16) | (*((ptr) + 3) << 24)))

/**
 * @brief   Encoding a uint16 value in little-endian format
 * @param[in]   v value to be encoded
 * @param[out]  p buffer where the encoded data will be written
 * @return  void
 */
#define uint16_encode_le(v, p)  ((p)[0] = (v) & 0xFF, (p)[1] = ((v) >> 8) & 0xFF)

/**
 * @brief   Encoding a uint32 value in little-endian format
 * @param[in]   v value to be encoded
 * @param[out]  p buffer where the encoded data will be written
 * @return  void
 */
#define uint32_encode_le(v, p)  ((p)[0] = (v) & 0xFF, (p)[1] = ((v) >> 8) & 0xFF, (p)[2] = ((v) >> 16) & 0xFF, (p)[3] = ((v) >> 24) & 0xFF)

/**
 * @brief   Get the first addres for a given type struct
 * @param[in]   ptr address of the member of this struct
 * @param[in]   type type of struct
 * @param[in]   member name of the member of this struct
 * @return  first addres of this struct
 */
#ifndef container_of
#define container_of(ptr, type, member) ((type *)(void *)((char *)(ptr) - offsetof(type, member)))
#endif

#ifdef __cplusplus
}
#endif

#endif // _UCI_UTILS_H_

/** @} */
