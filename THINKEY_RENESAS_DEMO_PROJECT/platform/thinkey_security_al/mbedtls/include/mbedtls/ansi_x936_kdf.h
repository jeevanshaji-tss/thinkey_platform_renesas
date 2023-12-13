

#ifndef MBEDTLS_KDF_H
#define MBEDTLS_KDF_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "mbedtls/md.h"
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


int mbedtls_ansi_x936_kdf( mbedtls_md_type_t md_type, size_t input_len, 
                           uint8_t input[], size_t shared_info_len, uint8_t shared_info[],
                           size_t output_len, uint8_t output[]);

#ifdef __cplusplus
}
#endif

#endif /*KDF_H*/
