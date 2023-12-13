#ifndef THINKEY_NAL_H
#define THINKEY_NAL_H

#include "thinkey_types.h"

typedef TKey_VOID (*TKey_pfnNfcKeyFound)(TKey_Handle hKeyHandle,
            TKey_UINT32 uiKeyId, TKey_Handle hNfcHandle);
typedef	TKey_VOID (*TKey_pfnNfcKeyRemoved)(TKey_Handle hKeyHandle,
			TKey_Handle hNfcHandle);
typedef	TKey_VOID (*TKey_pfnNfcDataReceived)(TKey_Handle hKeyHandle,
            TKey_Handle hNfcHandle, TKey_BYTE* pucData, TKey_UINT32 uiLength);

typedef struct {
    TKey_VOID (*TKey_pfnNfcKeyFound)(TKey_Handle,
            TKey_UINT32, TKey_Handle);
    TKey_VOID (*TKey_pfnNfcKeyRemoved)(TKey_Handle,
            TKey_Handle);
    TKey_VOID (*TKey_pfnNfcDataReceived)(TKey_Handle,
            TKey_Handle, TKey_BYTE*, TKey_UINT32);
} sNfcCallbackType;

TKey_Handle tkey_NAL_NfcStackInit(TKey_Handle hNalHandle,
        sNfcCallbackType *psCallbacks, THINKey_eTransportType eTransport);

TKey_StatusType tkey_NAL_NfcStackDeInit(TKey_Handle hNalHandle, THINKey_eTransportType eTransport);

TKey_StatusType	tkey_NAL_StartDiscovery(TKey_Handle hNalHandle);

TKey_StatusType tkey_NAL_StopDiscovery(TKey_Handle hNalHandle);

TKey_StatusType tkey_NAL_SendData(TKey_Handle hNalHandle,
        TKey_Handle hKeyHandle, TKey_BYTE* pucDataBuffer,
        TKey_UINT32 uiLength);
TKey_Status_t tkey_NAL_DeactivateDevice(TKey_Handle_t hNalHandle,
        TKey_Handle_t hKeyHandle);
#endif /* THINKEY_NAL_H */
