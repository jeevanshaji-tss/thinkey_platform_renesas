
#include "mbedtls/platform.h"

#if defined(MBEDTLS_KDF_C)

#include <string.h>
#include "mbedtls/ansi_x936_kdf.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#include "mbedtls/md.h"
#include "mbedtls/md_internal.h"
#include <math.h>


int mbedtls_ansi_x936_kdf( mbedtls_md_type_t md_type, size_t input_len, uint8_t input[], size_t shared_info_len, uint8_t shared_info[], size_t output_len, uint8_t output[])
{
    mbedtls_md_context_t md_ctx;
    const mbedtls_md_info_t *md_info = NULL;
    int i = 0;
    int hashlen = 0;
    int exit_code = MBEDTLS_EXIT_FAILURE;
    uint32_t counter = 1;
    uint8_t counter_buf[4];


    mbedtls_md_init( &md_ctx );

    md_info = mbedtls_md_info_from_type(md_type);
    
    if( md_info == NULL )
    {
        mbedtls_fprintf( stderr, "Message Digest type not found\n");
        return( exit_code );
    }

    if( mbedtls_md_setup( &md_ctx, md_info, 0 ) )
    {
        mbedtls_fprintf( stderr, "Failed to initialize context.\n" );
        return( exit_code );
    }

    // 
    // TODO MAX HASH LENGTH FROM MBEDTLS replace pow sttement 
    if (input_len + shared_info_len + 4 >= (pow(2,61))-1){
        mbedtls_fprintf( stderr, "Max hash length exceeded \n");
        return( exit_code );
    }

    // keydatalen equals output_len
    hashlen = md_info->size;
    uint8_t tmp_output[hashlen];
    if (output_len >=  hashlen * (pow(2,32)-1)){
        mbedtls_fprintf( stderr, "Max hash length exceeded \n");
        return( exit_code );
    }

    while(i < output_len){
        mbedtls_md_starts(&md_ctx);
        mbedtls_md_update(&md_ctx, input, input_len);
        
        //TODO: be careful with architecture little vs. big
        counter_buf[0] = (uint8_t) ((counter >> 24) & 0xff);
        counter_buf[1] = (uint8_t) ((counter >> 16) & 0xff);
        counter_buf[2] = (uint8_t) ((counter >> 8) & 0xff);
        counter_buf[3] = (uint8_t) ((counter >> 0) & 0xff);

        mbedtls_md_update(&md_ctx, counter_buf, 4);


        if (shared_info_len > 0 && shared_info != NULL){
            mbedtls_md_update(&md_ctx, shared_info, shared_info_len);
        }
        mbedtls_md_finish(&md_ctx, tmp_output); 
        memcpy(&output[i], tmp_output, (output_len - i < hashlen) ? output_len - i : hashlen);
        i += hashlen;
        counter++;
    }
    mbedtls_md_free( &md_ctx );
    return( exit_code );
}
#endif /*MBEDTLS_KDF_C */


