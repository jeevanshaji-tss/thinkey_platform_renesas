#ifndef THINKEY_PLATFORM_TYPES_H
#define THINKEY_PLATFORM_TYPES_H

/**
 *  @brief Basic Typedefs used in the THINKey Framework
 */

typedef unsigned int TKey_UINT32;
typedef int TKey_INT32;
typedef unsigned short TKey_UINT16;
typedef short TKey_INT16;
typedef unsigned char TKey_BYTE;
typedef char TKey_CHAR;
typedef char* TKey_STRING;
typedef const char* TKey_CONST_STRING;
typedef unsigned char TKey_BOOL;
typedef unsigned long long TKey_UINT64;
typedef void TKey_VOID;
typedef TKey_VOID* TKey_HANDLE;
typedef const char* TKey_DEBUG_TAG;

/**
 *  @brief THINKey Framework defines
 */
#define TKey_TRUE 1
#define TKey_FALSE 0
#define TKey_NULL 0x0
#define TKey_FOREVER 1
#define TKey_EXIT 0

/**
 *  @brief THINKey Framework return Status Type definition
 */
typedef enum
{
    E_TKEY_SUCCESS,
    E_TKEY_FAILURE
} TKey_StatusType_t;

/* TBD: These are to be eventually removed. We have these here to avoid
 * compilation issues. */
typedef unsigned int THINKey_UINT32;
typedef int THINKey_INT32;
typedef unsigned short THINKey_UINT16;
typedef short THINKey_INT16;
typedef unsigned char THINKey_BYTE;
typedef char THINKey_CHAR;
typedef char* THINKey_STRING;
typedef const char* THINKey_CONST_STRING;
typedef unsigned char THINKey_BOOL;
typedef unsigned long long THINKey_UINT64;
typedef void THINKey_VOID;
typedef THINKey_VOID* THINKey_HANDLE;
typedef TKey_VOID* TKey_Handle;
typedef TKey_VOID* TKey_Handle_t;
typedef const char* THINKey_DEBUG_TAG;

#define THINKey_TRUE TKey_TRUE
#define THINKey_FALSE TKey_FALSE
#define THINKey_NULL TKey_NULL
#define THINKey_FOREVER TKey_FOREVER
#define THINKey_EXIT TKey_EXIT

typedef enum
{
    E_THINKEY_SUCCESS,
    E_THINKEY_FAILURE
}THINKey_eStatusType;

typedef TKey_StatusType_t TKey_StatusType;

typedef TKey_StatusType TKey_Status_t;

#endif /* THINKEY_PLATFORM_TYPES_H */
